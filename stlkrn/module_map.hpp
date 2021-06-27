//
// Copyright (c) Johnny Shaw. All rights reserved.
// 
// File:     stlkrn/module_map.cpp
// Author:   Johnny Shaw
// Abstract: Module Map 
//
// This module map class managed module context objects. Each process context
// contains a module map which tracks each loaded module in the process.
// Each module context managed in the map is a shared (referenced) object.
//
#pragma once
#include <fltKernel.h>
#include <jxy/map.hpp>
#include <jxy/vector.hpp>
#include <jxy/locks.hpp>
#include "pool_tags.hpp"
#include "module_context.hpp"
#include "nthelp.hpp"

namespace jxy
{

class ModuleMap
{
public:

    using ModuleContextType = jxy::shared_ptr<ModuleContext, 
                                              PoolTypes::ModuleContext,
                                              PoolTags::ModuleContext>;
    using MapType = jxy::map<ModuleExtents, 
                             ModuleContextType,
                             PagedPool,
                             PoolTags::ModuleMap>;

    ~ModuleMap() noexcept = default;

    ModuleMap() = default;

    ModuleContextType TrackModule(ModuleContextType ModuleContext) noexcept(false);

    ModuleContextType UntrackModule(const ModuleExtents& Extents) noexcept;

    ModuleContextType UntrackModule(ModuleContextType ModuleContext) noexcept;

    ModuleContextType LookupModule(const ModuleExtents& Extents) noexcept;

    template <POOL_TYPE t_PoolType, 
              ULONG t_PoolTag, 
              typename TAllocator = jxy::allocator<ModuleContextType, t_PoolType, t_PoolTag>>
    jxy::vector<ModuleContextType, t_PoolType, t_PoolTag, TAllocator>
    Snapshot()  noexcept(false)
    {
        jxy::vector<ModuleContextType, t_PoolType, t_PoolTag, TAllocator> res;

        jxy::shared_lock<jxy::shared_mutex> lock(m_SharedMutex);

        res.reserve(m_Map.count());

        for (auto& entry : m_Map)
        {
            res.push_back(entry.second);
        }

        return res;
    }

    template <typename... TArgs>
    ModuleContextType TrackModule(TArgs&&... Args) noexcept(false)
    {
        return TrackModule(MakeModuleContext(std::forward<TArgs>(Args)...));
    }

private:

    template <typename... TArgs>
    ModuleContextType MakeModuleContext(TArgs&&... Args) noexcept(false)
    {
        return jxy::make_shared<ModuleContext,
                                PoolTypes::ModuleContext,
                                PoolTags::ModuleContext>(
                                    std::forward<TArgs>(Args)...);
    }

    jxy::shared_mutex m_SharedMutex;
    MapType m_Map;

};

}

