//
// Copyright (c) Johnny Shaw. All rights reserved.
// 
// File:     stlkrn/process_map.cpp
// Author:   Johnny Shaw
// Abstract: Process Map 
//
#include "process_map.hpp"
#include "ntfill.hpp"

namespace jxy
{

//
// This is the global process map singleton. It is allocated/generated and torn
// down during load and unload of the driver. It should be accessed by the
// public jxy::GetProcessMap function.
//
static jxy::ProcessMap* g_ProcessMap = nullptr;

}

jxy::ProcessMap& jxy::GetProcessMap()
{
    NT_ASSERT(g_ProcessMap != nullptr);
    return *g_ProcessMap;
}

NTSTATUS jxy::AllocateProcessMap() try
{
    NT_ASSERT(g_ProcessMap == nullptr);

    auto procMap = jxy::make_unique<ProcessMap,
                                    PagedPool, 
                                    PoolTags::ProcessMap>();
    g_ProcessMap = procMap.release();

    return STATUS_SUCCESS;
}
catch (const std::bad_alloc&)
{
    return STATUS_INSUFFICIENT_RESOURCES;
}

void jxy::DeleteProcessMap()
{
    jxy::unique_ptr<ProcessMap, PagedPool, PoolTags::ProcessMap> procMap;
    procMap.reset(g_ProcessMap);
    g_ProcessMap = nullptr;
}

NTSTATUS jxy::ProcessMap::Populate() try 
{
    //
    // This function populates the process contexts and thread maps at
    // driver entry. If we try and fail, we catch the exception and return a
    // failure status to the driver entry, allowing it to tear down the
    // tracking and exit cleanly.
    //
    // If allocations fail here, we'll catch it at function scope fail to load.
    // If the system can't satisfy our allocations during initialization
    // there isn't much we could do. We could run without a fully populated
    // map, but we'll let the driver entry make that decision.
    //

    jxy::unique_lock<jxy::shared_mutex> lock(m_SharedMutex);

    jxy::vector<uint8_t, PagedPool, 'mpij'> info;
    ULONG returnLen = 0;

    //
    // FIXME: should handle expected return codes 
    //
    ZwQuerySystemInformation(SystemProcessInformation, nullptr, 0, &returnLen);

    info.assign(returnLen, 0);

    auto status = ZwQuerySystemInformation(SystemProcessInformation,
                                           &info[0], 
                                           static_cast<ULONG>(info.size()),
                                           &returnLen);
    if (!NT_SUCCESS(status))
    {
        return status;
    }

    //
    // FIXME: this could be done a bit safer/cleaner
    //
    for (auto pi = reinterpret_cast<PSYSTEM_PROCESS_INFORMATION>(info.data());
         (pi->NextEntryOffset > 0);
         pi = reinterpret_cast<PSYSTEM_PROCESS_INFORMATION>(reinterpret_cast<ULONG_PTR>(pi) + pi->NextEntryOffset))
    {
        auto pid = HandleToULong(pi->UniqueProcessId);

        jxy::nt::scoped_eprocess eproc;

        if (pi->UniqueProcessId == 0)
        {
            //
            // KiInitialProcess is not exported and PsLookupProcessByProcessId
            // won't give us the process object of the idle process.
            // We'll special case and handle tracking this process below.
            //
            NOTHING;
        }
        else
        {
            status = PsLookupProcessByProcessId(pi->UniqueProcessId,
                                                &eproc);
            if (!NT_SUCCESS(status))
            {
                //
                // If this fails, continue on, we just won't have context on
                // this process. This is unexpected, so we'll assert.
                //
                NT_ASSERT(NT_SUCCESS(status));
                eproc.release();
                continue;
            }
        }

        ProcessContext::FileNameStringType fileName;
        ProcessContext::FilePartStringType filePart;

        if (pi->UniqueProcessId == 0)
        {
            fileName.assign(L"System Idle Process");
            filePart.assign(L"System Idle Process");
        }
        else if (eproc.get() == PsInitialSystemProcess)
        {
            fileName.assign(L"System");
            filePart.assign(L"System");
        }
        else
        {
            //
            // If we get here we should always have a process object.
            //
            NT_ASSERT(eproc.get() != nullptr);

            status = jxy::nt::GetProcessImageFileName(eproc, fileName);
            if (!NT_SUCCESS(status))
            {
                continue;
            }

            status = jxy::nt::GetFilePart(fileName, filePart);
            if (!NT_SUCCESS(status))
            {
                continue;
            }
        }

        //
        // Now, make the process context and emplace it in the map.
        // We try_emplace since we could see a process creation before
        // Populate is called, the race there is small, but if we do
        // we continue on with the existing process context, the
        // newly created on will be immediately dropped.
        //
        auto context = MakeProcessContext(
                              pid,
                              pi->SessionId,
                              HandleToULong(pi->InheritedFromUniqueProcessId),
                              std::move(fileName),
                              std::move(filePart));

        context = m_Map.try_emplace(pid, context).first->second;

        //
        // Now walk the threads and populate them.
        //
        for (ULONG i = 0; i < pi->NumberOfThreads; i++)
        {
            const auto& ti = pi->Threads[i];

            auto tid = HandleToULong(ti.ClientId.UniqueThread);
            NT_ASSERT(pid == HandleToULong(ti.ClientId.UniqueProcess));

            if (tid == 0)
            {
                //
                // System Idle Process (PID 0) has idle wait threads without
                // TIDs, we'll ignore those for now. All other processes
                // should have non-zero TIDs.
                //
                NT_ASSERT(context->GetProcessId() == 0);
                continue;
            }

            //
            // Track the thread in the global thread map and in this 
            // process context. The thread contexts are shared (referenced)
            // objects, so the same context exists in both maps.
            //
            auto threadContext = GetThreadMap().TrackThread(pid, tid);
            context->GetThreads().TrackThread(threadContext);
        }

        //
        // TODO: Enumerate loaded modules and populate image map.
        // For now we only will populate the image map for image's we've seen
        // get loaded through the load image notify routine.
        //
    }

    return STATUS_SUCCESS;
}
catch (const std::bad_alloc&)
{
    return STATUS_INSUFFICIENT_RESOURCES;
}

jxy::ProcessMap::ProcessContextType 
jxy::ProcessMap::TrackProcess(ProcessContextType ProcessContext) noexcept(false)
{
    jxy::unique_lock<jxy::shared_mutex> lock(m_SharedMutex);

    auto res = m_Map.try_emplace(ProcessContext->GetProcessId(),
                                 ProcessContext);

    return res.first->second;
}

jxy::ProcessMap::ProcessContextType 
jxy::ProcessMap::UntrackProcess(ProcessIdType ProcessId) noexcept
{
    jxy::unique_lock<jxy::shared_mutex> lock(m_SharedMutex);

    auto it = m_Map.find(ProcessId);
    if (it == m_Map.end())
    {
        return nullptr;
    }

    auto res = it->second;
    m_Map.erase(it);

    return res;
}

jxy::ProcessMap::ProcessContextType 
jxy::ProcessMap::UntrackProcess(ProcessContextType ProcessContext) noexcept
{
    return UntrackProcess(ProcessContext->GetProcessId());
}

jxy::ProcessMap::ProcessContextType 
jxy::ProcessMap::LookupProcess(ProcessIdType ProcessId) noexcept
{
    jxy::shared_lock<jxy::shared_mutex> lock(m_SharedMutex);

    auto it = m_Map.find(ProcessId);
    if (it == m_Map.end())
    {
        return nullptr;
    }

    return it->second;
}
