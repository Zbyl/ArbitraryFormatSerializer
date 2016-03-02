// TestRunner.cpp - tests for BinaryFormatSerializer
//

//#define _CRTDBG_MAP_ALLOC
//#include <stdlib.h>
//#include <crtdbg.h>

#include "gtest/gtest.h"

//#include "vld.h"

int main(int argc, char **argv) {
    //_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
    ::testing::InitGoogleTest(&argc, argv);
    //auto leak = new int[10];
    auto result = RUN_ALL_TESTS();
    //_CrtDumpMemoryLeaks();
    return result;
}

