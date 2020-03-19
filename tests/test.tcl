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

# output newline
puts ""

# interpeter tests
set i_tests [split [glob test_i_*] " "]

puts "\033\[33m->\033\[0m performing tests on interpeter..."
hr

foreach test $i_tests {
	# each test contains a test function,
	# which we then run.
	source $test
	
	# test name
	set t_name [string map {test_i_ ""} $test]
	set t_name [string map {.tcl    ""} $t_name]

	# test procedure name
	set p_name [string map {.tcl    ""} $test]

	set status ""

	for {set o 0} {$o <= $maxoptlevel} {incr o} {
		if {[$p_name $o]} {
			incr passed
			set status "\033\[32m✔\033\[0m"
		} else {
			incr failed
			set status "\033\[31m✖\033\[0m"
		}

		puts "$status │ $t_name (optimization: $o)"
	}
}

hr
puts "completed [expr $passed+$failed] tests. $passed passed, $failed failed."
puts ""
