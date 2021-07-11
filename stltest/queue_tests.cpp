//
// Copyright (c) Johnny Shaw. All rights reserved.
// 
// File:     stltest/queue_tests.cpp
// Author:   Johnny Shaw
//
#include "tests_common.hpp"
#include <jxy/queue.hpp>

namespace jxy::Tests
{

void QueueTests()
{
    {
        jxy::queue<int, PagedPool, '0GAT'> queue;

        UT_ASSERT(decltype(queue)::container_type::allocator_type::pool_tag == '0GAT');
        UT_ASSERT(decltype(queue)::container_type::allocator_type::pool_type == PagedPool);

        queue.push(1);
        UT_ASSERT(queue.front() == 1);
        UT_ASSERT(queue.back() == 1);
        UT_ASSERT(queue.empty() == false);
        UT_ASSERT(queue.size() == 1);

        queue.emplace(2);
        UT_ASSERT(queue.front() == 1);
        UT_ASSERT(queue.back() == 2);

        jxy::queue<int, PagedPool, '0GAT'> queue2;

        queue.swap(queue2);
        UT_ASSERT(queue.empty() == true);
        UT_ASSERT(queue2.empty() == false);

        queue2.pop();
        queue2.pop();
        UT_ASSERT(queue2.empty() == true);
    }
    {
        jxy::priority_queue<int, PagedPool, '0GAT'> pqueue;

        pqueue.push(1);
        pqueue.push(2);

        UT_ASSERT(pqueue.top() == 2);

        pqueue.pop();
    }
}

}
