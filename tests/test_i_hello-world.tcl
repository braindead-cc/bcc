#!/usr/bin/env tclsh
#
# Test the interpreter with ../samples/hello.bf
# (c) Kiëd Llaentenn
# See the LICENSE.md file for more information

proc test_i_hello-world {opt} {
	set output "[exec ../lbf lbfi < ../samples/hello.bf -O$opt]"

	if { $output == [exec cat hello-result.txt] } {
		return true
	}

	return false
}
