/////////////////////////////////////////////////////////////////////////////
/// BinaryFormatSerializer
///    Library for serializing data in arbitrary binary format.
///
/// string_formatter.h
///
/// This file contains string_formatter that formats string as length field followed by individual characters.
///
/// Distributed under Apache License, Version 2.0 (http://www.apache.org/licenses/LICENSE-2.0)
/// (c) 2014 Zbigniew Skowron, zbychs@gmail.com
///
/////////////////////////////////////////////////////////////////////////////

#ifndef BinaryFormatSerializer_thrash_H
#define BinaryFormatSerializer_thrash_H

namespace binary_format
{
class ISeekable
{
public:
    virtual uintmax_t position() = 0;
    virtual void seek(uintmax_t position) = 0;
};

template<typename Formatter, typename T>
class deferred_serialize
{
    ISerializer& serializer;
    T& object;
    Formatter formatter;
    uintmax_t original_offset;
    bool serialized;
public:
    deferred_serialize(ISerializer& serializer, T& object, Formatter formatter = Formatter())
        : serializer(serializer)
        , object(object)
        , formatter(formatter)
        , serialized(false)
    {
        ISeekable* seekable = dynamic_cast<ISeekable*>(&serializer);
        assert(seekable);
        original_offset = seekable->position();
        SizeCountingSerializer sizeCountingSerializer;
        sizeCountingSerializer.serialize(object, formatter);
        seekable->seek(original_offset + sizeCountingSerializer.getSize());
    }

    ~deferred_serialize()
    {
        assert(serialized);
    }

    void serialize()
    {
        assert(!serialized);
        serialized = true;
        ISeekable* seekable = dynamic_cast<ISeekable*>(&serializer);
        uintmax_t current_offset = seekable->position();
        seekable->seek(original_offset);
        serializer.serialize(object, formatter);
    }

};

template<typename SizeFormatter, typename Formatter, typename S, typename T>
void serialize_size(ISerializer& serializer, S& size, const T& object, SizeFormatter sizeFormatter = SizeFormatter(), Formatter formatter = Formatter())
{
    SizeCountingSerializer sizeCountingSerializer;
    sizeCountingSerializer.serialize(object, formatter);
    S computed_size = sizeCountingSerializer.getSize();
    if (serializer->saving())
    {
        serializer.save(computed_size, sizeFormatter);
        size = computed_size;
    }
    else
    {
        serializer.load(size, sizeFormatter);
        assert(size == computed_size);
    }
}

    ~deferred_serialize()
    {
        assert(serialized);
    }

    void serialize()
    {
        assert(!serialized);
        serialized = true;
        ISeekable* seekable = dynamic_cast<ISeekable*>(&serializer);
        uintmax_t current_offset = seekable->position();
        seekable->seek(original_offset);
        serializer.serialize(object, formatter);
    }

};

class SimpleClass
{
    int number;
    int number2;
private:
    friend void serialize_serializable(ISerializer& serializer, SimpleClass simpleClass)
    {
        serializer.serialize< little_endian<4> >(simpleClass.number);
        serializer.serialize< little_endian<1> >(simpleClass.number2);
    }
};

} // namespace binary_format

#endif // BinaryFormatSerializer_thrash_H
