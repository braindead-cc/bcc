#
# lbf: the LPTSTR Brainfsck Toolchain
# https://github.com/lptstr/lbf
#
# (c) Kiëd Llaentenn
# See the LICENSE.md file for more information
#

include config.mk

DATE    = $(shell date '+%Y-%m-%d %H%M')
VERSION = 0.1.0

SRC     = util.c terminfo.c status.c emitc.c warn.c parser.c opt-squash.c \
	  opt-nloops.c opt-sloops.c prepare.c lbfi.c lbfc.c lbf.c
HDR     = $(SRC:.c=.h) instructions.h tape.h
OBJ     = $(SRC:.c=.o)

WARNING = -Wall -Wextra -pedantic -Wmissing-prototypes -Wold-style-definition \
	  -Wno-incompatible-pointer-types -Wno-unused-parameter \
	  -Wno-unused-value -Wno-trigraphs

INC     = -I. -Iccommon/
DEF     = -DVERSION=\"$(VERSION)\" -DBUILDDATE="\"$(DATE)\"" -D_GNU_SOURCE \
	  -DMEMTYPE=$(MEMTYPE)

CFLAGS  = -std=c99 $(WARNING) $(INC) $(DEF)
LDFLAGS = -fuse-ld=$(LD)

ifeq ($(ENABLE_FANCY_SPINNER), true)
	DEF += -DSPINNER_FANCY
endif

all: debug docs/brainfuck.7 docs/$(BIN).1

.c.o:
	@echo "  CC       $<"
	$(CMD)$(CC) $(CFLAGS) $(CFLAGS_OPT) -c $< -o $(<:.c=.o)

debug: CFLAGS_OPT  = $(DEBUG_CFLAGS)
debug: LDFLAGS_OPT = $(DEBUG_LDFLAGS)
debug: $(BIN)

release: CFLAGS_OPT  = $(RELEASE_CFLAGS)
release: LDFLAGS_OPT = $(RELEASE_LDFLAGS)
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
	rm -f $(BIN) $(OBJ) *.tar *.tar.xz docs/*.1 docs/*.7

dist-src: clean $(SRC) $(HDR)
	mkdir -p $(BIN)-$(VERSION)-src
	cp -R $(SRC) $(HDR) README.md LICENSE.md \
		makefile config.mk docs/ samples/ ccommon/ etc/ \
		$(BIN)-$(VERSION)-src/
	tar -cf $(BIN)-$(VERSION)-src.tar $(BIN)-$(VERSION)-src/
	xz $(BIN)-$(VERSION)-src.tar
	rm -rf $(BIN)-$(VERSION)-src/

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

.PHONY: all debug release clean dist-src install tests
