/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  AFILE *AFsetRead (FILE *fp, int Ftype, const struct AF_read *AFr,
                    enum AF_FIX_T Fix)

Purpose:
  Set up the parameters in an audio file structure for input files

Description:
  This routine checks the parameters in structure AFr and puts them into a new
  audio file structure.
  1. The data length (Ldata) and number of samples (Nsamp) parameters are
     checked for consistency and modified if necessary.
  2. The scale factor for scaling input file data is determined.
  3. The sampling frequency is set (check for a non-integer value in an
     information record).
  4. Fill in the bits/sample field (check for a value in an information record).
  5. Fill in the loudspeaker configuration (check in an information record).

  The file must be positioned at the start of the audio data.

Parameters:
  <-  AFILE *AFsetRead
      Audio file pointer for the audio file.  This routine allocates the
      space for this structure.  If an error is detected, a NULL pointer is
      returned.
   -> int Ftype
      File type: FT_NH, FT_AU, FT_WAVE, etc.
   -> const struct AF_read *AFr
      Structure with data and file parameters from the file header
   -> enum AF_FIX_T Fix
      Fix flag, sum of the following subcodes
         AF_FIX_NSAMP_HIGH - Reduce Nsamp if it larger than the data space
                             available
         AF_FIX_NSAMP_LOW  - Increase Nsamp if it is smaller than the data space
                             available
         AF_FIX_LDATA_HIGH - Reduce Ldata if it is larger than the data space
                             available

Author / revision:
  P. Kabal  Copyright (C) 2017
  $Revision: 1.71 $  $Date: 2017/07/03 22:25:30 $

-------------------------------------------------------------------------*/

#include <libtsp/sysOS.h>
#if (SY_OS == SY_OS_WINDOWS)
#  define _CRT_SECURE_NO_WARNINGS     /* Allow strcpy */
#endif

#include <assert.h>
#include <math.h>
#include <string.h>

#include <libtsp.h>
#include <AFpar.h>
#include <libtsp/nucleus.h>
#include <libtsp/AFinfo.h>
#include <libtsp/AFmsg.h>

#define ICEILV(n,m) (((n) + ((m) - 1)) / (m)) /* int n,m >= 0 */

/* Local functions */
static int
AF_NbSInfo (const struct AF_info *AFInfo, const struct AF_dformat *DFormat);
static struct AF_info
AF_setInfo (const struct AF_info *RInfo);
static unsigned char *
AF_setSpeaker (unsigned char *SpkrConfigIn, struct AF_info *AFInfo);
static double
AF_srateInfo (const struct AF_info *AFInfo, double Sfreq);


AFILE *
AFsetRead (FILE *fp, int Ftype, const struct AF_read *AFr, enum AF_FIX_T Fix)

{
  AFILE *AFp;
  long int Dstart;
  int Lw;
  struct AF_ndata NData;

  assert (Ftype > 0 && Ftype < AF_NFT);
  assert (AFr->DFormat.Format > 0 && AFr->DFormat.Format < AF_NFD);

/* Check data layout consistency */
  Lw = AF_DL[AFr->DFormat.Format];
  NData = AFr->NData;     /* Copy, now writeable */
  if (AFcheckDataPar (fp, Lw, &Dstart, &NData, Fix))
    return NULL;

/* Set the parameters for file access */
  AFp = (AFILE *) UTmalloc (sizeof (AFILE));

  /* File pointers and parameters */
  AFp->fp = fp;
  AFp->Op = FO_RO;
  if (Lw <= 1)
    AFp->Swapb = DS_NATIVE;
  else
    AFp->Swapb = UTswapCode (AFr->DFormat.Swapb);
  AFp->Start = Dstart;
  AFp->Isamp = 0;

  /* Error indicators */
  AFp->Error = AF_NOERR;
  AFp->Novld = 0;

  /* File data layout parameters */
  AFp->Ftype = Ftype;
  AFp->Format = AFr->DFormat.Format;
  AFp->Nsamp = NData.Nsamp;
  AFp->Nchan = NData.Nchan;
  if (AFp->Nsamp != AF_NSAMP_UNDEF)
    AFp->Nframe = AFp->Nsamp / AFp->Nchan;
  else
    AFp->Nframe = AF_NFRAME_UNDEF;

  /* File data parameters */
  /* Resolve the sampling frequency */
  AFp->Sfreq = AF_srateInfo (&AFr->RInfo, AFr->Sfreq);
  if (AFr->Sfreq == AF_SFREQ_UNDEF) {
    UTwarn ("AFsetRead: %s", AFM_UndSfreq);
    return NULL;
  }
  /* Set up the significant number of bits per sample field */
  AFp->NbS = AF_NbSInfo (&AFr->RInfo, &AFr->DFormat);
  AFp->FullScale = AFr->DFormat.FullScale;
  if (AFp->FullScale == AF_FULLSCALE_DEFAULT)
    AFp->FullScale = AF_FULLSCALE[AFp->Format];

  /* Program data parameters */
  AFp->ScaleV = AFopt.ScaleV;
  AFp->ScaleF = AFp->ScaleV / AFp->FullScale;

  /* Data descriptors */
  /* Set up the information record structure */
  AFp->AFInfo = AF_setInfo (&AFr->RInfo);
  AFp->ChunkInfo = AFr->ChunkInfo;
  /* Set up the loudspeaker configuration */
  AFp->SpkrConfig = AF_setSpeaker (NData.SpkrConfig, &AFp->AFInfo);

  return AFp;
}


