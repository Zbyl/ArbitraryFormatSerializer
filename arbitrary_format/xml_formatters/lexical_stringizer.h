/////////////////////////////////////////////////////////////////////////////
/// ArbitraryFormatSerializer
///    Library for serializing data in arbitrary formats.
///
/// lexical_stringizer.h
///
/// This file contains lexical_stringizer that formats values using boost::lexical_cast.
/// Note: char types are formatted like ints. This is because int8_t should display as number.
/// Note: enums are formatted like ints.
///
/// Distributed under Apache License, Version 2.0 (http://www.apache.org/licenses/LICENSE-2.0)
/// (c) 2016 Zbigniew Skowron, zbychs@gmail.com
///
/////////////////////////////////////////////////////////////////////////////

#ifndef ArbitraryFormatSerializer_lexical_stringizer_H
#define ArbitraryFormatSerializer_lexical_stringizer_H

#include <arbitrary_format/utility/char_types.h>

#include <type_traits>

#include <boost/lexical_cast.hpp>

namespace arbitrary_format
{
namespace xml
{

class lexical_stringizer
{
public:
    template<typename T, typename String>
    typename std::enable_if< !is_char_type<T>::value && !std::is_enum<T>::value  >::type
    save(String& string, const T& object) const
    {
        string = boost::lexical_cast<String>(object);
    }

    template<typename T, typename String>
    typename std::enable_if< !is_char_type<T>::value && !std::is_enum<T>::value >::type
    load(const String& string, T& object) const
    {
        object = boost::lexical_cast<T>(string);
    }

    /// @note Overloads below are used to format chars like ints, not like characters.
    ///       This is most of the time the preferred way.

    template<typename T, typename String>
    typename std::enable_if< is_char_type<T>::value >::type
    save(String& string, const T& object) const
    {
        string = boost::lexical_cast<String>(static_cast<char_to_int<T>>(object));
    }

    template<typename T, typename String>
    typename std::enable_if< is_char_type<T>::value >::type
    load(const String& string, T& object) const
    {
        object = static_cast<T>(boost::lexical_cast<char_to_int<T>>(string));
    }

    /// @note Overloads below are used to format enums like ints.

    template<typename T, typename String>
    typename std::enable_if< std::is_enum<T>::value >::type
    save(String& string, const T& object) const
    {
        string = boost::lexical_cast<String>(static_cast< typename std::underlying_type<T>::type >(object));
    }

    template<typename T, typename String>
    typename std::enable_if< std::is_enum<T>::value >::type
    load(const String& string, T& object) const
    {
        object = static_cast<T>(boost::lexical_cast< typename std::underlying_type<T>::type >(string));
    }
};

inline lexical_stringizer create_lexical_stringizer()
{
    return lexical_stringizer();
}

} // namespace xml
} // namespace arbitrary_format

#endif // ArbitraryFormatSerializer_lexical_stringizer_H
