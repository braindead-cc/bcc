# Building from source

**NOTE:** in the future, prebuilt binaries will be available.


## Dependencies

You will need:

- GNU Make -- the `makefile` isn't compatible with `bmake` (yet)
- a C99 compiler (tested with Clang, GCC, and TCC)
- [`scdoc`](https://git.sr.ht/~sircmpwn/scdoc)
- TCL (*optional*, only required for test suite)

## Retrieve LBF source

There are two methods you may use to retrieve the source for `lbf`: via
`git`, or downloading a tarball from the releases.

**via git**:

```
$ git clone https://git.sr.ht/~kiedtl/lbf.git --recurse
```

**via source**:

TODO: add


## Building/Installing

```
$ vi config.mk           # edit config.mk to your liking
$ make release install   # may require root
```


**NOTE**: run `make debug` for a debug build instead of a release build.
