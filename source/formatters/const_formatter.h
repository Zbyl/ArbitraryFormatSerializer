/////////////////////////////////////////////////////////////////////////////
/// ArbitraryFormatSerializer
///    Library for serializing data in arbitrary formats.
///
/// const_formatter.h
///
/// This file contains const_formatter that verifies if deserialized value is equal to the one expected.
/// This is useful for serializing magic strings or version numbers.
///
/// Distributed under Apache License, Version 2.0 (http://www.apache.org/licenses/LICENSE-2.0)
/// (c) 2014 Zbigniew Skowron, zbychs@gmail.com
///
/////////////////////////////////////////////////////////////////////////////

#ifndef ArbitraryFormatSerializer_const_formatter_H
#define ArbitraryFormatSerializer_const_formatter_H

#include "serialization_exceptions.h"
#include "binary_formatters/verbatim_formatter.h"

#include <boost/type_traits/remove_const.hpp>
#include <boost/range/algorithm/equal.hpp>

namespace arbitrary_format
{

namespace detail
{

template<typename T>
struct loaded_value_different_from_expected_value
{
    typedef boost::error_info<struct tag_loaded_value_different, T> errinfo_loaded;
    typedef boost::error_info<struct tag_from_expected_value, T> errinfo_expected;
};

template<typename T>
bool const_formatter_equals(const T& value0, const T& value1)
{
    return value0 == value1;
}

template<typename T, int Size>
bool const_formatter_equals(const T (&array0)[Size], const T (&array1)[Size])
{
    return boost::equal(array0, array1);
}

} // namespace detail

template<typename ValueFormatter>
class const_formatter
{
    ValueFormatter value_formatter;

public:
    explicit const_formatter(ValueFormatter value_formatter = ValueFormatter())
        : value_formatter(value_formatter)
    {
    }

    template<typename ValueType, typename TSerializer>
    void save(TSerializer& serializer, const ValueType& value) const
    {
        value_formatter.save(serializer, value);
    }

    /// @brief Loads a value and verifies that it is equal to the provided one.
    ///        Throws invalid_data if the loaded value is not equal to the expected value.
    /// @note  Comparison is done by the means of "==" operator, and using boost::equal() for arrays.
    template<typename ValueType, typename TSerializer>
    void load(TSerializer& serializer, const ValueType& expected_value) const
    {
        typename boost::remove_const<ValueType>::type loaded_value;
        value_formatter.load(serializer, loaded_value);
        if (!detail::const_formatter_equals(loaded_value, expected_value))
        {
            BOOST_THROW_EXCEPTION(invalid_data());
#if 0
            BOOST_THROW_EXCEPTION(invalid_data()
                // doesn't work for arrays in Visual Studio 2013
                << detail::loaded_value_different_from_expected_value<ValueType>::errinfo_loaded(loaded_value)
                << detail::loaded_value_different_from_expected_value<ValueType>::errinfo_expected(expected_value)
            );
#endif
        }
    }
};

template<typename ValueFormatter>
const_formatter<ValueFormatter> create_const_formatter(ValueFormatter value_formatter = ValueFormatter())
{
    return const_formatter<ValueFormatter>(value_formatter);
}

namespace binary
{

/// @brief const_formatter<ValueFormatter> is a verbatim formatter if ValueFormatter is.
template<typename ValueFormatter, typename T>
struct is_verbatim_formatter< const_formatter<ValueFormatter>, T > : public is_verbatim_formatter<ValueFormatter, T>
{};

static_assert(is_verbatim_formatter< const_formatter< verbatim_formatter<4> >, uint32_t >::value, "const_formatter<verbatim formatter> should be a verbatim formatter.");
static_assert(!is_verbatim_formatter< const_formatter< int >, uint32_t >::value, "const_formatter<non-verbatim formatter> should not be a verbatim formatter.");

} // namespace binary

} // namespace arbitrary_format

#endif // ArbitraryFormatSerializer_const_formatter_H
