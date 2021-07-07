//
// Copyright (c) Johnny Shaw. All rights reserved.
// 
// File:     jxystl/list.hpp
// Author:   Johnny Shaw
// Abstract: STL wrappers for <list> 
//
// jxylib               STL equivalent
// ---------------------------------------------------------------------------
// jxy::list            std::list 
//
#pragma once
#include <jxy/memory.hpp>
#include <list>

namespace jxy
{

template <typename T, 
          POOL_TYPE t_PoolType, 
          ULONG t_PoolTag, 
          typename TAllocator = jxy::allocator<T, t_PoolType, t_PoolTag>> 
using list = std::list<T, TAllocator>;

}
