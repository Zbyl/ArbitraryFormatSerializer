/////////////////////////////////////////////////////////////////////////////
/// ArbitraryFormatSerializer
///    Library for serializing data in arbitrary formats.
///
/// document_formatter.h
///
/// This file contains document_formatter that formats entire xml document.
///
/// Distributed under Apache License, Version 2.0 (http://www.apache.org/licenses/LICENSE-2.0)
/// (c) 2016 Zbigniew Skowron, zbychs@gmail.com
///
/////////////////////////////////////////////////////////////////////////////

#ifndef ArbitraryFormatSerializer_document_formatter_H
#define ArbitraryFormatSerializer_document_formatter_H

#include <arbitrary_format/xml_formatters/declaration_formatter.h>
#include <arbitrary_format/xml_formatters/element_formatter.h>
#include <arbitrary_format/formatters/const_formatter.h>
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

template<typename ValueFormatter>
class document_formatter_impl
{
private:
    ValueFormatter value_formatter;

public:
    explicit document_formatter_impl(ValueFormatter value_formatter = ValueFormatter())
        : value_formatter(value_formatter)
    {
    }

    template<typename T, typename XmlDocument>
    void save(XmlDocument& xmlDocument, const T& object) const
    {
        declaration_formatter<>().save(xmlDocument, xmlDocument.getEncoding());

        auto element = xmlDocument.getDocumentElement();
        value_formatter.save(element, object);
        if (element.hasName())
        {
            BOOST_THROW_EXCEPTION(serialization_exception() << errinfo_description("Document must not be named."));
        }
        if (element.attributeCount() > 0)
        {
            BOOST_THROW_EXCEPTION(serialization_exception() << errinfo_description("Document must not have attributes."));
        }
    }

    template<typename T, typename XmlDocument>
    void load(XmlDocument& xmlDocument, T& object) const
    {
        arbitrary_format::const_formatter< declaration_formatter<> >().load(xmlDocument, xmlDocument.getEncoding());

        auto element = xmlDocument.getDocumentElement();
        value_formatter.load(element, object);
        if (!element.isEmpty())
        {
            BOOST_THROW_EXCEPTION(serialization_exception() << errinfo_description("Document not consumed completely."));
        }
    }
};

template<typename CompileTimeString, typename ValueFormatter = assign_text_content<>>
using document_formatter = document_formatter_impl< element_formatter<CompileTimeString, ValueFormatter> >;

template<typename ValueFormatter>
document_formatter_impl<ValueFormatter> create_document_formatter(ValueFormatter value_formatter = ValueFormatter())
{
    return document_formatter_impl<ValueFormatter>(value_formatter);
}

template<typename ValueFormatter>
auto create_document_formatter(const std::string& name, ValueFormatter value_formatter = ValueFormatter())
    -> document_formatter_impl<element_formatter_impl<ValueFormatter>>
{
    return create_document_formatter(create_element_formatter(name, value_formatter));
}

} // namespace xml
} // namespace arbitrary_format

#endif // ArbitraryFormatSerializer_document_formatter_H
