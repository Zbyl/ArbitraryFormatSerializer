/////////////////////////////////////////////////////////////////////////////
/// ArbitraryFormatSerializer
///    Library for serializing data in arbitrary formats.
///
/// string_formatter.h
///
/// This file contains string_formatter that formats std::string as length field followed by individual characters.
///
/// Distributed under Apache License, Version 2.0 (http://www.apache.org/licenses/LICENSE-2.0)
/// (c) 2014 Zbigniew Skowron, zbychs@gmail.com
///
/////////////////////////////////////////////////////////////////////////////

#ifndef ArbitraryFormatSerializer_string_formatter_H
#define ArbitraryFormatSerializer_string_formatter_H

#include <string>

namespace arbitrary_format
{
namespace binary
{

template<typename SizeFormatter>
class string_formatter
{
    SizeFormatter size_formatter;

public:
    explicit string_formatter(SizeFormatter size_formatter = SizeFormatter())
        : size_formatter(size_formatter)
    {
    }

    template<typename TSerializer>
    void save(TSerializer& serializer, const std::string& string) const
    {
        size_formatter.save(serializer, string.length());
        serializer.saveData(reinterpret_cast<const boost::uint8_t*>(string.c_str()), string.length());
    }

    template<typename TSerializer>
    void load(TSerializer& serializer, std::string& string) const
    {
        string.clear();
        size_t string_size;
        size_formatter.load(serializer, string_size);
        if (string_size > 0)
        {
            std::vector<char> data(string_size);
            serializer.loadData(reinterpret_cast<boost::uint8_t*>(&data[0]), string_size);
            string.assign(data.begin(), data.end());
        }
    }
};

template<typename SizeFormatter>
string_formatter<SizeFormatter> create_string_formatter(SizeFormatter size_formatter = SizeFormatter())
{
    return string_formatter<SizeFormatter>(size_formatter);
}

} // namespace binary
} // namespace arbitrary_format

#endif // ArbitraryFormatSerializer_string_formatter_H
