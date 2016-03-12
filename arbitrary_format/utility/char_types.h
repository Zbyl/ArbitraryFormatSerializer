/////////////////////////////////////////////////////////////////////////////
/// ArbitraryFormatSerializer
///    Library for serializing data in arbitrary formats.
///
/// char_types.h
///
/// This file contains metafunctions to help dealing with char types.
///
/// Distributed under Apache License, Version 2.0 (http://www.apache.org/licenses/LICENSE-2.0)
/// (c) 2016 Zbigniew Skowron, zbychs@gmail.com
///
/////////////////////////////////////////////////////////////////////////////

#ifndef ArbitraryFormatSerializer_char_types_H
#define ArbitraryFormatSerializer_char_types_H

#include <type_traits>

namespace arbitrary_format
{

/// @brief True type if T is char, signed char or unsiged char
template<typename T>
using is_char_type = typename std::conditional<std::is_same<T, char>::value || std::is_same<T, signed char>::value || std::is_same<T, unsigned char>::value, std::true_type, std::false_type>::type;

/// @brief Returns int32_t or uint32_t if T is respectively a signed char type or an unsigned char type, returns T otherwise.
template<typename T>
using char_to_int = typename std::conditional<is_char_type<T>::value,
                                              typename std::conditional<std::is_signed<T>::value, int32_t, uint32_t>::type,
                                              T>::type;

} // namespace arbitrary_format

#endif // ArbitraryFormatSerializer_char_types_H
