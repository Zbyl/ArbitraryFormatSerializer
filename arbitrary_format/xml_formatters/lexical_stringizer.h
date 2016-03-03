/////////////////////////////////////////////////////////////////////////////
/// ArbitraryFormatSerializer
///    Library for serializing data in arbitrary formats.
///
/// lexical_stringizer.h
///
/// This file contains lexical_stringizer that formats values using boost::lexical_cast.
///
/// Distributed under Apache License, Version 2.0 (http://www.apache.org/licenses/LICENSE-2.0)
/// (c) 2016 Zbigniew Skowron, zbychs@gmail.com
///
/////////////////////////////////////////////////////////////////////////////

#ifndef ArbitraryFormatSerializer_lexical_stringizer_H
#define ArbitraryFormatSerializer_lexical_stringizer_H

#include <boost/lexical_cast.hpp>

namespace arbitrary_format
{
namespace xml
{

class lexical_stringizer
{
public:
    template<typename T, typename String>
    void save(String& string, const T& object) const
    {
        string = boost::lexical_cast<String>(object);
    }

    template<typename T, typename String>
    void load(const String& string, T& object) const
    {
        object = boost::lexical_cast<T>(string);
    }
};

inline lexical_stringizer create_lexical_stringizer()
{
    return lexical_stringizer();
}

} // namespace xml
} // namespace arbitrary_format

#endif // ArbitraryFormatSerializer_lexical_stringizer_H
