/////////////////////////////////////////////////////////////////////////////
/// ArbitraryFormatSerializer
///    Library for serializing data in arbitrary formats.
///
/// hex_stringizer.h
///
/// This file contains hex_stringizer that saves integrals and enums as hexadecimal numbers with 0x prefix, loads them using lexical stringizer (so it accepts decimal numbers also).
/// All other types are serialized using provided fallback stringizer.
/// Note: char types are formatted like ints. This is because int8_t should display as number.
/// Note: enums are formatted like ints.
///
/// Distributed under Apache License, Version 2.0 (http://www.apache.org/licenses/LICENSE-2.0)
/// (c) 2016 Zbigniew Skowron, zbychs@gmail.com
///
/////////////////////////////////////////////////////////////////////////////

#ifndef ArbitraryFormatSerializer_hex_stringizer_H
#define ArbitraryFormatSerializer_hex_stringizer_H

#include <arbitrary_format/xml_formatters/lexical_stringizer.h>
#include <arbitrary_format/utility/char_types.h>

#include <sstream>
#include <iomanip>
#include <type_traits>

#include <boost/lexical_cast.hpp>

namespace arbitrary_format
{
namespace xml
{

template<typename FallbackStringizer = lexical_stringizer>
class hex_stringizer
{
private:
    /// @note Overloads below are used to format chars like ints, not like characters.
    ///       This is most of the time the preferred way.

    template<int ByteSize, typename T>
    static std::string asHex(T value)
    {
        std::stringstream str;
        str << std::noshowbase << std::hex << "0x" << std::setfill('0') << std::setw(ByteSize * 2) << value;
        return str.str();
    }

    FallbackStringizer fallback_stringizer;

public:
    explicit hex_stringizer(FallbackStringizer fallback_stringizer = FallbackStringizer())
        : fallback_stringizer(fallback_stringizer)
    {
    }

    /// @note We want the fallback saving for other types.
    template<typename T, typename String>
    typename std::enable_if< !std::is_integral<T>::value && !std::is_enum<T>::value >::type
    save(String& string, const T& object) const
    {
        fallback_stringizer.save(string, object);
    }

    /// @note We want the hexadecimal saving for integrals.
    template<typename T, typename String>
    typename std::enable_if< std::is_integral<T>::value && !is_char_type<T>::value >::type
    save(String& string, const T& object) const
    {
        string = asHex<sizeof(T)>(object);
    }

    /// @note We want the hexadecimal saving for characters.
    template<typename T, typename String>
    typename std::enable_if< is_char_type<T>::value >::type
    save(String& string, const T& object) const
    {
        string = asHex<sizeof(T)>(static_cast<char_to_int<T>>(object));
    }

    /// @note We want the hexadecimal saving for enums.
    template<typename T, typename String>
    typename std::enable_if< std::is_enum<T>::value >::type
    save(String& string, const T& object) const
    {
        string = asHex<sizeof(T)>(static_cast< typename std::underlying_type<T>::type >(object));
    }

    /// @note We want lexical loading for integrals (including chars) and enums.
    template<typename T, typename String>
    typename std::enable_if< std::is_integral<T>::value || std::is_enum<T>::value >::type
    load(const String& string, T& object) const
    {
        lexical_stringizer().load(string, object);
    }

    /// @note We want the fallback loading for other types.
    template<typename T, typename String>
    typename std::enable_if< !std::is_integral<T>::value && !std::is_enum<T>::value >::type
    load(const String& string, T& object) const
    {
        fallback_stringizer.load(string, object);
    }
};

template<typename FallbackStringizer = lexical_stringizer>
hex_stringizer<FallbackStringizer> create_hex_stringizer(FallbackStringizer fallback_stringizer = FallbackStringizer())
{
    return hex_stringizer<FallbackStringizer>(fallback_stringizer);
}

} // namespace xml
} // namespace arbitrary_format

#endif // ArbitraryFormatSerializer_hex_stringizer_H
