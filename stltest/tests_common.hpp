//
// Copyright (c) Johnny Shaw. All rights reserved.
// 
// File:     stltests/tests_common.cpp
// Author:   Johnny Shaw
//
#pragma once
#include <fltKernel.h>

//
// int3 in both release and debug - might have this return from the test
// in the future similar to gtest
//
#define UT_ASSERT(_X_) \
{ \
__pragma (warning(push)) \
__pragma (warning(disable: 4127)) \
if ((_X_) == false) { __debugbreak(); } \
__pragma (warning(pop)) \
}
