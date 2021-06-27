//
// Copyright (c) Johnny Shaw. All rights reserved.
// 
// File:     stlkrn/module_map.cpp
// Author:   Johnny Shaw
// Abstract: Module Map 
//
#include "module_map.hpp"

jxy::ModuleMap::ModuleContextType 
jxy::ModuleMap::TrackModule(ModuleContextType ModuleContext) noexcept(false)
{
    jxy::unique_lock<jxy::shared_mutex> lock(m_SharedMutex);

    auto res = m_Map.try_emplace(ModuleContext->GetExtents(),
                                 ModuleContext);

    return res.first->second;
}

jxy::ModuleMap::ModuleContextType 
jxy::ModuleMap::UntrackModule(const ModuleExtents& Extents) noexcept
{
    jxy::unique_lock<jxy::shared_mutex> lock(m_SharedMutex);

    auto it = m_Map.find(Extents);
    if (it == m_Map.end())
    {
        return nullptr;
    }

    auto res = it->second;
    m_Map.erase(it);

    return res;
}

jxy::ModuleMap::ModuleContextType 
jxy::ModuleMap::UntrackModule(ModuleContextType ModuleContext) noexcept
{
    return UntrackModule(ModuleContext->GetExtents());
}

jxy::ModuleMap::ModuleContextType 
jxy::ModuleMap::LookupModule(const ModuleExtents& Extents) noexcept
{
    jxy::shared_lock<jxy::shared_mutex> lock(m_SharedMutex);

    auto it = m_Map.find(Extents);
    if (it == m_Map.end())
    {
        return nullptr;
    }

    return it->second;
}
