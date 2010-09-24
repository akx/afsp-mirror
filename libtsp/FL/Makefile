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
	$(LIB)(FLbackup.o) \
	$(LIB)(FLbaseName.o) \
	$(LIB)(FLdefName.o) \
	$(LIB)(FLdirName.o) \
	$(LIB)(FLexpHome.o) \
	$(LIB)(FLextName.o) \
	$(LIB)(FLfileDate.o) \
	$(LIB)(FLfullName.o) \
	$(LIB)(FLgetLine.o) \
	$(LIB)(FLgetRec.o) \
	$(LIB)(FLhomeDir.o) \
	$(LIB)(FLpathList.o) \
	$(LIB)(FLpreName.o) \
	#
	ranlib $(LIB)

nucleus_routines:
	@(test -d nucleus && (cd nucleus; $(MAKE))) || true
	@(test -d dataio && (cd dataio; $(MAKE))) || true
	@(test -d header && (cd header; $(MAKE))) || true

mostlyclean:
	rm -f *.o */*.o
clean:	mostlyclean
