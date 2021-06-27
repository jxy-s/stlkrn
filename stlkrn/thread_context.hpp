//
// Copyright (c) Johnny Shaw. All rights reserved.
// 
// File:     stlkrn/thread_context.hpp
// Author:   Johnny Shaw
// Abstract: Thread Context 
//
// Process contexts are tracked in each process context. They are also tracked
// in a global thread map singleton (see: ThreadMap). Thread contexts may be
// looked up using the global table or through a given process context.
//
#pragma once
#include <fltKernel.h>
#include <atomic>

namespace jxy
{

class ThreadContext
{
public:

    ThreadContext(uint32_t ProcessId, 
                  uint32_t ThreadId);

    ThreadContext(uint32_t ProcessId,
                  uint32_t ThreadId,
                  uint32_t CreatorProcessId,
                  uint32_t CreatorThreadId);

    uint32_t GetProcessId() const;
    uint32_t GetThreadId() const;

    uint32_t GetCreatorProcessId() const;
    uint32_t GetCreatorThreadId() const;

    void SetExecuteThreadNotifyFired();
    bool HasExecuteThreadNotifyFired() const;

private:

    const uint32_t m_ProcessId;
    const uint32_t m_ThreadId;
    const uint32_t m_CreatorProcessId = 0;
    const uint32_t m_CreatorThreadId = 0;

    std::atomic<bool> m_ExecuteThreadNotifyFired = false;

};

}
