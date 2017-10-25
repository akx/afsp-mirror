/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
 AFILE *AFwrWVhead (FILE *fp, struct AF_write *AFw)

Purpose:
  Write header information to a WAVE file

Description:
  This routine writes header information to a WAVE format audio file.

  WAVE file:
   Offset Length Type    Contents
      0     4    char   "RIFF" file identifier
      4     4    int    Chunk length
      8     4    char   "WAVE" file identifier
     12     4    char   "fmt " chunk identifier
     16     4    int    Chunk length
     20     2    int      Audio data type
     22     2    int      Number of interleaved channels
     24     4    int      Sample rate
     28     4    int      Average bytes/sec
     32     2    int      Block align
     34     2    int      Data word length (bits)
     36     2    int      Extra info size
     38          ...     "fmt " extension
      C     4    int    "fact" chunk identifier (only for non-PCM data)
    C+4     4    int    Chunk length
    C+8     4    int      Number of samples (per channel)
    ...   ...    ...    ...
      D     4    char   "data" chunk identifier
    D+4     4    int    Chunk length
    D+8    ...   ...      Audio data

Parameters:
  <-  AFILE *AFwrWVhead
      Audio file pointer for the audio file.  This routine allocates the
      space for this structure.  If an error is detected, a NULL pointer is
      returned.
   -> FILE *fp
      File pointer for the audio file
  <-> struct AF_write *AFw
      Structure containing file parameters
      AFw->DFormat.Swapb - set to DS_EL

Author / revision:
  P. Kabal  Copyright (C) 2017
  $Revision: 1.84 $  $Date: 2017/05/24 16:15:38 $

-------------------------------------------------------------------------*/

#include <assert.h>
#include <setjmp.h>
#include <string.h>

#define WV_TEXT_CHUNKS

#include <libtsp.h>
#include <AFpar.h>
#include <libtsp/nucleus.h>
#include <libtsp/AFheader.h>
#include <libtsp/AFinfo.h>
#include <libtsp/AFmsg.h>
#ifdef WV_TEXT_CHUNKS
#  define WV_INFO_REC
#endif    /* WV_TEXT_CHUNKS */
#define WV_SPKR
#include <libtsp/WVpar.h>

#define ALIGN   2 /* Chunks padded out to a multiple of ALIGN */

/* setjmp / longjmp environment */
extern jmp_buf AFW_JMPENV;

/* Local functions */
static int
AF_setFMT (struct WV_Ckfmt *Ckfmt, const struct AF_write *AFw);
static int
AF_setTextChunks (struct WV_CkRIFF *CkRIFF, struct AF_info *TInfo, char *Text);
static UT_uint4_t
AF_spkrConfigMask (const unsigned char *SpkrConfig);
static int
AF_wrFMT (FILE *fp, const struct WV_Ckfmt *Ckfmt);
static void
AF_wrRIFF (FILE *fp, const struct WV_CkRIFF *CkRIFF);
static int
AF_wrTextChunks (FILE *fp, const struct WV_CkRIFF *CkRIFF);

AFILE *
AFwrWVhead (FILE *fp, struct AF_write *AFw)

