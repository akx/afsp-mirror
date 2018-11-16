/*------------ Telecommunications & Signal Processing Lab --------------
                         McGill University

Routine:
  CopyAudio.h

Description:
  Declarations for CopyAudio

Author / revision:
  P. Kabal  Copyright (C) 2018
  $Revision: 1.95 $  $Date: 2018/11/16 23:37:16 $

----------------------------------------------------------------------*/

#ifndef CopyAudio_h_
#define CopyAudio_h_

#define PROGRAM "CopyAudio"
#define VERSION "v10r2  2018-11-16"

#include <libtsp.h>
#include <AO.h>

#define AFPATH_ENV  "$AUDIOPATH"

#define MAXNCI    26
#define MAXNCO    12
#define MAXIFILE  10
enum {
  M_COMB   = 0,  /* Combine mode */
  M_CONCAT = 1   /* Concatenate mode */
};

/* y(n,k) = SUM Gain[k][i] * x(n,i) + Offset[k]
             i
*/
struct CP_Chgain {
  int NCI;
  int NCO;
  double Offset[MAXNCO];
  double Gain[MAXNCO][MAXNCI];
};

#define Chgain_INIT(p) { \
  int i__; \
  (p)->NCI = 0; \
  (p)->NCO = 0; \
  VRdZero ((p)->Offset, MAXNCO); \
  for (i__ = 0; i__ < MAXNCO; ++i__) \
    VRdZero ((p)->Gain[i__], MAXNCI); }

/* Warning messages */
#define CPM_DiffSFreq "Input sampling frequencies differ"

/* Error messages */
#define CPM_BadChanEx  "Invalid channel expression"
#define CPM_BadLimits  "Invalid limits specification"
#define CPM_ConfNFrame "Sample limits and number of samples conflict"
#define CPM_DiffNChan  "Different numbers of input channels"
#define CPM_EmptyChan  "Empty channel expression"
#define CPM_LateFPar   "Input file parameter specified after input files"
#define CPM_MFName     "Too few filenames specified"
#define CPM_NoChanSpec "No specification for output channel"
#define CPM_NSampNChan "No. samples not a multiple of no. channels"
#define CPM_XFName     "Too many filenames specified"

/* Error message formats */
#define CPMF_MNChan "%s: Too few input channels, need %d input channels"

/* Usage */
#define CPMF_Usage "\
Usage: %s [options] AFileI1 AFileI2 ... AFileO\n\
Options:\n\
  -c, --combine               Combine samples from multiple input files.\n\
  -C, --concatenate           Concatenate samples from multiple input files.\n\
  -g GAIN, --gain=GAIN        Gain factor applied to input files.\n\
  -l L:U, --limits=L:U        Input file sample limits.\n\
  -t FTYPE, --type=FTYPE      Input file type,\n\
                              \"auto\", \"AIFF\", \"AU\", \"WAVE\", \"text-audio\"\n\
                              \"noheader\", \"IRCAM\", \"SPHERE\", \"ESPS\", \"INRS\",\n\
                              \"SPPACK\", \"SPW\", \"NSP\".\n\
  -P PARMS, --parameters=PARMS  Parameters for input files,\n\
                              \"Format,Start,Sfreq,Swapb,Nchan,FullScale\".\n\
  -s SFREQ, --srate=SFREQ     Sampling frequency for the output file.\n\
  -n NSAMPLE, --number-samples=NSAMPLE  Number of output samples.\n\
  -cA CGAINS, --chanA=CGAINS  Scaling factors for output channel A.\n\
   ...                        ...\n\
  -cL CGAINS, --chanL=CGAINS  Scaling factors for output channel L.\n\
  -F FTYPE, --file-type=FTYPE  Output file type,\n\
                              \"AU\", \"WAVE\", \"WAVE-NOEX\", \"AIFF\",\n\
                              \"AIFF-C\", \"AIFF-C/sowt\", \"noheader\",\n\
                              \"noheader-swap\", \"text-audio\".\n\
  -D DFORMAT, --data-format=DFORMAT  Data format for the output file,\n\
                              \"mu-law8\", \"mu-lawR8\", \"A-law8\", \"unsigned8\",\n\
                              \"integer8\", \"integer16\", \"integer24\",\n\
                              \"float32\", \"float64\", \"text16\", \"text\".\n\
  -S SPEAKERS, --speakers=SPEAKERS  Loudspeaker positions,\n\
                              \"FL\", \"FR\", ... \n\
  -I INFO, --info=INFO        Add a header information record.\n\
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
CPcopy (int Mode, AFILE *AFp[], const struct AO_FIpar FI[], int Nifiles,
        const struct CP_Chgain *Chgain, long int Nframe, AFILE *AFpO);
long int
CPcopyChan (AFILE *AFp[], const long int StartF[], int Nifiles,
            long int Nframe, long int MaxNframe, AFILE *AFpO);
void
CPdecChan (const char String[], int Ichan, struct CP_Chgain *Chgain);
long int
CPlim (int Mode, AFILE *AFp[], const struct AO_FIpar FI[], int Nifiles,
       long int Nframe);
void
CPoptions (int argc, const char *argv[], int *Mode,
           struct AO_FIpar FI[MAXIFILE], int *Nifiles, struct AO_FOpar *FO,
           struct CP_Chgain *Chgain);

#ifdef __cplusplus
}
#endif

#endif /* CopyAudio_h_ */
