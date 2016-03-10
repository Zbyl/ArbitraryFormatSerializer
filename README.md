ArbitraryFormatSerializer
=========================

ArbitraryFormatSerializer is a header only library for serializing data into arbitrary xml, binary and other formats.

Distributed under Apache License, Version 2.0 (http://www.apache.org/licenses/LICENSE-2.0)  
(c) 2014-2016 Zbigniew Skowron, zbychs@gmail.com

[![Travis CI Status](https://travis-ci.org/Zbyl/ArbitraryFormatSerializer.svg?branch=master)](https://travis-ci.org/Zbyl/ArbitraryFormatSerializer)
[![AppVeyor Status](https://ci.appveyor.com/api/projects/status/yy0alijr3yobem79?svg=true)](https://ci.appveyor.com/project/Zbyl/arbitraryformatserializer)
[![Coverity Status](https://img.shields.io/coverity/scan/8206.svg)](https://scan.coverity.com/projects/zbyl-arbitraryformatserializer)

**Table of Contents**

- [Compilation instructions](#compilation-instructions)
- [Five minutes introduction](#five-minutes-introduction)
- [Serializers](#serializers)
- [Formatters](#formatters)
- [Writing a formatter for your type](#writing-a-formatter-for-your-type)
- [Serializing to xml and binary formats](#serializing-to-xml-and-binary-formats)
- [Stateful formatters](#stateful-formatters)
- [Further reading](#further-reading)
  - [Xml serialization](docs/XmlSerialization.md)
  - [Formatters quick reference](docs/Formatters.md)
- [Third-party libraries](#third-party-libraries)
- [Rationale](#rationale)
- [Competition](#competition)
- [Implementation notes](#implementation-notes)
- [Possible additions](#possible-additions)

Compilation instructions
========================

#### Compilers
The code has been tested on the following compilers:

Compiler        | Mode        | Notes
:---------------|:------------|:------------------------------------------------------------------------------------------------------------------
Visual C++ 2015 |             | `compile_time_string` doesn't work, use `declare_compile_time_string`
Visual C++ 2013 |             | `bit_formatter` is unsupported<br/>`compile_time_string` doesn't work, use `declare_compile_time_string` at **global scope**
clang 3.7       | --std=c++11 | Fully supported
g++ 5.2.1       | --std=c++11 | Fully supported

#### Includes

Please add `ArbitraryFormatSerializer` folder to the include directories of your project.
You will then be able to use includes like this:

```cpp
#include <arbitrary_format/serialize.h>
#include <arbitrary_format/binary_serializers/VectorSaveSerializer.h>
#include <arbitrary_format/binary_serializers/MemorySerializer.h>
#include <arbitrary_format/binary_formatters/endian_formatter.h>
#include <arbitrary_format/binary_formatters/string_formatter.h>
```

> **Note** This is a header only library, so you don't need to link to it. Just include the headers.

> **Note** This library uses boost, so you need to add boost to your include directories also.
> The library was tested on boost version 1.59, but most versions of boost should work.

#### Namespaces

All types in the library are in `arbitrary_format`, `arbitrary_format::xml` and `arbitrary_format::binary` namespaces.
You can use them like this:

```cpp
using namespace arbitrary_format;
using namespace arbitrary_format::binary;
using namespace arbitrary_format::xml;
```

See an example here: [Example-Person.cpp](thrash/Example-Person.cpp)

Five minutes introduction
=========================

Let's serialize a vector of pairs to both xml and binary formats:
```cpp
std::vector< std::pair<std::string, int> > vec =
    { {'Bjarne', 1}, {'Herb', 2}, {'Scott', 3} };
```
We want it to look like this in xml:
```xml
<gurus>
   <guru level="1">Bjarne</guru>
   <guru level="2">Herb</guru>
   <guru level="3">Scott</guru>
</gurus>
```

And like this in binary format:

vec.size() (2&nbsp;bytes)| name.length() (1&nbsp;byte) | name characters: 'B'&nbsp;'j'&nbsp;'a'&nbsp;'r'&nbsp;'n'&nbsp;'e' | level (1&nbsp;byte)| name.length() (1&nbsp;byte)| 'H'&nbsp;'e'&nbsp;... 
-------------------------|-----------------------------|-------------------------------------------------------------------|--------------------|----------------------------|----------------------------
 `0x03`&nbsp;`0x00`      |           `0x6`             | `0x42`&nbsp;`0x6A`&nbsp;`0x61`&nbsp;`0x72`&nbsp;`0x6E`&nbsp;`0x65`|       `0x01`       |           `0x04`           | `0x48`&nbsp;`0x65`&nbsp;... 

Let's first see the code for serializing to binary format:
```cpp
void saveVecToMemoryBuffer()
{
    std::vector< std::pair<std::string, int> > vec =
        { {'Bjarne', 1}, {'Herb', 2}, {'Scott', 3} };

    // vector size will be stored on 2 bytes (little endian order)
    using size_format = little_endian<2>;
    
    // name will be stored as length stored on 1 byte, followed by characters
    using name_format = string_formatter<little_endian<1>>;
    
    // name and level will be stored one after another; level as 1 byte
    using pair_format = tuple_formatter<name_format, little_endian<1>>;

    // vector will be stored as size, followed by elements
    using vector_format = vector_formatter<size_format, pair_format>;

    // prepare serialization object
    uint8_t buffer[50];
    MemorySaveSerializer serializer(buffer, sizeof(buffer));

    // let's save our vector to the memory buffer
    serialize<vector_format>(serializer, vec);
}
```
A more compact version of the above:
```cpp
void saveVecToMemoryBuffer()
{
    std::vector< std::pair<std::string, int> > vec =
        { {'Bjarne', 1}, {'Herb', 2}, {'Scott', 3} };
    uint8_t buffer[50];
    MemorySaveSerializer serializer(buffer, sizeof(buffer));

    using name_format = string_formatter<little_endian<1>>;
    using pair_format = tuple_formatter<name_format, little_endian<1>>;
    using vector_format = vector_formatter<little_endian<2>, pair_format>;
    serialize<vector_format>(serializer, vec);
}
```

Code for formatting to xml is a bit longer due to the more complicated nature of xml:
```cpp
void printXml()
{
    std::vector< std::pair<std::string, int> > vec =
        { {'Bjarne', 1}, {'Herb', 2}, {'Scott', 3} };

    declare_compile_time_string(_gurus_, "gurus");
    declare_compile_time_string(_guru_, "guru");
    declare_compile_time_string(_level_, "level");

    using name_format = element_formatter< _name_ >;
    using level_format = attribute_formatter< _level_ >;
    using pair_format = element_formatter< _guru_, tuple_formatter<name_format, level_format> >;
    using vector_format = vector_formatter< element_counter<_guru_>, pair_format >;
    using document_format = document_formatter< element_formatter<_gurus_, vector_format> >;

    RapidXmlSaveSerializer document("utf-8");
    serialize<document_format>(document, vec);
    std::cout << document;
}
```

Serializers
=========

This library provides two predefined serializer kinds:

- xml serializers: `RapidXmlSaveSerializer`, `RapidXmlLoadSerializer`,
- binary serializers: `MemorySaveSerializer`, `MemoryLoadSerializer`, `VectorSaveSerializer`, ...

But in fact those are only examples. You can write other serializers easily.

Conceptually any object that allows reading it's value is a 'loading serializer'.
And any object that allows setting it's value is a 'saving serializer'.

So even an int can be considered a loading and saving serializer! (Not a very useful one though...)

So serializer can load and store data. But we still need to define how to transform objects into this data.
*Formatters* are used for that.

Formatters
==========

A *formatter* is an object that knows how to convert some specific type into data that serializer can store.
It also knows how to convert data read from serializer into this type.

So *binary formatters* know how to convert objects to and from *bytes*.
And *xml formatters* know how to convert objects to and from *xml nodes*.

Some examples of formaters:

- **little_endian< 4 >** is a binary formatter that can store any int, bool, etc. as **4 bytes in little endian order**
- **element_formatter< name >** is an xml formatter that can store any object convertible to string as an xml node: `<name>object value</name>`

But there are formatters that don't interact directly with serializers. Instead they used other formatters to do that for them. An example of this is a formatter for `std::vector`. It takes formatters for vector size and vector elements as parameters:
`vector_formatter<size_formatter, element_formatter>`
We can use it as follows:
`vector_formatter< little_endian<2>, little_endian<1> >`
This formatter would store vector size on two bytes, followed by elements, each stored on 1 byte.

Formatters like `vector_formatter` are independent of the serializer type, and therefore can be used with all kinds of serializers: binary, xml, and others.

This library provides the following formatters that are independent from serializers:

- **vector_formatter< size_formatter, element_formatter >** for `std::vector`
- **map_formatter< size_formatter, key_formatter, value_formatter >** for `std::map`
- **map_kv_formatter< size_formatter, pair_formatter >** for `std::map`
- **optional_formatter< flag_formatter, value_formatter >** for `boost::optional`
-  **fixed_size_array_formatter< element_formatter >** for arrays like `int[10]`
- and a few more.

###Observation about formatters
From the examples above you can see, that formatters are of two kinds:

- dependent on serializers
- independent from serializers

First kind describes how to store some, usually simple, type as raw, serializer specific, data.
They will be written by the author of the serializer.

Second kind of formatter describes more the *structure* of the type.
Those are generic.

Most formatters you will need to write are of the second kind: they will describe the structure of your custom type.

Writing a formatter for your type
=================================

Each formatter has the following methods for saving and loading:
```cpp
void save(TSerializer& serializer, const T& object);
void load(TSerializer& serializer, T& object);
```

Let's write a formatter for a simple class `Person`.
```cpp
struct Person {
    std::string name;
    std::string surname;
    int age;
};
```
We want this formatter to be independent from serializer, so we will parametrize it with name, surname and age formatters. The load() and save() methods will be parametrized with serializer type.
Here's how it might look:
```cpp
template<typename name_formatter, typename surname_formatter, typename age_formatter>
struct person_formatter
{
    template<typename TSerializer>
    void save(TSerializer& serializer, const Person& person) const
    {
        serialize<name_formatter>(serializer, person.name);
        serialize<surname_formatter>(serializer, person.surname);
        serialize<age_formatter>(serializer, person.age);
    }

    template<typename TSerializer>
    void load(TSerializer& serializer, Person& person) const
    {
        serialize<name_formatter>(serializer, person.name);
        serialize<surname_formatter>(serializer, person.surname);
        serialize<age_formatter>(serializer, person.age);
    }
};
```
Let's notice, that save() and load() methods have exactly the same content!
We can rewrite the same formatter in a shorter way, using a helper class `implement_save_load`:
```cpp
template<typename name_formatter, typename surname_formatter, typename age_formatter>
struct person_formatter : public implement_save_load< person_formatter<name_formatter, surname_formatter, age_formatter> >
{
    template<typename TSerializer>
    void save_or_load(TSerializer& serializer, Person& person) const
    {
        serialize<name_formatter>(serializer, person.name);
        serialize<surname_formatter>(serializer, person.surname);
        serialize<age_formatter>(serializer, person.age);
    }
};
```

> **Note**
> Like we said in previous chapter: our custom formatter describes the *structure* of the Person class.
> The actual details of how it will be stored on bytes or xml nodes will be provided via parameters.

This is how we can use our new formatter in practice:
```cpp
void serializePerson(const Person& person)
{
    using str_format = string_formatter< little_endian<1> >;
    using age_format = little_endian<2>;
    using person_format = person_formatter<str_format, str_format, age_format>;

    VectorSaveSerializer serializer;
    serialize<person_format>(serializer, person);
}
```

See full example here: [Example-Person.cpp](thrash/Example-Person.cpp)

Serializing to xml and binary formats
=====================================

Let's say we want to serialize `Person` class from previous example to some very specific formats.
We want it to look like this in xml:
```xml
<person age="33">
    <name>John</name>
    <surname>Doe</surname>
</person>
```
And like this in binary format:

length of name |  'J'   |  'o'   |  'h'   |  'n'   |length of surname|  'D'   |  'o'   |  'e'   | age (2&nbsp;bytes) 
---------------|--------|--------|--------|--------|-----------------|--------|--------|--------|-------------------
    `0x04`     | `0x4A` | `0x6F` | `0x68` | `0x6E` |      `0x03`     | `0x44` | `0x6F` | `0x65` | `0x21`&nbsp;`0x00` 

We will of course use our `person_formatter` to do the job for us.

Actually, code for the binary serialization was shown already, at the end of previous chapter.

So let's see the code for xml serialization:
```cpp
void savePersonToXml(RapidXMLSaveSerializer& serializer, const Person& person)
{
    declare_compile_time_string(_person_, "person");
    declare_compile_time_string(_name_, "name");
    declare_compile_time_string(_surname_, "surname");
    declare_compile_time_string(_age_, "age");

    using name_format = element_formatter< _name_ >;
    using surname_format = element_formatter< _surname_ >;
    using age_format = attribute_formatter< _age_ >;
    using person_format = person_formatter<name_format, surname_format, age_format>;

    // The last thing we need to do is wrap the Person contents inside a <person> element.
    using element_format = element_formatter< _person_, person_format >;

    serialize< element_format >(serializer, person);
}
```
Since the code would look the same for loading from an xml serializer, we can parametrize the function with serializer type:
```cpp
template<typename XMLSerializer>
void serializePersonXml(XMLSerializer& serializer, const Person& person)
{
    ...
}
```
That's how we would use this code:
```cpp
void main()
{
    Person person = { "John", "Doe", 33 };
    RapidXmlSaveSerializer document("utf-8");
    serializePersonXml(document.getDocumentElement(), person);
    std::cout << document;
}
```
We can also wrap this code in a custom `xml_person_formatter` class.
Then the usage would be like this:
```cpp
void main()
{
    Person person = { "John", "Doe", 33 };
    RapidXmlSaveSerializer document("utf-8");
    serialize<xml_person_formatter>(document.getDocumentElement(), person);
    std::cout << document;
}
```
But wait...
We don't need to write a formatter class.
We can just use a typedef!
```cpp
using xml_person_formatter = element_formatter< _person_, person_format >;
serialize<xml_person_formatter>(document.getDocumentElement(), person);
```
> **Note** Actually the proper way of serializing an xml document is this:
>```cpp
>serialize< document_formatter<xml_person_formatter> >(document, person);
>```
>`document_formatter` adds an xml declaration (`<?xml version="1.0" encoding="utf-8"?>`) to the document.

Stateful formatters
===================

Let's analyze this line of code:
```cpp
serialize< formatter >(serializer, value);
```
Here the type of the formatter is passed as a template parameter to the `serialize()` function.
Inside, `serialize()` creates an object of this type (using default constructor) and then uses it to format `value`.
This is enough for most purposes, because most formatters don't need any state. They just operate on the `serializer` and `value` that were passed to their `save()` and `load()` methods.
Those are *stateless formatters*.

But sometimes there is a need for formatters that actually need state: *stateful formatters*.
If we have a *stateful formatter* it's not enough to pass the type of it to the `serialize()` function. We need to pass actual object of this type.
Here's how it's done:
```cpp
serialize(serializer, value, formatterObject);
```
Obviously we can use the same syntax also for stateless formatters. The following invocations are equivalent:
```cpp
serialize< little_endian<1> >(serializer, value);
serialize(serializer, value, little_endian<1>());
```

An example of a stateful formatter is an xml element formatter, where the name of the element is not known at compile time. The formatter itself must be initialized with a name of the element.
As a convention most formatters in the library provide a `create` method to construct formatter objects.
Here's an example:
```cpp
auto elementFormatter = create_element_formatter("element");
serialize(serializer, value, elementFormatter);
```
A more complicated example:
```cpp
auto sizeFormatter = create_attribute_formatter("size");
auto valueFormatter = create_element_formatter("value");
auto vectorFormatter = create_vector_formatter(sizeFormatter, valueFormatter);
serialize(serializer, value, vectorFormatter);
```
In this particular example the code above is equivalent to this one:
```cpp
using sizeFormatter = attribute_formatter<_size_>;
using valueFormatter = element_formatter<_value_>;
using vectorFormatter = vector_formatter<sizeFormatter, valueFormatter>;
serialize<vectorFormatter>(serializer, value);
```
But in general stateful formatters are more powerful than stateless ones.

Further reading
===============

Xml serialization is explained in more detail in this article:
[Xml serialization](docs/XmlSerialization.md)

A list of all currently available formatters is in this article:
[Formatters quick reference](docs/Formatters.md)

Third-party libraries
=====================

Xml serialization is built using "RapidXml" library by Marcin Kalicinski.
RapidXml is licensed under two licenses: "Boost Software License - Version 1.0" and "The MIT License"
See: http://rapidxml.sourceforge.net/

Compile time string types are built on top of "typestring" library by George Makrydakis.
Typestring is licensed under "Mozilla Public Licence 2.0".
See: https://github.com/irrequietus/typestring

Both of those libraries are provided in "third_party" directory.

Rationale
=========

This library is meant to solve two problems:

1. Arbitrary format should be defined entirely by the user.
  - The library does not limit what formats are possible.
  - Every call to a serialization function specifies the format explicitly. There are no default formats for any types. This way the code is easy to verify against the format specification.
2. Serialization and deserialization should be realized by the same, declarative code.
  - Declarative way of specifying the binary format is another feature that makes it easy to verify the code against the specification.
  - Unifying serialization and deserialization reduces the amout of code and eliminates the need to keep two functions in sync.
 
What this library will not provide:

1. Versioning of the binary format.
  - Since the library does not enforce any specific format, it also won't add any version tags. This is left for the user to implement is any way he chooses. Helper classes implementing versioning might be added in the future.
2. Default formats for common types.
  - The library provides formatters for common types, but it will not make them a default. The format will always have to be specified explicitly to make the code verifiable against the format specification.

Competition
===========

There are many serialization libraries out there, but most of them **enforce a specific format**, like Google Protocol Buffers or boost::serialization.
**If you need to support a specific format they cannot help you.**

boost::serialization also couples both the storage manipulation (i.e. `saveArbitrary()` function) and format definitions (`void save(T&)` functions).
Separating those two aspects (into *serializers* and *formatters*) makes ArbitraryFormatSerializer quite a bit more flexible.

Google Protocol Buffers on the other hand utilizes a code generator, which is always more trouble than pure C++.

Implementation notes
====================

1. This library uses boost libraries. Writing all those useful utilities by hand would be counter productive.
2.  XML serializers use RapidXML library for parsing XML.
3. This library uses exceptions. Not using exceptions is a very bad practice.
4. This library does not use asserts, even where they would make sense. Exceptions are thrown instead. This is because asserts are not overridable, and `BOOST_ASSERT()` has inconsistent behaviour in debug and release mode, depending on whether it was overriden by the user, or not. Once `BOOST_ASSERT()` is fixed I will probably use that.

Possible additions
==================
1. Serializers for YAML, INI, JSON and other formats.
2. Formatters for types defined by Google Protocol Buffers.
3. Helper classes implementing versioning would be a nice addition too.

