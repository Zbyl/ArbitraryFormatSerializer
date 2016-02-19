/////////////////////////////////////////////////////////////////////////////
/// ArbitraryFormatSerializer
///    Library for serializing data in arbitrary formats.
///
/// ISerializer.h
///
/// This file contains ISerializer interface that represents a source or destination for serialization.
///
/// Distributed under Apache License, Version 2.0 (http://www.apache.org/licenses/LICENSE-2.0)
/// (c) 2014 Zbigniew Skowron, zbychs@gmail.com
///
/////////////////////////////////////////////////////////////////////////////

#ifndef ArbitraryFormatSerializer_ISerializer_H
#define ArbitraryFormatSerializer_ISerializer_H

#include "serialization_exceptions.h"
#include "utility/has_member.h"

#include <utility>

#include <boost/cstdint.hpp>

namespace arbitrary_format
{
namespace binary
{

/// @brief Saves given object using specified serializer and formatter.
///        Will throw serialization_exception if serializer is a loading serializer.
template<typename Formatter, typename T, typename TSerializer>
void save(TSerializer& serializer, const T& object, Formatter&& formatter = Formatter())
{
    if (!serializer.saving())
    {
        BOOST_THROW_EXCEPTION(serialization_exception() << detail::errinfo_description("Can't save to a loading serializer."));
    }

    std::forward<Formatter>(formatter).save(serializer, object);
}

/// @brief Loads given object using specified serializer and formatter.
///        Will throw serialization_exception if serializer is a saving serializer.
template<typename Formatter, typename T, typename TSerializer>
void load(TSerializer& serializer, T&& object, Formatter&& formatter = Formatter())
{
    if (!serializer.loading())
    {
        BOOST_THROW_EXCEPTION(serialization_exception() << detail::errinfo_description("Can't load from a saving serializer."));
    }

    std::forward<Formatter>(formatter).load(serializer, std::forward<T>(object));
}

/// @brief Serializes given object using specified serializer and formatter.
///        Calls save() or load() depending on whether serializer if a saving or loading serializer.
template<typename Formatter, typename T, typename TSerializer>
void serialize(TSerializer& serializer, T&& object, Formatter&& formatter = Formatter())
{
    if (serializer.saving())
    {
        std::forward<Formatter>(formatter).save(serializer, std::forward<T>(object));
    }
    else
    {
        std::forward<Formatter>(formatter).load(serializer, std::forward<T>(object));
    }
}

/// @brief SerializerMixin is a class that provides implementation of the basic Serializer methods.
template<typename Derived>
class SerializerMixin
{
public:
    /// @brief Saves given object using specified formatter.
    ///        Will throw serialization_exception if serializer is a loading serializer.
    template<typename Formatter, typename T>
    void save(const T& object, Formatter&& formatter = Formatter())
    {
        Derived* derived = static_cast<Derived*>(this);
        if (!derived->saving())
        {
            BOOST_THROW_EXCEPTION(serialization_exception() << detail::errinfo_description("Can't save to a loading serializer."));
        }

        std::forward<Formatter>(formatter).save(*derived, object);
    }

    /// @brief Loads given object using specified formatter.
    ///        Will throw serialization_exception if serializer is a saving serializer.
    template<typename Formatter, typename T>
    void load(T&& object, Formatter&& formatter = Formatter())
    {
        Derived* derived = static_cast<Derived*>(this);
        if (!derived->loading())
        {
            BOOST_THROW_EXCEPTION(serialization_exception() << detail::errinfo_description("Can't load from a saving serializer."));
        }

        std::forward<Formatter>(formatter).load(*derived, std::forward<T>(object));
    }

#if 0
    /// @brief Loads given object of given size using specified formatter.
    ///        Will throw serialization_exception if serializer is a saving serializer.
    template<typename Formatter, typename T, typename TSize>
    void load(T&& object, TSize byteSize, Formatter&& formatter = Formatter())
    {
        if (!loading())
        {
            BOOST_THROW_EXCEPTION(serialization_exception() << detail::errinfo_description("Can't load from a saving serializer."));
        }

        Derived* derived = static_cast<Derived*>(this);
        std::forward<Formatter>(formatter).load(*derived, byteSize, std::forward<T>(object));
    }
#endif

    /// @brief Serializes given object using specified formatter.
    ///        Calls save() or load() depending on whether serializer if a saving or loading serializer.
    template<typename Formatter, typename T>
    void serialize(T&& object, Formatter&& formatter = Formatter())
    {
        Derived* derived = static_cast<Derived*>(this);
        if (derived->saving())
        {
            std::forward<Formatter>(formatter).save(*derived, std::forward<T>(object));
        }
        else
        {
            std::forward<Formatter>(formatter).load(*derived, std::forward<T>(object));
        }
    }

