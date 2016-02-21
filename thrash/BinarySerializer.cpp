// BinarySerializer.cpp : Defines the entry point for the console application.
//

#include "binary_serializers/ISerializer.h"

#include "formatters/implement_save_load_serialize.h"
#include "binary_formatters/string_formatter.h"
#include "formatters/map_formatter.h"
#include "formatters/vector_formatter.h"
#include "binary_formatters/endian_formatter.h"
#include "formatters/const_formatter.h"
#include "formatters/fixed_size_array_formatter.h"
#include "binary_formatters/inefficient_size_prefix_formatter.h"
#include "formatters/type_formatter.h"
#include "formatters/any_formatter.h"

#include "binary_serializers/VectorSaveSerializer.h"
#include "binary_serializers/CoutSerializer.h"
#include "binary_serializers/SizeCountingSerializer.h"
#include "binary_serializers/MemorySerializer.h"

#include <cstdint>
#include <string>
#include <map>
#include <vector>

using namespace arbitrary_format;
using namespace binary;

struct SimpleStruct
{
    int number;
    std::string text;
    std::map<int, std::string> map;

};

bool operator==(SimpleStruct, SimpleStruct)
{
    return false;
}

class simple_struct_formatter : public implement_save_load<simple_struct_formatter>
{
public:
    template<typename TSerializer>
    void save_or_load(TSerializer& serializer, SimpleStruct& simpleStruct) const
    {
        /// syntax 1: binary format specified as a template parameter
        serialize< little_endian<4> >(serializer, simpleStruct.number);
        serialize< string_formatter< little_endian<2> > >(serializer, simpleStruct.text);

        /// syntax 2: binary format specified as a function parameter (allows for stateful formatters)
        ///            count formatter     key formatter            value formatter
        map_formatter< little_endian<4>, little_endian<1>, string_formatter< little_endian<4> > > mapFormat;
        serialize(serializer, simpleStruct.map, mapFormat);
    }
};

void example()
{
    static_assert(!has_save_or_load<int, VectorSaveSerializer, SimpleStruct>::type::value, "A<int> doesn't have serialize");
    static_assert(has_save_or_load<simple_struct_formatter, VectorSaveSerializer, SimpleStruct>::value, "simple_struct_formatter doesn't have serialize");
    static_assert(has_save_or_load<const simple_struct_formatter, VectorSaveSerializer, SimpleStruct>::value, "const simple_struct_formatter doesn't have serialize");
    static_assert(is_saving_serializer<VectorSaveSerializer>::value, "VectorSaveSerializer can't save");
    static_assert(!is_loading_serializer<VectorSaveSerializer>::value, "VectorSaveSerializer can load");
    static_assert(is_loading_serializer<MemoryLoadSerializer>::value, "MemoryLoadSerializer can't load");
    static_assert(!is_saving_serializer<MemoryLoadSerializer>::value, "MemoryLoadSerializer can save");

    // serialization
    VectorSaveSerializer vectorWriter;
    SimpleStruct simple;
    serialize<simple_struct_formatter>(vectorWriter, simple);

    // deserialization
    MemoryLoadSerializer vectorReader(vectorWriter.getData());
    SimpleStruct simple2;
     serialize<simple_struct_formatter>(vectorReader, simple2);

    serialize< const_formatter<simple_struct_formatter> >(vectorWriter, simple);
    serialize< const_formatter<simple_struct_formatter> >(vectorReader, simple2);
}

int main(int argc, char* argv[])
{
    example();
    return 0;

    map_formatter< little_endian<4>, little_endian<1>, string_formatter< little_endian<4> > > mapFormat;

    std::map<int, std::string> map;

    map[0] = "Ala";
    map[1] = "Ma";
    map[2] = "Kota";

    CoutSerializer coutSerializer(true);
    serialize(coutSerializer, map, mapFormat);

    serialize< map_formatter<
                                    little_endian<4>,
                                    little_endian<1>,
                                    string_formatter< little_endian<4> >
                                > >(coutSerializer, map);

    std::map<int, std::string> map2;
    std::map<int, std::string> map3;
    std::map<int, std::string> map4;

    type_formatter<ISlowSerializer, std::map<int, std::string> > mapFormat2(mapFormat);

    any_formatter<ISlowSerializer> mapFormat3(make_any_formatter<ISlowSerializer, std::map<int, std::string> >(mapFormat));

    uint8_t lola = 5;
    uint8_t lola2;

    VectorSaveSerializer vectorWriter;
    serialize(vectorWriter, map, mapFormat);
    auto anyVectorWriter = AnySerializer<VectorSaveSerializer, ISlowSerializer>(vectorWriter);
    slow_serialize(anyVectorWriter, map, mapFormat2);
    slow_serialize(anyVectorWriter, map, mapFormat3);
    serialize< const_formatter< fixed_size_array_formatter< little_endian<4> > > >(vectorWriter, "MAGIC_STRING");
    serialize< inefficient_size_prefix_formatter< little_endian<1>, little_endian<4> > >(vectorWriter, lola);

    MemoryLoadSerializer vectorReader(vectorWriter.getData());
    auto anyVectorReader = AnySerializer<MemoryLoadSerializer, ISlowSerializer>(vectorReader);
    slow_serialize(anyVectorReader, map2, mapFormat3);
    serialize(vectorReader, map3, mapFormat);
    slow_serialize(anyVectorReader, map4, mapFormat2);
    serialize< const_formatter< fixed_size_array_formatter< little_endian<4> > > >(vectorReader, "MAGIC_STRING");
    serialize< inefficient_size_prefix_formatter< little_endian<1>, little_endian<4> > >(vectorReader, lola2);

    return 0;
}

