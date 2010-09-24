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
	$(LIB)(AFclose.o) \
	$(LIB)(AFdReadData.o) \
	$(LIB)(AFdWriteData.o) \
	$(LIB)(AFfReadData.o) \
	$(LIB)(AFfWriteData.o) \
	$(LIB)(AFopnRead.o) \
	$(LIB)(AFopnWrite.o) \
	$(LIB)(AFsetFileType.o) \
	$(LIB)(AFsetInfo.o) \
	$(LIB)(AFsetNHpar.o) \
	$(LIB)(AFsetSpeaker.o) \
	$(LIB)(AFopenRead.o) \
	$(LIB)(AFopenWrite.o) \
	$(LIB)(AFreadData.o) \
	$(LIB)(AFwriteData.o) \
	#
	ranlib $(LIB)

nucleus_routines:
	@(test -d nucleus && (cd nucleus; $(MAKE))) || true
	@(test -d dataio && (cd dataio; $(MAKE))) || true
	@(test -d header && (cd header; $(MAKE))) || true

mostlyclean:
	rm -f *.o */*.o
clean:	mostlyclean
