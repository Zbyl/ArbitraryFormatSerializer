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

#include "ISerializer.h"

#include <boost/cstdint.hpp>

namespace arbitrary_format
{
namespace binary
{

class SizeCountingSerializer : public SerializerMixin<SizeCountingSerializer>
{
    boost::uintmax_t byteCount;
public:
    SizeCountingSerializer()
        : byteCount(0)
    {
    }

    /// @brief Returns number of bytes that has been stored to this serializer so far.
    boost::uintmax_t getByteCount()
    {
        return byteCount;
    }

    bool saving()
    {
        return true;
    }

public:
    void serializeData(boost::uint8_t* data, size_t size)
    {
        byteCount += size;
    }
};

} // namespace binary
} // namespace arbitrary_format

#endif // ArbitraryFormatSerializer_SizeCountingSerializer_H
