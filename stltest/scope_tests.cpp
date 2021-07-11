//
// Copyright (c) Johnny Shaw. All rights reserved.
// 
// File:     stltest/scope_tests.cpp
// Author:   Johnny Shaw
//
#include "tests_common.hpp"
#include <jxy/scope.hpp>

namespace jxy::Tests
{

struct scope_release_test_functor
{
    void operator()(bool* Called)
    {
        UT_ASSERT(*Called == false);
        *Called = true;
    }
};

using scope_resource_test_obj = jxy::scope_resource<bool*, scope_release_test_functor>;

void ScopeTests()
{
    bool called = false;
    {
        scope_resource_test_obj obj(&called);
    }
    UT_ASSERT(called);

    called = false;
    {
        jxy::scope_exit exit([&called]() -> void { called = true; });
    }
    UT_ASSERT(called);

    called = false;
    {
        jxy::scope_exit exit([&called]() -> void { called = true; });
        exit.release();
    }
    UT_ASSERT(!called);
}

}
