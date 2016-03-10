/////////////////////////////////////////////////////////////////////////////
/// ArbitraryFormatSerializer
///    Library for serializing data in arbitrary formats.
///
/// implement_save_load_serialize.h
///
/// This file contains implement_save_load mixin class that provides save() and load() by the means of the save_or_load() method
///
/// Distributed under Apache License, Version 2.0 (http://www.apache.org/licenses/LICENSE-2.0)
/// (c) 2014 Zbigniew Skowron, zbychs@gmail.com
///
/////////////////////////////////////////////////////////////////////////////

#ifndef ArbitraryFormatSerializer_implement_save_load_serialize_H
#define ArbitraryFormatSerializer_implement_save_load_serialize_H

#include <arbitrary_format/utility/has_method.h>

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

AFS_GENERATE_HAS_METHOD(save);
AFS_GENERATE_HAS_METHOD(load);
AFS_GENERATE_HAS_METHOD(save_or_load);
AFS_GENERATE_HAS_METHOD(saving);

#define AFS_HAS_METHOD_0(objectType, method) \
    typename std::enable_if< has_method_ ## method <objectType, void() >::value>::type

#define AFS_HAS_METHOD_2(objectType, method, arg1Type, arg2Type) \
    typename std::enable_if< has_method_ ## method <objectType, void(arg1Type, arg2Type) >::value>::type

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
    /// @note has_save_or_load was used below to generate const methods only where underlying formatter has a const method. But the compiler errors were not very good then.

    template<typename T, typename TSerializer>
    void save(TSerializer& serializer, const T& object)
    {
        static_assert(is_saving_serializer<TSerializer>::value || !is_loading_serializer<TSerializer>::value, "Can't save to a loading serializer.");
        static_assert(has_save_or_load<Derived, TSerializer, typename std::remove_cv<T>::type>::value, "Formatter can't save given type to given serializer.");
        static_cast<Derived*>(this)->save_or_load(serializer, const_cast<typename std::remove_cv<T>::type&>(object));
    }

    template<typename T, typename TSerializer>
    void load(TSerializer& serializer, T& object)
    {
        static_assert(is_loading_serializer<TSerializer>::value || !is_saving_serializer<TSerializer>::value, "Can't load from a saving serializer.");
        static_assert(has_save_or_load<Derived, TSerializer, T>::value, "Formatter can't load given type from given serializer.");
        static_cast<Derived*>(this)->save_or_load(serializer, object);
    }

    template<typename T, typename TSerializer>
    // typename std::enable_if< has_save_or_load<Derived, TSerializer, typename std::remove_cv<T>::type>::value >::type
    void save(TSerializer& serializer, const T& object) const
    {
        static_assert(is_saving_serializer<TSerializer>::value || !is_loading_serializer<TSerializer>::value, "Can't save to a loading serializer.");
        static_assert(has_save_or_load<const Derived, TSerializer, typename std::remove_cv<T>::type>::value, "Const formatter can't save given type to given serializer.");
        static_cast<const Derived*>(this)->save_or_load(serializer, const_cast<typename std::remove_cv<T>::type&>(object));
    }

    template<typename T, typename TSerializer>
    // typename std::enable_if< has_save_or_load<Derived, TSerializer, T>::value >::type
    void load(TSerializer& serializer, T& object) const
    {
        static_assert(is_loading_serializer<TSerializer>::value || !is_saving_serializer<TSerializer>::value, "Can't load from a saving serializer.");
        static_assert(has_save_or_load<const Derived, TSerializer, T>::value, "Const formatter can't load given type from given serializer.");
        static_cast<const Derived*>(this)->save_or_load(serializer, object);
    }
};

/////////////////////////////////////////////////////////////////////////////

template<typename TSerializer>
typename std::enable_if< is_saving_serializer<TSerializer>::value && !is_loading_serializer<TSerializer>::value, bool >::type
is_serializer_saving(TSerializer& serializer)
{
    return true;
}

template<typename TSerializer>
typename std::enable_if< is_loading_serializer<TSerializer>::value && !is_saving_serializer<TSerializer>::value, bool>::type
is_serializer_saving(TSerializer& serializer)
{
    return false;
}

template<typename TSerializer>
typename std::enable_if< !is_saving_serializer<TSerializer>::value && !is_loading_serializer<TSerializer>::value, bool >::type
is_serializer_saving(TSerializer& serializer)
{
    static_assert(false && (sizeof(TSerializer) >= 0), "Serializer isn't specifically a loading or saving serializer.");
}

template<typename TSerializer>
typename std::enable_if< is_saving_serializer<TSerializer>::value && is_loading_serializer<TSerializer>::value, bool >::type
is_serializer_saving(TSerializer& serializer)
{
    static_assert(has_saving<const TSerializer>::value, "Formatter that is both saving and loading must have an instance, const saving() method.");
    return serializer.saving();
}

/////////////////////////////////////////////////////////////////////////////

} // namespace arbitrary_format

#endif // ArbitraryFormatSerializer_implement_save_load_serialize_H
