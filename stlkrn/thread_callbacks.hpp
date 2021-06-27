//
// Copyright (c) Johnny Shaw. All rights reserved.
// 
// File:     stlkrn/thread_callbacks.hpp
// Author:   Johnny Shaw
// Abstract: Thread Notify Callbacks 
//
#pragma once
#include <fltKernel.h>

namespace jxy::nt
{

NTSTATUS RegisterThreadCallback();
void UnregisterThreadCallback();

}