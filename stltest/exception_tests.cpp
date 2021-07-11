//
// Copyright (c) Johnny Shaw. All rights reserved.
// 
// File:     stltest/exception_tests.cpp
// Author:   Johnny Shaw
//
#include "tests_common.hpp"
#include <exception>
#include <stdexcept>
#include <jxy/scope.hpp>

namespace jxy::Tests
{

void ExceptionTests()
{
    bool caught = false;
    try
    {
        throw 1;
    }
    catch (int)
    {
        caught = true;
    }
    UT_ASSERT(caught);

    caught = false;
    try
    {
        throw std::exception("Test");
    }
    catch (...)
    {
        caught = true;
    }
    UT_ASSERT(caught);

    caught = false;
    try
    {
        throw std::exception("Test");
    }
    catch (const std::exception& exc)
    {
        caught = (strcmp(exc.what(), "Test") == 0);
    }
    UT_ASSERT(caught);

    caught = false;
    try 
    {
        throw std::logic_error("Test");
    }
    catch (const std::logic_error& exc)
    {
        caught = (strcmp(exc.what(), "Test") == 0);
    }
    UT_ASSERT(caught);

    caught = false;
    try 
    {
        throw std::invalid_argument("Test");
    }
    catch (const std::invalid_argument& exc)
    {
        caught = (strcmp(exc.what(), "Test") == 0);
    }
    UT_ASSERT(caught);

    caught = false;
    try 
    {
        throw std::invalid_argument("Test");
    }
    catch (const std::logic_error& exc)
    {
        caught = (strcmp(exc.what(), "Test") == 0);
    }
    UT_ASSERT(caught);

    caught = false;
    try 
    {
        throw std::runtime_error("Test");
    }
    catch (const std::runtime_error& exc)
    {
        caught = (strcmp(exc.what(), "Test") == 0);
    }
    UT_ASSERT(caught);

    caught = false;
    try 
    {
        throw std::range_error("Test");
    }
    catch (const std::runtime_error& exc)
    {
        caught = (strcmp(exc.what(), "Test") == 0);
    }
    UT_ASSERT(caught);

    caught = false;
    try 
    {
        throw std::bad_alloc();
    }
    catch (const std::bad_alloc&)
    {
        caught = true;
    }
    UT_ASSERT(caught);

    //
    // Again at dispatch
    //
    KIRQL prevIrql;
    KeRaiseIrql(DISPATCH_LEVEL, &prevIrql);

    caught = false;
    try
    {
        throw 1;
    }
    catch (int)
    {
        caught = true;
    }
    UT_ASSERT(caught);

    caught = false;
    try
    {
        throw std::exception("Test");
    }
    catch (...)
    {
        caught = true;
    }
    UT_ASSERT(caught);

    caught = false;
    try
    {
        throw std::exception("Test");
    }
    catch (const std::exception& exc)
    {
        caught = (strcmp(exc.what(), "Test") == 0);
    }
    UT_ASSERT(caught);

    caught = false;
    try 
    {
        throw std::logic_error("Test");
    }
    catch (const std::logic_error& exc)
    {
        caught = (strcmp(exc.what(), "Test") == 0);
    }
    UT_ASSERT(caught);

    caught = false;
    try 
    {
        throw std::invalid_argument("Test");
    }
    catch (const std::invalid_argument& exc)
    {
        caught = (strcmp(exc.what(), "Test") == 0);
    }
    UT_ASSERT(caught);

    caught = false;
    try 
    {
        throw std::invalid_argument("Test");
    }
    catch (const std::logic_error& exc)
    {
        caught = (strcmp(exc.what(), "Test") == 0);
    }
    UT_ASSERT(caught);

    caught = false;
    try 
    {
        throw std::runtime_error("Test");
    }
    catch (const std::runtime_error& exc)
    {
        caught = (strcmp(exc.what(), "Test") == 0);
    }
    UT_ASSERT(caught);

    caught = false;
    try 
    {
        throw std::range_error("Test");
    }
    catch (const std::runtime_error& exc)
    {
        caught = (strcmp(exc.what(), "Test") == 0);
    }
    UT_ASSERT(caught);

    caught = false;
    try 
    {
        throw std::bad_alloc();
    }
    catch (const std::bad_alloc&)
    {
        caught = true;
    }
    UT_ASSERT(caught);

    KeLowerIrql(prevIrql);

    //
    // Unwind test
    //
    bool unwound = false;
    caught = false;
    try
    {
        jxy::scope_exit exit([&unwound]() -> void { unwound = true; });
        throw 1;
    }
    catch (int)
    {
        caught = true;
    }
    UT_ASSERT(caught);
    UT_ASSERT(unwound);

    unwound = false;
    caught = false;
    try
    {
        jxy::scope_exit exit([&unwound]() -> void { unwound = true; });
        throw std::exception("Test");
    }
    catch (...)
    {
        caught = true;
    }
    UT_ASSERT(caught);
    UT_ASSERT(unwound);

    unwound = false;
    caught = false;
    try
    {
        jxy::scope_exit exit([&unwound]() -> void { unwound = true; });
        throw std::exception("Test");
    }
    catch (const std::exception& exc)
    {
        caught = (strcmp(exc.what(), "Test") == 0);
    }
    UT_ASSERT(caught);
    UT_ASSERT(unwound);

    unwound = false;
    caught = false;
    try 
    {
        jxy::scope_exit exit([&unwound]() -> void { unwound = true; });
        throw std::logic_error("Test");
    }
    catch (const std::logic_error& exc)
    {
        caught = (strcmp(exc.what(), "Test") == 0);
    }
    UT_ASSERT(caught);
    UT_ASSERT(unwound);

    unwound = false;
    caught = false;
    try 
    {
        jxy::scope_exit exit([&unwound]() -> void { unwound = true; });
        throw std::invalid_argument("Test");
    }
    catch (const std::invalid_argument& exc)
    {
        caught = (strcmp(exc.what(), "Test") == 0);
    }
    UT_ASSERT(caught);
    UT_ASSERT(unwound);

    unwound = false;
    caught = false;
    try 
    {
        jxy::scope_exit exit([&unwound]() -> void { unwound = true; });
        throw std::invalid_argument("Test");
    }
    catch (const std::logic_error& exc)
    {
        caught = (strcmp(exc.what(), "Test") == 0);
    }
    UT_ASSERT(caught);
    UT_ASSERT(unwound);

    unwound = false;
    caught = false;
    try 
    {
        jxy::scope_exit exit([&unwound]() -> void { unwound = true; });
        throw std::runtime_error("Test");
    }
    catch (const std::runtime_error& exc)
    {
        caught = (strcmp(exc.what(), "Test") == 0);
    }
    UT_ASSERT(caught);
    UT_ASSERT(unwound);

    unwound = false;
    caught = false;
    try 
    {
        jxy::scope_exit exit([&unwound]() -> void { unwound = true; });
        throw std::range_error("Test");
    }
    catch (const std::runtime_error& exc)
    {
        caught = (strcmp(exc.what(), "Test") == 0);
    }
    UT_ASSERT(caught);
    UT_ASSERT(unwound);

    unwound = false;
    caught = false;
    try 
    {
        jxy::scope_exit exit([&unwound]() -> void { unwound = true; });
        throw std::bad_alloc();
    }
    catch (const std::bad_alloc&)
    {
        caught = true;
    }
    UT_ASSERT(caught);
    UT_ASSERT(unwound);
}

}
