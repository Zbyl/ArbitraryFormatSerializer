/////////////////////////////////////////////////////////////////////////////
/// ArbitraryFormatSerializer
///    Library for serializing data in arbitrary formats.
///
/// attribute_formatter.h
///
/// This file contains attribute_formatter that formats object as attribute of specific name.
/// Attribute can be unnamed. Use then assign_name formatter to assign a name to it.
///
/// Distributed under Apache License, Version 2.0 (http://www.apache.org/licenses/LICENSE-2.0)
/// (c) 2016 Zbigniew Skowron, zbychs@gmail.com
///
/////////////////////////////////////////////////////////////////////////////

#ifndef ArbitraryFormatSerializer_attribute_formatter_H
#define ArbitraryFormatSerializer_attribute_formatter_H

#include "assign_text_content.h"
#include "utility/compile_time_string.h"

#include <string>

#include <boost/optional.hpp>

namespace arbitrary_format
{
namespace xml
{

template<typename ValueFormatter, bool Sequential = false>
class attribute_formatter_impl
{
private:
    boost::optional<std::string> name;
    ValueFormatter value_formatter;

public:
    explicit attribute_formatter_impl(const boost::optional<std::string>& name = boost::none, ValueFormatter value_formatter = ValueFormatter())
        : name(name)
        , value_formatter(value_formatter)
    {
    }

    explicit attribute_formatter_impl(const char* name, ValueFormatter value_formatter = ValueFormatter())
        : attribute_formatter_impl(std::string(name), value_formatter)
    {
    }

    template<typename T, typename XmlTree>
    void save(XmlTree& xmlTree, const T& object) const
    {
        auto attribute = xmlTree.addAttribute(name);
        value_formatter.save(attribute, object);
        if (!attribute.hasName())
        {
            BOOST_THROW_EXCEPTION(serialization_exception() << detail::errinfo_description("Attribute must be named."));
        }
    }

    template<typename T, typename XmlTree>
    void load(XmlTree& xmlTree, T& object) const
    {
        auto attribute = xmlTree.eatAttribute(name, Sequential);
        value_formatter.load(attribute, object);
        if (attribute.hasTextContent())
        {
            BOOST_THROW_EXCEPTION(serialization_exception() << detail::errinfo_description("Attribute value not consumed."));
        }
        if (!name && attribute.hasName())
        {
            BOOST_THROW_EXCEPTION(serialization_exception() << detail::errinfo_description("Attribute name must be consumed."));
        }
    }
};

template<typename CompileTimeString, typename ValueFormatter = assign_text_content<>, bool Sequential = true>
class attribute_formatter
{
private:
    static const attribute_formatter_impl<ValueFormatter, Sequential> formatter;
public:
    template<typename T, typename XmlTree>
    void save(XmlTree& xmlTree, const T& object) const
    {
        formatter.save(xmlTree, object);
    }

    template<typename T, typename XmlTree>
    void load(XmlTree& xmlTree, T& object) const
    {
        formatter.load(xmlTree, object);
    }
};

template<typename CompileTimeString, typename ValueFormatter = assign_text_content<>, bool Sequential = true>
const attribute_formatter_impl<ValueFormatter, Sequential> attribute_formatter<CompileTimeString, ValueFormatter, Sequential>::formatter = 
    attribute_formatter_impl<ValueFormatter, Sequential>( boost::optional<std::string>(!CompileTimeString::empty::value, compile_time_string_print<CompileTimeString>::str()) );

template<typename ValueFormatter = assign_text_content<>>
attribute_formatter_impl<ValueFormatter> create_attribute_formatter(const boost::optional<std::string>& name = boost::none, ValueFormatter value_formatter = ValueFormatter())
{
    return attribute_formatter_impl<ValueFormatter>(name, value_formatter);
}

template<typename ValueFormatter = assign_text_content<>>
attribute_formatter_impl<ValueFormatter> create_attribute_formatter(const char* name, ValueFormatter value_formatter = ValueFormatter())
{
    return create_attribute_formatter(std::string(name), value_formatter);
}

} // namespace xml
} // namespace arbitrary_format

#endif // ArbitraryFormatSerializer_attribute_formatter_H
