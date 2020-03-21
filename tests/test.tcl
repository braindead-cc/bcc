#!/usr/bin/env tclsh
#
# Some tests for the interpeter/compiler.
# (c) Kiëd Llaentenn
# See the LICENSE.md file for more information.

# maximum optimization level
set maxoptlevel 2

# test statistics
set passed 0
set failed 0

proc hr { } {
	set ttywidth [lindex [split [exec stty size] " "] 1]
	for {set i 0} {$i < $ttywidth} {incr i} {
		puts -nonewline "─"
	}
	puts ""
}

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

proc test_eof_chars { } {
	# check eof characters and linefeeds
	# see: http://www.hevanet.com/cristofd/brainfuck/tests.b

	# check that default EOF character is 0
	assert_eq [exec echo | ../lbf lbfi "test-eof.b"] "LB\nLB" \
		"EOF (default) == 0"

	# with no change
	assert_eq [exec echo | ../lbf lbfi -feof-value=none "test-eof.b"] \
		"LK\nLK" "EOF (-feof-value=none) == none"

	# with -1
	assert_eq [exec echo | ../lbf lbfi -feof-value=-1 "test-eof.b"] \
		"LA\nLA" "EOF (-feof-value=-1) == -1"
}

# output newline
puts ""

# test functions
set tests [list [split [info commands test*] " "]]

puts "\033\[33m->\033\[0m performing tests on interpeter..."
hr

foreach test $tests {
	$test
}

hr
puts "completed [expr $passed+$failed] tests. $passed passed, $failed failed."
puts ""
