/*------------ Telecommunications & Signal Processing Lab --------------
                         McGill University

Routine:
  PQevalAudio.h

Description:
  Declarations for PQevalAudio

Author / revision:
  P. Kabal  Copyright (C) 2009
  $Revision: 1.29 $  $Date: 2009/03/23 15:49:15 $

----------------------------------------------------------------------*/

#ifndef PQevalAudio_h_
#define PQevalAudio_h_

#define PROGRAM "PQevalAudio"
#define VERSION	"v3r0  2009-03-14"

#include <libtsp.h>		/* typedef for AFILE */
#include <libtsp/AFpar.h>	/* struct AF_NHpar */
#include <AO.h>			/* struct AO_FIpar */

#define AFPATH_ENV	"$AUDIOPATH"

#define PQ_FIpar	AO_FIpar	/* Input file structure */

#define PQ_FS		48000.	/* Sampling frequency (double) */
#define PQ_NF		2048	/* Frame length */
#define PQ_CB_ADV	(PQ_NF/2)  /* Frame advance */
#define PQ_FB_ADV	192	/* Sub-sampling factor in filter bank */
#define PQ_FCS		1019.5	/* Test sine frequency */
#define PQ_AMAX		32767.	/* Max. input level corresponding to
				   to the max. listening level */
#define PQ_NL		256	/* Number of correlation lags (EHS MOV) */

#define PQ_BASIC	0
#define PQ_ADVANCED	1
#define PQ_CBMODEL	0
#define PQ_FBMODEL	1

#define PQ_NC_B		109
#define PQ_NC_CB_A	55
#define PQ_NC_FB_A	40
#define PQ_MAXNC	PQ_NC_B	/* Maximum number of channels */

#define PQ_NMOV_B	11
#define PQ_NMOV_A	5
#define PQ_NMOV_MAX	11

/* PEAQ parameter tables */
struct Par_FFT {
  double hw[PQ_NF];		/* Hamming window (including all scaling) */
};
struct Par_CB {
  double W2[PQ_NF/2+1];		/* Middle and outer ear weighting (energy) */
  int Nc;			/* Number of bands */
  double dz;			/* Band width in Bark */
  const double *fc;		/* Centre frequencies */
  int kl[PQ_MAXNC];		/* Lower band edges (DFT bin number) */
  int ku[PQ_MAXNC];		/* Upper band edges (DFT bin number) */
  double Ul[PQ_MAXNC];		/* Fractional bins at the lower band edges */
  double Uu[PQ_MAXNC];		/* Fractional bins at the upper band edges */
  double Bs[PQ_MAXNC];		/* Spreading function normalization */
  double EIN[PQ_MAXNC];		/* Internal noise */
};
struct Par_TDS {
  int Nc;			/* Number of bands */
  double a[PQ_MAXNC];		/* Time domain spreading coefficients */
  double b[PQ_MAXNC];		/* b[k] = 1 - a[k] */
};  
struct Par_Patt {
  int Nc;			/* Number of bands */
  int M1;			/* Lower frequency averaging limit */
  int M2;			/* Upper frequency averaging limit */
  double a[PQ_MAXNC];		/* Smoothing coefficients */
  double b[PQ_MAXNC];		/* b[k] = 1 - a[k] */
};  
struct Par_Loud {
  int Nc;			/* Number of bands */
  double s[PQ_MAXNC];		/* Excitation index */
  double Et[PQ_MAXNC];		/* Energy threshold */
  double Ets[PQ_MAXNC];		/* Pre-computed multiplicative term */
};
struct Par_MPatt {
  int Nc;			/* Number of bands */
  double a[PQ_MAXNC];		/* Smoothing coefficients */
  double b[PQ_MAXNC];		/* b[k] = 1 - a[k] */
};
struct Par_MDiff {
  int Nc;			/* Number of bands */
  double a[PQ_MAXNC];		/* Time domain spreading, coefficients */
  double b[PQ_MAXNC];		/* b[k] = 1 - a[k] */
  double Ete[PQ_MAXNC];		/* Energy threshold (0.3 power) */
};
struct Par_NLoud {
  int Nc;			/* Number of bands */
  double Et[PQ_MAXNC];		/* Energy threshold */
};
struct Par_BW {
  int N;			/* Number of frequency points */
  int kx;			/* Upper limit (DFT bin, 21586 Hz) */
  int kl;			/* Lower limit (DFT bin, 8109 Hz) */
  double FR;			/* Threshold factor for the reference signal */
  double FT;			/* Threshold factor for the test signal */
};
struct Par_NMR {
  int Nc;			/* Number of bands */
  double gm[PQ_MAXNC];		/* Masking offset */
};
struct Par_EHS {
  double EnThr;			/* Energy threshold (half frame) */
  int kst;			/* Starting lag */
  int NL;			/* Number of correlation lags */
  int M;			/* Number of terms in a correlation sum */
  double Hw[PQ_NL];		/* Scaled Hann window */
};
struct Par_NNet {
  int I;			/* Number of inputs */
  int J;			/* Number of hidden layer nodes */
  const double *amin;		/* MOV scaling factors */
  const double *amax;
  const double *wx[PQ_NMOV_MAX]; /* Neural net weights */
  const double *wxb;
  const double *wy;
  double wyb;
  double bmin;			/* DI scaling factors */
  double bmax;
  int ClipMOV;			/* MOV clipping flag */
};

