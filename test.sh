#!/usr/bin/env bash

mkdir -p build && \
cd build && \
cmake .. && \
make rstd_test && \
./rstd_test $@
