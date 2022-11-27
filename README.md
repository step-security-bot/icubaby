# icubaby 

[![CI Build & Test](https://github.com/paulhuggett/icubaby/actions/workflows/ci.yaml/badge.svg)](https://github.com/paulhuggett/icubaby/actions/workflows/ci.yaml)
[![Quality Gate Status](https://sonarcloud.io/api/project_badges/measure?project=paulhuggett_icubaby&metric=alert_status)](https://sonarcloud.io/summary/new_code?id=paulhuggett_icubaby)

A C++ Baby Library to Immediately Convert Unicode. A portable, header-only, dependency-free, library for C++ 17 or later. Fast, minimal, and easy to use for converting a sequence in any of UTF-8, UTF-16, or UTF-32. It does not allocate dynamic memory and neither throws or catches exceptions.

> icubaby is in no way related to the [International Components for Unicode](https://icu.unicode.org) library!

## Introduction

C++ 17 deprecated the standard library's `<codecvt>` header file which contained its unicode conversion facets. 

## Usage

Let’s try converting a single Unicode emoji character 😀 (U+1F600 GRINNING FACE) which is expressed as four UTF-8 code units (0xF0, 0x9F, 0x98, 0x80) to UTF-16 (where it is the surrogate pair 0xD83D, 0xDE00).

~~~cpp
std::vector<char16_t> out;
auto it = std::back_inserter (out);
icubaby::t8_16 t;
for (auto cu: {0xF0, 0x9F, 0x98, 0x80}) {
  it = t (cu, it);
}
it = t.end_cp (it);
~~~

The `out` vector will contain a two UTF-16 code units 0xD83D and 0xDE00.

### Disecting this code 

1.  Define where and how the output should be written:

    ~~~cpp
    std::vector<char16_t> out;
    auto it = std::back_inserter (out);
    ~~~

    For the purposes of this example, we write the encoded output to a `std::vector<char16_t>`. Use the container of your choice!

2.  Create the transcoder instance:

    ~~~cpp
    icubaby::t8_16 t;
    ~~~

3.  Pass each code unit and the output iterator to the transcoder.

    ~~~cpp
    for (auto cu: {0xF0, 0x9F, 0x98, 0x80}) {
      it = t (cu, it);
    }
    ~~~

4.  Tell the transcoder that we’ve reached the end of the input. This ensures that the sequence didn’t end part way through a code point.

    ~~~cpp
    it = t.end_cp (it);
    ~~~

    It’s only necessary to make a single call to `end_cp()` once *all* of the input has been fed to the transcoder.

## API

### Macro constants

Macro name            | Description
--------------------- | -----------
ICUBABY_CXX20         | Has value 1 when compiled with C++ 20 or later and 0 otherwise.
ICUBABY_CXX20REQUIRES | Used to enable use of the `require` keyword to state template constaints when compiled with C++ 20. An empty macro when compiled with versions of C++ prior to 20.

### Helper types

Name | Description
------ | -------------------------------------------------
t8_8   | A transcoder which converts from UTF-8 to UTF-8.<br>Equivalent to `using t8_8 = transcoder<char8_t, char8_t>`.
t8_16  | A transcoder which converts from UTF-8 to UTF-16.<br>Equivalent to `using t8_16 = transcoder<char8_t, char16_t>`.
t8_32  | A transcoder which converts from UTF-8 to UTF-32.<br>Equivalent to `using t8_32 = transcoder<char8_t, char32_t>`.
t16_8  | A transcoder which converts from UTF-16 to UTF-8.<br>Equivalent to `using t16_8 = transcoder<char16_t, char8_t>`.
t16_16 | A transcoder which converts from UTF-16 to UTF-16.<br>Equivalent to `using t16_16 = transcoder<char16_t, char16_t>`.
t16_32 | A transcoder which converts from UTF-16 to UTF-32.<br>Equivalent to `using t16_32 = transcoder<char16_t, char32_t>`.
t32_8  | A transcoder which converts from UTF-32 to UTF-8.<br>Equivalent to `using 32_8 = transcoder<char32_t, char8_t>`.
t32_16 | A transcoder which converts from UTF-32 to UTF-16.<br>Equivalent to `using t32_16 = transcoder<char32_t, char16_t>`.
t32_32 | A transcoder which converts from UTF-32 to UTF-32.<br>Equivalent to `using t32_32 = transcoder<char32_t, char32_t>`.

### char8

~~~cpp
namespace icubaby {
#if ICUBABY_CXX20
using char8 = char8_t;
#else
using char8 = char;
#endif
} // end namespace icubaby
~~~

C++ 20 introduced `char8_t` as the type for UTF-8 character representation. Since this library needs to work with earlier versions of C++, we have `icubaby::char8` which is defined as either `char8_t` (for C++ 20 or later) or `char` to match the type of a u8 string literal.

### transcoder

~~~cpp
template <typename From, typename To>
class icubaby::transcoder {
public:
  using input_type = From;
  using output_type = To;

  template <typename OutputIterator>
  OutputIterator operator() (input_type c, OutputIterator dest);

  template <typename OutputIterator>
  OutputIterator end_cp (OutputIterator dest);

  bool well_formed () const;
};
~~~

Where `From` and `To` are each any of `icubaby::char8`, `char16_t`, or `char32_t`.

It’s possible for `From` and `To` to be the same character type. This can be used to both validate and/or correct unchecked input such as data arriving at a network port.

#### Member types

Member type | Definition
----------- | -----------
input_type  | The character type from which conversions will be performed. May be any of `icubaby::char8`, `char16_t` or `char32_t`.
output_type | The character type to which the transcoder will convert. May be any of `icubaby::char8`, `char16_t` or `char32_t`.

#### Member functions

Member function | Description
--------------- | -----------
(constructor)   | Constructs a new transcoder.
(destructor)    | Destructs a transcoder.
operator()      |  (input_type c, OutputIt dest)
end_cp          | Call once the entire input has been fed to operator() to ensures the sequence did not end with a partial character.
well_formed     | Returns true if the input was well formed, false otherwise.


### iterator

~~~cpp
template <typename Transcoder, typename OutputIterator>
class icubaby::iterator;
~~~

`Transcoder` should be a type which implements the `transcoder<>` interface described above; `OutputIterator` should be an output-iterator which produces values of type `Transcoder::output_type`.

#### Member types

Member type       | Definition
----------------- | --------------------------
iterator_category | `std::output_iterator_tag`
value_type        | `void`
difference_type   | `std::ptrdiff_t`
pointer           | `void`
reference         | `void`

#### Member functions

Member function | Description
--------------- | -----------
(constructor)   | Constructs a new iterator<br><small>(public member function)</small>.
operator=       | Passes an individual character to the associated transcoder<br><small>(public member function)</small>.
operator*       | no-op<br><small>(public member function)</small>
operator++      | no-op<br><small>(public member function)</small>
operator++(int) | no-op<br><small>(public member function)</small>
base            | Accesses the underlying iterator<br><small>(public member function)</small>.
transcoder      | Accesses the underlying transcoder<br><small>(public member function)</small>.

## Examples

### Convert using std::copy()

The example code below converts from UTF-8 to UTF-16 using `icubaby::t8_16` (this name is just a shortened form of `icubaby::transcoder<icubaby::char8, char16_t>`). To convert from UTF-<i>x</i> to UTF-<i>y</i> just use t<i>x</i>\_<i>y</i> (UTF-16 to UTF-8 is `t16_8`, UTF-32 to UTF-8 is `t32_8`, and so on).

In this code we use `std::copy()` to loop over the input code units and pass them to `icubaby::iterator<>`. `iterator<>` conveniently passes each code unit to the transcoder along with the supplied output iterator (`std::back_inserter(out)` here) and returns the updated iterator.

This code continues to process and generate characters even if we see badly formed input. Once all of the characters are processed, the function will return an empty optional if any input was bad. If the input was well formed, the UTF-16 equivalent is returned.

~~~cpp
#include "icubaby/icubaby.hpp"
#include <string_view>

// In C++17, std::u8stringview would become std::basic_string_view<icubaby::char8>

std::optional<std::u16string> convert (std::u8string_view const& src) {
  std::u16string out;

  // t8_16 is the class which converts from UTF-8 to UTF-16.
  icubaby::t8_16 utf_8_to_16;
  // We could combine the next three lines, if desired.
  auto it = icubaby::iterator{&utf_8_to_16, std::back_inserter (out)};
  it = std::copy (std::begin (src), std::end (src), it);
  utf_8_to_16.end_cp (it);
  if (!utf_8_to_16.well_formed ()) {
    // The input was malformed or ended with a partial character.
    return std::nullopt;
  }
  return out;
}

~~~

### Convert using an explicit loop

Whilst std::copy() provides a compact method of performing conversions, sometimes more control is needed. This example converts the code units using an explicit loop and exits as soon as malformed input is encountered.

~~~cpp
std::optional<std::u16string>
convert2 (std::u8string const & src) {
  // The UTF-16 code units are written to 'out' via the 'it' iterator.
  std::u16string out;
  auto it = std::back_inserter (out);
  icubaby::t8_16 utf_8_to_16;
  for (auto const c: src) {
    // Pass this UTF-8 code-unit to the transcoder.
    it = utf_8_to_16(c, it);
    if (!utf_8_to_16.well_formed()) {
      // The input was malformed. Bail immediately.
      return std::nullopt;
    }
  }
  // Tell the converter that this it the end of the sequence.
  utf_8_to_16.end_cp (it)
  // Check that we didn't end with a partial character.
  if (!utf_8_to_16.well_formed ()) {
    return std::nullopt;
  }
  return out; // Conversion was successful.
}
~~~
