/*------------ Telecommunications & Signal Processing Lab --------------
                         McGill University

Routine:
  ResampAudio.h

Description:
  Declarations for ResampAudio

Author / revision:
  P. Kabal  Copyright (C) 2005
  $Revision: 1.47 $  $Date: 2005/02/01 04:25:11 $

----------------------------------------------------------------------*/

#ifndef ResampAudio_h_
#define ResampAudio_h_

#define PROGRAM "ResampAudio"
#define VERSION	"v5r1  2005-01-31"

#include <float.h>		/* DBL_MAX */

#include <libtsp.h>		/* typedef for AFILE */
#include <libtsp/AFpar.h>	/* AF_NHpar */
#include <AO.h>			/* struct AO_FIpar, AO_FOpar */

#define AFPATH_ENV	"$AUDIOPATH"

#define MAXCOF		8001		/* Max number coeffs from a file */
#define MAXIR		24

#define RS_FIpar	AO_FIpar	/* Input file structure */
#define RS_FOpar	AO_FOpar	/* Output file structure */

#define IR_DEFAULT	0
#define DEL_DEFAULT	DBL_MAX
#define FC_DEFAULT	0.0
#define GAIN_DEFAULT	0.0
#define ALPHA_DEFAULT	(0.1102 * (80.0 - 8.7))	/* 80 dB */
#define NCOF_DEFAULT	0
#define WSPAN_DEFAULT	DBL_MAX
#define WOFFS_DEFAULT	DBL_MAX
#define BTN		0.15		/* Normalized transition width */

struct Fspec_T {
  char *FFile;		/* Input filter coefficient file name */
  char *WFile;		/* Output filter coefficient file name */
  int Ir;		/* Interpolation filter ratio */
  double Del;		/* Filter delay (samples) */
  double Fc;		/* Filter cutoff */
  double Gain;		/* Filter passband gain */
  double alpha;		/* Kaiser window parameter */
  int Ncof;		/* Number of filter coefficients */
  double Wspan;		/* Kaiser window span (samples) */
  double Woffs;		/* Kaier window offset (samples) */
};

struct Fpoly_T {
  double **hs;		/* Coefficients in subfilter order, hs[Ir+1][Ncmax] */
  int *offs;		/* Offset of first coefficient in each subfilter */
  int *Nc;		/* Number of coefficients in each subfilter */
  int Ncmax;		/* Maximum number of coefficients in any subfilter */
  int Ir;		/* Number of subfilters */
};
/*
  Note that there is an extra subfilter with the Ir'th one pointing to the
  zero'th one, but having a offset differing by 1.
*/

/* Time structure representing time t
   t = n + (dm + dmr) / M
*/
struct Tval_T {
  long int n;		/* Integer value */
  long int dm;		/* Integer value, 0 <= ds < M */
  double dmr;		/* Fractional value, 0 <= dsr < 1 */
  long int M;
};

#define Fspec_T_INIT(p) { \
	(p)->FFile = NULL; \
	(p)->WFile = NULL; \
	(p)->Ir = IR_DEFAULT; \
	(p)->Del = DEL_DEFAULT; \
	(p)->Fc = FC_DEFAULT; \
	(p)->Gain = GAIN_DEFAULT; \
	(p)->alpha = ALPHA_DEFAULT; \
	(p)->Ncof = NCOF_DEFAULT; \
	(p)->Wspan = WSPAN_DEFAULT; \
	(p)->Woffs = WOFFS_DEFAULT; }

/* Error messages */
#define RSM_BadAlign	"Invalid alignment offset"
#define RSM_BadAtten	"Invalid filter stopband attenuation"
#define RSM_BadDelay	"Invalid filter delay"
#define RSM_BadFCutoff	"Invalid filter cutoff frequency"
#define RSM_BadFGain	"Invalid filter gain"
#define RSM_BadFRatio	"Invalid filter ratio"
#define RSM_BadFilt	"Invalid filter type"
#define RSM_BadInterp	"Invalid filter interpolation factor"
#define RSM_BadKey	"Invalid keyword in filter specification"
#define RSM_BadNCoef	"Invalid number of coefficients"
#define RSM_BadNSamp	"Invalid number of samples"
#define RSM_BadRatio	"Invalid interpolation ratio"
#define RSM_BadWinOffs	"Invalid window offset value"
#define RSM_BadWinPar	"Invalid window parameter"
#define RSM_BadWinSpan	"Invalid window span value"
#define RSM_MFName	"Too few filenames specified"
#define RSM_NoCoef	"Empty coefficient file name"
#define RSM_NoDelay \
	"Filter delay must be specified for non-symmetric filters"
