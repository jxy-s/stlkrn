//
// Copyright (c) Johnny Shaw. All rights reserved.
// 
// File:     stlkrn/process_context.cpp
// Author:   Johnny Shaw
// Abstract: Process Context 
//
#include "process_context.hpp"

jxy::ProcessContext::ProcessContext(
    uint32_t ProcessId,
    uint32_t SessionId,
    uint32_t ParentProcessId,
    FileNameStringType&& FileName,
    FilePartStringType&& FilePart) noexcept
    : m_ProcessId(ProcessId),
      m_SessionId(SessionId),
      m_ParentProcessId(ParentProcessId),
      m_FileName(std::move(FileName)),
      m_FilePart(std::move(FilePart))
{
}

jxy::ProcessContext::ProcessContext(
    uint32_t ProcessId,
    uint32_t SessionId,
    uint32_t ParentProcessId,
    uint32_t CreatorProcessId,
    uint32_t CreatorThreadId,
    FileNameStringType&& FileName,
    FilePartStringType&& FilePart) noexcept
    : m_ProcessId(ProcessId),
      m_SessionId(SessionId),
      m_ParentProcessId(ParentProcessId),
      m_CreatorProcessId(CreatorProcessId),
      m_CreatorThreadId(CreatorThreadId),
      m_FileName(std::move(FileName)),
      m_FilePart(std::move(FilePart))
{

}

uint32_t jxy::ProcessContext::GetProcessId() const
{
    return m_ProcessId;
}

uint32_t jxy::ProcessContext::GetSessionId() const
{
    return m_SessionId;
}

uint32_t jxy::ProcessContext::GetParentProcessId() const
{
    return m_ParentProcessId;
}

const jxy::ProcessContext::FileNameStringType& 
jxy::ProcessContext::GetFileName() const
{
    return m_FileName;
}

const jxy::ProcessContext::FilePartStringType& 
jxy::ProcessContext::GetFilePart() const
{
    return m_FilePart;
}

uint32_t jxy::ProcessContext::GetCreatorProcessId() const
{
    return m_CreatorProcessId;
}

uint32_t jxy::ProcessContext::GetCreatorThreadId() const
{
    return m_CreatorThreadId;
}

jxy::ThreadMap& jxy::ProcessContext::GetThreads()
{
    return m_Threads;
}

jxy::ModuleMap& jxy::ProcessContext::GetModules()
{
    return m_Modules;
}
