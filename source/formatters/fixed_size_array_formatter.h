/////////////////////////////////////////////////////////////////////////////
/// ArbitraryFormatSerializer
///    Library for serializing data in arbitrary formats.
///
/// fixed_size_array_formatter.h
///
/// This file contains fixed_size_array_formatter that formats an array as a sequence of elements. Array length is not serialized.
///
/// Distributed under Apache License, Version 2.0 (http://www.apache.org/licenses/LICENSE-2.0)
/// (c) 2014 Zbigniew Skowron, zbychs@gmail.com
///
/////////////////////////////////////////////////////////////////////////////

#ifndef ArbitraryFormatSerializer_fixed_size_array_formatter_H
#define ArbitraryFormatSerializer_fixed_size_array_formatter_H

#include "unified_formatter_base.h"
#include "binary_formatters/verbatim_formatter.h"

#include <boost/static_assert.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/type_traits/is_array.hpp>

namespace arbitrary_format
{

template<typename ValueFormatter, int ArraySize = -1>
class fixed_size_array_formatter
{
    ValueFormatter value_formatter;

public:
    fixed_size_array_formatter(ValueFormatter value_formatter = ValueFormatter())
        : value_formatter(value_formatter)
    {
    }

    template<typename ValueType, typename TSerializer>
    typename std::enable_if< !binary::is_verbatim_formatter<ValueFormatter, ValueType>::value >::type 
    save(TSerializer& serializer, const ValueType *const array) const
    {
        BOOST_STATIC_ASSERT(ArraySize >= 0);
        for (int i = 0; i < ArraySize; ++i)
        {
            value_formatter.save(serializer, array[i]);
        }
    }

    template<typename ValueType, typename TSerializer>
    typename std::enable_if< binary::is_verbatim_formatter<ValueFormatter, ValueType>::value >::type 
    save(TSerializer& serializer, const ValueType *const array) const
    {
        BOOST_STATIC_ASSERT(ArraySize >= 0);
        serializer.saveData(reinterpret_cast<const boost::uint8_t*>(array), ArraySize * sizeof(ValueType));
    }

    template<typename ValueType, typename TSerializer>
    typename std::enable_if< !binary::is_verbatim_formatter<ValueFormatter, ValueType>::value >::type 
    load(TSerializer& serializer, ValueType *const array) const
    {
        BOOST_STATIC_ASSERT(ArraySize >= 0);
        for (int i = 0; i < ArraySize; ++i)
        {
            value_formatter.load(serializer, array[i]);
        }
    }

    template<typename ValueType, typename TSerializer>
    typename std::enable_if< binary::is_verbatim_formatter<ValueFormatter, ValueType>::value >::type 
    load(TSerializer& serializer, ValueType *const array) const
    {
        BOOST_STATIC_ASSERT(ArraySize >= 0);
        serializer.loadData(reinterpret_cast<boost::uint8_t*>(array), ArraySize * sizeof(ValueType));
    }
};

template<typename ValueFormatter>
class fixed_size_array_formatter<ValueFormatter, -1>
{
    ValueFormatter value_formatter;

public:
    explicit fixed_size_array_formatter(ValueFormatter value_formatter = ValueFormatter())
        : value_formatter(value_formatter)
    {
    }

    template<int Size, typename ValueType, typename TSerializer>
    typename std::enable_if< !binary::is_verbatim_formatter<ValueFormatter, ValueType>::value >::type 
    save(TSerializer& serializer, ValueType(&array)[Size]) const
    {
        for (int i = 0; i < Size; ++i)
        {
            value_formatter.save(serializer, array[i]);
        }
    }

    template<int Size, typename ValueType, typename TSerializer>
    typename std::enable_if< binary::is_verbatim_formatter<ValueFormatter, ValueType>::value >::type 
    save(TSerializer& serializer, ValueType(&array)[Size]) const
    {
        serializer.saveData(reinterpret_cast<const boost::uint8_t*>(array), Size * sizeof(ValueType));
    }

    template<int Size, typename ValueType, typename TSerializer>
    typename std::enable_if< !binary::is_verbatim_formatter<ValueFormatter, ValueType>::value >::type 
    load(TSerializer& serializer, ValueType(&array)[Size]) const
    {
        for (int i = 0; i < Size; ++i)
        {
            value_formatter.load(serializer, array[i]);
        }
    }

    template<int Size, typename ValueType, typename TSerializer>
    typename std::enable_if< binary::is_verbatim_formatter<ValueFormatter, ValueType>::value >::type 
    load(TSerializer& serializer, ValueType(&array)[Size]) const
    {
        serializer.loadData(reinterpret_cast<boost::uint8_t*>(array), Size * sizeof(ValueType));
    }
};


template<typename ValueFormatter, int ArraySize = -1>
fixed_size_array_formatter<ValueFormatter, ArraySize> create_fixed_size_array_formatter(ValueFormatter value_formatter = ValueFormatter())
{
    return fixed_size_array_formatter<ValueFormatter, ArraySize>(value_formatter);
}

namespace binary
{

/// @brief fixed_size_array_formatter<ValueFormatter, ArraySize> is a verbatim formatter if ValueFormatter is.
template<typename ValueFormatter, int ArraySize, typename T>
struct is_verbatim_formatter< fixed_size_array_formatter<ValueFormatter, ArraySize>, T > : public is_verbatim_formatter<ValueFormatter, typename std::remove_pointer<typename std::decay<T>::type>::type>
{};

static_assert(is_verbatim_formatter< fixed_size_array_formatter< verbatim_formatter<2>, 10 >, uint16_t[10] >::value, "fixed_size_array_formatter<verbatim formatter> should be a verbatim formatter.");
static_assert(!is_verbatim_formatter< fixed_size_array_formatter< int, 10 >, uint16_t[10] >::value, "fixed_size_array_formatter<non-verbatim formatter> should not be a verbatim formatter.");

} // namespace binary

} // namespace arbitrary_format

#endif // ArbitraryFormatSerializer_fixed_size_array_formatter_H
