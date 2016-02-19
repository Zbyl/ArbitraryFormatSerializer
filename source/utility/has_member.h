/////////////////////////////////////////////////////////////////////////////
/// ArbitraryFormatSerializer
///    Library for serializing data in arbitrary formats.
///
/// has_member.h
///
/// This file contains simple member detector.
/// (Boost Type Traits Introspection Library's has_member_data.hpp requires to specify type of the member, which is inconvenient.)
/// Or using expression SFINAE when Visual Studio will support it (see http://stackoverflow.com/a/13787502).
///
/// Distributed under Apache License, Version 2.0 (http://www.apache.org/licenses/LICENSE-2.0)
/// (c) 2014 Zbigniew Skowron, zbychs@gmail.com
///
/////////////////////////////////////////////////////////////////////////////

#ifndef ArbitraryFormatSerializer_has_member_H
#define ArbitraryFormatSerializer_has_member_H

#include <boost/type_traits/integral_constant.hpp>

namespace arbitrary_format
{

#define GENERATE_HAS_MEMBER(member) \
    template<typename T> \
    class has_member_impl_ ## member \
    { \
    private: \
        struct WithMember { int member; }; \
        struct MemberChecker : public T, public WithMember {}; \
     \
        typedef char Yes[1]; \
        typedef char No[2]; \
     \
        template<typename U, U> class fail_when_member_ambiguity; \
     \
        template<typename U> \
        static No& check(fail_when_member_ambiguity<int WithMember::*, &U::member> *); \
        template<typename U> \
        static Yes& check(...); \
     \
    public: \
        enum { value = (sizeof(check<MemberChecker>(0)) == sizeof(Yes)) }; \
    }; \
     \
    template<typename T> \
    class has_member_ ## member : public boost::integral_constant< bool, has_member_impl_ ## member <T>::value > {}

} // namespace arbitrary_format

#endif // ArbitraryFormatSerializer_has_member_H
