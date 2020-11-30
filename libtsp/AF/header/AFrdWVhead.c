/*-------------- Telecommunications & Signal Processing Lab ---------------

Routine:
  AFILE *AFrdWVhead(FILE *fp)

Purpose:
  Get file format information from a WAVE file

Description:
  This routine reads the header for a WAVE file. The header information is used
  to set the file data format information in the audio file pointer structure.

  WAVE file:
   Offset Length Type    Contents
      0     4    char   "RIFF" file identifier
      4     4    int    Chunk length
      8     4    char   "WAVE" file identifier
    ...   ...    ...    ...
      F     4    char   "fmt " chunk identifier
     +4     4    int    Chunk length
     +8     2    int      Audio data type
    +10     2    int      Number of interleaved channels
    +12     4    int      Sample rate
    +16     4    int      Average bytes/sec
    +20     2    int      Block align
    +22     2    int      Data word length (bits)
    +24     2    int      Extra data count (for non-PCM or extensible format)
    +26     2    int        Valid bits per sample
    +28     4    int        Channel mask
    +32    16    int        Sub-format type
    ...    ...   ...
      C     4    char   "fact" chunk identifier (for non-PCM)
     +4     4    int    Chunk length
     +8     4    int      Number of samples (per channel)
    ...    ...   ...    ...
      D     4    char   "data" chunk identifier
     +4     4    int    Chunk length
     +8    ...   ...      Audio data

  For WAVE files text information can appear in several different chunks. This
  information is extracted and stored in the audio file parameter structure.
  - "afsp" chunk text is extracted and stored as individual information
    records.
  - "DISP" chunk text is stored as a "display_text:" information record.
  - "LIST/INFO" chunk subcategories as follows:
    - "IARL" text is stored as an "archival_location:" information record.
    - "IART" text is stored as an "artist:" information record.
    - "ICMS" text is stored as a "commissioned:" information record.
    - "ICMT" text is stored as a "comment:" information record.
    - "ICOP" text is stored as a "copyright:" information record.
    - "ICRD" text is stored as a "creation_date:" information record.
    - "IENG" text is stored as an "engineer:" information record.
    - "IGNR" text is stored as a "genre:" information record.
    - "IKEY" text is stored as a "keywords:" information record.
    - "IMED" text is stored as a "medium:" information record.
    - "INAM" text is stored as a "title:" information record.
    - "IPRD" text is stored as a "product:" information record.
    - "ISBJ" text is stored as a "subject:" information record.
    - "ISFT" text is stored as a "software:" information record.
    - "ISRC" text is stored as a "source:" information record.
    - "ISRF" text is stored as a "source_form:" information record.
    - "ITCH" text is stored as a "technician:" information record.

Parameters:
  <-  AFILE *AFrdWVhead
      Audio file pointer for the audio file
   -> FILE *fp
      File pointer for the file

Author / revision:
  P. Kabal  Copyright (C) 2020
  $Revision: 1.127 $  $Date: 2020/11/26 11:29:47 $

-------------------------------------------------------------------------*/

#include <libtsp/sysOS.h>
#if (SY_OS == SY_OS_WINDOWS)
#  define _CRT_SECURE_NO_WARNINGS     /* Allow sprintf */
#endif

#include <setjmp.h>
#include <string.h>

#include <libtsp.h>
#include <AFpar.h>
#include <libtsp/AFdataio.h>
#include <libtsp/AFheader.h>
#include <libtsp/AFinfo.h>
#include <libtsp/AFmsg.h>
#define WV_INFO_REC
#define WV_SPKR
#include <libtsp/WVpar.h>
#include <libtsp/nucleus.h>

#define WV_FMT_MINSIZE  16
#define WV_LHMIN  (8 + 4 + 8 + WV_FMT_MINSIZE + 8)

/* setjmp / longjmp environment */
extern jmp_buf AFR_JMPENV;

AF_READ_DEFAULT(AFr_default); /* Define the AF_read defaults */

/* Local functions */
static void
AF_addInfoRec(const char Ident[], const char text[], int Size,
              struct AF_info *AFInfo);
