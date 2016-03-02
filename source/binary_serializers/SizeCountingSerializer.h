/////////////////////////////////////////////////////////////////////////////
/// ArbitraryFormatSerializer
///    Library for serializing data in arbitrary formats.
///
/// SizeCountingSerializer.h
///
/// This file contains SizeCountingSerializer that count number of bytes that would be produced by the serialization.
///
/// Distributed under Apache License, Version 2.0 (http://www.apache.org/licenses/LICENSE-2.0)
/// (c) 2014 Zbigniew Skowron, zbychs@gmail.com
///
/////////////////////////////////////////////////////////////////////////////

#ifndef ArbitraryFormatSerializer_SizeCountingSerializer_H
#define ArbitraryFormatSerializer_SizeCountingSerializer_H

#include <cstdint>
#include <type_traits>

namespace arbitrary_format
{
namespace binary
{

template<bool Saving>
class SizeCountingSerializerImpl
{
    uintmax_t byteCount;
public:
    SizeCountingSerializerImpl()
        : byteCount(0)
    {
    }

    /// @brief Returns number of bytes that has been stored to / loaded from this serializer so far.
    uintmax_t getByteCount()
    {
        return byteCount;
    }

public:
    using saving_serializer = std::integral_constant<bool, Saving>;
    using loading_serializer = std::integral_constant<bool, !Saving>;

    template<typename T>
    typename std::enable_if<Saving && std::is_same<T, uint8_t>::value>::type
    saveData(const T* data, size_t size)
    {
        byteCount += size;
    }

    template<typename T>
    typename std::enable_if<!Saving && std::is_same<T, uint8_t>::value>::type
    loadData(T* data, size_t size)
    {
        byteCount += size;
    }
};

using SizeCountingSerializer = SizeCountingSerializerImpl<true>;
using LoadSizeCountingSerializer = SizeCountingSerializerImpl<false>;

} // namespace binary
} // namespace arbitrary_format

#endif // ArbitraryFormatSerializer_SizeCountingSerializer_H
