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

#include "ISerializer.h"

#include "serialization_exceptions.h"

namespace arbitrary_format
{
namespace binary
{

template<typename TSerializer>
class ScopedSerializer : public SerializerMixin< ScopedSerializer<TSerializer> >
{
    TSerializer& serializer;
    boost::uintmax_t byteLimit;
    boost::uintmax_t bytesProcessed;
public:
    ScopedSerializer(TSerializer& serializer, boost::uintmax_t byteLimit)
        : serializer(serializer)
        , byteLimit(byteLimit)
        , bytesProcessed(0)
    {
    }

    /// @brief Returns number of bytes that this serializer was allowed to process.
    boost::uintmax_t getByteLimit()
    {
        return byteLimit;
    }

    /// @brief Returns number of bytes that has been processed by this serializer so far.
    boost::uintmax_t getBytesProcessed()
    {
        return bytesProcessed;
    }

    /// @brief Returns number of bytes that are still left to be processed.
    boost::uintmax_t getBytesLeft()
    {
        return byteLimit - bytesProcessed;
    }

    /// @brief This method will verify that serialization processed exactly the number of bytes specified as the byte limit.
    ///        It will throw invalid_data exception if not all data was processed.
    void verifyAllBytesProcessed()
    {
        if (bytesProcessed < byteLimit)
        {
            BOOST_THROW_EXCEPTION(invalid_data() << detail::errinfo_requested_this_many_bytes_more(byteLimit - bytesProcessed));
        }
    }

    virtual bool saving()
    {
        return serializer.saving();
    }

public:
    virtual void serializeData(boost::uint8_t* data, size_t size)
    {
        bytesProcessed += size;
        if (bytesProcessed > byteLimit)
        {
            if (saving())
            {
                BOOST_THROW_EXCEPTION(end_of_space() << detail::errinfo_requested_this_many_bytes_more(bytesProcessed - byteLimit));
            }
            else
            {
                BOOST_THROW_EXCEPTION(end_of_input() << detail::errinfo_requested_this_many_bytes_more(bytesProcessed - byteLimit));
            }
        }

        serializer.serializeData(data, size);
    }
};

} // namespace binary
} // namespace arbitrary_format

#endif // ArbitraryFormatSerializer_ScopedSerializer_H
