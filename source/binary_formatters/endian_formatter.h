/////////////////////////////////////////////////////////////////////////////
/// ArbitraryFormatSerializer
///    Library for serializing data in arbitrary formats.
///
/// endian_formatter.h
///
/// This file contains little_endian and big_endian formatters for POD types.
/// Integral types and enums can be stored on different number of bytes than their size.
///
/// Distributed under Apache License, Version 2.0 (http://www.apache.org/licenses/LICENSE-2.0)
/// (c) 2014 Zbigniew Skowron, zbychs@gmail.com
///
/////////////////////////////////////////////////////////////////////////////

#ifndef ArbitraryFormatSerializer_endian_formatter_H
#define ArbitraryFormatSerializer_endian_formatter_H

#include "utility/integer_of_size.h"
#include "serialization_exceptions.h"
#include "verbatim_formatter.h"

#include <type_traits>

#include <boost/exception/error_info.hpp>
#include <boost/endian/conversion.hpp>

namespace arbitrary_format
{
namespace binary
{

template<int Size, typename T>
struct cant_store_type_in_this_number_of_bytes
{
    typedef boost::error_info<struct tag_cant_store_type_in_this_number_of_bytes, T> errinfo;
};

template<boost::endian::order TargetOrder, int Size>
class endian_formatter
{
public:
    /// @brief Stores given bool in TargetOrder byte order.
    ///        Throws lossy_conversion if Size is not enough to represent actual run-time value of pod.
    template<typename TSerializer>
    void save(TSerializer& serializer, bool b) const
    {
        save(serializer, static_cast<int>(b));
    }

    /// @brief Loads given bool from TargetOrder byte order.
    template<typename TSerializer>
    void load(TSerializer& serializer, bool& b) const
    {
        using SizedInt = typename integer_of_size<false, sizeof(bool)>::type;
        load(serializer, reinterpret_cast<SizedInt&>(b));   // @todo: violates strict aliasing rules?
    }

    /// @brief Stores given pod in TargetOrder byte order.
    ///        Throws lossy_conversion if Size is not enough to represent actual run-time value of pod.
    template<typename T, typename TSerializer>
    typename std::enable_if< std::is_integral<T>::value || std::is_enum<T>::value >::type 
    save(TSerializer& serializer, const T& pod) const
    {
        static_assert(std::is_pod<T>::value, "Type must be a pod.");

        using SizedInt = typename integer_of_size<std::is_signed<T>::value, Size>::type;
        SizedInt resized_value = static_cast<SizedInt>(pod);

        // throw if conversion to sized_value was lossy
        if (static_cast<T>(resized_value) != pod)
        {
            BOOST_THROW_EXCEPTION((lossy_conversion() << typename cant_store_type_in_this_number_of_bytes<Size, T>::errinfo(pod)));
        }

        SizedInt endian_shuffled_value = boost::endian::conditional_reverse<boost::endian::order::native, TargetOrder>(resized_value); 
        serializer.saveData(reinterpret_cast<boost::uint8_t*>(&endian_shuffled_value), Size);
    }

    /// @brief Loads given pod from TargetOrder byte order.
    template<typename T, typename TSerializer>
    typename std::enable_if< std::is_integral<T>::value || std::is_enum<T>::value >::type 
    load(TSerializer& serializer, T& pod) const
    {
        static_assert(std::is_pod<T>::value, "Type must be a pod.");

        using SizedInt = typename integer_of_size<std::is_signed<T>::value, Size>::type;
        SizedInt endian_shuffled_value;
        serializer.loadData(reinterpret_cast<boost::uint8_t*>(&endian_shuffled_value), Size);

        SizedInt resized_value = boost::endian::conditional_reverse<TargetOrder, boost::endian::order::native>(endian_shuffled_value); 
        pod = static_cast<T>(resized_value);
    }

    /// @brief Stores given pod in TargetOrder byte order.
    template<typename T, typename TSerializer>
    typename std::enable_if< !(std::is_integral<T>::value || std::is_enum<T>::value) >::type 
    save(TSerializer& serializer, const T& pod) const
    {
        static_assert(std::is_pod<T>::value, "Type must be a pod.");
        static_assert(sizeof(T) == Size, "Only integral types can be stored on different number of bytes than their size.");

        if (TargetOrder == boost::endian::order::native)
        {
            serializer.saveData(reinterpret_cast<const boost::uint8_t*>(&pod), Size);
        }
        else
        {
            uint8_t rawValue[Size];
            auto begin = reinterpret_cast<const boost::uint8_t*>(&pod);
            std::reverse_copy(begin, begin + Size, rawValue);
            serializer.saveData(rawValue, Size);
        }
    }

    /// @brief Loads given pod from TargetOrder byte order.
    template<typename T, typename TSerializer>
    typename std::enable_if< !(std::is_integral<T>::value || std::is_enum<T>::value) >::type 
    load(TSerializer& serializer, T& pod) const
    {
        static_assert(std::is_pod<T>::value, "Type must be a pod.");
        static_assert(sizeof(T) == Size, "Only integral types can be loaded from different number of bytes than their size.");

        if (TargetOrder == boost::endian::order::native)
        {
            serializer.loadData(reinterpret_cast<boost::uint8_t*>(&pod), Size);
        }
        else
        {
            uint8_t rawValue[Size];
            serializer.loadData(rawValue, Size);
            auto begin = reinterpret_cast<boost::uint8_t*>(&pod);
            std::reverse_copy(rawValue, rawValue + Size, begin);
        }
    }
};

template<int Size>
class little_endian : public endian_formatter<boost::endian::order::little, Size>
{
};

template<int Size>
class big_endian : public endian_formatter<boost::endian::order::big, Size>
{
};

/// @brief little_endian<sizeof(T)> will serialize T the same way as verbatim_formatter<sizeof(T)>.
template<typename Formatter, typename T>
struct is_verbatim_formatter<Formatter, T,
    typename std::enable_if<
        std::is_base_of< endian_formatter<boost::endian::order::native, sizeof(T)>, Formatter >::value
        >::type
    > : public std::true_type
{};

static_assert(is_verbatim_formatter< endian_formatter<boost::endian::order::native, 4>, uint32_t >::value, "Native endian_formatter should be a verbatim formatter.");
static_assert((boost::endian::order::native != boost::endian::order::little) || is_verbatim_formatter< little_endian<4>, uint32_t >::value, "little_endian should be a verbatim formatter on little endian machines.");
static_assert((boost::endian::order::native != boost::endian::order::little) || !is_verbatim_formatter< big_endian<4>, uint32_t >::value, "big_endian should not be a verbatim formatter on little endian machines.");
static_assert((boost::endian::order::native != boost::endian::order::big) || is_verbatim_formatter< big_endian<4>, uint32_t >::value, "big_endian should be a verbatim formatter on big endian machines.");
static_assert((boost::endian::order::native != boost::endian::order::big) || !is_verbatim_formatter< little_endian<4>, uint32_t >::value, "little_endian should not be a verbatim formatter on big endian machines.");

} // namespace binary
} // namespace arbitrary_format

#endif // ArbitraryFormatSerializer_endian_formatter_H
