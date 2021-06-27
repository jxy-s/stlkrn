//
// Copyright (c) Johnny Shaw. All rights reserved.
// 
// File:     stlkrn/thread_map.cpp
// Author:   Johnny Shaw
// Abstract: Thread Map 
//
#include "thread_map.hpp"

namespace jxy
{

//
// This is the global thread map singleton. It is allocated/generated and torn
// down during load and unload of the driver. It should be accessed by the
// public jxy::GetThreadMap function.
//
static jxy::ThreadMap* g_ThreadMap = nullptr;

}

jxy::ThreadMap& jxy::GetThreadMap()
{
    NT_ASSERT(g_ThreadMap != nullptr);
    return *g_ThreadMap;
}

NTSTATUS jxy::AllocateThreadMap() try
{
    NT_ASSERT(g_ThreadMap == nullptr);

    auto thrdMap = jxy::make_unique<ThreadMap,
                                    PagedPool,
                                    PoolTags::ThreadMap>();
    g_ThreadMap = thrdMap.release();

    return STATUS_SUCCESS;
}
catch (const std::bad_alloc&)
{
    return STATUS_INSUFFICIENT_RESOURCES;
}

void jxy::DeleteThreadMap()
{
    jxy::unique_ptr<ThreadMap, PagedPool, PoolTags::ThreadMap> thrdMap;
    thrdMap.reset(g_ThreadMap);
}

jxy::ThreadMap::ThreadContextType 
jxy::ThreadMap::TrackThread(ThreadContextType ThreadContext) noexcept(false)
{
    jxy::unique_lock<jxy::shared_mutex> lock(m_SharedMutex);

    auto res = m_Map.try_emplace(ThreadContext->GetThreadId(),
                                 ThreadContext);

    return res.first->second;
}

jxy::ThreadMap::ThreadContextType 
jxy::ThreadMap::UntrackThread(ThreadIdType ThreadId) noexcept
{
    jxy::unique_lock<jxy::shared_mutex> lock(m_SharedMutex);

    auto it = m_Map.find(ThreadId);
    if (it == m_Map.end())
    {
        return nullptr;
    }

    auto res = it->second;
    m_Map.erase(it);

    return res;
}

jxy::ThreadMap::ThreadContextType 
jxy::ThreadMap::UntrackThread(ThreadContextType ThreadContext) noexcept
{
    return UntrackThread(ThreadContext->GetThreadId());
}

jxy::ThreadMap::ThreadContextType 
jxy::ThreadMap::LookupThread(ThreadIdType ThreadId) noexcept
{
    jxy::shared_lock<jxy::shared_mutex> lock(m_SharedMutex);

    auto it = m_Map.find(ThreadId);
    if (it == m_Map.end())
    {
        return nullptr;
    }

    return it->second;
}


