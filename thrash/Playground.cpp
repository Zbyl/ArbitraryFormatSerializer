// BinarySerializer.cpp : Defines the entry point for the console application.
//

// TODO: Base 128 Varint formatter and all of Google protocol buffers

#include "binary_serializers/ISerializer.h"

#include "binary_formatters/string_formatter.h"
#include "formatters/map_formatter.h"
#include "binary_formatters/endian_formatter.h"
#include "formatters/type_formatter.h"
#include "formatters/any_formatter.h"
#include "formatters/vector_formatter.h"
#include "formatters/const_formatter.h"
#include "binary_formatters/bit_formatter.h"

#include "binary_serializers/VectorSaveSerializer.h"
#include "binary_serializers/MemorySerializer.h"

#include <cstdint>
#include <string>
#include <map>
#include <vector>

void ela()
{
    using namespace arbitrary_format;
    using namespace binary;

    VectorSaveSerializer vectorWriter;
    bit_formatter<boost::endian::order::little, 1, 7>().save(vectorWriter, 1, 0xFF);
}

#include "utility/has_member.h"

struct WithX { int X; };
struct WithoutX { int Y; };
GENERATE_HAS_MEMBER(X);

struct Pos1 { int position; };
struct Pos2 { int position(int); };
GENERATE_HAS_MEMBER(position);

void ala()
{
    static_assert(has_member_X<WithX>::value == 1, "WithX should have X!");
    static_assert(has_member_X<WithoutX>::value == 0, "WithoutX should not have X!");

    static_assert(has_member_position<Pos1>::value == 1, "Pos1 should have position!");
    static_assert(has_member_position<Pos2>::value == 1, "Pos2 should have position!");
    static_assert(has_member_position<WithX>::value == 0, "WithX should not have position!");

    //static_assert(binary_format::has_data_position<WithoutX>::value == 0, "WithoutX should not have X!");
}

using namespace arbitrary_format;
using namespace binary;

class TempVectorSaveSerializer
{
    std::vector<boost::uint8_t> buffer;
public:
    const std::vector<boost::uint8_t>& getData()
    {
        return buffer;
    }

    bool saving()
    {
        return true;
    }

    /// @brief Returs true if serializer is a loading serializer.
    bool loading() { return !saving(); }

    /// @brief Saves given object using specified formatter.
    ///        Will throw serialization_exception if serializer is a loading serializer.
    template<typename Formatter, typename T>
    void save(const T& object, Formatter& formatter = Formatter())
    {
        if (!saving())
        {
            BOOST_THROW_EXCEPTION(serialization_exception() << detail::errinfo_description("Can't save to a loading serializer."));
        }

        formatter.save(*this, object);
    }

    /// @brief Loads given object using specified formatter.
    ///        Will throw serialization_exception if serializer is a saving serializer.
    template<typename Formatter, typename T>
    void load(T& object, Formatter& formatter = Formatter())
    {
        if (!loading())
        {
            BOOST_THROW_EXCEPTION(serialization_exception() << detail::errinfo_description("Can't load from a saving serializer."));
        }

        formatter.load(*this, object);
    }

    /// @brief Serializes given object using specified formatter.
    ///        Calls save() or load() depending on whether serializer if a saving or loading serializer.
    template<typename Formatter, typename T>
    void serialize(T& object, const Formatter& formatter = Formatter())
    {
        if (saving())
        {
            formatter.save(*this, object);
        }
        else
        {
            formatter.load(*this, object);
        }
    }

public:
    void saveData(const boost::uint8_t* data, size_t size)
    {
        serializeData(const_cast<boost::uint8_t*>(data), size);
    }

    void loadData(boost::uint8_t* data, size_t size)
    {
        serializeData(data, size);
    }

    void serializeData(boost::uint8_t* data, size_t size)
    {
        buffer.insert(buffer.end(), data, data + size);
    }
};

int main(int argc, char* argv[])
{
    map_formatter< little_endian<4>, little_endian<1>, string_formatter< little_endian<4> > > mapFormat;

    std::map<int, std::string> map;

    map[0] = "Ala";
    map[1] = "Ma";
    map[2] = "Kota";

    std::map<int, std::string> map2;
    std::map<int, std::string> map3;
    std::map<int, std::string> map4;

    type_formatter< TempVectorSaveSerializer, std::map<int, std::string> > mapFormat2(mapFormat);

    any_formatter<MemoryLoadSerializer> mapFormat3(make_any_formatter<MemoryLoadSerializer, std::map<int, std::string> >(mapFormat));

    type_formatter<ISeekableSerializer, std::map<int, std::string> > mapFormat4(mapFormat);

    TempVectorSaveSerializer vectorWriter;
    vectorWriter.serialize(map, mapFormat);
    vectorWriter.serialize(map, mapFormat2);
    make_seekable_serializer_force(vectorWriter).serialize(map, mapFormat4);

    MemoryLoadSerializer vectorReader(vectorWriter.getData());
    make_seekable_serializer_force(vectorReader).serialize(map2, mapFormat4);
    vectorReader.serialize(map3, mapFormat3);
    vectorReader.serialize(map4, mapFormat);

    std::vector<int> vector {1, 2, 3, 4, 5};

    VectorSaveSerializer vectorWriter2;
    serialize< vector_formatter< little_endian<1>, little_endian<4> > >(vectorWriter2, vector);
    serialize< vector_formatter< little_endian<1>, little_endian<1> > >(vectorWriter2, vector);
    serialize< vector_formatter< little_endian<1>, little_endian<1> > >(vectorWriter2, vector);
    save< big_endian<4> >(vectorWriter2, 0x12345678);
    save< big_endian<4> >(vectorWriter2, 0x12345678);

    std::vector<int> vector2;
    std::vector<int> vector3;
    std::vector<char> vector4 {1, 2, 3, 4, 5};
    MemoryLoadSerializer vectorReader2(vectorWriter2.getData());
    serialize< vector_formatter< little_endian<1>, little_endian<4> > >(vectorReader2, vector2);
    serialize< vector_formatter< little_endian<1>, little_endian<1> > >(vectorReader2, vector3);
    serialize< const_formatter< vector_formatter< little_endian<1>, little_endian<1> > > >(vectorReader2, vector4);
    serialize< const_formatter< big_endian<4> > >(vectorReader2, 0x12345678);
    load< const_formatter< little_endian<4> > >(vectorReader2, 0x78563412);

    return 0;
}

