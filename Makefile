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

OBJDIR  = build

SRC     = $(wildcard src/*.c)
OBJ     = $(subst src/,$(OBJDIR)/,$(SRC:.c=.o))

WARNING = -Wall -Wextra -Wpedantic -Wold-style-definition -Winit-self \
	  -Wfloat-equal -Wstrict-prototypes -Wredundant-decls -Wendif-labels \
	  -Wimplicit-fallthrough=2 -Woverflow -Wformat=2 \
	  -Wmissing-include-dirs -Wno-trigraphs -Wno-format-nonliteral

INC     = -Iinclude/ -Iinclude/ccommon/
DEF     = -DVERSION=\"$(VERSION)\" -DBUILDDATE="\"$(DATE)\"" -D_GNU_SOURCE \
	  -DMEMTYPE=$(MEMTYPE)

CFLAGS  = -std=c99 $(WARNING) $(INC) $(DEF)
LDFLAGS = -fuse-ld=$(LD) -lncursesw

ifeq ($(ENABLE_FANCY_SPINNER), true)
	DEF += -DSPINNER_FANCY
endif

all: debug docs/brainfuck.7 docs/$(BIN).1

$(OBJDIR):
	@echo "  MKDIR    $@"
	$(CMD)mkdir -p $@

$(OBJDIR)/%.o:: src/%.c $(OBJDIR)
	@echo "  CC       $<"
	$(CMD)$(CC) $(CFLAGS) $(CFLAGS_OPT) -c $< \
		-o $(subst src/,$(OBJDIR)/,$(<:.c=.o))

debug: CFLAGS_OPT  = $(DEBUG_CFLAGS)
debug: LDFLAGS_OPT = $(DEBUG_LDFLAGS)
debug: $(OBJDIR)/$(BIN)

release: CFLAGS_OPT  = $(RELEASE_CFLAGS)
release: LDFLAGS_OPT = $(RELEASE_LDFLAGS)
release: $(OBJDIR)/$(BIN)

unity: CFLAGS_OPT  = $(RELEASE_CFLAGS)
unity: LDFLAGS_OPT = $(RELEASE_LDFLAGS)
unity: $(SRC)
	$(CMD)rm -f build/unity.c
	$(CMD)for src in $(SRC); \
	do \
		echo "#include \"../$$src\"" >> build/unity.c; \
	done
	@echo "  CC       build/unity.c"
	$(CMD)$(CC) -o $(OBJDIR)/$(BIN) $(CFLAGS) $(CFLAGS_OPT) \
		$(LDFLAGS) $(LDFLAGS_OPT) build/unity.c

$(OBJDIR)/$(BIN): $(OBJ)
	@echo "  CCLD     $@"
	$(CMD)$(CC) -o $@ $^ $(CFLAGS) $(CFLAGS_OPT) $(LDFLAGS) $(LDFLAGS_OPT)

docs/$(BIN).1: docs/$(BIN).scd
	@echo "  SCDOC    $^"
	$(CMD)scdoc < $^ > $@

docs/brainfuck.7: docs/brainfuck.scd
	@echo "  SCDOC    $^"
	$(CMD)scdoc < $^ > $@

clean:
	rm -rf $(OBJDIR) *.tar *.tar.xz docs/*.1 docs/*.7

dist-bin: $(BIN) docs/brainfuck.7 docs/$(BIN).1
	mkdir -p $(BIN)-$(VERSION)-$(shell arch)-bin
	cp -R $(OBJDIR) $(BIN) README.md LICENSE.md docs/ samples/ \
		$(BIN)-$(VERSION)-$(shell arch)-bin/
	tar -cf $(BIN)-$(VERSION)-$(shell arch)-bin.tar \
		$(BIN)-$(VERSION)-$(shell arch)-bin/
	xz $(BIN)-$(VERSION)-$(shell arch)-bin.tar
	rm -rf $(BIN)-$(VERSION)-$(shell arch)-bin/

dist-src: $(SRC) $(HDR)
	mkdir -p $(BIN)-$(VERSION)-src
	cp -R src/ include/ README.md LICENSE.md \
		makefile config.mk docs/ samples/ ccommon/ etc/ \
		$(BIN)-$(VERSION)-src/
	tar -cf $(BIN)-$(VERSION)-src.tar $(BIN)-$(VERSION)-src/
	xz $(BIN)-$(VERSION)-src.tar
	rm -rf $(BIN)-$(VERSION)-src/

# for CI artifacts
artifacts: dist-bin dist-src
	mv $(BIN)-$(VERSION)-$(shell arch)-bin.tar.xz lbf-bin.tar.xz
	mv $(BIN)-$(VERSION)-src.tar.xz lbf-src.tar.xz

install: $(BIN) docs/$(BIN).1 docs/brainfuck.7
	@echo "  INSTALL  $(BIN)"
	$(CMD)install -Dm755 $(BIN) $(DESTDIR)/$(PREFIX)/bin/$(BIN)
	@echo "  STRIP    $(BIN)"
	$(CMD)strip --strip-all $(DESTDIR)/$(PREFIX)/bin/$(BIN)
	@echo "  INSTALL  $(BIN).1"
	$(CMD)install -Dm644 docs/$(BIN).1 \
		$(DESTDIR)/$(PREFIX)/share/man/man1/$(BIN).1
	@echo "  INSTALL  brainfuck.7"
	$(CMD)install -Dm644 docs/brainfuck.7 \
		$(DESTDIR)/$(PREFIX)/share/man/man7/brainfuck.7

tests:
	$(CMD)cd tests && tclsh ./test.tcl

.PHONY: all debug release clean dist-bin dist-src artifacts install tests
