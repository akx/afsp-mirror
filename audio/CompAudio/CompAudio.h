/*------------ Telecommunications & Signal Processing Lab --------------
                         McGill University

Routine:
  CompAudio.h

Description:
  Declarations for CompAudio

Author / revision:
  P. Kabal  Copyright (C) 2004
  $Revision: 1.63 $  $Date: 2004/06/25 00:15:59 $

----------------------------------------------------------------------*/

#ifndef CompAudio_h_
#define CompAudio_h_

#define PROGRAM "CompAudio"
#define VERSION	"v5r1  2003-07-11"

#include <float.h>		/* DBL_MAX */

#include <libtsp.h>		/* typedef for AFILE */
#include <libtsp/AFpar.h>	/* struct AF_NHpar */
#include <AO.h>			/* struct AO_FIpar */

#define AFPATH_ENV	"$AUDIOPATH"
#define SA_SFREQ_L	500		/* Lowest Sfreq for speech */
#define SA_SFREQ_U	500000		/* Highest Sfreq for speech */

#define CA_FIpar	AO_FIpar	/* Input file structure */

struct Stats_F {
  long int N;		/* Number of samples */
  double Sx;		/* Sum x[i] */
  double Sx2;		/* Sum x[i]*x[i] */
  double Vmin;		/* Min value */
  double Vmax;		/* Max value */
  long int Novload;	/* Number of overloads */
  long int Nrun;	/* Number of overload runs */
  long int Nanomal;	/* Number of anomalous transitions */
  int Region;		/* Sample value region (state variable) */
  double ActLev;	/* Active level */
};

struct Stats_T {
  double Sx2;		/* Sum x[i]*x[i] */
  double Sy2;		/* Sum y[i]*y[i] */
  double Sxy;		/* Sum x[i]*y[i] */
  long int Ndiff;	/* Number of different samples */
  double Diffmax;	/* Maximum (absolute) difference */
  long int Nrun;	/* Number of difference runs */
  int Inrun;		/* In difference run flag */
  long int Nsseg;	/* Number of samples in a segment */
  long int Nseg;	/* Number of segments */
  double SNRlog;	/* Segmental SNR (log10 value) */
};

/* Warning messages */
#define CAM_DiffSFreq	"Sampling frequencies differ"
#define CAM_XNChan 	"Too many channels; treating as single channel data"
#define CAMF_SegSize	"%s - Segment size set to %ld samples"

/* Error messages */
#define CAM_BadDelay	"Invalid delay range specification"
#define CAM_BadSegLen	"Invalid segment length"
#define CAM_DiffNChan	"Numbers of channels differ"
#define CAM_LateFPar	"Input file parameter specified after input files"
#define CAM_NoFName	"No filenames specified"
#define CAM_XFName	"Too many filenames specified"

/* Printout formats */
#define CAMF_ActLevel	"    Active Level: %.5g (%.4g%%), Activity Factor: %.1f%%\n"
#define CAMF_AeqB	" File A = File B\n"
#define CAMF_AeqBdelay	" File A = File B  (delay = %ld)\n"
#define CAMF_AxB	" File A = %.5g * File B\n"
#define CAMF_BestMatch	" Best match at delay = %ld\n"
#define CAMF_Channel	"  Channel %d:\n"
#define CAMF_Delay	" Delay = %ld\n"
#define CAMF_DelayAxB	" Delay: %3ld,  File A = %.5g * File B\n"
#define CAMF_DelaySNR \
	" Delay: %3ld,  SNR = %-9.5g dB  (Gain for File B = %.5g)\n"
#define CAMF_DiffAP1	" Max Diff = %.5g (%.4g%%),  No. Diff = %ld (1 run)\n"
#define CAMF_DiffAP2	" Max Diff = %.5g (%.4g%%),  No. Diff = %ld (%ld runs)\n"
#define CAMF_DiffP1	" Max Diff = %.4g%%,  No. Diff = %ld (1 run)\n"
#define CAMF_DiffP2	" Max Diff = %.4g%%,  No. Diff = %ld (%ld runs)\n"
#define CAMF_FileA	" File A:\n"
#define CAMF_FileB	" File B:\n"
#define CAMF_MaxMin	"    Maximum = %.5g (%.4g%%),  Minimum = %.5g (%.4g%%)\n"
#define CAMF_NumAnom	"    No. Anomalous Transitions = %ld\n"
#define CAMF_NumOvld1	"    No. Overloads = %ld (1 run)\n"
#define CAMF_NumOvldN	"    No. Overloads = %ld (%ld runs)\n"
#define CAMF_NumSamp	"    Number of Samples: %ld\n"
#define CAMF_SDevMean	"    Std Dev = %.5g (%.4g%%),  Mean = %.5g (%.4g%%)\n"
#define CAMF_SegSNR	" Seg. SNR = %-8.5g dB  (%ld sample segments)\n"
#define CAMF_SNR	" SNR      = %-8.5g dB\n"
#define CAMF_SNRwGain	" SNR      = %-8.5g dB  (Gain for File B = %.5g)\n"

/* Usage */
#define CAMF_Usage "\
Usage: %s [options] AFileA [AFileB]\n\
Options:\n\
  -d DL:DU, --delay=DL:DU     Delay range.\n\
  -s SAMP, --segment=SAMP     Segment length (samples).\n\
  -g GAIN, --gain=GAIN        Gain factor applied to input files.\n\
  -l L:U, --limits=L:U        Input file sample limits.\n\
  -t FTYPE, --type=FTYPE      Input file type,\n\
                              \"auto\", \"AU\", \"WAVE\", \"AIFF\", \"noheader\",\n\
                              \"SPHERE\", \"ESPS\", \"IRCAM\", \"SPPACK\",\n\
                              \"INRS\", \"SPW\", \"NSP\", or \"text\".\n\
  -P PARMS, --parameters=PARMS  Parameters for headerless input files,\n\
                              \"Format,Start,Sfreq,Swapb,Nchan,ScaleF\".\n\
  -h, --help                  Print this message and exit.\n\
  -v, --version               Print the version number and exit."


#ifdef __cplusplus
extern "C" {
#endif

/* Prototypes */
void
CASNR (const struct Stats_T *Stats, double *SNR, double *SNRG, double *SF,
       double *SSNR);
void
CAcomp (AFILE *AFp[2], long int Start[2], long int Nsamp, long int Nsseg,
	long int delayL, long int delayU, long int *delayM,
	struct Stats_T *Stats);
void
CAcorr (AFILE *AFp[2], long int Start[2], long int Nsamp, long int delay,
	long int Nsseg, struct Stats_T *Stats);
void
CAoptions (int argc, const char *argv[], long int *delayL, long int *delayU,
	   long int *Nsseg, struct CA_FIpar FI[2]);
void
CAprcorr (const struct Stats_T *Stats, const double ScaleF[2]);
void
CAprstat (const struct Stats_F Stats[], long int Nchan, double ScaleF);
void
CAstats (AFILE *AFp, long int Start, long int Nsamp, struct Stats_F Stats[]);

#ifdef __cplusplus
}
#endif

#endif /* CompAudio_h_ */