{
  AFILE *AFp;
  int Ext, Lw, PCM;
  long int size, Ldata;
  struct WV_CkRIFF CkRIFF;
  char *TempBuff, *Text;
  int NInfo, sizeT;
  struct AF_info TInfo;

/* Set the long jump environment; on error return a NULL */
  if (setjmp (AFW_JMPENV))
    return NULL; /* Return from a header write error */

/* Set up the encoding parameters */
  Lw = AF_DL[AFw->DFormat.Format];
  if (AFw->Nframe != AF_NFRAME_UNDEF)
    Ldata = AFw->Nframe * AFw->Nchan * Lw;
  else if (FLseekable (fp))
    Ldata = 0L;
  else {
    UTwarn ("AFwrWVhead - %s", AFM_WV_WRAccess);
    return NULL;
  }

  /* RIFF chunk */
  MCOPY (ckID_RIFF, CkRIFF.ckID);
  /* defer filling in the chunk size */
  MCOPY ("WAVE", CkRIFF.WAVEID);

  /* fmt chunk */
  Ext = AF_setFMT (&CkRIFF.Ckfmt, AFw);
  PCM = (CkRIFF.Ckfmt.wFormatTag == WAVE_FORMAT_PCM ||
         (Ext && CkRIFF.Ckfmt.SubFormat.wFormatTag == WAVE_FORMAT_PCM));

  /* fact chunk */
  if (! PCM) {
    MCOPY (ckID_fact, CkRIFF.Ckfact.ckID);
    CkRIFF.Ckfact.ckSize = (UT_uint4_t) sizeof (CkRIFF.Ckfact.dwSampleLength);
    if (AFw->Nframe != AF_NFRAME_UNDEF)
      CkRIFF.Ckfact.dwSampleLength = (UT_uint4_t) AFw->Nframe;
    else
      CkRIFF.Ckfact.dwSampleLength = (UT_uint4_t) 0;
  }

/* ----- ----- ----- ----- */
  /* Copy the information records to temp storage */
  /* NInfo chars are reserved for Info record structure (modified when records
     are deleted). Another NInfo characters are reserved for the text strings
     which are extracted from the information record structure.  The text
     array must be retained until afte the chunks are written to file. */
  NInfo = AFw->WInfo.N;
  TempBuff = UTmalloc (2 * NInfo);
  TInfo.Info = TempBuff;
  memcpy (TInfo.Info, AFw->WInfo.Info, NInfo);
  TInfo.N = NInfo;
  TInfo.Nmax = NInfo;
  Text = TempBuff + NInfo;

  /* Fill in the DISP, LIST/INFO, afsp chunks */
  sizeT = AF_setTextChunks (&CkRIFF, &TInfo, Text);

  /* data chunk */
  MCOPY (ckID_data, CkRIFF.Ckdata.ckID);
  CkRIFF.Ckdata.ckSize = (UT_uint4_t) Ldata;

  /* Fill in the RIFF chunk size */
  size  = 4 + 8 + RNDUPV (CkRIFF.Ckfmt.ckSize, ALIGN);
  if (! PCM)
    size += 8 + RNDUPV (CkRIFF.Ckfact.ckSize, ALIGN);
  size += sizeT;
  size += 8 + RNDUPV (CkRIFF.Ckdata.ckSize, ALIGN);
  CkRIFF.ckSize = (UT_uint4_t) size;

/* Update relevant parts of AFw */
  AFw->DFormat.Swapb = DS_EL;

/* Write out the header (error return via longjmp) */
/* Text chunks are written after the data by AFupdWVhead */
  AF_wrRIFF (fp, &CkRIFF);

  /* Recover temporary text buffer space */
  UTfree (TempBuff);

/* Create the audio file structure */
  if (Ext)
    AFp = AFsetWrite (fp, FT_WAVE_EX, AFw);
  else
    AFp = AFsetWrite (fp, FT_WAVE, AFw);

  return AFp;
}

/* Fill in the fmt chunk */


static int
AF_setFMT (struct WV_Ckfmt *Ckfmt, const struct AF_write *AFw)

