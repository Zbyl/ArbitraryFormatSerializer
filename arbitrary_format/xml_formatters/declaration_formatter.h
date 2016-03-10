/////////////////////////////////////////////////////////////////////////////
/// ArbitraryFormatSerializer
///    Library for serializing data in arbitrary formats.
///
/// declaration_formatter.h
///
/// This file contains declaration_formatter that formats xml declaration.
///
/// Distributed under Apache License, Version 2.0 (http://www.apache.org/licenses/LICENSE-2.0)
/// (c) 2016 Zbigniew Skowron, zbychs@gmail.com
///
/////////////////////////////////////////////////////////////////////////////

#ifndef ArbitraryFormatSerializer_declaration_formatter_H
#define ArbitraryFormatSerializer_declaration_formatter_H

#include <arbitrary_format/xml_formatters/lexical_stringizer.h>
#include <arbitrary_format/serialization_exceptions.h>

#include <string>
#include <vector>
#include <type_traits>

#include <boost/optional.hpp>
#include <boost/algorithm/string.hpp>

namespace arbitrary_format
{
namespace xml
{

template<typename ValueStringizer = lexical_stringizer>
class declaration_formatter
{
private:
    ValueStringizer value_stringizer;

public:
    explicit declaration_formatter(ValueStringizer value_stringizer = ValueStringizer())
        : value_stringizer(value_stringizer)
    {
    }

    template<typename T, typename XmlDocument>
    void save(XmlDocument& xmlDocument, const T& encoding) const
    {
        auto docElement = xmlDocument.getDocumentElement();
        if (!docElement.isEmpty())
        {
            BOOST_THROW_EXCEPTION(serialization_exception() << errinfo_description("xml declaration must be the first node of the document."));
        }

        std::string value;
        value_stringizer.save(value, encoding);

        auto xmlDeclaration = docElement.addElement(boost::none, true);
        auto version = xmlDeclaration.addAttribute("version");
        auto encodingAttrib = xmlDeclaration.addAttribute("encoding");
        version.setTextContent("1.0");
        encodingAttrib.setTextContent(value);
    }

    template<typename T, typename XmlDocument>
    void load(XmlDocument& xmlDocument, T& encoding) const
    {
        auto docElement = xmlDocument.getDocumentElement();

        auto xmlDeclaration = docElement.eatElement(boost::none, true);
        if (!xmlDeclaration.isDeclaration())
        {
            BOOST_THROW_EXCEPTION(serialization_exception() << errinfo_description("Document should start with an xml declaration."));
        }

        auto encodingAttr = xmlDeclaration.eatAttribute("encoding", false);
        auto encodingText = encodingAttr.eatTextContent();
        value_stringizer.load(encodingText, encoding);
    }
};

template<typename ValueStringizer = lexical_stringizer>
declaration_formatter<ValueStringizer> create_declaration_formatter(ValueStringizer value_stringizer = ValueStringizer())
{
    return declaration_formatter<ValueStringizer>(value_stringizer);
}

} // namespace xml
} // namespace arbitrary_format

#endif // ArbitraryFormatSerializer_declaration_formatter_H
