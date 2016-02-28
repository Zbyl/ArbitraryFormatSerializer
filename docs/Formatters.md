Formatters quick reference
==========================

This is a quick reference of all currently available formatters.

**Table of Contents**

- [Generic formatters](#generic-formatters)
- [Binary formatters](#binary-formatters)
- [Xml formatters](#xml-formatters)
- [String formatters (stringizers)](#string-formatters-stringizers)
- [external_value](#external_value)

### Generic formatters
Formatters that can be used with any serializer (xml, binary, etc.).

Name | Types supported | Description
:---------|:---------|:-------------
`any_formatter` | `boost::any` | **[not ready yet]**
`array_formatter` | `T[]`, `T*`, `std::array<T, Size>` | Formats fixed size arrays as a sequence of values.
`collection_formatter` | `std::list`, `std::set`, `std::map`, `std::vector`... | Formats collections as size followed by values. Takes arbitrary `size_formatter` and `value_formatter` as parameters.<br/>Use `vector_formatter` for `std::vector`. See `map_formatter` for a more convenient serializer for `std::map`.
`const_formatter` | *any type* | A formatter wrapper, that allows for saving a constant and verifying it on load, i.e.:<br/>`serialize< const_formatter<little_endian<1>> >(serializer, 5);`
`external_value` | *any type* | A formatter wrapper, that allows for using a value stored externally. It verifies that external value has proper value on save and loads external value on load. See example in [external_value](#external_value).
`generic_formatter` | *any type* |  Formats a type using a `save_or_load()` function found using Argument Dependent Lookup.
`map_formatter` | `std::map`, `std::multimap` | Formats maps as size followed by key and value for each element. Takes arbitrary `size_formatter`, `key_formatter` and `value_formatter` as parameters.
`object_formatter` | *any type* | Formats an object using it's `serialize()` method. This is more of an example than an actually useful formatter.
`optional_formatter` | `boost::optional` | Formats value as an is-not-empty flag followed by value.
`pair_formatter` | `std::pair` | Formats a pair as a first value followed by second value.
`shared_ptr_copy_formatter` | `std::shared_ptr` | This formatter stores a `shared_ptr` as a is-null flag followed by a value. It's has *copy* in it's name, since every instance of a `shared_ptr` will be serialized as an independent copy (so the shared ownership will NOT be preserved).
`tuple_formatter` | `std::tuple`, `std::pair` | Formats tuples as a sequence of values.<br/>Use `pair_formatter` for pairs to make debugging more straightforward.
`type_formatter` | *any type* | **[not ready yet]** A formatter wrapper that erases the type of the underlying formatter. Parametrized with the type of serializer and formatted value.
`vector_formatter` | `std::vector` | Formats vectors as size followed by elements.<br/>It is more optimized for vectors than `collection_formatter`.

### Binary formatters
Formatters that can be used with binary serializers.

Name | Types supported | Description
:---------|:---------|:-------------
`bit_formatter` | sequences of integers, `std::tuple` | Packs individual values or tuples of values in bitfields.
`endian_formatter` | `int`, `short`, `char`, `uint64_t`, integral types, enums, pods... | Formats given value on specified number of bytes, with specified endianness.
`little_endian` | `int`, `short`, `char`, `uint64_t`, integral types, enums, pods... | Formats given value on specified number of bytes, using little endian byte order.
`big_endian` | `int`, `short`, `char`, `uint64_t`, integral types, enums, pods... | Formats given value on specified number of bytes, using big endian byte order.
`inefficient_size_prefix_formatter` | *any type* | Formats value as it's serialized size followed by it's value. It's inefficient, because it serializes the value twice. (It can lead to exponential time complexity when used for trees.)<br/>Use `size_prefix_formatter` instead.
`size_prefix_formatter` |*any type* | Formats value as it's serialized size followed by it's value. Requires serializer that supports `position()` and `seek()` methods.
`string_formatter` | `std::string`, `std::wstring`, `std::u16string`, `std::u32string`, `std::basic_string`| Formats strings as length followed by characters.
`verbatim_formatter` | *any plain-old-data type* | Formats value as a raw dump of bytes from memory.

### Xml formatters
Formatters that can be used with xml serializers.
See [Xml serialization](XmlSerialization.md) for more details.

Name | Types supported | Description
:---------|:---------|:-------------
`assign_name` | *any type* | Formats value as a name of parent element or attribute.
`assign_text_content` | *any type* | Formats value as text content of parent element or attribute.
`attribute_counter` | `int`, `bool`, integral types... | On save does nothing. On load counts the number of attributes with given name (or all).
`attribute_formatter` | *any type* | Formats value as an xml attribute.
`content_exists` | `bool` | On save does nothing. On load checks if xml element has any child elements or a text content.<br/>*Note: it does not check for existence of attributes.*
`content_not_exists` | `bool` | On save does nothing. On load checks if xml element has no child elements and no text content.<br/>*Note: it does not check for existence of attributes.*
`text_content_exists` | `bool` | On save does nothing. On load checks if xml element has any text content.
`text_content_not_exists` | `bool` | On save does nothing. On load checks if xml element has no text content.
`attribute_exists` | `bool` | On save does nothing. On load checks if xml element has attributes with given name (or any attributes).
`attribute_not_exists` | `bool` | On save does nothing. On load checks if xml element has no attributes with given name (or no attributes at all).
`element_exists` | `bool` | On save does nothing. On load checks if xml element has child elements with given name (or any child elements).
`element_not_exists` | `bool` | On save does nothing. On load checks if xml element has no child element with given name (or no child elements at all).
`declaration_formatter` | *any type* | Formats an *encoding* as an xml declaration.
`document_formatter` | *any type* | Formats value as an xml document. Xml declaration is added, with encoding taken from the serializer.
`element_counter` | `int`, `bool`, integral types... | On save does nothing. On load counts the number of elements with given name (or all).
`element_formatter` | *any type* | Formats value as an xml element.

### String formatters (stringizers)
Formatters that can be used to save and load data from a `std::string`.

Name | Types supported | Description
:---------|:---------|:-------------
`lexical_stringizer` | *any type* | Formatter that uses `boost::lexical_cast` to convert data to and from an `std::string`.

### external_value
Example:
`vector_formatter` stores vector size followed by vector elements.
But what can we do, if we have a format, where between size and elements there is another value stored?
We can use `external_value` formatter to do this. This requires us to instantiate formatter objects, since `external_value` is a stateful formatter (it stores a reference to the external value). See [Stateful formatters](../README.md#stateful-formatters) for an overview of stateful formatters.
```cpp
auto vector_size = vec.size();
serialize< little_endian<1> >(serializer, vector_size);
serialize< little_endian<2> >(serializer, someOtherValue);

auto vec_format = create_vector_formatter( create_external_value(vector_size), little_endian<1>{} );
serialize(serializer, vec, vec_format);
```
