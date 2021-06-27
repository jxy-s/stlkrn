//
// Copyright (c) Johnny Shaw. All rights reserved.
// 
// File:     jxystl/string.hpp
// Author:   Johnny Shaw
// Abstract: STL wrappers for <map> 
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

}