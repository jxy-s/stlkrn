//
// Copyright (c) Johnny Shaw. All rights reserved.
// 
// File:     stltest/list_tests.cpp
// Author:   Johnny Shaw
//
#include "tests_common.hpp"
#include <jxy/list.hpp>

namespace jxy::Tests
{

void ListTests()
{
    {
        jxy::list<int, PagedPool, '0GAT'> list;

        UT_ASSERT(list.get_allocator().pool_tag == '0GAT');
        UT_ASSERT(list.get_allocator().pool_type == PagedPool);

        list.assign({ 1 });

        UT_ASSERT(list.size() == 1);
        UT_ASSERT(list.empty() == false);

        UT_ASSERT(list.front() == 1);
        UT_ASSERT(list.back() == 1);

        list.clear();
        UT_ASSERT(list.empty() == true);

        list.insert(list.begin(), 1);
        list.emplace(list.end(), 2);
        list.insert(list.begin(), { 0, 1 });
        auto it = list.begin();
        ++it;
        list.erase(list.begin(), it);
        list.push_back(9);
        list.push_front(1);
        list.pop_back();
        list.pop_front();

        list.insert(list.begin(), { 1, 2, 3, 4, 5, 9 });
        list.unique();

        jxy::list<int, PagedPool, '0GAT'> list2;

        list.swap(list2);
        
        UT_ASSERT(list.empty() == true);
        UT_ASSERT(list2.empty() == false);

        list.splice(list.begin(), list2);

        list.reverse();

        list.sort();
    }
}

}
