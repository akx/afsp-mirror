/*------------ Telecommunications & Signal Processing Lab --------------
                         McGill University

Routine:
  FiltAudio.h

Description:
  Declarations for FiltAudio

Author / revision:
  P. Kabal  Copyright (C) 2020
  $Revision: 1.80 $  $Date: 2020/11/30 12:32:12 $

----------------------------------------------------------------------*/

#ifndef FiltAudio_h_
#define FiltAudio_h_

#define PROGRAM "FiltAudio"
#define VERSION "v10r3  2020-11-30"

#include <limits.h>   /* LONG_MIN */

#include <libtsp.h>   /* typedef for AFILE */
#include <AO.h>       /* struct AO_FIpar, AO_FOpar */

#define AFPATH_ENV  "$AUDIOPATH"

#define MAXCOF      4000
#define MAXWUP      1000            /* Max warm-up points for IIR/ALL filters */
#define NBUF        5120            /* Buffer size for filter memory, data */
#define DOFFS_UNDEF LONG_MIN        /* Undefined data offset */

#define FA_LIM_UNDEF  AO_LIM_UNDEF  /* Undefined Limits */
#define FA_FIpar      AO_FIpar      /* Input file structure */
#define FA_FOpar      AO_FOpar      /* Output file structure */

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
  (p)->Doffs = DOFFS_UNDEF; }

/* Error messages */
#define FAM_APNoInt     "Interpolation not supported for all-pole filters"
#define FAM_BadAlign    "Invalid alignment offset"
#define FAM_BadFiltType "Invalid filter type"
#define FAM_BadRatio    "Invalid interpolation ratio"
#define FAM_BadSFreqRatio \
  "Incompatible sampling frequency and interpolation ratio"
#define FAM_IIRNoInt    "Interpolation not supported for IIR filters"
#define FAM_MFName      "Too few filenames specified"
#define FAM_NoCoef      "No coeffients specified"
#define FAM_NoFFile     "No filter file specified"
#define FAM_XFName      "Too many filenames specified"
#define FAM_XIIRSect    "Too many filter sections"
#define FAM_XNcof       "Too many filter coefficients"
#define FAM_XNcofIr     "No. coefficients and/or interpolation ratio too large"
#define FAM_XNchan      "Multiple input channels not supported"

/* Usage */
#define FAMF_Usage "\
Usage: %s [options] -f FiltFile AFileI AFileO\n\
Options:\n\
  -f FiltFile, --filter_file=FiltFile  Filter file name.\n\
  -i IR/NSUB, --interpolate=IR/NSUB  Interpolation ratio.\n\
  -a OFFS, --alignment=OFFS   Offset for first output.\n\
  -n NSAMPLE, --number_samples=NSAMPLE  Number of output samples.\n\
  -g GAIN, --gain=GAIN        Gain factor for the input file.\n\
  -F FTYPE, --file-type=FTYPE  Output file type,\n\
                              \"AU\", \"WAVE\", \"WAVE-NOEX\", \"AIFF\",\n\
                              \"AIFF-C\", \"AIFF-C/sowt\", \"noheader\",\n\
                              \"noheader-swap\", \"text-audio\".\n\
  -D DFORMAT, --data-format=DFORMAT  Data format for the output file,\n\
                              \"mu-law8\", \"mu-lawR8\", \"A-law8\", \"unsigned8\",\n\
                              \"integer8\", \"integer16\", \"integer24\",\n\
                              \"float32\", \"float64\", \"text16\", \"text\".\n\
  -I INFO, --info=INFO        Add a header information record.\n\
  -h, --help                  Print a list of options and exit.\n\
  -v, --version               Print the version number and exit.\n\
   Additional options [-t -P -S], see CopyAudio."

#ifdef __cplusplus
extern "C" {
#endif

/* Prototypes */
void
FAfiltAP(AFILE *AFpI, AFILE *AFpO, long int NsampO, const double h[],
         int Ncof, int Nsub, long int noffs);
void
FAfiltFIR(AFILE *AFpI, AFILE *AFpO, long int NsampO, const double h[],
          int Ncof, long int noffs);
void
FAfiltIIR(AFILE *AFpI, AFILE *AFpO, long int NsampO, const double h[][5],
          int Nsec, int Nsub, long int noffs);
void
FAfiltSI(AFILE *AFpI, AFILE *AFpO, long int NsampO, const double h[],
         int Ncof, int Nsub, int Ir, long int moffs);
void
FAoptions(int argc, const char *argv[], struct FA_FIpar *FI,
          struct FA_FFpar *FF, struct FA_FOpar *FO);

#ifdef __cplusplus
}
#endif

#endif /* FiltAudio_h_ */
