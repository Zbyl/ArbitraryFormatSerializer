/////////////////////////////////////////////////////////////////////////////
/// ArbitraryFormatSerializer
///    Library for serializing data in arbitrary formats.
///
/// element_formatter.h
///
/// This file contains element_formatter that formats object as element of specific name.
/// Element can be unnamed. Use then assign_name formatter to assign a name to it.
///
/// Distributed under Apache License, Version 2.0 (http://www.apache.org/licenses/LICENSE-2.0)
/// (c) 2016 Zbigniew Skowron, zbychs@gmail.com
///
/////////////////////////////////////////////////////////////////////////////

#ifndef ArbitraryFormatSerializer_element_formatter_H
#define ArbitraryFormatSerializer_element_formatter_H

#include <arbitrary_format/serialization_exceptions.h>
#include <arbitrary_format/utility/compile_time_string.h>

#include <string>
#include <vector>
#include <type_traits>

#include <boost/optional.hpp>

namespace arbitrary_format
{
namespace xml
{

template<typename ValueFormatter, bool Sequential = true>
class element_formatter_impl
{
private:
    boost::optional<std::string> name;
    ValueFormatter value_formatter;

public:
    explicit element_formatter_impl(const boost::optional<std::string>& name = boost::none, ValueFormatter value_formatter = ValueFormatter())
        : name(name)
        , value_formatter(value_formatter)
    {
    }

    explicit element_formatter_impl(const char* name, ValueFormatter value_formatter = ValueFormatter())
        : element_formatter_impl(std::string(name), value_formatter)
    {
    }

    template<typename T, typename XmlTree>
    void save(XmlTree& xmlTree, const T& object) const
    {
        auto element = xmlTree.addElement(name);
        value_formatter.save(element, object);
        if (!element.hasName())
        {
            BOOST_THROW_EXCEPTION(serialization_exception() << errinfo_description("Element must be named."));
        }
    }

    template<typename T, typename XmlTree>
    void load(XmlTree& xmlTree, T& object) const
    {
        auto element = xmlTree.eatElement(name, Sequential);
        value_formatter.load(element, object);
        if (!element.isEmpty())
        {
            BOOST_THROW_EXCEPTION(serialization_exception() << errinfo_description("Element not consumed completely."));
        }
        if (!name && element.hasName())
        {
            BOOST_THROW_EXCEPTION(serialization_exception() << errinfo_description("Element name must be consumed."));
        }
    }
};

template<typename CompileTimeString, typename ValueFormatter = assign_text_content<>, bool Sequential = true>
class element_formatter
{
private:
    static const element_formatter_impl<ValueFormatter, Sequential> formatter;
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

template<typename CompileTimeString, typename ValueFormatter, bool Sequential>
const element_formatter_impl<ValueFormatter, Sequential> element_formatter<CompileTimeString, ValueFormatter, Sequential>::formatter = 
    element_formatter_impl<ValueFormatter, Sequential>( boost::optional<std::string>(!compile_time_string_print<CompileTimeString>::str().empty(), compile_time_string_print<CompileTimeString>::str()) );

template<typename ValueFormatter = assign_text_content<>>
element_formatter_impl<ValueFormatter> create_element_formatter(const boost::optional<std::string>& name = boost::none, ValueFormatter value_formatter = ValueFormatter())
{
    return element_formatter_impl<ValueFormatter>(name, value_formatter);
}

template<typename ValueFormatter = assign_text_content<>>
element_formatter_impl<ValueFormatter> create_element_formatter(const char* name, ValueFormatter value_formatter = ValueFormatter())
{
    return create_element_formatter(std::string(name), value_formatter);
}

} // namespace xml
} // namespace arbitrary_format

#endif // ArbitraryFormatSerializer_element_formatter_H
