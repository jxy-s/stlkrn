//
// Copyright (c) Johnny Shaw. All rights reserved.
// 
// File:     jxystl/set.hpp
// Author:   Johnny Shaw
// Abstract: STL wrappers for <set> 
//
// jxylib               STL equivalent
// ---------------------------------------------------------------------------
// jxy::set             std::set 
// jxy::multiset        std::multiset 
//
#pragma once
#include <jxy/memory.hpp>
#include <set>

namespace jxy
{

template <typename T, 
          POOL_TYPE t_PoolType, 
          ULONG t_PoolTag, 
          typename TCompare = std::less<T>,
          typename TAllocator = jxy::allocator<T, t_PoolType, t_PoolTag>> 
using set = std::set<T, TCompare, TAllocator>;

template <typename T, 
          POOL_TYPE t_PoolType, 
          ULONG t_PoolTag, 
          typename TCompare = std::less<T>,
          typename TAllocator = jxy::allocator<T, t_PoolType, t_PoolTag>> 
using multiset = std::multiset<T, TCompare, TAllocator>;

}
