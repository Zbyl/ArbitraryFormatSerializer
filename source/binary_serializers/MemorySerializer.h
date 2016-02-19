/////////////////////////////////////////////////////////////////////////////
/// ArbitraryFormatSerializer
///    Library for serializing data in arbitrary formats.
///
/// MemorySerializer.h
///
/// This file contains MemorySaveSerializer and MemoryLoadSerializer that write to / read from an Memory.
///
/// Distributed under Apache License, Version 2.0 (http://www.apache.org/licenses/LICENSE-2.0)
/// (c) 2014 Zbigniew Skowron, zbychs@gmail.com
///
/////////////////////////////////////////////////////////////////////////////

#ifndef ArbitraryFormatSerializer_MemorySerializer_H
#define ArbitraryFormatSerializer_MemorySerializer_H

#include "ISerializer.h"

#include "serialization_exceptions.h"

#include <cstdint>
#include <type_traits>
#include <array>

namespace arbitrary_format
{
namespace binary
{

class MemorySaveSerializer : public SerializerMixin<MemorySaveSerializer>
{
    uint8_t* buffer;
    size_t bufferSize;
    size_t bufferPosition;
public:
    template<typename T>
    explicit MemorySaveSerializer(T* buffer, size_t bufferSize)
        : buffer(reinterpret_cast<uint8_t*>(buffer))
        , bufferSize(bufferSize)
        , bufferPosition(0)
    {
        static_assert(std::is_pod<T>::value, "Type of data in memory buffer must be a pod.");
        static_assert(sizeof(T) == 1, "Size of data in memory buffer must be 1 to avoid element cout / byte count mismatch errors.");
    }

    template<int Size>
    explicit MemorySaveSerializer(std::array<uint8_t, Size>& buffer)
        : MemorySaveSerializer(buffer.data(), Size)
    {
    }

    uint8_t* getData()
    {
        return buffer;
    }

    /// @brief Returns size of the underlying memory buffer.
    size_t getMaxDataSize()
    {
        return bufferSize;
    }

    bool saving()
    {
        return true;
    }

    size_t position()
    {
        return bufferPosition;
    }

    void seek(size_t pos)
    {
        if (pos > bufferSize)
        {
            BOOST_THROW_EXCEPTION(serialization_exception() << detail::errinfo_description("Requested position is greater than size."));
        }
        bufferPosition = pos;
    }

public:
    void serializeData(uint8_t* data, size_t size)
    {
        if (bufferPosition + size > bufferSize)
        {
            BOOST_THROW_EXCEPTION(serialization_exception() << detail::errinfo_description("No space in buffer."));
        }
        
        std::copy_n(data, size, buffer + bufferPosition);
        bufferPosition += size;
    }
};

class MemoryLoadSerializer : public SerializerMixin<MemoryLoadSerializer>
{
    const uint8_t* buffer;
    size_t bufferSize;
    size_t bufferPosition;
public:
    template<typename T>
    explicit MemoryLoadSerializer(const T* buffer, size_t bufferSize)
        : buffer(reinterpret_cast<const uint8_t*>(buffer))
        , bufferSize(bufferSize)
        , bufferPosition(0)
    {
        static_assert(std::is_pod<T>::value, "Type of data in memory buffer must be a pod.");
        static_assert(sizeof(T) == 1, "Size of data in memory buffer must be 1 to avoid element cout / byte count mismatch errors.");
    }

    template<typename T, int Size>
    explicit MemoryLoadSerializer(const std::array<T, Size>& buffer)
        : MemoryLoadSerializer(buffer.data(), Size)
    {
    }

    template<typename T>
    explicit MemoryLoadSerializer(const std::vector<T>& buffer)
        : MemoryLoadSerializer(buffer.data(), buffer.size())
    {
    }

    bool saving()
    {
        return false;
    }

    size_t position()
    {
        return bufferPosition;
    }

    void seek(size_t pos)
    {
        if (pos > bufferSize)
        {
            BOOST_THROW_EXCEPTION(end_of_input() << detail::errinfo_requested_this_many_bytes_more(pos - bufferSize));
        }
        bufferPosition = pos;
    }

public:
    void serializeData(uint8_t* data, size_t size)
    {
        if (bufferPosition + size > bufferSize)
        {
            BOOST_THROW_EXCEPTION(end_of_input() << detail::errinfo_requested_this_many_bytes_more(bufferPosition + size - bufferSize));
        }

        std::copy_n(buffer + bufferPosition, size, data);
        bufferPosition += size;
    }
};

} // namespace binary
} // namespace arbitrary_format

#endif // ArbitraryFormatSerializer_MemorySerializer_H
