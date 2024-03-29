/*-------------- Telecommunications & Signal Processing Lab ---------------

Routine:
  AFILE *AFrdNShead(FILE *fp)

Purpose:
  Get file format information from an NSP file

Description:
  This routine reads the header for an NSP file. The header information is used
  to set the file data format information in the audio file pointer structure.
  This routine sets up for reading of the first of the SD_A, SD_B, or SDAB data
  chunks. Other data chunks are ignored.

  WAVE file:
   Offset Length Type    Contents
      0     4    char   "FORM" file identifier
      4     4    char   "DS16" file identifier
      8     4    int    Block length (following chunks + data)
      H     4    char   "HEDR" or "HDR8" chunk identifier
     +4     4    int    Chunk length (32)
     +8    20    char     Date ("May 12 12:34:45 2001")
    +28     4    int      Sample rate
    +32     4    int      Length (bytes)
    +36     2    int      Max abs value for channel A
    +38     2    int      Max abs value for channel B
      N     4    char   "NOTE" chunk identifier
     +4     4    int      Chunk length
     +8     n    char     Comment string
     SA     4    char   "SDA_" "SD_B" or "SDAB" chunk identifier
     +4     4    int      Chunk length
     +8     4    int      Data length (bytes) for channel A, B or A and B
      D   ...    ...    Audio data

  For NSP files text information as a "NOTE" chunk in the header. This
  information is stored as a "comment:" information record in the audio file
  parameter structure.

Parameters:
  <-  AFILE *AFrdNShead
      Audio file pointer for the audio file
   -> FILE *fp
      File pointer for the file

Author / revision:
  P. Kabal  Copyright (C) 2020
  $Revision: 1.28 $  $Date: 2020/11/26 11:29:47 $

-------------------------------------------------------------------------*/

#include <libtsp/sysOS.h>
#if (SY_OS == SY_OS_WINDOWS)
#  define _CRT_SECURE_NO_WARNINGS     /* Allow sprintf */
#endif

#include <setjmp.h>
#include <string.h>

#include <libtsp.h>
#include <AFpar.h>
#include <libtsp/nucleus.h>     /* FLseekable */
#include <libtsp/AFdataio.h>
#include <libtsp/AFheader.h>
#include <libtsp/AFinfo.h>
#include <libtsp/AFmsg.h>
#include <libtsp/UTtypes.h>

#define ALIGN   2 /* Chunks padded out to a multiple of ALIGN */

#define NS_HEDR_SIZE      32
#define NS_LHMIN          (8 + 4 + 8 + NS_HEDR_SIZE + 8)
#define NS_MAXABS_UNDEF   (UT_UINT2_MAX)

/* setjmp / longjmp environment */
extern jmp_buf AFR_JMPENV;

AF_READ_DEFAULT(AFr_default); /* Define the AF_read defaults */

/* NSP files are created by the Computerized Speech Lab software. Files in this
   format are also used by STR for their PDB phonetic database. Information as
   to the format has been gleaned from the sources for WaveSurfer and from the
   file headers themselves.
*/

struct NS_CkPreamb {
  char ckID[4];
  UT_uint4_t ckSize;
};

struct NS_CkFORM {
  char ckID[8];
  UT_uint4_t Size;
};

struct NS_CkHEDR {
  char ckID[4];
  UT_uint4_t ckSize;
/* char Date[20]; */
  UT_uint4_t Srate;
  UT_uint4_t Nsamp;
  UT_uint2_t MaxAbsA;
  UT_uint2_t MaxAbsB;
};

static int
AF_rdFORM_DS16(FILE *fp, struct NS_CkFORM *CkFORM);
static int
AF_rdHEDR(FILE *fp, struct NS_CkHEDR *CkHEDR, struct AF_info *RInfo);
static void
AF_setIval(const char Ident[], int Ival, struct AF_info *RInfo);


AFILE *
AFrdNShead(FILE *fp)

