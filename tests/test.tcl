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

# outputs
set results [split [glob *-result.txt] " "]

puts "\033\[33m->\033\[0m performing tests on interpeter..."
hr

foreach r $results {
	set name [string map {-result.txt ""} $r]

	set status ""

	for {set o 0} {$o <= $maxoptlevel} {incr o} {
		set output [exec ../lbf lbfi < ../samples/$name.bf -O$o]

		if {$output == [exec cat $r]} {
			incr passed
			set status "\033\[32m✔\033\[0m"
		} else {
			incr failed
			set status "\033\[31m✖\033\[0m"
		}

		puts "$status │ $name (optimization: $o)"
	}
}

hr
puts "completed [expr $passed+$failed] tests. $passed passed, $failed failed."
puts ""