static int
AF_decFMT(const struct WV_Ckfmt *Ckfmt, struct AF_read *AFr);
static int
AF_rdbext(FILE *fp, int Size, struct AF_info *RInfo);
static int
AF_rdDISP_text(FILE *fp, int Size, struct AF_info *RInfo);
static int
AF_rdFMT(FILE *fp, struct WV_Ckfmt *Ckfmt);
static int
AF_rdFACT(FILE *fp, struct WV_Ckfact *Ckfact);
static int
AF_rdLIST_INFO(FILE *fp, int Size, struct AF_info *RInfo, long int pos,
               struct AF_chunkInfo *ChunkInfo);
static int
AF_rdRIFF_WAVE(FILE *fp, struct WV_CkRIFF *CkRIFF);
static void
AF_UnsFormat(int FormatTag);
static void
AF_decChannelConfig(UT_uint4_t ChannelMask, unsigned char *SpkrConfig);


AFILE *
AFrdWVhead(FILE *fp)

{
  AFILE *AFp;
  int AtData, Ftype, BWF;
  long int offs, poffs, LRIFF, Fact_Nframe, Dstart, EoD;
  char Info[AF_MAXINFO];
  struct WV_CkRIFF CkRIFF;
  struct WV_Ckpreamb CkHead;
  struct AF_read AFr;

/* Set the long jump environment; on error return a NULL */
  if (setjmp(AFR_JMPENV))
    return NULL;  /* Return from a header read error */

/* Defaults and initial values */
  AFr = AFr_default;
  AFr.RInfo.Info = Info;
  AFr.RInfo.N = 0;
  AFr.RInfo.Nmax = sizeof(Info);
  BWF = 0;

  Fact_Nframe = AF_NFRAME_UNDEF;

/* Check the file magic for a RIFF/WAVE file */
  if (AF_rdRIFF_WAVE(fp, &CkRIFF))
    return NULL;
  offs = 12L; /* Positioned after RIFF/WAVE preamble */
  LRIFF = CkRIFF.ckSize + 8;
  AFsetChunkLim(ckID_RIFF, 0, LRIFF, &AFr.ChunkInfo);
  AFsetChunkLim(FM_WAVE, 8, offs, &AFr.ChunkInfo);

  Dstart = 0L;
  EoD = 0L;
  AtData = 0;
  while (offs < LRIFF-8) {  /* Leave 8 bytes for the chunk preamble */

    poffs = offs;     /* Position at start of chunk */

    /* Read the chunk preamble */
    offs += RHEAD_S(fp, CkHead.ckID);

    /* fmt chunk */
    if (SAME_CSTR(CkHead.ckID, ckID_fmt)) {
      offs += AF_rdFMT(fp, &CkRIFF.Ckfmt);
      if (AF_decFMT(&CkRIFF.Ckfmt, &AFr))
        return NULL;
      AFsetChunkLim(CkHead.ckID, poffs, offs, &AFr.ChunkInfo);
    }

    /* fact chunk */
    else if (SAME_CSTR(CkHead.ckID, ckID_fact)) {
      offs += AF_rdFACT(fp, &CkRIFF.Ckfact);
      Fact_Nframe = CkRIFF.Ckfact.dwSampleLength;
      AFsetChunkLim(CkHead.ckID, poffs, offs, &AFr.ChunkInfo);
    }

    /* data chunk */
    else if (SAME_CSTR(CkHead.ckID, ckID_data)) {
      offs += RHEAD_V(fp, CkRIFF.Ckdata.ckSize, DS_EL);
      AFr.NData.Ldata = CkRIFF.Ckdata.ckSize;
      Dstart = offs;
      EoD = RNDUPV(Dstart + AFr.NData.Ldata, ALIGN);
      AFsetChunkLim(CkHead.ckID, poffs, EoD, &AFr.ChunkInfo);
      if (EoD >= LRIFF || !FLseekable(fp)) {
        AtData = 1;
        break;
      }
      else {
        AtData = 0;
        offs += RSKIP(fp, EoD - Dstart);
      }
    }

    /* BWF bext chunk */
    else if (SAME_CSTR(CkHead.ckID, ckID_bext)) {
      offs += RHEAD_V(fp, CkHead.ckSize, DS_EL);
      offs += AF_rdbext(fp, (int) CkHead.ckSize, &AFr.RInfo);
      AFsetChunkLim(CkHead.ckID, poffs, offs, &AFr.ChunkInfo);
      BWF = 1;
    }

    /* Text chunks */
    else if (SAME_CSTR(CkHead.ckID, ckID_afsp)) {
      offs += RHEAD_V(fp, CkHead.ckSize, DS_EL);
      offs += AFrdInfoAFspText(fp, (int) CkHead.ckSize, &AFr.RInfo, ALIGN);
      AFsetChunkLim(CkHead.ckID, poffs, offs, &AFr.ChunkInfo);
    }
    else if (SAME_CSTR(CkHead.ckID, ckID_DISP)) {
      offs += RHEAD_V(fp, CkHead.ckSize, DS_EL);
      offs += AF_rdDISP_text(fp, (int) CkHead.ckSize, &AFr.RInfo);
      AFsetChunkLim(CkHead.ckID, poffs, offs, &AFr.ChunkInfo);
    }
    else if (SAME_CSTR(CkHead.ckID, ckID_LIST)) {
      offs += RHEAD_V(fp, CkHead.ckSize, DS_EL);
      AFsetChunkLim(CkHead.ckID, poffs, RNDUPV(offs + CkHead.ckSize, ALIGN),
                    &AFr.ChunkInfo);
      offs += AF_rdLIST_INFO(fp, (int) CkHead.ckSize, &AFr.RInfo, offs,
                             &AFr.ChunkInfo);
    }
    /* Miscellaneous chunks */
    else {
      offs += RHEAD_V(fp, CkHead.ckSize, DS_EL);
      offs += RSKIP(fp, RNDUPV(CkHead.ckSize, ALIGN));
      AFsetChunkLim(CkHead.ckID, poffs, offs, &AFr.ChunkInfo);
    }
  }
  /* Error Checks */
  /* Check that we found a fmt and a data chunk */
  if (AFr.DFormat.Format == FD_UNDEF || AFr.NData.Ldata == AF_LDATA_UNDEF) {
    UTwarn("AFrdWVhead - %s", AFM_WV_BadHead);
    return NULL;
  }
  if ((!AtData && offs != LRIFF) || (AtData && EoD != LRIFF))
    UTwarn("AFrdWVhead - %s", AFM_WV_BadSize);

  /* Use the number of samples from the fact chunk only for non-PCM or WAVE-EX
     files. Some PCM files use the fact chunk, but it does NOT specify the
     number of samples */
  if (CkRIFF.Ckfmt.wFormatTag != WAVE_FORMAT_PCM &&
      Fact_Nframe != AF_NFRAME_UNDEF)
    AFr.NData.Nsamp = AFr.NData.Nchan * Fact_Nframe;

  /* Position at the start of data */
  if (!AtData) {
    if (AFseek(fp, Dstart, NULL))
      return NULL;
  }

/* Set the parameters for file access */
  if (CkRIFF.Ckfmt.wFormatTag == WAVE_FORMAT_EXTENSIBLE) {
    if (BWF)
      Ftype = FT_WAVE_EX_BWF;
    else
      Ftype = FT_WAVE_EX;
  }
  else {
    if (BWF)
      Ftype = FT_WAVE_BWF;
    else
      Ftype = FT_WAVE;
  }
  AFp = AFsetRead(fp, Ftype, &AFr, AF_FIX_NSAMP_HIGH + AF_FIX_LDATA_HIGH);

  return AFp;
}

