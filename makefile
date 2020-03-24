#
# lbf: the LPTSTR Brainfsck Toolchain
# https://github.com/lptstr/lbf
#
# (c) Kiëd Llaentenn
# See the LICENSE.md file for more information
#

CMD     = @
DESTDIR =
PREFIX  = /usr/local

DATE    = $(shell date '+%Y-%m-%d %H%M')
VERSION = 0.1.0

BIN     = lbf
SRC     = util.c terminfo.c status.c emitc.c warn.c parser.c opt-squash.c \
	  opt-nloops.c opt-sloops.c prepare.c lbfi.c lbfc.c lbf.c
OBJ     = $(SRC:.c=.o)

WARNING = -Wall -Wextra -pedantic -Wmissing-prototypes -Wold-style-definition \
	  -Wno-incompatible-pointer-types -Wno-unused-parameter \
	  -Wno-unused-value -Wno-trigraphs

INC     = -I. -Iccommon/ -Ilibutf/include/
DEF     = -DVERSION=\"$(VERSION)\" -DBUILDDATE="\"$(DATE)\"" -D_GNU_SOURCE \
	  -DSPINNER_FANCY -DMEMTYPE=u8

CC      = gcc
LD      = lld
CFLAGS  = -std=c99 $(WARNING) $(INC) $(DEF)
LDFLAGS = -fuse-ld=$(LD)

all: debug

.c.o:
	@echo "  CC       $<"
	$(CMD)$(CC) $(CFLAGS) $(CFLAGS_OPT) -c $< -o $(<:.c=.o)

debug: CFLAGS_OPT := -O0 -ggdb
debug: $(BIN)

release: CFLAGS_OPT := -Os -march=native
release: LDFLAGS_OPT := -s
release: $(BIN)

$(BIN): $(OBJ)
	@echo "  LD       $@"
	$(CMD)$(CC) -o $@ $^ $(CFLAGS) $(CFLAGS_OPT) $(LDFLAGS) $(LDFLAGS_OPT)

docs/$(BIN).1: docs/$(BIN).scd
	@echo "  SCDOC    $^"
	$(CMD)scdoc < $^ > $@

docs/brainfuck.7: docs/brainfuck.scd
	@echo "  SCDOC    $^"
	$(CMD)scdoc < $^ > $@

clean:
	$(CMD)rm -f $(BIN) $(OBJ)

install: $(BIN) docs/$(BIN).1 docs/brainfuck.7
	@echo "  INSTALL  $(BIN)"
	$(CMD)install -Dm755 $(BIN) $(DESTDIR)/$(PREFIX)/bin/$(BIN)
	@echo "  STRIP    $(BIN)"
	$(CMD)strip --strip-all $(DESTDIR)/$(PREFIX)/bin/$(BIN)
	@echo "  LN       $(BIN)i"
	$(CMD)ln -fs $(DESTDIR)/$(PREFIX)/bin/$(BIN) \
		$(DESTDIR)/$(PREFIX)/bin/$(BIN)i
	@echo "  LN       $(BIN)c"
	$(CMD)ln -fs $(DESTDIR)/$(PREFIX)/bin/$(BIN) \
		$(DESTDIR)/$(PREFIX)/bin/$(BIN)c
	@echo "  INSTALL  $(BIN).1"
	$(CMD)install -Dm644 docs/$(BIN).1 \
		$(DESTDIR)/$(PREFIX)/share/man/man1/$(BIN).1
	@echo "  INSTALL  brainfuck.7"
	$(CMD)install -Dm644 docs/brainfuck.7 \
		$(DESTDIR)/$(PREFIX)/share/man/man7/brainfuck.7

tests:
	$(CMD)cd tests && tclsh ./test.tcl

.PHONY: all debug release clean install tests
