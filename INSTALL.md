# Building from source

**NOTE:** in the future, prebuilt binaries will be available.

## Dependencies

You will need:

- either Ninja or Samurai
- Meson
- a POSIX-compliant shell (e.g. bash or dash)
- a C99 compiler (tested with Clang, GCC, and TCC)
- [`scdoc`](https://git.sr.ht/~sircmpwn/scdoc)
- TCL (*optional*, only required for test suite)

## Retrieve LBF source

There are two methods you may use to retrieve the source for `lbf`: via
`git`, or downloading a tarball from the releases.

**via git**:

```
$ git clone https://git.sr.ht/~kiedtl/lbf.git --recurse --depth=1
```

**via source**:

TODO: add

## Building/Installing

```
$ meson --buildtype=release build
$ ninja -C build
# ninja -C build install
```
