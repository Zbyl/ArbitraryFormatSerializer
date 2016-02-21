/////////////////////////////////////////////////////////////////////////////
/// ArbitraryFormatSerializer
///    Library for serializing data in arbitrary formats.
///
/// implement_save_load_serialize.h
///
/// This file contains implement_save_load mixin class that provides save() and load() by the means of the serialize() method,
/// and implementserialize mixin class that provides serialize() by the means of the save() and load() methods.
///
/// Distributed under Apache License, Version 2.0 (http://www.apache.org/licenses/LICENSE-2.0)
/// (c) 2014 Zbigniew Skowron, zbychs@gmail.com
///
/////////////////////////////////////////////////////////////////////////////

#ifndef ArbitraryFormatSerializer_implement_save_load_serialize_H
#define ArbitraryFormatSerializer_implement_save_load_serialize_H

#include <type_traits>

namespace arbitrary_format
{

/////////////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER) && (_MSC_VER < 1900)
#else
#define AFS_HAVE_EXPRESSION_SFINAE
#endif

#ifdef AFS_HAVE_EXPRESSION_SFINAE

/// @note: Rewrite using Boost Type Traits Introspection library when using newer Boost will be an option.

#define AFS_HAS_METHOD_0(objectType, method) \
    decltype( void(std::declval<objectType>().method()) )

#define AFS_HAS_METHOD_2(objectType, method, arg1Type, arg2Type) \
    decltype( void(std::declval<objectType>().method( std::declval<arg1Type>(), std::declval<arg2Type>() )) )

#else

// no expression sfinae
#define AFS_HAS_METHOD_0(objectType, method) void
#define AFS_HAS_METHOD_2(objectType, method, arg1Type, arg2Type) void

#endif

#define AFS_HAS_MEMBER_TYPE(objectType, typeName) \
    typename std::enable_if<std::is_pointer< typename objectType::typeName* >::value>::type

/////////////////////////////////////////////////////////////////////////////

template<typename TFormatter, typename TSerializer, typename TValue, typename Enable = void>
struct has_save_impl
{
    using type = std::false_type;
};

template<typename TFormatter, typename TSerializer, typename TValue>
struct has_save_impl<TFormatter, TSerializer, TValue, AFS_HAS_METHOD_2(TFormatter, save, TSerializer&, const TValue&)>
{
    using type = std::true_type;
};

template<typename TFormatter, typename TSerializer, typename TValue>
using has_save = typename has_save_impl<TFormatter, TSerializer, TValue>::type;

/////////////////////////////////////////////////////////////////////////////

template<typename TFormatter, typename TSerializer, typename TValue, typename Enable = void>
struct has_load_impl
{
    using type = std::false_type;
};

template<typename TFormatter, typename TSerializer, typename TValue>
struct has_load_impl<TFormatter, TSerializer, TValue, AFS_HAS_METHOD_2(TFormatter, load, TSerializer&, const TValue&)>
{
    using type = std::true_type;
};

template<typename TFormatter, typename TSerializer, typename TValue>
using has_load = typename has_load_impl<TFormatter, TSerializer, TValue>::type;

/////////////////////////////////////////////////////////////////////////////

template<typename TFormatter, typename TSerializer, typename TValue, typename Enable = void>
struct has_save_or_load_impl
{
    using type = std::false_type;
};

template<typename TFormatter, typename TSerializer, typename TValue>
struct has_save_or_load_impl<TFormatter, TSerializer, TValue, AFS_HAS_METHOD_2(TFormatter, save_or_load, TSerializer&, TValue&)>
{
    using type = std::true_type;
};

template<typename TFormatter, typename TSerializer, typename TValue>
using has_save_or_load = typename has_save_or_load_impl<TFormatter, TSerializer, TValue>::type;

/////////////////////////////////////////////////////////////////////////////

template<typename TSerializer, typename Enable = void>
struct has_saving_impl
{
    using type = std::false_type;
};

template<typename TSerializer>
struct has_saving_impl<TSerializer, AFS_HAS_METHOD_0(TSerializer, saving)>
{
    using type = std::true_type;
};

template<typename TSerializer>
using has_saving = typename has_saving_impl<TSerializer>::type;

/////////////////////////////////////////////////////////////////////////////

/// @brief Specialize this template for types that cannot have member type saving_serializer
template<typename TSerializer, typename Enable = void>
struct is_saving_serializer : public std::false_type {};

template<typename TSerializer>
struct is_saving_serializer<TSerializer, AFS_HAS_MEMBER_TYPE(TSerializer, saving_serializer)> : TSerializer::saving_serializer {};

/////////////////////////////////////////////////////////////////////////////

/// @brief Specialize this template for types that cannot have member type saving_serializer
template<typename TSerializer, typename Enable = void>
struct is_loading_serializer : public std::false_type {};

template<typename TSerializer>
struct is_loading_serializer<TSerializer, AFS_HAS_MEMBER_TYPE(TSerializer, loading_serializer)> : TSerializer::loading_serializer {};

/////////////////////////////////////////////////////////////////////////////

/// @brief Implements save() and load() methods in terms of the save_or_load() method.
template<typename Derived>
class implement_save_load
{
public:
    /// @note has_save_or_load is used below to limit specialization only for types supported by underlying serializer and formatter.

