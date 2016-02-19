/////////////////////////////////////////////////////////////////////////////
/// ArbitraryFormatSerializer
///    Library for serializing data in arbitrary formats.
///
/// map_formatter.h
///
/// This file contains map_formatter that formats std::map as a length field followed by keys and values.
///
/// Distributed under Apache License, Version 2.0 (http://www.apache.org/licenses/LICENSE-2.0)
/// (c) 2014 Zbigniew Skowron, zbychs@gmail.com
///
/////////////////////////////////////////////////////////////////////////////

#ifndef ArbitraryFormatSerializer_map_formatter_H
#define ArbitraryFormatSerializer_map_formatter_H

#include <map>

namespace arbitrary_format
{

template<typename SizeFormatter, typename KeyFormatter, typename ValueFormatter>
class map_formatter
{
    SizeFormatter size_formatter;
    KeyFormatter key_formatter;
    ValueFormatter value_formatter;

public:
    map_formatter(SizeFormatter size_formatter = SizeFormatter(), KeyFormatter key_formatter = KeyFormatter(), ValueFormatter value_formatter = ValueFormatter())
        : size_formatter(size_formatter)
        , key_formatter(key_formatter)
        , value_formatter(value_formatter)
    {
    }

    template<typename KeyType, typename ValueType, typename TSerializer>
    void save(TSerializer& serializer, const std::map<KeyType, ValueType>& map) const
    {
        size_formatter.save(serializer, map.size());
        for (auto& kv : map)
        {
            key_formatter.save(serializer, kv.first);
            value_formatter.save(serializer, kv.second);
        }
    }

    template<typename KeyType, typename ValueType, typename TSerializer>
    void load(TSerializer& serializer, std::map<KeyType, ValueType>& map) const
    {
        map.clear();
        size_t map_size;
        size_formatter.load(serializer, map_size);
        for (size_t i = 0; i < map_size; ++i)
        {
            KeyType key;
            key_formatter.load(serializer, key);
            value_formatter.load(serializer, map[key]);
        }
    }
};

template<typename SizeFormatter, typename KeyFormatter, typename ValueFormatter>
map_formatter<SizeFormatter, KeyFormatter, ValueFormatter> create_map_formatter(SizeFormatter size_formatter = SizeFormatter(), KeyFormatter key_formatter = KeyFormatter(), ValueFormatter value_formatter = ValueFormatter())
{
    return map_formatter<SizeFormatter, KeyFormatter, ValueFormatter>(size_formatter, key_formatter, value_formatter);
}

} // namespace arbitrary_format

#endif // ArbitraryFormatSerializer_map_formatter_H
