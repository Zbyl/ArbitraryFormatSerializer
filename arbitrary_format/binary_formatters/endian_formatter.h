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

#include <arbitrary_format/utility/integer_for_size.h>
#include <arbitrary_format/utility/integer_of_size.h>
#include <arbitrary_format/serialization_exceptions.h>
#include <arbitrary_format/binary_formatters/verbatim_formatter.h>

#include <type_traits>

#include <boost/exception/error_info.hpp>

#include <boost/version.hpp>

#if (BOOST_VERSION >= 105800)

#include <boost/endian/conversion.hpp>

namespace arbitrary_format_endian = boost::endian;

#else

#include <algorithm>

/// @note This is an emulation of boost::endian library, when it's not available.
///       It assumes a little endian machine.
namespace arbitrary_format_endian
{
    enum class order
    {
        big,
        little,
        native = little
    };

    template<order FromOrder, order ToOrder, typename T>
    T conditional_reverse(const T& value)
    {
        if (FromOrder == ToOrder)
        {
            return value;
        }

        T result;
        std::reverse_copy(reinterpret_cast<const uint8_t*>(&value), reinterpret_cast<const uint8_t*>(&value) + sizeof(T), reinterpret_cast<uint8_t*>(&result));
        return result;
    }

} // namespace arbitrary_format_endian

#endif

namespace arbitrary_format
{
namespace binary
{

template<int Size, typename T>
struct cant_store_type_in_this_number_of_bytes
{
    typedef boost::error_info<struct tag_cant_store_type_in_this_number_of_bytes, T> errinfo;
};

template<arbitrary_format_endian::order TargetOrder, int Size>
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

        /// @note First we shift data in such a way, that value is in Size most significant bytes. Then we save only those Size most significant bytes.

        using SizedInt = typename integer_for_size<std::is_signed<T>::value, Size>::type;
        SizedInt resized_value = static_cast<SizedInt>(pod) << (sizeof(SizedInt) - Size) * 8;  // now our value is in most significant bytes
        // throw if conversion to sized_value was lossy
        if (static_cast<T>(resized_value >> (sizeof(SizedInt) - Size) * 8) != pod)
        {
            BOOST_THROW_EXCEPTION((lossy_conversion() << typename cant_store_type_in_this_number_of_bytes<Size, T>::errinfo(pod)));
        }

        // we save only Size most significant bytes
        static const size_t saveOffset = (arbitrary_format_endian::order::little == TargetOrder) ? (sizeof(SizedInt) - Size) : 0;
        SizedInt endian_shuffled_value = arbitrary_format_endian::conditional_reverse<arbitrary_format_endian::order::native, TargetOrder>(resized_value); 
        serializer.saveData(reinterpret_cast<uint8_t*>(&endian_shuffled_value) + saveOffset, Size);
    }

    /// @brief Loads given pod from TargetOrder byte order.
    template<typename T, typename TSerializer>
    typename std::enable_if< std::is_integral<T>::value || std::is_enum<T>::value >::type 
    load(TSerializer& serializer, T& pod) const
    {
        static_assert(std::is_pod<T>::value, "Type must be a pod.");

        ///   @note When loading integrals we want to load data in such a way, that after endian reverse value will be in most significant bytes.
        ///         This way right shift will preserve the sign of the value.
        ///
        ///   Little endian machine
        ///           Little endian data   Load offset    Loaded        Reversed        Shifted
        ///   Memory      AA BB CC             1        XX AA BB CC    XX AA BB CC     AA BB CC ss
        ///   Value                                     0xCCBBAAXX     0xCCBBAAXX      0xssCCBBAA
        ///           Big endian data      Load offset    Loaded        Reversed        Shifted
        ///   Memory      CC BB AA             0        CC BB AA XX    XX AA BB CC     AA BB CC ss
        ///   Value                                     0xXXAABBCC     0xCCBBAAXX      0xssCCBBAA
        ///
        ///   Big endian machine
        ///           Little endian data   Load offset    Loaded        Reversed        Shifted
        ///   Memory      AA BB CC             1        XX AA BB CC    CC BB AA XX     ss CC BB AA
        ///   Value                                     0xXXAABBCC     0xCCBBAAXX      0xssCCBBAA
        ///           Big endian data      Load offset    Loaded        Reversed        Shifted
        ///   Memory      CC BB AA             0        CC BB AA XX    CC BB AA XX     ss CC BB AA
        ///   Value                                     0xCCBBAAXX     0xCCBBAAXX      0xssCCBBAA

        using SizedInt = typename integer_for_size<std::is_signed<T>::value, Size>::type;
        SizedInt endian_shuffled_value;

        // we load only Size most significant bytes
        static const size_t loadOffset = (arbitrary_format_endian::order::little == TargetOrder) ? (sizeof(SizedInt) - Size) : 0;
        serializer.loadData(reinterpret_cast<uint8_t*>(&endian_shuffled_value) + loadOffset, Size);
        SizedInt resized_value = arbitrary_format_endian::conditional_reverse<TargetOrder, arbitrary_format_endian::order::native>(endian_shuffled_value); 

        resized_value >>= (sizeof(SizedInt) - Size) * 8;

        pod = static_cast<T>(resized_value);
    }

