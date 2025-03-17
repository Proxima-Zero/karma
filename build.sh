#!/bin/sh

CC=${CC:=gcc}
SRCS="main.c karma.c karma_channel.c direct_link.c tcp_link.c"
TARGET=build/libkarma.a
INCLUDE_PATHS="-I./deps"

set -x

mkdir -p build

for src in $SRCS; do
	obj="build/${src%.c}.o"
	$CC $INCLUDE_PATHS -c "$src" -o "$obj"
	OBJS="$OBJS $obj"
done

ar rcs "$TARGET" $OBJS
