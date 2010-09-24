#
# Makefile for libtsplite/XX
#

SHELL = /bin/sh

dist_dir = ../..

dist_libdir = $(dist_dir)/lib
dist_includedir = $(dist_dir)/include

COPTS = -O
CFLAGS = $(COPTS) -I$(dist_includedir)

LIB = $(dist_libdir)/libtsplite.a

.PHONY: all nucleus_routines mostlylean clean

all:	$(LIB) nucleus_routines

.PRECIOUS: $(LIB)
$(LIB): \
	$(LIB)(FIdBiquad.o) \
	$(LIB)(FIdConvSI.o) \
	$(LIB)(FIdConvol.o) \
	$(LIB)(FIdFiltAP.o) \
	$(LIB)(FIdFiltIIR.o) \
	$(LIB)(FIdReadFilt.o) \
	$(LIB)(FIdWinHamm.o) \
	$(LIB)(FIfConvol.o) \
	$(LIB)(FIfDeem.o) \
	$(LIB)(FIfFiltAP.o) \
	$(LIB)(FIfPreem.o) \
	$(LIB)(FIfWinHamm.o) \
	#
	ranlib $(LIB)

nucleus_routines:
	@(test -d nucleus && (cd nucleus; $(MAKE))) || true
	@(test -d dataio && (cd dataio; $(MAKE))) || true
	@(test -d header && (cd header; $(MAKE))) || true

mostlyclean:
	rm -f *.o */*.o
clean:	mostlyclean
