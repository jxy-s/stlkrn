//
// Copyright (c) Johnny Shaw. All rights reserved.
// 
// File:     jxystl/alloc.cpp
// Author:   Johnny Shaw
// Abstract: cpp new/delete functions
//
// Default new/delete is intentionally unimplemented! This forces specifying
// the pool type and tags for all allocations.
//
#include <jxy/alloc.hpp>
#include <stdexcept>

void* __cdecl operator new(size_t Size, POOL_TYPE PoolType, ULONG PoolTag) noexcept(false)
{
    if (Size == 0)
    {
        Size = 1;
    }

    void* memory = ExAllocatePoolWithTag(PoolType, Size, PoolTag);
    if (!memory)
    {
        throw std::bad_alloc();
    }
    return memory;
}

void __cdecl operator delete(void* Memory, POOL_TYPE, ULONG PoolTag) noexcept
{
    if (Memory)
    {
        ExFreePoolWithTag(Memory, PoolTag);
    }
}

void* __cdecl operator new[](size_t Size, POOL_TYPE PoolType, ULONG PoolTag) noexcept(false)
{
    return operator new(Size, PoolType, PoolTag);
}

void __cdecl operator delete[](void* Memory, POOL_TYPE PoolType, ULONG PoolTag) noexcept
{
    return operator delete(Memory, PoolType, PoolTag);
}
