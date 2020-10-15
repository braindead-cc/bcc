brainfuck(7) "Useless Programmer's Manual"

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
points to the current memory cell. Each memory cell is typically an unsigned 8-bit
integer (although that detail can vary between implementations), and the array is
usually at least 30,000 cells wide. Each memory cell is initialized to zero.

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

# EXAMPLES

The following program simply prints out its input:

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
