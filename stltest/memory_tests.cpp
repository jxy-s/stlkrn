//
// Copyright (c) Johnny Shaw. All rights reserved.
// 
// File:     stltests/memory_tests.cpp
// Author:   Johnny Shaw
//
#include "tests_common.hpp"
#include <jxy/memory.hpp>

namespace jxy::Tests
{

void MemoryTests()
{
    {
        jxy::allocator<int, PagedPool, '0GAT'> alloc;
        auto mem = alloc.allocate(10);
        alloc.deallocate(mem, 10);

        UT_ASSERT(alloc.pool_tag == '0GAT');
        UT_ASSERT(alloc.pool_type == PagedPool);
    }
    {
        jxy::allocator<int, NonPagedPoolNx, '0GAT'> alloc;
        auto mem = alloc.allocate(10);
        alloc.deallocate(mem, 10);

        UT_ASSERT(alloc.pool_tag == '0GAT');
        UT_ASSERT(alloc.pool_type == NonPagedPoolNx);
    }
    {
        jxy::paged_allocator<int, '0GAT'> alloc;
        auto mem = alloc.allocate(10);
        alloc.deallocate(mem, 10);

        UT_ASSERT(alloc.pool_tag == '0GAT');
        UT_ASSERT(alloc.pool_type == PagedPool);
    }
    {
        jxy::non_paged_allocator<int, '0GAT'> alloc;
        auto mem = alloc.allocate(10);
        alloc.deallocate(mem, 10);

        UT_ASSERT(alloc.pool_tag == '0GAT');
        UT_ASSERT(alloc.pool_type == NonPagedPoolNx);
    }
    {
        jxy::default_delete<int, PagedPool, '0GAT'> deleter;
        UT_ASSERT(decltype(deleter)::pool_tag == '0GAT');
        UT_ASSERT(decltype(deleter)::pool_type == PagedPool);
    }
    {
        auto ptr = jxy::make_unique<int, PagedPool, '0GAT'>(1);
        UT_ASSERT(*ptr == 1);

        UT_ASSERT(ptr.get_deleter().pool_tag == '0GAT');
        UT_ASSERT(ptr.get_deleter().pool_type == PagedPool);
    }
    {
        auto ptr = jxy::make_unique<int, NonPagedPoolNx, '0GAT'>(1);
        UT_ASSERT(*ptr == 1);
        UT_ASSERT(ptr.get_deleter().pool_tag == '0GAT');
        UT_ASSERT(ptr.get_deleter().pool_type == NonPagedPoolNx);
    }
    {
        auto ptr = jxy::make_shared<int, PagedPool, '0GAT'>(1);
        UT_ASSERT(*ptr == 1);
    }
    {
        auto ptr = jxy::make_shared<int, NonPagedPoolNx, '0GAT'>(1);
        UT_ASSERT(*ptr == 1);
    }
}

}
