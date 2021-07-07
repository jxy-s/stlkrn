//
// Copyright (c) Johnny Shaw. All rights reserved.
// 
// File:     jxystl/map.hpp
// Author:   Johnny Shaw
// Abstract: STL wrappers for <map> 
//
// jxylib               STL equivalent
// ---------------------------------------------------------------------------
// jxy::map             std::map 
// jxy::multimap        std::multimap 
//
#pragma once
#include <jxy/memory.hpp>
#include <map>

namespace jxy
{

template <typename TKey,
          typename T,
          POOL_TYPE t_PoolType,
          ULONG t_PoolTag,
          typename TLess = std::less<TKey>,
          typename TAlloc = allocator<std::pair<const TKey, T>, t_PoolType, t_PoolTag>>
using map = std::map<TKey, T, TLess, TAlloc>;

template <typename TKey,
          typename T,
          POOL_TYPE t_PoolType,
          ULONG t_PoolTag,
          typename TLess = std::less<TKey>,
          typename TAlloc = allocator<std::pair<const TKey, T>, t_PoolType, t_PoolTag>>
using multimap = std::multimap<TKey, T, TLess, TAlloc>;

}