/////////////////////////////////////////////////////////////////////////////
/// ArbitraryFormatSerializer
///    Library for serializing data in arbitrary formats.
///
/// vector_formatter.h
///
/// This file contains vector_formatter that formats std::vector as length field followed by individual values.
///
/// Distributed under Apache License, Version 2.0 (http://www.apache.org/licenses/LICENSE-2.0)
/// (c) 2014 Zbigniew Skowron, zbychs@gmail.com
///
/////////////////////////////////////////////////////////////////////////////

#ifndef ArbitraryFormatSerializer_vector_formatter_H
#define ArbitraryFormatSerializer_vector_formatter_H

#include "binary_formatters/verbatim_formatter.h"

#include <vector>
#include <type_traits>

namespace arbitrary_format
{

template<typename SizeFormatter, typename ValueFormatter>
class vector_formatter
{
    SizeFormatter size_formatter;
    ValueFormatter value_formatter;

public:
    vector_formatter(SizeFormatter size_formatter = SizeFormatter(), ValueFormatter value_formatter = ValueFormatter())
        : size_formatter(size_formatter)
        , value_formatter(value_formatter)
    {
    }

    template<typename ValueType, typename TSerializer>
    typename std::enable_if< !binary::is_verbatim_formatter<ValueFormatter, ValueType>::value >::type 
    save(TSerializer& serializer, const std::vector<ValueType>& vector) const
    {
        size_formatter.save(serializer, vector.size());
        for (const auto& value : vector)
        {
            value_formatter.save(serializer, value);
        }
    }

    template<typename ValueType, typename TSerializer>
    typename std::enable_if< binary::is_verbatim_formatter<ValueFormatter, ValueType>::value >::type 
    save(TSerializer& serializer, const std::vector<ValueType>& vector) const
    {
        size_formatter.save(serializer, vector.size());
        serializer.saveData(reinterpret_cast<const boost::uint8_t*>(vector.data()), vector.size() * sizeof(ValueType));
    }

    template<typename ValueType, typename TSerializer>
    typename std::enable_if< !binary::is_verbatim_formatter<ValueFormatter, ValueType>::value >::type 
    load(TSerializer& serializer, std::vector<ValueType>& vector) const
    {
        size_t vector_size;
        size_formatter.load(serializer, vector_size);

        vector.clear();
        for (size_t i = 0; i < vector_size; ++i)
        {
            vector.push_back(ValueType());
            value_formatter.load(serializer, vector.back());
        }
    }

    template<typename ValueType, typename TSerializer>
    typename std::enable_if< binary::is_verbatim_formatter<ValueFormatter, ValueType>::value >::type 
    load(TSerializer& serializer, std::vector<ValueType>& vector) const
    {
        size_t vector_size;
        size_formatter.load(serializer, vector_size);

        vector.clear();
        vector.resize(vector_size);
        serializer.loadData(reinterpret_cast<boost::uint8_t*>(vector.data()), vector.size() * sizeof(ValueType));
    }
};

template<typename SizeFormatter, typename ValueFormatter>
vector_formatter<SizeFormatter, ValueFormatter> create_vector_formatter(SizeFormatter size_formatter = SizeFormatter(), ValueFormatter value_formatter = ValueFormatter())
{
    return vector_formatter<SizeFormatter, ValueFormatter>(size_formatter, value_formatter);
}

} // namespace arbitrary_format

#endif // ArbitraryFormatSerializer_vector_formatter_H
