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
	$(LIB)(AFdRdFx.o) \
	$(LIB)(AFdRdIx.o) \
	$(LIB)(AFdRdTA.o) \
	$(LIB)(AFdRdXlaw.o) \
	$(LIB)(AFdWrFx.o) \
	$(LIB)(AFdWrIx.o) \
	$(LIB)(AFdWrTA.o) \
	$(LIB)(AFdWrXlaw.o) \
	$(LIB)(AFfRdFx.o) \
	$(LIB)(AFfRdIx.o) \
	$(LIB)(AFfRdTA.o) \
	$(LIB)(AFfRdXlaw.o) \
	$(LIB)(AFfWrFx.o) \
	$(LIB)(AFfWrIx.o) \
	$(LIB)(AFfWrTA.o) \
	$(LIB)(AFfWrXlaw.o) \
	$(LIB)(AFgetLine.o) \
	$(LIB)(AFposition.o) \
	$(LIB)(AFseek.o) \
	$(LIB)(AFtell.o) \
	#
	ranlib $(LIB)

mostlyclean:
	rm -f *.o
clean:	mostlyclean
