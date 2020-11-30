/*------------ Telecommunications & Signal Processing Lab --------------
                         McGill University

Routine:
  CompAudio.h

Description:
  Declarations for CompAudio

Author / revision:
  P. Kabal  Copyright (C) 2020
  $Revision: 1.84 $  $Date: 2020/11/30 12:31:25 $

----------------------------------------------------------------------*/

#ifndef CompAudio_h_
#define CompAudio_h_

#define PROGRAM "CompAudio"
#define VERSION "v10r3  2020-11-30"

#include <float.h>    /* DBL_MAX */

#include <libtsp.h>
#include <AO.h>

#define AFPATH_ENV  "$AUDIOPATH"
#define SA_SFREQ_L  2000      /* Lowest Sfreq for speech */
#define SA_SFREQ_U  200000    /* Highest Sfreq for speech */
#define CA_MaxNchan 64        /* Largest number of channels for statistics of
                                 each channel (greater than one) */
#define SN_SEGTIME  16E-3     /* Segmental SNR, segment length (seconds),
                                 corresponding to 128 samples  at 8 kHz*/
#define SN_NSEG_MIN 64        /* Segmental SNR, min segment length */
#define SN_NSEG_MAX 768       /* Segmental SNR, max segment length */

struct Stats_F {
  long int N;       /* Number of samples */
  long double Sx;   /* Sum x[i] */
  long double Sxx;  /* Sum x[i]*x[i] */
  double Vmin;      /* Min value */
  double Vmax;      /* Max value */
  long int Novload; /* Number of overloads */
  long int Nrun;    /* Number of overload runs */
  long int Nanomal; /* Number of anomalous transitions */
  int Region;       /* Sample value region (state variable) */
  double ActLev;    /* Active level */
};

struct Stats_X {
  long double Sxx;  /* Sum x[i]*x[i] */
  long double Syy;  /* Sum y[i]*y[i] */
  long double Sxy;  /* Sum x[i]*y[i] */
  long double See;  /* Sum (x[i]-y[i])^2 */
  long int Delay;   /* Delay for best match */
  long int Ndiff;   /* Number of different samples */
  double Diffmax;   /* Maximum (absolute) difference */
  long int Nrun;    /* Number of difference runs */
  int Inrun;        /* In difference run flag */
  long int Nsseg;   /* Number of samples in a segment */
  long int Nseg;    /* Number of segments */
  double SNRlog;    /* Segmental SNR (log10 value) */
};

struct SNR_X {
  double SNR;       /* Conventional SNR */
  double SNRG;      /* Gain-optimized SNR */
  double SF;        /* Optimal gain for SNRG */
  double SSNR;      /* Segmental SNR */
  long int Nsseg;   /* Number of samples in a segment */
};

/* Warning messages */
#define CAM_DiffSFreq "Sampling frequencies differ"
#define CAM_XNChan    "Too many channels; treating as single channel data"
#define CAMF_SegSize  "%s - Segment size set to %ld samples"

/* Error messages */
#define CAM_BadDelay  "Invalid delay range specification"
#define CAM_BadLimits "Invalid limits specification"
#define CAM_BadSegLen "Invalid segment length"
#define CAM_DiffNChan "Numbers of channels differ"
#define CAM_LateFPar  "Input file parameter specified after input files"
#define CAM_NoFName   "No filenames specified"
#define CAM_XFName    "Too many filenames specified"

/* Printout formats */
#define CAMF_ActLevelP \
  "    Active Level: %.4g%%, Activity Factor: %.1f%%\n"
#define CAMF_ActLevelV \
  "    Active Level: %.5g (%.4g%%), Activity Factor: %.1f%%\n"
#define CAMF_AeqB      " File A = File B\n"
#define CAMF_AeqBdelay " File A = File B (delay = %ld)\n"
#define CAMF_AxB       " File A = %.5g * File B\n"
#define CAMF_BestMatch " Best match at delay = %ld\n"
#define CAMF_Channel   "  Channel %d:\n"
#define CAMF_Delay     " Delay = %ld\n"
#define CAMF_DelayAxB  " Delay: %3ld,  File A = %.4g * File B\n"
#define CAMF_DelaySNR \
  " Delay:%4ld,  SNR = %-8.3gdB  (File B Gain = %.3g)\n"