#define RSM_NoFName	"No coefficient file name"
#define RSM_NoFRatio	"Filter ratio must be specified"
#define RSM_NoSFreq	"Output sampling rate not specified"
#define RSM_OpenCErr	"Unable to open coefficient file"
#define RSM_SFreqRatio \
	"Both sampling frequency and interpolation ratio specified"
#define RSM_XFName	"Too many filenames specified"

/* Printout formats */
#define RSMF_CoefFile	" Coefficient file: %s\n"
#define RSMF_Ratio1	" Sampling ratio: %g\n"
#define RSMF_Ratio2	" Sampling ratio: %ld/%ld\n"
#define RSMF_IntFilt "\
 Interpolation filter:\n\
   ratio: %d, cutoff: %g, alpha: %g, gain = %g\n\
   delay: %g, no. coeffs: %d, offset: %g, span: %g\n"

/* Filter file formats */
#define RSMF_KWFile "\
!FIR - Kaiser windowed lowpass filter\n\
!  ratio: %d, cutoff: %g, alpha: %g, gain: %g\n\
!  delay: %g, offset: %g, span: %g\n"
#define RSMF_IntFile "\
!FIR - Interpolating filter\n\
!  ratio: %d, delay: %g\n"

/* Usage */
#define RSMF_Usage "\
Usage: %s [options] -f FiltFile AFileI AFileO\n\
Options:\n\
  -s SFREQ, --srate=SFREQ     Sampling frequency for the output file.\n\
  -i SRATIO, --interpolate=SRATIO  Sampling rate ratio.\n\
  -a OFFS, --alignment=OFFS   Offset for first output.\n\
  -f FPARMS, --filter_spec=FPARMS  Filter parameters,\n\
                              \"file=fname,ratio=Ir,delay=Del\" or\n\
                              \"ratio=Ir,cutoff=Fc,gain=g,atten=A,alpha=a,\n\
                              N=Ncof,span=Wspan,offset=Woffs,write=fname\".\n\
  -n NSAMPLE, --number_samples=NSAMPLE  Number of output samples.\n\
  -t FTYPE, --type=FTYPE      Input file type,\n\
                              \"auto\", \"AU\", \"WAVE\", \"AIFF\", \"noheader\",\n\
                              \"SPHERE\", \"ESPS\", \"IRCAM\", \"SPPACK\",\n\
                              \"INRS\", \"SPW\", \"NSP\", or \"text\".\n\
  -P PARMS, --parameters=PARMS  Parameters for headerless input files,\n\
                              \"Format,Start,Sfreq,Swapb,Nchan,ScaleF\".\n\
  -g GAIN, --gain=GAIN        Gain factor applied to the input file.\n\
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
RSKaiserLPF (double h[], int N, double Fc, double alpha, double Gain,
	     double Woffs, double Wspan);
double
RSKalphaXatt (double alpha);
double
RSKalphaXD (double alpha);
double
RSKattenXalpha (double atten);
void
RSexpTime (double t, long int M, struct Tval_T *T);
void
RSfiltSpec (const char String[], struct Fspec_T *Fspec);
void
RSincTime (struct Tval_T *T, double di);
void
RSintFilt (double Sratio, double Soffs, const struct Fspec_T *Fspec,
	   struct Fpoly_T *PF, double *FDel, FILE *fpinfo);
void
RSinterp (const double x[], int Nxm, double y[], int Ny, double Ds,
	  const struct Tval_T *T, const struct Fpoly_T *PF);
void
RSoptions (int argc, const char *argv[], struct RS_FIpar *FI, double *Soffs,
	   double *Sratio, struct Fspec_T *Fspec, struct RS_FOpar *FO);
void
RSratio (double Sratio, int Ir, long int *Ns, double *Ds, long int NsMax,
	 long int DsMax, FILE *fpinfo);
int
RSrefresh (AFILE *AFpI, long int offs, double x[], int Nx);
void
RSresamp (AFILE *AFpI, AFILE *AFpO, double Sratio, long int Nout,
	  double Soffs, const struct Fpoly_T *PF, FILE *fpinfo);
void
RSwriteCof (const char Fname[], const struct Fspec_T *Fs, const double h[]);

#ifdef __cplusplus
}
#endif

#endif /* ResampAudio_h_ */
