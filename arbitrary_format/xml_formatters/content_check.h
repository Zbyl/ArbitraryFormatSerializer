/////////////////////////////////////////////////////////////////////////////
/// ArbitraryFormatSerializer
///    Library for serializing data in arbitrary formats.
///
/// content_check.h
///
/// This file contains bool formatters, that check for various content of xml elements.
///
/// Distributed under Apache License, Version 2.0 (http://www.apache.org/licenses/LICENSE-2.0)
/// (c) 2016 Zbigniew Skowron, zbychs@gmail.com
///
/////////////////////////////////////////////////////////////////////////////

#ifndef ArbitraryFormatSerializer_content_check_H
#define ArbitraryFormatSerializer_content_check_H

#include <arbitrary_format/xml_formatters/lexical_stringizer.h>

#include <string>

namespace arbitrary_format
{
namespace xml
{

template<bool TrueIfExists>
class content_check
{
public:
    template<typename XmlTree>
    void save(XmlTree& xmlTree, const bool& object) const
    {
        // @note Nothing to save.
    }

    template<typename XmlTree>
    void load(XmlTree& xmlTree, bool& object) const
    {
        bool textContentExists = xmlTree.hasTextContent();
        bool elementExists = xmlTree.elementCount() > 0;
        bool contentExists = textContentExists || elementExists;
        object = contentExists == TrueIfExists;
    }
};

using content_exists = content_check<true>;
using content_not_exists = content_check<false>;

template<bool TrueIfExists>
class text_content_check
{
public:
    template<typename XmlTree>
    void save(XmlTree& xmlTree, const bool& object) const
    {
        // @note Nothing to save.
    }

    template<typename XmlTree>
    void load(XmlTree& xmlTree, bool& object) const
    {
        bool textContentExists = xmlTree.hasTextContent();
        object = textContentExists == TrueIfExists;
    }
};

using text_content_exists = text_content_check<true>;
using text_content_not_exists = text_content_check<false>;

template<bool TrueIfExists>
class attribute_check_impl
{
private:
    boost::optional<std::string> name;

public:
    explicit attribute_check_impl(const boost::optional<std::string>& name = boost::none)
        : name(name)
    {
    }

    explicit attribute_check_impl(const char* name)
        : attribute_check_impl(std::string(name))
    {
    }

    template<typename XmlTree>
    void save(XmlTree& xmlTree, const bool& object) const
    {
        // @note Nothing to save.
    }

    template<typename XmlTree>
    void load(XmlTree& xmlTree, bool& object) const 
    {
        bool attributeExists = xmlTree.attributeCount(name) > 0;
        object = attributeExists == TrueIfExists;
    }
};

template<bool TrueIfExists>
class element_check_impl
{
private:
    boost::optional<std::string> name;

public:
    explicit element_check_impl(const boost::optional<std::string>& name = boost::none)
        : name(name)
    {
    }

    explicit element_check_impl(const char* name)
        : element_check_impl(std::string(name))
    {
    }

    template<typename XmlTree>
    void save(XmlTree& xmlTree, const bool& object) const
    {
        // @note Nothing to save.
    }

    template<typename XmlTree>
    void load(XmlTree& xmlTree, bool& object) const
    {
        bool elementExists = xmlTree.elementCount(name) > 0;
        object = elementExists == TrueIfExists;
    }
};

template<typename CompileTimeString, bool TrueIfExists>
class attribute_check
{
private:
    static const attribute_check_impl<TrueIfExists> formatter;
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

template<typename CompileTimeString, bool TrueIfExists>
const attribute_check_impl<TrueIfExists> attribute_check<CompileTimeString, TrueIfExists>::formatter = attribute_check_impl<TrueIfExists>( boost::optional<std::string>(!compile_time_string_print<CompileTimeString>::str().empty(), compile_time_string_print<CompileTimeString>::str()) );

template<typename CompileTimeString, bool TrueIfExists>
class element_check
{
private:
    static const element_check_impl<TrueIfExists> formatter;
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

template<typename CompileTimeString, bool TrueIfExists>
const element_check_impl<TrueIfExists> element_check<CompileTimeString, TrueIfExists>::formatter = element_check_impl<TrueIfExists>( boost::optional<std::string>(!compile_time_string_print<CompileTimeString>::str().empty(), compile_time_string_print<CompileTimeString>::str()) );

template<typename CompileTimeString>
using attribute_exists = attribute_check<CompileTimeString, true>;
template<typename CompileTimeString>
using attribute_not_exists = attribute_check<CompileTimeString, false>;

template<typename CompileTimeString>
using element_exists = element_check<CompileTimeString, true>;
template<typename CompileTimeString>
using element_not_exists = element_check<CompileTimeString, false>;

attribute_check_impl<true> create_attribute_exists(const boost::optional<std::string>& name = boost::none)
{
    return attribute_check_impl<true>(name);
}

attribute_check_impl<true> create_attribute_exists(const char* name)
{
    return create_attribute_exists(std::string(name));
}

attribute_check_impl<false> create_attribute_not_exists(const boost::optional<std::string>& name = boost::none)
{
    return attribute_check_impl<false>(name);
}

attribute_check_impl<false> create_attribute_not_exists(const char* name)
{
    return create_attribute_not_exists(std::string(name));
}

element_check_impl<true> create_element_exists(const boost::optional<std::string>& name = boost::none)
{
    return element_check_impl<true>(name);
}

element_check_impl<true> create_element_exists(const char* name)
{
    return create_element_exists(std::string(name));
}

element_check_impl<false> create_element_not_exists(const boost::optional<std::string>& name = boost::none)
{
    return element_check_impl<false>(name);
}

element_check_impl<false> create_element_not_exists(const char* name)
{
    return create_element_not_exists(std::string(name));
}

} // namespace xml
} // namespace arbitrary_format

#endif // ArbitraryFormatSerializer_content_check_H 
