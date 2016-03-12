/////////////////////////////////////////////////////////////////////////////
/// ArbitraryFormatSerializer
///    Library for serializing data in arbitrary formats.
///
/// array_formatter.h
///
/// This file contains array_formatter that formats an array*, array[] or std::array as a sequence of elements. Array length is not serialized.
///
/// Distributed under Apache License, Version 2.0 (http://www.apache.org/licenses/LICENSE-2.0)
/// (c) 2014 Zbigniew Skowron, zbychs@gmail.com
///
/////////////////////////////////////////////////////////////////////////////

#ifndef ArbitraryFormatSerializer_array_formatter_H
#define ArbitraryFormatSerializer_array_formatter_H

#include <arbitrary_format/formatters/serialize_buffer.h>
#include <arbitrary_format/binary_formatters/verbatim_formatter.h>

#include <array>
#include <type_traits>

namespace arbitrary_format
{

template<typename ValueFormatter, int ArraySize = -1>
class array_formatter
{
    ValueFormatter value_formatter;

public:
    array_formatter(ValueFormatter value_formatter = ValueFormatter())
        : value_formatter(value_formatter)
    {
    }

    template<typename ValueType, typename TSerializer>
    void save(TSerializer& serializer, const ValueType *const array) const
    {
        static_assert(ArraySize >= 0, "Array size must not be negative.");
        save_buffer(serializer, ArraySize, array, value_formatter);
    }

    template<typename ValueType, typename TSerializer>
    void load(TSerializer& serializer, ValueType *const array) const
    {
        static_assert(ArraySize >= 0, "Array size must not be negative.");
        load_buffer(serializer, ArraySize, array, value_formatter);
    }
};

template<typename ValueFormatter>
class array_formatter<ValueFormatter, -1>
{
    ValueFormatter value_formatter;

public:
    explicit array_formatter(ValueFormatter value_formatter = ValueFormatter())
        : value_formatter(value_formatter)
    {
    }

    template<int Size, typename ValueType, typename TSerializer>
    void save(TSerializer& serializer, ValueType (&array)[Size]) const
    {
        save_buffer(serializer, Size, array, value_formatter);
    }

    template<int Size, typename ValueType, typename TSerializer>
    void save(TSerializer& serializer, const std::array<ValueType, Size>& array) const
    {
        save_buffer(serializer, Size, array.data(), value_formatter);
    }

    template<int Size, typename ValueType, typename TSerializer>
    void load(TSerializer& serializer, ValueType (&array)[Size]) const
    {
        load_buffer(serializer, Size, array, value_formatter);
    }

    template<int Size, typename ValueType, typename TSerializer>
    void load(TSerializer& serializer, std::array<ValueType, Size>& array) const
    {
        load_buffer(serializer, Size, array.data(), value_formatter);
    }
};


template<typename ValueFormatter, int ArraySize = -1>
array_formatter<ValueFormatter, ArraySize> create_array_formatter(ValueFormatter value_formatter = ValueFormatter())
{
    return array_formatter<ValueFormatter, ArraySize>(value_formatter);
}

namespace binary
{

/// @brief array_formatter<ValueFormatter, -1> is a verbatim formatter if ValueFormatter is.
///        array_formatter<ValueFormatter, ArraySize> is a verbatim formatter if ValueFormatter is and if ArraySize equals to size of array serialized.
template<typename ValueFormatter, int ArraySize, typename T>
struct declare_verbatim_formatter< array_formatter<ValueFormatter, -1>, T[ArraySize] > : public is_verbatim_formatter<ValueFormatter, typename std::remove_pointer<typename std::decay<T>::type>::type>
{};

template<typename ValueFormatter, int ArraySize, typename T>
struct declare_verbatim_formatter< array_formatter<ValueFormatter, -1>, std::array<T, ArraySize> > : public is_verbatim_formatter<ValueFormatter, typename std::remove_pointer<typename std::decay<T>::type>::type>
{};

template<typename ValueFormatter, int ArraySize, typename T>
struct declare_verbatim_formatter< array_formatter<ValueFormatter, ArraySize>, T[ArraySize] > : public is_verbatim_formatter<ValueFormatter, typename std::remove_pointer<typename std::decay<T>::type>::type>
{};

template<typename ValueFormatter, int ArraySize, typename T>
struct declare_verbatim_formatter< array_formatter<ValueFormatter, ArraySize>, std::array<T, ArraySize> > : public is_verbatim_formatter<ValueFormatter, typename std::remove_pointer<typename std::decay<T>::type>::type>
{};

static_assert(is_verbatim_formatter< array_formatter< verbatim_formatter<2>, 10 >, uint16_t[10] >::value, "array_formatter<verbatim formatter> should be a verbatim formatter.");
#ifndef __COVERITY__
/// @note For some reason CoverityScan doesn't like this assert.
static_assert(is_verbatim_formatter< array_formatter< verbatim_formatter<2>, 10 >, std::array<uint16_t, 10> >::value, "array_formatter<verbatim formatter> should be a verbatim formatter.");
#endif
static_assert(!is_verbatim_formatter< array_formatter< int, 10 >, uint16_t[10] >::value, "array_formatter<non-verbatim formatter> should not be a verbatim formatter.");
static_assert(!is_verbatim_formatter< array_formatter< verbatim_formatter<2>, 5 >, uint16_t[10] >::value, "array_formatter<verbatim formatter> should not be a verbatim formatter if we format part of the array.");
static_assert(!is_verbatim_formatter< array_formatter< int, 5 >, uint16_t[10] >::value, "array_formatter<non-verbatim formatter> should not be a verbatim formatter.");
static_assert(!is_verbatim_formatter< array_formatter< verbatim_formatter<2>, 5 >, uint16_t* >::value, "array_formatter<verbatim formatter> should not be a verbatim formatter if we format type other than an array.");

} // namespace binary

} // namespace arbitrary_format

#endif // ArbitraryFormatSerializer_array_formatter_H
