//
// Copyright (c) Johnny Shaw. All rights reserved.
// 
// File:     stlkrn/nthelp.hpp
// Author:   Johnny Shaw
// Abstract: Helpful functionality for bridging NT and modern C++  
//
#pragma once
#include <fltKernel.h>
#include <jxy/string.hpp>
#include <jxy/scope.hpp>

namespace jxy::nt
{

template <typename T> 
T ConvertUnicodeString(PCUNICODE_STRING UnicodeString) noexcept(false)
{
    T res;

    if ((UnicodeString == nullptr) ||
        (UnicodeString->Buffer == nullptr) ||
        (UnicodeString->Length == 0))
    {
        return res;
    }

    auto start = UnicodeString->Buffer;
    auto end = &UnicodeString->Buffer[UnicodeString->Length / sizeof(WCHAR)];

    res.assign(start, end);

    return res;
}

template <typename T>
NTSTATUS GetProcessImageFileName(PEPROCESS Process, T& String) noexcept
{
    PUNICODE_STRING imageFileName;
    auto status = SeLocateProcessImageName(Process, &imageFileName);
    if (!NT_SUCCESS(status))
    {
        return status;
    }

    try
    {
        String = ConvertUnicodeString<T>(imageFileName);
    }
    catch (const std::bad_alloc&)
    {
        ExFreePool(imageFileName);
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    ExFreePool(imageFileName);

    return STATUS_SUCCESS;
}

template <typename T, typename U>
NTSTATUS GetFilePart(const T& FileName, U& FilePart) try
{
    auto pos = FileName.rfind(L'\\');
    if (pos == T::npos)
    {
        FilePart.assign(FileName.begin(), FileName.end());
    }
    else
    {
        FilePart.assign(FileName.begin() + pos + 1, FileName.end());
    }

    return STATUS_SUCCESS;
}
catch (const std::bad_alloc&)
{
    return STATUS_INSUFFICIENT_RESOURCES;
}

namespace details
{


struct close_handle_functor
{
    void operator()(HANDLE Handle)
    {
        ObCloseHandle(Handle, KernelMode);
    }
};

struct object_dereference_functor
{
    void operator()(void* Object)
    {
        if (Object)
        {
            ObDereferenceObject(Object);
        }
    }
};

}

using scoped_handle = scope_resource<HANDLE, details::close_handle_functor>;
using scoped_eprocess = scope_resource<PEPROCESS, details::object_dereference_functor>;

}
