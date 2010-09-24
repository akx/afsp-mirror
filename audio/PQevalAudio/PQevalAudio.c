/*------------- Telecommunications & Signal Processing Lab -------------
                           McGill University

Routine:
  PQevalAudio [options] AFileR AFileT

Purpose:
  Evaluate the perceptual quality degradation for audio files

Description:
  This program takes a reference audio file and a test audio file and measures
  the perceptual degradation of the test signal with respect to the reference
  signal. The measurement is based on the ITU-R BS.1387 (PEAQ) standard. The
  output combines a number of model output variables (MOV's) into a single
  measure, the Objective Difference Grade which is an impairment scale with the
  following meanings.
     0 imperceptible
    -1 perceptible but not annoying
    -2 slightly annoying
    -3 annoying
    -4 very annoying

  The measurement procedure has only been calibrated for a sampling rate of
  48 kHz. The measurement procedure works for monaural or binaural (stereo)
  signals. Use ResampAudio to resample audio files with other sampling rates
  to 48 kHz. The measurement procedure also assumes that the files have been
  time and gain aligned. Use CompAudio to determine the required gain and
  delay compensation and CopyAudio to actually modify the gain and time-align
  the files.

  This program implements the basic version of PEAQ. The measurement results
  on a standard database do not fall within the tight bounds specified in the
  standard (see the report cited below for a discussion of the difficulties
  in interpreting the standard). However, the results are close enough to be
  useful for quality impairment measurements.

  References:
  ITU-R Recommendation BS.1387, "Method for Objective Measurements of Perceived
  Audio Quality", Dec. 1998 (and subsequent corrections).

  T. Thiede et al, "PEAQ - The ITU Standard for Objective Measurement of
  Perceived Audio Quality", J. Audio Eng. Soc., vol. 48, pp. 3-29, Jan.-Feb.
  2000.

  P. Kabal, "An Examination and Interpretation of ITU-R BS.1387: Perceptual  Copyright (C) 2009
  Evaluation of Audio Quality", TSP Lab Technical Report, Dept. Electrical &
  Computer Engineering, McGill University, May 2002.
  (http://www.TSP.ECE.McGill.CA/MMSP/Documents)

Options:
  Input file(s): AFileR AFileT:
      The environment variable AUDIOPATH specifies a list of directories to be
      searched for the input audio file(s).  Specifying "-" as the input file
      indicates that input is from standard input.
  -g GAIN, --gain=GAIN
      A gain factor applied to the data from the input files.  This gain
      applies to all channels in a file.  The gain value can be given as a
      real number (e.g., "0.003") or as a ratio (e.g., "1/256"). This option
      may be given more than once.  Each invocation applies to the input files
      that follow the option.
  -l L:U, --limits=L:U
      Sample limits for the input files (numbered from zero).  Each invocation
      applies to the input files that follow the option.  The specification
      ":" means the entire file; "L:" means from sample L to the end; ":U"
      means from sample 0 to sample U; "N" means from sample 0 to sample N-1.
  -L LEVEL --levelSPL=LEVEL
      Listening level (in dB SPL) for a maximum amplitude sine (default 92 dB
      SPL)
  -i NI --info==NI
      Print information for each NI'th PEAQ frame. The default is chosen to
      print PEAQ frame information for at most 50 frames. Set NI to zero to
      suppress the printout of intermediate information.
  -o OPTIONS --options=OPTIONS
      Processing options.
        "clip_MOV" or "no_clip_MOV" - Clip MOV values (default "no_clip_MOV")
        "PC_init=V" - Initial value for the pattern correction factors
	  (default 0)
        "PD_factor=V" - Forgetting factor for the maximum probability of
           detection calculation (default 1).
        "overlap_delay" or "no_overlap_delay" - Overlap warmup frames (frames
          before the data boundary) and the delay for calculating modulation
	  difference and noise loudness values (default "overlap_delay")
        "data_bounds" or "no_data_bounds" - Ignore frames with small data
          values at the beginning and end of the reference (default
	  "data_bounds")
        "end_min=N" - Stop processing at the frame that contains at least
	  N samples (default value of N is 1024). The "-l" command line option
	  can be used to shift the entire processing. For instance "-l -1024:"
	  will add half a frame of zero padding before the start of data.
  -t FTYPE, --type=FTYPE
      Input audio file type.  In the default automatic mode, the input file
      type is determined from the file header.  For input from a non-random
      access file (e.g. a pipe), the input file type can be explicitly
      specified to avoid the lookahead used to read the file header.  This
      option can be specified more than once.  Each invocation applies to the
      input files that follow the option.  See the description of the
      environment variable AF_FILETYPE below for a list of file types.
  -P PARMS, --parameters=PARMS
      Parameters to be used for headerless input files.  This option may be
      given more than once.  Each invocation applies to the input files that
      follow the option.  See the description of the environment variable
      AF_NOHEADER below for the format of the parameter specification.
  -h, --help
      Print a list of options and exit.
  -v, --version
      Print the version number and exit.

Environment variables:
  AF_FILETYPE:
  This environment variable defines the input audio file type.  In the default
  mode, the input file type is determined from the file header.
    "auto"       - determine the input file type from the file header
    "AU" or "au" - AU audio file
    "WAVE" or "wave" - WAVE file
    "AIFF" or "aiff" - AIFF or AIFF-C sound file
    "noheader"   - headerless (non-standard or no header) audio file
    "SPHERE"     - NIST SPHERE audio file
    "ESPS"       - ESPS sampled data feature file
    "IRCAM"      - IRCAM soundfile
    "SPPACK"     - SPPACK file
    "INRS"       - INRS-Telecom audio file
    "SPW"        - Comdisco SPW Signal file
    "CSL" or "NSP" - CSL NSP file
    "text"       - Text audio file

  AF_NOHEADER:
  This environment variable defines the data format for headerless or
  non-standard input audio files.  The string consists of a list of parameters
  separated by commas.  The form of the list is
    "Format, Start, Sfreq, Swapb, Nchan, ScaleF"
  Format: File data format
       "undefined" - Headerless files will be rejected
       "mu-law8"   - 8-bit mu-law data
       "A-law8"    - 8-bit A-law data
       "unsigned8" - offset-binary 8-bit integer data
       "integer8"  - two's-complement 8-bit integer data
       "integer16" - two's-complement 16-bit integer data
       "integer24" - two's-complement 24-bit integer data
       "integer32" - two's-complement 32-bit integer data
       "float32"   - 32-bit floating-point data
       "float64"   - 64-bit floating-point data
       "text"      - text data
  Start: byte offset to the start of data (integer value)
  Sfreq: sampling frequency in Hz (floating point number)
  Swapb: Data byte swap parameter
       "native"        - no byte swapping
       "little-endian" - file data is in little-endian byte order
       "big-endian"    - file data is in big-endian byte order
       "swap"          - swap the data bytes as the data is read
  Nchan: number of channels
    The data consists of interleaved samples from Nchan channels
  ScaleF: Scale factor
       "default" - Scale factor chosen appropriate to the type of data. The
          scaling factors shown below are applied to the data in the file.
          8-bit mu-law:    1/32768
          8-bit A-law:     1/32768
          8-bit integer:   128/32768
          16-bit integer:  1/32768
          24-bit integer:  1/(256*32768)
          32-bit integer:  1/(65536*32768)
          float data:      1
       "<number or ratio>" - Specify the scale factor to be applied to
          the data from the file.
  The default values for the audio file parameters correspond to the following
  string.
    "undefined, 0, 8000., native, 1, default"

  AUDIOPATH:
  This environment variable specifies a list of directories to be searched when
  opening the input audio files.  Directories in the list are separated by
  colons (semicolons for Windows).

Author / version:
  P. Kabal / v3r0  2009-03-14  Copyright (C) 2009

----------------------------------------------------------------------*/

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>	/* EXIT_SUCCESS */