/* Check the RIFF/WAVE file preamble */


static int
AF_rdRIFF_WAVE(FILE *fp, struct WV_CkRIFF *CkRIFF)

{
  long int Lfile, LRIFF;

  RHEAD_S(fp, CkRIFF->ckID);
  if (!SAME_CSTR(CkRIFF->ckID, ckID_RIFF)) {
    UTwarn("AFrdWVhead - %s", AFM_WV_BadId);
    return 1;
  }

  RHEAD_V(fp, CkRIFF->ckSize, DS_EL);
  LRIFF = CkRIFF->ckSize + 8;
  if (LRIFF < WV_LHMIN) {
    UTwarn("AFrdWVhead - %s", AFM_WV_BadRIFF);
    return 1;
  }

  if (FLseekable(fp)) {
    Lfile = FLfileSize(fp);
    if (LRIFF > Lfile) {
      CkRIFF->ckSize = Lfile - 8;
      UTwarn("AFrdWVhead - %s", AFM_WV_FixRIFF);
    }
  }

  RHEAD_S(fp, CkRIFF->WAVEID);
  if (!SAME_CSTR(CkRIFF->WAVEID, FM_WAVE)) {
    UTwarn("AFrdWVhead - %s", AFM_WV_BadId);
    return 1;
  }

  return 0;
}

