/////////////////////////////////////////////////////////////////////////////
/// ArbitraryFormatSerializer
///    Library for serializing data in arbitrary formats.
///
/// attribute_counter.h
///
/// This file contains attribute_counter that formats object as the count of attributes of xml element.
/// This counter can count all attributes or attributes with specific name.
///
/// Distributed under Apache License, Version 2.0 (http://www.apache.org/licenses/LICENSE-2.0)
/// (c) 2016 Zbigniew Skowron, zbychs@gmail.com
///
/////////////////////////////////////////////////////////////////////////////

#ifndef ArbitraryFormatSerializer_attribute_counter_H
#define ArbitraryFormatSerializer_attribute_counter_H

#include <string>

#include <boost/optional.hpp>

namespace arbitrary_format
{
namespace xml
{

class attribute_counter_impl
{
private:
    boost::optional<std::string> name;

public:
    explicit attribute_counter_impl(const boost::optional<std::string>& name = boost::none)
        : name(name)
    {
    }

    explicit attribute_counter_impl(const char* name)
        : attribute_counter_impl(std::string(name))
    {
    }

    template<typename T, typename XmlTree>
    void save(XmlTree& xmlTree, const T& object) const
    {
        // @note Nothing to save.
    }

    template<typename T, typename XmlTree>
    void load(XmlTree& xmlTree, T& object) const
    {
        object = static_cast<T>(xmlTree.attributeCount(name));
    }
};

template<typename CompileTimeString>
class attribute_counter
{
private:
    static const attribute_counter_impl formatter;
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

template<typename CompileTimeString>
const attribute_counter_impl attribute_counter<CompileTimeString>::formatter = attribute_counter_impl( boost::optional<std::string>(!compile_time_string_print<CompileTimeString>::str().empty(), compile_time_string_print<CompileTimeString>::str()) );

attribute_counter_impl create_attribute_counter(const boost::optional<std::string>& name = boost::none)
{
    return attribute_counter_impl(name);
}

attribute_counter_impl create_attribute_counter(const char* name)
{
    return create_attribute_counter(std::string(name));
}

} // namespace xml
} // namespace arbitrary_format

#endif // ArbitraryFormatSerializer_attribute_counter_H
