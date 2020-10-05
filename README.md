# CPP-RSTD

Very incomplete C++17 replica of Rust's std.

## Usage

```cpp
#include <rstd/prelude.hpp>

using namespace rstd;

// Your code
```

## Naming

+ Safe types and functions (that always provides defined result or panics) has usual names like `Option` or `drop`.
+ Unsafe types and function (that could result in **UB** in some circumstances) has a single underscore `_` before their name. Examples: `_Union`, `Option::_take_some` and others.
+ Internal entities names starts with double underscore `__`. They shouldn't be used externally because of unintended side-effects and possibility of change in future versions.

## Types

### Base types

+ `_Union<Elems...>` - Templated analog of C `union`. It can store non-trivial types and supports move semantics. In `DEBUG` mode union panics on double initialization or double free (but not type mismatch).
+ `Variant<Elems...>` - Union with id of stored type. Similar to Rust `enum` but with a significant difference - it also includes an *empty* (or *none*) state because of requirements of C++ move semantics. 
+ `Tuple<Elems...>` - Sequence of objects of different types. The special case is empty tuple `Tuple<>` that is used as a placeholder when we need to deal with nothing.

+ `Option<T>` - Type that stores something or nothing. Similar to Rust `Option`.
+ `Result<T, E>` - Type that stores one value on success and another on error. Similar to Rust `Result` but with additional *empty* state - see `Variant`. Also in `DEBUG` mode result panics if it wasn't explicitly handled - use `Result::unwrap` or `Result::clear`.

### Memory managements

+ `Box<T>` - Heap-located storage with ability to move the object inside and outside. Wrapper over C++ `std::unique_ptr`.
+ `Rc<T>` - Reference counting heap-located storage. Wrapper over C++ `std::shared_ptr`.
+ ~~`Weak<T>`~~ - to be implemented.

### Concurrency

+ `Thread<T>` - POSIX-thread wrapper. Has its own `stdin_`, `stdout_` and `stderr_` and panic hook. In case of panic simply returns a `Err` from `join` without causing the whole program to be terminated.
+ `_Mutex` and `Mutex<T>` - POSIX-thread mutex. The second is the safe version of the first. `Mutex<T>` wraps some value allowing to access it only with lock providing `Guard` object that unlocks the mutex when going out of scope.

## Functions

+ `clone` - Makes explicit copy of object and returns it.
+ `move` - Moves the object. Unlike `std::move` it actually performs moving of object resources even if the result isn't passed anywhere.
+ `drop` - Makes object to release its resources and enter an empty state.

## Macros

+ `panic_` - Print formatted message and terminate thread.
+ `assert_` and `assert_eq_` - Assertion macros, panics on false condition with an information of arguments.

### Formatted output

+ `format_` - Write to string.
+ `write_` and `writeln_` - Write to specified `std::ostream`.
+ `print_` and `println_` - Write to `stdout_()`.
+ `eprint_` and `eprintln_` - Write to `stderr_()`.

Format examples:

```cpp
print_(); // > ""
print_(123); // > "123"
print_("abc"); // > "abc"
print_("a: {}, b: {};", 123, "abc"); // > "a: 123, b: abc;"
print_("{{abc}}"); // > "{abc}"
print_(std::string("{}")); // > "{}"
```

## Testing

The library provides its own testing framework.

### Usage

Define test cases in you multiple `.cpp` files:

```cpp
#include <rtest.hpp>

using namespace rstd;

rtest_module_(my_test) {
    rtest_(dummy) {
        
    }
    rtest_should_panic_(panic) {
        panic_("Panic!");
    }
    rtest_should_panic_(segfault) {
        ((void(*)())nullptr)();
    }
}
```

And include the following header in your single main `.cpp` file:

```cpp
#include <rtest/main.hpp>
```
