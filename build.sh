#!/bin/sh

CC=${CC:=gcc}
SRCS=""
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
