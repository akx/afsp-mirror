/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  AFILE *AFsetWrite (FILE *fp, int Ftype, const struct AF_write *AFw)

Purpose:
  Set up the parameters in an audio file structure for writing

Description:
  This routine checks the input parameters and puts them into a new audio file
  structure.

  This routine checks for a "sample_rate:" record in the information string.
  This record can specify a non-integer sampling frequency.

Parameters:
  <-  AFILE *AFsetWrite
      Audio file pointer for the audio file.  This routine allocates the space
      for this structure.
   -> FILE *fp
      File pointer for the audio file
   -> int Ftype
      File type: FT_NH, FT_AU, FT_WAVE, or FT_AIFF_C
   -> const struct AF_write *AFw
      Structure with data and file parameters

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.43 $  $Date: 2003/01/27 13:56:11 $

-------------------------------------------------------------------------*/

static char rcsid [] = "$Id: AFsetWrite.c 1.43 2003/01/27 AFsp-v6r8 $";

#include <assert.h>
#include <string.h>

#include <libtsp.h>
#include <libtsp/nucleus.h>
#include <libtsp/AFdataio.h>
#include <libtsp/AFheader.h>
#include <libtsp/AFmsg.h>
#define AF_DATA_LENGTHS
#include <libtsp/AFpar.h>

static void
AF_setInfo (const struct AF_infoX *Hinfo, struct AF_info *AFinfo);
static int
AF_setWNbS (const struct AF_dformat *DFormat);


AFILE *
AFsetWrite (FILE *fp, int Ftype, const struct AF_write *AFw)

{
  AFILE *AFp;
  long int Start;
  int ErrCode, Nspkr;

  assert (Ftype == FT_WAVE || Ftype == FT_AU ||
	  Ftype == FT_AIFF || Ftype == FT_AIFF_C ||
	  Ftype == FT_NH);
  assert (AFw->DFormat.Format > 0 && AFw->DFormat.Format < NFD);
  assert (AFw->DFormat.ScaleF > 0.0);
  assert (AFw->Nchan > 0);

/* Warnings */
  if (AFw->Sfreq <= 0.0)
    UTwarn ("AFsetWrite - %s", AFM_NPSFreq);

/* Set the start of data */
  if (FLseekable (fp)) {
    ErrCode = 0;
    Start = AFtell (fp, &ErrCode);
    if (ErrCode)
      return NULL;
  }
  else
    Start = 0;

/* Set the parameters for file access */
  AFp = (AFILE *) UTmalloc (sizeof (AFILE));
  AFp->fp = fp;
  AFp->Op = FO_WO;
  AFp->Error = AF_NOERR;
  AFp->Novld = 0;
  AFp->Sfreq = AFw->Sfreq;
  AFp->Ftype = Ftype;
  AFp->Format = AFw->DFormat.Format;
  if (AF_DL[AFw->DFormat.Format] <= 1)
    AFp->Swapb = DS_NATIVE;
  else
    AFp->Swapb = UTswapCode (AFw->DFormat.Swapb);
  AFp->NbS = AF_setWNbS (&AFw->DFormat);
  AFp->ScaleF = AFw->DFormat.ScaleF;
  AFp->Nchan = AFw->Nchan;

  AFp->SpkrConfig = NULL;
  Nspkr = strlen ((const char *) AFw->SpkrConfig);
  if (Nspkr > 0) {
    AFp->SpkrConfig = (unsigned char *) UTmalloc (Nspkr + 1);
    strcpy ((char *) AFp->SpkrConfig, (const char *) AFw->SpkrConfig);
  }

  AFp->Start = Start;
  AFp->Isamp = 0;
  AFp->Nsamp = 0;
  AF_setInfo (&AFw->Hinfo, &AFp->Hinfo);

  return AFp;
}

/* Fill in the AFsp Information structure */


static void
AF_setInfo (const struct AF_infoX *Hinfo, struct AF_info *AFinfo)

{
  int N;

  if (Hinfo == NULL || Hinfo->N <= 0) {
    AFinfo->Info = NULL;
    AFinfo->N = 0;
  }
  else {
    N = Hinfo->N;
    if (Hinfo->Info[N-1] != '\0')
      ++N;
    AFinfo->Info = (char *) UTmalloc (N);
    memcpy (AFinfo->Info, Hinfo->Info, Hinfo->N);
    AFinfo->Info[N-1] = '\0';	/* Make sure there is a terminating null */
    AFinfo->N = N;
  }

  return;
}

/* Set the bits/sample value */


static int
AF_setWNbS (const struct AF_dformat *DFormat)

{
  int NbS;

  NbS = 8 * AF_DL[DFormat->Format];
  if (DFormat->NbS < 0 || DFormat->NbS > NbS)
    UTwarn (AFMF_InvNbS, "AFsetWrite -", DFormat->NbS, NbS);
  else if (DFormat->NbS != 0)
    NbS = DFormat->NbS;

  return NbS;
}
