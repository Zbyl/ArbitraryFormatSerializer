/////////////////////////////////////////////////////////////////////////////
/// ArbitraryFormatSerializer
///    Library for serializing data in arbitrary formats.
///
/// metaprogramming.h
///
/// This file contains template metaprogramming functions.
///
/// Distributed under Apache License, Version 2.0 (http://www.apache.org/licenses/LICENSE-2.0)
/// (c) 2014 Zbigniew Skowron, zbychs@gmail.com
///
/////////////////////////////////////////////////////////////////////////////

#ifndef ArbitraryFormatSerializer_metaprogramming_H
#define ArbitraryFormatSerializer_metaprogramming_H

#include <type_traits>
#include <utility>

namespace arbitrary_format
{

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// TypeDisplayer is a helper template used to generate compiler errors that show given type

template<typename T>
struct TypeDisplayer;

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/// This namespace contains meta functions that operate on type sequences, like type_list, std::tuple, std::pair etc.
namespace tsec
{

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// type_list is a helper template used to store a list of types

template<typename... Ts>
struct type_list
{};

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// to_types meta function converts a sequence of numbers into a sequence of std::integral_constants
// Example:
//   to_types< std::integer_sequence<int, 0, 1, 2> > == type_list< std::integral_constant<int, 0>, std::integral_constant<int, 1>, std::integral_constant<int, 2> >

template<typename seq>
struct to_types_impl;

template<typename T, template<typename U, U...> class seq, T... numbers>
struct to_types_impl<seq<T, numbers...>>
{
    using type = type_list< std::integral_constant<T, numbers>... >;
};

template<typename seq>
using to_types = typename to_types_impl<seq>::type;


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// get_element meta function retrieves from type list a type under given index
// Example:
//   get_element<1, type_list<int, float, short>  > == float

template<int I, typename TypeList>
struct get_element_impl;

template<template<typename... U> class TypeList, typename T, typename... Ts>
struct get_element_impl< 0, TypeList<T, Ts...> >
{
    using type = T;
};

template<int I, template<typename... U> class TypeList, typename T, typename... Ts>
struct get_element_impl< I, TypeList<T, Ts...> >
{
    using type = typename get_element_impl< I - 1, TypeList<Ts...> >::type;
};

template<int I, typename TypeList>
using get_element = typename get_element_impl<I, TypeList>::type;

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
} // namespace tsec

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/// This namespace contains meta functions that operate on integer sequences, like std::integer_sequence
namespace isec
{

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// to_values meta function converts a sequence of type with "value" field (like std::integral_constant) into a sequence of values
// Example:
//   to_values< std::integral_constant<int, 0>, std::integral_constant<int, 1>, std::integral_constant<int, 2> > == std::integer_sequence<int, 0, 1, 2>

template<typename T, typename TypeList>
struct to_values_impl;

template<typename T, template<typename... U> class TypeList, typename... Types>
struct to_values_impl< T, TypeList<Types...> >
{
    using type = std::integer_sequence<T, Types::value... >;
};

template<typename TypeList>
using to_values = typename to_values_impl<std::remove_cv_t<decltype(tsec::get_element<0, TypeList>::value)>, TypeList>::type;

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// element_type meta function returns element type of sequence of numbers
// Example:
//   element_type< std::integer_sequence<int, 0, 1, 2, 3> > == int

template<typename seq>
struct element_type_impl;

template<typename T, template<typename U, U...> class seq, T... numbers>
struct element_type_impl<seq<T, numbers...>>
{
    using type = T;
};

template<typename seq>
using element_type = typename element_type_impl<seq>::type;

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// rename meta function renames a sequence of values into another
// Example:
//   rename< std::integer_sequence<int, 0, 1, 2, 3>, myseq_t > == myseq_t<int, 0, 1, 2, 3>

template<class From, template<typename T, T...> class To>
struct rename_impl;

template<typename T, T... numbers, template<typename U, U...> class From, template<typename V, V...> class To>
struct rename_impl<From<T, numbers...>, To>
{
    using type = To<T, numbers...>;
};

template<typename From, template<typename T, T...> class To>
using rename = typename rename_impl<From, To>::type;

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// sum_args meta function computes a sum of it's arguments
// Example:
//   sum_args<int, 0, 1, 2, 3>::value == 6

template<typename T, T... numbers>
struct sum_args;

template<typename T>
struct sum_args<T> : std::integral_constant<int, 0>
{
};

template<typename T, T number, T... numbers>
struct sum_args<T, number, numbers...> : std::integral_constant<int, number + sum_args<T, numbers...>::value>
{
};

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// sum is a function from sequence of values to a sum of the values
// Example:
//   sum< std::integer_sequence<int, 0, 1, 2, 3> >::value == 6

template<typename seq>
using sum = rename< seq, sum_args >;

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// cast meta function casts sequences of numbers to a sequence of numbers of different type
// Example:
//   cast<short, std::integer_sequence<int, 0, 1> > == std::integer_sequence<short, 0, 1>

template<typename To, typename From, typename seq>
struct cast_impl;

template<typename To, typename From, template<typename U, U...> class seq, From... numbers>
struct cast_impl<To, From, seq<From, numbers...> >
{
    using type = seq<To, (static_cast<To>(numbers))...>;
};

template<typename To, typename seq>
using cast = typename cast_impl<To, element_type<seq>, seq>::type;

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// append meta function concatenates two sequences of numbers
// Example:
//   append< std::integer_sequence<int, 0, 1>, std::integer_sequence<int, 2, 3> > == std::integer_sequence<int, 0, 1, 2, 3>

template<typename T, typename seq0, typename seq1>
struct append_impl;

template<typename T, template<typename U, U...> class seq0, template<typename V, V...> class seq1, T... numbers0, T... numbers1>
struct append_impl<T, seq0<T, numbers0...>, seq1<T, numbers1...> >
{
    using type = seq0<T, numbers0..., numbers1...>;
};

template<typename seq0, typename seq1>
using append = typename append_impl<element_type<seq0>, seq0, seq1>::type;

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// reverse meta function reverses a sequence of numbers
// Example:
//   reverse< std::integer_sequence<int, 0, 1, 2, 3> > == std::integer_sequence<int, 3, 2, 1, 0>

template<typename T, typename seq_to_reverse, T... numbers_to_append>
struct reverse_impl;

template<typename T, template<typename U, U...> class seq_to_reverse, T... numbers_to_append>
struct reverse_impl<T, seq_to_reverse<T>, numbers_to_append...>
{
    using type = seq_to_reverse<T, numbers_to_append...>;
};

template<typename T, template<typename U, U...> class seq_to_reverse, T number, T... numbers, T... numbers_to_append>
struct reverse_impl<T, seq_to_reverse<T, number, numbers...>, numbers_to_append...>
{
    using type = typename reverse_impl<T, seq_to_reverse<T, numbers...>, number, numbers_to_append...>::type;
};

template<typename seq>
using reverse = typename reverse_impl<element_type<seq>, seq>::type;

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// push_front meta function pushes a number to the front of the sequence of numbers
// Example:
//   push_front<int, std::integer_sequence<int, 0, 1, 2, 3>, -1 > == std::integer_sequence<int, -1, 0, 1, 2, 3>

template<typename T, typename seq, T number>
struct push_front_impl;

template<typename T, template<typename T, T...> class seq, T number, T... numbers>
struct push_front_impl<T, seq<T, numbers...>, number>
{
    using type = seq<T, number, numbers...>;
};

template<typename T, typename seq, T number>
using push_front = typename push_front_impl<T, seq, number>::type;

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// pop_front meta function pops a number from the front of the sequence of numbers
// Example:
//   pop_front< std::integer_sequence<int, 0, 1, 2, 3> > == std::integer_sequence<int, 1, 2, 3>

template<typename seq>
struct pop_front_impl;

template<typename T, template<typename U, U...> class seq, T number, T... numbers>
struct pop_front_impl<seq<T, number, numbers...>>
{
    using type = seq<T, numbers...>;
};

template<typename seq>
using pop_front = typename pop_front_impl<seq>::type;

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// push_back meta function pushes a number to the back of the sequence of numbers
// Example:
//   push_back<int, std::integer_sequence<int, 0, 1, 2, 3>, 4 > == std::integer_sequence<int, 0, 1, 2, 3, 4>

template<typename T, typename seq, T number>
using push_back = reverse< push_front<T, reverse<seq>, number> >;

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// pop_back meta function pops a number from the back of the sequence of numbers
// Example:
//   pop_back< std::integer_sequence<int, 0, 1, 2, 3> > == std::integer_sequence<int, 0, 1, 2>

template<typename seq>
using pop_back = reverse< pop_front< reverse<seq> > >;

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// add meta function adds a number to all elements of a sequence
// Example:
//   add<int, std::integer_sequence<int, 0, 1, 2, 3>, 1 > == std::integer_sequence<int, 1, 2, 3, 4>

template<typename T, typename seq, T number>
struct add_impl;

template<typename T, template<typename T, T...> class seq, T number, T... numbers>
struct add_impl<T, seq<T, numbers...>, number>
{
    using type = seq<T, (number + numbers)...>;
};

template<typename T, typename seq, T number>
using add = typename add_impl<T, seq, number>::type;

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// partial_sum meta function computes a sequence of partial sums
// Example:
//   partial_sum< std::integer_sequence<int, 0, 1, 2, 3> > == std::integer_sequence<int, 0, 1, 3, 6>

template<typename T, typename seq, T... numbers>
struct partial_sum_impl;

template<typename T, template<typename U, U...> class seq>
struct partial_sum_impl<T, seq<T> >
{
    using type = seq<T>;
};

template<typename T, template<typename U, U...> class seq, T number, T... numbers>
struct partial_sum_impl<T, seq<T, number, numbers...>>
{
    using type = push_front<T, add<T, typename partial_sum_impl<T, seq<T, numbers...> >::type, number>, number>;
};

template<typename seq>
using partial_sum = typename partial_sum_impl<element_type<seq>, seq>::type;

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// partial_sum_args meta function computes a sequence of partial sums of it's arguments
// Example:
//   partial_sum_args<int, 0, 1, 2, 3> == std::integer_sequence<int, 0, 1, 3, 6>

template<typename T, T... numbers>
using partial_sum_args = partial_sum< std::integer_sequence<T, numbers...> >;

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// is_same meta function compares two integer sequences
// Example:
//   is_same< dummy_integer_sequence<int, 0, 1, 2, 3>, std::integer_sequence<int, 0, 1, 2, 3>> == true
//   is_same< dummy_integer_sequence<int, 0, 1, 2, 3>, std::integer_sequence<short, 0, 1, 2, 3>> == false
//   is_same< dummy_integer_sequence<int, 0, 3>, std::integer_sequence<int, 0, 1, 2, 3>> == false

template<typename seq0, typename seq1>
struct is_same_seq : std::false_type
{
};

template<typename T, template<typename U, U...> class seq0, template<typename V, V...> class seq1>
struct is_same_seq< seq0<T>, seq1<T> > : std::true_type
{
};

template<typename T, template<typename U, U...> class seq0, template<typename V, V...> class seq1, T number0, T... numbers0, T number1, T... numbers1>
struct is_same_seq< seq0<T, number0, numbers0...>, seq1<T, number1, numbers1...> >
    : std::integral_constant<bool, (number0 == number1) && is_same_seq< seq0<T, numbers0...>, seq1<T, numbers1...> >::value>
{
};

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

} // isec
} // namespace arbitrary_format

#endif // ArbitraryFormatSerializer_metaprogramming_H
