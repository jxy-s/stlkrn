//
// Copyright (c) Johnny Shaw. All rights reserved.
// 
// File:     stlkrn/module_context.hpp
// Author:   Johnny Shaw
// Abstract: Module Context 
//
// These module contexts are tracked in each process context. They are mapped
// by their extents as they are mapped into a given process.
// Then, for a given process context, you may look up a module context.
//
#pragma once
#include <fltKernel.h>
#include <jxy/locks.hpp>
#include <jxy/string.hpp>
#include "pool_tags.hpp"

namespace jxy
{

struct ModuleExtents
{
    uintptr_t Start;
    uintptr_t End;

    bool operator<(const ModuleExtents& Other) const
    {
        return ((Start < Other.Start) && (End < Other.End));
    }
};

struct ModuleProperties
{
    bool SystemModeImage;
    bool MachineTypeMismatch;
    SE_SIGNING_LEVEL ImageSignatureLevel;
    SE_IMAGE_SIGNATURE_TYPE ImageSignatureType;
    bool ImagePartialMap;
};

class ModuleContext
{
public:

    using FileNameStringType = jxy::wstring<PagedPool, PoolTags::ModuleFileName>;
    using FilePartStringType = jxy::wstring<PagedPool, PoolTags::ModuleFilePart>;

    ModuleContext(
        const ModuleExtents& Extents,
        const ModuleProperties& Properties,
        FileNameStringType&& FileName,
        FilePartStringType&& FilePart) noexcept;

    ModuleContext(
        uint32_t MappedByProcessId,
        uint32_t MappedByThreadId,
        const ModuleExtents& Extents,
        const ModuleProperties& Properties,
        FileNameStringType&& FileName,
        FilePartStringType&& FilePart) noexcept;

    const ModuleExtents& GetExtents() const;
    const FileNameStringType& GetFileName() const;
    const FilePartStringType& GetFilePart() const;

    uint32_t GetMappedByProcessId() const;
    uint32_t GetMappedByThreadId() const;

    bool IsSystemModeImage() const;
    bool IsMachineTypeMismatch() const;
    SE_SIGNING_LEVEL GetImageSignatureLevel() const;
    SE_IMAGE_SIGNATURE_TYPE GetImageSignatureType() const;
    bool IsImagePartialMap() const;

    void UpdateOnDuplicateImageLoad(
        uint32_t MappedByProcessId,
        uint32_t MappedByThreadId,
        const ModuleProperties& Properties);

private:

    const ModuleExtents m_Extents;
    const FileNameStringType m_FileName;
    const FilePartStringType m_FilePart;

    mutable jxy::shared_mutex m_Lock;
    ModuleProperties m_Properties;
    uint32_t m_MappedByProcessId = 0;
    uint32_t m_MappedByThreadId = 0;

};

}
