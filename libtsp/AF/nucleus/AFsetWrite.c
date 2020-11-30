/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  AFILE *AFsetWrite(FILE *fp, enum AF_FT_T Ftype, const struct AF_write *AFw)

Purpose:
  Set up the parameters in an audio file structure for writing

Description:
  This routine checks the input parameters and puts them into a new audio file
  structure.

Parameters:
  <-  AFILE *AFsetWrite
      Audio file pointer for the audio file. This routine allocates the space
      for this structure.
   -> FILE *fp
      File pointer for the audio file
   -> AF_FT_T Ftype
      File type: FT_NH, FT_AU, FT_WAVE, or FT_AIFF_C
   -> const struct AF_write *AFw
      Structure with data and file parameters

Author / revision:
  P. Kabal  Copyright (C) 2020
  $Revision: 1.66 $  $Date: 2020/11/26 11:33:35 $

-------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>

#include <libtsp.h>
#include <AFpar.h>
#include <libtsp/nucleus.h>
#include <libtsp/AFdataio.h>
#include <libtsp/AFheader.h>
#include <libtsp/AFmsg.h>

/* Local functions */
static void
AF_setInfo(const struct AF_info *WInfo, struct AF_info *AFInfo);
static int
AF_setWNbS(int NbS, int Format);


AFILE *
AFsetWrite(FILE *fp, enum AF_FT_T Ftype, const struct AF_write *AFw)

{
  AFILE *AFp;
  long int DStart;
  int Nspkr;
  enum AF_ERR_T ErrCode;
  AF_CHUNKINFO_INIT(AF_chunkInfo_init);

  assert(Ftype == FT_WAVE || Ftype == FT_WAVE_EX ||
         Ftype == FT_AU   ||
         Ftype == FT_AIFF || Ftype == FT_AIFF_C  ||
         Ftype == FT_NH   || Ftype == FT_TXAUD);
  assert(AFw->DFormat.Format > 0 && AFw->DFormat.Format < AF_NFD);
  assert(AFw->Nchan > 0);

/* Warnings */
  if (AFw->Sfreq <= 0.0)
    UTwarn("AFsetWrite - %s", AFM_NPSfreq);

/* Set the start of data */
  if (FLseekable(fp)) {
    ErrCode = AF_NOERR;
    DStart = AFtell(fp, &ErrCode);
    if (ErrCode)
      return NULL;
  }
  else
    DStart = 0;

/* Set the parameters for file access */
  AFp = (AFILE *) UTmalloc(sizeof(AFILE));

  /* File pointers and parameters */
  AFp->fp = fp;
  AFp->Op = FO_WO;
  if (AF_DL[AFw->DFormat.Format] <= 1)
    AFp->Swapb = DS_NATIVE;
  else
    AFp->Swapb = UTswapCode(AFw->DFormat.Swapb);
  AFp->Novld = 0;
  AFp->Start = DStart;
  AFp->Isamp = 0;

  /* Error indicators */
  AFp->Error = AF_NOERR;
  AFp->Novld = 0;

  /* File data layout parameters */
  AFp->Ftype = Ftype;
  AFp->Format = AFw->DFormat.Format;
  AFp->Nsamp = 0;
  AFp->Nchan = AFw->Nchan;
  AFp->Nframe = AFw->Nframe;

  /* File data parameters */
  AFp->Sfreq = AFw->Sfreq;
  AFp->NbS = AF_setWNbS(AFw->DFormat.NbS, AFp->Format);
  AFp->FullScale = AFw->DFormat.FullScale;

  /* Program data parameters */
  AFp->ScaleV = AFopt.ScaleV;
  AFp->ScaleF = AF_FULLSCALE[AFp->Format] / AFp->ScaleV;

  /* Data descriptors */
  /* Set up the information record structure */
  AF_setInfo(&AFw->WInfo, &AFp->AFInfo);
  /* Set up empty chunk info structure - not used for write */
  AFp->ChunkInfo = AF_chunkInfo_init;
  /* Set up the loudspeaker configuration */
  AFp->SpkrConfig = NULL;
  Nspkr = (int) strlen((const char *) AFw->SpkrConfig);
  if (Nspkr > 0) {
    AFp->SpkrConfig = (unsigned char *) UTmalloc(Nspkr + 1);
    STcopyMax((const char *) AFw->SpkrConfig, (char *) AFp->SpkrConfig, Nspkr);
  }

  return AFp;
}

/* Fill in the AFsp Information structure */


static void
AF_setInfo(const struct AF_info *WInfo, struct AF_info *InfoOut)

{
  int N;

  if (WInfo == NULL || WInfo->N <= 0) {
    InfoOut->Info = NULL;
    InfoOut->N = 0;
  }
  else {
    N = WInfo->N;
    InfoOut->Info = (char *) UTmalloc(N);
    memcpy(InfoOut->Info, WInfo->Info, N);
    InfoOut->N = N;
  }
}

/* Set the bits/sample value */


static int
AF_setWNbS(int NbS, int Format)

{
  int Res;

  Res = 8 * AF_DL[Format];
  if (NbS < 0 || NbS > Res)
    UTwarn(AFMF_InvNbS, "AFsetWrite -", NbS, Res);
  else if (NbS != 0)
    Res = NbS;

  return Res;
}
