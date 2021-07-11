//
// Copyright (c) Johnny Shaw. All rights reserved.
// 
// File:     stltest/locks_tests.cpp
// Author:   Johnny Shaw
//
#include "tests_common.hpp"
#include <jxy/locks.hpp>

namespace jxy::Tests
{

void LocksTests()
{
    //
    // Commented out parts here throw system error, which needs work
    //
    {
        jxy::shared_mutex slock;
        {
            jxy::shared_lock<jxy::shared_mutex> lock(slock);
            UT_ASSERT(lock.mutex() == &slock);
            //lock.unlock();
            //lock.lock();
            UT_ASSERT(lock.owns_lock() == true);
            //lock.unlock();
            //UT_ASSERT(lock.owns_lock() == false);
            //UT_ASSERT(lock.try_lock() == true);
        }
        {
            jxy::unique_lock<jxy::shared_mutex> lock(slock);
            UT_ASSERT(lock.mutex() == &slock);
            //lock.unlock();
            //lock.lock();
            UT_ASSERT(lock.owns_lock() == true);
            //lock.unlock();
            //UT_ASSERT(lock.owns_lock() == false);
            //UT_ASSERT(lock.try_lock() == true);
        }
        slock.lock();
        slock.unlock();
        slock.lock_shared();
        slock.unlock_shared();
        UT_ASSERT(slock.try_lock() == true);
        slock.unlock();
        UT_ASSERT(slock.try_lock_shared() == true);
        slock.unlock_shared();
    }
    {
        jxy::mutex<'0GAT'> mutex;
        {
            jxy::unique_lock<decltype(mutex)> lock(mutex);
            UT_ASSERT(lock.mutex() == &mutex);
            //lock.unlock();
            //lock.lock();
            UT_ASSERT(lock.owns_lock() == true);
            //lock.unlock();
            //UT_ASSERT(lock.owns_lock() == false);
            //UT_ASSERT(lock.try_lock() == true);
        }
        mutex.lock();
        mutex.unlock();
        UT_ASSERT(mutex.try_lock() == true);
        mutex.unlock();
    }
}

}
