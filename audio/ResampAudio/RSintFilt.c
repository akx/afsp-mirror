/*------------- Telecommunications & Signal Processing Lab -------------
                           McGill University

Routine:
  void RSintFilt (double Sratio, double Soffs, const struct Fspec_T *Fspec,
                  struct Fpoly_T *PF, double *FDel, FILE *fpinfo)

Purpose:
  Read or design an interpolating filter

Description:
  This routine reads an interpolation filter or designs a Kaiser windowed
  interpolating filter.  The filter coefficients are stored as polyphase
  components in a rectangular array.  An extra polyphase component is created
  for ease of wrap-around in the interpolation process.  This extra component
  has the same coefficients as the first component but with an adjusted
  offset value.

Parameters:
   -> double Sratio
      Ratio of output sampling rate to input sampling rate
   -> double Soffs
      Offset of the first input sample
   -> const struct Fspec_T *Fspec
      Filter specification structure
  <-  struct Ffilt_T *Filt
      Filter coefficient structure
  <-  double *FDel
      Filter delay in units of input samples
   -> FILE *fpinfo
      File pointer for printing filter information.  If fpinfo is not NULL,
      information about the filter is printed on the stream selected by fpinfo.

Author / revision:
  P. Kabal  Copyright (C) 2005
  $Revision: 1.21 $  $Date: 2005/02/01 04:19:45 $

----------------------------------------------------------------------*/

#include <limits.h>		/* LONG_MAX */
#include <math.h>

#include <libtsp/FIpar.h>
#include "ResampAudio.h"

#define MAXV(a, b)	(((a) > (b)) ? (a) : (b))
#define CHECKSYM(x,N)	((int) (1.00001 * VRdCorSym(x,N)))

static void
RS_parFilt (double Sratio, double Soffs, const struct Fspec_T *Fspec,
	    struct Fspec_T *Fs);
static void
RS_readFilt (const char Fname[], const struct Fspec_T *Fspec,
	     double h[], int *Ncof, int *Ir, double *Del, FILE *fpinfo);
static void
RS_polyphase (const double h[], int Ncof, int Ir, struct Fpoly_T *PF);


void
RSintFilt (double Sratio, double Soffs, const struct Fspec_T *Fspec,
	   struct Fpoly_T *PF, double *FDel, FILE *fpinfo)

{
  int i, is;
  struct Fspec_T Fs;
  double *h;

  if (Fspec->FFile != NULL) {

    /* Read the filter coefficients */
    h = (double *) UTmalloc (MAXCOF * sizeof (double));
    RS_readFilt (Fspec->FFile, Fspec, h, &Fs.Ncof, &Fs.Ir, &Fs.Del, fpinfo);
    Fs.FFile = Fspec->FFile;
  }

  else {
    /* Set the filter parameters */
    RS_parFilt (Sratio, Soffs, Fspec, &Fs);
    Fs.FFile = NULL;

    /* Print the filter specs */
    if (fpinfo != NULL) {
      fprintf (fpinfo, "\n");
      fprintf (fpinfo, RSMF_IntFilt, Fs.Ir, Fs.Fc, Fs.alpha, Fs.Gain,
	       Fs.Del, Fs.Ncof, Fs.Woffs, Fs.Wspan);
    }

    /* Design the filter */
    h = (double *) UTmalloc (Fs.Ncof * sizeof (double));
    RSKaiserLPF (h, Fs.Ncof, Fs.Fc / Fs.Ir, Fs.alpha, Fs.Gain, Fs.Woffs,
		 Fs.Wspan);
  }

  /* Remove leading and trailing zero coefficients */
  for (i = 0; i < Fs.Ncof; ++i) {
    if (h[i] != 0.0)
      break;
  }
  is = i;
  for (i = Fs.Ncof-1; i >= is; --i) {
    if (h[i] != 0.0)
      break;
  }
  Fs.Ncof = i - is + 1;
  Fs.Del = Fs.Del - is;

  /* Write the filter to a file */
  if (Fspec->WFile != NULL)
    RSwriteCof (Fspec->WFile, &Fs, &h[is]);

