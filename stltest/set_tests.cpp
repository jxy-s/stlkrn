//
// Copyright (c) Johnny Shaw. All rights reserved.
// 
// File:     stltest/set_tests.cpp
// Author:   Johnny Shaw
//
#include "tests_common.hpp"
#include <jxy/set.hpp>

namespace jxy::Tests
{

void SetTests()
{
    {
        jxy::set<int, PagedPool, '0GAT'> set;

        UT_ASSERT(set.get_allocator().pool_tag == '0GAT');
        UT_ASSERT(set.get_allocator().pool_type == PagedPool);

        set.insert({ 1, 2, 3 });

        UT_ASSERT(set.size() == 3);
        UT_ASSERT(set.empty() == false);

        set.emplace(4);

        UT_ASSERT(set.size() == 4);

        set.erase(set.begin(), set.end());

        UT_ASSERT(set.empty() == true);

        set.insert({ 1, 2, 3 });

        UT_ASSERT(set.find(1) != set.end());
        UT_ASSERT(set.find(4) == set.end());

        jxy::set<int, PagedPool, '0GAT'> set2;

        set.swap(set2);

        UT_ASSERT(set.empty() == true);
        UT_ASSERT(set2.empty() == false);

        set2.clear();
        UT_ASSERT(set2.empty() == true);

        set2.insert({ 1, 2, 4 });
        set.insert({ 3, 5 });
        set.merge(set2);
    }

}

}
