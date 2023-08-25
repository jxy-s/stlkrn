//
// Copyright (c) Johnny Shaw. All rights reserved.
// 
// File:     stltest/unordered_set_tests.cpp
// Author:   Johnny Shaw
//
#include "tests_common.hpp"
#include <jxy/unordered_set.hpp>
#include <jxy/string.hpp>

namespace jxy::Tests
{

void UnorderedSetTests()
{
    using StringType = jxy::string<PagedPool, '0GAT'>;

    {
        jxy::unordered_set<int, PagedPool, '0GAT'> set;

        set.insert(1);
        UT_ASSERT(*set.find(1) == 1);

        UT_ASSERT(set.get_allocator().pool_tag == '0GAT');
        UT_ASSERT(set.get_allocator().pool_type == PagedPool);
    }

    {
        jxy::unordered_set<int, NonPagedPoolNx, '0GAT'> set;

        set.insert(1);
        UT_ASSERT(*set.find(1) == 1);

        UT_ASSERT(set.get_allocator().pool_tag == '0GAT');
        UT_ASSERT(set.get_allocator().pool_type == NonPagedPoolNx);
    }

    {
        jxy::unordered_set<StringType, PagedPool, '0GAT'> set;

        set.insert("Hello");
        set.insert("World!");
        UT_ASSERT(*set.find("Hello") == "Hello");
        UT_ASSERT(*set.find("World!") == "World!");

        UT_ASSERT(set.get_allocator().pool_tag == '0GAT');
        UT_ASSERT(set.get_allocator().pool_type == PagedPool);
    }

    {
        jxy::unordered_set<int, PagedPool, '0GAT'> set;

        set.max_load_factor(11);

        for (int i = 0; i < 1024; i++)
        {
            set.insert(i);
        }

        for (int i = 0; i < 1024; i++)
        {
            UT_ASSERT(*set.find(i) == i);
        }
    }

    using StringType = jxy::string<PagedPool, '0GAT'>;

    {
        jxy::unordered_multiset<int, PagedPool, '0GAT'> set;

        set.insert(1);
        set.insert(1);
        set.insert(2);
        UT_ASSERT(set.count(1) == 2);
        UT_ASSERT(set.count(2) == 1);

        UT_ASSERT(set.get_allocator().pool_tag == '0GAT');
        UT_ASSERT(set.get_allocator().pool_type == PagedPool);
    }
}

}