/* Structure for parameter tables */
struct PQ_Par {
  struct Par_FFT   FFT;
  struct Par_CB    CB;
  struct Par_TDS   TDS;
  struct Par_Patt  Patt;
  struct Par_Loud  Loud;
  struct Par_MPatt MPatt;
  struct Par_MDiff MDiff;
  struct Par_NLoud NLoud;
  struct Par_BW	   BW;
  struct Par_NMR   NMR;
  struct Par_EHS   EHS;
  struct Par_NNet  NNet;
};

/* Filter memory arrays */
struct Mem_TDS {
  double *x[2];
};
struct Mem_Adap {
  double *P[2];
  double *PC[2];
  double *Rn;
  double *Rd;
};
struct Mem_Env {
  double *DE[2];
  double *Ese[2];
  double *Eavg[2];
};

/* Structure for filter memories */
struct PQ_FiltMem {
  struct Mem_TDS  TDS;
  struct Mem_Adap Adap;
  struct Mem_Env  Env;
};

/* Instantaneous MOV precursors */
struct MOVB_Loud {
  double NRef;
  double NTest;
};
struct MOVB_MDiff {
  double Mt1B;
  double Mt2B;
  double Wt;
};
struct MOVB_NLoud {
  double NL;
};
struct MOVB_BW {
  double BWRef;
  double BWTest;
};
struct MOVB_NMR {
  double NMRavg;
  double NMRmax;
};
struct MOVB_PD {
  double p[PQ_MAXNC];
  double q[PQ_MAXNC];
};
struct MOVB_EHS {
  double EHS;
};

/* Structure with MOV precursors for one time instant */
struct PQ_MOVBI {
  struct MOVB_Loud  LoudV;
  struct MOVB_MDiff MDiffV;
  struct MOVB_NLoud NLoudV;
  struct MOVB_BW    BWV;
  struct MOVB_NMR   NMRV;
  struct MOVB_PD    PDV;
  struct MOVB_EHS   EHSV;
};

/* Structures with time history of MOV precursors */
struct MOVC_Loud {
  double *NRef[2];
  double *NTest[2];
};
struct MOVC_MDiff {
  double *Mt1B[2];
  double *Mt2B[2];
  double *Wt[2];
};
struct MOVC_NLoud {
  double *NL[2];
};
struct MOVC_BW {
  double *BWRef[2];
  double *BWTest[2];
};
struct MOVC_NMR {
  double *NMRavg[2];
  double *NMRmax[2];
};
struct MOVC_PD {
  double c1;
  double *Pc;
  double *Qc;
};
struct MOVC_EHS {
  double *EHS[2];
};

