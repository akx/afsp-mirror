/*------------ Telecommunications & Signal Processing Lab --------------
                         McGill University

Routine:
  GenTone.h

Description:
  Declarations for GenTone

Author / revision:
  P. Kabal  Copyright (C) 2020
  $Revision: 1.33 $  $Date: 2020/11/30 12:32:28 $

----------------------------------------------------------------------*/

#ifndef GenTone_h_
#define GenTone_h_

#define PROGRAM "GenTone"
#define VERSION "v10r3  2020-11-30"

#define SQRT2 (1.41421356237309504880)
#define AMPL_DEF  (0.03 * SQRT2)


#include <libtsp.h>
#include <AO.h>       /* struct AO_FOpar */

#define GT_FOpar  AO_FOpar  /* Output file structure */

struct GT_Sine {
  double Freq;    /* Frequency Hz */
  double Ampl;    /* Amplitude */
  double Phase;   /* Phase in Radians */
};

/* Informational message */
#define GTMF_SineFreq " Sine wave frequency: %g\n"

/* Error messages */
#define GTM_BadAmpl   "Invalid amplitude value"
#define GTM_BadFreq   "Invalid sine wave frequency"
#define GTM_BadRMS    "Invalid rms value"
#define GTM_BadPhase  "Invalid sine wave phase"
#define GTM_XFName    "Too many filenames specified"
#define GTM_NoFName   "No filename specified"
#define GTM_NoNsamp   "Number of samples not specified"

/* Usage */
#define GTMF_Usage "\
Usage: %s [options] AFileO\n\
Options:\n\
  -n NSAMPLE, --number-samples=NSAMPLE  Number of output samples.\n\
  -f FREQ, --frequency=FREQ   Frequency of the sine wave.\n\
  -r RMS, --rms=RMS           Root-mean-square value of the sine wave.\n\
  -a AMPL, --amplitude=AMPL   Maximum amplitude of the sine wave.\n\
  -p PHASE, --phase=PHASE     Initial phase of the sine wave.\n\
  -s SFREQ, --srate=SFREQ     Sampling frequency for the output file.\n\
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
  -v, --version               Print the version number and exit."

#ifdef __cplusplus
extern "C" {
#endif

/* Prototypes */
void
GToptions(int argc, const char *argv[], struct GT_Sine *Sine,
          struct GT_FOpar *FO);

#ifdef __cplusplus
}
#endif

#endif /* GenTone_h_ */
