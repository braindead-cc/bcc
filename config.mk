#
# lbf: the LPTSTR brainfsck toolchain
# https://github.com/lptstr/lbf
#
# (c) Kiëd Llaentenn
# See the LICENSE.md file for more information
#

# set to '@' to hide executed commands, set it to empty
# to show commands
# default: '@'
CMD = @

# ----------------------------
# --- INSTALLATION OPTIONS ---
# ----------------------------

# destination directory for installation
# default: (null)
DESTDIR =

# prefix for installation path
# default: '/usr/local'
PREFIX = /usr/local

# ---------------------
# --- CONFIGURATION ---
# ---------------------

# binary name
# default: 'lbf'
BIN = lbf

# enable fancy Unicode spinners?
# (may cause strange behaviour on terminals that don't support Unicode)
# 'true' to enable, 'false' to disable
# default: 'true'
ENABLE_FANCY_SPINNER = true

# cell size for interpreter
# note: this DOES NOT affect compiler; do change compiler's cell size,
# use -fcell-size=X and -fcell-signed
#
# possible values: u8, u16, u32, u64 (unsigned),
#                  i8, i16, i32, i64 (signed)
# default: u8
MEMTYPE = u8

# -----------------------------
# --- COMPILE CONFIGURATION ---
# -----------------------------

# compiler
# tested with: clang, gcc, and tcc
# default: 'cc'
CC = cc

# linker
# possible values: bfd, gold, and lld
# NOTE: lld requires llvm installed
# default: 'gold'
LD = gold

# extra flags for release and debug
#
# default cflags for release: '-Os -march=native'
# default ldflags for release: '-s'
#
# default cflags for debug: '-O0 -ggdb'
# default ldflags for debug: (null)
DEBUG_CFLAGS    = -O0 -ggdb
DEBUG_LDFLAGS   =
RELEASE_CFLAGS  = -0s -march=native
RELEASE_LDFLAGS = -s
