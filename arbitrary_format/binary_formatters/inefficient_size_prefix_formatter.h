/////////////////////////////////////////////////////////////////////////////
/// ArbitraryFormatSerializer
///    Library for serializing data in arbitrary formats.
///
/// inefficient_size_prefix_formatter.h
///
/// This file contains inefficient_size_prefix_formatter that stores size of the serialized value, followed by the serialized value itself.
/// NOTE: This formatter performs the save serialization twice on the value: once to compute the size, and the second time to actually write the data.
///       This might have unintended side effects when used with stateful formatters.
///       It also will have *exponential* complexity on complicated data structures, like trees.
///
/// Distributed under Apache License, Version 2.0 (http://www.apache.org/licenses/LICENSE-2.0)
/// (c) 2014 Zbigniew Skowron, zbychs@gmail.com
///
/////////////////////////////////////////////////////////////////////////////

#ifndef ArbitraryFormatSerializer_inefficient_size_prefix_formatter_H
#define ArbitraryFormatSerializer_inefficient_size_prefix_formatter_H

#include <arbitrary_format/binary_serializers/SizeCountingSerializer.h>
#include <arbitrary_format/binary_serializers/ScopedSerializer.h>

namespace arbitrary_format
{
namespace binary
{

template<typename SizeFormatter, typename ValueFormatter>
class inefficient_size_prefix_formatter
{
    SizeFormatter size_formatter;
    ValueFormatter value_formatter;

public:
    inefficient_size_prefix_formatter(SizeFormatter size_formatter = SizeFormatter(), ValueFormatter value_formatter = ValueFormatter())
        : size_formatter(size_formatter)
        , value_formatter(value_formatter)
    {
    }

    template<typename ValueType, typename TSerializer>
    void save(TSerializer& serializer, const ValueType& value) const
    {
        SizeCountingSerializer sizeCountingSerializer;
        value_formatter.save(sizeCountingSerializer, value);

        boost::uintmax_t byteCount = sizeCountingSerializer.getByteCount();
        size_formatter.save(serializer, byteCount);

        value_formatter.save(serializer, value);
    }

    /// @brief This method will verify that deserialization read exactly the number of bytes stored in the size field.
    ///        It will throw end_of_input when more data was attepted to be read, and invalid_data if not all data was read.
    template<typename ValueType, typename TSerializer>
    void load(TSerializer& serializer, ValueType& value) const
    {
        boost::uintmax_t byteCount;
        size_formatter.load(serializer, byteCount);

        ScopedSerializer<TSerializer> scopedSerializer(serializer, byteCount);
        value_formatter.load(scopedSerializer, value);
        scopedSerializer.verifyAllBytesProcessed();
    }
};

template<typename SizeFormatter, typename ValueFormatter>
inefficient_size_prefix_formatter<SizeFormatter, ValueFormatter> create_inefficient_size_prefix_formatter(SizeFormatter size_formatter = SizeFormatter(), ValueFormatter value_formatter = ValueFormatter())
{
    return inefficient_size_prefix_formatter<SizeFormatter, ValueFormatter>(size_formatter, value_formatter);
}

} // namespace binary
} // namespace arbitrary_format

#endif // ArbitraryFormatSerializer_inefficient_size_prefix_formatter_H
