// BinarySerializer.cpp : Defines the entry point for the console application.
//

#include "binary_serializers/ISerializer.h"

#include "formatters/unified_formatter_base.h"
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

class simple_struct_formatter : public unified_formatter_base<simple_struct_formatter>
{
public:
    template<typename TSerializer>
    void serialize(TSerializer& serializer, SimpleStruct& simpleStruct) const
    {
        /// syntax 1: binary format specified as a template parameter
        binary::serialize< little_endian<4> >(serializer, simpleStruct.number);
        binary::serialize< string_formatter< little_endian<2> > >(serializer, simpleStruct.text);

        /// syntax 2: binary format specified as a function parameter (allows for stateful formatters)
        ///            count formatter     key formatter            value formatter
        map_formatter< little_endian<4>, little_endian<1>, string_formatter< little_endian<4> > > mapFormat;
        serializer.serialize(simpleStruct.map, mapFormat);
    }
};

void example()
{
    // serialization
    VectorSaveSerializer vectorWriter;
    SimpleStruct simple;
    vectorWriter.serialize<simple_struct_formatter>(simple);

    // deserialization
    MemoryLoadSerializer vectorReader(vectorWriter.getData());
    SimpleStruct simple2;
    vectorReader.serialize<simple_struct_formatter>(simple2);

    vectorWriter.serialize< const_formatter<simple_struct_formatter> >(simple);
    vectorReader.serialize< const_formatter<simple_struct_formatter> >(simple2);
}

int mainE(int argc, char* argv[])
{
    map_formatter< little_endian<4>, little_endian<1>, string_formatter< little_endian<4> > > mapFormat;

    std::map<int, std::string> map;

    map[0] = "Ala";
    map[1] = "Ma";
    map[2] = "Kota";

    CoutSerializer coutSerializer(true);
    coutSerializer.serialize(map, mapFormat);

    coutSerializer.serialize< map_formatter<
                                    little_endian<4>,
                                    little_endian<1>,
                                    string_formatter< little_endian<4> >
                                > >(map);

    std::map<int, std::string> map2;
    std::map<int, std::string> map3;
    std::map<int, std::string> map4;

    type_formatter<ISerializer, std::map<int, std::string> > mapFormat2(mapFormat);

    any_formatter<ISerializer> mapFormat3(make_any_formatter<ISerializer, std::map<int, std::string> >(mapFormat));

    boost::uint8_t lola = 5;
    boost::uint8_t lola2;

    VectorSaveSerializer vectorWriter;
    vectorWriter.serialize(map, mapFormat);
    AnySerializer<VectorSaveSerializer>(vectorWriter).serialize(map, mapFormat2);
    AnySerializer<VectorSaveSerializer>(vectorWriter).serialize(map, mapFormat3);
    vectorWriter.serialize< const_formatter< fixed_size_array_formatter< little_endian<4> > > >("MAGIC_STRING");
    vectorWriter.serialize< inefficient_size_prefix_formatter< little_endian<1>, little_endian<4> > >(lola);

    MemoryLoadSerializer vectorReader(vectorWriter.getData());
    AnySerializer<MemoryLoadSerializer>(vectorReader).serialize(map2, mapFormat3);
    vectorReader.serialize(map3, mapFormat);
    AnySerializer<MemoryLoadSerializer>(vectorReader).serialize(map4, mapFormat2);
    vectorReader.serialize< const_formatter< fixed_size_array_formatter< little_endian<4> > > >("MAGIC_STRING");
    vectorReader.serialize< inefficient_size_prefix_formatter< little_endian<1>, little_endian<4> > >(lola2);

    return 0;
}

