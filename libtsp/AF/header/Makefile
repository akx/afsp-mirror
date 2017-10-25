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
	$(LIB)(AFfindFtype.o) \
	$(LIB)(AFrdAIhead.o) \
	$(LIB)(AFrdAUhead.o) \
	$(LIB)(AFrdBLhead.o) \
	$(LIB)(AFrdEShead.o) \
	$(LIB)(AFrdInfoText.o) \
	$(LIB)(AFrdINhead.o) \
	$(LIB)(AFrdNShead.o) \
	$(LIB)(AFrdSFhead.o) \
	$(LIB)(AFrdSPhead.o) \
	$(LIB)(AFrdSWhead.o) \
	$(LIB)(AFrdTAhead.o) \
	$(LIB)(AFrdWVhead.o) \
	$(LIB)(AFreadHead.o) \
	$(LIB)(AFsetChunkLim.o) \
	$(LIB)(AFsetNHread.o) \
	$(LIB)(AFsetNHwrite.o) \
	$(LIB)(AFupdAIhead.o) \
	$(LIB)(AFupdAUhead.o) \
	$(LIB)(AFupdHead.o) \
	$(LIB)(AFupdWVhead.o) \
	$(LIB)(AFwrAIhead.o) \
	$(LIB)(AFwrAUhead.o) \
	$(LIB)(AFwriteHead.o) \
	$(LIB)(AFwrTAhead.o) \
	$(LIB)(AFwrWVhead.o) \
	#
	ranlib $(LIB)

mostlyclean:
	rm -f *.o
clean:	mostlyclean
