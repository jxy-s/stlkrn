//
// Copyright (c) Johnny Shaw. All rights reserved.
// 
// File:     stlkrn/thread_map.hpp
// Author:   Johnny Shaw
// Abstract: Thread Map 
//
// The thread map manages a set of thread contexts. Each process context
// contains a thread map for all threads associated with that process. There
// is also a global thread map.
// Each thread context managed here is a shared (referenced) object. So the
// thread context looked up by the global map or through a process context
// is the same referenced object.
//
// The global thread map is a singleton and should be accessed 
// through jxy::GetThreadMap.
//
#pragma once
#include <fltKernel.h>
#include <jxy/map.hpp>
#include <jxy/vector.hpp>
#include <jxy/locks.hpp>
#include "pool_tags.hpp"
#include "thread_context.hpp"
#include "nthelp.hpp"

namespace jxy
{

class ThreadMap
{
public:

    using ThreadIdType = uint32_t;

    using ThreadContextType = jxy::shared_ptr<ThreadContext, 
                                              PoolTypes::ThreadContext,
                                              PoolTags::ThreadContext>;
    using MapType = jxy::map<ThreadIdType, 
                             ThreadContextType, 
                             PagedPool, 
                             PoolTags::ThreadMap>;

    ~ThreadMap() noexcept = default;

    ThreadMap() = default;

    ThreadContextType TrackThread(ThreadContextType ThreadContext) noexcept(false);

    ThreadContextType UntrackThread(ThreadIdType ThreadId) noexcept;

    ThreadContextType UntrackThread(ThreadContextType ThreadContext) noexcept;

    ThreadContextType LookupThread(ThreadIdType ThreadId) noexcept;

    template <POOL_TYPE t_PoolType, 
              ULONG t_PoolTag, 
              typename TAllocator = jxy::allocator<ThreadContextType, t_PoolType, t_PoolTag>>
    jxy::vector<ThreadContextType, t_PoolType, t_PoolTag, TAllocator>
    Snapshot()  noexcept(false)
    {
        jxy::vector<ThreadContextType, t_PoolType, t_PoolTag, TAllocator> res;

        jxy::shared_lock<jxy::shared_mutex> lock(m_SharedMutex);

        res.reserve(m_Map.count());

        for (auto& entry : m_Map)
        {
            res.push_back(entry.second);
        }

        return res;
    }

    template <typename... TArgs>
    ThreadContextType TrackThread(TArgs&&... Args) noexcept(false)
    {
        return TrackThread(MakeThreadContext(std::forward<TArgs>(Args)...));
    }

private:

    template <typename... TArgs>
    ThreadContextType MakeThreadContext(TArgs&&... Args) noexcept(false)
    {
        return jxy::make_shared<ThreadContext,
                                PoolTypes::ThreadContext,
                                PoolTags::ThreadContext>(
                                    std::forward<TArgs>(Args)...);
    }

    jxy::shared_mutex m_SharedMutex;
    MapType m_Map;

};

ThreadMap& GetThreadMap();
NTSTATUS AllocateThreadMap();
void DeleteThreadMap();

}
