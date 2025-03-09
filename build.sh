#!/bin/sh

CC=${CC:=gcc}
SRCS="main.c karma.c karma_topic.c direct_link.c tcp_link.c"
BIN=karma

set -x

mkdir -p build
$CC -g -o build/$BIN $SRCS -fno-strict-aliasing