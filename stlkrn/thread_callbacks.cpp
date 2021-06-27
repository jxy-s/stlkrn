//
// Copyright (c) Johnny Shaw. All rights reserved.
// 
// File:     stlkrn/thread_callbacks.cpp
// Author:   Johnny Shaw
// Abstract: Thread Notify Callbacks 
//
#include "thread_callbacks.hpp"
#include "process_map.hpp"

namespace jxy::nt
{

static bool g_ThreadCreateCallbackRegistered = false;
static bool g_ThreadExecuteCallbackRegistered = false;

void CreateThreadNotifyRoutine(
    HANDLE ProcessId,
    HANDLE ThreadId,
    BOOLEAN Create)
{
    ProcessMap::ProcessContextType proc;
    ThreadMap::ThreadContextType thrd;

    auto pid = HandleToULong(ProcessId);
    auto tid = HandleToULong(ThreadId);

    if (Create == FALSE)
    {
        //
        // Thread exiting, stop tracking it from both the process context 
        // and the global thread map.
        //
        proc = GetProcessMap().LookupProcess(pid);
        if (proc != nullptr)
        {
            thrd = proc->GetThreads().UntrackThread(tid);
        }

        thrd = GetThreadMap().UntrackThread(tid);
        return;
    }

    //
    // New thread
    //

    //
    // We will only track the thread if we have a process context to 
    // associate it to.
    //
    proc = GetProcessMap().LookupProcess(pid);
    if (proc == nullptr)
    {
        return;
    }

    //
    // Try to track it in the global map
    //
    try
    {
        thrd = GetThreadMap().TrackThread(pid,
                                          tid,
                                          HandleToULong(PsGetCurrentProcessId()),
                                          HandleToULong(PsGetCurrentThreadId()));
    }
    catch (const std::bad_alloc&)
    {
        //
        // Out of memory, bail.
        //
        return;
    }

    NT_ASSERT(thrd != nullptr);

    try
    {
        proc->GetThreads().TrackThread(thrd);
    }
    catch (const std::bad_alloc&)
    {
        //
        // If we failed to track it in the process map, stop tracking it
        // in the global map. This won't throw.
        //
        GetThreadMap().UntrackThread(thrd);
    }
}

void ExecuteThreadNotifyRoutine(
    HANDLE ProcessId,
    HANDLE ThreadId,
    BOOLEAN Create)
{
    UNREFERENCED_PARAMETER(ProcessId);

    if (Create == FALSE)
    {
        return;
    }

    //
    // There isn't much we do here for now, just set some state in the thread
    // denoting we've seen the thread execute notification fire.
    //

    auto thrd = GetThreadMap().LookupThread(HandleToULong(ThreadId));
    if (thrd != nullptr)
    {
        thrd->SetExecuteThreadNotifyFired();
    }
}

}

NTSTATUS jxy::nt::RegisterThreadCallback()
{
    NT_ASSERT(!g_ThreadCreateCallbackRegistered && 
              !g_ThreadExecuteCallbackRegistered);

    auto status = PsSetCreateThreadNotifyRoutineEx(PsCreateThreadNotifySubsystems,
                                                   CreateThreadNotifyRoutine);
    if (!NT_SUCCESS(status))
    {
        return status;
    }

    g_ThreadCreateCallbackRegistered = true;

    if (NT_SUCCESS(PsSetCreateThreadNotifyRoutineEx(PsCreateThreadNotifyNonSystem,
                                                    ExecuteThreadNotifyRoutine)))
    {
        g_ThreadExecuteCallbackRegistered = true;
    }

    return STATUS_SUCCESS;
}

void jxy::nt::UnregisterThreadCallback()
{
    if (g_ThreadExecuteCallbackRegistered)
    {
        PsRemoveCreateThreadNotifyRoutine(ExecuteThreadNotifyRoutine);
        g_ThreadExecuteCallbackRegistered = false;
    }

    if (g_ThreadCreateCallbackRegistered)
    {
        PsRemoveCreateThreadNotifyRoutine(CreateThreadNotifyRoutine);
        g_ThreadCreateCallbackRegistered = false;
    }
}
