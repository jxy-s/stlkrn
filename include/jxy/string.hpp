//
// Copyright (c) Johnny Shaw. All rights reserved.
// 
// File:     jxystl/string.hpp
// Author:   Johnny Shaw
// Abstract: STL wrappers for <string> 
//
// jxylib               STL equivalent
// ---------------------------------------------------------------------------
// jxy::basic_string    std::basic_string 
// jxy::string          std::string 
// jxy::wstring         std::wstring 
//
#pragma once
#include <jxy/memory.hpp>
#include <string>

namespace jxy
{

template <typename T, 
          POOL_TYPE t_PoolType, 
          ULONG t_PoolTag, 
          typename TAllocator = jxy::allocator<T, t_PoolType, t_PoolTag>>
using basic_string = std::basic_string<T, std::char_traits<T>, TAllocator>;

template <POOL_TYPE t_PoolType, 
          ULONG t_PoolTag, 
          typename TAllocator = jxy::allocator<char, t_PoolType, t_PoolTag>>
using string = basic_string<char, t_PoolType, t_PoolTag, TAllocator>;

template <POOL_TYPE t_PoolType, 
          ULONG t_PoolTag, 
          typename TAllocator = jxy::allocator<wchar_t, t_PoolType, t_PoolTag>>
using wstring = basic_string<wchar_t, t_PoolType, t_PoolTag, TAllocator>;

}
