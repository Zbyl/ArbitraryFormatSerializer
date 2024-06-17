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

#include <arbitrary_format/implement_save_load_serialize.h>
#include <arbitrary_format/serialization_exceptions.h>
#include <arbitrary_format/utility/has_member.h>

#include <utility>

#include <cstdint>

namespace arbitrary_format
{
namespace binary
{

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
        (void)data;
        (void)size;

        if (!saving())
        {
            BOOST_THROW_EXCEPTION(serialization_exception() << errinfo_description("Can't save to a loading serializer."));
        }

        BOOST_THROW_EXCEPTION(serialization_exception() << errinfo_description("Function should be implemented by subclass."));
    }

    void loadDataImpl(uint8_t* data, size_t size) override
    {
        (void)data;
        (void)size;

        if (!loading())
        {
            BOOST_THROW_EXCEPTION(serialization_exception() << errinfo_description("Can't load from a saving serializer."));
        }

        BOOST_THROW_EXCEPTION(serialization_exception() << errinfo_description("Function should be implemented by subclass."));
    }
};    

template<typename TSerializer>
struct CantMakePolymorphicSerializer
{
    CantMakePolymorphicSerializer()
    {
        static_assert(false && (sizeof(TSerializer) >= 0), "Can't make a polymorphic serializer from a serializer that isn't a loading nor a saving serializer.");
    }
};

template<typename TSerializer>
using polymorphic_serializer_t = typename std::conditional<is_saving_serializer<TSerializer>::value,
                                                        typename std::conditional<is_loading_serializer<TSerializer>::value, ISlowSerializer, ISaveSerializer>::type,
                                                        typename std::conditional<is_loading_serializer<TSerializer>::value, ILoadSerializer, CantMakePolymorphicSerializer<TSerializer>>::type >::type;

template<typename PolymorphicSerializer>
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

    template<typename T, typename std::enable_if< is_saving_serializer<T>::value && !is_loading_serializer<T>::value >::type* sfinae = nullptr>
    void doSaveData(const uint8_t* data, size_t size)
    {
        return serializer.saveData(data, size);
    }

    template<typename T, typename std::enable_if< is_saving_serializer<T>::value && !is_loading_serializer<T>::value >::type* sfinae = nullptr>
    void doLoadData(uint8_t* data, size_t size)
    {
        BOOST_THROW_EXCEPTION(serialization_exception() << errinfo_description("Can't load from a saving serializer."));
    }

    template<typename T, typename std::enable_if< is_loading_serializer<T>::value && !is_saving_serializer<T>::value >::type* sfinae = nullptr>
    void doSaveData(const uint8_t* data, size_t size)
    {
        BOOST_THROW_EXCEPTION(serialization_exception() << errinfo_description("Can't save to a loading serializer."));
    }

    template<typename T, typename std::enable_if< is_loading_serializer<T>::value && !is_saving_serializer<T>::value >::type* sfinae = nullptr>
    void doLoadData(uint8_t* data, size_t size)
    {
        return serializer.loadData(data, size);
    }

    template<typename T, typename std::enable_if< is_saving_serializer<T>::value && is_loading_serializer<T>::value >::type* sfinae = nullptr>
    void doSaveData(const uint8_t* data, size_t size)
    {
        if (!saving())
        {
            BOOST_THROW_EXCEPTION(serialization_exception() << errinfo_description("Can't save to a loading serializer."));
        }

        return serializer.saveData(data, size);
    }

    template<typename T, typename std::enable_if< is_saving_serializer<T>::value && is_loading_serializer<T>::value >::type* sfinae = nullptr>
    void doLoadData(uint8_t* data, size_t size)
    {
        if (!this->loading())
        {
            BOOST_THROW_EXCEPTION(serialization_exception() << errinfo_description("Can't load from a saving serializer."));
        }

        return serializer.loadData(data, size);
    }

private:
    TSerializer& serializer;
};

AFS_GENERATE_HAS_MEMBER(position);
AFS_GENERATE_HAS_MEMBER(seek);

/// @brief AnySerializer is a decorator, that converts a non-polymorphic serializer into a polymorphic serializer.
/// @param ForceCreate  If true, then type will be created, but missing methods in TSerializer will result in generation of methods that throw not_implemented exception.
///                     If false, then type will not be created if there are any missing methods in TSerializer.
template<typename TSerializer, bool ForceCreate = false, typename PolymorphicSerializer = polymorphic_serializer_t<TSerializer>>
class AnySeekableSerializer : public virtual AnySerializer<TSerializer, PolymorphicSerializer>, public virtual ISeekableSerializer<PolymorphicSerializer>
{
public:
    using offset_t = typename ISeekableSerializer<PolymorphicSerializer>::offset_t;

    explicit AnySeekableSerializer(TSerializer& serializer)
        : AnySerializer<TSerializer, PolymorphicSerializer>(serializer)
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
    template<typename ASerializer>
    typename std::enable_if<has_member_position<ASerializer>::value, offset_t>::type
    position_impl(ASerializer* dummy) const
    {
        return serializer.position();
    }

    template<typename ASerializer>
    typename std::enable_if<!has_member_position<ASerializer>::value && ForceCreate, offset_t>::type
    position_impl(ASerializer* dummy) const
    {
        BOOST_THROW_EXCEPTION(not_implemented());
    }

    template<typename ASerializer>
    typename std::enable_if<has_member_seek<ASerializer>::value, void>::type
    seek_impl(offset_t position, ASerializer* dummy)
    {
        return serializer.seek(position);
    }

    template<typename ASerializer>
    typename std::enable_if<!has_member_seek<ASerializer>::value && ForceCreate, void>::type
    seek_impl(offset_t position, ASerializer* dummy)
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
AnySeekableSerializer<T> make_seekable_serializer_force(T& serializer)
{
    return AnySeekableSerializer<T>(serializer);
}

template<typename PolymorphicSerializer, typename T>
AnySeekableSerializer<T, false, PolymorphicSerializer> make_serializer_ex(T& serializer)
{
    return AnySeekableSerializer<T, false, PolymorphicSerializer>(serializer);
}

template<typename PolymorphicSerializer, typename T>
AnySeekableSerializer<T, false, PolymorphicSerializer> make_seekable_serializer_ex(T& serializer)
{
    return AnySeekableSerializer<T, false, PolymorphicSerializer>(serializer);
}

template<typename PolymorphicSerializer, typename T>
AnySeekableSerializer<T, true, PolymorphicSerializer> make_seekable_serializer_force_ex(T& serializer)
{
    return AnySeekableSerializer<T, true, PolymorphicSerializer>(serializer);
}

} // namespace binary
} // namespace arbitrary_format

#endif // ArbitraryFormatSerializer_ISerializer_H
