// BitFormatterTests.cpp - tests for bit_formatter
//

#if defined(_MSC_VER) && (_MSC_VER < 1900)
#pragma message("Microsoft Visual C++ older than 2015 cannot compile this code.")
#else

#include "binary_serializers/VectorSaveSerializer.h"
#include "binary_serializers/MemorySerializer.h"

#include "binary_formatters/bit_formatter.h"
#include "formatters/const_formatter.h"

#include "gtest/gtest.h"

namespace {

using namespace arbitrary_format;
using namespace binary;

TEST(BitFormatterWorks, Saving)
{
    {
        VectorSaveSerializer vectorWriter;

        EXPECT_NO_THROW(( bit_formatter<boost::endian::order::little, 1, 7>().save(vectorWriter, 0, 0) ));
        EXPECT_NO_THROW(( bit_formatter<boost::endian::order::little, 1, 7>().save(vectorWriter, -1, 0) ));
        EXPECT_NO_THROW(( bit_formatter<boost::endian::order::little, 1, 7>().save(vectorWriter, 0, 63) ));
        EXPECT_NO_THROW(( bit_formatter<boost::endian::order::little, 1, 7>().save(vectorWriter, 0, -64) ));
        EXPECT_NO_THROW(( bit_formatter<boost::endian::order::little, 1, 7>().save(vectorWriter, 0, -1) ));
        EXPECT_NO_THROW(( bit_formatter<boost::endian::order::little, 1, 7>().save(vectorWriter, -1, -1) ));
        EXPECT_NO_THROW(( bit_formatter<boost::endian::order::little, 1, 7>().save(vectorWriter, 0, 127u) ));

        auto value = std::vector<uint8_t> { 0x00, 0x01, 0x7e, 0x80, 0xfe, 0xff, 0xfe };
        EXPECT_EQ(vectorWriter.getData(), value);
    }

    {
        VectorSaveSerializer vectorWriter;

        EXPECT_THROW(( bit_formatter<boost::endian::order::little, 1, 7>().save(vectorWriter, 1, 0) ), lossy_conversion);
        EXPECT_THROW(( bit_formatter<boost::endian::order::little, 1, 7>().save(vectorWriter, -2, 0) ), lossy_conversion);
        EXPECT_THROW(( bit_formatter<boost::endian::order::little, 1, 7>().save(vectorWriter, 0, 64) ), lossy_conversion);
        EXPECT_THROW(( bit_formatter<boost::endian::order::little, 1, 7>().save(vectorWriter, 0, -65) ), lossy_conversion);

        EXPECT_THROW(( bit_formatter<boost::endian::order::little, 1, 7>().save(vectorWriter, 2u, 0u) ), lossy_conversion);
        EXPECT_THROW(( bit_formatter<boost::endian::order::little, 1, 7>().save(vectorWriter, 0u, 128u) ), lossy_conversion);
    }

    {
        VectorSaveSerializer vectorWriter;

        bit_formatter<boost::endian::order::little, 1, 3, 4>().save(vectorWriter, 0, 0, 0);
        bit_formatter<boost::endian::order::little, 1, 3, 4>().save(vectorWriter, -1, 1, 1);
        bit_formatter<boost::endian::order::little, 1, 3, 4>().save(vectorWriter, -1, -1, -1);
        bit_formatter<boost::endian::order::little, 1, 3, 4>().save(vectorWriter, -1, 0, 0);
        bit_formatter<boost::endian::order::little, 1, 3, 4>().save(vectorWriter, 0, -1, 0);
        bit_formatter<boost::endian::order::little, 1, 3, 4>().save(vectorWriter, 0, 0, -1);

        auto value = std::vector<uint8_t> { 0x00, 0x13, 0xff, 0x01, 0x0e, 0xf0 };
        EXPECT_EQ(vectorWriter.getData(), value);
    }

    {
        VectorSaveSerializer vectorWriter;

        bit_formatter<boost::endian::order::little, 1, 3, 4>().save(vectorWriter, 0u, 0u, 0u);
        bit_formatter<boost::endian::order::little, 1, 3, 4>().save(vectorWriter, 1u, 1u, 1u);
        bit_formatter<boost::endian::order::little, 1, 3, 4>().save(vectorWriter, 1u, 7u, 15u);
        bit_formatter<boost::endian::order::little, 1, 3, 4>().save(vectorWriter, 1u, 0u, 0u);
        bit_formatter<boost::endian::order::little, 1, 3, 4>().save(vectorWriter, 0u, 7u, 0u);
        bit_formatter<boost::endian::order::little, 1, 3, 4>().save(vectorWriter, 0u, 0u, 15u);

        auto value = std::vector<uint8_t> { 0x00, 0x13, 0xff, 0x01, 0x0e, 0xf0 };
        EXPECT_EQ(vectorWriter.getData(), value);
    }
}

TEST(BitFormatterWorks, Loading)
{
    std::vector<uint8_t> data { 0x00, 0x01, 0x7e, 0x80, 0xfe, 0xff };
    MemoryLoadSerializer vectorReader(data);

    {
        int v0, v1;
        bit_formatter<boost::endian::order::little, 1, 7>().load(vectorReader, v0, v1);
        EXPECT_EQ(v0, 0);
        EXPECT_EQ(v1, 0);
    }

    {
        int v0, v1;
        bit_formatter<boost::endian::order::little, 1, 7>().load(vectorReader, v0, v1);
        EXPECT_EQ(v0, -1);
        EXPECT_EQ(v1, 0);
    }

    {
        int v0, v1;
        bit_formatter<boost::endian::order::little, 1, 7>().load(vectorReader, v0, v1);
        EXPECT_EQ(v0, 0);
        EXPECT_EQ(v1, 63);
    }

    {
        int v0, v1;
        bit_formatter<boost::endian::order::little, 1, 7>().load(vectorReader, v0, v1);
        EXPECT_EQ(v0, 0);
        EXPECT_EQ(v1, -64);
    }

    {
        int v0, v1;
        bit_formatter<boost::endian::order::little, 1, 7>().load(vectorReader, v0, v1);
        EXPECT_EQ(v0, 0);
        EXPECT_EQ(v1, -1);
    }

    {
        int v0, v1;
        bit_formatter<boost::endian::order::little, 1, 7>().load(vectorReader, v0, v1);
        EXPECT_EQ(v0, -1);
        EXPECT_EQ(v1, -1);
    }
}

TEST(BitFormatterWorks, LoadingThree)
{
    std::vector<uint8_t> data { 0x00, 0x13, 0xff, 0x01, 0x0e, 0xf0 };
    MemoryLoadSerializer vectorReader(data);

    {
        int v0, v1, v2;
        bit_formatter<boost::endian::order::little, 1, 3, 4>().load(vectorReader, v0, v1, v2);
        EXPECT_EQ(v0, 0);
        EXPECT_EQ(v1, 0);
        EXPECT_EQ(v2, 0);
    }

    {
        int v0, v1, v2;
        bit_formatter<boost::endian::order::little, 1, 3, 4>().load(vectorReader, v0, v1, v2);
        EXPECT_EQ(v0, -1);
        EXPECT_EQ(v1, 1);
        EXPECT_EQ(v2, 1);
    }

    {
        int v0, v1, v2;
        bit_formatter<boost::endian::order::little, 1, 3, 4>().load(vectorReader, v0, v1, v2);
        EXPECT_EQ(v0, -1);
        EXPECT_EQ(v1, -1);
        EXPECT_EQ(v2, -1);
    }

    {
        int v0, v1, v2;
        bit_formatter<boost::endian::order::little, 1, 3, 4>().load(vectorReader, v0, v1, v2);
        EXPECT_EQ(v0, -1);
        EXPECT_EQ(v1, 0);
        EXPECT_EQ(v2, 0);
    }

    {
        int v0, v1, v2;
        bit_formatter<boost::endian::order::little, 1, 3, 4>().load(vectorReader, v0, v1, v2);
        EXPECT_EQ(v0, 0);
        EXPECT_EQ(v1, -1);
        EXPECT_EQ(v2, 0);
    }

    {
        int v0, v1, v2;
        bit_formatter<boost::endian::order::little, 1, 3, 4>().load(vectorReader, v0, v1, v2);
        EXPECT_EQ(v0, 0);
        EXPECT_EQ(v1, 0);
        EXPECT_EQ(v2, -1);
    }
}

TEST(BitFormatterWorks, LoadingThreeUnsigned)
{
    std::vector<uint8_t> data { 0x00, 0x13, 0xff, 0x01, 0x0e, 0xf0 };
    MemoryLoadSerializer vectorReader(data);

    {
        unsigned v0, v1, v2;
        bit_formatter<boost::endian::order::little, 1, 3, 4>().load(vectorReader, v0, v1, v2);
        EXPECT_EQ(v0, 0);
        EXPECT_EQ(v1, 0);
        EXPECT_EQ(v2, 0);
    }

    {
        unsigned v0, v1, v2;
        bit_formatter<boost::endian::order::little, 1, 3, 4>().load(vectorReader, v0, v1, v2);
        EXPECT_EQ(v0, 1u);
        EXPECT_EQ(v1, 1u);
        EXPECT_EQ(v2, 1u);
    }

    {
        unsigned v0, v1, v2;
        bit_formatter<boost::endian::order::little, 1, 3, 4>().load(vectorReader, v0, v1, v2);
        EXPECT_EQ(v0, 1u);
        EXPECT_EQ(v1, 7u);
        EXPECT_EQ(v2, 15u);
    }

    {
        unsigned v0, v1, v2;
        bit_formatter<boost::endian::order::little, 1, 3, 4>().load(vectorReader, v0, v1, v2);
        EXPECT_EQ(v0, 1u);
        EXPECT_EQ(v1, 0u);
        EXPECT_EQ(v2, 0u);
    }

    {
        unsigned v0, v1, v2;
        bit_formatter<boost::endian::order::little, 1, 3, 4>().load(vectorReader, v0, v1, v2);
        EXPECT_EQ(v0, 0u);
        EXPECT_EQ(v1, 7u);
        EXPECT_EQ(v2, 0u);
    }

    {
        unsigned v0, v1, v2;
        bit_formatter<boost::endian::order::little, 1, 3, 4>().load(vectorReader, v0, v1, v2);
        EXPECT_EQ(v0, 0u);
        EXPECT_EQ(v1, 0u);
        EXPECT_EQ(v2, 15u);
    }
}

TEST(BitFormatterWorks, Packer)
{
    {
        VectorSaveSerializer vectorWriter;

        save< little_endian<1> >(vectorWriter, bit_packer<1, 3, 4>::pack(0, 0, 0));
        save< little_endian<1> >(vectorWriter, bit_packer<1, 3, 4>::pack(-1, 1, 1));
        save< little_endian<1> >(vectorWriter, bit_packer<1, 3, 4>::pack(-1, -1, -1));
        save< little_endian<1> >(vectorWriter, bit_packer<1, 3, 4>::pack(-1, 0, 0));
        save< little_endian<1> >(vectorWriter, bit_packer<1, 3, 4>::pack(0, -1, 0));
        save< little_endian<1> >(vectorWriter, bit_packer<1, 3, 4>::pack(0, 0, -1));

        auto value = std::vector<uint8_t> { 0x00, 0x13, 0xff, 0x01, 0x0e, 0xf0 };
        EXPECT_EQ(vectorWriter.getData(), value);
    }

    {
        std::vector<uint8_t> data { 0x00, 0x13, 0xff, 0x01, 0x0e, 0xf0 };
        MemoryLoadSerializer vectorReader(data);

        load< const_formatter< little_endian<1> > >(vectorReader, bit_packer<1, 3, 4>::pack(0, 0, 0));
        load< const_formatter< little_endian<1> > >(vectorReader, bit_packer<1, 3, 4>::pack(-1, 1, 1));
        load< const_formatter< little_endian<1> > >(vectorReader, bit_packer<1, 3, 4>::pack(-1, -1, -1));
        load< const_formatter< little_endian<1> > >(vectorReader, bit_packer<1, 3, 4>::pack(-1, 0, 0));
        load< const_formatter< little_endian<1> > >(vectorReader, bit_packer<1, 3, 4>::pack(0, -1, 0));
        load< const_formatter< little_endian<1> > >(vectorReader, bit_packer<1, 3, 4>::pack(0, 0, -1));
    }
}

TEST(BitFormatterWorks, Tuples)
{
    {
        VectorSaveSerializer vectorWriter;

        save< bit_formatter<boost::endian::order::little, 1, 3, 4> >(vectorWriter, std::make_tuple(0, 0, 0));
        save< bit_formatter<boost::endian::order::little, 1, 3, 4> >(vectorWriter, std::make_tuple(-1, 1, 1));
        save< bit_formatter<boost::endian::order::little, 1, 3, 4> >(vectorWriter, std::make_tuple(-1, -1, -1));
        save< bit_formatter<boost::endian::order::little, 1, 3, 4> >(vectorWriter, std::make_tuple(-1, 0, 0));
        save< bit_formatter<boost::endian::order::little, 1, 3, 4> >(vectorWriter, std::make_tuple(0, -1, 0));
        save< bit_formatter<boost::endian::order::little, 1, 3, 4> >(vectorWriter, std::make_tuple(0, 0, -1));

        auto value = std::vector<uint8_t> { 0x00, 0x13, 0xff, 0x01, 0x0e, 0xf0 };
        EXPECT_EQ(vectorWriter.getData(), value);
    }

    {
        std::vector<uint8_t> data { 0x00, 0x13, 0xff, 0x01, 0x0e, 0xf0 };
        MemoryLoadSerializer vectorReader(data);

        // test for loading into a reference to a tuple
        {
            std::tuple<int, int, int> tuple;
            load< bit_formatter<boost::endian::order::little, 1, 3, 4> >(vectorReader, tuple);
            EXPECT_EQ(tuple, std::make_tuple(0, 0, 0));
        }
        {
            std::tuple<int, int, int> tuple;
            load< bit_formatter<boost::endian::order::little, 1, 3, 4> >(vectorReader, tuple);
            EXPECT_EQ(tuple, std::make_tuple(-1, 1, 1));
        }
        {
            std::tuple<int, int, int> tuple;
            load< bit_formatter<boost::endian::order::little, 1, 3, 4> >(vectorReader, tuple);
            EXPECT_EQ(tuple, std::make_tuple(-1, -1, -1));
        }
        {
            std::tuple<int, int, int> tuple;
            load< bit_formatter<boost::endian::order::little, 1, 3, 4> >(vectorReader, tuple);
            EXPECT_EQ(tuple, std::make_tuple(-1, 0, 0));
        }
        {
            std::tuple<int, int, int> tuple;
            load< bit_formatter<boost::endian::order::little, 1, 3, 4> >(vectorReader, tuple);
            EXPECT_EQ(tuple, std::make_tuple(0, -1, 0));
        }
        {
            std::tuple<int, int, int> tuple;
            load< bit_formatter<boost::endian::order::little, 1, 3, 4> >(vectorReader, tuple);
            EXPECT_EQ(tuple, std::make_tuple(0, 0, -1));
        }
    }

    {
        std::vector<uint8_t> data { 0x00, 0x13, 0xff, 0x01, 0x0e, 0xf0 };
        MemoryLoadSerializer vectorReader(data);

        // test for loading into a tuple of references
        {
            int v0, v1, v2;
            load< bit_formatter<boost::endian::order::little, 1, 3, 4> >(vectorReader, std::tie(v0, v1, v2));
            EXPECT_EQ(std::make_tuple(v0, v1, v2), std::make_tuple(0, 0, 0));
        }
        {
            int v0, v1, v2;
            load< bit_formatter<boost::endian::order::little, 1, 3, 4> >(vectorReader, std::tie(v0, v1, v2));
            EXPECT_EQ(std::make_tuple(v0, v1, v2), std::make_tuple(-1, 1, 1));
        }
        {
            int v0, v1, v2;
            load< bit_formatter<boost::endian::order::little, 1, 3, 4> >(vectorReader, std::tie(v0, v1, v2));
            EXPECT_EQ(std::make_tuple(v0, v1, v2), std::make_tuple(-1, -1, -1));
        }
        {
            int v0, v1, v2;
            load< bit_formatter<boost::endian::order::little, 1, 3, 4> >(vectorReader, std::tie(v0, v1, v2));
            EXPECT_EQ(std::make_tuple(v0, v1, v2), std::make_tuple(-1, 0, 0));
        }
        {
            int v0, v1, v2;
            load< bit_formatter<boost::endian::order::little, 1, 3, 4> >(vectorReader, std::tie(v0, v1, v2));
            EXPECT_EQ(std::make_tuple(v0, v1, v2), std::make_tuple(0, -1, 0));
        }
        {
            int v0, v1, v2;
            load< bit_formatter<boost::endian::order::little, 1, 3, 4> >(vectorReader, std::tie(v0, v1, v2));
            EXPECT_EQ(std::make_tuple(v0, v1, v2), std::make_tuple(0, 0, -1));
        }
    }

    {
        std::vector<uint8_t> data { 0x00, 0x13, 0xff, 0x01, 0x0e, 0xf0 };
        MemoryLoadSerializer vectorReader(data);

        // test for loading into a reference to a tuple
        load< const_formatter< bit_formatter<boost::endian::order::little, 1, 3, 4> > >(vectorReader, std::make_tuple(0, 0, 0));
        load< const_formatter< bit_formatter<boost::endian::order::little, 1, 3, 4> > >(vectorReader, std::make_tuple(-1, 1, 1));
        load< const_formatter< bit_formatter<boost::endian::order::little, 1, 3, 4> > >(vectorReader, std::make_tuple(-1, -1, -1));
        load< const_formatter< bit_formatter<boost::endian::order::little, 1, 3, 4> > >(vectorReader, std::make_tuple(-1, 0, 0));
        load< const_formatter< bit_formatter<boost::endian::order::little, 1, 3, 4> > >(vectorReader, std::make_tuple(0, -1, 0));
        load< const_formatter< bit_formatter<boost::endian::order::little, 1, 3, 4> > >(vectorReader, std::make_tuple(0, 0, -1));
    }
}

TEST(BitFormatterWorks, Bools)
{
    {
        VectorSaveSerializer vectorWriter;

        save< bit_formatter<boost::endian::order::little, 1, 3, 4> >(vectorWriter, std::make_tuple(false, false, false));
        save< bit_formatter<boost::endian::order::little, 1, 3, 4> >(vectorWriter, std::make_tuple(true, true, true));

        auto value = std::vector<uint8_t> { 0x00, 0x13 };
        EXPECT_EQ(vectorWriter.getData(), value);
    }

    {
        std::vector<uint8_t> data { 0x00, 0x13 };
        MemoryLoadSerializer vectorReader(data);

        // test for loading into a reference to a tuple
        {
            std::tuple<bool, bool, bool> tuple;
            load< bit_formatter<boost::endian::order::little, 1, 3, 4> >(vectorReader, tuple);
            EXPECT_EQ(tuple, std::make_tuple(false, false, false));
        }
        {
            std::tuple<bool, bool, bool> tuple;
            load< bit_formatter<boost::endian::order::little, 1, 3, 4> >(vectorReader, tuple);
            EXPECT_EQ(tuple, std::make_tuple(true, true, true));
        }
    }
}

}  // namespace

#endif
