#
# Makefile for libtsp/XX
#

SHELL = /bin/sh

dist_dir = ../..

dist_libdir = $(dist_dir)/lib
dist_includedir = $(dist_dir)/include

COPTS = -O
CFLAGS = $(COPTS) -I$(dist_includedir)

LIB = $(dist_libdir)/libtsp.a

.PHONY: all nucleus_routines

all:	$(LIB) nucleus_routines

.PRECIOUS: $(LIB)
$(LIB): \
	$(LIB)(FIfBiquad.o) \
	$(LIB)(FIfConvSI.o) \
	$(LIB)(FIfConvol.o) \
	$(LIB)(FIfDeem.o) \
	$(LIB)(FIfFiltAP.o) \
	$(LIB)(FIfFiltIIR.o) \
	$(LIB)(FIfPreem.o) \
	$(LIB)(FIfWinHamm.o) \
	$(LIB)(FIreadFilt.o) \
	#
	ranlib $(LIB)

nucleus_routines:
	@(test -d nucleus && (cd nucleus; $(MAKE))) || true
	@(test -d dataio && (cd dataio; $(MAKE))) || true
	@(test -d header && (cd header; $(MAKE))) || true
