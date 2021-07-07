//
// Copyright (c) Johnny Shaw. All rights reserved.
// 
// File:     jxystl/thread.cpp
// Author:   Johnny Shaw
// Abstract: jxy kernel thread <thread> 
//
#include <jxy/thread.hpp>

jxy::thread::thread(thread&& Other) noexcept
    : m_Thread(std::exchange(Other.m_Thread, nullptr))
{
}

jxy::thread& jxy::thread::operator=(thread&& Other) noexcept
{
    if (joinable())
    {
        std::terminate();
    }

    m_Thread = std::exchange(Other.m_Thread, nullptr);
    return *this;
}

void jxy::thread::swap(thread&& Other) noexcept
{
    std::swap(m_Thread, Other.m_Thread);
}

bool jxy::thread::joinable() const noexcept
{
    return (m_Thread != nullptr);
}

void jxy::thread::join()
{
    if (!joinable())
    {
        std::_Throw_Cpp_error(std::_INVALID_ARGUMENT);
    }

    if (m_Thread == PsGetCurrentThread())
    {
        std::_Throw_Cpp_error(std::_RESOURCE_DEADLOCK_WOULD_OCCUR);
    }

    if (KeWaitForSingleObject(m_Thread, 
                              Executive, 
                              KernelMode, 
                              FALSE,
                              NULL) != STATUS_SUCCESS)
    {
        std::_Throw_Cpp_error(std::_NO_SUCH_PROCESS);
    }
    m_Thread = nullptr;
}

void jxy::thread::detach()
{
    if (!joinable())
    {
        std::_Throw_Cpp_error(std::_INVALID_ARGUMENT);
    }

    m_Thread = nullptr;
}

jxy::thread::id jxy::thread::get_id() const noexcept
{
    if (m_Thread != nullptr)
    {
        return id(PsGetThreadId(m_Thread));
    }

    return id();
}

unsigned int jxy::thread::hardware_concurrency() noexcept
{
    return static_cast<unsigned int>(KeQueryActiveProcessorCount(nullptr));
}

jxy::thread::native_handle_type jxy::thread::native_handle()
{
    return m_Thread;
}

jxy::thread::id jxy::this_thread::get_id() noexcept
{
    return thread::id(PsGetCurrentThreadId());
}

jxy::thread::id::id() noexcept : m_ThreadId(nullptr)
{
}

jxy::thread::id::id(HANDLE ThreadId) noexcept : m_ThreadId(ThreadId)
{
}
