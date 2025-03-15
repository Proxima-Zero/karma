#!/bin/sh

CC=${CC:=gcc}
SRCS="karma.c karma_channel.c direct_link.c tcp_link.c"
TESTS=tests/*.c

for test in $TESTS; do
	echo "Running test $test..."
	testbin="$test.bin"
	$CC -g -o "$testbin" $SRCS $test
	if ! ./$testbin > /dev/null; then
		echo "FAILURE"
		exit 1
	else
		echo "SUCCESS"
		rm $testbin
	fi
done