    /// @brief Returs true if serializer is a loading serializer.
    bool loading()
    {
        Derived* derived = static_cast<Derived*>(this);
        return !derived->saving();
    }

public:
    void saveData(const boost::uint8_t* data, size_t size)
    {
        Derived* derived = static_cast<Derived*>(this);
        derived->serializeData(const_cast<boost::uint8_t*>(data), size);
    }

    void loadData(boost::uint8_t* data, size_t size)
    {
        Derived* derived = static_cast<Derived*>(this);
        derived->serializeData(data, size);
    }
};

/// @brief ISerializer is a base class for polymorphic serializers.
class ISerializer : public SerializerMixin<ISerializer>
{
public:
    /// @brief Virtual destructor.
    virtual ~ISerializer() {}

    /// @brief Returs true if serializer is a saving serializer.
    virtual bool saving() = 0;

public:
    /// @brief Serializes a buffer of bytes.
    virtual void serializeData(boost::uint8_t* data, size_t size) = 0;
};    

class ISeekableSerializer : public virtual ISerializer, public SerializerMixin<ISeekableSerializer>
{
public:
    using SerializerMixin<ISeekableSerializer>::save;
    using SerializerMixin<ISeekableSerializer>::load;
    using SerializerMixin<ISeekableSerializer>::serialize;
    using SerializerMixin<ISeekableSerializer>::loading;
    using SerializerMixin<ISeekableSerializer>::saveData;
    using SerializerMixin<ISeekableSerializer>::loadData;

    /// @brief Type used for offsets and positions in stream of serialized data.
    typedef boost::intmax_t offset_t;

    /// @brief Returns current position.
    virtual offset_t position() = 0;

    /// @brief Seeks to given position.
    virtual void seek(offset_t position) = 0;
};


/// @brief AnySerializer is a decorator, that converts a non-polymorphic serializer into a polymorphic serializer.
template<typename TSerializer>
class AnySerializer : public virtual ISerializer
{
public:
    explicit AnySerializer(TSerializer& serializer)
        : serializer(serializer)
    {
    }

private:
    virtual bool saving()
    {
        return serializer.saving();
    }

    virtual void serializeData(boost::uint8_t* data, size_t size)
    {
        return serializer.serializeData(data, size);
    }

private:
    TSerializer& serializer;
};

GENERATE_HAS_MEMBER(position);
GENERATE_HAS_MEMBER(seek);

/// @brief AnySerializer is a decorator, that converts a non-polymorphic serializer into a polymorphic serializer.
/// @param ForceCreate  If true, then type will be created, but missing methods in TSerializer will result in generation of methods that throw not_implemented exception.
///                     If false, then type will not be created if there are any missing methods in TSerializer.
template<typename TSerializer, bool ForceCreate = false>
class AnySeekableSerializer : public virtual AnySerializer<TSerializer>, public virtual ISeekableSerializer
{
public:
    explicit AnySeekableSerializer(TSerializer& serializer)
        : AnySerializer<TSerializer>(serializer)
        , serializer(serializer)
    {
    }

private:
    virtual offset_t position()
    {
        return position_impl(static_cast<TSerializer*>(nullptr));
    }

    virtual void seek(offset_t position)
    {
        return seek_impl(position, static_cast<TSerializer*>(nullptr));
    }

private:
    template<typename TSerializer>
    typename std::enable_if<has_member_position<TSerializer>::value, offset_t>::type
    position_impl(TSerializer* dummy)
    {
        return serializer.position();
    }

    template<typename TSerializer>
    typename std::enable_if<!has_member_position<TSerializer>::value && ForceCreate, offset_t>::type
    position_impl(TSerializer* dummy)
    {
        BOOST_THROW_EXCEPTION(not_implemented());
    }

    template<typename TSerializer>
    typename std::enable_if<has_member_seek<TSerializer>::value, void>::type
    seek_impl(offset_t position, TSerializer* dummy)
    {
        return serializer.seek(position);
    }

    template<typename TSerializer>
    typename std::enable_if<!has_member_seek<TSerializer>::value && ForceCreate, void>::type
    seek_impl(offset_t position, TSerializer* dummy)
    {
        BOOST_THROW_EXCEPTION(not_implemented());
    }

private:
    TSerializer& serializer;
};

template<typename T>
AnySeekableSerializer<T> make_serializer(T& serializer)
{
    return AnySeekableSerializer<T>(serializer);
}

template<typename T>
AnySeekableSerializer<T> make_seekable_serializer(T& serializer)
{
    return AnySeekableSerializer<T>(serializer);
}

template<typename T>
AnySeekableSerializer<T, true> make_seekable_serializer_force(T& serializer)
{
    return AnySeekableSerializer<T, true>(serializer);
}

} // namespace binary
} // namespace arbitrary_format

#endif // ArbitraryFormatSerializer_ISerializer_H
