//
// Copyright (c) Johnny Shaw. All rights reserved.
// 
// File:     stltest/unordered_map_tests.cpp
// Author:   Johnny Shaw
//
#include "tests_common.hpp"
#include <jxy/unordered_map.hpp>
#include <jxy/string.hpp>

namespace jxy::Tests
{

void UnorderedMapTests()
{
    using StringType = jxy::string<PagedPool, '0GAT'>;

    {
        jxy::unordered_map<int, int, PagedPool, '0GAT'> map;

        map[1] = 1;
        UT_ASSERT(map[1] == 1);

        UT_ASSERT(map.get_allocator().pool_tag == '0GAT');
        UT_ASSERT(map.get_allocator().pool_type == PagedPool);
    }

    {
        jxy::unordered_map<int, int, NonPagedPoolNx, '0GAT'> map;

        map[1] = 1;
        UT_ASSERT(map[1] == 1);

        UT_ASSERT(map.get_allocator().pool_tag == '0GAT');
        UT_ASSERT(map.get_allocator().pool_type == NonPagedPoolNx);
    }

    {
        jxy::unordered_map<StringType, StringType, PagedPool, '0GAT'> map;

        map["Hello"] = "World!";
        UT_ASSERT(map["Hello"] == "World!");

        UT_ASSERT(map.get_allocator().pool_tag == '0GAT');
        UT_ASSERT(map.get_allocator().pool_type == PagedPool);
    }

    {
        jxy::unordered_map<StringType, StringType, PagedPool, '0GAT'> map;

        map["Hello"] = "Hello";
        map["World!"] = "World!";
        UT_ASSERT(map["Hello"] == "Hello");
        UT_ASSERT(map["World!"] == "World!");

        UT_ASSERT(map.get_allocator().pool_tag == '0GAT');
        UT_ASSERT(map.get_allocator().pool_type == PagedPool);
    }

    {
        jxy::unordered_map<int, StringType, PagedPool, '0GAT'> map;

        map.max_load_factor(11);

        for (int i = 0; i < 1024; i++)
        {
            map[i] = "testing";
        }

        for (int i = 0; i < 1024; i++)
        {
            UT_ASSERT(map[i] == "testing");
        }
    }

    using StringType = jxy::string<PagedPool, '0GAT'>;

    {
        jxy::unordered_multimap<int, int, PagedPool, '0GAT'> map;

        map.insert({ 1, 1 });
        map.insert({ 1, 1 });
        map.insert({ 2, 1 });
        UT_ASSERT(map.count(1) == 2);
        UT_ASSERT(map.count(2) == 1);

        UT_ASSERT(map.get_allocator().pool_tag == '0GAT');
        UT_ASSERT(map.get_allocator().pool_type == PagedPool);
    }
}

}