/* -------------------- */
/* Fill in the AFsp Information structure */

/* This routine allocates the space for the information string.  It is expected
   that the calling routine takes care of freeing up the space.
*/


static struct AF_info
AF_setInfo (const struct AF_info *RInfo)

{
  int N;
  struct AF_info AFInfo;

  N = RInfo->N;
  if (N <= 0) {
    AFInfo.Info = NULL;
    AFInfo.N = 0;
    AFInfo.Nmax = 0;
  }
  else {
    AFInfo.Info = (char *) UTmalloc (N);
    memcpy (AFInfo.Info, RInfo->Info, N);
    AFInfo.N = N;
    AFInfo.Nmax = N;
  }

  return AFInfo;
}

/* Scan the header information string for the sampling frequency */

#define ABSV(x)   (((x) < 0) ? -(x) : (x))


static double
AF_srateInfo (const struct AF_info *AFInfo, double Sfreq)

{
  double Fv;

  /* Check "sample_rate:" record, if the sampling frequency is integer-valued */
  if (Sfreq == AF_SFREQ_UNDEF || Sfreq == floor(Sfreq)) {
    Fv = AFgetInfoSfreq (AFInfo);
    if (Fv != AF_SFREQ_UNDEF) {
      if (Sfreq == AF_SFREQ_UNDEF || ABSV(Fv - Sfreq) <= 0.5)
        Sfreq = Fv;
      else
        UTwarn ("AFsetRead - %s, %g : %g", AFM_MisSRate, Sfreq, Fv);
    }
  }

  return Sfreq;
}

/* Set the bits/sample value */


static int
AF_NbSInfo (const struct AF_info *AFInfo, const struct AF_dformat *DFormat)

{
  int Res, NbS, Format, IntFormat;
  struct AF_NbS AFNbS;

  Format = DFormat->Format;
  Res = 8 * AF_DL[Format];
  NbS = DFormat->NbS;

  /* Integer formats */
  IntFormat = (Format == FD_UINT8 || Format == FD_INT8 || Format == FD_INT16 ||
               Format == FD_INT24 || Format == FD_INT32);

  /* Pick up NbS from the Info string, if not already set */
  if (NbS == 0) {
    NbS = Res;
    if (IntFormat) {
      AFNbS = AFgetInfoNbS (AFInfo);
      if (AFNbS.Res > 0) {
        if (AFNbS.Res != Res)
          UTwarn (AFMF_InvNbS, "AFsetRead -", AFNbS.Res, Res);
        else
          NbS = AFNbS.NbS;
      }
    }
  }

  /* Res == 0 for text files */
  if (Res > 0 && (NbS <= 0 || NbS > Res)) {
    UTwarn (AFMF_InvNbS, "AFsetRead -", NbS, Res);
    NbS = Res;
  }
  else if (Res < NbS) {
    UTwarn ("AFsetRead - %s", AFM_BadNbS);
    NbS = Res;
  }

  /* Warn and reset NbS for inappropriate formats */
  if (NbS != Res && ! IntFormat) {
    UTwarn ("AFsetRead - %s", AFM_InaNbS);
    NbS = Res;
  }

  return NbS;
}

/* Allocate space for and set the loudspeaker configuration */

#define MINV(a, b)  (((a) < (b)) ? (a) : (b))


static unsigned char *
AF_setSpeaker (unsigned char *SpkrConfigIn, struct AF_info *AFInfo)

{
  int Nspkr;
  unsigned char *SpkrConfigTemp;
  unsigned char *SpkrConfig;

  if (SpkrConfigIn[0] == '\0')
    SpkrConfigTemp = AFgetInfoSpkr (AFInfo);
  else
    SpkrConfigTemp = SpkrConfigIn;

  Nspkr = (int) strlen ((const char *) SpkrConfigTemp);
  SpkrConfig = NULL;
  if (Nspkr > 0) {
    SpkrConfig = (unsigned char *) UTmalloc (Nspkr + 1);
    strcpy ((char *) SpkrConfig, (const char *) SpkrConfigTemp);
  }

  return SpkrConfig;
}
