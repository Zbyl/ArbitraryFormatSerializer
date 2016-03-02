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

#if defined(_MSC_VER) && (_MSC_VER < 1900)

namespace arbitrary_format
{

template <const char* Str>
struct compile_time_string
{
};

template <typename str>
struct compile_time_string_print;

template <const char* Str>
struct compile_time_string_print< compile_time_string<Str> >
{
    static std::string str()
    {
        return Str;
    }
};

/// @note This version must be used at global scope.
#define declare_compile_time_string(name, str) \
    extern const char declare_compile_time_string_ ## name [] = str; \
    using name = compile_time_string<declare_compile_time_string_ ## name>;

} // namespace arbitrary_format

#elif defined(_MSC_VER)

namespace arbitrary_format
{

template <const char... Cs>
struct compile_time_string
{
    //using empty = std::integral_constant<bool, sizeof...(Cs) == 0>;
};

template <typename str>
struct compile_time_string_print;

template <template<char...> class CTString, char... Cs>
struct compile_time_string_print< CTString<Cs...> >
{
    static std::string str()
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
        static auto make_string() \
        { \
            struct c_string \
            { \
                const char * chars = str; \
            }; \
            return make_compile_time_string1<sizeof(str) - 1, c_string>::type{}; \
        } \
    }; \
    using name = decltype( declare_compile_time_string_ ## name ::make_string() );

} // namespace arbitrary_format

#else

#include "../third_party/typestring/typestring.hh"

namespace arbitrary_format
{

template <typename str>
struct compile_time_string_print;

template <template<char...> class CTString, char... Cs>
struct compile_time_string_print< CTString<Cs...> >
{
    static std::string str()
    {
        return {Cs...};
    }
};

#define declare_compile_time_string(name, str) \
    using name = typestring_is(str)

#define compile_time_string(str) typestring_is(str)

} // namespace arbitrary_format

#endif

#endif // ArbitraryFormatSerializer_compile_time_string_H
