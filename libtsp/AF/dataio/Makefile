#
# Makefile libtsp/XX/YY
#

SHELL = /bin/sh

dist_dir = ../../..

dist_libdir = $(dist_dir)/lib
dist_includedir = $(dist_dir)/include

COPTS = -O
CFLAGS = $(COPTS) -I$(dist_includedir)

LIB = $(dist_libdir)/libtsp.a

.PHONY: all

all:	$(LIB)

.PRECIOUS: $(LIB)
$(LIB): \
	$(LIB)(AFgetLine.o) \
	$(LIB)(AFrdAlaw.o) \
	$(LIB)(AFrdFx.o) \
	$(LIB)(AFrdIx.o) \
	$(LIB)(AFrdMulaw.o) \
	$(LIB)(AFrdTA.o) \
	$(LIB)(AFrdU1.o) \
	$(LIB)(AFseek.o) \
	$(LIB)(AFtell.o) \
	$(LIB)(AFwrAlaw.o) \
	$(LIB)(AFwrFx.o) \
	$(LIB)(AFwrIx.o) \
	$(LIB)(AFwrMulaw.o) \
	$(LIB)(AFwrTA.o) \
	$(LIB)(AFwrU1.o) \
	#
	ranlib $(LIB)
