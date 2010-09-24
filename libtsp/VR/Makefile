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
	$(LIB)(VRdAdd.o) \
	$(LIB)(VRdCopy.o) \
	$(LIB)(VRdCorSYm.o) \
	$(LIB)(VRdDotProd.o) \
	$(LIB)(VRdLinInc.o) \
	$(LIB)(VRdMult.o) \
	$(LIB)(VRdRFFTMSq.o) \
	$(LIB)(VRdScale.o) \
	$(LIB)(VRdSet.o) \
	$(LIB)(VRdShift.o) \
	$(LIB)(VRdZero.o) \
	$(LIB)(VRfCorSym.o) \
	$(LIB)(VRfDotProd.o) \
	$(LIB)(VRfMult.o) \
	$(LIB)(VRfShift.o) \
	$(LIB)(VRfZero.o) \
	$(LIB)(VRiZero.o) \
	#
	ranlib $(LIB)

nucleus_routines:
	@(test -d nucleus && (cd nucleus; $(MAKE))) || true
	@(test -d dataio && (cd dataio; $(MAKE))) || true
	@(test -d header && (cd header; $(MAKE))) || true

mostlyclean:
	rm -f *.o */*.o
clean:	mostlyclean
