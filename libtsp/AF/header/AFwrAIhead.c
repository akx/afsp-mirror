/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  AFILE * AFwrAIhead (FILE *fp, struct AF_write *AFw)

Purpose:
  Write header information to an AIFF or AIFF-C sound file

Description:
  This routine writes header information to an AIFF or AIFF-C format sound file.

  AIFF / AIFF-C sound file:
   Offset Length Type    Contents
      0     4    char   File identifier ("FORM")
      4     4    int    Chunk length
      8     4    char   File identifier ("AIFC" or "AIFF")
     12     4    char   "FVER" chunk identifier (AIFF-C files only)
     16     4    int    Chunk length
     20     4    int      Format version identifier
      C     4    char   "COMM" chunk identifier (C is 12 (AIFF) or 24 (AIFF-C))
            4    int    Chunk length
            2    int      Number of interleaved channels
    +10     4    int      Number of sample frames
            2    int      Bits per sample
           10    float    Sample frames per second
            4    int      Compression type (AIFF-C files only)
          ...    char     Compression name (AIFF-C files only)
    ...    ...   ...    ...
      S     4    char   SSND chunk identifier ("SSND")
     +4     4    int    Chunk length
     +8     4    int      Data offset (0)
    +12     4    int      Block size (0)
    +16   ...    ...      Audio data

  Note: The number of bits per sample can be less than the the number of bits
  in the data samples, for instance 21 bits in a 24-bit data sample.  The AIFF/
  AIFF-C standard requires that the data appear in the high order bits and that
  the low order bits of the container be set to zero. In these routines, this
  masking is not done on write or read.  This means the bits per sample is
  informational only.  Masking, if desired, must be done on the data before
  writing and after reading.

Parameters:
  <-  AFILE *AFwrAIhead
      Audio file pointer for the audio file.  This routine allocates the
      space for this structure.  If an error is detected, a NULL pointer is
      returned.
   -> FILE *fp
      File pointer for the audio file
  <-> struct AF_write *AFw
      Structure containing file parameters

Author / revision:
  P. Kabal  Copyright (C) 2017
  $Revision: 1.72 $  $Date: 2017/09/22 00:31:47 $

-------------------------------------------------------------------------*/

#include <assert.h>
#include <setjmp.h>
#include <string.h>

#include <libtsp.h>
#include <AFpar.h>
#include <libtsp/nucleus.h>
#include <libtsp/AFheader.h>
#include <libtsp/AFinfo.h>
#include <libtsp/AFmsg.h>
#define AI_INFOREC
#include <libtsp/AIpar.h>

#define STCOPY(src,dest)        STcopyMax (src, dest, sizeof (dest) - 1)

#define ALIGN   2          /* Chunks padded out to a multiple of ALIGN */

/* setjmp / longjmp environment */
extern jmp_buf AFW_JMPENV;

/* Local functions */
static void
AF_setCOMM (struct AI_CkCOMM *CkCOMM, struct AF_write *AFw);
static void
AF_wrFORM (FILE *fp, const struct AI_CkFORM *CkFORM);
static int
AF_wrPstring (FILE *fp, const char string[]);
static int
AF_checkAISpeaker (const unsigned char *SpkrConfig, int Nchan);


AFILE *
AFwrAIhead (FILE *fp, struct AF_write *AFw)

