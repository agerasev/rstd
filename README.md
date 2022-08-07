# cpp-core

## Testing

```sh
mkdir -p build && cd build
conan install ../test/
cmake ../test/
make
./core_test
```

## Clang-Tidy

```sh
cd build
make core_tidy
```
