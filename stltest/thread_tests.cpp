//
// Copyright (c) Johnny Shaw. All rights reserved.
// 
// File:     stltests/thread_tests.cpp
// Author:   Johnny Shaw
//
#include "tests_common.hpp"
#include <jxy/thread.hpp>

namespace jxy::Tests
{

void ThreadTests()
{
    {
        int val = 0;
        PETHREAD threadObj = nullptr;
        jxy::thread::id threadId;
        jxy::thread t([&val, &threadObj, &threadId]() -> void
                      {
                          UT_ASSERT(val == 0);
                          val = 1;
                          threadObj = PsGetCurrentThread();
                          threadId = jxy::this_thread::get_id();
                      });
        auto expectedThreadId = t.get_id();
        auto expectedThreadObj = t.native_handle();
        UT_ASSERT(t.joinable() == true);
        t.join();
        UT_ASSERT(val == 1);
        UT_ASSERT(expectedThreadId == threadId);
        UT_ASSERT(expectedThreadObj == threadObj);
    }
}

}
