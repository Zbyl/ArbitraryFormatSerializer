/////////////////////////////////////////////////////////////////////////////
/// ArbitraryFormatSerializer
///    Library for serializing data in arbitrary formats.
///
/// serialization.h
///
/// This file contains serialization function.
///
/// Distributed under Apache License, Version 2.0 (http://www.apache.org/licenses/LICENSE-2.0)
/// (c) 2014 Zbigniew Skowron, zbychs@gmail.com
///
/////////////////////////////////////////////////////////////////////////////

#ifndef ArbitraryFormatSerializer_serialization_H
#define ArbitraryFormatSerializer_serialization_H

#include <arbitrary_format/implement_save_load_serialize.h>
#include <arbitrary_format/serialization_exceptions.h>

#include <utility>
#include <cstdint>

namespace arbitrary_format
{

/// @brief Saves given object using specified serializer and formatter.
template<typename Formatter, typename T, typename TSerializer>
void save(TSerializer& serializer, const T& object, Formatter&& formatter = Formatter())
{
    static_assert(is_saving_serializer<TSerializer>::value || !is_loading_serializer<TSerializer>::value, "Can't save to a loading serializer.");
    std::forward<Formatter>(formatter).save(serializer, object);
}

/// @brief Loads given object using specified serializer and formatter.
template<typename Formatter, typename T, typename TSerializer>
void load(TSerializer& serializer, T&& object, Formatter&& formatter = Formatter())
{
    static_assert(is_loading_serializer<TSerializer>::value || !is_saving_serializer<TSerializer>::value, "Can't load from a saving serializer.");
    std::forward<Formatter>(formatter).load(serializer, std::forward<T>(object));
}

/// @brief Serializes given object using specified serializer and formatter.
///        Calls save() or load() depending on whether serializer is a saving or loading serializer.
template<typename Formatter, typename T, typename TSerializer>
typename std::enable_if< is_saving_serializer<TSerializer>::value && !is_loading_serializer<TSerializer>::value >::type
serialize(TSerializer& serializer, T&& object, Formatter&& formatter = Formatter())
{
    std::forward<Formatter>(formatter).save(serializer, std::forward<T>(object));
}

/// @brief Serializes given object using specified serializer and formatter.
///        Calls save() or load() depending on whether serializer is a saving or loading serializer.
template<typename Formatter, typename T, typename TSerializer>
typename std::enable_if< is_loading_serializer<TSerializer>::value && !is_saving_serializer<TSerializer>::value >::type
serialize(TSerializer& serializer, T&& object, Formatter&& formatter = Formatter())
{
    std::forward<Formatter>(formatter).load(serializer, std::forward<T>(object));
}

/// @brief Serializes given object using specified serializer and formatter.
///        Calls save() or load() depending on whether serializer is a saving or loading serializer.
template<typename Formatter, typename T, typename TSerializer>
typename std::enable_if< !is_saving_serializer<TSerializer>::value && !is_loading_serializer<TSerializer>::value >::type
serialize(TSerializer& serializer, T&& object, Formatter&& formatter = Formatter())
{
    static_assert(false && (sizeof(TSerializer) > 0), "Can't call serialize() function for a serializer that isn't specifically loading or saving serializer.");
}

/// @brief Serializes given object using specified serializer and formatter.
///        Calls save() or load() depending on whether serializer is a saving or loading serializer.
template<typename Formatter, typename T, typename TSerializer>
typename std::enable_if< is_saving_serializer<TSerializer>::value && is_loading_serializer<TSerializer>::value >::type
serialize(TSerializer& serializer, T&& object, Formatter&& formatter = Formatter())
{
    static_assert(false && (sizeof(TSerializer) > 0), "Can't call serialize() function for a serializer that is both loading and saving serializer. Use slow_serialize() to serialize with runtime check.");
}

/// @brief Serializes given object using specified serializer and formatter.
///        Calls save() or load() depending on whether serializer is a saving or loading serializer.
template<typename Formatter, typename T, typename TSerializer>
typename std::enable_if< is_saving_serializer<TSerializer>::value && is_loading_serializer<TSerializer>::value >::type
slow_serialize(TSerializer& serializer, T&& object, Formatter&& formatter = Formatter())
{
    static_assert(has_saving<TSerializer>::value, "Formatter that is both saving and loading must have an instance, const saving() method.");
    if (serializer.saving())
    {
        std::forward<Formatter>(formatter).save(serializer, std::forward<T>(object));
    }
    else
    {
        std::forward<Formatter>(formatter).load(serializer, std::forward<T>(object));
    }
}

} // namespace arbitrary_format

#endif // ArbitraryFormatSerializer_serialization_H
