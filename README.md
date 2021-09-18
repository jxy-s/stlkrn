# C++ STL in Windows Drivers 
This project uses MSVC C++ STL in a Windows Kernel Driver. In this solution 
`jxystl.lib` is implemented as a kernel-tuned, pool type/tag aware, template 
library and MSVC implementation. Which, under the hood, uses the MSVC C++ STL.

```cpp
#include <wdm.h>
#include <jxy/string.hpp>

extern "C"
NTSTATUS DriverEntry(
    PDRIVER_OBJECT DriverObject,
    PUNICODE_STRING RegistryPath)
{
    jxy::wstring<PagedPool, '0GAT'> helloWorld;

    try
    {
        helloWorld.assign(L"Hello, World!");
    }
    catch (const std::bad_alloc&)
    {
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    return STATUS_SUCCESS;
}
```

```
1: kd> dv
   DriverObject = 0xffffca83`5380d300 Driver "\Driver\stlkrn"
   RegistryPath = 0xffffca83`5227f000 "\REGISTRY\MACHINE\SYSTEM\ControlSet001\Services\stlkrn"
     helloWorld = "Hello, World!"
```

## Supported Standards

| Standard | Supported | Notes |
| :------: | :-------: | ----- |
| cpp11    | No        | untested, your milage my vary |
| cpp14    | **Yes**   |       |
| cpp17    | **Yes**   |       |
| cpp20    | **Yes**   |       |

The test driver in this solution, `stdtest.sys`, houses the unit tests for the 
project. Unit tests are run in the kernel with driver verifier. The unit test 
framework is bare bones but is sufficient for exercising `jxystl.lib`.

Another driver implemented in this solution, `stdkrn.sys`, puts `jxystl.lib` 
to use in a practical scenario. It uses various `std` namespace facilities and 
containers (wrapped under the `jxy` namespace). This driver registers for 
process, thread, and image notifications; then uses modern C++ to track 
process contexts, thread contexts, and module contexts.

## Exception Handling - `vcrtl`
Exception handling enables C++ objects to unwind when an exception is thrown.
This is a core feature of C++ which gets little attention for kernel drivers. 
Microsoft does not natively support C++ exceptions for kernel drivers.

