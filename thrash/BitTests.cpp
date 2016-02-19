// BitTests.cpp - tests for BinaryFormatSerializer
//

#include "serializers/BitSerializer.h"
#include "serializers/VectorSaveSerializer.h"

#include "endian_formatter.h"
#include "const_formatter.h"
#include "size_prefix_formatter.h"

#include "gtest/gtest.h"

namespace {

using namespace binary_format;
    
TEST(BitSerializerWorks, Loading)
{
    {
        std::vector<uint8_t> data { 0x21, 0x43, 0x65, 0x87 };
        // 0    4     8    12    16   20    24   28    32
        // 1000 0100  1100 0010  1010 0110  1110 0001
        ZeroCopyVectorLoadSerializer vectorReader(data);
        BitLoader<ZeroCopyVectorLoadSerializer> bitLoader(vectorReader);

        uint64_t bits0 = bitLoader.loadBits(1);
        EXPECT_EQ(bits0, 1);

        uint64_t bits1_6 = bitLoader.loadBits(6);
        EXPECT_EQ(bits1_6, 16);

        uint64_t bits7_10 = bitLoader.loadBits(4);
        EXPECT_EQ(bits7_10, 6);

        uint64_t bits11_18 = bitLoader.loadBits(8);
        EXPECT_EQ(bits11_18, 8 + 32 + 128);

        uint64_t bits19_31 = bitLoader.loadBits(13);
        EXPECT_EQ(bits19_31, 4 + 8 + 32 + 64 + 128 + 4096);

        ASSERT_THROW(bitLoader.loadBits(1), end_of_input);
    }
}

}  // namespace
