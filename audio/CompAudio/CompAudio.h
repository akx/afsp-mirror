/*------------ Telecommunications & Signal Processing Lab --------------
                         McGill University

Routine:
  CompAudio.h

Description:
  Declarations for CompAudio

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.57 $  $Date: 2003/04/29 23:13:12 $

----------------------------------------------------------------------*/

#ifndef CompAudio_h_
#define CompAudio_h_

#define PROGRAM "CompAudio"
#define VERSION	"v4r3  2003-04-28"

#include <float.h>		/* FLT_MAX */

#include <libtsp.h>		/* typedef for AFILE */
#include <libtsp/AFpar.h>	/* struct AF_NHpar */
#include <AO.h>			/* struct AO_FIpar */

#define AFPATH_ENV	"$AUDIOPATH"

#define CA_FIpar	AO_FIpar	/* Input file structure */

#define NTHR	15	/* Number of levels for speech activity level */

struct SpAct_P {
  float c[NTHR];	/* Threshold values */
  float g;		/* Envelope filter parameter */
  long int I;		/* Hangover limit */
};

struct SpAct_S {
  long int a[NTHR];	/* Threshold counts */
  long int h[NTHR];	/* Hangover counts */
  float p;		/* Envelope intermediate filter state */
  float q;		/* Envelope value (state variable) */
};

struct Stats_F {
  long int N;		/* Number of samples */
  double Sx;		/* Sum x[i] */
  double Sx2;		/* Sum x[i]*x[i] */
  float Vmin;		/* Min value */
  float Vmax;		/* Max value */
  long int Novload;	/* Number of overloads */
  long int Nrun;	/* Number of overload runs */
  long int Nanomal;	/* Number of anomalous transitions */
  int Inrange;		/* state variable */
  double ActLev;	/* Active level */
};

struct Stats_T {
  double Sx2;		/* Sum x[i]*x[i] */
  double Sy2;		/* Sum y[i]*y[i] */
  double Sxy;		/* Sum x[i]*y[i] */
  long int Ndiff;	/* Number of different samples */
  float Diffmax;	/* Maximum (absolute) difference */
  long int Nrun;	/* Number of difference runs */
  int Inrun;		/* In difference run flag */
  long int Nsseg;	/* Number of samples in a segment */
  long int Nseg;	/* Number of segments */
  double SNRlog;	/* Segmental SNR (log10 value) */
  double Ssx2;		/* Sum x[i]*x[i] for a segment */
  double Ssd2;		/* Sum (x[i]-y[i])(x[i]-y[i]) for a segment */
  int ks;		/* Segment sample counter */
};

#define Stats_F_INIT(p) { \
	(p)->N = 0L; \
	(p)->Sx = 0.0; \
	(p)->Sx2 = 0.0; \
	(p)->Vmin = FLT_MAX; \
	(p)->Vmax = -FLT_MAX; \
	(p)->Novload = 0L; \
	(p)->Nrun = 0L; \
	(p)->Nanomal = 0L; \
	(p)->Inrange = 0; \
	(p)->ActLev = 0.0; }

#define Stats_T_INIT(p, Nsseg) { \
	(p)->Sx2 = 0.0; \
	(p)->Sy2 = 0.0; \
	(p)->Sxy = 0.0; \
	(p)->Ndiff = 0L; \
	(p)->Diffmax = 0.0; \
	(p)->Nrun = 0L; \
	(p)->Inrun = 0; \
	(p)->Nsseg = Nsseg; \
	(p)->Nseg = 0L; \
	(p)->SNRlog = 0.0; \
	(p)->Ssx2 = 0.0; \
	(p)->Ssd2 = 1e-2; \
	(p)->ks = 0; }

/* Warning messages */
#define CAM_DiffSFreq	"Sampling frequencies differ"
#define CAM_XNChan 	"Too many channels; treating as single channel data"
#define CAMF_SegSize	"%s - Segment size set to %d samples"

/* Error messages */
#define CAM_BadDelay	"Invalid delay range specification"
#define CAM_BadSegLen	"Invalid segment length"
#define CAM_DiffNChan	"Numbers of channels differ"
#define CAM_LateFPar	"Input file parameter specified after input files"
#define CAM_NoFName	"No filenames specified"
#define CAM_XFName	"Too many filenames specified"

/* Printout formats */
#define CAMF_ActLevel	"    Active Level: %.1f (Activity Factor: %.1f%%)\n"
#define CAMF_AeqB	" File A = File B\n"
#define CAMF_AeqBdelay	" File A = File B  (delay = %d)\n"
#define CAMF_AxB	" File A = %.5g * File B\n"
#define CAMF_BestMatch	" Best match at delay = %d\n"
#define CAMF_Channel	"  Channel %d:\n"
#define CAMF_Delay	" Delay = %d\n"
#define CAMF_DelayAxB	" Delay: %3d,  File A = %.5g * File B\n"
#define CAMF_DelaySNR \
	" Delay: %3d,  SNR = %-9.5g dB  (Gain for File B = %.5g)\n"
#define CAMF_FileA	" File A:\n"
#define CAMF_FileB	" File B:\n"
#define CAMF_NumAnom	"    No. Anomalous Transitions = %ld\n"
#define CAMF_NumDiff1	" No. differing samples = %ld (1 run)\n"
#define CAMF_NumDiffN	" No. differing samples = %ld (%ld runs)\n"
#define CAMF_NumOvld1	"    No. Overloads = %ld (1 run)\n"
#define CAMF_NumOvldN	"    No. Overloads = %ld (%ld runs)\n"
#define CAMF_NumSamp	"    Number of Samples: %ld\n"
#define CAMF_MaxDiff	" Max. difference = %.5g\n"
#define CAMF_MaxMin	"    Max     = %-11.5g Min  = %.5g\n"
#define CAMF_MaxMinX	"    Max     = ***         Min  = ***\n"
#define CAMF_SDevMean	"    Std Dev = %-11.5g Mean = %.5g\n"
#define CAMF_SDevMeanX	"    Std Dev = ***         Mean = ***\n"
#define CAMF_SegSNR	" Seg. SNR = %-8.5g dB (%ld sample segments)\n"
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
	int delayL, int delayU, int *delayM, struct Stats_T *Stats);
void
CAcorr (AFILE *AFp[2], long int Start[2], long int Nsamp, int delay,
	long int Nsseg, struct Stats_T *Stats);
void
CAoptions (int argc, const char *argv[], int *delayL, int *delayU,
	   long int *Nsseg, struct CA_FIpar FI[2]);
void
CAprcorr (const struct Stats_T *Stats);
void
CAprstat (const struct Stats_F Stats[], int Nchan);
void
CAstats (AFILE *AFp, long int Start, long int Nsamp, struct Stats_F Stats[],
	 int Nchan);

#ifdef __cplusplus
}
#endif

#endif /* CompAudio_h_ */
