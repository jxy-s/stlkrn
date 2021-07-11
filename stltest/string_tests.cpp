//
// Copyright (c) Johnny Shaw. All rights reserved.
// 
// File:     stltests/string_tests.cpp
// Author:   Johnny Shaw
//
#include "tests_common.hpp"
#include <jxy/string.hpp>

namespace jxy::Tests
{

void StringTests()
{
    {
        jxy::string<PagedPool, '0GAT'> str("Hello");
        UT_ASSERT(str.get_allocator().pool_tag == '0GAT');
        UT_ASSERT(str.get_allocator().pool_type == PagedPool);
        UT_ASSERT(str == "Hello");
        UT_ASSERT(strcmp(str.c_str(), "Hello") == 0);
        UT_ASSERT(str.length() == strlen("Hello"));
        str.append(", World!");
        UT_ASSERT(str == "Hello, World!");
        str.assign("Hello Again");
        UT_ASSERT(str == "Hello Again");
        UT_ASSERT(str.at(0) == 'H');
        UT_ASSERT(str[1] == 'e');
        UT_ASSERT(str.compare("Hello Again") == 0);
        UT_ASSERT(str.empty() == false);
        str.erase(str.begin(), str.end());
        UT_ASSERT(str.empty() == true);
        UT_ASSERT(str.length() == 0);
        str.assign("yep");
        UT_ASSERT(str.find("yep") == 0);
        UT_ASSERT(str.rfind("yep") == 0);
        str.pop_back();
        UT_ASSERT(str.back() == 'e');
        UT_ASSERT(str.front() == 'y');
    }
    {
        jxy::wstring<PagedPool, '0GAT'> str(L"Hello");
        UT_ASSERT(str.get_allocator().pool_tag == '0GAT');
        UT_ASSERT(str.get_allocator().pool_type == PagedPool);
        UT_ASSERT(str == L"Hello");
        UT_ASSERT(wcscmp(str.c_str(), L"Hello") == 0);
        UT_ASSERT(str.length() == wcslen(L"Hello"));
        str.append(L", World!");
        UT_ASSERT(str == L"Hello, World!");
        str.assign(L"Hello Again");
        UT_ASSERT(str == L"Hello Again");
        UT_ASSERT(str.at(0) == L'H');
        UT_ASSERT(str[1] == L'e');
        UT_ASSERT(str.compare(L"Hello Again") == 0);
        UT_ASSERT(str.empty() == false);
        str.erase(str.begin(), str.end());
        UT_ASSERT(str.empty() == true);
        UT_ASSERT(str.length() == 0);
        str.assign(L"yep");
        UT_ASSERT(str.find(L"yep") == 0);
        UT_ASSERT(str.rfind(L"yep") == 0);
        str.pop_back();
        UT_ASSERT(str.back() == L'e');
        UT_ASSERT(str.front() == L'y');
    }
}

}