#define CAMF_DiffV     " Max Diff = %.5g (%.4g%%),  No. Diff = %ld\n"
#define CAMF_DiffV1    " Max Diff = %.5g (%.4g%%),  No. Diff = %ld (1 run)\n"
#define CAMF_DiffVN    " Max Diff = %.5g (%.4g%%),  No. Diff = %ld (%ld runs)\n"
#define CAMF_DiffP     " Max Diff = %.4g%%,  No. Diff = %ld\n"
#define CAMF_DiffP1    " Max Diff = %.4g%%,  No. Diff = %ld (1 run)\n"
#define CAMF_DiffPN    " Max Diff = %.4g%%,  No. Diff = %ld (%ld runs)\n"
#define CAMF_FileA     " File A:\n"
#define CAMF_FileB     " File B:\n"
#define CAMF_MaxMinP   "    Maximum = %.4g%%,  Minimum = %.4g%%\n"
#define CAMF_MaxMinV   "    Maximum = %.5g (%.4g%%),  Minimum = %.5g (%.4g%%)\n"
#define CAMF_NumAnom   "    No. Anomalous Transitions = %ld\n"
#define CAMF_NumOvld   "    No. Overloads = %ld\n"
#define CAMF_NumOvld1  "    No. Overloads = %ld (1 run)\n"
#define CAMF_NumOvldN  "    No. Overloads = %ld (%ld runs)\n"
#define CAMF_NumSamp   "    Number of Samples: %ld\n"
#define CAMF_SDevMeanP "    Std Dev = %.4g%%,  Mean = %.4g%%\n"
#define CAMF_SDevMeanV "    Std Dev = %.5g (%.4g%%),  Mean = %.5g (%.4g%%)\n"
#define CAMF_SegSNR    " Seg. SNR = %-8.4gdB  (%ld sample segments)\n"
#define CAMF_SNR       " SNR      = %-8.4gdB\n"
#define CAMF_SNRwGain  " SNR      = %-8.4gdB  (File B Gain = %.3g)\n"

/* Usage */
#define CAMF_Usage "\
Usage: %s [options] AFileA [AFileB]\n\
Options:\n\
  -d DL:DU, --delay=DL:DU     Delay range.\n\
  -g GAIN, --gain=GAIN        Gain factor applied to input files.\n\
  -l L:U, --limits=L:U        Input file sample limits.\n\
  -t FTYPE, --type=FTYPE      Input file type,\n\
                              \"auto\", \"AIFF\", \"AU\", \"WAVE\", \"text-audio\"\n\
                              \"noheader\", \"IRCAM\", \"SPHERE\", \"ESPS\", \"INRS\",\n\
                              \"SPPACK\", \"SPW\", \"NSP\".\n\
  -P PARMS, --parameters=PARMS  Parameters for input files,\n\
                              \"Format,Start,Sfreq,Swapb,Nchan,FullScale\".\n\
  -h, --help                  Print this message and exit.\n\
  -v, --version               Print the version number and exit."

#ifdef __cplusplus
extern "C" {
#endif

/* Prototypes */
struct SNR_X
CASNR(const struct Stats_X *StatsX);
struct Stats_X
CAcomp(AFILE *AFp[2], const long int Start[2], long int Nsamp,
       const long int Delay[2]);
struct Stats_X
CAcorr(AFILE *AFp[2], const long int Start[2], long int Nsamp, long int delay,
       long int Nsseg);
void
CAoptions(int argc, const char *argv[], long int Delay[2],
          struct AO_FIpar FI[2]);
void
CAprcorr(struct Stats_X *StatsX, const long int Delay[2],
         const double ScaleF[2]);
void
CAprstat(const struct Stats_F StatsF[], long int Nch, double ScaleF);
void
CAstats(AFILE *AFp, long int Start, long int Nsamp, long int Nc,
        struct Stats_F StatsF[]);

#ifdef __cplusplus
}
#endif

#endif /* CompAudio_h_ */