/* Read the fmt chunk, starting at ckSize */


static int
AF_rdFMT(FILE *fp, struct WV_Ckfmt *Ckfmt)

{
  int offs, NB;

  offs = RHEAD_V(fp, Ckfmt->ckSize, DS_EL);
  if (Ckfmt->ckSize < WV_FMT_MINSIZE) {
    UTwarn("AFrdWVhead - %s", AFM_WV_BadfmtSize);
    longjmp(AFR_JMPENV, 1);
  }

  offs += RHEAD_V(fp, Ckfmt->wFormatTag, DS_EL);
  offs += RHEAD_V(fp, Ckfmt->nChannels, DS_EL);
  offs += RHEAD_V(fp, Ckfmt->nSamplesPerSec, DS_EL);
  offs += RHEAD_V(fp, Ckfmt->nAvgBytesPerSec, DS_EL);
  offs += RHEAD_V(fp, Ckfmt->nBlockAlign, DS_EL);
  offs += RHEAD_V(fp, Ckfmt->wBitsPerSample, DS_EL);

  NB = ((int) Ckfmt->ckSize + 4) - offs;
  if (NB >= 24) {
    offs += RHEAD_V(fp, Ckfmt->cbSize, DS_EL);
    if (Ckfmt->cbSize >= 22) {
      offs += RHEAD_V(fp, Ckfmt->wValidBitsPerSample, DS_EL);
      offs += RHEAD_V(fp, Ckfmt->dwChannelMask, DS_EL);
      offs += RHEAD_V(fp, Ckfmt->SubFormat.wFormatTag, DS_EL);
      offs += RHEAD_S(fp, Ckfmt->SubFormat.guidx);
    }
    else
      Ckfmt->cbSize = 0;
  }

  /* Skip over any extra data at the end of the fmt chunk */
  offs += RSKIP(fp, RNDUPV(Ckfmt->ckSize + 4, ALIGN) - offs);

  return offs;
}

/* Decode the data format */


static int
AF_decFMT(const struct WV_Ckfmt *Ckfmt, struct AF_read *AFr)

