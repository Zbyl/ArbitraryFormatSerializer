// BinarySerializer.cpp : Defines the entry point for the console application.
//

// TODO: Base 128 Varint formatter and all of Google protocol buffers

#include <arbitrary_format/serialize.h>
#include <arbitrary_format/binary_serializers/ISerializer.h>

#include <arbitrary_format/binary_formatters/string_formatter.h>
#include <arbitrary_format/formatters/map_formatter.h>
#include <arbitrary_format/binary_formatters/endian_formatter.h>
//#include <arbitrary_format/formatters/type_formatter.h>
//#include <arbitrary_format/formatters/any_formatter.h>
#include <arbitrary_format/formatters/vector_formatter.h>
#include <arbitrary_format/formatters/const_formatter.h>
//#include <arbitrary_format/binary_formatters/bit_formatter.h>
#include <arbitrary_format/binary_formatters/inefficient_size_prefix_formatter.h>

#include <arbitrary_format/binary_serializers/VectorSaveSerializer.h>
#include <arbitrary_format/binary_serializers/MemorySerializer.h>

#include <cstdint>
#include <string>
#include <map>
#include <vector>

void ela()
{
    using namespace arbitrary_format;
    using namespace binary;

    VectorSaveSerializer vectorWriter;
    //bit_formatter<arbitrary_format_endian::order::little, 1, 7>().save(vectorWriter, 1, 0xFF);
}

#include <arbitrary_format/utility/has_member.h>

struct WithX { int X; };
struct WithoutX { int Y; };
AFS_GENERATE_HAS_MEMBER(X);

struct Pos1 { int position; };
struct Pos2 { int position(int); };
AFS_GENERATE_HAS_MEMBER(position);

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

struct weird_size_formatter
{
    little_endian<1> byte_format;

    template<typename TSerializer>
    void save(TSerializer& serializer, uintmax_t size) const
    {
        while (size > 0x7Fu)
        {
            uint8_t part = size & 0x7Fu;
            part |= 0x80u;
            size >>= 7;
            byte_format.save(serializer, part);
        }
        byte_format.save(serializer, size);
    }

    template<typename TSerializer>
    void load(TSerializer& serializer, uintmax_t& size) const
    {
        size = 0;
        while (true)
        {
            uint8_t part;
            byte_format.load(serializer, part);
            size <<= 7;
            size |= part & 0x7Fu;

            if (part < 0x80u)
                break;
        }
    }
};

struct MyDescriptor
{
    int bla_bla_bla_my_data;
    std::vector<int> bla_bla_bla_my_children;
};

struct my_descriptor_payload_formatter : public implement_save_load<my_descriptor_payload_formatter>
{
    template<typename TSerializer>
    void save_or_load(TSerializer& serializer, MyDescriptor& myDescriptor) const
    {
        serialize< little_endian<1> >(serializer, myDescriptor.bla_bla_bla_my_data);
        serialize< vector_formatter<little_endian<1>, little_endian<2>> >(serializer, myDescriptor.bla_bla_bla_my_children);
    }
};

using my_descriptor_formatter = inefficient_size_prefix_formatter<weird_size_formatter, my_descriptor_payload_formatter>;

class TempVectorSaveSerializer
{
    std::vector<uint8_t> buffer;
public:
    const std::vector<uint8_t>& getData()
    {
        return buffer;
    }

public:
    using saving_serializer = std::true_type;

    void saveData(const uint8_t* data, size_t size)
    {
        buffer.insert(buffer.end(), data, data + size);
    }
};

int main(int argc, char* argv[])
{
    {
        VectorSaveSerializer vectorWriter;
        MyDescriptor myDescriptor;
        myDescriptor.bla_bla_bla_my_data = 7;
        myDescriptor.bla_bla_bla_my_children.assign(128, 5);
    
        serialize<my_descriptor_formatter>(vectorWriter, myDescriptor);

        MemoryLoadSerializer vectorReader(vectorWriter.getData());
        serialize<my_descriptor_formatter>(vectorReader, myDescriptor);
    }

    map_formatter< little_endian<4>, little_endian<1>, string_formatter< little_endian<4> > > mapFormat;

    std::map<int, std::string> map;

    map[0] = "Ala";
    map[1] = "Ma";
    map[2] = "Kota";

    std::map<int, std::string> map2;
    std::map<int, std::string> map3;
    std::map<int, std::string> map4;

#if 0
    type_formatter< TempVectorSaveSerializer, std::map<int, std::string> > mapFormat2(mapFormat);

    any_formatter<MemoryLoadSerializer> mapFormat3(make_any_formatter<MemoryLoadSerializer, std::map<int, std::string> >(mapFormat));

    type_formatter<ISeekableSerializer<ISlowSerializer>, std::map<int, std::string> > mapFormat4(mapFormat);

    TempVectorSaveSerializer vectorWriter;
    serialize(vectorWriter,map, mapFormat);
    serialize(vectorWriter, map, mapFormat2);
    slow_serialize(make_seekable_serializer_force_ex<ISlowSerializer>(vectorWriter), map, mapFormat4);

    MemoryLoadSerializer vectorReader(vectorWriter.getData());
    slow_serialize(make_seekable_serializer_force_ex<ISlowSerializer>(vectorReader), map2, mapFormat4);
    serialize(vectorReader, map3, mapFormat3);
    serialize(vectorReader, map4, mapFormat);
#endif

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