{
  AFILE *AFp;
  long int offs, poffs, LFORM;
  char Info[AF_MAXINFO];
  struct NS_CkFORM CkFORM;
  struct NS_CkHEDR CkHEDR;
  struct NS_CkPreamb CkHead;
  struct AF_read AFr;

/* Set the long jump environment; on error return a NULL */
  if (setjmp(AFR_JMPENV))
    return NULL;  /* Return from a header read error */

/* Defaults and initial values */
  AFr = AFr_default;
  AFr.RInfo.Info = Info;
  AFr.RInfo.N = 0;
  AFr.RInfo.Nmax = sizeof(Info);
  AFr.NData.Nchan = 0L;

/* Check the file magic for an NSP file */
  if (AF_rdFORM_DS16(fp, &CkFORM))
    return NULL;
  offs = 12L; /* Positioned after FORM/DS16 preamble */
  LFORM = CkFORM.Size + 12;
  AFsetChunkLim("FORM", 0, 4, &AFr.ChunkInfo);
  AFsetChunkLim("DS16", 4, RNDUPV(LFORM-8, ALIGN), &AFr.ChunkInfo);

  while (offs < LFORM) {

    poffs = offs;     /* Position at start of chunk */

    /* Read the chunk preamble */
    offs += RHEAD_S(fp, CkHead.ckID);

    /* HEDR or HDR8 chunk */
    if (SAME_CSTR(CkHead.ckID, "HEDR") || SAME_CSTR(CkHead.ckID, "HDR8")) {
      offs += AF_rdHEDR(fp, &CkHEDR, &AFr.RInfo);
      AFr.Sfreq = (double) CkHEDR.Srate;
      AFr.DFormat.Format = FD_INT16;
      AFr.DFormat.Swapb = DS_EL;
      AFr.NData.Nsamp = CkHEDR.Nsamp;
      if (CkHEDR.MaxAbsA != NS_MAXABS_UNDEF)
        AF_setIval("max_abs_A: ", CkHEDR.MaxAbsA, &AFr.RInfo);
      if (CkHEDR.MaxAbsB != NS_MAXABS_UNDEF)
        AF_setIval("max_abs_B: ", CkHEDR.MaxAbsB, &AFr.RInfo);
      AFsetChunkLim(CkHead.ckID, poffs, offs, &AFr.ChunkInfo);
    }

    /* NOTE chunk */
    else if (SAME_CSTR(CkHead.ckID, "NOTE")) {
      offs += RHEAD_V(fp, CkHead.ckSize, DS_EL);
      offs += AFrdInfoIdentText(fp, CkHead.ckSize, "comment:", &AFr.RInfo,
                                ALIGN);
     AFsetChunkLim(CkHead.ckID, poffs, offs, &AFr.ChunkInfo);
   }

    /* SDA_, SD_B or SDAB chunk */
    else if (SAME_CSTR(CkHead.ckID, "SDA_") ||
             SAME_CSTR(CkHead.ckID, "SD_B") ||
             SAME_CSTR(CkHead.ckID, "SDAB")) {
      offs += RHEAD_V(fp, CkHead.ckSize, DS_EL);
      if (SAME_CSTR(CkHead.ckID, "SDAB"))
        AFr.NData.Nchan = 2;
      else
        AFr.NData.Nchan = 1;
      AFr.NData.Ldata = CkHead.ckSize;
      AFsetChunkLim(CkHead.ckID, poffs, RNDUPV(CkHead.ckSize + 8, ALIGN),
                     &AFr.ChunkInfo);
      break;    /* Leave loop positioned at beginning of data */
    }

    /* Miscellaneous chunks */
    else {
      offs += RHEAD_V(fp, CkHead.ckSize, DS_EL);
      offs += RSKIP(fp, RNDUPV(CkHead.ckSize, ALIGN));
      AFsetChunkLim(CkHead.ckID, poffs, offs, &AFr.ChunkInfo);
    }
  }

  /* Check that we have found a HEDR/HDR8 and a SDxx chunk */
  if (AFr.DFormat.Format == FD_UNDEF || AFr.NData.Ldata == AF_LDATA_UNDEF) {
    UTwarn("AFrdNShead - %s", AFM_NS_BadHead);
    return NULL;
  }
  if (LFORM != offs + RNDUPV(AFr.NData.Ldata, ALIGN))
    UTwarn("AFrdNShead - %s", AFM_NS_FixFORM);

/* Set the parameters for file access */
  AFp = AFsetRead(fp, FT_NSP, &AFr, AF_NOFIX);

  return AFp;
}

/* Check the FORM/DS16 file preamble */


static int
AF_rdFORM_DS16(FILE *fp, struct NS_CkFORM *CkFORM)

{
  long int Lfile, LFORM;

  RHEAD_S(fp, CkFORM->ckID);
  if (!SAME_CSTR(CkFORM->ckID, "FORMDS16")) {
    UTwarn("AFrdNShead - %s", AFM_NS_BadId);
    return 1;
  }

  RHEAD_V(fp, CkFORM->Size, DS_EL);
  LFORM = CkFORM->Size + 12;
  if (LFORM < NS_LHMIN) {
    UTwarn("AFrdNShead - %s", AFM_NS_BadFORM);
    return 1;
  }

  if (FLseekable(fp)) {
    Lfile = FLfileSize(fp);
    if (LFORM > Lfile) {
      CkFORM->Size = Lfile - 12;
      UTwarn("AFrdNShead - %s", AFM_NS_FixFORM);
    }
  }

  return 0;
}

/* Read the HEDR chunk, starting at ckSize */


static int
AF_rdHEDR(FILE *fp, struct NS_CkHEDR *CkHEDR, struct AF_info *RInfo)

{
  int offs;

  offs = RHEAD_V(fp, CkHEDR->ckSize, DS_EL);
  if (CkHEDR->ckSize != NS_HEDR_SIZE) {
    UTwarn("AFrdNShead - %s", AFM_NS_BadHEDR);
    longjmp(AFR_JMPENV, 1);
  }

  offs += AFrdInfoIdentText(fp, 20, "date:", RInfo, ALIGN);
  offs += RHEAD_V(fp, CkHEDR->Srate, DS_EL);
  offs += RHEAD_V(fp, CkHEDR->Nsamp, DS_EL);
  offs += RHEAD_V(fp, CkHEDR->MaxAbsA, DS_EL);
  offs += RHEAD_V(fp, CkHEDR->MaxAbsB, DS_EL);

  /* Skip over any extra data at the end of the HEDR chunk */
  offs += RSKIP(fp, RNDUPV(CkHEDR->ckSize + 4, ALIGN) - offs);

  return offs;
}

/* Place max absolute values into the header */


static void
AF_setIval(const char Ident[], int Ival, struct AF_info *RInfo)

{
  int Nv;
  char str[20];

  Nv = sprintf(str, "%d", Ival);
  AFaddInfoRec(Ident, str, Nv, RInfo);
}
