/////////////////////////////////////////////////////////////////////////////
/// ArbitraryFormatSerializer
///    Library for serializing data in arbitrary formats.
///
/// ZeroCopyVectorSerializer.h
///
/// This file contains ZeroCopyVectorSaveSerializer and ZeroCopyVectorLoadSerializer that write to / read from a vector.
///
/// Distributed under Apache License, Version 2.0 (http://www.apache.org/licenses/LICENSE-2.0)
/// (c) 2014 Zbigniew Skowron, zbychs@gmail.com
///
/////////////////////////////////////////////////////////////////////////////

#ifndef ArbitraryFormatSerializer_ZeroCopyVectorSerializer_H
#define ArbitraryFormatSerializer_ZeroCopyVectorSerializer_H

#include "ISerializer.h"
#include "IZeroCopySerializer.h"

#include "serialization_exceptions.h"

#include <vector>

namespace arbitrary_format
{
namespace binary
{

class ZeroCopyVectorSaveSerializer : public SerializerMixin<ZeroCopyVectorSaveSerializer>, public IZeroCopySerializer
{
    std::vector<uint8_t> buffer;
    size_t chunkSize;
    size_t position;
public:
    ZeroCopyVectorSaveSerializer(size_t chunkSize)
        : chunkSize(chunkSize)
        , position(0)
    {
    }

    const std::vector<uint8_t>& getData()
    {
        return buffer;
    }

    bool saving()
    {
        return true;
    }

public:
    bool nextChunk(uint8_t*& data, size_t& size)
    {
        giveBack(0);    // commit previously returned buffer

        buffer.resize(buffer.capacity());
        if (position >= buffer.size())
        {
            buffer.resize(buffer.size() + chunkSize);
        }

        data = buffer.data() + position;
        size = buffer.size() - position;

        return true;
    }

    void giveBack(size_t unprocessed)
    {
        buffer.resize(buffer.size() - unprocessed);
        position = buffer.size();
    }
};

class ZeroCopyVectorLoadSerializer // : public SerializerMixin<ZeroCopyVectorLoadSerializer>, public IZeroCopySerializer
{
    const std::vector<uint8_t>& buffer;
    size_t position;
public:
    ZeroCopyVectorLoadSerializer(const std::vector<uint8_t>& buffer)
        : buffer(buffer)
        , position(0)
    {
    }

    bool saving()
    {
        return false;
    }

public:
    bool nextChunk(const uint8_t*& data, size_t& size)
    {
        giveBack(0);    // commit previously returned buffer

        data = buffer.data() + position;
        size = buffer.size() - position;
        position = buffer.size();

        return size > 0;
    }

    void giveBack(size_t unprocessed)
    {
        position -= unprocessed;
    }
};

} // namespace binary
} // namespace arbitrary_format

#endif // ArbitraryFormatSerializer_ZeroCopyVectorSerializer_H
