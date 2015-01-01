// Tests.cpp : Defines the entry point for the console application.
//

#include "precompiled_headers.h"

#include "gtest/gtest.h"

int _tmain(int argc, _TCHAR* argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