{
  int Lw, Ext, Res, NbS;
  UT_uint2_t FormatTag;

/* First: Set the fmt chunk without the extension */
/* Later: Modify the parameters for an extension, if appropriate */
  Lw = AF_DL[AFw->DFormat.Format];
  switch (AFw->DFormat.Format) {
  case FD_UINT8:
  case FD_INT16:
  case FD_INT24:
  case FD_INT32:
    FormatTag = WAVE_FORMAT_PCM;
    break;
  case FD_MULAW8:
    FormatTag = WAVE_FORMAT_MULAW;
    break;
  case FD_ALAW8:
    FormatTag = WAVE_FORMAT_ALAW;
    break;
  case FD_FLOAT32:
  case FD_FLOAT64:
    FormatTag = WAVE_FORMAT_IEEE_FLOAT;
    break;
  default:
    FormatTag = WAVE_FORMAT_UNKNOWN;
    assert (0);
  }

  /* Set the chunk size (minimum size for now) */
  MCOPY (ckID_fmt, Ckfmt->ckID);
  Ckfmt->ckSize = sizeof (Ckfmt->wFormatTag) + sizeof (Ckfmt->nChannels)
      + sizeof (Ckfmt->nSamplesPerSec) + sizeof (Ckfmt->nAvgBytesPerSec)
      + sizeof (Ckfmt->nBlockAlign) + sizeof (Ckfmt->wBitsPerSample);

  Ckfmt->nChannels = (UT_uint2_t) AFw->Nchan;
  Ckfmt->nSamplesPerSec = (UT_uint4_t) (AFw->Sfreq + 0.5);  /* Rounding */
  Ckfmt->nAvgBytesPerSec = (UT_uint4_t) (Ckfmt->nSamplesPerSec * AFw->Nchan * Lw);
  Ckfmt->nBlockAlign = (UT_uint2_t) (Lw * AFw->Nchan);

  /* Determine whether to use an extensible header */
  Res = 8 * Lw;
  NbS = AFw->DFormat.NbS;
  if (AFw->FtypeW == FTW_WAVE_NOEX)
    Ext = 0;
  else {
    Ckfmt->dwChannelMask = AF_spkrConfigMask (AFw->SpkrConfig);
    Ext = (AFw->FtypeW == FTW_WAVE_EX ||
           AFw->Nchan > 2 || NbS != Res || AFw->DFormat.Format == FD_INT24 ||
           AFw->DFormat.Format == FD_INT32 || Ckfmt->dwChannelMask != 0L);
  }

  if (Ext) {
    Ckfmt->wBitsPerSample = (UT_uint2_t) Res;
    Ckfmt->wFormatTag = WAVE_FORMAT_EXTENSIBLE;
    Ckfmt->cbSize = 22;
    if (FormatTag == WAVE_FORMAT_PCM || FormatTag == WAVE_FORMAT_IEEE_FLOAT)
      Ckfmt->wValidBitsPerSample = (UT_uint2_t) NbS;
    else
      Ckfmt->wValidBitsPerSample = (UT_uint2_t) Res;
    Ckfmt->SubFormat.wFormatTag = FormatTag;
    MCOPY (WAVEFORMATEX_TEMPLATE.guidx, Ckfmt->SubFormat.guidx);
  }
  else {
    /* Use wBitsPerSample to specify the "valid bits per sample" */
    if (RNDUPV (NbS, 8) != Res) {
      UTwarn (AFMF_WV_InvNbS, "AFwrWVhead -", NbS, Res);
      NbS = Res;
    }
    Ckfmt->wBitsPerSample = (UT_uint2_t) NbS;
    Ckfmt->wFormatTag = FormatTag;
    Ckfmt->cbSize = 0;
  }

  /* Update the chunk size with the size of the chunk extension */
  if (Ckfmt->wFormatTag != WAVE_FORMAT_PCM)
    Ckfmt->ckSize += (sizeof (Ckfmt->cbSize) + Ckfmt->cbSize);

  return Ext;
}

/* Encode channel/speaker information */


static UT_uint4_t
AF_spkrConfigMask (const unsigned char *SpkrConfig)

{
  int i, n, mp, m, Nspkr;
  UT_uint4_t ChannelMask;

  if (SpkrConfig[0] == '\0')
    Nspkr = 0;
  else
    Nspkr = (int) strlen ((const char *) SpkrConfig);

  ChannelMask = 0;
  mp = -1;
  for (i = 0; i < Nspkr; ++i) {
    n = SpkrConfig[i];
    if (n == AF_SPKR_AUX)
      mp = WV_N_SPKR_MASK; /* If any more locations => out of order error */
    else {
      m = AF_WV_ChannelMask[n-1];
      if (m == WV_X) {
        UTwarn ("AFwrWVhead - %s", AFM_WV_NSSpkr);
        ChannelMask = 0;
        break;
      }
      if (m <= mp) {   /* Check ordering */
        UTwarn ("AFwrWVhead - %s", AFM_WV_NSSpkrO);
        ChannelMask = 0;
        break;
      }
      ChannelMask = ChannelMask | WV_Spkr_Mask[m];
      mp = m;
    }
  }

  return ChannelMask;
}

/* Write out the header */


static void
AF_wrRIFF (FILE *fp, const struct WV_CkRIFF *CkRIFF)

