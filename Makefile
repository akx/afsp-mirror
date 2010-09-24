#
# Makefile for AFsp
#   make [COPTS=-O]
#
#   make test
#
#   make mostlyclean   - remove object files, test logs
#   make clean         - remove object files, test logs, binaries, libraries
#
# $Id: Makefile 1.21 2003/08/19 AFsp-v9r0 $

SHELL=/bin/sh

INSTALL = install -c
INSTALL_PROGRAM = $(INSTALL)
INSTALL_DATA = $(INSTALL) -m 664
INSTALL_DIR = $(INSTALL) -d

prefix=/usr/local
exec_prefix=$(prefix)
bindir=$(exec_prefix)/bin
libdir=$(exec_prefix)/lib
includedir=$(prefix)/include
mandir=$(prefix)/man
datadir=$(prefix)/share

.PHONY: all libtsp libAO audio test install install_bin install_data \
	install-strip mostlyclean clean

all:	libtsp libAO audio

libtsp:
	@-test -d lib || mkdir lib
	cd libtsp; $(MAKE)
libAO:
	@-test -d lib || mkdir lib
	cd libAO; $(MAKE)
audio:
	@-test -d bin || mkdir bin
	cd audio; $(MAKE)
test:
	cd test; $(MAKE)

# Install
install_bin: all
	@-test -d $(bindir) || $(INSTALL_DIR) $(bindir)
	$(INSTALL_PROGRAM) bin/* $(bindir)
	@-test -d $(libdir) ||	$(INSTALL_DIR) $(libdir)
	$(INSTALL_DATA) lib/* $(libdir)

install: install_bin
	@-test -d $(includedir) || $(INSTALL_DIR) $(includedir)
	$(INSTALL_DATA) include/*.h $(includedir)
	@-test -d $(includedir)/libtsp || $(INSTALL_DIR) $(includedir)/libtsp
	$(INSTALL_DATA) include/libtsp/* $(includedir)/libtsp
	@-test -d $(mandir)/man1 || $(INSTALL_DIR) $(mandir)/man1
	$(INSTALL_DATA) man/audio/* $(mandir)/man1
	@-test -d $(mandir)/man3 || $(INSTALL_DIR) $(mandir)/man3
	$(INSTALL_DATA) man/libtsp/AF/* $(mandir)/man3

# Install extras
install_data:
	@-test -d $(datadir)/AFsp/html/audio || \
		$(INSTALL_DIR) $(datadir)/AFsp/html/audio
	$(INSTALL_DATA) html/audio/* $(datadir)/AFsp/html/audio
	@-test -d $(datadir)/AFsp/html/libtsp/AF || \
		$(INSTALL_DIR) $(datadir)/AFsp/html/libtsp/AF
	$(INSTALL_DATA) html/libtsp/AF/* $(datadir)/AFsp/html/libtsp/AF
	@-test -d $(datadir)/AFsp/filters || \
		$(INSTALL_DIR) $(datadir)/AFsp/filters
	$(INSTALL_DATA) filters/* $(datadir)/AFsp/filters

install-strip:
	$(MAKE) INSTALL_PROGRAM='$(INSTALL_PROGRAM) -s' install

# Clean-up
mostlyclean:
	cd audio; $(MAKE) mostlyclean
	cd libAO; $(MAKE) mostlyclean
	cd libtsp; $(MAKE) mostlyclean
	cd test; rm -f *.log
clean:	mostlyclean
	cd lib; rm -f *
	cd bin; rm -f *
