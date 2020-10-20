#!/usr/bin/env tclsh
#
# Some tests for the interpeter/compiler.
# (c) Kiëd Llaentenn
# See ../COPYING for more information.

# maximum optimization level
set maxoptlevel 2

# test statistics
set passed 0
set failed 0

set bcc "../target/debug/riir"

proc assert_eq {val1 val2 msg} {
	global passed
	global failed

	if {$val1 == $val2} {
		incr passed
		puts "\033\[32m✔\033\[0m | $msg "
	} else {
		incr failed
		puts "\033\[31m✖\033\[0m | $msg "
	}
}

proc test_i_read_io { } {
	global bcc

	# check eof characters and linefeeds
	# see: http://www.hevanet.com/cristofd/brainfuck/tests.b

	# check that default EOF character is 0
	assert_eq [exec echo | $bcc "test-io.b"] "LB\nLB" \
		"EOF (default) == 0"

	# with 0
	assert_eq [exec echo | $bcc -feof=0 "test-io.b"] \
		"LB\nLB" "EOF (-feof-value=0) == 0"

	# with no change
	assert_eq [exec echo | $bcc -feof=none "test-io.b"] \
		"LK\nLK" "EOF (-feof-value=none) == none"

	# with -1
	assert_eq [exec echo | $bcc -feof=-1 "test-io.b"] \
		"LA\nLA" "EOF (-feof-value=-1) == -1"
}

proc test_c_read_io { } {
	global bcc

	# default
	exec $bcc c test-io.b | cc -xc -
	assert_eq [exec echo | ./a.out] "LB\nLB" "EOF (default) == 0"

	# with 0
	exec $bcc c -feof-value=0 test-io.b | cc -xc -
	assert_eq [exec echo | ./a.out] "LB\nLB" "EOF (-feof-value=0) == 0"

	# with -1
	exec $bcc c -feof-value=-1 test-io.b | cc -xc -
	assert_eq [exec echo | ./a.out] "LA\nLA" "EOF (-feof-value=-1) == -1"

	# with no change
	exec $bcc c -feof-value=none test-io.b | cc -xc -
	assert_eq [exec echo | ./a.out] "LK\nLK" \
		"EOF (-feof-value=none) == none"

}

proc test_i_tape_length { } {
	global bcc

	# test dynamic allocation
	assert_eq [exec echo | $bcc "test-tape-length.b"] \
		"#" "dynamic tape allocation"
}

proc test_c_tape_length { } {
	global bcc

	# test dynamic allocation on compiler
	exec $bcc c test-tape-length.b | cc -xc -
	assert_eq [exec ./a.out] "#" "dynamic tape allocation"
}

proc test_i_cell_size { } {
	global bcc

	# ensure default cell size for interpreter
	# is 8bits
	assert_eq [exec $bcc test-cell-size.b] \
		"8" "cell size (default) == 8"
}

proc test_c_cell_size { } {
	global bcc

	# test cell size for compiler
	
	# test default cell size
	exec $bcc c test-cell-size.b | cc -xc -
	assert_eq [exec ./a.out] "8" "cell size (default) == 8"

	# test cell size with cmd options
	exec $bcc c -fcell-size=8 test-cell-size.b | cc -xc -
	assert_eq [exec ./a.out] "8" "cell size (-fcell-size=8) == 8"

	exec $bcc c -fcell-size=16 test-cell-size.b | cc -xc -
	assert_eq [exec ./a.out] "16" "cell size (-fcell-size=16) == 16"

	exec $bcc c -fcell-size=32 test-cell-size.b | cc -xc -
	assert_eq [exec ./a.out] "32" "cell size (-fcell-size=32) == 32"

	# TODO: fix test-cell-size.b to print
	# 64 instead of 32 on 64-bit cell size

	#exec $bcc c -fcell-size=64 test-cell-size.b | cc -xc -
	#assert_eq [exec ./a.out] "64" "cell size (-fcell-size=64) == 64"
}

proc test_p_comments { } {
	global bcc

	# test commenting

	# test default comments
	assert_eq [exec $bcc "test-comments.b"] \
		"C" "comments (default)"

	# others
	assert_eq [exec $bcc -fcomment-char=\# "test-comments.b"] \
		"@" "comments (-fcomment-char='#')"
	assert_eq [exec $bcc -fcomment-char=\% "test-comments.b"] \
		"E" "comments (-fcomment-char='%')"
}

# output newline
puts ""

# test functions
set intp_tests [split [info commands test_i_*] " "]
set comp_tests [split [info commands test_c_*] " "]
set pars_tests [split [info commands test_p_*] " "]

puts "\033\[33m->\033\[0m performing tests on interpeter..."
foreach test $intp_tests {
	$test
}
puts ""

#puts "\033\[33m->\033\[0m performing tests on compiler..."
#foreach test $comp_tests {
#	$test
#}
#puts ""

puts "\033\[33m->\033\[0m performing tests on parser..."
foreach test $pars_tests {
	$test
}
puts ""

puts "completed [expr $passed+$failed] tests. $passed passed, $failed failed."
exec rm -f a.out
