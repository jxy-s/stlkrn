//
// Copyright (c) Johnny Shaw. All rights reserved.
// 
// File:     stltest/exception_tests.cpp
// Author:   Johnny Shaw
//
#include "tests_common.hpp"
#include <jxy/stack.hpp>

namespace jxy::Tests
{

void StackTests()
{
    {
        jxy::stack<int, PagedPool, '0GAT'> stack;

        stack.push(1);
        UT_ASSERT(stack.top() == 1);
        stack.push(2);
        UT_ASSERT(stack.top() == 2);
        stack.pop();
        UT_ASSERT(stack.top() == 1);
        stack.emplace(2);
        UT_ASSERT(stack.top() == 2);
        stack.pop();
        UT_ASSERT(stack.top() == 1);
        UT_ASSERT(stack.empty() == false);

        jxy::stack<int, PagedPool, '0GAT'> stack2;

        stack.swap(stack2);

        UT_ASSERT(stack.empty() == true);
        UT_ASSERT(stack2.empty() == false);
        UT_ASSERT(stack2.size() == 1);
    }
}

}

