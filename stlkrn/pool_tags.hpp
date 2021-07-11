//
// Copyright (c) Johnny Shaw. All rights reserved.
// 
// File:     stlkrn/pool_tags.hpp
// Author:   Johnny Shaw
// Abstract: Common location for pool tag and pool type definitions. 
//
#pragma once
#include <fltKernel.h>

namespace jxy
{

struct PoolTags
{
    static constexpr ULONG ProcessMap = 'mpXJ';
    static constexpr ULONG ThreadMap = 'mtXJ';
    static constexpr ULONG ModuleMap = 'mmXJ';
    static constexpr ULONG ProcessContext = 'cpXJ';
    static constexpr ULONG ThreadContext = 'ctXJ';
    static constexpr ULONG ModuleContext = 'cmXJ';
    static constexpr ULONG ProcessFileName = 'npXJ';
    static constexpr ULONG ProcessFilePart = 'ppXJ';
    static constexpr ULONG ModuleFileName = 'nmXJ';
    static constexpr ULONG ModuleFilePart = 'pmXJ';
};

struct PoolTypes
{
    static constexpr POOL_TYPE ProcessContext = PagedPool;
    static constexpr POOL_TYPE ThreadContext = PagedPool;
    static constexpr POOL_TYPE ModuleContext = PagedPool;
};

}
