#
# Makefile for libAO
#

SHELL = /bin/sh

dist_dir = ..

dist_libdir = $(dist_dir)/lib
dist_includedir = $(dist_dir)/include

COPTS = -O
CFLAGS = $(COPTS) -I$(dist_includedir)

LIB = $(dist_libdir)/libAO.a

.PHONY: all

all:	$(LIB)

.PRECIOUS: $(LIB)
$(LIB): \
	$(LIB)(AOdecFI.o) \
	$(LIB)(AOdecFO.o) \
	$(LIB)(AOdecHelp.o) \
	$(LIB)(AOdecOpt.o) \
	$(LIB)(AOnFrame.o) \
	$(LIB)(AOsetFIopt.o) \
	$(LIB)(AOsetFOopt.o) \
	$(LIB)(AOsetFormat.o) \
	$(LIB)(AOstdin.o) \
	#
	ranlib $(LIB)
