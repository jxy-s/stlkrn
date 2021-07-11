//
// Copyright (c) Johnny Shaw. All rights reserved.
// 
// File:     jxystl/vector.hpp
// Author:   Johnny Shaw
// Abstract: STL wrappers for <vector> 
//
// jxylib               STL equivalent
// ---------------------------------------------------------------------------
// jxy::vector          std::vector 
//
#pragma once
#include <jxy/memory.hpp>
#include <vector>

namespace jxy
{

template <typename T, 
          POOL_TYPE t_PoolType, 
          ULONG t_PoolTag, 
          typename TAllocator = jxy::allocator<T, t_PoolType, t_PoolTag>> 
using vector = std::vector<T, TAllocator>;

template <typename T,
          ULONG t_PoolTag,
          typename TAllocator = jxy::allocator<T, PagedPool, t_PoolTag>>
using paged_vector = std::vector<T, TAllocator>;

template <typename T,
          ULONG t_PoolTag,
          typename TAllocator = jxy::allocator<T, NonPagedPoolNx, t_PoolTag>>
using non_paged_vector = std::vector<T, TAllocator>;

}
