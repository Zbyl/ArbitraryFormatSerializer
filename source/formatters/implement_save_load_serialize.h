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

// no expression sfinae
#define AFS_HAS_METHOD_0(objectType, method) void
#define AFS_HAS_METHOD_2(objectType, method, arg1Type, arg2Type) void

#else

#define AFS_HAS_METHOD_0(objectType, method) \
    decltype( void(std::declval<objectType>().serialize()) )

#define AFS_HAS_METHOD_2(objectType, method, arg1Type, arg2Type) \
    decltype( void(std::declval<objectType>().serialize( std::declval<arg1Type>(), std::declval<arg2Type>() )) )

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
struct has_serialize_impl
{
    using type = std::false_type;
};

template<typename TFormatter, typename TSerializer, typename TValue>
struct has_serialize_impl<TFormatter, TSerializer, TValue, AFS_HAS_METHOD_2(TFormatter, serialize, TSerializer&, TValue&)>
{
    using type = std::true_type;
};

template<typename TFormatter, typename TSerializer, typename TValue>
using has_serialize = typename has_serialize_impl<TFormatter, TSerializer, TValue>::type;

/////////////////////////////////////////////////////////////////////////////

template<typename TSerializer, typename Enable = void>
struct can_save_impl
{
    using type = std::false_type;
};

template<typename TSerializer>
struct can_save_impl<TSerializer, AFS_HAS_MEMBER_TYPE(TSerializer, serializer_can_save)>
{
    using type = std::true_type;
};

template<typename TSerializer>
using can_save = typename can_save_impl<TSerializer>::type;

/////////////////////////////////////////////////////////////////////////////

template<typename TSerializer, typename Enable = void>
struct can_load_impl
{
    using type = std::false_type;
};

template<typename TSerializer>
struct can_load_impl<TSerializer, AFS_HAS_MEMBER_TYPE(TSerializer, serializer_can_load)>
{
    using type = std::true_type;
};

template<typename TSerializer>
using can_load = typename can_load_impl<TSerializer>::type;

/////////////////////////////////////////////////////////////////////////////

template<typename Derived>
class implement_save_load
{
public:
    template<typename T, typename TSerializer, std::enable_if_t< has_serialize<Derived, TSerializer, std::remove_cv_t<T>>::value >* = nullptr>
    void save(TSerializer& serializer, const T& object)
    {
        static_cast<Derived*>(this)->serialize(serializer, const_cast<std::remove_cv_t<T>&>(object));
    }

    template<typename T, typename TSerializer, std::enable_if_t< has_serialize<Derived, TSerializer, T>::value >* = nullptr>
    void load(TSerializer& serializer, T& object)
    {
        static_cast<Derived*>(this)->serialize(serializer, object);
    }

    template<typename T, typename TSerializer, std::enable_if_t< has_serialize<const Derived, TSerializer, std::remove_cv_t<T>>::value >* = nullptr>
    void save(TSerializer& serializer, const T& object) const
    {
        static_cast<const Derived*>(this)->serialize(serializer, const_cast<std::remove_cv_t<T>&>(object));
    }

    template<typename T, typename TSerializer, std::enable_if_t< has_serialize<const Derived, TSerializer, T>::value >* = nullptr>
    void load(TSerializer& serializer, T& object) const
    {
        static_cast<const Derived*>(this)->serialize(serializer, object);
    }
};

template<typename Derived>
class implement_serialize
{
public:
    template<typename T, typename TSerializer, std::enable_if_t< has_save<Derived, TSerializer, T>::value && has_load<Derived, TSerializer, T>::value && can_save<TSerializer>::value && can_load<TSerializer>::value >* = nullptr>
    void serialize(TSerializer& serializer, T& object)
    {
        if (serializer.saving())
        {
            static_cast<Derived*>(this)->save(serializer, object);
        }
        else
        {
            static_cast<Derived*>(this)->load(serializer, object);
        }
    }

    template<typename T, typename TSerializer, std::enable_if_t< has_save<Derived, TSerializer, T>::value && can_save<TSerializer>::value && !can_load<TSerializer>::value >* = nullptr>
    void serialize(TSerializer& serializer, T& object)
    {
        static_cast<Derived*>(this)->save(serializer, object);
    }

    template<typename T, typename TSerializer, std::enable_if_t< has_load<Derived, TSerializer, T>::value && !can_save<TSerializer>::value && can_load<TSerializer>::value >* = nullptr>
    void serialize(TSerializer& serializer, T& object)
    {
        static_cast<Derived*>(this)->load(serializer, object);
    }

    template<typename T, typename TSerializer, std::enable_if_t< has_save<const Derived, TSerializer, T>::value && has_load<const Derived, TSerializer, T>::value && can_save<TSerializer>::value && can_load<TSerializer>::value >* = nullptr>
    void serialize(TSerializer& serializer, T& object) const
    {
        if (serializer.saving())
        {
            static_cast<const Derived*>(this)->save(serializer, object);
        }
        else
        {
            static_cast<const Derived*>(this)->load(serializer, object);
        }
    }

    template<typename T, typename TSerializer, std::enable_if_t< has_save<const Derived, TSerializer, T>::value && can_save<TSerializer>::value && !can_load<TSerializer>::value >* = nullptr>
    void serialize(TSerializer& serializer, T& object) const
    {
        static_cast<const Derived*>(this)->save(serializer, object);
    }

    template<typename T, typename TSerializer, std::enable_if_t< has_load<const Derived, TSerializer, T>::value && !can_save<TSerializer>::value && can_load<TSerializer>::value >* = nullptr>
    void serialize(TSerializer& serializer, T& object) const
    {
        static_cast<const Derived*>(this)->load(serializer, object);
    }
};

/////////////////////////////////////////////////////////////////////////////

} // namespace arbitrary_format

#endif // ArbitraryFormatSerializer_implement_save_load_serialize_H
