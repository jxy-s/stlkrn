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

}
