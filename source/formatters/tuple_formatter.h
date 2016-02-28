/////////////////////////////////////////////////////////////////////////////
/// ArbitraryFormatSerializer
///    Library for serializing data in arbitrary formats.
///
/// tuple_formatter.h
///
/// This file contains tuple_formatter that formats std::tuple and std::pair as a sequence of values.
///
/// Distributed under Apache License, Version 2.0 (http://www.apache.org/licenses/LICENSE-2.0)
/// (c) 2014 Zbigniew Skowron, zbychs@gmail.com
///
/////////////////////////////////////////////////////////////////////////////

#ifndef ArbitraryFormatSerializer_tuple_formatter_H
#define ArbitraryFormatSerializer_tuple_formatter_H

namespace arbitrary_format
{

template<size_t Idx, typename... ValueFormatters>
class tuple_formatter_impl;

template<size_t Idx>
class tuple_formatter_impl<Idx>
{
public:
    template<typename Tuple, typename TSerializer>
    void save(TSerializer& serializer, const Tuple& tuple) const
    {
        // nothing to do
    }

    template<typename Tuple, typename TSerializer>
    void load(TSerializer& serializer, Tuple& tuple) const
    {
        // nothing to do
    }

    /// @note This overload is to support std::tie seamlessly. This, unfortunately won't work if the tuple has to be passed from another formatter.
    template<typename Tuple, typename TSerializer>
    void load(TSerializer& serializer, const Tuple& tuple) const
    {
        // nothing to do
    }
};

template<size_t Idx, typename ValueFormatter, typename... ValueFormatters>
class tuple_formatter_impl<Idx, ValueFormatter, ValueFormatters...>
{
    ValueFormatter value_formatter;
    tuple_formatter_impl<Idx + 1, ValueFormatters...> tail_formatter;

public:
    tuple_formatter_impl(ValueFormatter value_formatter = ValueFormatter(), ValueFormatters... value_formatters)
        : value_formatter(value_formatter)
        , tail_formatter(value_formatters...)
    {
    }

    template<typename Tuple, typename TSerializer>
    void save(TSerializer& serializer, const Tuple& tuple) const
    {
        value_formatter.save(serializer, std::get<Idx>(tuple));
        tail_formatter.save(serializer, tuple);
    }

    template<typename Tuple, typename TSerializer>
    void load(TSerializer& serializer, Tuple& tuple) const
    {
        value_formatter.load(serializer, std::get<Idx>(tuple));
        tail_formatter.load(serializer, tuple);
    }

    /// @note This overload is to support std::tie seamlessly. This, unfortunately won't work if the tuple has to be passed from another formatter.
    template<typename Tuple, typename TSerializer>
    void load(TSerializer& serializer, const Tuple& tuple) const
    {
        value_formatter.load(serializer, std::get<Idx>(tuple));
        tail_formatter.load(serializer, tuple);
    }
};

template<typename... ValueFormatters>
using tuple_formatter = tuple_formatter_impl<0, ValueFormatters...>;

template<typename... ValueFormatters>
tuple_formatter<ValueFormatters...> create_tuple_formatter(ValueFormatters... value_formatters)
{
    return tuple_formatter<ValueFormatters...>(value_formatters...);
}

} // namespace arbitrary_format

#endif // ArbitraryFormatSerializer_tuple_formatter_H
