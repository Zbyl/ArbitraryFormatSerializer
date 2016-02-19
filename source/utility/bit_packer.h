/////////////////////////////////////////////////////////////////////////////
/// ArbitraryFormatSerializer
///    Library for serializing data in arbitrary formats.
///
/// bit_packer.h
///
/// This file contains bit_packer that packs and unpacks values or tuples of values in bitfields.
///
/// Distributed under Apache License, Version 2.0 (http://www.apache.org/licenses/LICENSE-2.0)
/// (c) 2014 Zbigniew Skowron, zbychs@gmail.com
///
/////////////////////////////////////////////////////////////////////////////

#ifndef ArbitraryFormatSerializer_bit_packer_H
#define ArbitraryFormatSerializer_bit_packer_H

#include "integer_of_size.h"
#include "metaprogramming.h"

#include <tuple>

namespace arbitrary_format
{
namespace binary
{

template<int... Bits>
class bit_packer
{
    static constexpr int bits_size = isec::sum_args<int, Bits...>::value;
    static_assert( (bits_size == 8) || (bits_size == 16) || (bits_size == 32) || (bits_size == 64), "Sum of bits must be a full 1, 2, 4 or 8 bytes.");
    static constexpr int byte_size = bits_size / 8;

public:
    using packed_type = typename integer_of_size<false, byte_size>::type;

private:
    template<typename T>
    static T fsum()
    {
        return T();
    }

    template<typename T, typename... Ts>
    static T fsum(T val, Ts... vals)
    {
        return val + fsum<T>(vals...);
    }

    /// Cuts specified number of bits from given position in a value, and casts them to given type. Ignores other bits.
    /// Example:
    ///    bitsToVal<int, 4, 1>(00010011) will cast bits 1..4 (1001) to int, which will be -7.
    ///    bitsToVal<unsigned, 4, 1>(00010011) will cast bits 1..4 (1001) to unsigned int, which will be 9.
    template<int Bits, int Shift, typename T>
    static T bitsToVal(packed_type val, T dummy)
    {
        static_assert(Bits > 0, "Number of bits for each component must be greater than 0");
        constexpr packed_type mask = ~( (~packed_type()) << Bits );
        constexpr packed_type sign_mask = packed_type(1) << (Bits - 1);
        packed_type result = (val >> Shift) & mask;
        if ( std::is_signed<T>::value && (result & sign_mask) )
        {
            result |= ~mask;
        }

        using resizedT = typename integer_of_size< std::is_signed<T>::value, byte_size>::type;
        auto properlySignedResult = static_cast<resizedT>(result);
        T finalVal = static_cast<T>(properlySignedResult);
        return finalVal;
    }

    /// Overload of bitsToVal for bools
    template<int Bits, int Shift>
    static bool bitsToVal(packed_type val, bool dummy)
    {
        static_assert(Bits > 0, "Number of bits for each component must be greater than 0");
        constexpr packed_type mask = ~( (~packed_type()) << Bits );
        packed_type result = (val >> Shift) & mask;

        if (result == 0)
            return false;
        if (result == 1)
            return true;

        BOOST_THROW_EXCEPTION(invalid_data());
    }

    /// Represents given value on given number of bits. Sets remaining bits of result to 0.
    /// Example (assumes packed_type == uint16_t):
    ///    valToBits<int8_t, 4>(11111111) will represent -1 on 4 bits, which will be (00000000 00001111).
    ///    valToBits<int8_t, 9>(11111111) will represent -1 on 9 bits, which will be (00000001 11111111).
    ///    valToBits<uint8_t, 9>(11111111) will represent 256 on 9 bits, which will be (00000000 11111111).
    template<int Bits, typename T>
    static packed_type valToBits(T val)
    {
        static_assert(Bits > 0, "Number of bits for each component must be greater than 0");
        constexpr packed_type mask = ~( (~packed_type()) << Bits );
        using resizedT = typename integer_of_size< std::is_signed<T>::value, byte_size>::type;
        resizedT resized_value = static_cast<resizedT>(val);
        packed_type finalVal = static_cast<packed_type>(resized_value) & mask;

        // throw if conversion was lossy
        T reconstructedVal = bitsToVal<Bits, 0>(finalVal, T());
        if (reconstructedVal != val)
        {
            BOOST_THROW_EXCEPTION((lossy_conversion() << typename cant_store_type_in_this_number_of_bytes<byte_size, T>::errinfo(val)));
        }

        return finalVal;
    }

