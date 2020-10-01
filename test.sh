#!/usr/bin/env bash

mkdir -p build && \
cd build && \
cmake .. && \
make test_rstd && \
./test_rstd $@
