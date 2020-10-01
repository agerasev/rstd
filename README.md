# CPP-RSTD

C++17 replica of Rust's `std`.

## Usage

```cpp
#include <rstd/prelude.hpp>

using namespace rstd;

// Your code
```

## Types

+ `Union<Elems...>` - Templated analog of C `union`. It can store non-trivial types and supports move semantics.
+ `Variant<Elems...>` - Union with id of stored type. Similar to Rust `enum`.
+ `Tuple<Elems...>` - Tuple.

+ `Option<T>` - Type that stores something or nothing. Similar to Rust `Option`.
+ `Result<T, E>` - Type that stores one value on success and another on error. Similar to Rust `Result`.

+ `Box<T>` - Heap-located storage with ability to move the object inside and outside. Wrapper over C++ `std::unique_ptr`.
+ `Rc<T>` - Reference counting heap-located storage. Wrapper over C++ `std::shared_ptr`.

## Functions

+ `clone` - Makes explicit copy of object and returns it.
+ `drop` - Makes object to release its resources and enter an empty state.

## Macros

+ `panic_` - Print formatted message and call `std::abort`.

### Formatted output

+ `format_` - Write to string.
+ `write_` and `writeln_` - Write to specified `std::ostream`.
+ `print_` and `println_` - Write to `std::cout`.
+ `eprint_` and `eprintln_` - Write to `std::cerr`.

Format examples:

```cpp
print_(); // > ""
print_(123); // > "123"
print_("abc"); // > "abc"
print_("a: {}, b: {};", 123, "abc"); // > "a: 123, b: abc;"
print_("{{abc}}"); // > "{abc}"
print_(std::string("{}")); // > "{}"
```