    /// Overload of valToBits for bools
    template<int Bits>
    static packed_type valToBits(bool val)
    {
        static_assert(Bits > 0, "Number of bits for each component must be greater than 0");
        return val ? 1 : 0;
    }

    template<int... Shifts, typename... Ts>
    static packed_type pack(std::integer_sequence<int, Shifts...>, const Ts&... vals)
    {
         return fsum( (valToBits<Bits>(vals) << Shifts ) ... );
    }
    
    template<typename... Ts, int... Shifts>
    static void unpack(std::integer_sequence<int, Shifts...>, packed_type packed, Ts&... vals)
    {
        auto vars = std::tie(vals...);
        auto values = std::make_tuple(bitsToVal<Bits, Shifts>(packed, Ts())...);
        vars = values;
    }

    template<typename... Ts, std::size_t... Ints>
    static packed_type tuple_pack_impl(const std::tuple<Ts...>& vals, std::index_sequence<Ints...> Is)
    {
        packed_type val = pack(std::get<Ints>(vals)...);
        return val;
    }

    /// Unpacks into a tuple of references.
    template<typename... Ts, std::size_t... Ints>
    static void tuple_unpack_impl(packed_type val, const std::tuple<Ts&...>& vals, std::index_sequence<Ints...> Is)
    {
        unpack(val, std::get<Ints>(vals)...);
    }

    /// Unpacks into a reference to a tuple of values.
    template<typename... Ts, std::size_t... Ints>
    static void tuple_unpack_impl(packed_type val, std::tuple<Ts...>& vals, std::index_sequence<Ints...> Is)
    {
        unpack(val, std::get<Ints>(vals)...);
    }

public:
    template<typename... Ts>
    static packed_type pack(Ts... vals)
    {
        using bits_seq = std::integer_sequence<int, Bits...>;
        using shifts_seq = isec::push_front<int, isec::pop_back< isec::partial_sum<bits_seq> >, 0>;
        //TypeDisplayer<isec::push_front<int, isec::pop_back< isec::partial_sum<bits_seq> >, 0>> td;

        packed_type val = pack(shifts_seq(), vals...);
        return val;
    }

    template<typename... Ts>
    static void unpack(packed_type val, Ts&... vals)
    {
        using bits_seq = std::integer_sequence<int, Bits...>;
        using shifts_seq = isec::push_front<int, isec::pop_back< isec::partial_sum<bits_seq> >, 0>;
        unpack(shifts_seq(), val, vals...);
    }

    template<typename... Ts>
    static packed_type pack(const std::tuple<Ts...>& vals)
    {
        packed_type val = tuple_pack_impl(vals, std::index_sequence_for<Ts...>());
        return val;
    }

    /// Unpacks into a tuple of references.
    template<typename... Ts>
    static void unpack(packed_type val, const std::tuple<Ts&...>& vals)
    {
        tuple_unpack_impl(val, vals, std::index_sequence_for<Ts...>());
    }

    /// Unpacks into a reference to a tuple of values.
    template<typename... Ts>
    static void unpack(packed_type val, std::tuple<Ts...>& vals)
    {
        tuple_unpack_impl(val, vals, std::index_sequence_for<Ts...>());
    }
};

} // namespace binary
} // namespace arbitrary_format

#endif // ArbitraryFormatSerializer_bit_packer_H
