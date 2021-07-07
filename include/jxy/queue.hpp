//
// Copyright (c) Johnny Shaw. All rights reserved.
// 
// File:     jxystl/queue.hpp
// Author:   Johnny Shaw
// Abstract: STL wrappers for <queue> 
//
// jxylib               STL equivalent
// ---------------------------------------------------------------------------
// jxy::queue           std::queue 
// jxy::priority_queue  std::priority_queue 
//
#pragma once
#include <jxy/deque.hpp>
#include <queue>

namespace jxy
{

template <typename T,
          POOL_TYPE t_PoolType, 
          ULONG t_PoolTag, 
          typename TContainer = jxy::deque<T, t_PoolType, t_PoolTag>>
using queue = std::queue<T, TContainer>;

template <typename T, 
          POOL_TYPE t_PoolType, 
          ULONG t_PoolTag, 
          typename TContainer = jxy::deque<T, t_PoolType, t_PoolTag>,
          typename TCompare = std::less<typename TContainer::value_type>>
using priority_queue = std::priority_queue<T, TContainer, TCompare>;

}
