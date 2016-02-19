/////////////////////////////////////////////////////////////////////////////
/// ArbitraryFormatSerializer
///    Library for serializing data in arbitrary formats.
///
/// assign_name.h
///
/// This file contains assign_name that formats object as name of xml element.
///
/// Distributed under Apache License, Version 2.0 (http://www.apache.org/licenses/LICENSE-2.0)
/// (c) 2016 Zbigniew Skowron, zbychs@gmail.com
///
/////////////////////////////////////////////////////////////////////////////

#ifndef ArbitraryFormatSerializer_assign_name_H
#define ArbitraryFormatSerializer_assign_name_H

#include "lexical_stringizer.h"

#include <string>

namespace arbitrary_format
{
namespace xml
{

template<typename ValueStringizer = lexical_stringizer>
class assign_name
{
    ValueStringizer value_stringizer;

public:
    explicit assign_name(ValueStringizer value_stringizer = ValueStringizer())
        : value_stringizer(value_stringizer)
    {
    }

    template<typename T, typename XmlTreeOrAttrib>
    void save(XmlTreeOrAttrib& xmlTreeOrAttrib, const T& object) const
    {
        std::string value;
        value_stringizer.save(value, object);
        xmlTreeOrAttrib.setName(value);
    }

    template<typename T, typename XmlTreeOrAttrib>
    void load(XmlTreeOrAttrib& xmlTreeOrAttrib, T& object) const
    {
        auto value = xmlTreeOrAttrib.eatName();
        value_stringizer.load(value, object);
    }
};

template<typename ValueStringizer = lexical_stringizer<>>
assign_name<ValueStringizer> create_assign_name(ValueStringizer value_stringizer = ValueStringizer())
{
    return assign_name<ValueStringizer>(value_stringizer);
}

} // namespace xml
} // namespace arbitrary_format

#endif // ArbitraryFormatSerializer_assign_name_H