{
  UT_uint2_t FormatTag;
  int NBytesS;

/*
  Ordinary WAVE (not EXTENSIBLE format) files
  - The data format is over-specified
    - The number of bits per sample is wBitsPerSample.
    - The number of bytes per sample is NByteS = nBlockAlign/nChannels.
  - A reasonable approach is to get the container size from the block size
    and the data size from wBitsPerSample.
  - The CoolEdit float format specifies float data (flagged as 24 bits per
    sample, in a 4-byte container)
  - In calculating the container size, it is expected that nBlockAlign be a
    multiple of the number of channels. Note, however, that some compressed
    schemes set nBlockAlign to 1.
  WAVE EXTENSIBLE files
  - wBitsPerSample now explicitly is the container size in bits and hence
    should agree with 8 * NByteS, and nBlockAlign must be a multiple of
    nChannels.
*/
  if (Ckfmt->nChannels == 0)
    NBytesS = 0;
  else
    NBytesS = (int) (Ckfmt->nBlockAlign / Ckfmt->nChannels);

  if (Ckfmt->wFormatTag == WAVE_FORMAT_EXTENSIBLE) {
    if (Ckfmt->cbSize < 22 || !IS_VALID_WAVEFORMATEX_GUID(Ckfmt->SubFormat)) {
      UTwarn("AFrdWVhead - %s", AFM_WV_BadfmtEx);
      return 1;
    }
    /* Extensible file: container size must match wBitsPerSample */
    AFr->DFormat.NbS = 8 * NBytesS;
    if (Ckfmt->wBitsPerSample != 8 * NBytesS) {
      UTwarn("AFrdWVhead - %s: %d <-> %d", AFM_WV_BadSS,
             (int) Ckfmt->wBitsPerSample, 8 * NBytesS);
    }
    AFr->DFormat.NbS = (int) Ckfmt->wValidBitsPerSample;
    FormatTag = Ckfmt->SubFormat.wFormatTag;
    AF_decChannelConfig(Ckfmt->dwChannelMask, AFr->NData.SpkrConfig);
  }
  else {
    AFr->DFormat.NbS = (int) Ckfmt->wBitsPerSample;
    FormatTag = Ckfmt->wFormatTag;
  }

  switch (FormatTag) {
  case WAVE_FORMAT_PCM:

    if (NBytesS == FDL_INT16)
      AFr->DFormat.Format = FD_INT16;
    /* Special case: IEEE float from CoolEdit (samples are flagged as 24-bit
       integer, but are 32-bit IEEE float) */
    else if (Ckfmt->wFormatTag == WAVE_FORMAT_PCM &&
             AFr->DFormat.NbS == 24 && NBytesS == 4) {
      AFr->DFormat.Format = FD_FLOAT32;
      AFr->DFormat.NbS = 8 * FDL_FLOAT32;
      AFr->DFormat.FullScale = AF_FULLSCALE_INT24;  /* Same scaling as INT24 */
      if (!UTcheckIEEE())
        UTwarn("AFrdAIhead - %s", AFM_NoIEEE);
    }
    else if (NBytesS == FDL_INT24)
      AFr->DFormat.Format = FD_INT24;
    else if (NBytesS == FDL_INT32)
      AFr->DFormat.Format = FD_INT32;
    else if (NBytesS == FDL_UINT8)
      AFr->DFormat.Format = FD_UINT8;
    else {
      UTwarn("AFrdWVhead - %s: \"%d\"", AFM_WV_UnsDSize, 8 * NBytesS);
      return 1;
    }
    break;

  case WAVE_FORMAT_MULAW:
    if (NBytesS != 1) {
      UTwarn("AFrdWVhead - %s: \"%d\"", AFM_WV_BadMulaw, 8 * NBytesS);
      return 1;
    }
    if (AFr->DFormat.NbS != 8) {
      UTwarn("AFrdWVhead - %s: \"%d\"", AFM_WV_BadMulaw, AFr->DFormat.NbS);
      AFr->DFormat.NbS = 8;
    }
    AFr->DFormat.Format = FD_MULAW8;
    break;

  case WAVE_FORMAT_ALAW:
    if (NBytesS != 1) {
      UTwarn("AFrdWVhead - %s: \"%d\"", AFM_WV_BadAlaw, 8 * NBytesS);
      return 1;
    }
    if (AFr->DFormat.NbS != 8) {
      UTwarn("AFrdWVhead - %s: \"%d\"", AFM_WV_BadAlaw, AFr->DFormat.NbS);
      AFr->DFormat.NbS = 8;
    }
    AFr->DFormat.Format = FD_ALAW8;
    break;

  case WAVE_FORMAT_IEEE_FLOAT:
    if (NBytesS == FDL_FLOAT32)
      AFr->DFormat.Format = FD_FLOAT32;
    else if (NBytesS == FDL_FLOAT64)
      AFr->DFormat.Format = FD_FLOAT64;
    else
      UTwarn("AFrdWVhead - %s: \"%d\"", AFM_WV_BadFloat, 8 * NBytesS);
    if (!UTcheckIEEE())
      UTwarn("AFrdWVhead - %s", AFM_NoIEEE);
    break;

  /* Unsupported data formats */
  default:
    AF_UnsFormat((int) Ckfmt->wFormatTag);
    return 1;
  }

  AFr->NData.Nchan = (long int) Ckfmt->nChannels;

  /* Error checks */
  if (AFr->NData.Nchan <= 0) {
    UTwarn("AFrdWVhead - %s : %d", AFM_WV_BadNchan, Ckfmt->nChannels);
    return 1;
  }
  if ((Ckfmt->nBlockAlign % Ckfmt->nChannels) != 0) {
    UTwarn("AFrdWVhead - %s: %d", AFM_WV_BadBlock, Ckfmt->nBlockAlign);
    return 1;
  }
  if (AFr->DFormat.NbS > 8 * NBytesS) {
    UTwarn("AFrdWVhead - %s: %d", AFM_WV_BadNbS, AFr->DFormat.NbS);
    AFr->DFormat.NbS = 8 * NBytesS;
  }

  /* Consistency check */
  if (Ckfmt->nAvgBytesPerSec != Ckfmt->nChannels * Ckfmt->nSamplesPerSec *
      NBytesS)
    UTwarn("AFrdWVhead - %s", AFM_WV_BadBytesSec);

  AFr->DFormat.Swapb = DS_EL;
  AFr->Sfreq = (double) Ckfmt->nSamplesPerSec;

  return 0;
}