{
  AFILE *AFp;
  char *TempBuff, *Text;
  struct AF_info TInfo;

  int NInfo, Nmax, Nc, Lw;
  long int size, Ldata;
  int n;
  struct AI_CkFORM CkFORM;

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
    UTwarn ("AFwrAIhead - %s", AFM_AIFFX_WRAccess);
    return NULL;
  }

  /* Use a staging ground (CkFORM) to assemble the header, and when complete
     write the header to the file */

  /* FORM chunk */
  MCOPY (FM_IFF, CkFORM.ckID);
  /* defer filling in the chunk size */
  if (AFw->FtypeW == FTW_AIFF_C || AFw->FtypeW == FTW_AIFF_C_SOWT)
    MCOPY (FM_AIFF_C, CkFORM.AIFFID);
  else
    MCOPY (FM_AIFF, CkFORM.AIFFID);

  /* FVER chunk */
  if (AFw->FtypeW == FTW_AIFF_C || AFw->FtypeW == FTW_AIFF_C_SOWT) {
    MCOPY (ckID_FVER, CkFORM.CkFVER.ckID);
    CkFORM.CkFVER.ckSize = 4;
    CkFORM.CkFVER.timestamp = AIFCVersion1;
  }
  else
    CkFORM.CkFVER.ckSize = 0;   /* Flag to avoid writing the FVER chunk */

  /* COMM chunk */
  AF_setCOMM (&CkFORM.CkCOMM, AFw);

  /* Copy the information records to temp storage */
  /* NInfo chars are reserved for Info record structure (modified when records
     are deleted). Another NInfo characters are reserved for the text strings
     which are extracted from the information record structure. Any given text
     string is extracted only once (since the information record in which it
     appears is then deleted), the maximum size required is less than NInfo. */
  NInfo = AFw->WInfo.N;
  TempBuff = UTmalloc (2 * NInfo);
  TInfo.Info = TempBuff;
  memcpy (TInfo.Info, AFw->WInfo.Info, NInfo);
  TInfo.N = NInfo;
  TInfo.Nmax = NInfo;

  Text = TempBuff + NInfo;
  Nmax = NInfo - 1;      /* Room available for the next text string not counting
                            the terminal null */

  /* Text chunks:
     - The text in NAME, AUTH, (c), and ANNO chunks is not null terminated
     - For each chunk, pick up the text and delete the info record
  */

  /* Name chunk for title */
  MCOPY (ckID_NAME, CkFORM.CkNAME.ckID);
  Nc = AFgetDelInfoRec (AI_NAMEid, &TInfo, Text, Nmax);
  CkFORM.CkNAME.ckSize = Nc;
  CkFORM.CkNAME.text = Text;

  /* Author chunk for artist */
  Text += Nc;        /* Will overlay the trailing null of the previous text */
  Nmax -= Nc;
  MCOPY (ckID_AUTH, CkFORM.CkAUTH.ckID);
  Nc = AFgetDelInfoRec (AI_AUTHid, &TInfo, Text, Nmax);
  CkFORM.CkAUTH.ckSize = Nc;
  CkFORM.CkAUTH.text = Text;

  /* Copyright chunk */
  Text += Nc;
  Nmax -= Nc;
  MCOPY (ckID_CPRT, CkFORM.CkCPRT.ckID);
  Nc = AFgetDelInfoRec (AI_CPRTid, &TInfo, Text, Nmax);
  CkFORM.CkCPRT.ckSize = Nc;
  CkFORM.CkCPRT.text = Text;

  /* ANNO chunks for comment */
  for (n = 0; n < NANNO_MAX; ++n) {
    Text += Nc;
    Nmax -= Nc;
    Nc = AFgetDelInfoRec (AI_ANNOid, &TInfo, Text, Nmax);
    MCOPY (ckID_ANNO, CkFORM.CkANNO[n].ckID);
    CkFORM.CkANNO[n].ckSize = Nc;
    CkFORM.CkANNO[n].text = Text;
  }

  /* Special ANNO chunk with AFsp identifier */
  if (TInfo.N > 0) {
    MCOPY (ckID_ANNO, CkFORM.CkANNOx.ckID);
    CkFORM.CkANNOx.ckSize = (4 + TInfo.N);  /* Info records */
    MCOPY (FM_AFSP, CkFORM.CkANNOx.InfoID);
    CkFORM.CkANNOx.text = TInfo.Info;
  }
  else
    CkFORM.CkANNOx.ckSize = 0;

  /* SSND chunk */
  MCOPY (ckID_SSND, CkFORM.CkSSND.ckID);
  CkFORM.CkSSND.ckSize = 8 + Ldata;
  CkFORM.CkSSND.offset = 0;
  CkFORM.CkSSND.blockSize = 0;

  /* Calculate and fill in the FORM chunk size */
  size  = 4;    /* "AIFF" or "AIFC" identifier */
  size += 8 + RNDUPV (CkFORM.CkCOMM.ckSize, ALIGN);
  if (CkFORM.CkFVER.ckSize > 0)
    size += 8 + RNDUPV (CkFORM.CkFVER.ckSize, ALIGN);
  if (CkFORM.CkNAME.ckSize > 0)
    size += 8 + RNDUPV (CkFORM.CkNAME.ckSize, ALIGN);
  if (CkFORM.CkAUTH.ckSize > 0)
    size += 8 + RNDUPV (CkFORM.CkAUTH.ckSize, ALIGN);
  if (CkFORM.CkCPRT.ckSize > 0)
    size += 8 + RNDUPV (CkFORM.CkCPRT.ckSize, ALIGN);
  for (n = 0; n < NANNO_MAX; ++n) {
    if (CkFORM.CkANNO[n].ckSize > 0)
      size += 8 + RNDUPV (CkFORM.CkANNO[n].ckSize, ALIGN);
  }
  if (CkFORM.CkANNOx.ckSize > 0)
    size += 8 + RNDUPV (CkFORM.CkANNOx.ckSize, ALIGN);
  size += 8 + RNDUPV (CkFORM.CkSSND.ckSize, ALIGN);
  CkFORM.ckSize = (UT_uint4_t) size;

