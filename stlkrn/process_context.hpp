//
// Copyright (c) Johnny Shaw. All rights reserved.
// 
// File:     stlkrn/process_context.cpp
// Author:   Johnny Shaw
// Abstract: Process Context 
//
// Process contexts are tracked in a global singleton consisting of all 
// running processes (see: ProcessMap). The process context includes a map of
// all threads and modules that are running/loaded for a given process.
//
#pragma once
#include <fltKernel.h>
#include <jxy/string.hpp>
#include "pool_tags.hpp"
#include "thread_map.hpp"
#include "module_map.hpp"

namespace jxy
{

class ProcessContext
{
public:

    using FileNameStringType = jxy::wstring<PagedPool, PoolTags::ProcessFileName>;
    using FilePartStringType = jxy::wstring<PagedPool, PoolTags::ProcessFilePart>;

    ~ProcessContext() noexcept = default;

    ProcessContext(
        uint32_t ProcessId,
        uint32_t SessionId,
        uint32_t ParentProcessId,
        FileNameStringType&& FileName,
        FilePartStringType&& FilePart) noexcept;

    ProcessContext(
        uint32_t ProcessId,
        uint32_t SessionId,
        uint32_t ParentProcessId,
        uint32_t CreatorProcessId,
        uint32_t CreatorThreadId,
        FileNameStringType&& FileName,
        FilePartStringType&& FilePart) noexcept;

    uint32_t GetProcessId() const;
    uint32_t GetSessionId() const;
    uint32_t GetParentProcessId() const;
    const FileNameStringType& GetFileName() const;
    const FilePartStringType& GetFilePart() const;

    uint32_t GetCreatorProcessId() const;
    uint32_t GetCreatorThreadId() const;

    ThreadMap& GetThreads();
    ModuleMap& GetModules();

private:

    const uint32_t m_ProcessId;
    const uint32_t m_SessionId;
    const uint32_t m_ParentProcessId;
    const FileNameStringType m_FileName;
    const FilePartStringType m_FilePart;
    const uint32_t m_CreatorProcessId = 0;
    const uint32_t m_CreatorThreadId = 0;

    ThreadMap m_Threads;
    ModuleMap m_Modules;

};

}
