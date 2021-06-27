//
// Copyright (c) Johnny Shaw. All rights reserved.
// 
// File:     stlkrn/module_callbacks.hpp
// Author:   Johnny Shaw
// Abstract: Image Load Callbacks
//
#pragma once
#include <fltKernel.h>

namespace jxy::nt
{

NTSTATUS RegisterLoadImageCallback();
void UnregisterLoadImageCallback();

}
