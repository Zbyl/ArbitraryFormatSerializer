// MetaProgrammingTests.cpp - tests for meta programming helpers
//

#include "utility/metaprogramming.h"

#include <utility>
#include <type_traits>

#include "gtest/gtest.h"

namespace {

using namespace arbitrary_format;

template<typename T, T... numbers>
struct dummy_integer_sequence;

TEST(MetaProgrammingTests, StaticAsserts)
{
    using namespace isec;

    using seq_0_1 = std::integer_sequence<int, 0, 1>;
    using seq_2_3 = std::integer_sequence<int, 2, 3>;
    using seq_0_3 = std::integer_sequence<int, 0, 1, 2, 3>;

    static_assert(sum_args<int, 0, 1, 2, 3>::value == 6, "sum_args return value invalid");
    static_assert(sum<seq_0_3>::value == 6, "sum return value invalid");
    static_assert(std::is_same< element_type<seq_0_3>, int >::value, "element_type return value invalid");
    static_assert(std::is_same< reverse<seq_0_3>, std::integer_sequence<int, 3, 2, 1, 0> >::value, "reverse return value invalid");
    static_assert(std::is_same< push_front<int, seq_0_3, 4>, std::integer_sequence<int, 4, 0, 1, 2, 3> >::value, "push_front return value invalid");
    static_assert(std::is_same< push_back<int, seq_0_3, 4>, std::integer_sequence<int, 0, 1, 2, 3, 4> >::value, "push_back return value invalid");
    static_assert(std::is_same< pop_front<seq_0_3>, std::integer_sequence<int, 1, 2, 3> >::value, "pop_front return value invalid");
    static_assert(std::is_same< pop_back<seq_0_3>, std::integer_sequence<int, 0, 1, 2> >::value, "pop_back return value invalid");
    static_assert(std::is_same< add<int, seq_0_3, 1>, std::integer_sequence<int, 1, 2, 3, 4> >::value, "add return value invalid");
    static_assert(std::is_same< partial_sum<seq_0_3>, std::integer_sequence<int, 0, 1, 3, 6> >::value, "partial_sum return value invalid");
    static_assert(std::is_same< partial_sum_args<int, 0, 1, 2, 3>, std::integer_sequence<int, 0, 1, 3, 6> >::value, "partial_sum_args return value invalid");
    static_assert(is_same_seq< dummy_integer_sequence<int, 0, 1, 2, 3>, std::integer_sequence<int, 0, 1, 2, 3> >::value, "is_same_seq return value invalid");
    static_assert(!is_same_seq< dummy_integer_sequence<int, 0, 1, 2, 3>, std::integer_sequence<short, 0, 1, 2, 3> >::value, "is_same_seq return value invalid");
    static_assert(!is_same_seq< dummy_integer_sequence<int, 0, 1, 2, 3>, std::integer_sequence<int, 0, 1, 2> >::value, "is_same_seq return value invalid");
    static_assert(!is_same_seq< dummy_integer_sequence<int, 0, 1, 2, 3>, std::integer_sequence<int, 0, 1, 2, 4> >::value, "is_same_seq return value invalid");
    static_assert(std::is_same< append<seq_0_1, seq_2_3>, seq_0_3 >::value, "append return value invalid");
    static_assert(std::is_same< cast<short, seq_0_3>, std::integer_sequence<short, 0, 1, 2, 3> >::value, "cast return value invalid");
    static_assert(std::is_same< cast<unsigned, std::integer_sequence<int, -1>>, std::integer_sequence<unsigned, static_cast<unsigned>(-1)> >::value, "cast return value invalid");
    static_assert(std::is_same< isec::rename<dummy_integer_sequence<int, 0, 1, 2, 3>, std::integer_sequence>, seq_0_3 >::value, "rename return value invalid");

    using namespace tsec;
    static_assert(std::is_same< to_types<seq_0_1>, type_list< std::integral_constant<int, 0>, std::integral_constant<int, 1> > >::value, "to_types return value invalid");
    static_assert(std::is_same< get_element<1, type_list<int, float, short>>, float >::value, "get_element return value invalid");
    static_assert(std::is_same< std::integer_sequence<int,0,1>, seq_0_1 >::value, " return value invalid");
    static_assert(std::is_same< to_values< type_list< std::integral_constant<int, 0>, std::integral_constant<int, 1> > >, std::integer_sequence<int,0,1> >::value, "to_values return value invalid");
} 

}  // namespace
