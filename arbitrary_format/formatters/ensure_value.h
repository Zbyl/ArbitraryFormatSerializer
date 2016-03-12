/////////////////////////////////////////////////////////////////////////////
/// ArbitraryFormatSerializer
///    Library for serializing data in arbitrary formats.
///
/// ensure_value.h
///
/// This file contains ensure_value that verifies if value being serialized is equal to the one expected.
/// This is useful for stubbing out serialization of complex structures.
///
/// Distributed under Apache License, Version 2.0 (http://www.apache.org/licenses/LICENSE-2.0)
/// (c) 2016 Zbigniew Skowron, zbychs@gmail.com
///
/////////////////////////////////////////////////////////////////////////////

#ifndef ArbitraryFormatSerializer_ensure_value_H
#define ArbitraryFormatSerializer_ensure_value_H

#include <arbitrary_format/serialization_exceptions.h>
#include <arbitrary_format/formatters/const_formatter.h>

namespace arbitrary_format
{

template<typename Value>
class ensure_value
{
    Value storedValue;

public:
    explicit ensure_value(Value storedValue = Value())
        : storedValue(storedValue)
    {
    }

    /// @brief Verifies that value it equal to the stored one.
    ///        Throws lossy_conversion if the value being saved is not equal to the expected value.
    /// @note  Comparison is done by the means of "==" operator, and using boost::equal() for arrays.
    template<typename ValueType, typename TSerializer>
    void save(TSerializer& serializer, const ValueType& value) const
    {
        if (!detail::const_formatter_equals(value, storedValue))
        {
            BOOST_THROW_EXCEPTION(lossy_conversion());
        }
    }

    /// @brief Sets value to the stored reference value.
    /// @note  Assignment is done by the means of "=" operator.
    /// @todo  Fix it for arrays.
    template<typename ValueType, typename TSerializer>
    void load(TSerializer& serializer, ValueType& value) const
    {
        value = storedValue;
    }
};

template<typename Value>
ensure_value<Value> create_ensure_value(Value storedValue = Value())
{
    return ensure_value<Value>(storedValue);
}

} // namespace arbitrary_format

#endif // ArbitraryFormatSerializer_ensure_value_H
