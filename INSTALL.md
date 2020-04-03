# Building from source

**NOTE:** in the future, prebuilt binaries will be available.

## Dependencies

You will need:

- CMake
- GNU Make (`gmake`)
- a POSIX-compliant shell (e.g. bash or dash)
- a C99 compiler (tested with Clang, GCC, and TCC)
- [`scdoc`](https://git.sr.ht/~sircmpwn/scdoc)
- TCL (*optional*, only required for test suite)

## Retrieve the source

There are two methods you may use to retrieve the source: via
`git`, or downloading a tarball from the releases.

**via git**:

```
$ git clone https://git.sr.ht/~kiedtl/bcc.git --recurse --depth=1
```

**via source**:

TODO: add

## Building/Installing

With CMake:

```
$ mkdir -p build
$ cd build
$ cmake ..
# make install
```

With GNU Make:

```
$ vi config.mk # do your configuring here
# gmake install
```
