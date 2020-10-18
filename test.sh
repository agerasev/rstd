#!/usr/bin/env bash

mkdir -p build && \
cd build && \
cmake .. -DUSE_PCH=ON && \
make rstd_test -j $(grep -c ^processor /proc/cpuinfo) && \
./rstd_test $@
