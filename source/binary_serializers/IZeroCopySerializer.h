/////////////////////////////////////////////////////////////////////////////
/// ArbitraryFormatSerializer
///    Library for serializing data in arbitrary formats.
///
/// IZeroCopySerializer.h
///
/// This file contains IZeroCopySerializer interface that is a serializer providing access to it's internal buffer.
///
/// Distributed under Apache License, Version 2.0 (http://www.apache.org/licenses/LICENSE-2.0)
/// (c) 2014 Zbigniew Skowron, zbychs@gmail.com
///
/////////////////////////////////////////////////////////////////////////////

#ifndef ArbitraryFormatSerializer_IZeroCopySerializer_H
#define ArbitraryFormatSerializer_IZeroCopySerializer_H

#include "ISerializer.h"
#include "serialization_exceptions.h"

#include <algorithm>
#include <cstdint>

namespace arbitrary_format
{
namespace binary
{

class IZeroCopySerializer : public ISerializer
{
public:
    virtual void serializeData(uint8_t* data, size_t size)
    {
        while (size > 0)
        {
            uint8_t* chunk;
            size_t chunkSize;
            if (!nextChunk(chunk, chunkSize))
            {
                if (saving())
                {
                    BOOST_THROW_EXCEPTION(end_of_space() << detail::errinfo_requested_this_many_bytes_more(size));
                }
                else
                {
                    BOOST_THROW_EXCEPTION(end_of_input() << detail::errinfo_requested_this_many_bytes_more(size));
                }
            }

            size_t toCopy = std::min(size, chunkSize);
            if (saving())
            {
                std::copy(data, data + toCopy, chunk);
            }
            else
            {
                std::copy(chunk, chunk + toCopy, data);
            }

            data += toCopy;
            size -= toCopy;

            giveBack(0);
        }
    }

    /// @brief nextChunk() returns the internal buffer of the serializer.
    ///        This buffered is considered processed by the client (either read from, or written to)
    ///        when nextChunk() is called again, or some bytes are returned by calling giveBack().
    ///        If value returned from this function is false, then it means that no more data can be
    ///        read or written (the input/output reached it's end). size will then be set to zero.
    virtual bool nextChunk(uint8_t*& data, size_t& size) = 0;

    /// @brief Informs that buffer returned by nextChunk() has been processed by the client,
    ///        except for the last 'unprocessed' number bytes.
    virtual void giveBack(size_t unprocessed) = 0;
};

} // namespace binary
} // namespace arbitrary_format

#endif // ArbitraryFormatSerializer_IZeroCopySerializer_H
