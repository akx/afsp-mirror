#
# Makefile libAO/XX
#

SHELL = /bin/sh

dist_dir = ../..

dist_libdir = $(dist_dir)/lib
dist_includedir = $(dist_dir)/include

COPTS = -O
CFLAGS = $(COPTS) -I$(dist_includedir)
ARFLAGS = rUv

LIB = $(dist_libdir)/libAO.a

.PHONY: all mostlyclean clean

all:	$(LIB)

.PRECIOUS: $(LIB)
$(LIB): \
	$(LIB)(AOsetFtypeW.o) \
	#
	ranlib $(LIB)

mostlyclean:
	rm -f *.o
clean:	mostlyclean
