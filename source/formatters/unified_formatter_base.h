/////////////////////////////////////////////////////////////////////////////
/// ArbitraryFormatSerializer
///    Library for serializing data in arbitrary formats.
///
/// unified_formatter_base.h
///
/// This file contains unified_formatter_base mixin class that provides save() and load() by the means of the serialize() method.
///
/// Distributed under Apache License, Version 2.0 (http://www.apache.org/licenses/LICENSE-2.0)
/// (c) 2014 Zbigniew Skowron, zbychs@gmail.com
///
/////////////////////////////////////////////////////////////////////////////

#ifndef ArbitraryFormatSerializer_unified_formatter_base_H
#define ArbitraryFormatSerializer_unified_formatter_base_H

#include <boost/type_traits/remove_const.hpp>

namespace arbitrary_format
{

template<typename Derived>
class unified_formatter_base
{
public:
    template<typename T, typename TSerializer>
    void save(TSerializer& serializer, const T& object) const
    {
        static_cast<const Derived*>(this)->serialize(serializer, const_cast<typename boost::remove_const<T>::type&>(object));
    }

    template<typename T, typename TSerializer>
    void load(TSerializer& serializer, T& object) const
    {
        static_cast<const Derived*>(this)->serialize(serializer, object);
    }
};

} // namespace arbitrary_format

#endif // ArbitraryFormatSerializer_unified_formatter_base_H
