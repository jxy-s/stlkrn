//
// Copyright (c) Johnny Shaw. All rights reserved.
// 
// File:     jxystl/unordered_set.hpp
// Author:   Johnny Shaw
// Abstract: STL wrappers for <unordered_set> 
//
// jxylib                   STL equivalent
// ---------------------------------------------------------------------------
// jxy::unordered_set       std::unordered_set
// jxy::unordered_miltiset  std::unordered_miltiset 
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
#include <unordered_set>

namespace jxy
{

template <typename TKey,
          POOL_TYPE t_PoolType,
          ULONG t_PoolTag,
          typename THash = std::hash<TKey>,
          typename TKeyEqual = std::equal_to<TKey>,
          typename TAllocator = jxy::allocator<TKey, t_PoolType, t_PoolTag>>
using unordered_set = std::unordered_set<TKey, THash, TKeyEqual, TAllocator>;

template <typename TKey,
          POOL_TYPE t_PoolType,
          ULONG t_PoolTag,
          typename THash = std::hash<TKey>,
          typename TKeyEqual = std::equal_to<TKey>,
          typename TAllocator = jxy::allocator<TKey, t_PoolType, t_PoolTag>>
using unordered_multiset = std::unordered_multiset<TKey, THash, TKeyEqual, TAllocator>;

}
