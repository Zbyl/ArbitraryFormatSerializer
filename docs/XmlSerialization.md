Xml serialization
=================

At the center of Xml serialization there are following classes:

- RapidXmlSaveSerializer,
- RapidXmlLoadSerializer,
- formatters from xml_formatters directory:
  - element_formatter,
  - attribute_formatter,
  - etc.

> **Note** Xml serialization is built using RapidXml library by Marcin Kalicinski.
> See: http://rapidxml.sourceforge.net/

**Table of Contents**

- [RapidXmlSaveSerializer and RapidXmlLoadSerializer](#rapidxmlsaveserializer-and-rapidxmlloadserializer)
- [Quick note about element names](#quick-note-about-element-names)
- [Example: formatting boost::optional](#example-formatting-boostoptional)
  - [Format #1: content_exists](#format-1-content_exists)
  - [Format #2: attribute_formatter](#format-2-attribute_formatter)
  - [Format #3: element_formatter](#format-3-element_formatter)
  - [Format #4: element_counter](#format-4-element_counter)
  - [Format #5: assign_name](#format-5-assign_name)
  - [Final words](#final-words)

RapidXmlSaveSerializer and RapidXmlLoadSerializer
=================================================

RapidXmlSaveSerializer and RapidXmlLoadSerializer both represent a whole xml document.
The both provide access to *document element*, which is a top level *node* of the xml.
We can use the *document element* to store (or load) nodes inside it, like this:
```cpp
RapidXmlSaveSerializer document("utf-8");
serialize< element_formatter<_name_> >(document.getDocumentElement(), value);
```
The resulting xml will look like this:
```xml
<name>value</name>
```
But this xml lacks one important part: the xml declaration. Here's how to add it:
```cpp
RapidXmlSaveSerializer document("utf-8");
serialize< declaration_formatter<> >(document.getDocumentElement(), encoding);
serialize< element_formatter<_name_> >(document.getDocumentElement(), value);
```
Now the xml will look like this:
```xml
<?xml version="1.0" encoding="utf-8"?>
<name>value</name>
```
Alternatively we can use a special `document_formatter` to achieve the same result:
```cpp
RapidXmlSaveSerializer document("utf-8");
using doc_format = document_formatter<element_formatter<_name_>>;
serialize<doc_format>(document, value);
```
`document_formatter` simply adds an xml declaration. Please not that it uses `document` as a serializer (not `document.getDocumentElement()` as before). It also extracts encoding from the `document`, so there's no need to specify it manually.

Quick note about element names
==============================
Element and attribute names are provided to formatters as compile time strings. Due to limitations of the C++ language, we need to declare such compile time strings as follows:
```cpp
declare_compile_time_string(name, "value");
```
In all examples below we use convention, that compile time string for `"something"`is declared as:
```cpp
declare_compile_time_string(_something_, "something");
```
> **Note** It is also possible to use runtime created strings as element and attribute names.
> Two ways to do this are:
>
> - using a stateful formatter, for example by calling `create_element_formatter(name)`,
> - using a `assign_name` formatter.
>
> Those options will be discussed later.

Example: formatting boost::optional
===================================
`boost::optional<T>` is a class that can hold either object T or `boost::none`. In other words it either holds a value, or is empty.
This type will allow us to showcase how to use various formatters to achieve desired results.

So how do we want `boost::optional` to be expressed in xml?

### Format #1: content_exists
Here's the obvious proposal, for empty and not-empty optionals:
```xml
<optional/>
<optional>value</optional>
```
We could write a formatter for this by hand (by creating a class with `save()` and `load()` methods).
But let's use `optional_formatter`, that is already provided by this library . It has two parameters:
```cpp
optional_formatter< flag_formatter, value_formatter >
```
It then formats `boost::optional` as an empty/not-empty flag (formatted by flag\_formatter), followed by the value (if present; formatted by value_formatter).
So a formatter for the xml shown above would be like this:
```cpp
using opt_format = optional_formatter< content_exists, assign_text_content<> >;
```
In this example we use `content_exists` formatter for flag. On load it simply checks if element has any content. On save it does nothing.
We also use `assign_text_content<>` as a formatter for value. It simply stores given value as text inside element.
So if we will now call this code:
```cpp
boost::optional<std::string> opt("Hello!");
RapidXmlSaveSerializer document("utf-8");
serialize< opt_format >(document.getDocumentElement(), opt);
```
We will get:
```xml
Hello!
```
Wait. That's not what we wanted!
But let's analyze: we asked to format value as text content of an element. And the element we gave was `document.getDocumentElement()`.  So in fact we got exactly what we asked for: document element (which does not have a name, and is not marked in any way), filled with text `Hello!`.
But we wanted the optional to be enclosed in an `<optional>` element, and that is the part that we were missing:
```cpp
boost::optional<std::string> optEmpty;
boost::optional<std::string> optHello("Hello!");

using opt_format = optional_formatter< content_exists, assign_text_content<> >;
using opt_elt_format = element_formatter< _optional_, opt_format >;

serialize< opt_elt_format >(document.getDocumentElement(), optEmpty);
serialize< opt_elt_format >(document.getDocumentElement(), optHello);
```
This will produce the desired output:
```xml
<optional/>
<optional>Hello!</optional>
```

### Format #2: attribute_formatter

Here is another way we might want to format empty and not-empty optionals:
```xml
<optional initialized="false"/>
<optional initialized="true">value</optional>
```
The formatter for this is trivial:
```cpp
using opt_format = optional_formatter< attribute_formatter<_initialized_>, assign_text_content<> >;
using opt_elt_format = element_formatter< _optional_, opt_format >;
```
Here we used `attribute_formatter` to format empty/not-empty flag as an attribute.

### Format #3: element_formatter

And yet another way of formatting optionals:
```xml
<optional>
    <flag>false</false>
</optional>
<optional>
    <flag>true</false>
    <value>value</value>
</optional>
```
The formatter for this:
```cpp
using opt_format = optional_formatter< element_formatter<_flag_>, element_formatter<_value_> >;
using opt_elt_format = element_formatter< _optional_, opt_format >;
```
### Format #4: element_counter

Now something more interesting: element that exists when optional has value, and doesn't exist, when optional is empty:
```xml
<value>value</value>
```
The formatter for this:
```cpp
using opt_format = optional_formatter< element_counter<_value_>, element_formatter<_value_> >;
```
Here we used `element_counter` to format empty/not-empty flag. If the count is greater than zero, then it means optional has a value.

### Format #5: assign_name

Here we will format optional is an unusual way: as an element, whose name is the value of the empty/not-empty flag:
```xml
<false/>
<true>value</true>
```
The formatter for this:
```cpp
declare_compile_time_string(_empty_, "");
using opt_format = optional_formatter< assign_name, assign_text_content<> >;
using opt_elt_format = element_formatter< _empty_, opt_format >;
```
Here we used `assign_name` to name the parent element.

### Final words
On the example of `boost::optional` we saw various ways to format data. Using the same tools it's very easy to format almost anything exactly the way you want it.
