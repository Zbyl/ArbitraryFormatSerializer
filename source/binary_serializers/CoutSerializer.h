/////////////////////////////////////////////////////////////////////////////
/// ArbitraryFormatSerializer
///    Library for serializing data in arbitrary formats.
///
/// CoutSerializer.h
///
/// This file contains CoutSerializer that outputs all data to std::cout. Useful for debugging of formatters.
///
/// Distributed under Apache License, Version 2.0 (http://www.apache.org/licenses/LICENSE-2.0)
/// (c) 2014 Zbigniew Skowron, zbychs@gmail.com
///
/////////////////////////////////////////////////////////////////////////////

#ifndef ArbitraryFormatSerializer_CoutSerializer_H
#define ArbitraryFormatSerializer_CoutSerializer_H

#include "ISerializer.h"

#include <iostream>

namespace arbitrary_format
{
namespace binary
{

class CoutSerializer : public SerializerMixin<CoutSerializer>
{
    bool appendNewLines;
public:
    /// @brief Constructs the serializer.
    /// @param appendNewLines    If true the serializer will add a new line after every call to serializeData(). Useful for debugging of formatters.
    explicit CoutSerializer(bool appendNewLines = false)
        : appendNewLines(appendNewLines)
    {
    }

    bool saving()
    {
        return true;
    }

public:
    void serializeData(boost::uint8_t* data, size_t size)
    {
        for (size_t i = 0; i < size; ++i)
        {
            std::cout << data[i];
        }

        if (appendNewLines)
        {
            std::cout << std::endl;
        }
    }
};

} // namespace binary
} // namespace arbitrary_format

#endif // ArbitraryFormatSerializer_CoutSerializer_H
