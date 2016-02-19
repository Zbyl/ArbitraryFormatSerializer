/////////////////////////////////////////////////////////////////////////////
/// ArbitraryFormatSerializer
///    Library for serializing data in arbitrary formats.
///
/// optional_formatter.h
///
/// This file contains optional_formatter that formats boost::optional as a flag field followed by the value.
///
/// Distributed under Apache License, Version 2.0 (http://www.apache.org/licenses/LICENSE-2.0)
/// (c) 2014 Zbigniew Skowron, zbychs@gmail.com
///
/////////////////////////////////////////////////////////////////////////////

#ifndef ArbitraryFormatSerializer_optional_formatter_H
#define ArbitraryFormatSerializer_optional_formatter_H

#include <boost/optional.hpp>

namespace arbitrary_format
{

template<typename FlagFormatter, typename ValueFormatter>
class optional_formatter
{
    FlagFormatter flag_formatter;
    ValueFormatter value_formatter;

public:
    optional_formatter(FlagFormatter flag_formatter = FlagFormatter(), ValueFormatter value_formatter = ValueFormatter())
        : flag_formatter(flag_formatter)
        , value_formatter(value_formatter)
    {
    }

    template<typename ValueType, typename TSerializer>
    void save(TSerializer& serializer, const boost::optional<ValueType>& value) const
    {
        flag_formatter.save(serializer, value.is_initialized());
        if (value)
        {
            value_formatter.save(serializer, *value);
        }
    }

    template<typename ValueType, typename TSerializer>
    void load(TSerializer& serializer, boost::optional<ValueType>& value) const
    {
        value.reset();
        bool value_flag;
        flag_formatter.load(serializer, value_flag);
        if (value_flag)
        {
            value = ValueType();
            value_formatter.load(serializer, *value);
        }
    }
};

template<typename FlagFormatter, typename ValueFormatter>
optional_formatter<FlagFormatter, ValueFormatter> create_optional_formatter(FlagFormatter flag_formatter = FlagFormatter(), ValueFormatter value_formatter = ValueFormatter())
{
    return optional_formatter<FlagFormatter, ValueFormatter>(flag_formatter, value_formatter);
}

} // namespace arbitrary_format

#endif // ArbitraryFormatSerializer_optional_formatter_H
