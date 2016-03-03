/////////////////////////////////////////////////////////////////////////////
/// ArbitraryFormatSerializer
///    Library for serializing data in arbitrary formats.
///
/// object_formatter.h
///
/// This file contains object_formatter that formats an object using it's serialize() method.
///
/// Distributed under Apache License, Version 2.0 (http://www.apache.org/licenses/LICENSE-2.0)
/// (c) 2014 Zbigniew Skowron, zbychs@gmail.com
///
/////////////////////////////////////////////////////////////////////////////

#ifndef ArbitraryFormatSerializer_object_formatter_H
#define ArbitraryFormatSerializer_object_formatter_H

#include "implement_save_load_serialize.h"

namespace arbitrary_format
{

class object_formatter : public implement_save_load<object_formatter>
{
public:
    template<typename T, typename TSerializer>
    void save_or_load(TSerializer& serializer, T& object)
    {
        object->serialize(serializer);
    }
};

} // namespace arbitrary_format

#endif // ArbitraryFormatSerializer_object_formatter_H
