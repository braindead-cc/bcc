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

VERSION = \"0.1.0\"

BIN     = lbf
SRC     = exec.c lbf.c lbfi.c parser.c util.c
OBJ     = $(SRC:.c=.o)
LIBUTF  = libutf/lib/libutf.a

WARNING = -Wall -Wextra -pedantic -Wmissing-prototypes -Wold-style-definition \
	  -Wno-incompatible-pointer-types -Wno-unused-parameter \
	  -Wno-unused-value -Wno-trigraphs
INC     = -I. -Iccommon/ -Ilibutf/include/

CC      = clang
LD      = lld
CFLAGS  = -std=c99 -DVERSION=$(VERSION) -D_DEFAULT_SOURCE $(WARNING) $(INC)
LDFLAGS = -fuse-ld=$(LD)

all: debug

.c.o:
	@echo "  CC       $<"
	$(CMD)$(CC) $(CFLAGS) $(CFLAGS_OPT) -c $< -o $(<:.c=.o)

debug: CFLAGS_OPT := -O0 -ggdb
debug: $(BIN)

release: CFLAGS_OPT := -O4 -s
release: $(BIN)

$(BIN): $(OBJ) $(LIBUTF)
	@echo "  CC       $@"
	$(CMD)$(CC) -o $@ $^ $(CFLAGS) $(CFLAGS_OPT) $(LDFLAGS)

$(LIBUTF):
	$(CMD)cd libutf && make

$(BIN).1: $(BIN).scd
	@echo "  SCDOC    $^"
	$(CMD)scdoc < $^ > $@

clean:
	$(CMD)rm -f $(BIN) $(OBJ)

install: $(BIN) $(BIN).1
	@echo "  INSTALL  $(BIN)"
	$(CMD)install -Dm755 $(BIN) $(DESTDIR)/$(PREFIX)/bin/$(BIN)
	@echo "  INSTALL  $(BIN).1"
	$(CMD)install -Dm644 $(BIN).1 $(DESTDIR)/$(PREFIX)/share/man/man1/$(BIN).1

uninstall:
	@echo "  RM       $(DESTDIR)/$(PREFIX)/bin/$(BIN)"
	$(CMD)rm -f $(DESTDIR)/$(PREFIX)/bin/$(BIN)
	@echo "  RM       $(DESTDIR)/$(PREFIX)/bin/$(BIN).1"
	$(CMD)rm -f $(DESTDIR)/$(PREFIX)/share/man/man1/$(BIN).1

.PHONY: all debug release clean install uninstall