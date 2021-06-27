//
// Copyright (c) Johnny Shaw. All rights reserved.
// 
// File:     jxystl/locks.cpp
// Author:   Johnny Shaw
// Abstract: STL wrappers for locks (<mutex>, <shared_mutex>, etc.)
//
// I chose to have one "locks.hpp" file rather than mutex, shared_mutex, etc. 
// These could be moved to their relevant files to have parity with the standard.
// But I personally find no reason to make that distinction.
//
// jxylib               STL equivalent       Primitive for NT Kernel 
// ---------------------------------------------------------------------------
// jxy::shared_mutex    std::shared_mutex    EX_PUSH_LOCK
// jxy::mutex           std::mutex           KGUARDED_MUTEX
// jxy::shared_lock     std::shared_lock     n/a
// jxy::unique_lock     std::unique_lock     n/a
//
#pragma once
#include <fltKernel.h>
#include <shared_mutex>

namespace jxy
{

class shared_mutex
{
public:

    shared_mutex() noexcept;
    ~shared_mutex() noexcept;
    void lock() noexcept;
    bool try_lock() noexcept;
    void unlock() noexcept;
    void lock_shared() noexcept;
    bool try_lock_shared() noexcept;
    void unlock_shared() noexcept;

private:

    EX_PUSH_LOCK m_PushLock;

};

template <ULONG t_PoolTag>
class mutex
{
public:

    mutex() noexcept(false)
    {
        m_GuardedMutex = ExAllocatePoolWithTag(NonPagedPoolNx,
                                               sizeof(*m_GuardedMutex),
                                               t_PoolTag);
        if (!m_GuardedMutex)
        {
            throw std::bad_alloc();
        }

        KeInitializeGuardedMutex(m_GuardedMutex);
    }

    ~mutex() noexcept
    {
        if (m_GuardedMutex)
        {
            ExFreePoolWithTag(m_GuardedMutex, t_PoolTag);
        }
    }

    void lock() noexcept
    {
        KeAcquireGuardedMutex(m_GuardedMutex);
    }

    bool try_lock() noexcept
    {
        return (KeTryToAcquireGuardedMutex(m_GuardedMutex) != FALSE ? true : false);
    }

    void unlock() noexcept
    {
        KeReleaseGuardedMutex(m_GuardedMutex);
    }

private:

    PKGUARDED_MUTEX m_GuardedMutex = nullptr;

};

template <typename T>
using shared_lock = std::shared_lock<T>;

template <typename T>
using unique_lock = std::unique_lock<T>;

}
