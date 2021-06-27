//
// Copyright (c) Johnny Shaw. All rights reserved.
// 
// File:     stlkrn/module_context.cpp
// Author:   Johnny Shaw
// Abstract: Module Context 
//
#include "module_context.hpp"

jxy::ModuleContext::ModuleContext(
    const ModuleExtents& Extents,
    const ModuleProperties& Properties,
    FileNameStringType&& FileName,
    FilePartStringType&& FilePart) noexcept
    : m_Extents(Extents),
      m_Properties(Properties),
      m_FileName(std::move(FileName)),
      m_FilePart(std::move(FilePart))
{
}

jxy::ModuleContext::ModuleContext(
    uint32_t MappedByProcessId,
    uint32_t MappedByThreadId,
    const ModuleExtents& Extents,
    const ModuleProperties& Properties,
    FileNameStringType&& FileName,
    FilePartStringType&& FilePart) noexcept
    : m_MappedByProcessId(MappedByProcessId),
      m_MappedByThreadId(MappedByThreadId),
      m_Extents(Extents),
      m_Properties(Properties),
      m_FileName(std::move(FileName)),
      m_FilePart(std::move(FilePart))
{
}

const jxy::ModuleExtents& jxy::ModuleContext::GetExtents() const
{
    return m_Extents;
}

const jxy::ModuleContext::FileNameStringType& jxy::ModuleContext::GetFileName() const
{
    return m_FileName;
}

const jxy::ModuleContext::FilePartStringType& jxy::ModuleContext::GetFilePart() const
{
    return m_FilePart;
}

uint32_t jxy::ModuleContext::GetMappedByProcessId() const
{
    jxy::shared_lock<jxy::shared_mutex> lock(m_Lock);
    return m_MappedByProcessId;
}

uint32_t jxy::ModuleContext::GetMappedByThreadId() const
{
    jxy::shared_lock<jxy::shared_mutex> lock(m_Lock);
    return m_MappedByThreadId;
}

bool jxy::ModuleContext::IsSystemModeImage() const
{
    jxy::shared_lock<jxy::shared_mutex> lock(m_Lock);
    return m_Properties.SystemModeImage;
}

bool jxy::ModuleContext::IsMachineTypeMismatch() const
{
    jxy::shared_lock<jxy::shared_mutex> lock(m_Lock);
    return m_Properties.MachineTypeMismatch;
}

SE_SIGNING_LEVEL jxy::ModuleContext::GetImageSignatureLevel() const
{
    jxy::shared_lock<jxy::shared_mutex> lock(m_Lock);
    return m_Properties.ImageSignatureLevel;
}

SE_IMAGE_SIGNATURE_TYPE jxy::ModuleContext::GetImageSignatureType() const
{
    jxy::shared_lock<jxy::shared_mutex> lock(m_Lock);
    return m_Properties.ImageSignatureType;
}

bool jxy::ModuleContext::IsImagePartialMap() const
{
    jxy::shared_lock<jxy::shared_mutex> lock(m_Lock);
    return m_Properties.ImagePartialMap;
}

void jxy::ModuleContext::UpdateOnDuplicateImageLoad(
    uint32_t MappedByProcessId,
    uint32_t MappedByThreadId,
    const ModuleProperties& Properties)
{
    jxy::unique_lock<jxy::shared_mutex> lock(m_Lock);
    m_MappedByProcessId = MappedByProcessId;
    m_MappedByThreadId = MappedByThreadId;
    m_Properties = Properties;
}

