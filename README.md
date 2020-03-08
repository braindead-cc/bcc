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
simple, readable[^1], and maintainable code.

Despite being simple, Brainfsck was designed to give power to the developer.
Anything you can do in any other language, you can theoretically do in
Brainfsck.

### Safety

- no `NULL`.
- no variables.
	- no global variables.
- bounds checking.
- only one memory allocation.

### Performance
- as fast as C (as long as you use the C backend when compiling).
- only one `malloc` call.
- compiles to native binaries with no dependencies.

### Fast compilation

<!-- TODO: add benches -->

### Small, portable, and easy to build compiler

- This toolchain is written in C, and as we all know, no platform
  is complete unless it has a fully-functional C compiler.

<!-- TODO: add more marketing blurbs here -->

[^1]: whether your Brainfsck code is readable depends on what font you're
using. (I would recommend either TTyp0 or Cozette.)