#include <libtsp.h>
#include <libtsp/AFpar.h>
#include <AO.h>
#include "PQevalAudio.h"

#define ABSV(x)		(((x) < 0) ? -(x) : (x))
#define ICEILV(n,m)	(((n) + ((m) - 1)) / (m))	/* int n,m >= 0 */

static struct PQ_MOVBC *
PQ_InitMOVC (int Nchan, int Np);
static void
PQ_Bounds (AFILE *AFp[2], const struct PQ_FIpar FI[2], int Nchan,
	   long int StartS[2], const struct PQ_Opt *PQopt,
	   int *Fstart, int *Fend);


int
main (int argc, const char *argv[])

{
  struct PQ_FIpar FI[2];
  AFILE *AFp[2];
  long int Nchan[2], Nsamp[2], StartS[2];
  int  Fstart, Fend, Np, Nwup, is, i, j;
  int NsampND, RAccess, Nch;
  double Sfreq[2];
  double DI, ODG;
  struct PQ_Par PQpar;
  struct PQ_FiltMem Fmem[2];
  struct PQ_MOVBI MOVBI[2];
  double **xR;
  double **xT;
  struct PQ_MOVBC *MOVC;
  struct PQ_Opt PQopt;
  double MOV[PQ_NMOV_MAX];

/*  Get the input parameters */
  PQoptions (argc, argv, &PQopt, FI);

/* Input file access options */
  NsampND = 0;		/* Always return the number of samples */
  RAccess = 1;		/* Input must be random access */

/* Open the input files */
  for (i = 0; i < 2; ++i) {
    AOsetFIopt (&FI[i], NsampND, RAccess);
    FLpathList (FI[i].Fname, AFPATH_ENV, FI[i].Fname);
    AFp[i] = AFopnRead (FI[i].Fname, &Nsamp[i], &Nchan[i], &Sfreq[i], stdout);
    AFp[i]->ScaleF *= 32768.;
    AFp[i]->ScaleF *= FI[i].Gain;  /* Gain absorbed into the scaling factor */
  }

  /* Number of channels */
  if (Nchan[0] != Nchan[1])
    UThalt ("%s: %s", PROGRAM, PQM_DiffNChan);
  if (Nchan[0] > 2)
    UThalt ("%s: %s", PROGRAM, PQM_XNChan);
  Nch = (int) Nchan[0];

/* Sampling frequency */
  if (Sfreq[0] != Sfreq[1])
    UThalt ("%s: %s", PROGRAM, PQM_DiffSFreq);
  if (Sfreq[0] != PQ_FS)
    UThalt ("%s: %s", PROGRAM, PQM_InvSFreq);

  /* Data boundaries */
  PQ_Bounds (AFp, FI, Nch, StartS, &PQopt, &Fstart, &Fend);

  /* Number of PEAQ frames */
  Np = Fend - Fstart + 1;
  if (PQopt.Ni < 0)
    PQopt.Ni = ICEILV (Np, ABSV(PQopt.Ni));

  /* Initialize the MOV structure */
  MOVC = PQ_InitMOVC (Nch, Np);
  MOVC->PD.c1 = PQopt.PDfactor;

  /* Fill in the parameter tables */
  PQgenTables (PQ_BASIC, &PQopt, &PQpar);
  for (j = 0; j < Nch; ++j)
    PQinitFMem (PQpar.CB.Nc, PQopt.PCinit, &Fmem[j]);

  /* Set up the data buffer */
  xR = MAdAllocMat (Nch, PQ_NF);
  xT = MAdAllocMat (Nch, PQ_NF);
 
  is = 0;
  for (i = -Fstart; i < Np; ++i) {

    /* Read a frame of data */
    PQreadChan (AFp[0], Nch, StartS[0] + is, xR, PQ_NF);   /* Reference file */
    PQreadChan (AFp[1], Nch, StartS[1] + is, xT, PQ_NF);   /* Test file */
    is = is + PQ_CB_ADV;

    /* Process a frame */
    for (j = 0; j < Nch; ++j)
      MOVBI[j] = PQeval (xR[j], xT[j], &PQpar, &Fmem[j]);

    if (i >= 0) {
      /* Move the MOV precursors into a new structure */
      PQframeMOVB (MOVBI, Nch, i, MOVC);

      /* Print the MOV precursors */
      if (PQopt.Ni != 0 && i % PQopt.Ni == 0)
	PQprtMOVCi (Nch, i, MOVC);
    }
  }

  /* Time average of the MOV values */
  if (PQopt.OverlapDelay)
    Nwup = Fstart;
  else
    Nwup = 0;
  PQavgMOVB (MOVC, Nch, Nwup, Np, MOV);

  /* Neural net */
  ODG = PQnNet (MOV, &PQpar.NNet, &DI);

  /* Summary printout */
  PQprtMOV (MOV, PQ_NMOV_B, DI, ODG);

/* Clean up */
  MAdFreeMat (xR);
  MAdFreeMat (xT);

  for (i = 0; i < 2; ++i)
    AFclose (AFp[i]);

  return EXIT_SUCCESS;
}

