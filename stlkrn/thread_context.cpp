//
// Copyright (c) Johnny Shaw. All rights reserved.
// 
// File:     stlkrn/thread_context.hpp
// Author:   Johnny Shaw
// Abstract: Thread Context 
//
#include "thread_context.hpp"

jxy::ThreadContext::ThreadContext(
    uint32_t ProcessId,
    uint32_t ThreadId) 
    : m_ProcessId(ProcessId),
      m_ThreadId(ThreadId)
{
}

jxy::ThreadContext::ThreadContext(
    uint32_t ProcessId,
    uint32_t ThreadId,
    uint32_t CreatorProcessId,
    uint32_t CreatorThreadId)
    : m_ProcessId(ProcessId),
      m_ThreadId(ThreadId),
      m_CreatorProcessId(CreatorProcessId),
      m_CreatorThreadId(CreatorThreadId)
{
}


uint32_t jxy::ThreadContext::GetProcessId() const
{
    return m_ProcessId;
}

uint32_t jxy::ThreadContext::GetThreadId() const
{
    return m_ThreadId;
}

uint32_t jxy::ThreadContext::GetCreatorProcessId() const
{
    return m_CreatorProcessId;
}

uint32_t jxy::ThreadContext::GetCreatorThreadId() const
{
    return m_CreatorThreadId;
}

void jxy::ThreadContext::SetExecuteThreadNotifyFired()
{
    m_ExecuteThreadNotifyFired.store(true, std::memory_order_seq_cst);
}

bool jxy::ThreadContext::HasExecuteThreadNotifyFired() const
{
    return m_ExecuteThreadNotifyFired.load(std::memory_order_acquire);
}
