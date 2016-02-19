/////////////////////////////////////////////////////////////////////////////
/// ArbitraryFormatSerializer
///    Library for serializing data in arbitrary formats.
///
/// element_counter.h
///
/// This file contains element_counter that formats object as the count of attributes of xml element.
/// This counter can count all attributes or attributes with specific name.
///
/// Distributed under Apache License, Version 2.0 (http://www.apache.org/licenses/LICENSE-2.0)
/// (c) 2016 Zbigniew Skowron, zbychs@gmail.com
///
/////////////////////////////////////////////////////////////////////////////

#ifndef ArbitraryFormatSerializer_element_counter_H
#define ArbitraryFormatSerializer_element_counter_H

#include <string>

#include <boost/optional.hpp>

namespace arbitrary_format
{
namespace xml
{

class element_counter
{
private:
    boost::optional<std::string> name;

public:
    explicit element_counter(const boost::optional<std::string>& name = boost::none)
        : name(name)
    {
    }

    explicit element_counter(const char* name)
        : element_counter(std::string(name))
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
        object = static_cast<T>(xmlTree.elementCount(name));
    }
};

} // namespace xml
} // namespace arbitrary_format

#endif // ArbitraryFormatSerializer_element_counter_H
