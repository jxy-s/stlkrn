//
// Copyright (c) Johnny Shaw. All rights reserved.
// 
// File:     stlkrn/main.cpp
// Author:   Johnny Shaw
// Abstract: STL in the Windows Kernel - main entry point 
//
// This file implements DriverEntry and DriverUnload for the stlkrn driver.
//
// The driver will register for process, thread, and image load callbacks.
// It will then use the MSVC STL, jxystl wrappers, and vcrtl to show usage
// of modern C++ with exception handling in the Windows kernel. The jxystl 
// library is tuned for Windows kernel workloads, pool handling, and pool
// tagging.
//
// The "jxy" namespace wraps the "std" namespace functionality and forces
// specifying pool type and pool tags. The jxystl library "fills" the missing
// MSVC functionality. This functionality is implemented in jxystl.lib and
// imported here - it is implemented/tuned for kernel-use.
// 
// The vcrtl implements cpp exception support. When an exception is 
// encountered this library will unwind the stack and reclaim C++ objects.
// https://github.com/avakar/vcrtl
//
#include <fltKernel.h>
#include <jxy/scope.hpp>
#include "process_map.hpp"
#include "process_callbacks.hpp"
#include "thread_callbacks.hpp"
#include "module_callbacks.hpp"

void TeardownCallbacksAndTracking()
{
    jxy::nt::UnregisterLoadImageCallback();
    jxy::nt::UnregisterThreadCallback();
    jxy::nt::UnregisterProcessCallback();
    jxy::DeleteProcessMap();
    jxy::DeleteThreadMap();
}

extern "C"
void DriverUnload(PDRIVER_OBJECT DriverObject)
{
    UNREFERENCED_PARAMETER(DriverObject);

    TeardownCallbacksAndTracking();
}

extern "C"
NTSTATUS DriverEntry(
    PDRIVER_OBJECT DriverObject,
    PUNICODE_STRING RegistryPath)
{
    UNREFERENCED_PARAMETER(RegistryPath);

    NTSTATUS status = STATUS_UNSUCCESSFUL;

    //
    // At scope exit, if we do not succeed, cleanup after ourselves.
    //
    jxy::scope_exit cleanup(
        [&status]() -> void
        {
            if (!NT_SUCCESS(status))
            {
                TeardownCallbacksAndTracking();
            }
        });

    //
    // Set our unload routine.
    //
    DriverObject->DriverUnload = DriverUnload;

    //
    // Allocate the global thread and process map singletons.
    //

    status = jxy::AllocateThreadMap();
    if (!NT_SUCCESS(status))
    {
        return status;
    }

    status = jxy::AllocateProcessMap();
    if (!NT_SUCCESS(status))
    {
        return status;
    }

    //
    // Register for process, thread, and image load callbacks.
    //

    status = jxy::nt::RegisterProcessCallback();
    if (!NT_SUCCESS(status))
    {
        return status;
    }

    status = jxy::nt::RegisterThreadCallback();
    if (!NT_SUCCESS(status))
    {
        return status;
    }

    status = jxy::nt::RegisterLoadImageCallback();
    if (!NT_SUCCESS(status))
    {
        return status;
    }

    //
    // Now that we're registered, tell the global process map singleton to
    // populate itself. The order here is important, we do so after
    // registering for the process, thread, and image load callbacks.
    // This function will grab the internal process map lock exclusive, then 
    // walk the system information to populate itself. This means it will
    // block the registered callbacks until we've fully populated the
    // necessary state. We might "see" a new process before getting here,
    // that's fine - Populate handles it cleanly.
    //

    status = jxy::GetProcessMap().Populate();
    if (!NT_SUCCESS(status))
    {
        return status;
    }

    return STATUS_SUCCESS;
}