/* Write out the header, leaving the file at the start of data */
  AF_wrFORM (fp, &CkFORM);

/* Set the data swap code */
  AFw->DFormat.Swapb = DS_EB;
  if (AFw->FtypeW == FTW_AIFF_C_SOWT)
    AFw->DFormat.Swapb = DS_EL;

  /* If Ldata == 0 (number of samples unknown), then after writing the data,
  - Write a pad byte if the number of bytes written is odd
  - Update three fields in the header
      CkFORM.ckSize (offset 4 bytes)
      - End of file is Nbytes = AFp->Start + Ldata + rounding
      - CkForm.ckSize -> Nbytes - 8
      CkFORM.CkCOMM.numSampleFrames (offset 22 for AIFF, offset 34 for AIFF-C)
      - CkFORM.CkCOMM.numSampleFrames -> Nsamp / Nchan
      CkFORM.SSND.ckSize (offset -12 from AFp-> Start)
      - CkFORM.SSND.ckSize -> 8 + Ldata
*/

/* Check the speaker configuration */
  AF_checkAISpeaker (AFw->SpkrConfig, AFw->Nchan);

/* Recover temporary buffer space */
  UTfree (TempBuff);

/* Create the audio file structure */
  if (AFw->FtypeW == FTW_AIFF_C || AFw->FtypeW == FTW_AIFF_C_SOWT)
    AFp = AFsetWrite (fp, FT_AIFF_C, AFw);
  else
    AFp = AFsetWrite (fp, FT_AIFF, AFw);

  return AFp;
}

/* Fill in the COMM chunk */
/* Note that a fix may be applied to AFw->DFormat.NbS */


static void
AF_setCOMM (struct AI_CkCOMM *CkCOMM, struct AF_write *AFw)

