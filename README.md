<!-- begin joke -->

# The Braindead Compiler Collection

Brainfsck is an extremely simple, safe, and fast language for developing
maintainable software.

## Features

### Simplicity

> *complexity* is the bane of all software, *simplicity* is the most important
> quality.
>
> -- [cat-v.org](http://harmful.cat-v.org/software/)

The language itself consists of just a few commands, which can be learned by
going through the documentation in less than a minute. This results in
simple, readable<sub>1</sub>, and maintainable code.

Despite being simple, Brainfsck was designed to give power to the developer.
Anything you can do in any other language, you can theoretically do in
Brainfsck.

### Safety

- no `NULL`.
- no variables.
	- no global variables.
- bounds checking.

### Performance
- as fast as C (the compiler/interpreter is written in C).
- compiles to native binaries with no dependencies. <sub>2</sub>

<!-- TODO: add more marketing blurbs here -->

<!-- end joke -->

### Fast compilation

- compiling [`samples/lostkng.b`](samples/lostkng.b) with the C backend takes
  less than on a second with `bcc c -O2 samples/lostkng.b > lostkng.c`.

<!-- TODO: add more benches -->

### Small, portable, and easy to build compiler

- *portable*: this toolchain is written in C, and as we all know, no platform
  is complete unless it has a fully-functional C compiler.
- *fast to build*: The entire toolchain builds in less than a second. <sub>3</sub>
- *small*: the toolchain is about 1,500 lines of code total (including the
  Makefile and test script). The resulting binary is just 23k in size.

### Flexibility

- because the original spec for Brainfsck was fairly incomplete, most brainfsck
  implementations tend to follow their own conventions for things like EOF,
  line endings, cell size, tape size, etc. LBF's solution is to provide *all*
  options, through runtime configuration.

## Installation

See [INSTALL.md](INSTALL.md).

## Usage

See `bcc(1)` and `brainfuck(7)`.

## FAQ

1. **Q**: is this a joke?
    - **A**: if you mean the weeks I spent working on this, no. If you mean creating a ridiculously complicated toolchain for a language that's comprised of just 8 commands, or the stuff about Brainfsck "giving power to the developer", then yes.

---

1: whether your Brainfsck code is readable depends on what font you're
using. (I would recommend either `ttyp0` or `cozette`.)

2: *except* the `libc`. If you want to remove that dependency as well, you
can compile the generated C code with `-static`.

3: compiled with `gcc` with flags to optimize for space (`-Os -s`) on x86_64.
The command used was `make WARNINGS= LD=bfd release`. See the `makefile` for
more info.
