#!/usr/bin/env bash

mkdir -p build && \
cd build && \
cmake -G "MinGW Makefiles" .. -DUSE_PCH=ON && \
mingw32-make rstd_test -j $(grep -c ^processor /proc/cpuinfo) && \
./rstd_test $@
