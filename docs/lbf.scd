lbf(1) "General Commands Manual"

# NAME

lbf - an optimizing brainfsck compiler, interpreter, and debugger

# SYNOPSIS

lbf lbf[icd] [OPTION]... [FILE]

# DESCRIPTION

Compile, execute, or debug brainfsck program from FILE.

With no FILE, or when FILE is -, read from standard input.

*-V*
	Print version and exit.
*-h*
	Print a short help message and exit.
*-d*
	Enable debug messages.
*-v*
	Enable verbose messages.

*-b BACKEND*
	Set compiler backend (default: C backend).
	- *-bC*: enable C backend.
	- *-bLLVM*: enable LLVM IR backend.
	- *-bRust*: enable Rust backend.
	- *-bGo*: enable Go backend.
	- *-bQBE*: enable QBE backend.

*-f OPTION*
	Set OPTION.
	- *-fenable-dbg*: enable debug ('#') command. (usually ignored.)
	- *-foptimize-multiple-commands*: compress multiple adjacent commands of the same type into a single instruction.
	- *-foptimize-scan-loops*: optimize scan loops (e.g., '[>]') into a single instruction.
	- *-foptimize-nullify-loops*: optimize nullifying loops (e.g., '[-]') into a single instruction.
	- *-fcell-size=SIZE*: set the compiler cell bit size. possible values: 8 (default), 16, 32, 64.
	- *-fcell-signed*: enable signed (that is, negative) cell values.
	- *-finitial-tape-size=SIZE*: set the initial size allocated for tape. 16 bytes are allocated by default.
	- *-fdisable-dynamic-alloc*: normally, when the tape's pointer moves beyond the tape, additional memory is allocated. this flag disables that behaviour, and causes any pointer movements beyond the tape to be silently ignored.
	- *-fcommand-char=CHAR*: cause any text beyond CHAR to be ignored as a command. default: *;*
	- *-feof-value=EOF*: set the EOF value for the read (',') command. possible values: -1 (interpreted as 255 on unsigned cells, -1 otherwise), 0 (default), none (no change).

*-O OPTLEVEL*
	Set the optimization level to OPTLEVEL.
	- *-O0*: no optimization.
	- *-O1*: equivalent to '-foptimize-multiple-commands'.
	- *-O2*: equivalent to '-O1 -foptimize-nullify-loops -foptimize-scan-loops'.

*-W WARNING*
	Enable WARNING.
	- *-Wlong-lines*: warn if a line in FILE is longer than 80 characters.
	- *-Wdead-code*: warn if adjacent loops or other dead code is encountered.
	- *-Wignored-dbg*: warn instead of silently ignoring a debug ('#') command when an '!' follows it. (see QUIRKS below.)

# COMPILER

# INTERPRETER

# DEBUGGER

# EXAMPLE USAGE

# QUIRKS

The following unexpected behaviours are features and not bugs.

- The '#' command is ignored if it precedes a '!' character. This is to avoid
  showing a debug message in a brainfsck script with a shebang.

# REPORTING BUGS

Please report any encountered bugs to the Sourcehut ticket tracker
at _https://todo.sr.ht/~kiedtl/lbf_.

# COPYRIGHT

Copyright © 2020 Kiëd Llaentenn.

Permission is granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights to
(mis)use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is furnished
to do so, subject to the following conditions:

- The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

*this software is provided "as is", without warranty of any kind, express or
implied, including but not limited to the warranties of merchantability,
fitness for a particular purpose and noninfringement. in no event shall the
authors or copyright holders be liable for any claim, damages or other
liability, whether in an action of contract, tort or otherwise, arising from,
out of or in connection with the software or the use or other dealings in
the software.*

# SEE ALSO

*brainfsck*(7)

The full documentation for *lbf* is not maintained as a Texinfo manual.
If the *info* and *lbf* programs are properly installed on your system,
the command

	*info lbf*

should not give you access to the complete manual.
