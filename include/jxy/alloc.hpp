//
// Copyright (c) Johnny Shaw. All rights reserved.
// 
// File:     jxystl/alloc.hpp
// Author:   Johnny Shaw
// Abstract: cpp new/delete functions
//
// Default new/delete is intentionally unimplemented! This forces specifying
// the pool type and tags for all allocations.
//
#pragma once
#include <fltKernel.h>
#include <cstddef>

void* __cdecl operator new(size_t Size, POOL_TYPE PoolType, ULONG PoolTag) noexcept(false);

void __cdecl operator delete(void* Memory, POOL_TYPE PoolType, ULONG PoolTag) noexcept;

void* __cdecl operator new[](size_t Size, POOL_TYPE PoolType, ULONG PoolTag) noexcept(false);

void __cdecl operator delete[](void* Memory, POOL_TYPE PoolType, ULONG PoolTag) noexcept;
