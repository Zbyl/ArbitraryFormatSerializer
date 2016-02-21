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

#include "formatters/implement_save_load_serialize.h"
#include "serialization_exceptions.h"
#include "utility/has_member.h"

#include <utility>

#include <cstdint>

namespace arbitrary_format
{
namespace binary
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
///        Calls save() or load() depending on whether serializer if a saving or loading serializer.
template<typename Formatter, typename T, typename TSerializer, std::enable_if_t< is_saving_serializer<TSerializer>::value && !is_loading_serializer<TSerializer>::value >* = nullptr>
void serialize(TSerializer& serializer, T&& object, Formatter&& formatter = Formatter())
{
    std::forward<Formatter>(formatter).save(serializer, std::forward<T>(object));
}

/// @brief Serializes given object using specified serializer and formatter.
///        Calls save() or load() depending on whether serializer if a saving or loading serializer.
template<typename Formatter, typename T, typename TSerializer, std::enable_if_t< is_loading_serializer<TSerializer>::value && !is_saving_serializer<TSerializer>::value >* = nullptr>
void serialize(TSerializer& serializer, T&& object, Formatter&& formatter = Formatter())
{
    std::forward<Formatter>(formatter).load(serializer, std::forward<T>(object));
}

/// @brief Serializes given object using specified serializer and formatter.
///        Calls save() or load() depending on whether serializer if a saving or loading serializer.
template<typename Formatter, typename T, typename TSerializer, std::enable_if_t< !is_saving_serializer<TSerializer>::value && !is_loading_serializer<TSerializer>::value >* = nullptr>
void serialize(TSerializer& serializer, T&& object, Formatter&& formatter = Formatter())
{
    static_assert(false, "Can't call serialize() function for a serializer that isn't specifically loading or saving serializer.");
}

/// @brief Serializes given object using specified serializer and formatter.
///        Calls save() or load() depending on whether serializer if a saving or loading serializer.
template<typename Formatter, typename T, typename TSerializer, std::enable_if_t< is_saving_serializer<TSerializer>::value && is_loading_serializer<TSerializer>::value >* = nullptr>
void serialize(TSerializer& serializer, T&& object, Formatter&& formatter = Formatter())
{
    static_assert(false, "Can't call serialize() function for a serializer that is both loading and saving serializer. Use slow_serialize() to serialize with runtime check.");
}

/// @brief Serializes given object using specified serializer and formatter.
///        Calls save() or load() depending on whether serializer if a saving or loading serializer.
template<typename Formatter, typename T, typename TSerializer, std::enable_if_t< is_saving_serializer<TSerializer>::value && is_loading_serializer<TSerializer>::value >* = nullptr>
void slow_serialize(TSerializer& serializer, T&& object, Formatter&& formatter = Formatter())
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

/// @brief ISaveSerializer is a base class for polymorphic saving binary serializers.
///        It's slow, because serialization of every chunk is a virtual function call.
/// @todo Implement a "chunking", zero-copy serializer to improve speed.
class ISaveSerializer
{
public:
    /// @brief Virtual destructor.
    virtual ~ISaveSerializer() {}

    using saving_serializer = std::true_type;

    /// @brief Saves a buffer of bytes.
    void saveData(const uint8_t* data, size_t size)
    {
        saveDataImpl(data, size);
    }

protected:
    /// @brief Saves a buffer of bytes.
    virtual void saveDataImpl(const uint8_t* data, size_t size) = 0;
};    

/// @brief ILoadSerializer is a base class for polymorphic loading binary serializers.
///        It's slow, because serialization of every chunk is a virtual function call.
/// @todo Implement a "chunking", zero-copy serializer to improve speed.
class ILoadSerializer
{
public:
    /// @brief Virtual destructor.
    virtual ~ILoadSerializer() {}

    using loading_serializer = std::true_type;

    /// @brief Loads a buffer of bytes.
    void loadData(uint8_t* data, size_t size)
    {
        loadDataImpl(data, size);
    }

protected:
    /// @brief Loads a buffer of bytes.
    virtual void loadDataImpl(uint8_t* data, size_t size) = 0;
};    

/// @brief ISlowSerializer is a base class for polymorphic saving and loading binary serializers.
///        It's called "slow", because, in addition to virtual function call overhead, every use results in a runtime check whether it's a saving or loading serializer.
/// @note  We could implement a "QuickSerializer" that would wrap this one and help compiler to remove many checks, but it's not a good idea to use this serializer in the first place.
class ISlowSerializer : public virtual ISaveSerializer, public virtual ILoadSerializer
{
public:
    /// @brief Returs true if serializer is a saving serializer.
    virtual bool saving() const = 0;

    /// @brief Returs true if serializer is a loading serializer.
    bool loading() const
    {
        return !saving();
    }

protected:
    void saveDataImpl(const uint8_t* data, size_t size) override
    {
        if (!saving())
        {
            BOOST_THROW_EXCEPTION(serialization_exception() << detail::errinfo_description("Can't save to a loading serializer."));
        }

        BOOST_THROW_EXCEPTION(serialization_exception() << detail::errinfo_description("Function should be implemented by subclass."));
    }

