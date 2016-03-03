/////////////////////////////////////////////////////////////////////////////
/// ArbitraryFormatSerializer
///    Library for serializing data in arbitrary formats.
///
/// integer_of_size.h
///
/// This file contains integer_of_size, int_of_size and uint_of_size templates providing ints of desired size and signedness.
///
/// Distributed under Apache License, Version 2.0 (http://www.apache.org/licenses/LICENSE-2.0)
/// (c) 2014 Zbigniew Skowron, zbychs@gmail.com
///
/////////////////////////////////////////////////////////////////////////////

#ifndef ArbitraryFormatSerializer_integer_of_size_H
#define ArbitraryFormatSerializer_integer_of_size_H

#include <cstdint>
#include <type_traits>

namespace arbitrary_format
{
namespace binary
{

template<bool isSigned, int Size>
struct integer_of_size;

////////////////////////////////////////////////////////////////////////////////

template<>
struct integer_of_size<true, 1>
{
    typedef int8_t type;
};

template<>
struct integer_of_size<true, 2>
{
    typedef int16_t type;
};

template<>
struct integer_of_size<true, 4>
{
    typedef int32_t type;
};

template<>
struct integer_of_size<true, 8>
{
    typedef int64_t type;
};

////////////////////////////////////////////////////////////////////////////////

template<>
struct integer_of_size<false, 1>
{
    typedef uint8_t type;
};

template<>
struct integer_of_size<false, 2>
{
    typedef uint16_t type;
};

template<>
struct integer_of_size<false, 4>
{
    typedef uint32_t type;
};

template<>
struct integer_of_size<false, 8>
{
    typedef uint64_t type;
};

////////////////////////////////////////////////////////////////////////////////

template<int Size>
using int_of_size = integer_of_size<true, Size>;

template<int Size>
using uint_of_size = integer_of_size<false, Size>;

} // namespace binary
} // namespace arbitrary_format

#endif // ArbitraryFormatSerializer_integer_of_size_H
