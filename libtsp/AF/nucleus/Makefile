#
# Makefile libtsplite/XX/YY
#

SHELL = /bin/sh

dist_dir = ../../..

dist_libdir = $(dist_dir)/lib
dist_includedir = $(dist_dir)/include

COPTS = -O
CFLAGS = $(COPTS) -I$(dist_includedir)
ARFLAGS = rUv

LIB = $(dist_libdir)/libtsplite.a

.PHONY: all mostlyclean clean

all:	$(LIB)

.PRECIOUS: $(LIB)
$(LIB): \
	$(LIB)(AFcheckDataPar.o) \
	$(LIB)(AFoptions.o) \
	$(LIB)(AFpreSetWPar.o) \
	$(LIB)(AFprintAFpar.o) \
	$(LIB)(AFsetRead.o) \
	$(LIB)(AFsetWrite.o) \
	#
	ranlib $(LIB)

mostlyclean:
	rm -f *.o
clean:	mostlyclean
