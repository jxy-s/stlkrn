//
// Copyright (c) Johnny Shaw. All rights reserved.
// 
// File:     jxystl/thread.hpp
// Author:   Johnny Shaw
// Abstract: jxy kernel thread <thread> 
//
// jxylib               STL equivalent
// ---------------------------------------------------------------------------
// jxy::thread          std::thread
//
#pragma once
#include <fltKernel.h>
#include <thread>
#include <jxy/memory.hpp>

namespace jxy
{

class thread
{
public:
    class id;
    
    using native_handle_type = PETHREAD;

    //
    // Hard-coded type and tag. This is for the "thread context" which
    // is wrapped into a unique_ptr tuple and passed to the invoked target of
    // the thread object. This is done because PsCreateSystemThread must be
    // called at passive level and KSTART_ROUTINE is invoked at passive too.
    // The caller may still raise the thread routine IRQL, if they access data
    // passed to their thread routine at or above dispatch they must, as always,
    // ensure the parameters handled appropriately. For this object, it doesn't
    // matter - the caller is in control of that.
    //

    static constexpr POOL_TYPE pool_type = PagedPool;
    static constexpr ULONG pool_tag = 'TYXJ';

    ~thread();

    thread() noexcept = default;

    template <typename TFunc, 
              typename... TArgs,
              std::enable_if_t<!std::is_same_v<std::_Remove_cvref_t<TFunc>, thread>, int> = 0>
    [[nodiscard]] 
    explicit 
    _IRQL_requires_max_(PASSIVE_LEVEL)
    thread(TFunc&& Func, TArgs&&... Args)
    {
        NT_ASSERT(KeGetCurrentIrql() == PASSIVE_LEVEL);
        start(std::forward<TFunc>(Func), std::forward<TArgs>(Args)...);
    }

    thread(thread&& Other) noexcept;
    thread& operator=(thread&& Other) noexcept;

    thread(const thread&) = delete;
    thread& operator=(const thread&) = delete;

    void swap(thread&& Other) noexcept;

    [[nodiscard]]
    bool joinable() const noexcept;

    void join();

    void detach();

    [[nodiscard]]
    id get_id() const noexcept;

    [[nodiscard]]
    static
    unsigned int hardware_concurrency() noexcept;

    [[nodiscard]]
    native_handle_type native_handle();

private:

    template <typename TTuple, size_t... t_Indices>
    static 
    _IRQL_requires_max_(PASSIVE_LEVEL)
    void __stdcall invoke(void* StartContext) noexcept
    {
        //
        // We should be at passive per contract with PsCreateSystemThread
        //
        NT_ASSERT(KeGetCurrentIrql() == PASSIVE_LEVEL);

        const jxy::unique_ptr<TTuple, pool_type, pool_tag> funcVals(static_cast<TTuple*>(StartContext));
        TTuple& tuple = *funcVals;
        std::invoke(std::move(std::get<t_Indices>(tuple))...);
        
        //
        // Check that no matter what the invoked routine does it returns
        // to passive.
        //
        NT_ASSERT(KeGetCurrentIrql() == PASSIVE_LEVEL);
        PsTerminateSystemThread(STATUS_SUCCESS);
    }

    template <typename TTuple, size_t... t_Indices>
    [[nodiscard]]
    static
    constexpr
    auto get_invoke(std::index_sequence<t_Indices...>) noexcept
    {
        return &invoke<TTuple, t_Indices...>;
    }

    template <typename TFunc, typename... TArgs>
    _IRQL_requires_max_(PASSIVE_LEVEL)
    void start(TFunc&& Func, TArgs&&... Args)
    {
        NT_ASSERT(KeGetCurrentIrql() == PASSIVE_LEVEL);
        NT_ASSERT(m_Thread == nullptr);

        using TTuple = std::tuple<std::decay_t<TFunc>, std::decay_t<TArgs>...>;
        auto params = jxy::make_unique<TTuple, pool_type, pool_tag>(std::forward<TFunc>(Func), std::forward<TArgs>(Args)...);
        constexpr auto routine = get_invoke<TTuple>(std::make_index_sequence<1 + sizeof...(TArgs)>{});

        HANDLE threadHandle = nullptr;
        if (!NT_SUCCESS(PsCreateSystemThread(&threadHandle,
                                            THREAD_ALL_ACCESS,
                                            nullptr,
                                            nullptr,
                                            nullptr,
                                            routine,
                                            params.get())))
        {
            std::_Throw_Cpp_error(std::_RESOURCE_UNAVAILABLE_TRY_AGAIN);
        }
        else
        {
            NT_VERIFY(NT_SUCCESS(
                ObReferenceObjectByHandle(threadHandle,
                                          THREAD_ALL_ACCESS,
                                          *PsThreadType,
                                          KernelMode,
                                          reinterpret_cast<void**>(&m_Thread),
                                          nullptr)));
            NT_VERIFY(NT_SUCCESS(ObCloseHandle(threadHandle, KernelMode)));

            //
            // Release the memory to the invoke routine.
            //
            params.release();
        }
    }

    PETHREAD m_Thread = nullptr;

};

namespace this_thread
{

[[nodiscard]]
thread::id get_id() noexcept;

inline void yield() noexcept
{
    YieldProcessor();
}

// TODO this_thread::sleep... funcs

}

class thread::id
{
public:

    id() noexcept;

private:

    id(HANDLE ThreadId) noexcept;

    HANDLE m_ThreadId;

    friend thread::id thread::get_id() const noexcept;
    friend thread::id this_thread::get_id() noexcept;
    friend bool operator==(thread::id Left, thread::id Right) noexcept;
    friend bool operator<(thread::id Left, thread::id Right) noexcept;

};

[[nodiscard]] inline bool operator==(thread::id Left, thread::id Right) noexcept 
{
    return Left.m_ThreadId == Right.m_ThreadId;
}

[[nodiscard]] inline bool operator!=(thread::id Left, thread::id Right) noexcept 
{
    return !(Left == Right);
}

[[nodiscard]] inline bool operator<(thread::id Left, thread::id Right) noexcept 
{
    return Left.m_ThreadId < Right.m_ThreadId;
}

[[nodiscard]] inline bool operator<=(thread::id Left, thread::id Right) noexcept 
{
    return !(Right < Left);
}

[[nodiscard]] inline bool operator>(thread::id Left, thread::id Right) noexcept 
{
    return Right < Left;
}

[[nodiscard]] inline bool operator>=(thread::id Left, thread::id Right) noexcept 
{
    return !(Left < Right);
}

}
