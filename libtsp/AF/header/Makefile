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
	$(LIB)(AFdecSpeaker.o) \
	$(LIB)(AFfindType.o) \
	$(LIB)(AFgetHrec.o) \
	$(LIB)(AFoptions.o) \
	$(LIB)(AFprAFpar.o) \
	$(LIB)(AFpreSetWPar.o) \
	$(LIB)(AFrdAIhead.o) \
	$(LIB)(AFrdAUhead.o) \
	$(LIB)(AFrdBLhead.o) \
	$(LIB)(AFrdEShead.o) \
	$(LIB)(AFrdHinfo.o) \
	$(LIB)(AFrdHtext.o) \
	$(LIB)(AFrdINhead.o) \
	$(LIB)(AFrdNShead.o) \
	$(LIB)(AFrdSFhead.o) \
	$(LIB)(AFrdSPhead.o) \
	$(LIB)(AFrdSWhead.o) \
	$(LIB)(AFrdTAhead.o) \
	$(LIB)(AFrdWVhead.o) \
	$(LIB)(AFreadHead.o) \
	$(LIB)(AFsetNHread.o) \
	$(LIB)(AFsetNHwrite.o) \
	$(LIB)(AFsetRead.o) \
	$(LIB)(AFsetWrite.o) \
	$(LIB)(AFupdAIhead.o) \
	$(LIB)(AFupdAUhead.o) \
	$(LIB)(AFupdHead.o) \
	$(LIB)(AFupdWVhead.o) \
	$(LIB)(AFwrAIhead.o) \
	$(LIB)(AFwrAUhead.o) \
	$(LIB)(AFwriteHead.o) \
	$(LIB)(AFwrWVhead.o) \
	#
	ranlib $(LIB)
