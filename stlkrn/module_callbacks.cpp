//
// Copyright (c) Johnny Shaw. All rights reserved.
// 
// File:     stlkrn/module_callbacks.cpp
// Author:   Johnny Shaw
// Abstract: Image Load Callbacks
//
#include "module_callbacks.hpp"
#include "process_map.hpp"

namespace jxy::nt
{

static bool g_ImageLoadCallbackRegistered = false;

void LoadImageNotifyRoutine(
    PUNICODE_STRING FullImageName,
    HANDLE ProcessId,
    PIMAGE_INFO ImageInfo)
{
    UNREFERENCED_PARAMETER(ImageInfo);

    auto pid = HandleToULong(ProcessId);

    auto proc = GetProcessMap().LookupProcess(pid);
    if (proc == nullptr)
    {
        //
        // We have no context on this process, nothing to do.
        //
        return;
    }

    ModuleProperties props;
    props.ImagePartialMap = (ImageInfo->ImagePartialMap ? true : false);
    props.ImageSignatureLevel = ImageInfo->ImageSignatureLevel;
    props.ImageSignatureType = static_cast<SE_IMAGE_SIGNATURE_TYPE>(ImageInfo->ImageSignatureType);
    props.MachineTypeMismatch = (ImageInfo->MachineTypeMismatch ? true : false);
    props.SystemModeImage = (ImageInfo->SystemModeImage ? true : false);

    ModuleContext::FileNameStringType fileName;
    ModuleContext::FilePartStringType filePart;
    try
    {
        fileName = ConvertUnicodeString<decltype(fileName)>(FullImageName);
    }
    catch (const std::bad_alloc&)
    {
        return;
    }

    auto status = GetFilePart(fileName, filePart);
    if (!NT_SUCCESS(status))
    {
        return;
    }

    ModuleExtents extents;
    extents.Start = reinterpret_cast<uintptr_t>(ImageInfo->ImageBase);
    extents.End = extents.Start + ImageInfo->ImageSize;
    NT_ASSERT(extents.Start <= extents.End);

    //
    // Look up the module in the process context.
    //
    auto modl = proc->GetModules().LookupModule(extents);
    if (modl == nullptr)
    {
        //
        // The module doesn't exist, try to make and track it in the
        // process context.
        //
        try
        {
            modl = proc->GetModules().TrackModule(
                                       HandleToULong(PsGetCurrentProcessId()),
                                       HandleToULong(PsGetCurrentThreadId()),
                                       extents,
                                       props,
                                       std::move(fileName),
                                       std::move(filePart));
        }
        catch (const std::bad_alloc&)
        {
        }
        return;
    }

    //
    // The module exists already in the process context.
    //
    // Another module may have been mapped to conflicting addresses.
    // Account for this by checking the image file name. If they are
    // different we will replace it.
    // This could be improved but not worth the effort for this example.
    //
    if (modl->GetFileName() == fileName)
    {
        //
        // Same module was loaded again. Just update who loaded it.
        // Again could be improved.
        //
        modl->UpdateOnDuplicateImageLoad(HandleToULong(PsGetCurrentProcessId()),
                                         HandleToULong(PsGetCurrentThreadId()),
                                         props);
        return;
    }

    //
    // A module with a different file name was mapped over the first.
    // Replace it.
    //

    proc->GetModules().UntrackModule(modl);

    try
    {
        modl = proc->GetModules().TrackModule(
                                       HandleToULong(PsGetCurrentProcessId()),
                                       HandleToULong(PsGetCurrentThreadId()),
                                       extents,
                                       props,
                                       std::move(fileName),
                                       std::move(filePart));
    }
    catch (const std::bad_alloc&)
    {
    }
}

}

NTSTATUS jxy::nt::RegisterLoadImageCallback()
{
    auto status = PsSetLoadImageNotifyRoutineEx(LoadImageNotifyRoutine,
                                                PS_IMAGE_NOTIFY_CONFLICTING_ARCHITECTURE);
    if (!NT_SUCCESS(status))
    {
        return status;
    }

    g_ImageLoadCallbackRegistered = true;
    return STATUS_SUCCESS;
}

void jxy::nt::UnregisterLoadImageCallback()
{
    if (g_ImageLoadCallbackRegistered)
    {
        PsRemoveLoadImageNotifyRoutine(LoadImageNotifyRoutine);
        g_ImageLoadCallbackRegistered = false;
    }
}
