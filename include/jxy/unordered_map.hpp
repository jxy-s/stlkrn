//
// Copyright (c) Johnny Shaw. All rights reserved.
// 
// File:     jxystl/unordered_map.hpp
// Author:   Johnny Shaw
// Abstract: STL wrappers for <unordered_map> 
//
// jxylib                   STL equivalent
// ---------------------------------------------------------------------------
// jxy::unordered_map       std::unordered_map 
// jxy::unordered_miltimap  std::unordered_miltimap 
//
#pragma once
#include <jxy/memory.hpp>

//
// We make a patch to xhash to eliminate floating point arithmetic. To do this
// jxystl copies the MSVC xhash and modifies it slightly. To enforce this we
// make our new define and include "xhash" directly from our source, then
// include the functionality that will use it.
//
// I would like, in the future, to PR this change to the MSVC STL.
//
#define _XHASH_NO_FLOATING_POINT 1
#include "xhash"
#include <unordered_map>

namespace jxy
{

template <typename TKey,
          typename T,
          POOL_TYPE t_PoolType,
          ULONG t_PoolTag,
          typename THash = std::hash<TKey>,
          typename TKeyEqual = std::equal_to<TKey>,
          typename TAllocator = jxy::allocator<std::pair<const TKey, T>, t_PoolType, t_PoolTag>>
using unordered_map = std::unordered_map<TKey, T, THash, TKeyEqual, TAllocator>;

template <typename TKey,
          typename T,
          POOL_TYPE t_PoolType,
          ULONG t_PoolTag,
          typename THash = std::hash<TKey>,
          typename TKeyEqual = std::equal_to<TKey>,
          typename TAllocator = jxy::allocator<std::pair<const TKey, T>, t_PoolType, t_PoolTag>>
using unordered_multimap = std::unordered_multimap<TKey, T, THash, TKeyEqual, TAllocator>;

}