{
  int ncP, Res, NbS;

  MCOPY (ckID_COMM, CkCOMM->ckID);
  Res = 8  * AF_DL[AFw->DFormat.Format];
  NbS = AFw->DFormat.NbS;

/* Set up the encoding parameters */
  switch (AFw->DFormat.Format) {
  case FD_INT8:
  case FD_INT16:
  case FD_INT24:
  case FD_INT32:
    if (AFw->FtypeW == FTW_AIFF_C_SOWT) {
      MCOPY (CT_SOWT, CkCOMM->compressionType);
      STCOPY (CN_SOWT, CkCOMM->compressionName);
    }
    else {
      MCOPY (CT_NONE, CkCOMM->compressionType);
      STCOPY (CN_NONE, CkCOMM->compressionName);
    }
    if (RNDUPV (NbS, 8) != Res) {
      UTwarn (AFMF_AIFFX_InvNbS, "AFwrAIhead -", NbS, Res);
      NbS = Res;
      AFw->DFormat.NbS = NbS;   /* Fix number bits / sample */
    }
    CkCOMM->sampleSize = NbS;
    break;
  /* There are two codes for each of mu-law and A-law coding.  The codes
     "ULAW" and "ALAW" originated with SGI software.  The Apple QuickTime
     software only recognizes "ulaw" and "alaw". Here we use the QuickTime
     conventions.
  */
  case FD_MULAW8:
    MCOPY (CT_ULAW, CkCOMM->compressionType);
    STCOPY (CN_ULAW, CkCOMM->compressionName);
    CkCOMM->sampleSize = 16;    /* Uncompressed data size in bits */
    break;
  case FD_ALAW8:
    MCOPY (CT_ALAW, CkCOMM->compressionType);
    STCOPY (CN_ALAW, CkCOMM->compressionName);
    CkCOMM->sampleSize = 16;    /* Uncompressed data size in bits */
    break;
  case FD_FLOAT32:
    MCOPY (CT_FLOAT32, CkCOMM->compressionType);
    STCOPY (CN_FLOAT32, CkCOMM->compressionName);
    CkCOMM->sampleSize = Res;
    break;
  case FD_FLOAT64:
    MCOPY (CT_FLOAT64, CkCOMM->compressionType);
    STCOPY (CN_FLOAT64, CkCOMM->compressionName);
    CkCOMM->sampleSize = Res;
    break;
  default:
    assert (0);
  }

  CkCOMM->numChannels = (UT_uint2_t) AFw->Nchan;
  if (AFw->Nframe == AF_NFRAME_UNDEF)
    CkCOMM->numSampleFrames = (UT_uint4_t) 0;
  else
    CkCOMM->numSampleFrames = (UT_uint4_t) AFw->Nframe;
  /* CkCOMM.sampleSize filled in above */
  UTeIEEE80 (AFw->Sfreq, CkCOMM->sampleRate);
  /* CkCOMM.compressionType filled in above */
  /* CkCOMM.compressionName filled in above */

  if (AFw->FtypeW == FTW_AIFF_C || AFw->FtypeW == FTW_AIFF_C_SOWT) {
    ncP = AF_wrPstring (NULL, CkCOMM->compressionName);
    CkCOMM->ckSize = 22 + ncP;
  }
  else {
    assert (AFw->DFormat.Format == FD_INT8 ||
            AFw->DFormat.Format == FD_INT16 ||
            AFw->DFormat.Format == FD_INT24 ||
            AFw->DFormat.Format == FD_INT32);
    CkCOMM->ckSize = 18;
  }

  return;
}

/* Write out the header */


static void
AF_wrFORM (FILE *fp, const struct AI_CkFORM *CkFORM)

{
  int n;

  WHEAD_S (fp, CkFORM->ckID);
  WHEAD_V (fp, CkFORM->ckSize, DS_EB);
  WHEAD_S (fp, CkFORM->AIFFID);

  /* FVER chunk */
  if (CkFORM->CkFVER.ckSize > 0) {
    WHEAD_S (fp, CkFORM->CkFVER.ckID);
    WHEAD_V (fp, CkFORM->CkFVER.ckSize, DS_EB);
    WHEAD_V (fp, CkFORM->CkFVER.timestamp, DS_EB);
  }

  /* COMM chunk */
  WHEAD_S (fp, CkFORM->CkCOMM.ckID);
  WHEAD_V (fp, CkFORM->CkCOMM.ckSize, DS_EB);
  WHEAD_V (fp, CkFORM->CkCOMM.numChannels, DS_EB);
  WHEAD_V (fp, CkFORM->CkCOMM.numSampleFrames, DS_EB);
  WHEAD_V (fp, CkFORM->CkCOMM.sampleSize, DS_EB);
  WHEAD_S (fp, CkFORM->CkCOMM.sampleRate);
  if (CkFORM->CkCOMM.ckSize > 18) {
    WHEAD_S (fp, CkFORM->CkCOMM.compressionType);
    AF_wrPstring (fp, CkFORM->CkCOMM.compressionName);
    WRPAD (fp, CkFORM->CkCOMM.ckSize, ALIGN);
  }

  /* NAME chunk */
  if (CkFORM->CkNAME.ckSize > 0) {
    WHEAD_S (fp, CkFORM->CkNAME.ckID);
    WHEAD_V (fp, CkFORM->CkNAME.ckSize, DS_EB);
    WHEAD_SN (fp, CkFORM->CkNAME.text, CkFORM->CkNAME.ckSize);
    WRPAD (fp, CkFORM->CkNAME.ckSize, ALIGN);
  }
  /* AUTH chunk */
  if (CkFORM->CkAUTH.ckSize > 0) {
    WHEAD_S (fp, CkFORM->CkAUTH.ckID);
    WHEAD_V (fp, CkFORM->CkAUTH.ckSize, DS_EB);
    WHEAD_SN (fp, CkFORM->CkAUTH.text, CkFORM->CkAUTH.ckSize);
    WRPAD (fp, CkFORM->CkAUTH.ckSize, ALIGN);
  }
  /* Copyright chunk */
  if (CkFORM->CkCPRT.ckSize > 0) {
    WHEAD_S (fp, CkFORM->CkCPRT.ckID);
    WHEAD_V (fp, CkFORM->CkCPRT.ckSize, DS_EB);
    WHEAD_SN (fp, CkFORM->CkCPRT.text, CkFORM->CkCPRT.ckSize);
    WRPAD (fp, CkFORM->CkCPRT.ckSize, ALIGN);
  }
  /* ANNO chunks */
  for (n = 0; n < NANNO_MAX; ++n) {
    if (CkFORM->CkANNO[n].ckSize > 0) {
      WHEAD_S (fp, CkFORM->CkANNO[n].ckID);
      WHEAD_V (fp, CkFORM->CkANNO[n].ckSize, DS_EB);
      WHEAD_SN (fp, CkFORM->CkANNO[n].text, CkFORM->CkANNO[n].ckSize);
      WRPAD (fp, CkFORM->CkANNO[n].ckSize, ALIGN);
    }
  }
  /* Special ANNO-AFsp chunk */
  if (CkFORM->CkANNOx.ckSize > 0) {
    WHEAD_S (fp, CkFORM->CkANNOx.ckID);
    WHEAD_V (fp, CkFORM->CkANNOx.ckSize, DS_EB);
    WHEAD_S (fp, CkFORM->CkANNOx.InfoID);        /* AFsp identifier */
    WHEAD_SN (fp, CkFORM->CkANNOx.text, CkFORM->CkANNOx.ckSize - 4);
    WRPAD (fp, CkFORM->CkANNOx.ckSize, ALIGN);
  }

  /* SSND chunk preamble */
  WHEAD_S (fp, CkFORM->CkSSND.ckID);
  WHEAD_V (fp, CkFORM->CkSSND.ckSize, DS_EB);
  WHEAD_V (fp, CkFORM->CkSSND.offset, DS_EB);
  WHEAD_V (fp, CkFORM->CkSSND.blockSize, DS_EB);

  return;
}

