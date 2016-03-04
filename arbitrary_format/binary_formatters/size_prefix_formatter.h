/////////////////////////////////////////////////////////////////////////////
/// ArbitraryFormatSerializer
///    Library for serializing data in arbitrary formats.
///
/// size_prefix_formatter.h
///
/// This file contains size_prefix_formatter that stores size of the serialized value, followed by the serialized value itself.
/// NOTE: This formatter requires it's serializer to be ISeekable.
///
/// Distributed under Apache License, Version 2.0 (http://www.apache.org/licenses/LICENSE-2.0)
/// (c) 2014 Zbigniew Skowron, zbychs@gmail.com
///
/////////////////////////////////////////////////////////////////////////////

#ifndef ArbitraryFormatSerializer_size_prefix_formatter_H
#define ArbitraryFormatSerializer_size_prefix_formatter_H

#include <arbitrary_format/binary_serializers/ScopedSerializer.h>

#include <cstdint>

namespace arbitrary_format
{
namespace binary
{

/// @brief sized_formatter function returns formatter that should consume given number of bytes.
///        It should be defined for all formatters that are intended to be used with size_prefix_formatter.
///        Overload this function to provide more meaningful semantics.
///        Overloads of this function will be found using Argument Dependent Lookup.
template<typename Formatter>
const Formatter& sized_formatter(const Formatter& formatter, uintmax_t byteCount)
{
    return formatter;
}

/// @brief size_prefix_formatter will prefix the serialized data with field containing size of data.
///        Serializers used with this formatter must define position() and seek() methods.
///        ValueFormatters used this formatter must have proper sized_formatter overload provided.
template<typename SizeFormatter, typename ValueFormatter>
class size_prefix_formatter
{
    SizeFormatter size_formatter;
    ValueFormatter value_formatter;


public:
    /// @note size_formatter must always store the same number of bytes
    size_prefix_formatter(SizeFormatter size_formatter = SizeFormatter(), ValueFormatter value_formatter = ValueFormatter())
        : size_formatter(size_formatter)
        , value_formatter(value_formatter)
    {
    }

    template<typename ValueType, typename TSerializer>
    void save(TSerializer& serializer, const ValueType& value)
    {
        auto initialPosition = serializer.position();

        uintmax_t byteCount = 0;
        size_formatter.save(serializer, byteCount);  // reserve space for storing byteCount

        auto dataPosition = serializer.position();  // position where data starts
        value_formatter.save(serializer, value);
        auto endPosition = serializer.position();  // position where data ends

        byteCount = static_cast<uintmax_t>(endPosition - dataPosition);
        if (byteCount < 0)
        {
            BOOST_THROW_EXCEPTION(serialization_exception() << detail::errinfo_description("Data size cannot be less than zero."));
        }

        serializer.seek(initialPosition);
        size_formatter.save(serializer, byteCount); // write actual byteCount

        auto afterSizePosition = serializer.position();

        if (afterSizePosition != dataPosition)
        {
            BOOST_THROW_EXCEPTION(serialization_exception() << detail::errinfo_description("size_formatter must always store the same number of bytes."));
        }
    }

    /// @brief This method will verify that deserialization read exactly the number of bytes stored in the size field.
    ///        It will throw end_of_input when more data was attepted to be read, and invalid_data if not all data was read.
    template<typename ValueType, typename TSerializer>
    void load(TSerializer& serializer, ValueType& value)
    {
        uintmax_t byteCount;
        size_formatter.load(serializer, byteCount);
        if (byteCount < 0)
        {
            BOOST_THROW_EXCEPTION(serialization_exception() << detail::errinfo_description("Data size cannot be less than zero."));
        }

        ScopedSerializer<TSerializer> scopedSerializer(serializer, byteCount);
        sized_formatter(value_formatter, byteCount).load(scopedSerializer, value);
        scopedSerializer.verifyAllBytesProcessed();
    }
};

template<typename SizeFormatter, typename ValueFormatter>
size_prefix_formatter<SizeFormatter, ValueFormatter> create_size_prefix_formatter(SizeFormatter size_formatter = SizeFormatter(), ValueFormatter value_formatter = ValueFormatter())
{
    return size_prefix_formatter<SizeFormatter, ValueFormatter>(size_formatter, value_formatter);
}

} // namespace binary
} // namespace arbitrary_format

#endif // ArbitraryFormatSerializer_size_prefix_formatter_H
