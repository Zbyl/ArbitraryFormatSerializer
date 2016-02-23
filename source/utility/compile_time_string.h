/////////////////////////////////////////////////////////////////////////////
/// ArbitraryFormatSerializer
///    Library for serializing data in arbitrary formats.
///
/// compile_time_string.h
///
/// This file contains compile_time_string that formats object as element of specific name.
/// Element can be unnamed. Use then assign_name formatter to assign a name to it.
///
/// Distributed under Apache License, Version 2.0 (http://www.apache.org/licenses/LICENSE-2.0)
/// (c) 2016 Zbigniew Skowron, zbychs@gmail.com
///
/////////////////////////////////////////////////////////////////////////////

#ifndef ArbitraryFormatSerializer_compile_time_string_H
#define ArbitraryFormatSerializer_compile_time_string_H

#include <string>
#include <type_traits>

namespace arbitrary_format
{

template <const char... Cs>
struct compile_time_string
{
    using empty = std::bool_constant<sizeof...(Cs) == 0>;
};

template <typename str>
struct compile_time_string_print;

template <char... Cs>
struct compile_time_string_print< compile_time_string<Cs...> >
{
    constexpr static std::string str()
    {
        return {Cs...};
    }
};

template <typename T, typename str>
struct make_compile_time_string2;

template <size_t... Is, typename str>
struct make_compile_time_string2<std::index_sequence<Is...>, str>
{
    using type = compile_time_string<str{}.chars[Is]...>;
};

template <int size, typename str>
struct make_compile_time_string1
{
    using type = typename make_compile_time_string2<std::make_index_sequence<size>, str>::type;
};

#define declare_compile_time_string(name, str) \
    struct declare_compile_time_string_ ## name \
    { \
        struct constexpr_string_type { const char * chars = str; }; \
        auto strFunc() { return make_compile_time_string1<sizeof(str) - 1, constexpr_string_type>::type(); } \
    }; \
    using name = decltype(declare_compile_time_string_ ## name {}.strFunc())

} // namespace arbitrary_format

#endif // ArbitraryFormatSerializer_compile_time_string_H
