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
	$(LIB)(VRswapBytes.o) \
	#
	ranlib $(LIB)
