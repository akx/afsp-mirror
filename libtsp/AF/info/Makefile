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
	$(LIB)(AFaddInfoChunk.o) \
	$(LIB)(AFaddInfoRec.o) \
	$(LIB)(AFdecInfoVVU.o) \
	$(LIB)(AFdecSpeaker.o) \
	$(LIB)(AFdelInfoRec.o) \
	$(LIB)(AFgetDelInfoRec.o) \
	$(LIB)(AFgetInfoBytes.o) \
	$(LIB)(AFgetInfoChan.o) \
	$(LIB)(AFgetInfoFrame.o) \
	$(LIB)(AFgetInfoFullScale.o) \
	$(LIB)(AFgetInfoNbS.o) \
	$(LIB)(AFgetInfoRec.o) \
	$(LIB)(AFgetInfoSfreq.o) \
	$(LIB)(AFgetInfoSpkr.o) \
	$(LIB)(AFgetInfoSwap.o) \
	$(LIB)(AFspeakerNames.o) \
	#
	ranlib $(LIB)

mostlyclean:
	rm -f *.o
clean:	mostlyclean