/* Read the fact chunk */


static int
AF_rdFACT(FILE *fp, struct WV_Ckfact *Ckfact)

{
  int offs;

  offs = RHEAD_V(fp, Ckfact->ckSize, DS_EL);
  if (Ckfact->ckSize < 4) {
    UTwarn("AFrdWVhead - %s", AFM_WV_BadFACT);
    longjmp(AFR_JMPENV, 1);
  }
  offs += RHEAD_V(fp, Ckfact->dwSampleLength, DS_EL);
  offs += RSKIP(fp, RNDUPV(Ckfact->ckSize + 4, ALIGN) - offs);

  return offs;
}

/* Unsupported data format */


static void
AF_UnsFormat(int FormatTag)

{
  switch (FormatTag) {
  case WAVE_FORMAT_ADPCM:
    UTwarn("AFrdWVhead - %s: \"%s\"", AFM_WV_UnsData, WV_FD_ADPCM);
    break;
  case WAVE_FORMAT_IMA_ADPCM:
    UTwarn("AFrdWVhead - %s: \"%s\"", AFM_WV_UnsData, WV_FD_IMA_ADPCM);
    break;
  case WAVE_FORMAT_DSPGROUP_TRUESPEECH:
    UTwarn("AFrdWVhead - %s: \"%s\"", AFM_WV_UnsData,
           WV_FD_DSPGROUP_TRUESPEECH);
    break;
  case WAVE_FORMAT_GSM610:
    UTwarn("AFrdWVhead - %s: \"%s\"", AFM_WV_UnsData, WV_FD_GSM610);
    break;
  case WAVE_FORMAT_MSG723:
    UTwarn("AFrdWVhead - %s: \"%s\"", AFM_WV_UnsData, WV_FD_MSG723);
    break;
  case WAVE_FORMAT_MPEGLAYER3:
    UTwarn("AFrdWVhead - %s: \"%s\"", AFM_WV_UnsData, WV_FD_MPEGLAYER3);
    break;
  default:
    UTwarn("AFrdWVhead - %s: \"0x%04X\"", AFM_WV_UnsData, FormatTag);
    break;
  }
}

/* Read a bext chunk */


static int
AF_rdbext(FILE *fp, int Size, struct AF_info *RInfo)

