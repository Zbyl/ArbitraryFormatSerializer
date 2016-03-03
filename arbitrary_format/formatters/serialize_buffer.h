/////////////////////////////////////////////////////////////////////////////
/// ArbitraryFormatSerializer
///    Library for serializing data in arbitrary formats.
///
/// serialize_buffer.h
///
/// This file contains functions for saving and loading buffers of data.
/// Those are meant as building blocks for other formatters.
///
/// Distributed under Apache License, Version 2.0 (http://www.apache.org/licenses/LICENSE-2.0)
/// (c) 2014 Zbigniew Skowron, zbychs@gmail.com
///
/////////////////////////////////////////////////////////////////////////////

#ifndef ArbitraryFormatSerializer_buffer_formatter_H
#define ArbitraryFormatSerializer_buffer_formatter_H

#include "binary_formatters/verbatim_formatter.h"

#include <boost/utility/enable_if.hpp>

namespace arbitrary_format
{

template<typename ValueFormatter, typename ValueType, typename TSerializer, typename SizeType>
typename std::enable_if< !binary::is_verbatim_formatter<ValueFormatter, ValueType>::value >::type 
save_buffer(TSerializer& serializer, SizeType size, const ValueType *const array, ValueFormatter&& value_formatter)
{
    for (SizeType i = 0; i < size; ++i)
    {
        std::forward<ValueFormatter>(value_formatter).save(serializer, array[i]);
    }
}

template<typename ValueFormatter, typename ValueType, typename TSerializer, typename SizeType>
typename std::enable_if< binary::is_verbatim_formatter<ValueFormatter, ValueType>::value >::type 
save_buffer(TSerializer& serializer, SizeType size, const ValueType *const array, ValueFormatter&& value_formatter)
{
    serializer.saveData(reinterpret_cast<const boost::uint8_t*>(array), size * sizeof(ValueType));
}

template<typename ValueFormatter, typename ValueType, typename TSerializer, typename SizeType>
typename std::enable_if< !binary::is_verbatim_formatter<ValueFormatter, ValueType>::value >::type 
load_buffer(TSerializer& serializer, SizeType size, ValueType *const array, ValueFormatter&& value_formatter)
{
    for (SizeType i = 0; i < size; ++i)
    {
        std::forward<ValueFormatter>(value_formatter).load(serializer, array[i]);
    }
}

template<typename ValueFormatter, typename ValueType, typename TSerializer, typename SizeType>
typename std::enable_if< binary::is_verbatim_formatter<ValueFormatter, ValueType>::value >::type 
load_buffer(TSerializer& serializer, SizeType size, ValueType *const array, ValueFormatter&& value_formatter)
{
    serializer.loadData(reinterpret_cast<boost::uint8_t*>(array), size * sizeof(ValueType));
}

} // namespace arbitrary_format

#endif // ArbitraryFormatSerializer_buffer_formatter_H
