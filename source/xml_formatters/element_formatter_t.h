/////////////////////////////////////////////////////////////////////////////
/// ArbitraryFormatSerializer
///    Library for serializing data in arbitrary formats.
///
/// element_formatter_t.h
///
/// This file contains element_formatter_t that formats object as element of specific name.
/// Element can be unnamed. Use then assign_name formatter to assign a name to it.
///
/// Distributed under Apache License, Version 2.0 (http://www.apache.org/licenses/LICENSE-2.0)
/// (c) 2016 Zbigniew Skowron, zbychs@gmail.com
///
/////////////////////////////////////////////////////////////////////////////

#ifndef ArbitraryFormatSerializer_element_formatter_t_H
#define ArbitraryFormatSerializer_element_formatter_t_H

#include "element_formatter.h"
#include "utility/compile_time_string.h"

namespace arbitrary_format
{
namespace xml
{

template<typename CompileTimeString, typename ValueFormatter = assign_text_content<>, bool Sequential = true>
class element_formatter_t
{
private:
    static const element_formatter<ValueFormatter, Sequential> formatter;
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
const element_formatter<ValueFormatter, Sequential> element_formatter_t<CompileTimeString, ValueFormatter, Sequential>::formatter = 
    element_formatter<ValueFormatter, Sequential>( boost::optional<std::string>(!CompileTimeString::empty::value, compile_time_string_print<CompileTimeString>::str()) );

} // namespace xml
} // namespace arbitrary_format

#endif // ArbitraryFormatSerializer_element_formatter_t_H