{
  int offs, nc;
  char buff[256];
  struct WV_Ckbext Ckbext;

  offs  = RHEAD_S(fp, Ckbext.Description);
  offs += RHEAD_S(fp, Ckbext.Originator);
  offs += RHEAD_S(fp, Ckbext.OriginatorReference);
  offs += RHEAD_S(fp, Ckbext.OriginatorDate);
  offs += RHEAD_S(fp, Ckbext.OriginatorTime);
  offs += RHEAD_V(fp, Ckbext.TimeReferenceLow, DS_EL);
  offs += RHEAD_V(fp, Ckbext.TimeReferenceHigh, DS_EL);
  offs += RHEAD_V(fp, Ckbext.Version, DS_EL);
  offs += RHEAD_S(fp, Ckbext.UMID);
  offs += RHEAD_V(fp, Ckbext.LoudnessValue, DS_EL);
  offs += RHEAD_V(fp, Ckbext.LoudnessRange, DS_EL);
  offs += RHEAD_V(fp, Ckbext.MaxTruePeakLevel, DS_EL);
  offs += RHEAD_V(fp, Ckbext.MaxMomentaryLoudness, DS_EL);
  offs += RHEAD_V(fp, Ckbext.MaxShortTermLoudness, DS_EL);
  offs += RSKIP(fp, sizeof(Ckbext.Reserved));
  if (offs < Size)
    offs += RSKIP(fp, RNDUPV(Size, ALIGN) - offs);

/* Return selected values as info records */
  AF_addInfoRec(BW_RecID_Description, Ckbext.Description,
                sizeof(Ckbext.Description), RInfo);
  AF_addInfoRec(BW_RecID_Originator, Ckbext.Originator,
                sizeof(Ckbext.Originator), RInfo);
  AF_addInfoRec(BW_RecID_OriginatorDate, Ckbext.OriginatorDate,
                sizeof(Ckbext.OriginatorDate), RInfo);
  AF_addInfoRec(BW_RecID_OriginatorTime, Ckbext.OriginatorTime,
                sizeof(Ckbext.OriginatorTime), RInfo);
  nc = sprintf(buff, "%d", Ckbext.Version);
  AF_addInfoRec(BW_RecID_Version, buff, nc, RInfo);

  if (Ckbext.Version >= 1) {
    AF_addInfoRec(BW_RecID_UMID, Ckbext.UMID, sizeof(Ckbext.UMID), RInfo);
  }
  if (Ckbext.Version >= 2) {
    if (Ckbext.LoudnessValue >= -9999 &&
        Ckbext.LoudnessValue <=  9999) {
      nc = sprintf(buff, "%.2f LUFS", (double) Ckbext.LoudnessValue / 100);
      AF_addInfoRec(BW_RecID_LoudnessValue, buff, nc, RInfo);
    }
    if (Ckbext.LoudnessRange >= 0 &&
        Ckbext.LoudnessRange <= 9999) {
      nc = sprintf(buff, "%.2f LU", (double) Ckbext.LoudnessRange / 100);
      AF_addInfoRec(BW_RecID_LoudnessRange, buff, nc, RInfo);
    }
    if (Ckbext.MaxTruePeakLevel >= -9999 &&
        Ckbext.MaxTruePeakLevel <=  9999) {
      nc = sprintf(buff, "%.2f dBTP", (double) Ckbext.MaxTruePeakLevel / 100);
      AF_addInfoRec(BW_RecID_MaxTruePeakLevel, buff, nc, RInfo);
    }
    if (Ckbext.MaxMomentaryLoudness >= -9999 &&
        Ckbext.MaxMomentaryLoudness <=  9999) {
      nc = sprintf(buff, "%.2f LUFS",
                  (double) Ckbext.MaxMomentaryLoudness / 100);
      AF_addInfoRec(BW_RecID_MaxMomentaryLoudness, buff, nc, RInfo);
    }
    if (Ckbext.MaxShortTermLoudness >= -9999 &&
        Ckbext.MaxShortTermLoudness <=  9999) {
      nc = sprintf(buff, "%.2f LUFS",
                   (double) Ckbext.MaxShortTermLoudness / 100);
      AF_addInfoRec(BW_RecID_MaxShortTermLoudness, buff, nc, RInfo);
    }
  }

  return offs;
}


