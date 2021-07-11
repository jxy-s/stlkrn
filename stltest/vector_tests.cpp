//
// Copyright (c) Johnny Shaw. All rights reserved.
// 
// File:     stltest/vector_tests.cpp
// Author:   Johnny Shaw
//
#include "tests_common.hpp"
#include <jxy/vector.hpp>

namespace jxy::Tests
{

void VectorTests()
{
    {
        jxy::vector<int, PagedPool, '0GAT'> vec;
        vec.push_back(1);
        UT_ASSERT(vec.size() == 1);
        UT_ASSERT(vec[0] == 1);
        UT_ASSERT(vec.front() == 1);
        vec.assign({ 5, 6, 7 });
        UT_ASSERT(vec[0] == 5);
        UT_ASSERT(vec[1] == 6);
        UT_ASSERT(vec[2] == 7);
        UT_ASSERT(vec.front() == 5);
        UT_ASSERT(vec.back() == 7);
        UT_ASSERT(vec.size() == 3);
        vec.emplace(vec.begin(), 4);
        UT_ASSERT(vec.size() == 4);
        UT_ASSERT(vec.front() == 4);
        vec.emplace(vec.end(), 8);
        UT_ASSERT(vec.empty() == false);
        vec.insert(vec.begin(), 3);
        UT_ASSERT(vec.front() == 3);
        vec.erase(vec.begin(), vec.begin() + 1);
        vec.shrink_to_fit();

        jxy::vector<int, PagedPool, '0GAT'> vec2;
        vec.swap(vec2);
        vec.clear();
        vec2.clear();
        vec2.push_back(1);
        vec.push_back(1);
        vec2.pop_back();

        UT_ASSERT(vec.get_allocator().pool_tag == '0GAT');
        UT_ASSERT(vec.get_allocator().pool_type == PagedPool);
    }
    {
        jxy::paged_vector<int, '0GAT'> vec;
        vec.push_back(1);
        UT_ASSERT(vec.get_allocator().pool_tag == '0GAT');
        UT_ASSERT(vec.get_allocator().pool_type == PagedPool);
    }
    {
        jxy::non_paged_vector<int, '0GAT'> vec;
        vec.push_back(1);
        UT_ASSERT(vec.get_allocator().pool_tag == '0GAT');
        UT_ASSERT(vec.get_allocator().pool_type == NonPagedPoolNx);
    }
}

}
