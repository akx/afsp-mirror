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

.PHONY: all mostlyclean clean

all:	$(LIB)

.PRECIOUS: $(LIB)
$(LIB): \
	$(LIB)(AOdecFI.o) \
	$(LIB)(AOdecFO.o) \
	$(LIB)(AOdecHelp.o) \
	$(LIB)(AOdecOpt.o) \
	$(LIB)(AOnFrame.o) \
	$(LIB)(AOsetDformat.o) \
	$(LIB)(AOsetFIopt.o) \
	$(LIB)(AOsetFOopt.o) \
	$(LIB)(AOsetFtype.o) \
	$(LIB)(AOstdin.o) \
	$(LIB)(AOsetFormat.o) \
	#
	ranlib $(LIB)

mostlyclean:
	rm -f *.o
clean:	mostlyclean

