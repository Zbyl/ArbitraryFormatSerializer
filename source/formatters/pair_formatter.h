/////////////////////////////////////////////////////////////////////////////
/// ArbitraryFormatSerializer
///    Library for serializing data in arbitrary formats.
///
/// pair_formatter.h
///
/// This file contains pair_formatter that formats std::pair as a first value followed by second value.
///
/// Distributed under Apache License, Version 2.0 (http://www.apache.org/licenses/LICENSE-2.0)
/// (c) 2014 Zbigniew Skowron, zbychs@gmail.com
///
/////////////////////////////////////////////////////////////////////////////

#ifndef ArbitraryFormatSerializer_pair_formatter_H
#define ArbitraryFormatSerializer_pair_formatter_H

namespace arbitrary_format
{

/// @note This formatter could be a typedef for tuple_formatter, but isn't to make debugging more straightforward.
template<typename FirstFormatter, typename SecondFormatter>
class pair_formatter
{
    FirstFormatter first_formatter;
    SecondFormatter second_formatter;

public:
    pair_formatter(FirstFormatter first_formatter = FirstFormatter(), SecondFormatter second_formatter = SecondFormatter())
        : first_formatter(first_formatter)
        , second_formatter(second_formatter)
    {
    }

    template<typename Pair, typename TSerializer>
    void save(TSerializer& serializer, const Pair& pair) const
    {
        first_formatter.save(serializer, pair.first);
        second_formatter.save(serializer, pair.second);
    }

    template<typename Pair, typename TSerializer>
    void load(TSerializer& serializer, Pair& pair) const
    {
        first_formatter.load(serializer, pair.first);
        second_formatter.load(serializer, pair.second);
    }
};

template<typename FirstFormatter, typename SecondFormatter>
pair_formatter<FirstFormatter, SecondFormatter> create_pair_formatter(FirstFormatter first_formatter = FirstFormatter(), SecondFormatter second_formatter = SecondFormatter())
{
    return pair_formatter<FirstFormatter, SecondFormatter>(first_formatter, second_formatter);
}

} // namespace arbitrary_format

#endif // ArbitraryFormatSerializer_pair_formatter_H
