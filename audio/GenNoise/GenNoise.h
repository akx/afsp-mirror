/*------------ Telecommunications & Signal Processing Lab --------------
                         McGill University

Routine:
  GenNoise.h

Description:
  Declarations for GenNoise

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.35 $  $Date: 2003/11/06 13:23:59 $

----------------------------------------------------------------------*/

#ifndef GenNoise_h_
#define GenNoise_h_

#define PROGRAM "GenNoise"
#define VERSION	"v5r0a  2003-11-06"

#define RMS_DEFAULT 0.03

#include <libtsp.h>		/* p_ */
#include <AO.h>			/* struct AO_FOpar */

#define GN_FOpar	AO_FOpar	/* Output file structure */

/* Error messages */
#define GNM_BadSeed	"Invalid seed value"
#define GNM_BadStdDev	"Invalid standard deviation"
#define GNM_XFName	"Too many filenames specified"
#define GNM_NoFName	"No filename specified"
#define GNM_NoNsamp	"Number of samples not specified"

/* Usage */
#define GNMF_Usage "\
Usage: %s [options] AFileO\n\
Options:\n\
  -n NSAMPLE, --number_samples=NSAMPLE  Number of output samples.\n\
  -d SDEV, --std_deviation=SDEV  Standard deviation of the noise samples.\n\
  -x SEED, --seed=SEED        Seed for the random number generator.\n\
  -s SFREQ, --srate=SFREQ     Sampling frequency for the output file.\n\
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
GNoptions (int argc, const char *argv[], double *rms, int *seed,
	   struct GN_FOpar *FO);

#ifdef __cplusplus
}
#endif

#endif /* GenNoise_h_ */