    /// @brief Stores given pod in TargetOrder byte order.
    template<typename T, typename TSerializer>
    typename std::enable_if< !(std::is_integral<T>::value || std::is_enum<T>::value) >::type 
    save(TSerializer& serializer, const T& pod) const
    {
        save_pod(serializer, pod);
    }

    /// @brief Loads given pod from TargetOrder byte order.
    template<typename T, typename TSerializer>
    typename std::enable_if< !(std::is_integral<T>::value || std::is_enum<T>::value) >::type 
    load(TSerializer& serializer, T& pod) const
    {
        load_pod(serializer, pod);
    }

private:
    /// @brief Stores given pod in TargetOrder byte order.
    template<typename T, typename TSerializer>
    void save_pod(TSerializer& serializer, const T& pod) const
    {
        static_assert(std::is_pod<T>::value, "Type must be a pod.");
        static_assert(sizeof(T) == Size, "Only integral types can be stored on different number of bytes than their size.");

        if (TargetOrder == arbitrary_format_endian::order::native)
        {
            serializer.saveData(reinterpret_cast<const uint8_t*>(&pod), Size);
        }
        else
        {
            uint8_t rawValue[Size];
            auto begin = reinterpret_cast<const uint8_t*>(&pod);
            std::reverse_copy(begin, begin + Size, rawValue);
            serializer.saveData(rawValue, Size);
        }
    }

    /// @brief Loads given pod from TargetOrder byte order.
    template<typename T, typename TSerializer>
    void load_pod(TSerializer& serializer, T& pod) const
    {
        static_assert(std::is_pod<T>::value, "Type must be a pod.");
        static_assert(sizeof(T) == Size, "Only integral types can be loaded from different number of bytes than their size.");

        if (TargetOrder == arbitrary_format_endian::order::native)
        {
            serializer.loadData(reinterpret_cast<uint8_t*>(&pod), Size);
        }
        else
        {
            uint8_t rawValue[Size];
            serializer.loadData(rawValue, Size);
            auto begin = reinterpret_cast<uint8_t*>(&pod);
            std::reverse_copy(rawValue, rawValue + Size, begin);
        }
    }
};

template<int Size>
using little_endian = endian_formatter<arbitrary_format_endian::order::little, Size>;

template<int Size>
using big_endian = endian_formatter<arbitrary_format_endian::order::big, Size>;

/// @brief For pods of size one endian_formatter<TargetOrder, 1> will serialize T the same way as a verbatim formatter.
///        For pods of size S endian_formatter<native, S> will serialize T the same way as a verbatim formatter.
/// @note  For bools it is more problematic.
template<typename T>
struct declare_verbatim_formatter< endian_formatter<(arbitrary_format_endian::order::native == arbitrary_format_endian::order::big) ? arbitrary_format_endian::order::little : arbitrary_format_endian::order::big, 1>, T > : public std::integral_constant<bool, (sizeof(T) == 1) && std::is_pod<T>::value>::type
{};

template<typename T>
struct declare_verbatim_formatter< endian_formatter<arbitrary_format_endian::order::native, sizeof(T)>, T > : public std::integral_constant<bool, std::is_pod<T>::value>::type
{};

static_assert(is_verbatim_formatter< little_endian<1>, uint8_t >::value, "little_endian<1> should be a verbatim formatter for uint8_t.");
static_assert(is_verbatim_formatter< big_endian<1>, uint8_t >::value, "big_endian<1> should be a verbatim formatter for uint8_t.");
static_assert(is_verbatim_formatter< endian_formatter<arbitrary_format_endian::order::native, 4>, uint32_t >::value, "Native endian_formatter should be a verbatim formatter.");
static_assert((arbitrary_format_endian::order::native != arbitrary_format_endian::order::little) || is_verbatim_formatter< little_endian<4>, uint32_t >::value, "little_endian should be a verbatim formatter on little endian machines.");
static_assert((arbitrary_format_endian::order::native != arbitrary_format_endian::order::little) || !is_verbatim_formatter< big_endian<4>, uint32_t >::value, "big_endian should not be a verbatim formatter on little endian machines.");
static_assert((arbitrary_format_endian::order::native != arbitrary_format_endian::order::big) || is_verbatim_formatter< big_endian<4>, uint32_t >::value, "big_endian should be a verbatim formatter on big endian machines.");
static_assert((arbitrary_format_endian::order::native != arbitrary_format_endian::order::big) || !is_verbatim_formatter< little_endian<4>, uint32_t >::value, "little_endian should not be a verbatim formatter on big endian machines.");

} // namespace binary
} // namespace arbitrary_format

#endif // ArbitraryFormatSerializer_endian_formatter_H
