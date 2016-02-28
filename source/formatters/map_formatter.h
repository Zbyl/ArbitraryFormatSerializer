/////////////////////////////////////////////////////////////////////////////
/// ArbitraryFormatSerializer
///    Library for serializing data in arbitrary formats.
///
/// map_formatter.h
///
/// This file contains map_formatter that formats std::map and std::multimap as a length field followed by keys and values.
///
/// Distributed under Apache License, Version 2.0 (http://www.apache.org/licenses/LICENSE-2.0)
/// (c) 2014 Zbigniew Skowron, zbychs@gmail.com
///
/////////////////////////////////////////////////////////////////////////////

#ifndef ArbitraryFormatSerializer_map_formatter_H
#define ArbitraryFormatSerializer_map_formatter_H

#include "collection_formatter.h"
#include "pair_formatter.h"

namespace arbitrary_format
{

template<typename SizeFormatter, typename KeyFormatter, typename ValueFormatter>
using map_formatter = collection_formatter<SizeFormatter, pair_formatter<KeyFormatter, ValueFormatter>>;

template<typename SizeFormatter, typename KeyFormatter, typename ValueFormatter>
map_formatter<SizeFormatter, KeyFormatter, ValueFormatter> create_map_formatter(SizeFormatter size_formatter = SizeFormatter(), KeyFormatter key_formatter = KeyFormatter(), ValueFormatter value_formatter = ValueFormatter())
{
    return create_collection_formatter(size_formatter, create_pair_formatter(key_formatter, value_formatter));
}

} // namespace arbitrary_format

#endif // ArbitraryFormatSerializer_map_formatter_H
