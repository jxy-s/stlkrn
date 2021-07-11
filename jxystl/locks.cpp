//
// Copyright (c) Johnny Shaw. All rights reserved.
// 
// File:     jxystl/locks.cpp
// Author:   Johnny Shaw
// Abstract: STL wrappers for locks (<mutex>, <shared_mutex>, etc.)
//
#include <jxy/locks.hpp>

jxy::shared_mutex::shared_mutex() noexcept
{
    FltInitializePushLock(&m_PushLock);
}

jxy::shared_mutex::~shared_mutex() noexcept
{
    FltDeletePushLock(&m_PushLock);
}

void jxy::shared_mutex::lock() noexcept
{
    FltAcquirePushLockExclusiveEx(&m_PushLock, 0);
}

bool jxy::shared_mutex::try_lock() noexcept
{
    lock();
    return true;
}

void jxy::shared_mutex::unlock() noexcept
{
    FltReleasePushLockEx(&m_PushLock, 0);
}

void jxy::shared_mutex::lock_shared() noexcept
{
    FltAcquirePushLockSharedEx(&m_PushLock, 0);
}

bool jxy::shared_mutex::try_lock_shared() noexcept
{
    lock();
    return true;
}

void jxy::shared_mutex::unlock_shared() noexcept
{
    FltReleasePushLockEx(&m_PushLock, 0);
}

jxy::shared_mutex::native_handle_type jxy::shared_mutex::native_handle() noexcept
{
    return &m_PushLock;
}