C++ exception handling is made possible by [avakar's vcrtl libraray][github.vcrtl].
This project would have been far more work without avakar's awesome contribution. 
For information on exception handling in Windows Drivers head over to 
[avakar's vcrtl github][github.vcrtl]. Also, [this page][github.vcrtl.x64] 
gives excellent details on exception handling on AMD64.

## MSVC C++ STL Support - `jxystl`
Windows Kernel allocations are associated with a memory pool. Further, pool 
tagging is built into the Windows Kernel. Pool tagging facilitates tracking of 
allocations made by drivers. This tagging facility enables debugging and 
monitoring of allocations.

The `jxy` namespace, in this solution, empowers development of Windows drivers
using the `std` namespace objects with pool typing and tagging.

The library opts not to implement "global" `new`/`delete` operators. It 
implements only `new`/`delete` operators with pool typing and tagging 
capability. This requires specifying pool types and tags. If some functionality 
is used that would require a "global allocator" it will not link. This is an 
intentional design decision such that no global allocators are used, all 
allocations must specify a pool type and tag.

The `jxy` namespace implements allocators and deleters which conform to the 
standard for use in template containers. These allocators and deleters are 
pool type/tag aware. They require specifying the pool type and tag and prevent 
conversions/rebinding across tool types and tags - they should be used in place 
of the STL allocators.

```cpp
jxy::allocator<T, PagedPool, '0GAT'>;
jxy::default_delete<T, PagedPool, '0GAT'>;
```

`jxystl.lib` implements necessary "fill" functionality for use of MSVC STL 
containers. The implementations (in `msvcfill.cpp`) are considerate to the 
kernel. This functionality enables the MSVC STL containers to link to 
kernel-appropriate functionality. This also means that if some `std` container 
functionality is used that doesn't have "fill" functionality behind  it - the 
linker will fail. This is an intentional design decision such that any 
implementations are thought through for use in the kernel.

CRT initialization and atexit functionality is intentionally not supported. 
Order of CRT initialization is unclear and non-obvious. When a kernel driver 
loads global data should be clearly setup and torn down during driver load and 
unload. Global CRT initialization "hides" this initialization in a non-obvious 
way. Further, CRT atexit functionality is not supported. Emission of necessary 
synchronization enabling local static initialization of C++ objects is not done 
by the compiler.  And would introduces non-obvious synchronization in the 
kernel. Lack of CRT initialization and atexit support is an intentional design 
decision. I strongly recommend avoiding it when developing kernel drivers.

As an example, the `jxy` namespace "wraps" `std::vector` and forces use of 
pool types and tags:

```cpp
namespace jxy
{

template <typename T, 
          POOL_TYPE t_PoolType, 
          ULONG t_PoolTag, 
          typename TAllocator = jxy::allocator<T, t_PoolType, t_PoolTag>> 
using vector = std::vector<T, TAllocator>;

}

jxy::vector<int, PagedPool, '0GAT'> integers;
```

```
stlkrn!DriverEntry+0xea:
0: kd> dx integers
integers                 : { size=10 } [Type: std::vector<int,jxy::details::allocator<int,1,809976148> >]
    [<Raw View>]     [Type: std::vector<int,jxy::details::allocator<int,1,809976148> >]
    [capacity]       : 10
    [allocator]      : {...} [Type: std::_Compressed_pair<jxy::details::allocator<int,1,809976148>,std::_Vector_val<std::_Simple_types<int> >,1>]
    [0]              : 1 [Type: int]
    [1]              : 2 [Type: int]
    [2]              : 3 [Type: int]
    [3]              : 4 [Type: int]
    [4]              : 5 [Type: int]
    [5]              : 6 [Type: int]
    [6]              : 7 [Type: int]
    [7]              : 8 [Type: int]
    [8]              : 9 [Type: int]
    [9]              : 10 [Type: int]
```

Below is table of functionality under the `jxy` namespace:

| jxy | STL equivalent | Include | Notes |
| ------ | -------------- | ------- | ----- |
| `jxy::allocator` | `std::allocator` | `<jxy/memory.hpp>` | |
| `jxy::default_delete` | `std::default_delete` | `<jxy/memory.hpp>` | |
| `jxy::unique_ptr` | `std::unique_ptr` | `<jxy/memory.hpp>` | |
| `jxy::shared_ptr` | `std::shared_ptr` | `<jxy/memory.hpp>` | |
| `jxy::basic_string` | `std::basic_string` | `<jxy/string.hpp>`| |
| `jxy::string` | `std::string` | `<jxy/string.hpp>` | |
| `jxy::wstring` | `std::wstring` | `<jxy/string.hpp>` | |
| `jxy::vector` | `std::vector` | `<jxy/vector.hpp>` | |
| `jxy::map` | `std::map` | `<jxy/map.hpp>` | |
| `jxy::multimap` | `std::miltimap` | `<jxy/map.hpp>` | |
| `jxy::mutex` | `std::mutex` | `<jxy/locks.hpp>` | Uses `KGUARDED_MUTEX` |
| `jxy::shared_mutex` | `std::shared_mutex` | `<jxy/locks.hpp>` | Uses `EX_PUSH_LOCK` |
| `jxy::unique_lock` | `std::unique_lock` | `<jxy/locks.hpp>` | |
| `jxy::shared_lock` | `std::shared_lock` | `<jxy/locks.hpp>` | |
| `jxy::scope_resource` | None | `<jxy/scope.hpp>` | Similar to `std::experimental::unique_resource` |
| `jxy::scope_exit` | None | `<jxy/scope.hpp>` | Similar to `std::experimental::scope_exit` |
| `jxy::thread` | `std::thread` | `<jxy/thread.hpp>` | |
| `jxy::deque` | `std::deque` | `<jxy/deque.hpp>` | |
| `jxy::queue` | `std:queue` | `<jxy/queue.hpp>` | |
| `jxy::priority_queue` | `std::priority_queue` | `<jxy/queue.hpp>` | |
| `jxy::set` | `std::set` | `<jxy/set.hpp>` | |
| `jxy::multiset` | `std::multiset` | `<jxy/set.hpp>` | |
| `jxy::stack` | `std::stack` | `<jxy/stack.hpp>` | |
| `jxy::unordered_map` | `std::unordered_map` | `<jxy/unordered_map.hpp>` | |
| `jxy::unordered_multimap` | `std::unordered_multimap` | `<jxy/unordered_map.hpp>` | |
| `jxy::unordered_set` | `std::unordered_set` | `<jxy/unordered_set.hpp>` | |
| `jxy::unordered_multiset` | `std::unordered_multiset` | `<jxy/unordered_set.hpp>` | |

## Tests - `stltest.sys`

The `stltest` project implements a driver that runs some tests against jxystl, 
usage of STL, and exceptions in the Windows Kernel.

## Practical Usage - `stlkrn.sys` 
The `stlkrn` project is a Windows Driver that uses `jxystl.lib` to implement 
process, thread, and module tracking in the Windows Kernel.

`stlkrn.sys` registers for process, thread, and image notifications using 
functionality exported by `ntoskrnl`. Using these callbacks it tracks 
processes, threads, and image loads in various objects which use `jxy::map`, 
`jxy::shared_mutex`, `jxy::wstring`, and more.

The driver has two singletons. `jxy::ProcessMap` and `jxy::ThreadMap`, these 
are constructed when the driver loads (`DriverEntry`) and torn down when 
the driver unloads (`DriverUnload`). It is worth noting here each process 
tracked in the `jxy::ProcessMap` (implemented as `jxy::ProcessContext`) also 
manages a `jxy::ThreadMap`. Each "context" (`jxy::ProcessContext`, 
`jxy::ThreadContext`, and `jxy::ModuleContext`) is a shared (referenced) 
object (`jxy::shared_ptr`). Therefore, the thread context that exists in the 
thread map singleton is the same context associated with the process context.

Key components of `stlkrn.sys`:

| Object | Purpose | Source | Notes |
| ------ | ------- | ------ | ----- |
| `jxy::ProcessContext` | Information for a process running on the system. | `process_context.hpp/cpp` | Uses `jxy::wstring`. Has thread (`jxy::ThreadMap`) and module (`jxy::ModuleMap`) map members. | 
| `jxy::ThreadContext` | Information for a thread running on the system. | `thread_context.hpp/cpp` | Uses `std::atomic`. |
| `jxy::ModuleContext` | Information for an image loaded in a given process. | `module_context.hpp/cpp` | Uses `jxy::wstring` and `jxy::shared_mutex`. |
| `jxy::ProcessMap` | Singleton, maps shared `jxy::ProcessContext` objects to a PID. | `process_map.hpp/cpp` | Singleton is accessed via `jxy::GetProcessMap`. Uses `jxy::shared_mutex` and `jxy::map`. |
| `jxy::ThreadMap` | Maps shared `jxy::ThreadContext` objects to a TID. | `thread_map.hpp/cpp` | The global thread table (singleton) is accessed via `jxy::GetThreadMap`. Each `jxy::ProcessContext` also has a thread map which is accessed through `jxy::ProcessContext::GetThreads`. Uses `jxy::shared_mutex` and `jxy::map`. |
| `jxy::GetModuleMap` | Maps shared `jxy::ModuleContext` to a loaded image extents (base and end address). | `module_map.hpp/cpp` | Each process context has a module map member. Loaded images for a given process are tracked using this object. Uses `jxy::shared_mutex` and `jxy::map` |

```
stlkrn!jxy::nt::CreateProcessNotifyRoutine+0xa6:
3: kd> dx proc
proc                 : {...} [Type: std::shared_ptr<jxy::ProcessContext>]
    [<Raw View>]     [Type: std::shared_ptr<jxy::ProcessContext>]
    [ptr]            : 0xffffaa020d73cf70 [Type: jxy::ProcessContext *]
    [control block]  : custom deleter, custom allocator [Type: std::_Ref_count_resource_alloc<jxy::ProcessContext *,jxy::details::default_delete<jxy::ProcessContext,1,1668307018>,jxy::details::allocator<jxy::ProcessContext,1,1668307018> > (derived from std::_Ref_count_base)]
    [+0x000] m_ProcessId      : 0x2760 [Type: unsigned int]
    [+0x004] m_SessionId      : 0x2 [Type: unsigned int]
    [+0x008] m_ParentProcessId : 0xcc4 [Type: unsigned int]
    [+0x010] m_FileName       : "\Device\HarddiskVolume4\Windows\System32\cmd.exe" [Type: std::basic_string<unsigned short,std::char_traits<unsigned short>,jxy::details::allocator<unsigned short,1,1852856394> >]
    [+0x030] m_FilePart       : "cmd.exe" [Type: std::basic_string<unsigned short,std::char_traits<unsigned short>,jxy::details::allocator<unsigned short,1,1886410826> >]
    [+0x050] m_CreatorProcessId : 0x1b08 [Type: unsigned int]
    [+0x054] m_CreatorThreadId : 0x26a0 [Type: unsigned int]
    [+0x058] m_Threads        [Type: jxy::ThreadMap]
    [+0x070] m_Modules        [Type: jxy::ModuleMap]
```

## A note on `xhash` and `_XHASH_NO_FLOATING_POINT`
In order to support `unordered_map`, `unordered_set`, and the multi variants 
`jxystl.lib` copies `xhash` from the MSVC STL and implements an alternative
bucket calculation algorithm. The STL implementation uses floating point
arithmetic, supporting this in the kernel is non-trivial and ill advised. To 
avoid floating point arithmetic `jxystl` patches `xhash` with a conditionally
compiled `_XHASH_NO_FLOATING_POINT`. I expect the `jxystl` implementation here
to be faster than the STL's since it completely avoids floating point arithmetic,
however I have not yet had time run good experiments on this hypothesis.

In the future, I would like to submit a pull request to the MSVC STL to affect
this change natively in the MSVC `xhash` implementation.

## Disclaimer
This solution is a passion project. At this time it is not intended for 
production code. `x64` is well tested and stable, `stlkrn.sys` passes full 
driver verifier options (including randomized low resource simulation). 
Exception handling at or above dispatch has been tested, but not in practical 
use cases. `x86` has *not* been tested.  There is functionality under the 
`jxy` namespace that is incomplete/unused/untested.  _Your milage may vary_ - 
I would like to continue this work over time, if any issues/bugs are found 
feel free to open issues against this repo.

## Related Work

This project provides STL support in the Windows Kernel by using as much of the 
STL facility as possible. There are other solutions for use of STL in kernel 
development. This section will outline alternatives, first I will summarize 
this work:

This Project:
 - Uses the STL directly. Does **not** reimplement any STL functionality unless absolutely necessary.
 - Requires pool types and tags. No global `new` or `delete` is implemented.
 - Forbid moving data between objects of different pools or tags.
 - Avoids CRT initialization and `atexit` functionality. CRT initialization order 
   is non-obvious, driver initialization and teardown *should be obvious*. `atexit` functionality 
   may introduce data races for kernel code, `atexit` is not implemented.

[Bareflank Hypervisor][github.bareflank]:

Bareflank implements support for running C++ in their hypervisor. They have full STL and CRT 
support. This is a comprehensive project that enables a plethora features of the standard in 
kernel mode (including exceptions). As I understand their solution forces `NonPagedPool` on global 
`new`/`delete` allocations. I have to commend Bareflank with their implementation, it's well 
thought out and cross platform. However the Windows implementation builds through cygwin and 
"shims" in support for the Windows kernel. In comparison, this project aims to be considerate to 
the Windows kernel. It enables specifying pool tags and types (paged vs non-paged) and hopes 
to minimize "sharp edges" associated with using C++ and the STL in kernel mode. All that said, 
Bareflank is impressive for what is does. For an excellent presentation on Bareflank's support of 
C++ I highly recommend watching [Dr. Rian Quinn's presentation at cppcon 2016][channel9.bareflank].

[Win32KernelSTL][github.Win32KernelSTL]:

The Win32KernelSTL project does allow you to use STL functionality directly in the kernel. The project 
implements global `new`/`delete` and forces `NonPagedPool`, it implements CRT initialization support, 
and bugchecks when a cpp exception is thrown. It makes no attempt to do cpp exception unwinding. Due 
to the assumptions it makes I find it unpractical for any serious use cases. The code is reasonably 
clear and documented, I recommend giving this project a browse for educating around C++ support in the 
kernel. One note, the CRT code in Win32KernelSTL does implement `atexit` but keep in mind there is no 
synchronization emitted by the compiler here (as opposed to user mode). So a local static requiring 
insertion of an entry in the `atexit` list may race causing a double-init or double-free.

[Driver Plus Plus][github.dxx]:

This project implements necessary C++ facility for pulling in a number of C++ solutions into 
kernel mode (`EASTL`, `msgpack`, etc.). Driver Plus Plus implements CRT initialization and global 
`new`/`delete` support (which forces `NonPagedPool`). Again this is counter to the goals of this 
project. However, this project does enable a lot of great C++ facility for use in kernel mode. It 
does make modifications to the C++ solutions it pulls in to shim in support for it's use cases. 
Driver Plus Plus also makes the assumption around `atexit` as mentioned previously.

[KTL][github.ktl]:

KTL (Windows Kernel Template Library) reimplements a good amount of modern C++ functionality for 
use in the Windows Kernel. It also implements global `new`/`delete` but does a decent job 
at providing facility for specifying pool tags and types where possible. However this does mean 
the global allocator might hide an allocation in a non-obvious pool. Further the template 
allocators in this project carry the cost of two points for an allocator and deallocator object, 
I am also concerned that conversion between the allocator types may allow for cross pool/tag 
allocs/frees. Overall I'm impressed by the amount of facility that is implemented here. 
Reimplementation of STL functionality and the global allocators are counter to the ideologies of 
this project.

[Kernel-Bridge][github.KernelBridge]:

Kernel-Bridge implements some great facility for Windows Kernel development. The library provides 
wrappers for registering for Windows callbacks using C++ objects. I would like to find more time 
to use and investigate this solution. It does implement CRT support. The `atexit` functionality 
implemented is not dynamic - it uses a static array, if it runs out of slots, it fails. The 
default `new`/`delete` forces `NonPagedPool`. It does not have full exception support, it will 
bugcheck if a cpp exception is thrown - it will not unwind objects on the stack.

## Credits
This repository draws from some preexisting work. Credits to their authors.

- [C++ Exceptions in Windows Drivers][github.vcrtl]  
This project implements parts of the Visual Studio runtime library that are 
needed for C++ exception handling. Currently, x86 and x64 platforms are 
supported.
- [Process Hacker Native API Headers][github.phnt]   
Collection of Native API header files. Gathered from Microsoft header files and 
symbol files, as well as a lot of reverse engineering and guessing.

[//]: # (Hyperlink IDs)
[github.vcrtl]: https://github.com/avakar/vcrtl
[github.vcrtl.x64]: https://github.com/avakar/vcrtl/tree/master/src/x64
[github.phnt]: https://github.com/processhacker/phnt
[github.bareflank]: https://github.com/Bareflank/hypervisor
[channel9.bareflank]: https://channel9.msdn.com/Events/CPP/CppCon-2016/CppCon-2016-Rian-Quinn-Making-C-and-the-STL-Work-in-the-Linux--Windows-Kernels
[github.Win32KernelSTL]: https://github.com/DragonQuestHero/Win32KernelSTL
[github.dxx]: https://github.com/sidyhe/dxx
[github.ktl]: https://github.com/MeeSong/KTL
[github.KernelBridge]: https://github.com/HoShiMin/Kernel-Bridge