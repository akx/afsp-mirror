/*------------ Telecommunications & Signal Processing Lab --------------
                         McGill University

Routine:
  AO.h

Description:
  Declarations for audio utility program options routines

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.36 $  $Date: 2003/11/04 12:51:30 $

----------------------------------------------------------------------*/

#ifndef AO_h_
#define AO_h_

#include <limits.h>
#include <stdio.h>		/* FILENAME_MAX */

#include <libtsp/AFpar.h>	/* struct AF_NHpar, AFILE */
#include <libtsp/sysOS.h>	/* SY_OS */

#include <AObase.h>		/* Basic AO (no audio files) definitions */

#define AFNH_ENV	"$AF_NOHEADER"
#define AFFTYPE_ENV	"$AF_FILETYPE"

#define AO_LIM_UNDEF	LONG_MIN	/* Undefined Limits */

struct AO_dformat {
  int Format;
  int NbS;
};
struct AO_FIpar {
  char Fname[FILENAME_MAX];
  double Gain;
  long int Lim[2];
  int Ftype;
  struct AF_NHpar NHpar;
};
struct AO_FOpar {
  char Fname[FILENAME_MAX];
  long int Nframe;
  int Ftype;
  double Sfreq;
  struct AO_dformat DFormat;
  unsigned char *SpkrConfig;
  const char *Info;
};

/* Initialization macros */
#define FIpar_INIT(fi)	{ \
	if (AFsetNHpar (AFNH_ENV)) \
	  UTwarn ("%s - %s \"%s\"", PROGRAM, AOM_BadEnv, &(AFNH_ENV)[1]); \
	if (AFsetFileType (AFFTYPE_ENV)) \
	  UTwarn ("%s - %s \"%s\"", PROGRAM, AOM_BadEnv, &(AFFTYPE_ENV)[1]); \
	(fi)->Fname[0] = '\0'; \
	(fi)->Gain = 1.0; \
	(fi)->Lim[0] = 0L; \
	(fi)->Lim[1] = AO_LIM_UNDEF; \
	(fi)->Ftype = (AFoptions ())->FtypeI; \
	(fi)->NHpar = (AFoptions ())->NHpar; }

#define FOpar_INIT(fo) { \
	(fo)->Fname[0] = '\0'; \
	(fo)->Nframe = AF_NFRAME_UNDEF; \
	(fo)->Sfreq = 0.0; \
	(fo)->DFormat.Format = FD_UNDEF; \
	(fo)->DFormat.NbS = 0; \
	(fo)->Ftype = FTW_UNDEF; \
	(fo)->SpkrConfig = NULL; \
	(fo)->Info = NULL; }

/* Default output file type */
#if (SY_OS == SY_OS_UNIX)
#  define AO_FTYPEO_DEFAULT	FTW_AU
#else
#  define AO_FTYPEO_DEFAULT	FTW_WAVE
#endif

/* Default output data type */
#define AO_DFORMATO_DEFAULT	FD_INT16

/* Warning messages */
#define AOM_BadEnv	"Invalid value for environment variable"
#define AOM_DiffLim	"Input file sample limits differ, using maximum"
#define AOM_DiffNSamp	"Different numbers of samples, using maximum"
#define AOM_NotAllLim	"Not all sample limits defined"
#define AOM_NotAllNSamp	"Number of samples not defined for all files"

/* Error messages */
#define AOM_BadData	"Invalid data format specification"
#define AOM_BadFType	"Invalid file type specification"
#define AOM_BadGain	"Invalid gain value"
#define AOM_BadLimits	"Invalid limits specification"
#define AOM_BadNFrame	"Invalid number of samples"
#define AOM_BadNHPar	"Error setting headerless file parameters"
#define AOM_BadOption	"Option error, use -h for a list of options"
#define AOM_BadSFreq	"Invalid sampling frequency"
#define AOM_BadSpkr	"Invalid speaker configuration"
#define AOM_InvFTypeC	"Invalid output file type code"
#define AOM_InvNbS	"Invalid no. bits/sample"
#define AOM_stdin1	"Error, standard input can be used only once"
#define AOMF_DataFType	"%s: Invalid data format (%s) for %s"

#ifdef __cplusplus
extern "C" {
#endif

/* Prototypes */
struct AO_CmdArg *
AOArgs (void);
int
AOdecFI (struct AO_FIpar *FI);
int
AOdecFO (struct AO_FOpar *FO);
void
AOinitOpt (const int argc, const char *argv[]);
long int
AOnFrame (AFILE *AFp[], const struct AO_FIpar FI[], int Nifiles,
	  long int Nframe);
int
AOsetDformat (const struct AO_FOpar *FO, AFILE *AFp[], int Nf);
void
AOsetFIopt (const struct AO_FIpar *FI, int NsampND, int RAccess);
void
AOsetFOopt (const struct AO_FOpar *FO);
int
AOsetFormat (const struct AO_FOpar *FO, AFILE *AFp[], int Nf);
int
AOsetFtype (const struct AO_FOpar *FO);
void
AOstdin (const struct AO_FIpar FI[], int N);

#ifdef __cplusplus
}
#endif

#endif /* AO_h_ */
