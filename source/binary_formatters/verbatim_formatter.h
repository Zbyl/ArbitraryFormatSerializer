/////////////////////////////////////////////////////////////////////////////
/// ArbitraryFormatSerializer
///    Library for serializing data in arbitrary formats.
///
/// verbatim_formatter.h
///
/// This file contains verbatim_formatter, that formats data the same way they are stored in memory. It can be used only for PODs.
/// It also contains is_verbatim_formatter type trait, that checks if formatter will serialize data the same way as verbatim formatter, which allows for certain optimizations.
///
/// Distributed under Apache License, Version 2.0 (http://www.apache.org/licenses/LICENSE-2.0)
/// (c) 2014 Zbigniew Skowron, zbychs@gmail.com
///
/////////////////////////////////////////////////////////////////////////////

#ifndef ArbitraryFormatSerializer_verbatim_formatter_H
#define ArbitraryFormatSerializer_verbatim_formatter_H

#include <type_traits>

namespace arbitrary_format
{
namespace binary
{

template<int Size>
class verbatim_formatter
{
public:
    /// @brief Stores given pod the same way as it's stored in memory.
    template<typename T, typename TSerializer>
    void save(TSerializer& serializer, const T& pod) const
    {
        static_assert(std::is_pod<T>::value, "verbatim_formatter can store only PODs");
        static_assert(sizeof(T) == Size, "verbatim_formatter<Size> can store only PODs of size == Size");
        serializer.saveData(reinterpret_cast<boost::uint8_t*>(&pod), Size);
    }

    /// @brief Loads given pod the same way as it's stored in memory.
    template<typename T, typename TSerializer>
    void load(TSerializer& serializer, T& pod) const
    {
        static_assert(std::is_pod<T>::value, "verbatim_formatter can store only PODs");
        static_assert(sizeof(T) == Size, "verbatim_formatter<Size> can store only PODs of size == Size");
        serializer.loadData(reinterpret_cast<boost::uint8_t*>(&pod), Size);
    }
};

/// @note is_verbatim_formatter type trait is intended to be specialized for other foratters. little_endian is also a verbatim_formatter.
/// @brief is_verbatim_formatter is a false_type if formatter will serialize given type differently than verbatim_formatter<sizeof(T)>.
/// @note Last type parameter is to allow for enable_if usage in specializations. See endian_formatter for an example.
template<typename Formatter, typename T, typename = void>
struct is_verbatim_formatter : public std::false_type
{};

/// @brief is_verbatim_formatter is a true_type if formatter will serialize given type the same way as verbatim_formatter<sizeof(T)>.
template<typename T>
struct is_verbatim_formatter< verbatim_formatter<sizeof(T)>, T > : public std::true_type
{};

} // namespace binary
} // namespace arbitrary_format

#endif // ArbitraryFormatSerializer_verbatim_formatter_H