/* Add an information record */


static void
AF_addInfoRec(const char Ident[], const char text[], int Size,
              struct AF_info *AFInfo)

{
  char *p;

  /* text is fixed length (Size), if shorter than Size, it is NUL terminated */
  p = memchr(text, '\0', Size);
  if (p != NULL)
    Size = (int) (p - text);

  if (Size == 0)
    return;

  AFaddInfoRec(Ident, text, Size, AFInfo);
}

/* Read a DISP chunk */


static int
AF_rdDISP_text(FILE *fp, int Size, struct AF_info *RInfo)

{
  int offs;
  UT_uint4_t DISP_ID;

  /* The DISP chunk can hold any of the MS clipboard formats.
       CF_TEXT: Text format. Each line ends with a carriage return/linefeed
       (CR-LF) combination. A null character signals the end of the data. Use
       this format for ANSI text.
     We will ignore the DISP chunk if the identifier is not CF_TEXT.
  */
  offs = RHEAD_V(fp, DISP_ID, DS_EL);
  if (DISP_ID == CF_TEXT)
    offs += AFrdInfoIdentText(fp, Size - offs, WV_DISPid[0], RInfo, ALIGN);
  else
    offs += RSKIP(fp, RNDUPV(Size, ALIGN) - offs);

  return offs;
}

/* Read the LIST-INFO records from the header */


static int
AF_rdLIST_INFO(FILE *fp, int Size, struct AF_info *RInfo, long int pos,
               struct AF_chunkInfo *ChunkInfo)

{
  int k, offs;
  long int poffs;
  char ID[4], key[7];
  struct WV_Ckpreamb CkHead;

  offs = RHEAD_S(fp, ID);
  if (SAME_CSTR(ID, FM_INFO)) {
    AFsetChunkLim(FM_INFO, pos, pos + offs, ChunkInfo);

    poffs = offs;
    while (offs < Size) {
      offs += RHEAD_S(fp, CkHead.ckID);
      offs += RHEAD_V(fp, CkHead.ckSize, DS_EL);
      AFsetChunkLim(CkHead.ckID, pos + poffs,
                    RNDUPV(pos + offs + CkHead.ckSize, ALIGN), ChunkInfo);

      /* Look for standard INFO ID values */
      for (k = 0; k < N_LIMAP; ++k) {
        if (SAME_CSTR(CkHead.ckID, WV_LImap[k].ckID)) {
          offs += AFrdInfoIdentText(fp, (int) CkHead.ckSize,
                                    WV_LImap[k].RecID[0], RInfo, ALIGN);
          break;
        }
      }

      /* No match, use the INFO ID field as the information record keyword */
      /* Example: key = "IXXX: \0", where IXXX is from CkHead.ckID */
      if (k == N_LIMAP) {
        STcopyNMax(CkHead.ckID, key, 4, (int) sizeof(key) - 1);
        STcatMax(": ", key, (int) sizeof(key) - 1);
        offs += AFrdInfoIdentText(fp, (int) CkHead.ckSize, key, RInfo, ALIGN);
      }
      poffs = offs;
    }
  }
  offs += RSKIP(fp, RNDUPV(Size, ALIGN) - offs);

  return offs;
}

/* Decode channel/speaker information */


static void
AF_decChannelConfig(UT_uint4_t ChannelMask, unsigned char *SpkrConfig)

{
  int m, k;

  k = 0;
  if (ChannelMask & WV_SPKR_ALL)
    ;
  else if (ChannelMask & ~WV_SPKR_KNOWN)
    UTwarn("AFrdWVhead - %s", AFM_WV_UnkChannel);
  else if (ChannelMask != 0) {
    for (m = 0; m < WV_N_SPKR_MASK; ++m) {
      if (ChannelMask & WV_Spkr_Mask[m]) {
        SpkrConfig[k] = WV_AF_Spkr_Index[m];
        ++k;
      }
    }
  }
  SpkrConfig[k] = '\0';
}