  /* Rearrange the coefficients in polyphase form */
  RS_polyphase (&h[is], Fs.Ncof, Fs.Ir, PF);
  UTfree ((void *) h);

  *FDel = Fs.Del / Fs.Ir;		/* Filter delay in samples */

  return;
}

/* Read the filter coefficients from a file */


static void
RS_readFilt (const char Fname[], const struct Fspec_T *Fspec, double h[],
	     int *Ncof, int *Ir, double *Del, FILE *fpinfo)

{
  int FiltType, ncof, ir;
  double del;

  /* Read the coefficients */
  FiltType = FIdReadFilt (Fname, MAXCOF, h, &ncof, fpinfo);
  if (FiltType != FI_FIR)
    UThalt ("%s: %s", PROGRAM, RSM_BadFilt);

  ir = Fspec->Ir;
  if (ir == IR_DEFAULT)
    UThalt ("%s: %s", PROGRAM, RSM_NoFRatio);
  if (ir <= 0)
    UThalt ("%s: %s", PROGRAM, RSM_BadFRatio);

  del = Fspec->Del;
  if (del == DEL_DEFAULT) {
    if (CHECKSYM (h, ncof) != 1)
      UThalt ("%s: %s", PROGRAM, RSM_NoDelay);
    del = 0.5 * (ncof - 1);
  }

  *Ncof = ncof;
  *Ir = ir;
  *Del = del;

  return;
}

/* Fill in the default filter parameters */


static void
RS_parFilt (double Sratio, double Soffs, const struct Fspec_T *Fspec,
	    struct Fspec_T *Fs)

{
  int n, m;
  long int Nv, Dv;

  /* Choose the interpolating ratio, max MAXIR for the default design */
  Fs->Ir = Fspec->Ir;
  if (Fs->Ir == IR_DEFAULT) {
    MSratio (Sratio, &Nv, &Dv, 0.0, LONG_MAX, LONG_MAX);
    if (Nv <= MAXIR)
      Fs->Ir = (int) Nv;
    else
      Fs->Ir = MAXIR;
  }

  /* Choose the filter cutoff */
  /*
    The default cutoff will give zero crossings in the filter coefficients.
    Consider an input signal whose spectrum goes to zero at the half-sampling
    frequency.  With little energy below the half-sampling frequency, there
    will be little aliasing just above the half-sampling frequency.  Hence
    a low-pass response with its midpoint at the half-sampling frequency is
    a good compromise between excessive loss of signal components (below the
    cutoff frequency) and adequate suppression of aliasing terms (above the
    cutoff frequency).
  */
  Fs->Fc = Fspec->Fc;
  if (Fs->Fc == FC_DEFAULT) {
    if (Sratio >= 1.0)
      Fs->Fc = 0.5;
    else
      Fs->Fc = 0.5 * Sratio;
  }

  /* Choose the filter gain */
  Fs->Gain = Fspec->Gain;
  if (Fs->Gain == GAIN_DEFAULT)
    Fs->Gain = Fs->Ir;

  /* Save alpha */
  Fs->alpha = Fspec->alpha;

  /* Choose the number of coefficients */
  Fs->Ncof = Fspec->Ncof;
  if (Fs->Ncof == NCOF_DEFAULT) {
    /*
      D = (n-1) * Bt,
      where Bt is the normalized transition bandwidth BTN * Fc / Ir
      The value of Ncof is chosen to be of the form Ncof = 2*m*Ir + 1.
    */
    n = (int) ceil (Fs->Ir * RSKalphaXD (Fs->alpha) / (BTN * Fs->Fc)) + 1;
    m = MSiCeil (n - 1, 2 * Fs->Ir);
    Fs->Ncof = 2 * m * Fs->Ir + 1;
  }

  /* Choose the window span */
  Fs->Wspan = Fspec->Wspan;
  if (Fs->Wspan == WSPAN_DEFAULT)
    Fs->Wspan = Fs->Ncof - 1;