static struct PQ_MOVBC *
PQ_InitMOVC (int Nchan, int Np)

{
  int Ni, NC, N, j;
  double *x;
  double *xp;
  static struct PQ_MOVBC MOVC;

  Ni = 2 * Np;
  NC = 11 * Np;
  if (Nchan == 1)
    N = Ni + NC;
  else
    N = Ni + 2 * NC;
  x = (double *) UTmalloc (N * sizeof (double));

  xp = x;
  MOVC.PD.Pc = xp;
  xp += Np;
  MOVC.PD.Qc = xp;
  xp += Np;

  for (j = 0; j < Nchan; ++j) {
    MOVC.Loud.NRef[j] = xp;
    xp += Np;
    MOVC.Loud.NTest[j] = xp;
    xp += Np;
    MOVC.MDiff.Mt1B[j] = xp;
    xp += Np;
    MOVC.MDiff.Mt2B[j] = xp;
    xp += Np;
    MOVC.MDiff.Wt[j] = xp;
    xp += Np;
    MOVC.NLoud.NL[j] = xp;
    xp += Np;
    MOVC.BW.BWRef[j] = xp;
    xp += Np;
    MOVC.BW.BWTest[j] = xp;
    xp += Np;
    MOVC.NMR.NMRavg[j] = xp;
    xp += Np;
    MOVC.NMR.NMRmax[j] = xp;
    xp += Np;
    MOVC.EHS.EHS[j] = xp;
    xp += Np;
  }

  assert (xp - x == N);

  return &MOVC;
}


