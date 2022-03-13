# cerealise -- minimal binary serialisation for C++

Cerealise implements binary serialisation and deserialisation of user-defined
and built-in types in C++.

It has minimal dependencies (no STL use by default), doesn't use exceptions,
and doesn't allocate memory, and should therefore be possible to use in
restricted environments, like microcontrollers or real-time systems.

It currently requires C++20, though would be easy enough to port to earlier
versions.

Cerealise is inspired by [cereal](https://uscilab.github.io/cereal/), which has
many more features, but is much bigger. If cereal fits your requirements, you
should use it instead!

## Usage

Include the header:

```cpp
#include "cerealise/cerealise.hpp"
```

### Type Adapters

There are two ways to add cerealise support to your types -- intrusive, or
via specialisation.

To add support intrusively, define a static `cerealise` method in your types::

```cpp
struct Test {
  uint8_t x;
  uint32_t y;

  template <typename T, typename F> static bool cerealise(T &value, F &f) {
    return f(value.x) && f(value.y);
  }
};
```

Or, specialise `cerealise::Adapter`:

```cpp
template <> struct cerealise::Adapter<Test> {
  template <typename T, typename F> static bool adapt(T &value, F &f) {
    return f(value.x) && f(value.y);
  }
};
```

- `T` will be `Test` or `const Test`, and `value` is the value to parse to or
  unparse from.

- `F` is an internal cerealise buffer type. When parsing, operations on it will
  read from a buffer, and when unparsing it will write to a buffer.

- The method should return true if value was successfully parsed/unparsed.
  Buffer operations return true for success, so operations can be chained with
  `&&`.

### API

The following functions are defined in the `cerealise` namespace:

```cpp
/// parse data from buf, writing into v
///
/// buf_len: length of buf, the maximum possible size
/// bytes_read: the number of bytes read from buf
///
/// returns true if parsing was successful (enough data, and no other errors)
template <typename T>
bool parse(T &v, uint8_t *buf, size_t buf_len, size_t &bytes_read);

/// serialise v into buf
///
/// buf_len: length of buf, the maximum possible size
/// bytes_written: the number of bytes written to buf
///
/// returns true if serialisation was successful (enough space, and no other
/// errors)
template <typename T>
bool unparse(const T &v, uint8_t *buf, size_t buf_len, size_t &bytes_written);

/// get the number of bytes required to serialise v
///
/// returns 0 in case of error
template <typename T>
size_t measure(const T &v);
```

A typical usage would look something like:

```cpp
// the value to serialise
Test value{1, 999};

// how many bytes do we need
size_t len = cerealise::measure(value);
assert(len);

std::vector<uint8_t> buf(len);

// unparse to buf
size_t unparse_len;
bool result = cerealise::unparse(value, buf.data(), len, unparse_len);
assert(result);
assert(unparse_len == len);

// parse from buf
Test parsed_value;
size_t parse_len;
result = cerealise::parse(parsed_value, buf.data(), len, parse_len);
assert(result);
assert(parse_len == len);

// now, value == parsed_value
```

### STL Adapters

The library includes adapters for some STL types in separate headers:

`std::array<T>`: `cerealise/array.hpp`

`std::optional<T>`: `cerealise/optional.hpp`

`std::string<T>`: `cerealise/string.hpp`

`std::variant<T>`: `cerealise/variant.hpp`

`std::vector<T>`: `cerealise/vector.hpp`

Others are easy to add, just not done yet.

## Details

### Buffer Operations

The following operations/formats are defined on all buffer types:

- `f(T &value)` parses/unparses `value`, using one of the registered adapters.

- `f.byte(uint8_t &value)` reads or writes one byte.

- `f.bytes(uint8_t *data, size_t n)` reads or writes n bytes.

- `f.boolean(bool &value)` reads or writes a bool encoded as a byte containing 0
  or 1.

- `f.native(T &value)` reads or writes the native representation of value in
  big-endian order, (with appropriate byte swapping). This must be a single
  value, like a float or double.

- `f.fixedint<size_t size>(T &value)` reads or writes a big-endian integer with
  `size` bytes (which defaults to `sizeof(T)`. For signed types the native
  representation is used (i.e. two's complement on any sensible platform). This
  may fail if there's not enough bits in the type to represent the value.

- `f.varint(T &value)` reads or writes a signed or unsigned integer using a
  big-endian variable length encoding, [similar to
  protobuf](https://developers.google.com/protocol-buffers/docs/encoding#varints).
  Parsing may fail if there's not enough bits in the type to represent the
  value.

### Parsing or Unparsing?

Both parsing and unparsing are implemented in one method. Often the operations
required are the same for both, but not always. In that case you can use
`constexpr bool F::parsing` to handle both cases. For an example of this, see
[vector.hpp](include/cerealise/vector.hpp).

## License

    Copyright 2022 Thomas Nixon

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
