/////////////////////////////////////////////////////////////////////////////
/// ArbitraryFormatSerializer
///    Library for serializing data in arbitrary formats.
///
/// integer_for_size.h
///
/// This file contains integer_for_size, int_for_size and uint_for_size templates providing ints of minimal size greater or equal to requested (with requested signedness).
///
/// Distributed under Apache License, Version 2.0 (http://www.apache.org/licenses/LICENSE-2.0)
/// (c) 2014 Zbigniew Skowron, zbychs@gmail.com
///
/////////////////////////////////////////////////////////////////////////////

#ifndef ArbitraryFormatSerializer_integer_for_size_H
#define ArbitraryFormatSerializer_integer_for_size_H

#include <cstdint>
#include <type_traits>

namespace arbitrary_format
{
namespace binary
{

template<bool isSigned, int Size>
struct integer_for_size;

////////////////////////////////////////////////////////////////////////////////

template<>
struct integer_for_size<true, 1>
{
    typedef int8_t type;
};

template<>
struct integer_for_size<true, 2>
{
    typedef int16_t type;
};

template<>
struct integer_for_size<true, 3>
{
    typedef int32_t type;
};

template<>
struct integer_for_size<true, 4>
{
    typedef int32_t type;
};

template<>
struct integer_for_size<true, 5>
{
    typedef int64_t type;
};

template<>
struct integer_for_size<true, 6>
{
    typedef int64_t type;
};

template<>
struct integer_for_size<true, 7>
{
    typedef int64_t type;
};

template<>
struct integer_for_size<true, 8>
{
    typedef int64_t type;
};

////////////////////////////////////////////////////////////////////////////////

template<>
struct integer_for_size<false, 1>
{
    typedef uint8_t type;
};

template<>
struct integer_for_size<false, 2>
{
    typedef uint16_t type;
};

template<>
struct integer_for_size<false, 3>
{
    typedef uint32_t type;
};

template<>
struct integer_for_size<false, 4>
{
    typedef uint32_t type;
};

template<>
struct integer_for_size<false, 5>
{
    typedef uint64_t type;
};

template<>
struct integer_for_size<false, 6>
{
    typedef uint64_t type;
};

template<>
struct integer_for_size<false, 7>
{
    typedef uint64_t type;
};

template<>
struct integer_for_size<false, 8>
{
    typedef uint64_t type;
};

////////////////////////////////////////////////////////////////////////////////

template<int Size>
using int_for_size = integer_for_size<true, Size>;

template<int Size>
using uint_for_size = integer_for_size<false, Size>;

} // namespace binary
} // namespace arbitrary_format

#endif // ArbitraryFormatSerializer_integer_for_size_H
