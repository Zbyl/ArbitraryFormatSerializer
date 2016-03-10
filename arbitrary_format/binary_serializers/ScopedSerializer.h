/////////////////////////////////////////////////////////////////////////////
/// ArbitraryFormatSerializer
///    Library for serializing data in arbitrary formats.
///
/// ScopedSerializer.h
///
/// This file contains ScopedSerializer that limits the number of bytes that is allowed to be processed.
/// It also allows for verifying that all data was processed.
///
/// Distributed under Apache License, Version 2.0 (http://www.apache.org/licenses/LICENSE-2.0)
/// (c) 2014 Zbigniew Skowron, zbychs@gmail.com
///
/////////////////////////////////////////////////////////////////////////////

#ifndef ArbitraryFormatSerializer_ScopedSerializer_H
#define ArbitraryFormatSerializer_ScopedSerializer_H

#include <arbitrary_format/binary_serializers/ISerializer.h>
#include <arbitrary_format/serialization_exceptions.h>

#include <cstdint>

namespace arbitrary_format
{
namespace binary
{

template<typename TSerializer>
class ScopedSerializer
{
    TSerializer& serializer;
    uintmax_t byteLimit;
    uintmax_t bytesProcessed;
public:
    ScopedSerializer(TSerializer& serializer, uintmax_t byteLimit)
        : serializer(serializer)
        , byteLimit(byteLimit)
        , bytesProcessed(0)
    {
        static_assert(is_saving_serializer<TSerializer>::value || is_loading_serializer<TSerializer>::value, "Serializer isn't a loading or saving serializer. Don't know how it should work.");
    }

    /// @brief Returns number of bytes that this serializer was allowed to process.
    uintmax_t getByteLimit()
    {
        return byteLimit;
    }

    /// @brief Returns number of bytes that has been processed by this serializer so far.
    uintmax_t getBytesProcessed()
    {
        return bytesProcessed;
    }

    /// @brief Returns number of bytes that are still left to be processed.
    uintmax_t getBytesLeft()
    {
        return byteLimit - bytesProcessed;
    }

    /// @brief This method will verify that serialization processed exactly the number of bytes specified as the byte limit.
    ///        It will throw invalid_data exception if not all data was processed.
    void verifyAllBytesProcessed()
    {
        if (bytesProcessed < byteLimit)
        {
            BOOST_THROW_EXCEPTION(invalid_data() << errinfo_requested_this_many_bytes_more(byteLimit - bytesProcessed));
        }
    }

    bool saving() const
    {
        return is_serializer_saving(serializer);
    }

public:
    using saving_serializer = is_saving_serializer<TSerializer>;
    using loading_serializer = is_loading_serializer<TSerializer>;

    template<typename T>
    typename std::enable_if<saving_serializer::value && std::is_same<T, uint8_t>::value>::type
    saveData(const T* data, size_t size)
    {
        countData(size);
        serializer.saveData(data, size);
    }

    template<typename T>
    typename std::enable_if<loading_serializer::value && std::is_same<T, uint8_t>::value>::type
    loadData(T* data, size_t size)
    {
        countData(size);
        serializer.loadData(data, size);
    }

private:
    void countData(size_t size)
    {
        bytesProcessed += size;
        if (bytesProcessed > byteLimit)
        {
            if (saving())
            {
                BOOST_THROW_EXCEPTION(end_of_space() << errinfo_requested_this_many_bytes_more(bytesProcessed - byteLimit));
            }
            else
            {
                BOOST_THROW_EXCEPTION(end_of_input() << errinfo_requested_this_many_bytes_more(bytesProcessed - byteLimit));
            }
        }
    }
};

} // namespace binary
} // namespace arbitrary_format

#endif // ArbitraryFormatSerializer_ScopedSerializer_H
