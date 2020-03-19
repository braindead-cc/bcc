# The LPTSTR Brainfsck Toolchain

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
- as fast as C (the interpreter is written in C).
- compiles to native binaries with no dependencies. <sub>2</sub>

### Fast compilation

<!-- TODO: add benches -->

### Small, portable, and easy to build compiler

- *portable*: this toolchain is written in C, and as we all know, no platform
  is complete unless it has a fully-functional C compiler.
- *fast to build*: The entire toolchain builds in less than a minute on
  sub-optimal hardware. <sub>3</sub>
- *small*: the toolchain is about 1,500 lines of code total (including the
  Makefile and test script). The resulting binary is less than 45k.

<!-- TODO: add more marketing blurbs here -->

---

1: whether your Brainfsck code is readable depends on what font you're
using. (I would recommend either `ttyp0` or `cozette`.)

2: *except* the `libc`. If you want to remove that dependency as well, you
can compile the generated C code with `-static`.

3: on a Raspberry Pi Zero, running Void Linux `musl` with 512MiB memory.
Compiled with the Clang compiler with flags to optimize for space (`-Os`).