  /* Choose the window offset */
  /*
    Moving the window to compensate for fractional part of Soffs is useful if
    Ir is equal to Ns.  Then only ordinary interpolation is used (no need for
    linear interpolation).  This reduces the filtering effort by a factor of
    two.  For other cases, moving the window will align one of the subfilters
    with the first output sample.  However, in general no other output samples
    will benefit from this exact alignment, i.e. they will have to be bracketed
    and linear interpolation used.  Furthermore, even if the default cutoff is
    used, then shifting the window will cause the loss of the regular zero
    crossings in the filter coefficients.

    In practice, it is not common to specify Soffs to be non-zero or more
    generally to be a fractional shift with respect to the interpolation ratio.
  */
  Fs->Woffs = Fspec->Woffs;
  if (Fs->Woffs == WOFFS_DEFAULT)
    Fs->Woffs = Fs->Ir * Soffs - floor (Fs->Ir * Soffs);

  /* Set the filter delay */
  Fs->Del = 0.5 * Fs->Wspan - Fs->Woffs;

  /* Adjust the number of coefficients */
  /* If Woffs is non-zero and the number of coefficients is chosen by default,
     then sampling the window with an offset will push the last window value
     off the end of the window into the zero part.
  */
  if (Fs->Woffs != 0.0 && Fspec->Ncof == NCOF_DEFAULT)
    --Fs->Ncof;

  return;
}

/* Arrange the filter coefficients in polyphase form */

static void
RS_polyphase (const double h[], int Ncof, int Ir, struct Fpoly_T *PF)

{
  int i, j, m, mst, nc;
  int Ncmax;

  /* Allocate storage for the polyphase filter parameters */
  /* Include room for pointers to an extra subfilter */
  PF->offs = (int *) UTmalloc (2 * (Ir+1) * sizeof (int));
  PF->Nc = PF->offs + (Ir+1);

  /* For each sub-filter, find the first and last non-zero component */
  Ncmax = 0;
  for (i = 0; i < Ir; ++i) {
    nc = (Ncof - 1 - i + Ir) / Ir;	/* nc >= 0 for Ncof >= 0 */
    for (mst = 0, j = i; nc > 0; ++mst, --nc, j+=Ir) {
      if (h[j] != 0.0)
	break;
    }
    PF->offs[i] = mst;
    for (j += (nc-1)*Ir; nc > 0; --nc, j-=Ir) {
      if (h[j] != 0)
	break;
    }
    PF->Nc[i] = nc;
    Ncmax = MAXV (nc, Ncmax);
  }
  PF->Ncmax = Ncmax;
  PF->Ir = Ir;

  /* Allocate storage for the polyphase filters */
  /* Include room for pointers to an extra subfilter */
  /* (A deallocate via MAdFreeMat will free up the space properly) */
  PF->hs = (double **) MAdAllocMat (Ir, Ncmax);
  PF->hs = (double **) UTrealloc (PF->hs, (Ir+1) * sizeof (double **));

  /*
    Rearrange the filter coefficients into polyphase components
      h[i][m] = h[j],
    where
      m = floor(j/Ir), and i = j - Ir*m.
    Actually, only the non-zero span of subfilter i is stored.  Thus the first
    element of the array hs[i] corresponds to the filter element h[i][mo],
    where mo is an offset,
      hs[i][m] = h[i][m+mo] = h[j],
    where
      m = floor(j/Ir) - mo, and i = j - Ir*(m+mo). 
  */
  for (i = 0; i < Ir; ++i) {
    mst = PF->offs[i];
    nc = PF->Nc[i];
    for (m = 0, j = i + mst*Ir; m < nc ; ++m, j+=Ir)
      PF->hs[i][m] = h[j];
  }

  /* Set up the extra subfilter */
  PF->hs[Ir] = PF->hs[0];	/* Same coefficients */
  PF->Nc[Ir] = PF->Nc[0];	/* Same number of coefficients */
  PF->offs[Ir] = PF->offs[0]-1; /* Adjust the offset */

  return;
}
