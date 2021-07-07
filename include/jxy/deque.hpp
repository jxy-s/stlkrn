//
// Copyright (c) Johnny Shaw. All rights reserved.
// 
// File:     jxystl/deque.hpp
// Author:   Johnny Shaw
// Abstract: STL wrappers for <deque> 
//
// jxylib               STL equivalent
// ---------------------------------------------------------------------------
// jxy::deque            std::deque 
//
#pragma once
#include <jxy/memory.hpp>
#include <deque>

namespace jxy
{

template <typename T, 
          POOL_TYPE t_PoolType, 
          ULONG t_PoolTag, 
          typename TAllocator = jxy::allocator<T, t_PoolType, t_PoolTag>> 
using deque = std::deque<T, TAllocator>;

}
