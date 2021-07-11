//
// Copyright (c) Johnny Shaw. All rights reserved.
// 
// File:     stltest/map_tests.cpp
// Author:   Johnny Shaw
//
#include "tests_common.hpp"
#include <jxy/map.hpp>

namespace jxy::Tests
{

void MapTests()
{
    {
        jxy::map<int, int, PagedPool, '0GAT'> map{ { 1, 1 }, { 2, 2 } };

        UT_ASSERT(map.get_allocator().pool_tag == '0GAT');
        UT_ASSERT(map.get_allocator().pool_type == PagedPool);

        UT_ASSERT(map.count(1) == 1);
        UT_ASSERT(map.size() == 2);
        UT_ASSERT(map[1] == 1);
        UT_ASSERT(map[2] == 2);
    }
    {
        jxy::map<int, int, PagedPool, '0GAT'> map;
        auto res = map.emplace(1, 1);  
        UT_ASSERT(res.second == true);
        UT_ASSERT(res.first->first == 1);
        UT_ASSERT(res.first->second == 1);
        UT_ASSERT(map.at(1) == 1);
        res = map.insert({ 2, 2 });
        UT_ASSERT(res.second == true);
        UT_ASSERT(res.first->first == 2);
        UT_ASSERT(res.first->second == 2);
        auto it = map.find(1);
        UT_ASSERT(it != map.end());
        UT_ASSERT(it->second == 1);
        it = map.find(3);
        UT_ASSERT(it == map.end());
        UT_ASSERT(map.size() == 2);
        UT_ASSERT(map.empty() == false);
        map.clear();
        UT_ASSERT(map.size() == 0);
        UT_ASSERT(map.empty() == true);

        res = map.try_emplace(1, 1);
        UT_ASSERT(res.second == true);
        res = map.try_emplace(1, 2);
        UT_ASSERT(res.second == false);
        UT_ASSERT(res.first->second == 1);

        jxy::map<int, int, PagedPool, '0GAT'> map2{ { 0, 0 }, { 2, 2 } };

        map.merge(map2);
        UT_ASSERT(map[0] == 0);
        UT_ASSERT(map[1] == 1);
        UT_ASSERT(map[2] == 2);
        UT_ASSERT(map2.empty());

        map.swap(map2);
        UT_ASSERT(map2[0] == 0);
        UT_ASSERT(map2[1] == 1);
        UT_ASSERT(map2[2] == 2);
        UT_ASSERT(map.empty());

        map[9] = 9;
        UT_ASSERT(map[9] == 9);
    }
    {
        jxy::multimap<int, int, PagedPool, '0GAT'> map;

        map.insert({ 1, 1 });
        map.insert({ 1, 2 });
        
        UT_ASSERT(map.count(1) == 2);
    }
}

}