/* Write a P-string, returning the full length */
/* P-string:
   - number of characters in string (stored in a byte)
   - string
   - trailing null to make the overall size even
   Note:
     If the file pointer is NULL, the length of the string string that would
     have been written is returned.
*/


static int
AF_wrPstring (FILE *fp, const char string[])

{
  unsigned char slen;
  int nc, Nbytes;

  nc = (int) strlen (string);
  Nbytes = RNDUPV (nc+1, ALIGN);

  if (fp != NULL) {
    slen = nc;
    WHEAD_V (fp, slen, DS_EB);
    WHEAD_SN (fp, string, nc);
    WRPAD (fp, nc + 1, ALIGN);
  }

  return Nbytes;
}

/* Check the loudspeaker locations */


static int
AF_checkAISpeaker (const unsigned char *SpkrConfig, int Nchan)

{
  int Nspkr, i, Nconfig, Err;

  /* Standard speaker configurations per AIFF-C documentation */
  static const char *AI_SpkrConfig[] = {
    "\x01\x02",                 /* FL FR */
    "\x01\x02\x03",             /* FL FR FC */
    "\x01\x02\x05\x06",         /* FL FR BL BR */
    "\x01\x03\x02\x09",         /* FL FC FR BC */
    "\x01\x07\x03\x02\x08\x09"  /* FL FLc FC FR FRc BC */
  };

  if (SpkrConfig[0] == '\0')
    Nspkr = 0;
  else
    Nspkr = (int) strlen ((const char *) SpkrConfig);

  if (Nchan == 1 || Nspkr == 0)
    Err = 0;
  else {
    Err = 1;
    if (Nchan == Nspkr) {
      Nconfig = NELEM (AI_SpkrConfig);
      for (i = 0; i < Nconfig; ++i) {
        if (strcmp ((const char *) SpkrConfig, AI_SpkrConfig[i]) == 0) {
          Err = 0;
          break;
        }
      }
    }
  }

  if (Err)
    UTwarn ("AFwrAIhead - %s", AFM_AIFFX_BadSpkr);

  return Err;
}
