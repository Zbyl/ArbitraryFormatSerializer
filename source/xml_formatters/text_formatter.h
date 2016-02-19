/////////////////////////////////////////////////////////////////////////////
/// ArbitraryFormatSerializer
///    Library for serializing data in arbitrary formats.
///
/// text_formatter.h
///
/// This file contains text_formatter that formats std::vector as length field followed by individual values.
///
/// Distributed under Apache License, Version 2.0 (http://www.apache.org/licenses/LICENSE-2.0)
/// (c) 2016 Zbigniew Skowron, zbychs@gmail.com
///
/////////////////////////////////////////////////////////////////////////////

#ifndef ArbitraryFormatSerializer_text_formatter_H
#define ArbitraryFormatSerializer_text_formatter_H

#include "lexical_stringizer.h"

#include <string>
#include <vector>
#include <type_traits>

namespace arbitrary_format
{
namespace xml
{

template<typename ValueStringizer = lexical_stringizer>
class text_formatter
{
    ValueStringizer value_stringizer;

public:
    explicit text_formatter(ValueStringizer value_stringizer = ValueStringizer())
        : value_stringizer(value_stringizer)
    {
    }

    template<typename T, typename XmlTree>
    void save(XmlTree& xmlTree, const T& object) const
    {
        std::string value;
        value_stringizer.save(value, object);
        xmlTree.setTextContent(value);
    }

    template<typename T, typename XmlTree>
    void load(XmlTree& xmlTree, T& object) const
    {
        auto value = xmlTree.eatTextContent();
        value_stringizer.load(value, object);
    }
};

template<typename ValueStringizer = lexical_stringizer<>>
text_formatter<ValueStringizer> create_text_formatter(ValueStringizer value_stringizer = ValueStringizer())
{
    return text_formatter<ValueStringizer>(value_stringizer);
}

} // namespace xml
} // namespace arbitrary_format

#endif // ArbitraryFormatSerializer_text_formatter_H
