ArbitraryFormatSerializer
======================

ArbitraryFormatSerializer is a library for serializing data in arbitrary formats.
This library is proven in production code.

Distributed under Apache License, Version 2.0 (http://www.apache.org/licenses/LICENSE-2.0)  
(c) 2014-2016 Zbigniew Skowron, zbychs@gmail.com

Available Serializers
=====================

Currently serializers for binary formats and xml formats are provided.
User of the library can provide his own serializers for other format kinds, and use library provided formatters for the heavy lifting.

Rationale
=========

This library is meant to solve two problems:

1. Arbitrary format should be defined entirely by the user.
  - The library does not limit what formats are possible.
  - Every call to a serialization function specifies the format explicitly. There are no default values for formats. This way the code is easy to verify against the format specification.
2. Serialization and deserialization should be realized by the same, declarative code.
  - Declarative way of specifying the binary format is another feature that makes it easy to verify the code against the specification.
  - Unifying serialization and deserialization reduces the amout of code and eliminates the need to keep two functions in sync.
 
Other goals:

1. The serialization functions shouldn't need to be templatized on the serializer type.
  - In boost::serialization you generally provide serialization function as a template on the 'Archive' type. This has many inconvenient implications related to inlining serialization code in the headers or providing explicit template specializations. There is no easy way to make the boost serialization function work for different archives and still not be a template. The reason for this is that 'Archive' in boost::serialization defines both the storage manipulation (i.e. saveArbitrary() function) and format definitions (void save(T&) functions). Separating those two aspects (into 'serializers' and 'formatters') makes ArbitraryFormatSerializer quite a bit more flexible.
 
What this library will not provide:

1. Versioning of the binary format.
  - Since the library does not enforce any specific format, it also won't add any version tags. This is left for the user to implement is any way he chooses. Helper classes implementing versioning might be added in the future.
2. Default formats for common types.
  - The library provides formatters for common types, but it will not make them a default. The format will always have to be specified explicitly to make the code verifiable against the format specification.

Competition
===========

There are many serialization libraries out there, but most of them enforce a specific format, like Google Protocol Buffers or boost::serialization.
If you need to support a specific format they cannot help you.

Google Protocol Buffers also utilizes a code generator, which is always more trouble than pure C++.

Example (binary format)
=======================

```cpp
struct SimpleStruct
{
    int number;
    std::string text;
    std::map<int, std::string> map;
};

class simple_struct_formatter : public formatter_base<simple_struct_formatter>
{
public:
    void serialize(ISerializer& serializer, SimpleStruct& simpleStruct)
    {
        /// syntax 1: binary format specified as a template parameter
        serializer.serialize< little_endian<4> >(simpleStruct.number);
        serializer.serialize< string_formatter< little_endian<2> > >(simpleStruct.text);

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
}
```

Notes
=====

1. This library uses boost where standard C++ 11 would suffice. This was done because I needed to support C++ 03.
   I am slowly migrating the code to use C++ 11.
2. Google Protocol Buffers formatters would be a nice addition to this library.
3. Helper classes implementing versioning would be a nice addition too.
4. This library does not use asserts, even where they would make sense. Exceptions are thrown instead. This is because asserts are not overridable, and BOOST_ASSERT() has inconsistent behaviour in debug and release mode, depending on whether it was overriden by the user, or not. Once BOOST_ASSERT() is fixed I will probably use that.
5. It would be a nice exercise to try to make ISerializer a concept for boost::poly or adobe::poly. This is partially done now, by making serializers non-polymorphic, and providing a polymorphic AnySerializer<>.
