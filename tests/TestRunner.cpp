// TestRunner.cpp - tests for BinaryFormatSerializer
//

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#include "gtest/gtest.h"

//#include "vld.h"

namespace {

// The fixture for testing class Foo.
class FooTest : public ::testing::Test {
protected:
    FooTest() {
        // You can do set-up work for each test here.
    }

    virtual ~FooTest() {
        // You can do clean-up work that doesn't throw exceptions here.
    }

    // Objects declared here can be used by all tests in the test case for Foo.
};

TEST_F(FooTest, BigEndianWorks) {
    EXPECT_EQ(0, 0);
}

}  // namespace

int main(int argc, char **argv) {
    _CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
    ::testing::InitGoogleTest(&argc, argv);
    auto leak = new int[10];
    auto result = RUN_ALL_TESTS();
    //_CrtDumpMemoryLeaks();
    return result;
}

