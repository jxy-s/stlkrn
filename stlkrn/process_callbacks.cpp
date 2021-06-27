//
// Copyright (c) Johnny Shaw. All rights reserved.
// 
// File:     stlkrn/process_callbacks.cpp
// Author:   Johnny Shaw
// Abstract: Process Notify Callbacks 
//
#include "process_callbacks.hpp"
#include "process_map.hpp"
#include "ntfill.hpp"

namespace jxy::nt
{

static bool g_ProcessCreateCallbackRegistered = false;

void CreateProcessNotifyRoutine(
    PEPROCESS Process,
    HANDLE ProcessId,
    PPS_CREATE_NOTIFY_INFO CreateInfo)
{
    jxy::ProcessMap::ProcessContextType proc;

    auto pid = HandleToULong(ProcessId);

    if (!CreateInfo)
    {
        //
        // Process is exiting, stop tracking it.
        //
        proc = jxy::GetProcessMap().UntrackProcess(pid);
        return;
    }

    //
    // New process
    //
    
    jxy::ProcessContext::FileNameStringType fileName;
    auto status = jxy::nt::GetProcessImageFileName(Process, fileName);
    if (!NT_SUCCESS(status))
    {
        return;
    }

    jxy::ProcessContext::FilePartStringType filePart;
    status = jxy::nt::GetFilePart(fileName, filePart);
    if (!NT_SUCCESS(status))
    {
        return;
    }

    //
    // Try to begin tracking this process.
    //
    try
    {
        proc = jxy::GetProcessMap().TrackProcess(
                                   pid,
                                   PsGetProcessSessionId(Process),
                                   HandleToULong(CreateInfo->ParentProcessId),
                                   HandleToULong(PsGetCurrentProcessId()),
                                   HandleToULong(PsGetCurrentThreadId()),
                                   std::move(fileName),
                                   std::move(filePart));
    }
    catch (const std::bad_alloc&)
    {
    }
}

}

NTSTATUS jxy::nt::RegisterProcessCallback()
{
    NT_ASSERT(!g_ProcessCreateCallbackRegistered);

    auto status = PsSetCreateProcessNotifyRoutineEx2(PsCreateProcessNotifySubsystems,
                                                     CreateProcessNotifyRoutine,
                                                     FALSE);
    if (!NT_SUCCESS(status))
    {
        return status;
    }

    g_ProcessCreateCallbackRegistered = true;
    return STATUS_SUCCESS;
}

void jxy::nt::UnregisterProcessCallback()
{
    if (g_ProcessCreateCallbackRegistered)
    {
        PsSetCreateProcessNotifyRoutineEx2(PsCreateProcessNotifySubsystems,
                                           CreateProcessNotifyRoutine,
                                           TRUE);
        g_ProcessCreateCallbackRegistered = false;
    }
}
