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
	$(LIB)(UTdate.o) \
	$(LIB)(UTerror.o) \
	$(LIB)(UTfree.o) \
	$(LIB)(UTgetOption.o) \
	$(LIB)(UThalt.o) \
	$(LIB)(UTmalloc.o) \
	$(LIB)(UTrealloc.o) \
	$(LIB)(UTsetProg.o) \
	$(LIB)(UTsysMsg.o) \
	$(LIB)(UTuserName.o) \
	$(LIB)(UTwarn.o) \
	#
	ranlib $(LIB)

nucleus_routines:
	@(test -d nucleus && (cd nucleus; $(MAKE))) || true
	@(test -d dataio && (cd dataio; $(MAKE))) || true
	@(test -d header && (cd header; $(MAKE))) || true