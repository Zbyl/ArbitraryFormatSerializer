/////////////////////////////////////////////////////////////////////////////
/// ArbitraryFormatSerializer
///    Library for serializing data in arbitrary formats.
///
/// bit_formatter.h
///
/// This file contains bit_formatter that stores individual values or tuples of values packed in bitfields.
///
/// Distributed under Apache License, Version 2.0 (http://www.apache.org/licenses/LICENSE-2.0)
/// (c) 2014 Zbigniew Skowron, zbychs@gmail.com
///
/////////////////////////////////////////////////////////////////////////////

#ifndef ArbitraryFormatSerializer_bit_formatter_H
#define ArbitraryFormatSerializer_bit_formatter_H

#include <arbitrary_format/binary_formatters/endian_formatter.h>
#include <arbitrary_format/utility/bit_packer.h>

namespace arbitrary_format
{
namespace binary
{

template<boost::endian::order TargetOrder, int... Bits>
class bit_formatter
{
    using packer = bit_packer<Bits...>;
    using value_formatter = endian_formatter<TargetOrder, sizeof(typename packer::packed_type)>;

public:
    template<typename TSerializer, typename... Ts>
    void save(TSerializer& serializer, Ts... vals) const
    {
        typename packer::packed_type val = packer::pack(vals...);
        value_formatter().save(serializer, val);
    }

    template<typename TSerializer, typename... Ts>
    void load(TSerializer& serializer, Ts&... vals) const
    {
        typename packer::packed_type val;
        value_formatter().load(serializer, val);
        packer::unpack(val, vals...);
    }
    template<typename TSerializer, typename... Ts>
    void save(TSerializer& serializer, const std::tuple<Ts...>& vals) const
    {
        typename packer::packed_type val = packer::pack(vals);
        value_formatter().save(serializer, val);
    }

    /// Unpacks into a tuple of references.
    template<typename TSerializer, typename... Ts>
    void load(TSerializer& serializer, const std::tuple<Ts&...>& vals) const
    {
        typename packer::packed_type val;
        value_formatter().load(serializer, val);
        packer::unpack(val, vals);
    }

    /// Unpacks into a reference to a tuple of values.
    template<typename TSerializer, typename... Ts>
    void load(TSerializer& serializer, std::tuple<Ts...>& vals) const
    {
        typename packer::packed_type val;
        value_formatter().load(serializer, val);
        packer::unpack(val, vals);
    }
};

} // namespace binary
} // namespace arbitrary_format

#endif // ArbitraryFormatSerializer_bit_formatter_H