{
  WHEAD_S (fp, CkRIFF->ckID);
  WHEAD_V (fp, CkRIFF->ckSize, DS_EL);
  WHEAD_S (fp, CkRIFF->WAVEID);

  AF_wrFMT (fp, &CkRIFF->Ckfmt);

  /* Write the Fact chunk if the data format is not PCM */
  if (!(CkRIFF->Ckfmt.wFormatTag == WAVE_FORMAT_PCM ||
      (CkRIFF->Ckfmt.wFormatTag == WAVE_FORMAT_EXTENSIBLE &&
       CkRIFF->Ckfmt.SubFormat.wFormatTag == WAVE_FORMAT_PCM))) {
    WHEAD_S (fp, CkRIFF->Ckfact.ckID);
    WHEAD_V (fp, CkRIFF->Ckfact.ckSize, DS_EL);
    WHEAD_V (fp, CkRIFF->Ckfact.dwSampleLength, DS_EL);
  }

  /* Write out the DISP, LIST/INFO, afsp chunks */
  AF_wrTextChunks (fp, CkRIFF);

  WHEAD_S (fp, CkRIFF->Ckdata.ckID);
  WHEAD_V (fp, CkRIFF->Ckdata.ckSize, DS_EL);

  return;
}

/* Write the format chunk */


static int
AF_wrFMT (FILE *fp, const struct WV_Ckfmt *Ckfmt)

{
  long int offs, LFMT;

  offs  = WHEAD_S (fp, Ckfmt->ckID);
  offs += WHEAD_V (fp, Ckfmt->ckSize, DS_EL);
  LFMT = Ckfmt->ckSize + 8;
  offs += WHEAD_V (fp, Ckfmt->wFormatTag, DS_EL);
  offs += WHEAD_V (fp, Ckfmt->nChannels, DS_EL);
  offs += WHEAD_V (fp, Ckfmt->nSamplesPerSec, DS_EL);
  offs += WHEAD_V (fp, Ckfmt->nAvgBytesPerSec, DS_EL);
  offs += WHEAD_V (fp, Ckfmt->nBlockAlign, DS_EL);
  offs += WHEAD_V (fp, Ckfmt->wBitsPerSample, DS_EL);
  if (offs < LFMT)
    offs += WHEAD_V (fp, Ckfmt->cbSize, DS_EL);
  if (offs < LFMT) {
    offs += WHEAD_V (fp, Ckfmt->wValidBitsPerSample, DS_EL);
    offs += WHEAD_V (fp, Ckfmt->dwChannelMask, DS_EL);
    offs += WHEAD_V (fp, Ckfmt->SubFormat.wFormatTag, DS_EL);
    offs += WHEAD_S (fp, Ckfmt->SubFormat.guidx);
  }

  assert (offs == LFMT);

  return offs;
}

/* Fill in the DISP, LIST/INFO, and afsp chunks */
/* Text chunks:
   - The text in DISP and LIST/INFO chunks is null terminated
   - For each chunk, pick up the record text and delete the info record
   - Any remaining records are written to the AFsp chunk
  The text strings for the chunks point to pieces of the text buffer Text.  This
  buffer must be retained until after the chunks have been written to the file.

*/

static int
AF_setTextChunks (struct WV_CkRIFF *CkRIFF, struct AF_info *TInfo, char *Text)

