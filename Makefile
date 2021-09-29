_cflags := -pipe -std=c11 -Werror -Wall -Wpedantic
CC = cc
DESTDIR = .
PREFIX = $(HOME)/.local
PREFIX_BIN = $(PREFIX)/bin

.PHONY: all
all: $(DESTDIR)/ucprint

.PHONY: demo
demo: $(DESTDIR)/ucprint
	$< 0x259

.PHONY: install
install: all | $(PREFIX)/bin
	install -m 0755 $(DESTDIR)/ucprint $(PREFIX_BIN)

$(DESTDIR)/ucprint: ucprint.c | $(DESTDIR)
	$(CC) $(_cflags) -o $@ $<

$(PREFIX_BIN):
	mkdir -p $@
$(DESTDIR):
	mkdir -p $@
