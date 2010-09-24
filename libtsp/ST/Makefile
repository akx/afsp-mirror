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
	$(LIB)(STcatMax.o) \
	$(LIB)(STcopyMax.o) \
	$(LIB)(STcopyNMax.o) \
	$(LIB)(STdec1double.o) \
	$(LIB)(STdec1float.o) \
	$(LIB)(STdec1int.o) \
	$(LIB)(STdec1long.o) \
	$(LIB)(STdecDfrac.o) \
	$(LIB)(STdecIfrac.o) \
	$(LIB)(STdecLrange.o) \
	$(LIB)(STdecNdouble.o) \
	$(LIB)(STdecNfloat.o) \
	$(LIB)(STfindToken.o) \
	$(LIB)(STkeyMatch.o) \
	$(LIB)(STkeyXpar.o) \
	$(LIB)(STstrLC.o) \
	$(LIB)(STtrim.o) \
	$(LIB)(STunQuote.o) \
	#
	ranlib $(LIB)

nucleus_routines:
	@(test -d nucleus && (cd nucleus; $(MAKE))) || true
	@(test -d dataio && (cd dataio; $(MAKE))) || true
	@(test -d header && (cd header; $(MAKE))) || true

mostlyclean:
	rm -f *.o */*.o
clean:	mostlyclean
