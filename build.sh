#!/bin/sh

CC=${CC:=gcc}
SRCS=main.c
BIN=karma

set -x

mkdir -p build
$CC -o build/$BIN $SRCS