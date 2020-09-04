#!/usr/bin/env bash

mkdir -p build && \
cd build && \
cmake .. && \
make test_core && \
./test_core $@
