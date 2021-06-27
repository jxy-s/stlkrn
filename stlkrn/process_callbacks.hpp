//
// Copyright (c) Johnny Shaw. All rights reserved.
// 
// File:     stlkrn/process_callbacks.hpp
// Author:   Johnny Shaw
// Abstract: Process Notify Callbacks 
//
#pragma once
#include <fltKernel.h>

namespace jxy::nt
{

NTSTATUS RegisterProcessCallback();
void UnregisterProcessCallback();

}