#define MAXV(a, b)	(((a) > (b)) ? (a) : (b))


static void
PQ_Bounds (AFILE *AFp[2], const struct PQ_FIpar FI[2], int Nchan,
	   long int StartS[2], const struct PQ_Opt *PQopt,
	   int *Fstart, int *Fend)

{
  long int Ns, Lim[2];

  /* Resolve the file limits */
  Ns = AOnFrame (AFp, FI, 2, AF_NFRAME_UNDEF);
  assert (Ns != AF_NFRAME_UNDEF);

  /* Determine the actual samples to be processed */
  StartS[0] = FI[0].Lim[0];
  StartS[1] = FI[1].Lim[0];

  /* Data boundaries (determined from the reference file) */
  if (PQopt->DataBounds) {
    PQdataBoundary (AFp[0], Nchan, StartS[0], Ns, Lim);
    printf ("PEAQ Data Boundaries: %ld (%.3f s) - %ld (%.3f s)\n",
	    Lim[0], Lim[0]/AFp[0]->Sfreq, Lim[1], Lim[1]/AFp[0]->Sfreq);
  }
  else {
    Lim[0] = StartS[0];
    Lim[1] = Lim[0] + Ns - 1;
  }

  /* Start frame number */
  assert (Lim[0] >= StartS[0]);
  *Fstart = (int) ((Lim[0] - StartS[0]) / PQ_CB_ADV);

  /* End frame number */
  assert (PQopt->EndMin >= 0 && PQopt->EndMin <= PQ_NF);
  *Fend = MAXV (*Fstart,
	      (int) ((Lim[1] - StartS[0] + 1 - PQopt->EndMin) / PQ_CB_ADV));

  return;
}