/* Structure with MOV precursors for each frame */
struct PQ_MOVBC {
  struct MOVC_Loud  Loud;
  struct MOVC_MDiff MDiff;
  struct MOVC_NLoud NLoud;
  struct MOVC_BW    BW;
  struct MOVC_NMR   NMR;
  struct MOVC_PD    PD;
  struct MOVC_EHS   EHS;
};

/* PEAQ options */
struct PQ_Opt {
  double Lp;			/* Maximum listening level (dB SPL) */
  int Ni;			/* Print every NI'th PEAQ frame */
  int ClipMOV;			/* MOV clipping flag */
  double PCinit;		/* Initial value for pattern corrections */
  double PDfactor;		/* Forgetting factor for prob. detection */
  int OverlapDelay;		/* Overlap warmup frames and delay */
  int DataBounds;		/* Find data boundaries */
  int EndMin;			/* Min. number of samples in last PEAQ frame */
};

#define LP_DEFAULT		92.
#define NI_DEFAULT		-50   /* -50 means 50 printouts */
#define CLIP_MOV_DEFAULT	0
#define PC_INIT_DEFAULT		0.
#define PD_FACTOR_DEFAULT	1.
#define OVERLAP_DELAY_DEFAULT	1
#define DATA_BOUNDS_DEFAULT	1
#define END_MIN_DEFAULT		(PQ_NF/2)

#define OPTIONS_INIT(p) { \
	(p)->Lp = LP_DEFAULT; \
	(p)->Ni = NI_DEFAULT; \
	(p)->ClipMOV = CLIP_MOV_DEFAULT; \
	(p)->PCinit = PC_INIT_DEFAULT; \
	(p)->PDfactor = PD_FACTOR_DEFAULT; \
        (p)->OverlapDelay = OVERLAP_DELAY_DEFAULT; \
        (p)->DataBounds = DATA_BOUNDS_DEFAULT; \
	(p)->EndMin = END_MIN_DEFAULT; }

/* Error messages */
#define PQM_BadEndMin	"Invald end_min value"
#define PQM_BadInfo	"Invalid info specification"
#define PQM_BadKey	"Invalid option keyword"
#define PQM_BadPCinit	"Invalid PC_init value"
#define PQM_BadPDfactor	"Invalid PD factor"
#define PQM_BadLevel	"Invalid level specification"
#define PQM_DiffNChan	"Numbers of channels differ"
#define PQM_DiffSFreq	"Sampling frequencies differ"
#define PQM_InvSFreq	"Invalid sampling frequency, must be 48000"
#define PQM_LateFPar	"Input file parameter specified after input files"
#define PQM_NoFName	"No filenames specified"
#define PQM_XFName	"Too many filenames specified"
#define PQM_XNChan 	"Too many channels"

/* Usage */
#define PQMF_Usage "\
Usage: %s [options] AFileR AFileT\n\
Options:\n\
  -g GAIN, --gain=GAIN        Gain factor to the input files.\n\
  -l L:U, --limits=L:U        Input frame limits.\n\
  -L LEVEL, --levelSPL LEVEL  Level for a max amplitude sine (dB SPL).\n\
  -i NI, --info=NI            Print frame information every NI'th frame.\n\
  -o OPTIONS, --options=OPTIONS Processing options,\n\
                              \"clip_MOV\", \"no_clip_MOV\",\n\
                              \"PC_init=V\", \"PD_factor=V\",\n\
                              \"overlap_delay\", \"no_overlap_delay\",\n\
                              \"data_bounds\", \"no_data_bounds\",\n\
                              \"end_min=N\".\n\
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
PQDFTFrame (const double x[], const double hw[], double X2[], int NF);
void
PQWOME (const double f[], double W2[], int N);
void
PQadapt (const double *Ehs[2], const struct Par_Patt *Patt,
	 double *EP[2], struct Mem_Adap *Adap);
