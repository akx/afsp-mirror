/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  AFILE *AFrdBLhead(FILE *fp)

Purpose:
  Get file format information from an SPPACK file

Description:
  This routine reads the header for an SPPACK file. The header information is
  used to set the file data format information in the audio file pointer
  structure.

  SPPACK sampled data file:
   Offset Length Type    Contents
      0   160    char   Text strings (2 * 80)
    160    80    char   Command line
    240     2    int    Domain
    242     2    int    Frame size
    244     4    float  Sampling frequency
    252     2    int    File identifier
    254     2    int    Data type
    256     2    int    Resolution
    258     2    int    Companding
    260   240    char   Text strings (3 * 80)
    500     6    int    DAT-Link information (3 * 2)
    512   ...    --     Audio data

  For SPPACK files, text information is embedded in the header. This
  information (if not null) is stored as "comment:" and"command:" information
  records in the audio file parameter structure.

Parameters:
  <-  AFILE *AFrdBLhead
      Audio file pointer for the audio file
   -> FILE *fp
      File pointer for the file

Author / revision:
  P. Kabal  Copyright (C) 2020
  $Revision: 1.69 $  $Date: 2020/11/25 17:54:49 $

-------------------------------------------------------------------------*/

#include <ctype.h>
#include <setjmp.h>
#include <string.h>

#include <libtsp.h>
#include <AFpar.h>
#include <libtsp/nucleus.h>
#include <libtsp/AFheader.h>
#include <libtsp/AFmsg.h>
#include <libtsp/UTtypes.h>

#define SWAPB(value) \
  VRswapBytes((const void *) &(value), \
              (void *) &(value), sizeof(value), 1)

#define LHEAD         512L
#define FM_SPPACK_BE  "\100\303"  /* Magic value in file byte order */
#define FM_SPPACK_LE  "\303\100"

#define S_SAMPLEDDATA   ((UT_uint2_t) 0xFC0E) /* Sampled data file */
#define DLMAGIC         4567
#define BL_MAXINFO      LHEAD

enum {
  C_UNIFORM = 1,  /* uniform */
  C_ALAW = 2,     /* A-law companding */
  C_MULAW = 3     /* mu-law companding */
};
enum {
  D_TIME = 1,    /* time domain */
  D_FREQ = 2,     /* frequency domain */
  D_QUEFR = 3    /* quefrency domain */
};

struct SPPACK_head {
/*  char cs1[80]; */
/*  char cs2[80]; */
/*  char cmd[80]; */
  UT_uint2_t Domain;      /* Domain */
/*  UT_uint2_t FrameSize; */
  UT_float4_t Sfreq;      /* Sampling frequency */
/*  UT_uint2_t unused[2]; */
  char Magic[2];          /* File magic */
  UT_uint2_t Dtype;       /* Data type */
  UT_uint2_t Resolution;  /* Resolution in bits */
  UT_uint2_t Compand;     /* Companding */
/*  char lstr1[80]; */
/*  char lstr2[80]; */
/*  char lstr3[80]; */
  UT_uint2_t dl_magic;    /* DAT-Link magic */
  UT_uint2_t left;        /* Left channel flag */
  UT_uint2_t right;       /* Right channel flag */
  /* UT_uint2_t unused2[2]; */
};

/* setjmp / longjmp environment */
extern jmp_buf AFR_JMPENV;

AF_READ_DEFAULT(AFr_default); /* Define the AF_read defaults */


AFILE *
AFrdBLhead(FILE *fp)