{
  int N, i, Nc, Nmax, size;
  const struct WV_LI *LImap;
  struct WV_CkDISP *CkDISP;
  struct WV_CkLIST *CkLIST;
  struct WV_Ckafsp *Ckafsp;

  Nmax = TInfo->N - 1;  /* Maximum room needed to decode subsequent strings */

  /* DISP chunk */
  CkDISP = &CkRIFF->CkDISP;
  Nc = AFgetDelInfoRec (WV_DISPid, TInfo, Text, Nmax);
  if (Nc > 0) {
    Nc++;   /* Nc +  terminating null */
    MCOPY (ckID_DISP, CkDISP->ckID);
    CkDISP->ckSize = 4 + Nc;
    CkDISP->CFtype = (UT_uint4_t) (CF_TEXT);
    CkDISP->text = Text;
  }
  else
    CkDISP->ckSize = 0;
  Text += Nc;
  Nmax -= Nc;

  /* LIST/INFO preamble */
  CkLIST = &CkRIFF->CkLIST;
  MCOPY (ckID_LIST, CkLIST->ckID);
  CkLIST->ckSize = 4;       /* CkLIST->ckSize updated in the loop below */
  MCOPY (FM_INFO, CkLIST->INFOID);

  /* Look for a record with the given identifier */
  N = 0;
  for (i = 0; i < N_LIMAP; ++i) {
    LImap = &WV_LImap[i];
    Nc = AFgetDelInfoRec ((const char **) LImap->RecID, TInfo, Text, Nmax);
    if (Nc > 0) {
      ++Nc;     /* Count terminating null */
      MCOPY (LImap->ckID, CkLIST->CkINFO[N].ckID); /* Item ID */
      CkLIST->CkINFO[N].ckSize = Nc;
      CkLIST->CkINFO[N].text = Text;
      CkLIST->ckSize += 8 + RNDUPV (CkLIST->CkINFO[N].ckSize, ALIGN);
      ++N;
      Text += Nc;
      Nmax -= Nc;
    }
  }
  if (CkLIST->ckSize == 4)
    CkLIST->ckSize = 0;
  CkLIST->N = N;            /* Number of INFO chunks (not written to file) */

  /* afsp chunk */
  Ckafsp = &CkRIFF->Ckafsp;
  if (TInfo->N > 0) {
    MCOPY (ckID_afsp, Ckafsp->ckID);
    Ckafsp->ckSize = 4 + TInfo->N;
    MCOPY (FM_AFSP, Ckafsp->AFspID);
    Ckafsp->text = TInfo->Info;
  }
  else
    Ckafsp->ckSize = 0;

  /* Total up the size to be written to file */
  size = 0;
  if (CkDISP->ckSize > 0)
    size += 8 + RNDUPV (CkDISP->ckSize, ALIGN);
  if (CkLIST->ckSize > 0)
    size += 8 + RNDUPV (CkLIST->ckSize, ALIGN);
  if (Ckafsp->ckSize > 0)
    size += 8 + RNDUPV (Ckafsp->ckSize, ALIGN);

  return size;
}

/* Write the DISP, LIST/INFO, afsp chunks */


static int
AF_wrTextChunks (FILE *fp, const struct WV_CkRIFF *CkRIFF)

{
  int i, offs;
  const struct WV_CkDISP *CkDISP;
  const struct WV_CkLIST *CkLIST;
  const struct WV_Ckafsp *Ckafsp;

  offs = 0;

/* DISP chunk */
  CkDISP = &CkRIFF->CkDISP;
  if (CkDISP->ckSize > 0) {
    offs += WHEAD_S (fp, CkDISP->ckID);
    offs += WHEAD_V (fp, CkDISP->ckSize, DS_EL);
    offs += WHEAD_V (fp, CkDISP->CFtype, DS_EL);
    offs += WHEAD_SN (fp, CkDISP->text, CkDISP->ckSize - 4);
    offs += WRPAD (fp, offs, ALIGN);
  }

/* LIST chunk */
  CkLIST = &CkRIFF->CkLIST;
  if (CkLIST->ckSize > 0) {
    offs += WHEAD_S (fp, CkLIST->ckID);
    offs += WHEAD_V (fp, CkLIST->ckSize, DS_EL);
    offs += WHEAD_S (fp, CkLIST->INFOID);
    for (i = 0; i < CkLIST->N; ++i) {
      offs += WHEAD_S (fp, CkLIST->CkINFO[i].ckID);
      offs += WHEAD_V (fp, CkLIST->CkINFO[i].ckSize, DS_EL);
      offs += WHEAD_SN (fp, CkLIST->CkINFO[i].text, CkLIST->CkINFO[i].ckSize);
      offs += WRPAD (fp, CkLIST->CkINFO[i].ckSize, ALIGN);
    }
  }

/* afsp chunk */
  Ckafsp = &CkRIFF->Ckafsp;
  if (Ckafsp->ckSize > 0) {
    offs += WHEAD_S (fp, Ckafsp->ckID);
    offs += WHEAD_V (fp, Ckafsp->ckSize, DS_EL);
    offs += WHEAD_S (fp, Ckafsp->AFspID);
    offs += WHEAD_SN (fp, Ckafsp->text, Ckafsp->ckSize - 4);
    offs += WRPAD (fp, offs, ALIGN);
  }

  return offs;
}
