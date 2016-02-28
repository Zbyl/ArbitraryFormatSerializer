// EndianFormatterTests.cpp - tests for BinaryFormatSerializer
//

#include "binary_serializers/VectorSaveSerializer.h"
#include "binary_serializers/MemorySerializer.h"

#include "binary_formatters/endian_formatter.h"
#include "formatters/const_formatter.h"

#include "gtest/gtest.h"

#include <string>
#include <vector>
#include <cstdint>
#include <limits>

namespace {

using namespace arbitrary_format;
using namespace binary;

TEST(EndianFormattersWork, AreVerbatim)
{
    if (boost::endian::order::native == boost::endian::order::little)
    {
        ASSERT_TRUE((is_verbatim_formatter< little_endian<4>, uint32_t >::value));
        ASSERT_FALSE((is_verbatim_formatter< big_endian<4>, uint32_t >::value));
    }
    else
    {
        ASSERT_FALSE((is_verbatim_formatter< little_endian<4>, uint32_t >::value));
        ASSERT_TRUE((is_verbatim_formatter< big_endian<4>, uint32_t >::value));
    }
}

TEST(EndianFormattersWork, Loading)
{
    std::vector<uint8_t> data { 0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE, 0xF0 };

    {
        MemoryLoadSerializer vectorReader(data);
        uint32_t value;
        load< little_endian<1> >(vectorReader, value);
        EXPECT_EQ(value, 0x12);
        load< little_endian<2> >(vectorReader, value);
        EXPECT_EQ(value, 0x5634);
        load< little_endian<4> >(vectorReader, value);
        EXPECT_EQ(value, 0xDEBC9A78);
    }
    {
        MemoryLoadSerializer vectorReader(data);
        uint64_t value;
        load< little_endian<8> >(vectorReader, value);
        EXPECT_EQ(value, 0xF0DEBC9A78563412);
    }

    {
        MemoryLoadSerializer vectorReader(data);
        uint32_t value;
        load< big_endian<1> >(vectorReader, value);
        EXPECT_EQ(value, 0x12);
        load< big_endian<2> >(vectorReader, value);
        EXPECT_EQ(value, 0x3456);
        load< big_endian<4> >(vectorReader, value);
        EXPECT_EQ(value, 0x789ABCDE);
    }
    {
        MemoryLoadSerializer vectorReader(data);
        uint64_t value;
        load< big_endian<8> >(vectorReader, value);
        EXPECT_EQ(value, 0x123456789ABCDEF0);
    }
}

TEST(EndianFormattersWork, Saving)
{
    {
        VectorSaveSerializer vectorWriter;
        save< little_endian<1> >(vectorWriter, 0x12);
        save< little_endian<2> >(vectorWriter, 0x5634);
        save< little_endian<4> >(vectorWriter, 0xDEBC9A78);
        auto value = std::vector<uint8_t> { 0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE };
        EXPECT_EQ(vectorWriter.getData(), value);
    }
    {
        VectorSaveSerializer vectorWriter;
        save< little_endian<8> >(vectorWriter, 0xF0DEBC9A78563412);
        auto value = std::vector<uint8_t> { 0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE, 0xF0 };
        EXPECT_EQ(vectorWriter.getData(), value);
    }

    {
        VectorSaveSerializer vectorWriter;
        save< big_endian<1> >(vectorWriter, 0x12);
        save< big_endian<2> >(vectorWriter, 0x3456);
        save< big_endian<4> >(vectorWriter, 0x789ABCDE);
        auto value = std::vector<uint8_t> { 0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE };
        EXPECT_EQ(vectorWriter.getData(), value);
    }
    {
        VectorSaveSerializer vectorWriter;
        save< big_endian<8> >(vectorWriter, 0x123456789ABCDEF0);
        auto value = std::vector<uint8_t> { 0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE, 0xF0 };
        EXPECT_EQ(vectorWriter.getData(), value);
    }
}

TEST(EndianFormattersWork, SaveOverflows)
{
    VectorSaveSerializer vectorWriter;
    EXPECT_NO_THROW( save< little_endian<1> >(vectorWriter, 0x7F));
    EXPECT_NO_THROW( save< little_endian<2> >(vectorWriter, 0x7FFF));
    EXPECT_NO_THROW( save< little_endian<4> >(vectorWriter, 0x7FFFFFFF));
    ASSERT_THROW( save< little_endian<1> >(vectorWriter, 0xFF), lossy_conversion);
    ASSERT_THROW( save< little_endian<2> >(vectorWriter, 0xFFFF), lossy_conversion);
    EXPECT_NO_THROW( save< little_endian<4> >(vectorWriter, 0xFFFFFFFF));

    EXPECT_NO_THROW( save< big_endian<1> >(vectorWriter, 0x7F));
    EXPECT_NO_THROW( save< big_endian<2> >(vectorWriter, 0x7FFF));
    EXPECT_NO_THROW( save< big_endian<4> >(vectorWriter, 0x7FFFFFFF));
    ASSERT_THROW( save< big_endian<1> >(vectorWriter, 0xFF), lossy_conversion);
    ASSERT_THROW( save< big_endian<2> >(vectorWriter, 0xFFFF), lossy_conversion);
    EXPECT_NO_THROW( save< big_endian<4> >(vectorWriter, 0xFFFFFFFF));

    ASSERT_THROW( save< little_endian<1> >(vectorWriter, 0x123), lossy_conversion);
    ASSERT_THROW( save< little_endian<2> >(vectorWriter, 0x12345), lossy_conversion);
    ASSERT_THROW( save< little_endian<4> >(vectorWriter, 0x123456789), lossy_conversion);
    ASSERT_THROW( save< big_endian<1> >(vectorWriter, 0x123), lossy_conversion);
    ASSERT_THROW( save< big_endian<2> >(vectorWriter, 0x12345), lossy_conversion);
    ASSERT_THROW( save< big_endian<4> >(vectorWriter, 0x123456789), lossy_conversion);
}

template<typename T, int Size>
void saveMinusOne()
{
    std::vector<uint8_t> data(Size, 0xFF);
    VectorSaveSerializer vectorWriter;
    save< little_endian<Size> >(vectorWriter, static_cast<T>(-1));
    EXPECT_EQ(vectorWriter.getData(), data);
}

template<typename T, int Size>
void loadMinusOne()
{
    std::vector<uint8_t> data(Size, 0xFF);
    MemoryLoadSerializer vectorReader(data);
    T value;
    load< little_endian<Size> >(vectorReader, value);
    EXPECT_EQ(value, -1);
}

TEST(EndianFormattersWork, SignedNumbersSave)
{
    saveMinusOne<int8_t, 1>();
    saveMinusOne<int8_t, 2>();
    saveMinusOne<int8_t, 4>();
    saveMinusOne<int8_t, 8>();

    saveMinusOne<int16_t, 1>();
    saveMinusOne<int16_t, 2>();
    saveMinusOne<int16_t, 4>();
    saveMinusOne<int16_t, 8>();

    saveMinusOne<int32_t, 1>();
    saveMinusOne<int32_t, 2>();
    saveMinusOne<int32_t, 4>();
    saveMinusOne<int32_t, 8>();

    saveMinusOne<int64_t, 1>();
    saveMinusOne<int64_t, 2>();
    saveMinusOne<int64_t, 4>();
    saveMinusOne<int64_t, 8>();
}
    
TEST(EndianFormattersWork, SignedNumbersLoad)
{
    loadMinusOne<int8_t, 1>();
    loadMinusOne<int8_t, 2>();
    loadMinusOne<int8_t, 4>();
    loadMinusOne<int8_t, 8>();

    loadMinusOne<int16_t, 1>();
    loadMinusOne<int16_t, 2>();
    loadMinusOne<int16_t, 4>();
    loadMinusOne<int16_t, 8>();

    loadMinusOne<int32_t, 1>();
    loadMinusOne<int32_t, 2>();
    loadMinusOne<int32_t, 4>();
    loadMinusOne<int32_t, 8>();

    loadMinusOne<int64_t, 1>();
    loadMinusOne<int64_t, 2>();
    loadMinusOne<int64_t, 4>();
    loadMinusOne<int64_t, 8>();
}

TEST(EndianFormattersWork, SizeWithEnums)
{
    enum OldEnum { OVAL0, OVAL1 };
    enum class NewEnum : uint16_t { NVAL0, NVAL1 };

    VectorSaveSerializer vectorWriter;

    save< little_endian<1> >(vectorWriter, OVAL1);
    save< little_endian<2> >(vectorWriter, OVAL1);
    save< little_endian<4> >(vectorWriter, OVAL1);
    save< little_endian<8> >(vectorWriter, OVAL1);
    save< big_endian<1> >(vectorWriter, OVAL1);
    save< big_endian<2> >(vectorWriter, OVAL1);
    save< big_endian<4> >(vectorWriter, OVAL1);
    save< big_endian<8> >(vectorWriter, OVAL1);

    save< little_endian<1> >(vectorWriter, NewEnum::NVAL1);
    save< little_endian<2> >(vectorWriter, NewEnum::NVAL1);
    save< little_endian<4> >(vectorWriter, NewEnum::NVAL1);
    save< little_endian<8> >(vectorWriter, NewEnum::NVAL1);
    save< big_endian<1> >(vectorWriter, NewEnum::NVAL1);
    save< big_endian<2> >(vectorWriter, NewEnum::NVAL1);
    save< big_endian<4> >(vectorWriter, NewEnum::NVAL1);
    save< big_endian<8> >(vectorWriter, NewEnum::NVAL1);

    const auto value = std::vector<uint8_t> { 0x01, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                              0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
                                              0x01, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                              0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01 };
    EXPECT_EQ(vectorWriter.getData(), value);

    MemoryLoadSerializer vectorReader(value);

    load< const_formatter< little_endian<1> > >(vectorReader, OVAL1);
    load< const_formatter< little_endian<2> > >(vectorReader, OVAL1);
    load< const_formatter< little_endian<4> > >(vectorReader, OVAL1);
    load< const_formatter< little_endian<8> > >(vectorReader, OVAL1);
    load< const_formatter< big_endian<1> > >(vectorReader, OVAL1);
    load< const_formatter< big_endian<2> > >(vectorReader, OVAL1);
    load< const_formatter< big_endian<4> > >(vectorReader, OVAL1);
    load< const_formatter< big_endian<8> > >(vectorReader, OVAL1);

    load< const_formatter< little_endian<1> > >(vectorReader, NewEnum::NVAL1);
    load< const_formatter< little_endian<2> > >(vectorReader, NewEnum::NVAL1);
    load< const_formatter< little_endian<4> > >(vectorReader, NewEnum::NVAL1);
    load< const_formatter< little_endian<8> > >(vectorReader, NewEnum::NVAL1);
    load< const_formatter< big_endian<1> > >(vectorReader, NewEnum::NVAL1);
    load< const_formatter< big_endian<2> > >(vectorReader, NewEnum::NVAL1);
    load< const_formatter< big_endian<4> > >(vectorReader, NewEnum::NVAL1);
    load< const_formatter< big_endian<8> > >(vectorReader, NewEnum::NVAL1);
}

TEST(EndianFormattersWork, Floats)
{
    VectorSaveSerializer vectorWriter;

    float f = std::numeric_limits<float>::max();
    float ft = 1.0f / 3.0f;
    double d = std::numeric_limits<double>::max();
    double dt = 1.0 / 3.0;

    save< little_endian<4> >(vectorWriter, f);
    save< little_endian<4> >(vectorWriter, ft);
    save< big_endian<4> >(vectorWriter, f);
    save< big_endian<4> >(vectorWriter, ft);
    save< little_endian<8> >(vectorWriter, d);
    save< little_endian<8> >(vectorWriter, dt);
    save< big_endian<8> >(vectorWriter, d);
    save< big_endian<8> >(vectorWriter, dt);

    const auto value = std::vector<uint8_t> { 0xFF, 0xFF, 0x7F, 0x7F, 0xAB, 0xAA, 0xAA, 0x3E,
                                              0x7F, 0x7F, 0xFF, 0xFF, 0x3E, 0xAA, 0xAA, 0xAB,
                                              0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xEF, 0x7F,
                                              0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0xD5, 0x3F,
                                              0x7F, 0xEF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
                                              0x3F, 0xD5, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55,
                                              };
    EXPECT_EQ(vectorWriter.getData(), value);

    MemoryLoadSerializer vectorReader(value);

    load< const_formatter< little_endian<4> > >(vectorReader, f);
    load< const_formatter< little_endian<4> > >(vectorReader, ft);
    load< const_formatter< big_endian<4> > >(vectorReader, f);
    load< const_formatter< big_endian<4> > >(vectorReader, ft);
    load< const_formatter< little_endian<8> > >(vectorReader, d);
    load< const_formatter< little_endian<8> > >(vectorReader, dt);
    load< const_formatter< big_endian<8> > >(vectorReader, d);
    load< const_formatter< big_endian<8> > >(vectorReader, dt);
}

TEST(EndianFormattersWork, Bool)
{
    VectorSaveSerializer vectorWriter;
    bool bt = true;
    bool bf = false;
    save< little_endian<1> >(vectorWriter, bf);
    save< little_endian<1> >(vectorWriter, bt);
    save< little_endian<2> >(vectorWriter, bt);
    save< big_endian<2> >(vectorWriter, bt);
    save< big_endian<2> >(vectorWriter, bf);

    const auto value = std::vector<uint8_t> { 0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00 };
    MemoryLoadSerializer vectorReader(value);
    load< const_formatter< little_endian<1> > >(vectorReader, bf);
    load< const_formatter< little_endian<1> > >(vectorReader, bt);
    load< const_formatter< little_endian<2> > >(vectorReader, bt);
    load< const_formatter< big_endian<2> > >(vectorReader, bt);
    load< const_formatter< big_endian<2> > >(vectorReader, bf);
}

TEST(EndianFormattersWork, NonPowerOfTwo)
{
    VectorSaveSerializer vectorWriter;

    int valueUnderPos = 0x00223344;
    int valueUnderNeg = 0xFFDDCCBB;
    int valueOverPos = 0x11223344;
    int valueOverNeg = 0x88223344;

    save< little_endian<3> >(vectorWriter, valueUnderPos);
    save< little_endian<3> >(vectorWriter, valueUnderNeg);
    ASSERT_THROW( save< little_endian<3> >(vectorWriter, valueOverPos), lossy_conversion);
    ASSERT_THROW( save< little_endian<3> >(vectorWriter, valueOverNeg), lossy_conversion);

    save< big_endian<3> >(vectorWriter, valueUnderPos);
    save< big_endian<3> >(vectorWriter, valueUnderNeg);
    ASSERT_THROW( save< big_endian<3> >(vectorWriter, valueOverPos), lossy_conversion);
    ASSERT_THROW( save< big_endian<3> >(vectorWriter, valueOverNeg), lossy_conversion);

    const auto value = std::vector<uint8_t> { 0x44, 0x33, 0x22, 0xBB, 0xCC, 0xDD, 0x22, 0x33, 0x44, 0xDD, 0xCC, 0xBB };
    MemoryLoadSerializer vectorReader(value);
    load< const_formatter< little_endian<3> > >(vectorReader, valueUnderPos);
    load< const_formatter< little_endian<3> > >(vectorReader, valueUnderNeg);
    load< const_formatter< big_endian<3> > >(vectorReader, valueUnderPos);
    load< const_formatter< big_endian<3> > >(vectorReader, valueUnderNeg);
}

}  // namespace
