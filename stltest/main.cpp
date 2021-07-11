//
// Copyright (c) Johnny Shaw. All rights reserved.
// 
// File:     stltest/main.cpp
// Author:   Johnny Shaw
// Abstract: STL in the Windows Kernel test driver - main entry point 
//
// This file implements DriverEntry and DriverUnload for the stltest driver.
//
// This driver will run some tests against the jxystl lib, usage of stl, and
// exceptions in the windows kernel
//
#include <fltKernel.h>
#include "tests.hpp"

extern "C"
void DriverUnload(PDRIVER_OBJECT DriverObject)
{
    UNREFERENCED_PARAMETER(DriverObject);
}

extern "C"
NTSTATUS DriverEntry(
    PDRIVER_OBJECT DriverObject,
    PUNICODE_STRING RegistryPath)
{
    UNREFERENCED_PARAMETER(RegistryPath);

    DriverObject->DriverUnload = DriverUnload;

    //
    // jxy::Test::RunTests is meant to be a smoke test against the STL/jxy
    // functionality in the kernel. For now they make some assumptions, such as
    // memory allocations, if a memory allocation fails the tests will fail.
    // That said, it *is* recommended to run the tests with driver verifier
    // enabled (albeit without low resource simulation) - some more effort
    // in the tests would support handling allocation failures and expectations 
    // in such scenarios.
    //

    if (!jxy::Tests::RunTests())
    {
        return STATUS_UNSUCCESSFUL;
    }

    return STATUS_SUCCESS;
}
