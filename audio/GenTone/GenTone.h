/*------------ Telecommunications & Signal Processing Lab --------------
                         McGill University

Routine:
  GenTone.h

Description:
  Declarations for GenTone

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.17 $  $Date: 2003/11/06 13:25:38 $

----------------------------------------------------------------------*/

#ifndef GenTone_h_
#define GenTone_h_

#define PROGRAM "GenTone"
#define VERSION	"v5r0a  2003-11-06"

#define SQRT2	1.41421356237309504880
#define AMPL_DEF	(0.03 * SQRT2)


#include <libtsp.h>		/* p_ */
#include <AO.h>			/* struct AO_FOpar */

#define GT_FOpar	AO_FOpar	/* Output file structure */

struct GT_Sine {
  double Freq;		/* Frequency Hz */
  double Ampl;		/* Amplitude */
  double Phase;		/* Phase in Radians */
};

/* Informational message */
#define GTMF_SineFreq	" Sine wave frequency: %g\n"

/* Error messages */
#define GTM_BadAmpl	"Invalid amplitude value"
#define GTM_BadFreq	"Invalid sine wave frequency"
#define GTM_BadRMS	"Invalid rms value"
#define GTM_BadPhase	"Invalid sine wave phase"
#define GTM_XFName	"Too many filenames specified"
#define GTM_NoFName	"No filename specified"
#define GTM_NoNsamp	"Number of samples not specified"

/* Usage */
#define GTMF_Usage "\
Usage: %s [options] AFileO\n\
Options:\n\
  -n NSAMPLE, --number_samples=NSAMPLE  Number of output samples.\n\
  -f FREQ, --frequency=FREQ   Frequency of the sine wave.\n\
  -r RMS, --rms=RMS           Root-mean-square value of the sine wave.\n\
  -a AMPL, --amplitude=AMPL   Maximum amplitude of the sine wave.\n\
  -p PHASE, --phase=PHASE     Initial phase of the sine wave.\n\
  -s SFREQ, --srate=SFREQ     Sampling frequency for the output file.\n\
  -F FTYPE, --file_type=FTYPE  Output file type,\n\
                              \"AU\", \"WAVE\", \"AIFF\", \"AIFF-C\",\n\
                              \"noheader\", or \"noheader_swap\".\n\
  -D DFORMAT, --data_format=DFORMAT  Data format for the output file,\n\
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
GToptions (int argc, const char *argv[], struct GT_Sine *Sine,
	   struct GT_FOpar *FOfloat);

#ifdef __cplusplus
}
#endif

#endif /* GenTone_h_ */
