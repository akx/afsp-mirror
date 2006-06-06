/*------------ Telecommunications & Signal Processing Lab --------------
                         McGill University

Routine:
  FiltAudio.h

Description:
  Declarations for FiltAudio

Author / revision:
  P. Kabal  Copyright (C) 2005
  $Revision: 1.63 $  $Date: 2005/02/01 13:26:09 $

----------------------------------------------------------------------*/

#ifndef FiltAudio_h_
#define FiltAudio_h_

#define PROGRAM "FiltAudio"
#define VERSION	"v6r1  2005-02-01"

#include <limits.h>		/* LONG_MIN */

#include <libtsp.h>		/* typedef for AFILE */
#include <AO.h>			/* struct AO_FIpar, AO_FOpar */

#define AFPATH_ENV	"$AUDIOPATH"

#define MAXCOF		4000
#define MAXWUP		1000
#define DOFFS_DEF	LONG_MIN

#define FA_LIM_UNDEF	AO_LIM_UNDEF	/* Undefined Limits */
#define FA_FIpar	AO_FIpar	/* Input file structure */
#define FA_FOpar	AO_FOpar	/* Output file structure */

struct FA_FFpar {
  char Fname[FILENAME_MAX];
  int Ir;
  int Nsub;
  long int Doffs;
};

#define FFpar_INIT(p) { \
	(p)->Fname[0] = '\0'; \
	(p)->Ir = 1; \
	(p)->Nsub = 1; \
	(p)->Doffs = DOFFS_DEF; }

/* Error messages */
#define FAM_APNoInt	"Interpolation not supported for all-pole filters"
#define FAM_BadAlign	"Invalid alignment offset"
#define FAM_BadFiltType	"Invalid filter type"
#define FAM_BadRatio	"Invalid interpolation ratio"
#define FAM_IIRNoInt	"Interpolation not supported for IIR filters"
#define FAM_MFName	"Too few filenames specified"
#define FAM_NoCoef	"No coeffients specified"
#define FAM_NoFFile	"No filter file specified"
#define FAM_XAPCof	"Too many filter coefficients"
#define FAM_XFName	"Too many filenames specified"
#define FAM_XIIRSect	"Too many filter sections"
#define FAM_XNchan	"Multiple input channels not supported"

/* Usage */
#define FAMF_Usage "\
Usage: %s [options] -f FiltFile AFileI AFileO\n\
Options:\n\
  -f FiltFile, --filter_file=FiltFile  Filter file name.\n\
  -i IR/NSUB, --interpolate=IR/NSUB  Interpolation ratio.\n\
  -a OFFS, --alignment=OFFS   Offset for first output.\n\
  -n NSAMPLE, --number_samples=NSAMPLE  Number of output samples.\n\
  -t FTYPE, --type=FTYPE      Input file type,\n\
                              \"auto\", \"AU\", \"WAVE\", \"AIFF\", \"noheader\",\n\
                              \"SPHERE\", \"ESPS\", \"IRCAM\", \"SPPACK\",\n\
                              \"INRS\", \"SPW\", \"NSP\", or \"text\".\n\
  -P PARMS, --parameters=PARMS  Parameters for headerless input files,\n\
                              \"Format,Start,Sfreq,Swapb,Nchan,ScaleF\".\n\
  -g GAIN, --gain=GAIN        Gain factor for the input file.\n\
  -F FTYPE, --file_type=FTYPE  Output file type,\n\
                              \"AU\", \"WAVE\", \"AIFF\", \"AIFF-C\",\n\
                              \"noheader\", or \"noheader_swap\".\n\
  -D DFORMAT, --data_format=DFORMAT  Data format for the output file,\n\
                              \"mu-law\", \"A-law\", \"unsigned8\", \"integer8\",\n\
                              \"integer16\", \"integer24\", \"float32\",\n\
                              \"float64\", or \"text\".\n\
  -I INFO, --info=INFO        Header information string.\n\
  -h, --help                  Print a list of options and exit.\n\
  -v, --version               Print the version number and exit."


#ifdef __cplusplus
extern "C" {
#endif

/* Prototypes */
void
FAfiltAP (AFILE *AFpI, AFILE *AFpO, long int NsampO, const double h[],
	  int Ncof, int Nsub, long int loffs);
void
FAfiltFIR (AFILE *AFpI, AFILE *AFpO, long int NsampO, const double h[],
	   int Ncof, long int loffs);
void
FAfiltIIR (AFILE *AFpI, AFILE *AFpO, long int NsampO, const double h[][5],
	   int Nsec, int Nsub, long int loffs);
void
FAfiltSI (AFILE *AFpI, AFILE *AFpO, long int NsampO, const double h[],
          int Ncof, int Nsub, int Ir, long int moffs);
void
FAoptions (int argc, const char *argv[], struct FA_FIpar *FI,
	   struct FA_FFpar *FF, struct FA_FOpar *FO);

#ifdef __cplusplus
}
#endif

#endif /* FiltAudio_h_ */
