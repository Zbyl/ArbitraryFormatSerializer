/////////////////////////////////////////////////////////////////////////////
/// ArbitraryFormatSerializer
///    Library for serializing data in arbitrary formats.
///
/// attribute_formatter_t.h
///
/// This file contains attribute_formatter_t that formats object as attribute of specific name.
/// Element can be unnamed. Use then assign_name formatter to assign a name to it.
///
/// Distributed under Apache License, Version 2.0 (http://www.apache.org/licenses/LICENSE-2.0)
/// (c) 2016 Zbigniew Skowron, zbychs@gmail.com
///
/////////////////////////////////////////////////////////////////////////////

#ifndef ArbitraryFormatSerializer_attribute_formatter_t_H
#define ArbitraryFormatSerializer_attribute_formatter_t_H

#include "attribute_formatter.h"
#include "utility/compile_time_string.h"

namespace arbitrary_format
{
namespace xml
{

template<typename CompileTimeString, typename ValueFormatter = assign_text_content<>, bool Sequential = true>
class attribute_formatter_t
{
private:
    static const attribute_formatter<ValueFormatter, Sequential> formatter;
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
const attribute_formatter<ValueFormatter, Sequential> attribute_formatter_t<CompileTimeString, ValueFormatter, Sequential>::formatter = 
    attribute_formatter<ValueFormatter, Sequential>( boost::optional<std::string>(!CompileTimeString::empty::value, compile_time_string_print<CompileTimeString>::str()) );

} // namespace xml
} // namespace arbitrary_format

#endif // ArbitraryFormatSerializer_attribute_formatter_t_H
