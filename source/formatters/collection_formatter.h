/////////////////////////////////////////////////////////////////////////////
/// ArbitraryFormatSerializer
///    Library for serializing data in arbitrary formats.
///
/// collection_formatter.h
///
/// This file contains collection_formatter that formats collections, such as std::map, vector, set etc. as a length field followed by values.
/// Collection type needs to have the following members:
///   begin(), end(), size(), clear(), insert(iterator, value), value_type
/// For std::vector a more optimized formatter is available: vector_formatter
///
/// Distributed under Apache License, Version 2.0 (http://www.apache.org/licenses/LICENSE-2.0)
/// (c) 2014 Zbigniew Skowron, zbychs@gmail.com
///
/////////////////////////////////////////////////////////////////////////////

#ifndef ArbitraryFormatSerializer_collection_formatter_H
#define ArbitraryFormatSerializer_collection_formatter_H

#include <map>
#include <set>
#include <utility>

namespace arbitrary_format
{

/// @note This structure returns a mutable version of the collection's value type.
///       It is needed to support maps and multimaps, whose value types are std::pair<const Key, Value>.
template<typename Collection>
struct collection_mutable_value_type
{
    using type = typename Collection::value_type;
};

template<typename KeyType, typename ValueType>
struct collection_mutable_value_type< std::map<KeyType, ValueType> >
{
    using type = std::pair<KeyType, ValueType>;
};

template<typename KeyType, typename ValueType>
struct collection_mutable_value_type< std::multimap<KeyType, ValueType> >
{
    using type = std::pair<KeyType, ValueType>;
};

template<typename SizeFormatter, typename ValueFormatter>
class collection_formatter
{
    SizeFormatter size_formatter;
    ValueFormatter value_formatter;

public:
    collection_formatter(SizeFormatter size_formatter = SizeFormatter(), ValueFormatter value_formatter = ValueFormatter())
        : size_formatter(size_formatter)
        , value_formatter(value_formatter)
    {
    }

    template<typename Collection, typename TSerializer>
    void save(TSerializer& serializer, const Collection& collection) const
    {
        size_formatter.save(serializer, collection.size());
        for (auto& value : collection)
        {
            value_formatter.save(serializer, value);
        }
    }

    template<typename Collection, typename TSerializer>
    void load(TSerializer& serializer, Collection& collection) const
    {
        collection.clear();
        size_t collection_size;
        size_formatter.load(serializer, collection_size);
        for (size_t i = 0; i < collection_size; ++i)
        {
            typename collection_mutable_value_type<Collection>::type value;
            value_formatter.load(serializer, value);
            collection.insert(collection.end(), value);
        }
    }
};

template<typename SizeFormatter, typename ValueFormatter>
collection_formatter<SizeFormatter, ValueFormatter> create_collection_formatter(SizeFormatter size_formatter = SizeFormatter(), ValueFormatter value_formatter = ValueFormatter())
{
    return collection_formatter<SizeFormatter, ValueFormatter>(size_formatter, value_formatter);
}

} // namespace arbitrary_format

#endif // ArbitraryFormatSerializer_collection_formatter_H
