//
// Copyright (c) Johnny Shaw. All rights reserved.
// 
// File:     stltest/deque_tests.cpp
// Author:   Johnny Shaw
//
#include "tests_common.hpp"
#include <jxy/deque.hpp>

namespace jxy::Tests
{

void DequeTests()
{
    {
        jxy::deque<int, PagedPool, '0GAT'> deque;

        UT_ASSERT(deque.get_allocator().pool_tag == '0GAT');
        UT_ASSERT(deque.get_allocator().pool_type == PagedPool);

        deque.push_front(1);
        deque.push_back(2);

        UT_ASSERT(deque.front() == 1);
        UT_ASSERT(deque.back() == 2);

        UT_ASSERT(deque.empty() == false);
        UT_ASSERT(deque.size() == 2);

        deque.pop_back();
        deque.pop_front();

        UT_ASSERT(deque.empty() == true);

        deque.emplace_front(1);
        deque.emplace_back(1);

        UT_ASSERT(deque.front() == 1);
        UT_ASSERT(deque.back() == 1);

        UT_ASSERT(deque.empty() == false);
        UT_ASSERT(deque.size() == 2);

        deque.resize(10);

        deque.erase(deque.begin(), deque.end());

        UT_ASSERT(deque.empty() == true);

        deque.push_front(3);
        deque.push_front(2);
        deque.push_front(1);

        UT_ASSERT(deque.at(0) == 1);
        UT_ASSERT(deque[1] == 2);

        jxy::deque<int, PagedPool, '0GAT'> deque2;

        deque.swap(deque2);

        UT_ASSERT(deque.empty() == true);
        UT_ASSERT(deque2.empty() == false);
    }
}

}