    void loadDataImpl(uint8_t* data, size_t size) override
    {
        if (!loading())
        {
            BOOST_THROW_EXCEPTION(serialization_exception() << detail::errinfo_description("Can't load from a saving serializer."));
        }

        BOOST_THROW_EXCEPTION(serialization_exception() << detail::errinfo_description("Function should be implemented by subclass."));
    }
};    

template<typename TSerializer>
struct CantMakePolymorphicSerializer
{
    CantMakePolymorphicSerializer()
    {
        static_assert(false, "Can't make a polymorphic serializer from a serializer that isn't a loading nor a saving serializer.");
    }
};

template<typename TSerializer>
using polymorphic_serializer_t = std::conditional_t<is_saving_serializer<TSerializer>::value,
                                                        std::conditional_t<is_loading_serializer<TSerializer>::value, ISlowSerializer, ISaveSerializer>,
                                                        std::conditional_t<is_loading_serializer<TSerializer>::value, ILoadSerializer, CantMakePolymorphicSerializer<TSerializer>> >;

template<typename TSerializer, typename PolymorphicSerializer = polymorphic_serializer_t<TSerializer>>
class ISeekableSerializer : public virtual PolymorphicSerializer
{
public:
    /// @brief Type used for offsets and positions in stream of serialized data.
    typedef intmax_t offset_t;

    /// @brief Returns current position.
    virtual offset_t position() const = 0;

    /// @brief Seeks to given position.
    virtual void seek(offset_t position) = 0;
};

/// @brief AnySerializer is a decorator, that converts a non-polymorphic serializer into a polymorphic serializer.
template<typename TSerializer, typename PolymorphicSerializer = polymorphic_serializer_t<TSerializer>>
class AnySerializer : public virtual PolymorphicSerializer
{
public:
    explicit AnySerializer(TSerializer& serializer)
        : serializer(serializer)
    {
        static_assert(is_saving_serializer<TSerializer>::value || is_loading_serializer<TSerializer>::value, "Serializer isn't a loading or saving serializer. Don't know how it should work.");
    }

    bool saving() const //override (only for ISlowSerializer)
    {
        return is_serializer_saving(serializer);
    }

private:
    void saveDataImpl(const uint8_t* data, size_t size) //override (only for ISaveSerializer and ISlowSerializer)
    {
        return doSaveData<TSerializer>(data, size);
    }

    void loadDataImpl(uint8_t* data, size_t size) //override (only for ILoadSerializer and ISlowSerializer)
    {
        return doLoadData<TSerializer>(data, size);
    }

    template<typename T, std::enable_if_t< is_saving_serializer<T>::value && !is_loading_serializer<T>::value >* sfinae = nullptr>
    void doSaveData(const uint8_t* data, size_t size)
    {
        return serializer.saveData(data, size);
    }

    template<typename T, std::enable_if_t< is_saving_serializer<T>::value && !is_loading_serializer<T>::value >* sfinae = nullptr>
    void doLoadData(uint8_t* data, size_t size)
    {
        BOOST_THROW_EXCEPTION(serialization_exception() << detail::errinfo_description("Can't load from a saving serializer."));
    }

    template<typename T, std::enable_if_t< is_loading_serializer<T>::value && !is_saving_serializer<T>::value >* sfinae = nullptr>
    void doSaveData(const uint8_t* data, size_t size)
    {
        BOOST_THROW_EXCEPTION(serialization_exception() << detail::errinfo_description("Can't save to a loading serializer."));
    }

    template<typename T, std::enable_if_t< is_loading_serializer<T>::value && !is_saving_serializer<T>::value >* sfinae = nullptr>
    void doLoadData(uint8_t* data, size_t size)
    {
        return serializer.loadData(data, size);
    }

    template<typename T, std::enable_if_t< is_saving_serializer<T>::value && is_loading_serializer<T>::value >* sfinae = nullptr>
    void doSaveData(const uint8_t* data, size_t size)
    {
        if (!saving())
        {
            BOOST_THROW_EXCEPTION(serialization_exception() << detail::errinfo_description("Can't save to a loading serializer."));
        }

        return serializer.saveData(data, size);
    }

    template<typename T, std::enable_if_t< is_saving_serializer<T>::value && is_loading_serializer<T>::value >* sfinae = nullptr>
    void doLoadData(uint8_t* data, size_t size)
    {
        if (!loading())
        {
            BOOST_THROW_EXCEPTION(serialization_exception() << detail::errinfo_description("Can't load from a saving serializer."));
        }

        return serializer.loadData(data, size);
    }

private:
    TSerializer& serializer;
};

GENERATE_HAS_MEMBER(position);
GENERATE_HAS_MEMBER(seek);

/// @brief AnySerializer is a decorator, that converts a non-polymorphic serializer into a polymorphic serializer.
/// @param ForceCreate  If true, then type will be created, but missing methods in TSerializer will result in generation of methods that throw not_implemented exception.
///                     If false, then type will not be created if there are any missing methods in TSerializer.
template<typename TSerializer, bool ForceCreate = false, typename PolymorphicSerializer = polymorphic_serializer_t<TSerializer>>
class AnySeekableSerializer : public virtual AnySerializer<TSerializer, PolymorphicSerializer>, public virtual ISeekableSerializer<TSerializer, PolymorphicSerializer>
{
public:
    explicit AnySeekableSerializer(TSerializer& serializer)
        : AnySerializer<TSerializer>(serializer)
        , serializer(serializer)
    {
    }

private:
    virtual offset_t position() const
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
    position_impl(TSerializer* dummy) const
    {
        return serializer.position();
    }

    template<typename TSerializer>
    typename std::enable_if<!has_member_position<TSerializer>::value && ForceCreate, offset_t>::type
    position_impl(TSerializer* dummy) const
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
