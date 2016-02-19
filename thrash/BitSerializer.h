/////////////////////////////////////////////////////////////////////////////
/// ArbitraryFormatSerializer
///    Library for serializing data in arbitrary formats.
///
/// BitSerializer.h
///
/// This file contains BitSerializer that is a serializer for individual bits.
///
/// Distributed under Apache License, Version 2.0 (http://www.apache.org/licenses/LICENSE-2.0)
/// (c) 2014 Zbigniew Skowron, zbychs@gmail.com
///
/////////////////////////////////////////////////////////////////////////////

#ifndef ArbitraryFormatSerializer_BitSerializer_H
#define ArbitraryFormatSerializer_BitSerializer_H

#include "IZeroCopySerializer.h"
#include "serialization_exceptions.h"

namespace arbitrary_format
{
namespace binary
{

template<typename ZeroCopySerializer>
class BitLoader
{
    ZeroCopySerializer& serializer;
    const boost::uint8_t* chunk;
    int bitsLeft;               ///< Number of bits left in chunk
public:
    BitLoader(ZeroCopySerializer& serializer)
        : serializer(serializer)
        , chunk(nullptr)
        , bitsLeft(0)
    {
    }

    /// @brief Loads given number bits into least significant bits of data. Zeros all other bits of result.
    boost::uint64_t loadBits(int numberOfBits)
    {
        if (numberOfBits < 0)
            BOOST_THROW_EXCEPTION(serialization_exception());
        if (numberOfBits > 64)
            BOOST_THROW_EXCEPTION(serialization_exception());

        boost::uint64_t bitsLoaded = 0;
        int bitsToLoad = numberOfBits;
        while (bitsToLoad > 0)
        {
            int bitsToLoadNow = bitsLeft;
            if (bitsToLoadNow > bitsToLoad)
            {
                bitsToLoadNow = bitsToLoad;
            }
            boost::uint64_t newBits = loadBitsRaw(bitsToLoadNow);
            bitsLoaded += newBits << (numberOfBits - bitsToLoad);
            bitsToLoad -= bitsToLoadNow;
        }

        return bitsLoaded;
    }

private:
    boost::uint64_t loadBitsRaw(int numberOfBits)
    {
        if (bitsLeft <= 0)
            prefetchBits(numberOfBits);

        //assert(numberOfBits <= bitsLeft);
        int bitsLeftInByte = ((bitsLeft + 7) & 7) + 1;
        boost::uint64_t result = *chunk >> (8 - bitsLeftInByte);
        if (bitsLeftInByte > numberOfBits)
        {
            bitsLeft -= numberOfBits;
            result &= ~(~0u << numberOfBits);
            return result;
        }

        bitsLeft -= bitsLeftInByte;
        chunk++;

        boost::uint64_t nextResult = loadBitsRaw(numberOfBits - bitsLeftInByte);
        return result | (nextResult << bitsLeftInByte);
    }

    void prefetchBits(int numberOfBits)
    {
        //assert(bitsLeft == 0);
        size_t size;
        if (!serializer.nextChunk(chunk, size))
        {
            if (numberOfBits > 0)
                BOOST_THROW_EXCEPTION(end_of_input());
        }
        bitsLeft = size * 8;
    }
};

template<typename ZeroCopySerializer>
class BitSaver
{
    ZeroCopySerializer& serializer;
    boost::uint8_t* chunk;
    int bitsLeft;               ///< Number of bits left in chunk
    boost::uint64_t currentByte;
public:
    BitSaver(ZeroCopySerializer& serializer)
        : serializer(serializer)
        , chunk(nullptr)
        , bitsLeft(0)
        , currentByte(0)
    {
    }

    /// @brief Saves given number of least significant bits from data.
    void saveBits(boost::uint64_t data, int numberOfBits)
    {
        if (numberOfBits < 0)
            BOOST_THROW_EXCEPTION(serialization_exception());
        if (numberOfBits > 64)
            BOOST_THROW_EXCEPTION(serialization_exception());

        boost::uint64_t bits = data;
        int bitsToSave = numberOfBits;
        while (bitsToSave > 0)
        {
            int bitsToSaveNow = bitsLeft;
            if (bitsToSaveNow > bitsToSave)
            {
                bitsToSaveNow = bitsToSave;
            }
            saveBitsRaw(bits, bitsToSaveNow);
            bits >>= bitsToSaveNow;
            bitsToSave -= bitsToSaveNow;
        }
    }

private:
    void saveBitsRaw(boost::uint64_t data, int numberOfBits)
    {
        if (bitsLeft <= 0)
            prefetchBits();

        //assert(numberOfBits <= bitsLeft);
        int bitsLeftInByte = ((bitsLeft + 7) & 3) + 1;
        currentByte |= data << bitsLeftInByte;
        if (bitsLeftInByte > numberOfBits)
        {
            bitsLeft -= numberOfBits;
            currentByte &= 0xFFu << numberOfBits;
            return;
        }

        *chunk = static_cast<uint8_t>(currentByte);

        bitsLeft -= bitsLeftInByte;
        chunk++;

        saveBitsRaw(data >> bitsLeftInByte, numberOfBits - bitsLeftInByte);
    }

    void prefetchBits()
    {
        //assert(bitsLeft == 0);
        size_t size;
        serializer.nextChunk(chunk, size);
        bitsLeft = size * 8;
    }
};

} // namespace binary
} // namespace arbitrary_format

#endif // ArbitraryFormatSerializer_BitSerializer_H
