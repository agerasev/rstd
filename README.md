# CPP-Core

Rust-like types and routines for C++17

## Usage

```cpp
#include <core/prelude.hpp>

using namespace core;

// Your code
```

## Types

+ `Union<Elems...>` - Templated analog of C `union`. It can store non-trivial types and supports move semantics.
+ `Variant<Elems...>` - Union with id of stored type. Similar to Rust `enum`.
+ `Tuple<Elems...>` - Tuple.

+ `Option<T>` - Type that stores something or nothing. Similar to Rust `Option`.
+ `Result<T, E>` - Type that stores one value on success and another on error. Similar to Rust `Result`.

+ `Box<T>` - Heap-located storage with ability to move the object inside and outside. Wrapper over C++ `unique_ptr`.
+ `Rc<T>` - Reference counting heap-located storage. Wrapper over C++ `shared_ptr`.

## Functions

+ `clone` - Makes explicit copy of object and returns it.
+ `drop` - Makes object to release its resources and enter an empty state.
