Arbitrary Format Serializer Todo
================================

Enchancements
=============

This is an assorted list of possible improvements:

1. Lambdas for creating stateful formatters. To simplify code in case like TeansportStream formatter in SWU.
2. Maybe serializer wrappers for storing state? Alternative way of solving the same problem.
3. Serializers for raw memory references. Zero copy serializers.
4. Nicer way of returning formatters from functions - for Visual 2013 (renaming the "_impl" formatters to something nicer, like "_stateful"?):
    ```cpp
    auto getPidPairFormat(uint16_t pid) const
            -> element_formatter_impl< pair_formatter<
    attribute_formatter_impl<assign_text_content<>>, pid_formatter > >
        {
            auto pair_format = create_element_formatter("pid",
    create_pair_formatter(create_attribute_formatter("pid"),
    pid_formatter(m_tsDescription, pid)));
            return pair_format;
        }
    ```
5. declare_compile_time_string / define_compile_time_string for VisualStudio 2013
6. enum_stringizer?
7. convert_value - on save - mutate the value, on load - un mutate it
8. collect_elements - makes sense for xml to collect actual serializers (xml nodes); generic collect_serializers?
9. take_all_formatter - read up till the end
10. await based corutines for collecting stuff?
11. Splitting is_verbatim_formatter into save and load aspects could be useful.
12. Write a sub-library for verbose, detailed exceptions.
13. Add `mutable` to all formatter fields of formatters?
