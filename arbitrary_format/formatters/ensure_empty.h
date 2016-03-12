/////////////////////////////////////////////////////////////////////////////
/// ArbitraryFormatSerializer
///    Library for serializing data in arbitrary formats.
///
/// ensure_empty.h
///
/// This file contains ensure_empty that verifies if value is empty when saving, and clears value when loading.
/// This is useful for stubbing out serialization of complex structures.
///
/// Distributed under Apache License, Version 2.0 (http://www.apache.org/licenses/LICENSE-2.0)
/// (c) 2016 Zbigniew Skowron, zbychs@gmail.com
///
/////////////////////////////////////////////////////////////////////////////

#ifndef ArbitraryFormatSerializer_ensure_empty_H
#define ArbitraryFormatSerializer_ensure_empty_H

#include <arbitrary_format/serialization_exceptions.h>
#include <arbitrary_format/formatters/const_formatter.h>

namespace arbitrary_format
{

class ensure_empty
{
public:
    /// @brief Verifies that value is empty().
    ///        Throws lossy_conversion if the value is not empty().
    template<typename ValueType, typename TSerializer>
    void save(TSerializer& serializer, const ValueType& value) const
    {
        if (!value.empty())
        {
            BOOST_THROW_EXCEPTION(lossy_conversion());
        }
    }

    /// @brief Calls clear() on the value.
    template<typename ValueType, typename TSerializer>
    void load(TSerializer& serializer, ValueType& value) const
    {
        value.clear();
    }
};

ensure_empty create_ensure_empty()
{
    return ensure_empty();
}

} // namespace arbitrary_format

#endif // ArbitraryFormatSerializer_ensure_empty_H
