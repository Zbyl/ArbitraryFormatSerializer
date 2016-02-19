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

#include "lexical_stringizer.h"

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
class attribute_check
{
private:
    boost::optional<std::string> name;

public:
    explicit attribute_check(const boost::optional<std::string>& name = boost::none)
        : name(name)
    {
    }

    explicit attribute_check(const char* name)
        : attribute_check(std::string(name))
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

using attribute_exists = attribute_check<true>;
using attribute_not_exists = attribute_check<false>;

template<bool TrueIfExists>
class element_check
{
private:
    boost::optional<std::string> name;

public:
    explicit element_check(const boost::optional<std::string>& name = boost::none)
        : name(name)
    {
    }

    explicit element_check(const char* name)
        : element_check(std::string(name))
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

using element_exists = element_check<true>;
using element_not_exists = element_check<false>;

} // namespace xml
} // namespace arbitrary_format

#endif // ArbitraryFormatSerializer_content_check_H 
