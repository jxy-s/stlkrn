//
// Copyright (c) Johnny Shaw. All rights reserved.
// 
// File:     jxystl/stack.hpp
// Author:   Johnny Shaw
// Abstract: STL wrappers for <stack> 
//
// jxylib               STL equivalent
// ---------------------------------------------------------------------------
// jxy::stack           std::stack 
//
#pragma once
#include <jxy/deque.hpp>
#include <stack>

namespace jxy
{

template <typename T, 
          POOL_TYPE t_PoolType, 
          ULONG t_PoolTag, 
          typename TContainer = jxy::deque<T, t_PoolType, t_PoolTag>>
using stack = std::stack<T, TContainer>;

}
