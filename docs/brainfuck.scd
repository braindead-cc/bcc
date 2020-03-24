brainfuck(7) "Linux Programmer's Manual"

# NAME

brainfuck - overview of the Brainfuck programming language

# DESCRIPTION

Brainfuck, also known as brainfsck, brainfrick, brainf\*\*\*, or simply bf, is
an esoteric language designed in 1993 by Urban Müller in an attempt to create a
language for which he could write the smallest possible compiler.

# LANGUAGE

Each character in a brainfuck program is executed as a command if it is
recognized as such, and is ignored as a comment if it is not. Brainfuck programs
operate on an array of memory cells called the *tape*, and the *pointer*, which
points to the current memory cell. Each memory cell is typically 8-bits wide
(although it can vary between implementations), and the array is usually at
least 30,000 cells wide. Each memory cell is initialized to 0x00.

The list of recognized commands is as follows:

[- *COMMAND*
:< *DESCRIPTION*
|   +
:  increment the current memory cell.
|  -
:  decrement the current memory cell.
|  >
:  move the pointer to the right.
|  <
:  move the pointer to the left.
|  [
:  jump to the matching ']' if the current memory cell is zero.
|  ]
:  jump back to the matching ']' if the current memory cell is nonzero.
|  .
:  print the current memory cell as a character to stdout.
|  ,
:  read a character from stdin and store it in the current memory cell.


Nested loops (i.e., the \[ and \] commands) are allowed, as long as each \[/\]
has a matching bracket. Non-matching brackets are a syntax error, and may
be either ignored or cause the interpreter/compiler to crash (depending on
the implementation).

# EXTENSIONS

Several extra commands for brainfuck exist, most notably the \# command, which
prints out the values of all memory cells for debugging purposes.

LBF also adds several non-standard extensions, the use of which is strongly
discouraged, as it makes the brainfuck program incompatible with other
implementations. For the LBF interpreter/compiler to recognize them, they must
be invoked with the `-fenable-lbf-std` option.

[- *LBF COMMAND*
:< *DESCRIPTION*
|   \*
:  set the current memory cell to zero. (equivalent to [-] in normal brainfuck
   code.
|  ^
:  set the pointer to the first cell in the tape.
|  &
:  output the current memory cell as a character to stderr.
|  {
:  move the pointer to the first memory cell of the value zero left of the
   current pointer location.
|  }
:  move the pointer to the first memory cell of the value zero right of the
   current pointer location.
|  @
:  exit the program with the value of the current memory cell as the exit code.

# EXAMPLES

The following program simply prints out it's input:

```
,[.,]
```

The classic "Hello World!" program, written by KSab from
_https://codegolf.stackexchange.com/a/163590/59487_:

```
+[-->-[>>+>-----<<]<--<---]>-.>>>+.>>..+++[.>]<<<<.+++.------.<<-.
>>>>+.
```

# NOTABLE PROGRAMS

- *The Lost Kingdom* is a small text adventure game by Jon Ripley. At 2.2M,
  this monster is most likely the largest brainfuck program in existence.
    - *Source*: jonripley.com/i-fiction/games/LostKingdomBF.zip
    - *Readme*: jonripley.com/i-fiction/games/LostKingdomBF.html
- Claire Wolf's 54kB *Towers of Hanoi* program.
    - *Source*: www.clifford.at/bfcpu/hanoi.bf
    - *Readme*: www.clifford.at/bfcpu/hanoi.html
- Erik Bosman's *mandelbrot* set viewer.
    - *Source*: esoteric.sange.fi/brainfuck/utils/mandelbrot.tar.gz
