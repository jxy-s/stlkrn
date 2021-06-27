//
// Copyright (c) Johnny Shaw. All rights reserved.
// 
// File:     stlkrn/process_map.hpp
// Author:   Johnny Shaw
// Abstract: Process Map 
//
// The process map manages all process contexts. The map is populated at
// load of the driver and updated on every process create notify callbacks.
// Each process context managed in the map is a shared (referenced) object.
//
// This is a singleton that should be accessed via jxy::GetProcessMap.
//
#pragma once
#include <fltKernel.h>
#include <jxy/map.hpp>
#include <jxy/vector.hpp>
#include <jxy/locks.hpp>
#include "pool_tags.hpp"
#include "process_context.hpp"
#include "nthelp.hpp"

namespace jxy
{

class ProcessMap
{
public:

    using ProcessIdType = uint32_t;

    using ProcessContextType = jxy::shared_ptr<ProcessContext, 
                                               PoolTypes::ProcessContext,
                                               PoolTags::ProcessContext>;
    using MapType = jxy::map<ProcessIdType, 
                             ProcessContextType, 
                             PagedPool, 
                             PoolTags::ProcessMap>;

    ~ProcessMap() noexcept = default;

    ProcessMap() = default;

    NTSTATUS Populate();

    ProcessContextType TrackProcess(ProcessContextType ProcessContext) noexcept(false);

    ProcessContextType UntrackProcess(ProcessIdType ProcessId) noexcept;

    ProcessContextType UntrackProcess(ProcessContextType ProcessContext) noexcept;

    ProcessContextType LookupProcess(ProcessIdType ProcessId) noexcept;

    template <POOL_TYPE t_PoolType, 
              ULONG t_PoolTag, 
              typename TAllocator = jxy::allocator<ProcessContextType, t_PoolType, t_PoolTag>>
    jxy::vector<ProcessContextType, t_PoolType, t_PoolTag, TAllocator>
    Snapshot()  noexcept(false)
    {
        jxy::vector<ProcessContextType, t_PoolType, t_PoolTag, TAllocator> res;

        jxy::shared_lock<jxy::shared_mutex> lock(m_SharedMutex);

        res.reserve(m_Map.count());

        for (auto& entry : m_Map)
        {
            res.push_back(entry.second);
        }

        return res;
    }

    template <typename... TArgs>
    ProcessContextType TrackProcess(TArgs&&... Args) noexcept(false)
    {
        return TrackProcess(MakeProcessContext(std::forward<TArgs>(Args)...));
    }

private:

    template <typename... TArgs>
    ProcessContextType MakeProcessContext(TArgs&&... Args) noexcept(false)
    {
        return jxy::make_shared<ProcessContext,
                                PoolTypes::ProcessContext,
                                PoolTags::ProcessContext>(
                                    std::forward<TArgs>(Args)...);
    }

    jxy::shared_mutex m_SharedMutex;
    MapType m_Map;

};

ProcessMap& GetProcessMap();
NTSTATUS AllocateProcessMap();
void DeleteProcessMap();

}
