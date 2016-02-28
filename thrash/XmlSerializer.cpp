// XmlSerializer.cpp : Defines the entry point for the console application.
//

#include "xml_formatters/lexical_stringizer.h"
#include "xml_formatters/text_formatter.h"
#include "xml_formatters/attribute_formatter.h"
#include "xml_formatters/element_formatter.h"
#include "xml_formatters/element_counter.h"
#include "xml_formatters/attribute_counter.h"
#include "xml_formatters/assign_name.h"
#include "xml_formatters/assign_text_content.h"
#include "xml_formatters/content_check.h"
#include "xml_formatters/document_formatter.h"
#include "xml_formatters/declaration_formatter.h"

#include "formatters/vector_formatter.h"
#include "formatters/optional_formatter.h"
#include "formatters/map_formatter.h"
#include "formatters/collection_formatter.h"
#include "formatters/tuple_formatter.h"

#include "xml_serializers/RapidXmlTree.h"
#include "binary_serializers/ISerializer.h"

#include "utility/metaprogramming.h"

#include <cstdint>
#include <iostream>
#include <sstream>
#include <vector>
#include <map>
#include <utility>

using namespace arbitrary_format;
using namespace xml;

template<const char* str>
class A
{
};

int main(int argc, char* argv[])
{
#if 0
    constexpr str_const text("ala");
    static_assert(text.data() == "ala", "Different!");
    static const char bb[] = "ala";
    struct cc
    {
        const char* ll = "ala";
    }
    constexpr ac = cc{};
    A<ac.ll> xx;
    A<bb> xx;
    A<text.data()> xx;
#endif
    RapidXmlSaveSerializer document("utf-8");
    RapidXmlSaveSerializer allDocument("utf-8");

    auto vec = std::vector<int> { 1, 2, 3, 4, 5 };
    auto nameToAge = std::map<std::string, int> { {"Ela", 5}, {"Ala", 6}, {"Ola", 7} };
    auto nameToAgeVec = std::vector<std::pair<std::string, int>> { {"Ela", 5}, {"Ala", 6}, {"Ola", 7} };
    boost::optional<std::string> optNone;
    boost::optional<std::string> optStr("A value!");

    auto numberFormatter = create_attribute_formatter("number");
    auto sizeFormatter = attribute_counter("number");
    auto vectorFormatter = create_vector_formatter(sizeFormatter, numberFormatter);
    auto vectorElementFormatter = create_element_formatter("numbers", vectorFormatter);

    auto pairFormatter = create_tuple_formatter(assign_name<>(), assign_text_content<>());
    auto valueFormatter = create_attribute_formatter(boost::none, pairFormatter);
    auto vectorFormatter2 = create_vector_formatter(attribute_counter(), valueFormatter);
    auto vectorElementFormatter2 = create_element_formatter("nameToAgeVec", vectorFormatter2);

    auto kvFormatter = create_attribute_formatter(boost::none, create_tuple_formatter(assign_name<>(), assign_text_content<>()));
    auto mapFormatter = create_collection_formatter(attribute_counter(), kvFormatter);
    auto mapElementFormatter = create_element_formatter("nameToAge", mapFormatter);

    auto k2Formatter = create_attribute_formatter("value");
    auto kvFormatter2 = create_element_formatter("key", create_tuple_formatter(k2Formatter, assign_text_content<>()));
    auto mapFormatter2 = create_collection_formatter(element_counter(), kvFormatter2);
    auto mapElementFormatter2 = create_element_formatter("nameToAge", mapFormatter2);

    auto k3Formatter = create_element_formatter("key");
    auto v3Formatter = create_element_formatter("value");
    auto mapFormatter3 = create_map_formatter(element_counter("key"), k3Formatter, v3Formatter);
    auto mapElementFormatter3 = create_element_formatter("nameToAge", mapFormatter3);

    auto optionalFormatter = create_element_formatter("optional", create_optional_formatter(content_exists(), assign_text_content<>()));

    declare_compile_time_string(_optional_, "optional");
    declare_compile_time_string(_initialized_, "initialized");
    declare_compile_time_string(_value_, "value");
    using opt_format = element_formatter< _optional_, optional_formatter< attribute_formatter<_initialized_>, element_formatter<_value_> > >;

    //auto doc_format = create_element_formatter< text_formatter<lexical_stringizer> >("myxml");
    //doc_format.save(document.getDocumentElement(), 6);

    declaration_formatter<>().save(document, "utf-8");
    vectorElementFormatter.save(document.getDocumentElement(), vec);
    vectorElementFormatter2.save(document.getDocumentElement(), nameToAgeVec);
    mapElementFormatter.save(document.getDocumentElement(), nameToAge);
    mapElementFormatter2.save(document.getDocumentElement(), nameToAge);
    mapElementFormatter3.save(document.getDocumentElement(), nameToAge);
    optionalFormatter.save(document.getDocumentElement(), optNone);
    optionalFormatter.save(document.getDocumentElement(), optStr);
    boost::optional<std::string> a("ala");
    binary::serialize<opt_format>(document.getDocumentElement(), a);

    auto allFormatter = create_element_formatter("all", create_tuple_formatter(vectorElementFormatter, vectorElementFormatter2, mapElementFormatter));

    allFormatter.save(document.getDocumentElement(), std::tie(vec, nameToAgeVec, nameToAge));

    auto docFormatter = create_document_formatter("doc", allFormatter);
    docFormatter.save(allDocument, std::tie(vec, nameToAgeVec, nameToAge));

    std::stringstream str;
    str << document;
    std::string documentText = str.str();
    std::cout << documentText;

    std::stringstream allStr;
    allStr << allDocument;
    std::string allDocumentText = allStr.str();
    std::cout << allDocumentText;

    RapidXmlLoadSerializer document2(documentText, "utf-8");
    std::cout << document2;
    RapidXmlLoadSerializer allDocument2(allDocumentText, "utf-8");
    std::cout << allDocument2;

    std::string encoding;
    std::vector<int> loadedVec;
    std::vector<std::pair<std::string, int>> loadedNameToAgeVec;
    std::map<std::string, int> loadedNameToAge;
    std::map<std::string, int> loadedNameToAge2;
    std::map<std::string, int> loadedNameToAge3;
    boost::optional<std::string> optNone2;
    boost::optional<std::string> optStr2;

    declaration_formatter<>().load(document2, encoding);
    vectorElementFormatter.load(document2.getDocumentElement(), loadedVec);
    vectorElementFormatter2.load(document2.getDocumentElement(), loadedNameToAgeVec);
    mapElementFormatter.load(document2.getDocumentElement(), loadedNameToAge);
    mapElementFormatter2.load(document2.getDocumentElement(), loadedNameToAge2);
    mapElementFormatter3.load(document2.getDocumentElement(), loadedNameToAge3);
    optionalFormatter.load(document2.getDocumentElement(), optNone2);
    optionalFormatter.load(document2.getDocumentElement(), optStr2);
    boost::optional<std::string> b;
    binary::load<opt_format>(document2.getDocumentElement(), b);

    std::vector<int> loadedVec2;
    std::vector<std::pair<std::string, int>> loadedNameToAgeVec2;
    std::map<std::string, int> loadedNameToAge4;

    auto tiedValues = std::tie(loadedVec2, loadedNameToAgeVec2, loadedNameToAge4);
    allFormatter.load(document2.getDocumentElement(), tiedValues);

    std::vector<int> loadedVec5;
    std::vector<std::pair<std::string, int>> loadedNameToAgeVec5;
    std::map<std::string, int> loadedNameToAge5;

    auto tiedValues2 = std::tie(loadedVec5, loadedNameToAgeVec5, loadedNameToAge5);
    docFormatter.load(allDocument2, tiedValues2);

    std::cout << "Eaten Xml: '"<< document2 << "'";

    return 0;
}