    template<typename T, typename TSerializer>
    void save(TSerializer& serializer, const T& object)
    {
        static_assert(is_saving_serializer<TSerializer>::value || !is_loading_serializer<TSerializer>::value, "Can't save to a loading serializer.");
        static_assert(has_save_or_load<Derived, TSerializer, std::remove_cv_t<T>>::value, "Formatter can't save given type to given serializer.");
        static_cast<Derived*>(this)->save_or_load(serializer, const_cast<std::remove_cv_t<T>&>(object));
    }

    template<typename T, typename TSerializer>
    void load(TSerializer& serializer, T& object)
    {
        static_assert(is_loading_serializer<TSerializer>::value || !is_saving_serializer<TSerializer>::value, "Can't load from a saving serializer.");
        static_assert(has_save_or_load<Derived, TSerializer, T>::value, "Formatter can't load given type from given serializer.");
        static_cast<Derived*>(this)->save_or_load(serializer, object);
    }

    template<typename T, typename TSerializer, std::enable_if_t< has_save_or_load<Derived, TSerializer, std::remove_cv_t<T>>::value >* = nullptr>
    void save(TSerializer& serializer, const T& object) const
    {
        static_assert(is_saving_serializer<TSerializer>::value || !is_loading_serializer<TSerializer>::value, "Can't save to a loading serializer.");
        static_assert(has_save_or_load<const Derived, TSerializer, std::remove_cv_t<T>>::value, "Only non-const formatter can save given type to given serializer.");
        static_cast<const Derived*>(this)->save_or_load(serializer, const_cast<std::remove_cv_t<T>&>(object));
    }

    template<typename T, typename TSerializer, std::enable_if_t< has_save_or_load<Derived, TSerializer, T>::value >* = nullptr>
    void load(TSerializer& serializer, T& object) const
    {
        static_assert(is_loading_serializer<TSerializer>::value || !is_saving_serializer<TSerializer>::value, "Can't load from a saving serializer.");
        static_assert(has_save_or_load<const Derived, TSerializer, T>::value, "Only non-const formatter can load given type from given serializer.");
        static_cast<const Derived*>(this)->save_or_load(serializer, object);
    }
};

/////////////////////////////////////////////////////////////////////////////

template<typename TSerializer, std::enable_if_t< is_saving_serializer<TSerializer>::value && !is_loading_serializer<TSerializer>::value >* = nullptr>
bool is_serializer_saving(TSerializer& serializer)
{
    return true;
}

template<typename TSerializer, std::enable_if_t< is_loading_serializer<TSerializer>::value && !is_saving_serializer<TSerializer>::value>* = nullptr>
bool is_serializer_saving(TSerializer& serializer)
{
    return false;
}

template<typename TSerializer, std::enable_if_t< !is_saving_serializer<TSerializer>::value && !is_loading_serializer<TSerializer>::value >* = nullptr>
bool is_serializer_saving(TSerializer& serializer)
{
    static_assert(false, "Serializer isn't specifically a loading or saving serializer.");
}

template<typename TSerializer, std::enable_if_t< is_saving_serializer<TSerializer>::value && is_loading_serializer<TSerializer>::value >* = nullptr>
bool is_serializer_saving(TSerializer& serializer)
{
    static_assert(has_saving<const TSerializer>::value, "Formatter that is both saving and loading must have an instance, const saving() method.");
    return serializer.saving();
}

/////////////////////////////////////////////////////////////////////////////

} // namespace arbitrary_format

#endif // ArbitraryFormatSerializer_implement_save_load_serialize_H
