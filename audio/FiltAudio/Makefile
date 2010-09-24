#
#	Makefile for FiltAudio
#

PROG = FiltAudio
OBJECTS = \
	FiltAudio.o \
	FAfiltAP.o \
	FAfiltFIR.o \
	FAfiltIIR.o \
	FAfiltSI.o \
	FAoptions.o \
	#

HEADERS = FiltAudio.h

SHELL = /bin/sh

dist_dir = ../..

dist_libdir = $(dist_dir)/lib
dist_bindir = $(dist_dir)/bin
dist_includedir = $(dist_dir)/include

EXEEXT =
COPTS = -O
CFLAGS = $(COPTS) -I$(dist_includedir)
LDFLAGS = -s -lm

LLIBS = $(dist_libdir)/libAO.a $(dist_libdir)/libtsplite.a

.PHONY: mostlyclean clean

BIN = $(dist_bindir)/$(PROG)$(EXEEXT)
$(BIN): $(OBJECTS) $(LLIBS)
	$(CC) $(OBJECTS) $(LLIBS) $(LDFLAGS) -o $(BIN)

$(OBJECTS): $(HEADERS)

mostlyclean:
	rm -f *.o
clean:	mostlyclean
	rm -f $(BIN)
