//
// Copyright (c) Johnny Shaw. All rights reserved.
// 
// File:     jxystl/memory.hpp
// Author:   Johnny Shaw
// Abstract: MSVC fill functionality 
//
// Some MSVC functionality must be implemented for the STL containers to link
// to. I have consciously decided to implement each with care given to the
// operating environment (kernel). This means that code using the STL directly
// and not through the jxylib wrappers might encounter linker errors.
// This is done intentionally as to not introduce a path which may be unsafe
// for the kernel.
//
#include <fltKernel.h>
#include <stdexcept>
#include <system_error>
#include <intrin.h>

namespace std
{

void __cdecl _Xbad_alloc()
{
    throw std::bad_alloc();
}

void __cdecl _Xinvalid_argument(_In_z_ const char* What)
{
    throw std::invalid_argument(What);
}

void __cdecl _Xlength_error(_In_z_ const char* What)
{
    throw std::length_error(What);
}

void __cdecl _Xout_of_range(_In_z_ const char* What)
{
    throw std::out_of_range(What);
}

void __cdecl _Xoverflow_error(_In_z_ const char* What)
{
    throw std::overflow_error(What);
}

void __cdecl _Xruntime_error(_In_z_ const char* What)
{
    throw std::runtime_error(What);
}

void __cdecl _Throw_Cpp_error(int Code)
{
    //
    // FIXME: This should throw a std::system_error but it comes with global
    // allocator requirements. For now we'll throw a faked "system error" as
    // a runtime error. 
    //
    UNREFERENCED_PARAMETER(Code);
    throw std::runtime_error("jxy: Faked system error!");
}

}

//
// For exception copy, we'll allocate from non paged pool with our own tag.
// We don't necessarily know the full context of the exception, so we use
// non paged pool in case we're executing at dispatch or above.
// In the event we should allocate and copy the exception but the allocation
// fails, we'll store our own global immutable string to help communicate
// the internal failure, rather than just dropping the "What" entirely.
//
static constexpr POOL_TYPE k_ExcCopyPoolType = NonPagedPoolNx;
static constexpr ULONG k_ExcCopyPoolTag = 'pCxE';
static constexpr char g_ExcCopyFailedReplace[] = "jxy: Exception copy allocation failed!";

// std_exception.cpp
extern "C"
void __cdecl __std_exception_copy(
    _In_  __std_exception_data const* From,
    _Out_ __std_exception_data * To)
{
    NT_ASSERT((To->_What == nullptr) && (To->_DoFree == false));

    if (!From->_DoFree || !From->_What)
    {
        To->_What = From->_What;
        To->_DoFree = false;
        return;
    }

    size_t len = strlen(From->_What) + 1;

#pragma warning(push)
#pragma warning(disable : 4996) // FIXME - deprecated function
    char* buff = static_cast<char*>(ExAllocatePoolWithTag(k_ExcCopyPoolType,
                                                          len,
                                                          k_ExcCopyPoolTag));
#pragma warning(pop)
    if (!buff)
    {
        //
        // If we fail to allocate when copying an exception, we'll replace
        // "What" with our own global string rather than just null the "What". 
        //
        To->_What = g_ExcCopyFailedReplace;
        To->_DoFree = false;
        return;
    }

    strcpy_s(buff, len, From->_What);
    To->_What = buff;
    To->_DoFree = true;
}

// std_exception.cpp
extern "C"
void __cdecl __std_exception_destroy(_Inout_ __std_exception_data * Data)
{
    if (Data->_DoFree && Data->_What)
    {
        ExFreePoolWithTag(const_cast<char*>(Data->_What), k_ExcCopyPoolTag);
    }

    Data->_What = nullptr;
    Data->_DoFree = false;
}

// invalid_parameter.cpp
extern "C" 
__declspec(noreturn)
void __cdecl _invalid_parameter_noinfo_noreturn(void)
{
    //
    // AFAIK attempting to continue here is going to blow up regardless.
    // So, we have no choice but to bug check for now.
    //
    NT_ASSERT(false);
    KeBugCheckEx(
        static_cast<ULONG>(STATUS_INVALID_PARAMETER),
        static_cast<ULONG_PTR>(0ull + reinterpret_cast<ULONG_PTR>(_ReturnAddress())),
        static_cast<ULONG_PTR>(0ull + reinterpret_cast<ULONG_PTR>(_AddressOfReturnAddress())),
        0, 
        0);
}

extern "C"
int _fltused = 0;

#pragma warning(push)
#pragma warning(disable: 4510 4512 4610) // This type can never be instantiated
struct __std_type_info_data
{
    char const* _UndecoratedName;
    char const  _DecoratedName[1];
};
#pragma warning(pop)

// std_type_info.cpp
extern "C"
int __cdecl __std_type_info_compare(
    _In_ __std_type_info_data const* Lhs,
    _In_ __std_type_info_data const* Rhs 
    )
{
    if (Lhs == Rhs)
    {
        return 0;
    }

    //
    // Added assurance checks here...
    //
    NT_ASSERT((Lhs != nullptr) && (Rhs != nullptr));
    if ((Lhs == nullptr) && (Rhs != nullptr))
    {
        return -1;
    }
    if ((Lhs != nullptr) && (Rhs == nullptr))
    {
        return 1;
    }

    //
    // Now do the compare.
    //
    return strcmp(Lhs->_DecoratedName + 1, Rhs->_DecoratedName + 1);
}

extern "C"
__declspec(noreturn) 
void __cdecl terminate() throw()
{
    NT_ASSERT(false);
    abort();
}

extern "C"
__declspec(noreturn) 
void __cdecl abort()
{
    NT_ASSERT(false);
    KeBugCheckEx(
        static_cast<ULONG>(STATUS_DRIVER_PROCESS_TERMINATED),
        static_cast<ULONG_PTR>(0ull + reinterpret_cast<ULONG_PTR>(_ReturnAddress())),
        static_cast<ULONG_PTR>(0ull + reinterpret_cast<ULONG_PTR>(_AddressOfReturnAddress())),
        0, 
        0);
}
