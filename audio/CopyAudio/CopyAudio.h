/*------------ Telecommunications & Signal Processing Lab --------------
                         McGill University

Routine:
  CopyAudio.h

Description:
  Declarations for CopyAudio

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.79 $  $Date: 2003/05/13 01:32:38 $

----------------------------------------------------------------------*/

#ifndef CopyAudio_h_
#define CopyAudio_h_

#define PROGRAM "CopyAudio"
#define VERSION	"v6r0  2003-05-08"

#include <libtsp.h>		/* typedef for AFILE */
#include <libtsp/AFpar.h>	/* struct AF_NHpar */
#include <AO.h>			/* struct AO_FIpar, AO_FOpar */

#define AFPATH_ENV	"$AUDIOPATH"

#define MAXNI		20
#define MAXNO		12
#define MAXIFILE	10
#define MAXFILE		(MAXIFILE+1)
#define M_COMB		0		/* Combine mode */
#define M_CONCAT	1		/* Concatenate mode */

#define CP_LIM_UNDEF	AO_LIM_UNDEF	/* Undefined Limits */
#define CP_FIpar	AO_FIpar	/* Input file structure */
#define CP_FOpar	AO_FOpar	/* Output file structure */

/* y(n,k) = SUM Gain[k][i] * x(n,i) + Offset[k] 
             i
*/
struct CP_Chgain {
  int NI;
  int NO;
  double Offset[MAXNO];
  double Gain[MAXNO][MAXNI];
};  

#define Chgain_INIT(p) { \
	int i__; \
	(p)->NI = 0; \
	(p)->NO = 0; \
	VRdZero ((p)->Offset, MAXNO); \
	for (i__ = 0; i__ < MAXNO; ++i__) \
	  VRdZero ((p)->Gain[i__], MAXNI); }

/* Warning messages */
#define CPM_DiffSFreq	"Input sampling frequencies differ"

/* Error messages */
#define CPM_BadChanEx	"Invalid channel expression"
#define CPM_DiffNChan	"Different numbers of input channels"
#define CPM_EmptyChan	"Empty channel expression"
#define CPM_ConfNFrame	"Sample limits and number of samples conflict"
#define CPM_LateFPar	"Input file parameter specified after input files"
#define CPM_MFName	"Too few filenames specified"
#define CPM_NoChanSpec	"No specification for output channel"
#define CPM_NSampNChan	"No. samples not a multiple of no. channels"
#define CPM_XFName	"Too many filenames specified"

/* Error message formats */
#define CPMF_MNChan	"%s: Too few input channels, need %d input channels"

/* Usage */
#define CPMF_Usage "\
Usage: %s [options] AFileI1 AFileI2 ... AFileO\n\
Options:\n\
  -c, --combine               Combine samples from multiple input files.\n\
  -C, --concatenate           Concatenate samples from multiple input files.\n\
  -g GAIN, --gain=GAIN        Gain factor applied to input files.\n\
  -l L:U, --limits=L:U        Input file sample limits.\n\
  -t FTYPE, --type=FTYPE      Input file type,\n\
                              \"auto\", \"AU\", \"WAVE\", \"AIFF\", \"noheader\",\n\
                              \"SPHERE\", \"ESPS\", \"IRCAM\", \"SPPACK\",\n\
                              \"INRS\", \"SPW\", \"NSP\", or \"text\".\n\
  -P PARMS, --parameters=PARMS  Parameters for headerless input files,\n\
                              \"Format,Start,Sfreq,Swapb,Nchan,ScaleF\".\n\
  -s SFREQ, --srate=SFREQ     Sampling frequency for the output file.\n\
  -n NSAMPLE, --number_samples=NSAMPLE  Number of output samples.\n\
  -cA CGAINS, --chanA=CGAINS  Scaling factors for output channel A.\n\
   ...                        ...\n\
  -cL CGAINS, --chanL=CGAINS  Scaling factors for output channel L.\n\
  -F FTYPE, --file_type=FTYPE  Output file type,\n\
                              \"AU\", \"WAVE\", \"AIFF\", \"AIFF-C\",\n\
                              \"noheader\", or \"noheader_swap\".\n\
  -D DFORMAT, --data_format=DFORMAT  Data format for the output file,\n\
                              \"mu-law\", \"A-law\", \"unsigned8\", \"integer8\",\n\
                              \"integer16\", \"integer24\", \"float32\",\n\
                              \"float64\", or \"text\".\n\
  -S SPEAKERS, --speakers=SPEAKERS  Loudspeaker positions,\n\
                              \"FL\", \"FR\", \"FC\", \"LF\", \"BL\", \"BR\", \"FLC\",\n\
                              \"FRC\", \"BC\", \"SL\", \"SR\", \"TC\", \"TFL\", \"TFC\",\n\
                              \"TFR\", \"TBL\", \"TBC\", \"TBR\", \"-\"\n\
  -I INFO, --info=INFO        Header information string.\n\
  -h, --help                  Print a list of options and exit.\n\
  -v, --version               Print the version number and exit."


#ifdef __cplusplus
extern "C" {
#endif

/* Prototypes */
long int
CPcombChan (AFILE *AFp[], const long int StartF[], int Nifiles,
	    long int Nframe, const struct CP_Chgain *Chgain,
	    long int MaxNframe, AFILE *AFpO);
void
CPcopy (int Mode, AFILE *AFp[], const struct CP_FIpar FI[], int Nifiles,
	const struct CP_Chgain *Chgain, long int Nframe, AFILE *AFpO);
long int
CPcopyChan (AFILE *AFp[], const long int StartF[], int Nifiles,
	    long int Nframe, long int MaxNframe, AFILE *AFpO);
void
CPdecChan (const char String[], int Ichan, struct CP_Chgain *Chgain);
long int
CPlim (int Mode, AFILE *AFp[], const struct CP_FIpar FI[], int Nifiles,
       long int Nframe);
void
CPoptions (int argc, const char *argv[], int *Mode,
	   struct CP_FIpar FI[MAXIFILE], int *Nifiles, struct CP_FOpar *FO,
	   struct CP_Chgain *Chgain);

#ifdef __cplusplus
}
#endif

#endif /* CopyAudio_h_ */
