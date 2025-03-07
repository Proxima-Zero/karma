#!/bin/sh

CC=${CC:=gcc}
SRCS="main.c karma.c direct_link.c"
BIN=karma

set -x

mkdir -p build
$CC -g -o build/$BIN $SRCS -fno-strict-aliasing