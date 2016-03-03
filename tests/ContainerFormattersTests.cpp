// VariousTests.cpp - tests for BinaryFormatSerializer
//

#include "serialize.h"
#include "binary_serializers/VectorSaveSerializer.h"
#include "binary_serializers/MemorySerializer.h"

#include "binary_formatters/endian_formatter.h"
#include "formatters/const_formatter.h"
#include "binary_formatters/string_formatter.h"
#include "formatters/vector_formatter.h"
#include "formatters/map_formatter.h"
#include "formatters/array_formatter.h"

#include "gtest/gtest.h"

namespace {

using namespace arbitrary_format;
using namespace binary;
    
TEST(StringFormatterWorks, SavingAndLoading)
{
    {
        VectorSaveSerializer vectorWriter;
        save< string_formatter< little_endian<4> > >(vectorWriter, "ala");
        const auto value = std::vector<uint8_t> { 0x03, 0x00, 0x00, 0x00, 'a', 'l', 'a' };
        EXPECT_EQ(vectorWriter.getData(), value);
    }

    {
        VectorSaveSerializer vectorWriter;
        save< string_formatter< little_endian<4> > >(vectorWriter, "");
        const auto value = std::vector<uint8_t> { 0x00, 0x00, 0x00, 0x00 };
        EXPECT_EQ(vectorWriter.getData(), value);
    }

    {
        VectorSaveSerializer vectorWriter;
        ASSERT_THROW( save< string_formatter< little_endian<1> > >(vectorWriter,
                                                                    "0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789"
                                                                    "0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789"
                                                                    "01234567890123456789012345678901234567890123456789"
                                                                    "0123456789"), lossy_conversion );
    }

    {
        std::vector<uint8_t> data { 0x0A, 0x00, '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' };
        MemoryLoadSerializer vectorReader(data);
        load< const_formatter< string_formatter< little_endian<2> > > >(vectorReader, std::string("0123456789"));
    }

    {
        std::vector<uint8_t> data { 0x00 };
        MemoryLoadSerializer vectorReader(data);
        load< const_formatter< string_formatter< little_endian<1> > > >(vectorReader, std::string(""));
    }
}

TEST(VectorFormatterWorks, SavingAndLoading)
{
    {
        VectorSaveSerializer vectorWriter;
        const auto value = std::vector<uint8_t> { 0x01, 0x02, 0x03, 0x04 };
        save< vector_formatter< little_endian<2>, big_endian<2> > >(vectorWriter, value);
        const auto checkValue = std::vector<uint8_t> { 0x04, 0x00, 0x00, 0x01, 0x00, 0x02, 0x00, 0x03, 0x00, 0x04 };
        EXPECT_EQ(vectorWriter.getData(), checkValue);
    }

    {
        VectorSaveSerializer vectorWriter;
        save< vector_formatter< little_endian<4>, big_endian<2> > >(vectorWriter, std::vector<uint8_t>());
        const auto checkValue = std::vector<uint8_t> { 0x00, 0x00, 0x00, 0x00 };
        EXPECT_EQ(vectorWriter.getData(), checkValue);
    }

    {
        VectorSaveSerializer vectorWriter;
        const auto value = std::vector<int>(256, 5);
        using vectorFormat = vector_formatter< little_endian<1>, little_endian<1> >;
        ASSERT_THROW( save<vectorFormat>(vectorWriter, value), lossy_conversion );
    }

    {
        VectorSaveSerializer vectorWriter;
        const auto value = std::vector<int>(5, 256);
        using vectorFormat = vector_formatter< little_endian<1>, little_endian<1> >;
        ASSERT_THROW( save<vectorFormat>(vectorWriter, value), lossy_conversion );
    }

    {
        std::vector<uint8_t> data { 0x0A, 0x00, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
        MemoryLoadSerializer vectorReader(data);
        const auto checkValue = std::vector<uint8_t> { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
        load< const_formatter< vector_formatter< little_endian<2>, little_endian<1> > > >(vectorReader, checkValue);
    }

    {
        std::vector<uint8_t> data { 0x00 };
        MemoryLoadSerializer vectorReader(data);
        const auto checkValue = std::vector<uint8_t> {};
        load< const_formatter< vector_formatter< little_endian<1>, big_endian<8> > > >(vectorReader, checkValue);
    }
}

TEST(MapFormatterWorks, SavingAndLoading)
{
    {
        VectorSaveSerializer vectorWriter;
        const auto value = std::map<std::string, uint8_t> { {"ala", 5}, {"ola", 15}, {"ela", 10}  };
        save< map_formatter< big_endian<2>, string_formatter< little_endian<1> >, big_endian<2> > >(vectorWriter, value);
        const auto checkValue = std::vector<uint8_t> { 0x00, 0x03, 0x03, 'a', 'l', 'a', 0x00, 5, 0x03, 'e', 'l', 'a', 0x00, 10, 0x03, 'o', 'l', 'a', 0x00, 15 };
        EXPECT_EQ(vectorWriter.getData(), checkValue);
    }

    {
        VectorSaveSerializer vectorWriter;
        save< map_formatter< big_endian<2>, string_formatter< little_endian<1> >, big_endian<2> > >(vectorWriter, std::map<std::string, uint8_t>());
        const auto checkValue = std::vector<uint8_t> { 0x00, 0x00 };
        EXPECT_EQ(vectorWriter.getData(), checkValue);
    }

    {
        std::vector<uint8_t> data { 0x0A, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 0 };
        MemoryLoadSerializer vectorReader(data);
        const auto checkValue = std::map<int, int> { {0, 1}, {1, 2}, {2, 3}, {3, 4}, {4, 5}, {5, 6}, {6, 7}, {7, 8}, {8, 9}, {9, 0} };
        load< const_formatter< map_formatter< big_endian<1>, little_endian<1>, big_endian<1> > > >(vectorReader, checkValue);
    }

    {
        std::vector<uint8_t> data { 0x00, 0x00, 0x00, 0x00 };
        MemoryLoadSerializer vectorReader(data);
        const auto checkValue = std::map<int, std::string> {};
        load< const_formatter< map_formatter< big_endian<4>, big_endian<2>, string_formatter< little_endian<1> > > > >(vectorReader, checkValue);
    }
}

TEST(FixedSizeArrayFormatterWorks, SavingAndLoading)
{
    {
        VectorSaveSerializer vectorWriter;
        uint8_t value[4] = { 0x01, 0x02, 0x03, 0x04 };
        save< array_formatter< little_endian<2>, 4 > >(vectorWriter, value);
        const auto checkValue = std::vector<uint8_t> { 0x01, 0x00, 0x02, 0x00, 0x03, 0x00, 0x04, 0x00 };
        EXPECT_EQ(vectorWriter.getData(), checkValue);
    }

    {
        VectorSaveSerializer vectorWriter;
        uint8_t value[4] = { 0x01, 0x02, 0x03, 0x04 };
        save< array_formatter< little_endian<2> > >(vectorWriter, value);
        const auto checkValue = std::vector<uint8_t> { 0x01, 0x00, 0x02, 0x00, 0x03, 0x00, 0x04, 0x00 };
        EXPECT_EQ(vectorWriter.getData(), checkValue);
    }

    {
        VectorSaveSerializer vectorWriter;
        uint8_t value[4] = { 0x01, 0x02, 0x03, 0x04 };
        save< array_formatter< little_endian<2>, 0 > >(vectorWriter, value);
        const auto checkValue = std::vector<uint8_t> {};
        EXPECT_EQ(vectorWriter.getData(), checkValue);
    }

    {
        std::vector<uint8_t> data { 0x01, 0x00, 0x02, 0x00, 0x03, 0x00, 0x04, 0x00 };
        MemoryLoadSerializer vectorReader(data);
        uint8_t checkValue[4] = { 0x01, 0x02, 0x03, 0x04 };
        load< const_formatter< array_formatter< little_endian<2>, 4 > > >(vectorReader, checkValue);
    }

    {
        std::vector<uint8_t> data { 0x01, 0x02, 0x03, 0x04 };
        MemoryLoadSerializer vectorReader(data);
        uint8_t checkValue[4] = { 0x01, 0x02, 0x03, 0x04 };
        load< const_formatter< array_formatter< little_endian<1> > > >(vectorReader, checkValue);
    }

    {
        std::vector<uint8_t> data {};
        MemoryLoadSerializer vectorReader(data);
        uint8_t checkValue[4] = {1, 2, 3, 4};
        using arrayFormat = const_formatter< array_formatter< little_endian<2>, 0 > >;
        ASSERT_THROW(load<arrayFormat>(vectorReader, checkValue), invalid_data);
    }
}

TEST(FixedSizeArrayFormatterWorks, Constness)
{
    {
        VectorSaveSerializer vectorWriter;
        const uint8_t value[4] = { 0x01, 0x02, 0x03, 0x04 };
        save< array_formatter< little_endian<2>, 4 > >(vectorWriter, value);
        const auto checkValue = std::vector<uint8_t> { 0x01, 0x00, 0x02, 0x00, 0x03, 0x00, 0x04, 0x00 };
        EXPECT_EQ(vectorWriter.getData(), checkValue);
    }

    {
        VectorSaveSerializer vectorWriter;
        const uint8_t value[4] = { 0x01, 0x02, 0x03, 0x04 };
        const uint8_t * const cvalue = value;
        save< array_formatter< little_endian<2>, 4 > >(vectorWriter, cvalue);
        const auto checkValue = std::vector<uint8_t> { 0x01, 0x00, 0x02, 0x00, 0x03, 0x00, 0x04, 0x00 };
        EXPECT_EQ(vectorWriter.getData(), checkValue);
    }

    {
        std::vector<uint8_t> data { 0x01, 0x02, 0x03, 0x04 };
        MemoryLoadSerializer vectorReader(data);
        uint8_t checkValue[4] = { 0x01, 0x02, 0x03, 0x04 };
        load< array_formatter< little_endian<1> > >(vectorReader, checkValue);
    }
}

}  // namespace