{
  AFILE *AFp;
  int Lb;
  long int offs, poffs;
  char Info[BL_MAXINFO];
  struct SPPACK_head Fhead;
  struct AF_read AFr;

/* Set the long jump environment; on error return a NULL */
  if (setjmp(AFR_JMPENV))
    return NULL;  /* Return from a header read error */

/* Defaults and initial values */
  AFr = AFr_default;
  AFr.RInfo.Info = Info;
  AFr.RInfo.N = 0;
  AFr.RInfo.Nmax = sizeof(Info);

/* Read the first part of the header */
  poffs = 0;
  offs  = AFrdInfoIdentText(fp, 80, "comment:", &AFr.RInfo, 1);
  AFsetChunkLim("cs1 ", poffs, offs, &AFr.ChunkInfo);
  poffs = offs;
  offs += AFrdInfoIdentText(fp, 80, "comment:", &AFr.RInfo, 1);
  AFsetChunkLim("cs2 ", poffs, offs, &AFr.ChunkInfo);
  poffs = offs;
  offs += AFrdInfoIdentText(fp, 80, "command:", &AFr.RInfo, 1);
  AFsetChunkLim("cmd ", poffs, offs, &AFr.ChunkInfo);

  poffs = offs;
  offs += RHEAD_V(fp, Fhead.Domain, DS_NATIVE);
  offs += RSKIP(fp, 2);  /* skip FrameSize */
  offs += RHEAD_V(fp, Fhead.Sfreq, DS_NATIVE);
  offs += RSKIP(fp, 4);  /* skip two fill values */
  AFsetChunkLim("info", poffs, offs, &AFr.ChunkInfo);
  poffs = offs;

  /* Check the preamble file magic */
  offs += RHEAD_S(fp, Fhead.Magic);
  if (SAME_CSTR(Fhead.Magic, FM_SPPACK_BE)) {
    AFr.DFormat.Swapb = DS_EB;
    AFsetChunkLim("BL-b", poffs, offs, &AFr.ChunkInfo);
  }
  else if (SAME_CSTR(Fhead.Magic, FM_SPPACK_LE)) {
    AFr.DFormat.Swapb = DS_EL;
    AFsetChunkLim("BL-l", poffs, offs, &AFr.ChunkInfo);
  }
  else {
    UTwarn("AFrdBLhead - %s", AFM_BL_BadId);
    return NULL;
  }
  poffs = offs;

/* Fix up the values we have already read */
  if (UTswapCode(AFr.DFormat.Swapb) == DS_SWAP) {
    SWAPB(Fhead.Domain);
    SWAPB(Fhead.Sfreq);
  }

/* Continue reading the header */
  offs += RHEAD_V(fp, Fhead.Dtype, AFr.DFormat.Swapb);
  offs += RHEAD_V(fp, Fhead.Resolution, AFr.DFormat.Swapb);
  offs += RHEAD_V(fp, Fhead.Compand, AFr.DFormat.Swapb);
  AFsetChunkLim("fmt ", poffs, offs, &AFr.ChunkInfo);

  poffs = offs;
  offs += RSKIP(fp, 80);
  AFsetChunkLim("lst1", poffs, offs, &AFr.ChunkInfo);
  poffs = offs;
  offs += RSKIP(fp, 80);
  AFsetChunkLim("lst2", poffs, offs, &AFr.ChunkInfo);
  poffs = offs;
  offs += RSKIP(fp, 80);
  AFsetChunkLim("lst3", poffs, offs, &AFr.ChunkInfo);

  poffs = offs;
  offs += RHEAD_V(fp, Fhead.dl_magic, AFr.DFormat.Swapb);
  offs += RHEAD_V(fp, Fhead.left, AFr.DFormat.Swapb);
  offs += RHEAD_V(fp, Fhead.right, AFr.DFormat.Swapb);
  AFsetChunkLim("chan", poffs, offs, &AFr.ChunkInfo);

  poffs = offs;
  offs += RSKIP(fp, (LHEAD - offs));
  AFsetChunkLim("skip", poffs, offs, &AFr.ChunkInfo);

/* Set up the data format parameters */
  if (Fhead.Dtype == S_SAMPLEDDATA) {
    switch (Fhead.Compand) {
    case C_MULAW:
      Lb = 8;
      AFr.DFormat.Format = FD_MULAW8;
      break;
    case C_ALAW:
      Lb = 8;
      AFr.DFormat.Format = FD_ALAW8;
      break;
    case C_UNIFORM:
      Lb = 16;
      AFr.DFormat.Format = FD_INT16;
      break;
    default:
      UTwarn("AFrdBLhead - %s: \"%d\"", AFM_BL_UnsComp, (int) Fhead.Compand);
      return NULL;
    }
  }
  else {
    UTwarn("AFrdBLhead - %s: \"%d\"", AFM_BL_UnsData, (int) Fhead.Dtype);
    return NULL;
  }

  /* Error checks */
  if (Fhead.Resolution != Lb) {
    UTwarn("AFrdBLhead - %s: \"%d\"", AFM_BL_UnsWLen, (int) Fhead.Resolution);
    return NULL;
  }
  if (Fhead.Domain != D_TIME) {
    UTwarn("AFrdBLhead - %s: \"%d\"", AFM_BL_UnsDomain, (int) Fhead.Domain);
    return NULL;
  }
  if (Fhead.dl_magic == DLMAGIC) {
    AFr.NData.Nchan = 0L;
    if (Fhead.left)
      ++AFr.NData.Nchan;
    else if (Fhead.right)
      ++AFr.NData.Nchan;
  }

/* Set the parameters for file access */
  AFr.Sfreq = (double) Fhead.Sfreq;

  AFsetChunkLim("data", offs, AF_EoF, &AFr.ChunkInfo);

  AFp = AFsetRead(fp, FT_SPPACK, &AFr, AF_NOFIX);

  return AFp;
}
