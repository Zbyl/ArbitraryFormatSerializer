/////////////////////////////////////////////////////////////////////////////
/// ArbitraryFormatSerializer
///    Library for serializing data in arbitrary formats.
///
/// assign_text_content.h
///
/// This file contains assign_text_content that formats object as text content of an xml element or attribute.
///
/// Distributed under Apache License, Version 2.0 (http://www.apache.org/licenses/LICENSE-2.0)
/// (c) 2016 Zbigniew Skowron, zbychs@gmail.com
///
/////////////////////////////////////////////////////////////////////////////

#ifndef ArbitraryFormatSerializer_assign_text_content_H
#define ArbitraryFormatSerializer_assign_text_content_H

#include "lexical_stringizer.h"

#include <string>

namespace arbitrary_format
{
namespace xml
{

template<typename ValueStringizer = lexical_stringizer>
class assign_text_content
{
    ValueStringizer value_stringizer;

public:
    explicit assign_text_content(ValueStringizer value_stringizer = ValueStringizer())
        : value_stringizer(value_stringizer)
    {
    }

    template<typename T, typename XmlTreeOrAttrib>
    void save(XmlTreeOrAttrib& xmlTreeOrAttrib, const T& object) const
    {
        std::string value;
        value_stringizer.save(value, object);
        xmlTreeOrAttrib.setTextContent(value);
    }

    template<typename T, typename XmlTreeOrAttrib>
    void load(XmlTreeOrAttrib& xmlTreeOrAttrib, T& object) const
    {
        auto value = xmlTreeOrAttrib.eatTextContent();
        value_stringizer.load(value, object);
    }
};

template<typename ValueStringizer = lexical_stringizer>
assign_text_content<ValueStringizer> create_assign_text_content(ValueStringizer value_stringizer = ValueStringizer())
{
    return assign_text_content<ValueStringizer>(value_stringizer);
}

} // namespace xml
} // namespace arbitrary_format

#endif // ArbitraryFormatSerializer_assign_text_content_H 