void
PQavgBW (const struct MOVC_BW *BW, int Nchan, int Np,
	 double *BandwidthRefB, double *BandwidthTestB);
double
PQavgEHS (const struct MOVC_EHS *EHS, int Nchan, int Np);
void
PQavgModDiffB (int Ndel, const struct MOVC_MDiff *MDiff, int Nchan, int Np,
	       double *WinModDiff1B, double *AvgModDiff1B,
	       double *AvgModDiff2B);
void
PQavgMOVB (const struct PQ_MOVBC *MOVC, int Nchan, int Nwup, int Np,
	   double MOV[]);
double
PQavgNLoud (int ist, const struct MOVC_NLoud *NLoud, int Nchan, int Np);
void
PQavgNMRB (const struct MOVC_NMR *NMR, int Nchan, int Np, double *TotalNMRB,
	   double *RelDistFramesB);
void
PQavgPD (const struct MOVC_PD *PD, int Np, double *ADBB, double *MFPDB);
void
PQchanPD (int Nc, int Nchan, const struct PQ_MOVBI MOVBI[],
	  double *Pc, double *Qc);
void
PQdataBoundary (AFILE *AFp, long int Nchan, long int StartS, long int Ns,
		long int Lim[2]);
struct PQ_MOVBI
PQeval (const double xR[], const double xT[], const struct PQ_Par *PQpar,
	struct PQ_FiltMem *Fmem);
void
PQenThresh (const double f[], double Et[], int N);
void
PQexIndex (const double f[], double s[], int N);
void
PQframeMOVB (const struct PQ_MOVBI MOVBI[], int Nchan, int i,
	     struct PQ_MOVBC *MOVC);
void
PQgenTables (int Version, const struct PQ_Opt *PQopt, struct PQ_Par *PQpar);
void
PQinitFMem (int Nc, double PCinit, struct PQ_FiltMem *Fmem);
void
PQintNoise (const double f[], double EIN[], int N);
void
PQgroupCB (const double F[], const struct Par_CB *CB, double Pe[]);
double
PQloud (const double Ehs[], const struct Par_Loud *Loud);
int
PQloudTest (const struct MOVC_Loud *Loud, int Nchan, int Np);
struct MOVB_BW
PQmovBW (const double *X2[], const struct Par_BW *BW);
double
PQmovEHS (const double xR[], const double xT[], const double *X2[],
	  const struct Par_EHS *EHS);
struct MOVB_MDiff
PQmovModDiffB (const double *M[2], const double ERavg[],
	       const struct Par_MDiff *MDiff);
double
PQmovNLoudB (const double *M[2], const double *EP[2],
	     const struct Par_NLoud *NLoud);
double
PQmovNMRA (const double EbN[], const double Ehs[], const struct Par_NMR *NMR);
struct MOVB_NMR
PQmovNMRB (const double EbN[], const double Ehs[], const struct Par_NMR *NMR);
struct MOVB_PD
PQmovPD (const double *Ehs[]);
void
PQmodPatt (const double *Es[2], const struct Par_MPatt *MPatt,
	   double *M[2], double ERavg[], struct Mem_Env *Env);
double
PQnNet (const double MOV[], const struct Par_NNet *NNet, double *DIp);
void
PQoptions (int argc, const char *argv[], struct PQ_Opt *PQopt,
	   struct PQ_FIpar FI[2]);
void
PQprtMOV (const double MOV[], int N, double DI, double ODG);
void
PQprtMOVCi (int Nchan, int i, const struct PQ_MOVBC *MOVC);
int
PQreadChan (AFILE *AFp, long int Nchan, long int offs, double *x[],
	    int Nreq);
void
PQspreadCB (const double E[], const struct Par_CB *CB, double Es[]);
void
PQtConst (double t100, double tmin, double Fs, const double f[],
	  double a[], double b[], int N);
void
PQtimeSpread (const double Es[], const struct Par_TDS *TDS, double Ehs[],
	      double Ef[]);

#ifdef __cplusplus
}
#endif

#endif /* PQevalAudio_h_ */
