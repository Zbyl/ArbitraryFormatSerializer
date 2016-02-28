/////////////////////////////////////////////////////////////////////////////
/// ArbitraryFormatSerializer
///    Library for serializing data in arbitrary formats.
///
/// string_formatter.h
///
/// This file contains string_formatter that formats std::string (and similar types) as length field followed by individual characters.
///
/// Distributed under Apache License, Version 2.0 (http://www.apache.org/licenses/LICENSE-2.0)
/// (c) 2014 Zbigniew Skowron, zbychs@gmail.com
///
/////////////////////////////////////////////////////////////////////////////

#ifndef ArbitraryFormatSerializer_string_formatter_H
#define ArbitraryFormatSerializer_string_formatter_H

#include "formatters/serialize_buffer.h"
#include "binary_formatters/endian_formatter.h"

#include <string>

namespace arbitrary_format
{
namespace binary
{

/// @note This formatter is very generic. It doesn't need to be in "binary" namespace.
///       It is here however, because this formatter most likely woudn't be very useful for non-binary formats.
template<typename CharT, typename SizeFormatter, typename CharFormatter>
class basic_string_formatter
{
    SizeFormatter size_formatter;
    CharFormatter char_formatter;

public:
    explicit basic_string_formatter(SizeFormatter size_formatter = SizeFormatter(), CharFormatter char_formatter = CharFormatter())
        : size_formatter(size_formatter)
        , char_formatter(char_formatter)
    {
    }

    template<typename TSerializer>
    void save(TSerializer& serializer, const std::basic_string<CharT>& string) const
    {
        size_formatter.save(serializer, string.length());
        save_buffer(serializer, string.length(), &string[0], char_formatter);   /// @note This is ok in C++ 11, even for empty strings.
    }

    template<typename TSerializer>
    void load(TSerializer& serializer, std::basic_string<CharT>& string) const
    {
        size_t string_size;
        size_formatter.load(serializer, string_size);

        string.resize(string_size);
        load_buffer(serializer, string_size, &string[0], char_formatter);   /// @note This is ok in C++ 11, even for empty strings.
    }
};

/// @note string_formatter has a default value for char formatter, because this is a lossless, natural format, and the most common case.
///       For other character types no format is inherently natural.
template<typename SizeFormatter, typename CharFormatter = binary::little_endian<1>>
using string_formatter = basic_string_formatter<char, SizeFormatter, CharFormatter>;

template<typename SizeFormatter, typename CharFormatter>
using wstring_formatter = basic_string_formatter<wchar_t, SizeFormatter, CharFormatter>;

template<typename SizeFormatter, typename CharFormatter>
using u16string_formatter = basic_string_formatter<char16_t, SizeFormatter, CharFormatter>;

template<typename SizeFormatter, typename CharFormatter>
using u32string_formatter = basic_string_formatter<char32_t, SizeFormatter, CharFormatter>;

template<typename SizeFormatter, typename CharFormatter>
string_formatter<SizeFormatter, CharFormatter> create_string_formatter(SizeFormatter size_formatter = SizeFormatter(), CharFormatter char_formatter = CharFormatter())
{
    return string_formatter<SizeFormatter, CharFormatter>(size_formatter, char_formatter);
}

template<typename SizeFormatter, typename CharFormatter>
wstring_formatter<SizeFormatter, CharFormatter> create_wstring_formatter(SizeFormatter size_formatter = SizeFormatter(), CharFormatter char_formatter = CharFormatter())
{
    return wstring_formatter<SizeFormatter, CharFormatter>(size_formatter, char_formatter);
}

template<typename SizeFormatter, typename CharFormatter>
u16string_formatter<SizeFormatter, CharFormatter> create_u16string_formatter(SizeFormatter size_formatter = SizeFormatter(), CharFormatter char_formatter = CharFormatter())
{
    return u16string_formatter<SizeFormatter, CharFormatter>(size_formatter, char_formatter);
}

template<typename SizeFormatter, typename CharFormatter>
u32string_formatter<SizeFormatter, CharFormatter> create_u32string_formatter(SizeFormatter size_formatter = SizeFormatter(), CharFormatter char_formatter = CharFormatter())
{
    return u32string_formatter<SizeFormatter, CharFormatter>(size_formatter, char_formatter);
}

} // namespace binary
} // namespace arbitrary_format

#endif // ArbitraryFormatSerializer_string_formatter_H
