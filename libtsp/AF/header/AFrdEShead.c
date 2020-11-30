/*-------------- Telecommunications & Signal Processing Lab ---------------

Routine:
  AFILE *AFrdEShead(FILE *fp)

Purpose:
  Get file format information from an ESPS sampled data feature file

Description:
  This routine reads the header for an ESPS sampled data feature file. The
  header information is used to set the file data format information in the
  audio file pointer structure.

  ESPS sampled data feature file header:
   Offset Length Type    Contents
      8     4    --     Header size (bytes)
     12     4    int    Sampled data record size
     16     4    int    File identifier
     40    26    char   File creation date
     66     8    char   Header version
     74    16    char   Program
     90     8    char   Program version
     98    26    char   Program date
    124     4    int    Number of samples (may indicate zero)
    132     4    int    Number of doubles in a data record
    136     4    int    Number of floats in a data record
    140     4    int    Number of longs in a data record
    144     4    int    Number of shorts in a data record
    148     4    int    Number of chars in a data record
    160     8    char   User name
    333    ...   --     Generic header items, including "record_freq"
      -    ...   --     Audio data

  For ESPS sampled data feature files, additional information is embedded in
  the  header. This information is stored as "date:", "header_version:",
  "program:", "program_version:", "program_compile_date:", "start_time:",
  and "max_value:" information records in the audio file parameter structure.

Parameters:
  <-  AFILE *AFrdEShead
      Audio file pointer for the audio file
   -> FILE *fp
      File pointer for the file

Author / revision:
  P. Kabal  Copyright (C) 2020
  $Revision: 1.99 $  $Date: 2020/11/26 11:29:47 $

-------------------------------------------------------------------------*/

#include <libtsp/sysOS.h>
#if (SY_OS == SY_OS_WINDOWS)
#  define _CRT_SECURE_NO_WARNINGS     /* Allow sprintf */
#endif

#include <assert.h>
#include <ctype.h>
#include <setjmp.h>
#include <string.h>

#include <libtsp.h>
#include <AFpar.h>
#include <libtsp/nucleus.h>
#include <libtsp/AFheader.h>
#include <libtsp/AFinfo.h>
#include <libtsp/AFmsg.h>
#include <libtsp/UTtypes.h>

#define ES_LHMIN  333   /* start of generic items */
/* Generic item, "record freq"
   15             short
   3              short   generic item identifier
   record_freq\0  char    identifier (null terminated, multiple of 4 bytes)
   1              long    data count
   1              short   data type (double)
*/
#define ES_MAXGENERIC 8192
#define ES_MAXINFO     256

/*  File magic value in file byte order. ESPS files can be written in either
    big-endian or little-endian byte order.
*/
#define FM_ESPS_BE  "\0\0\152\32"   /* Big-endian data */
#define FM_ESPS_LE  "\32\152\0\0"   /* Little-endian data */

/* File codes */
enum {
  ES_FT_SD    =  9,  /* (Old) Sampled data file */
  ES_FT_FEA   = 13,  /* Feature file */
  ES_FEA_SPEC =  7,  /* Feature file - spectrum file subcode */
  ES_FEA_SD   =  8,  /* Feature file - sampled data file subcode */
  ES_FEA_FILT =  9   /* Feature file - filter file subcode */
};

/* Machine codes */
enum {
  ES_SUN4_CODE   =  4,  /* Sun sparc 4 machine code */
  ES_DS3100_CODE =  7,  /* Decstation machine code */
  ES_SG_CODE     =  9,  /* SGI machine code */
  ES_HP700_CODE  = 21   /* HP machine code */
};

/* Data codes for generic header items */
enum {
  ES_DOUBLE = 1,
  ES_FLOAT  = 2,
  ES_LONG   = 3,
  ES_SHORT  = 4
};

struct ES_preamb {
/*  UT_int4_t Machine_code; */  /* machine which wrote file */
/*  uT_int4_t Check_code; */    /* version check code (3000) */
  UT_int4_t Data_offset;        /* data offset in bytes */
  UT_int4_t Record_size;        /* record size */
  char Magic[4];                /* file magic */
/*  UT_int4_t Edr; */           /* EDR_ESPS flag */
/*  UT_int4_t Align_pad_size; */  /* alignment padding */
/*  UT_int4_t Foreign_hd; */    /* pointer to foreign header */
};

struct ES_fixhead {
  UT_int2_t Type;          /* File type (e.g. ES_FT_FEA) */
/*  UT_int2_t pad1; */
  char Magic[4];           /* File magic */
/*  char Datetime[26]; */  /* File creation date */
/*  char Version[8];   */  /* Header version */
/*  char Prog[16];     */  /* Program name */
/*  char Vers[8];      */  /* Program version */
/*  char Progdate[26]; */  /* Program compile date */
  UT_int4_t Ndrec;         /* Number of data records (0 means unknown) */
/*  UT_int2_t Tag; */      /* Non-zero if data is tagged */
/*  UT_int2_t nd1; */
  UT_int4_t Ndouble;
  UT_int4_t Nfloat;
  UT_int4_t Nlong;
  UT_int4_t Nshort;
  UT_int4_t Nchar;
/*  UT_int4_t Fixsiz; */   /* Fixed header structure size (40 longs) */
/*  UT_int4_t Hsize; */    /* Variable header structure size (bytes) */
/*  char User[8]; */
/*  UT_int2_t spares[10]; */
};

struct ES_FEAhead {
  UT_int2_t Fea_type;      /* Feature file subcode (e.g. ES_FEA_SD) */
};

/* Structure for a Generic Item */
#define ES_MAX_gID  32
struct ES_genItem {
  UT_uint2_t code;         /* Generic Item code */
  UT_uint2_t ID_len;       /* Length of ID in 4-byte words */
  char ID[ES_MAX_gID];     /* ID */
  UT_int4_t count;         /* Number of data items */
  UT_uint2_t data_code;    /* Data type */
  /* data */
};

#define SWAPB(value) \
  VRswapBytes((const void *) &(value), (void *) &(value), sizeof(value), 1)
#define SWAPBXY(x,y) \
  VRswapBytes((const void *) &(x), (void *) &(y), sizeof(x), 1)

/* setjmp / longjmp environment */
extern jmp_buf AFR_JMPENV;

AF_READ_DEFAULT(AFr_default); /* Define the AF_read defaults */

/* Local function */
static int
AF_getGeneric(const char buff[], int Nbuff, const char ID[], int Fbo,
              int Nval, int Type, void *Val);


AFILE *
AFrdEShead(FILE *fp)

{
  AFILE *AFp;
  int NgI, Nv, Ntype;
  long int offs, poffs, Ldata;
  UT_float8_t DTemp;
  char Info[ES_MAXINFO];
  struct ES_preamb Fpreamb;
  struct ES_fixhead FheadF;
  struct ES_FEAhead FheadV;
  char GenItems[ES_MAXGENERIC];
  char str[20];
  struct AF_read AFr;

/* Set the long jump environment; on error return a NULL */
  if (setjmp(AFR_JMPENV))
    return NULL;  /* Return from a header read error */

/* Defaults and initial values */
  AFr = AFr_default;
  AFr.RInfo.Info = Info;
  AFr.RInfo.N = 0;
  AFr.RInfo.Nmax = sizeof(Info);

/* Read selected preamble values */
/* We do not know the byte order until after we have read the file magic */
  poffs = 0;
  offs  = RSKIP(fp, 8L);
  offs += RHEAD_V(fp, Fpreamb.Data_offset, DS_NATIVE);
  offs += RHEAD_V(fp, Fpreamb.Record_size, DS_NATIVE);
  AFsetChunkLim("pre ", poffs, offs, &AFr.ChunkInfo);
  poffs = offs;

/* Check the preamble file magic */
  offs += RHEAD_S(fp, Fpreamb.Magic);
  if (SAME_CSTR(Fpreamb.Magic, FM_ESPS_BE)) {
    AFr.DFormat.Swapb = DS_EB;
    AFsetChunkLim("ES-b", poffs, offs, &AFr.ChunkInfo);
  }
  else if (SAME_CSTR(Fpreamb.Magic, FM_ESPS_LE)) {
    AFr.DFormat.Swapb = DS_EL;
    AFsetChunkLim("ES-l", poffs, offs, &AFr.ChunkInfo);
  }
  else {
    UTwarn("AFrdEShead - %s", AFM_ES_BadId);
    return NULL;
  }

/* Fix up the words we have already read */
  if (UTswapCode(AFr.DFormat.Swapb) == DS_SWAP) {
    SWAPB(Fpreamb.Data_offset);
    SWAPB(Fpreamb.Record_size);
  }

/* Read selected values from the fixed part of the header */
  poffs = offs;
  offs += RSKIP(fp, 32 - offs);
  AFsetChunkLim("skip", poffs, offs, &AFr.ChunkInfo);

  poffs = offs;
  offs += RHEAD_V(fp, FheadF.Type, AFr.DFormat.Swapb);
  offs += RSKIP(fp, 2);
  offs += RHEAD_S(fp, FheadF.Magic);
  offs += AFrdInfoIdentText(fp, 26, "date:", &AFr.RInfo, 1);
  offs += AFrdInfoIdentText(fp,  8, "header_version:", &AFr.RInfo, 1);
  offs += AFrdInfoIdentText(fp, 16, "program:", &AFr.RInfo, 1);
  offs += AFrdInfoIdentText(fp,  8, "program_version:", &AFr.RInfo, 1);
  offs += AFrdInfoIdentText(fp, 26, "program_compile_date:", &AFr.RInfo, 1);
  offs += RHEAD_V(fp, FheadF.Ndrec, AFr.DFormat.Swapb);
  offs += RSKIP(fp, 4);

  offs += RHEAD_V(fp, FheadF.Ndouble, AFr.DFormat.Swapb);
  Ntype  = (FheadF.Ndouble > 0);
  offs += RHEAD_V(fp, FheadF.Nfloat, AFr.DFormat.Swapb);
  Ntype += (FheadF.Nfloat > 0);
  offs += RHEAD_V(fp, FheadF.Nlong, AFr.DFormat.Swapb);
  Ntype += (FheadF.Nlong > 0);
  offs += RHEAD_V(fp, FheadF.Nshort, AFr.DFormat.Swapb);
  Ntype += (FheadF.Nshort > 0);
  offs += RHEAD_V(fp, FheadF.Nchar, AFr.DFormat.Swapb);
  Ntype += (FheadF.Nchar > 0);
  if (Ntype > 1) {
     UTwarn("AFrdEShead - %s", AFM_ES_MixData);
     return NULL;
  }

  offs += RSKIP(fp, 8);
  offs += AFrdInfoIdentText(fp, 8, "user:", &AFr.RInfo, 1);
  AFsetChunkLim("fmt ", poffs, offs, &AFr.ChunkInfo);

/* Read selected feature file header values */
  poffs = offs;
  offs += RSKIP(fp, 188 - offs);
  AFsetChunkLim("skip", poffs, offs, &AFr.ChunkInfo);

  poffs = offs;
  offs += RHEAD_V(fp, FheadV.Fea_type, AFr.DFormat.Swapb);
  AFsetChunkLim("feat", poffs, offs, &AFr.ChunkInfo);

/* Generic items */
  poffs = offs;
  offs += RSKIP(fp, ES_LHMIN - offs);
  AFsetChunkLim("skip", poffs, offs, &AFr.ChunkInfo);

  poffs = offs;
  NgI = MINV(ES_MAXGENERIC, Fpreamb.Data_offset - ES_LHMIN);
  offs += RHEAD_SN(fp, GenItems, NgI);
  AFsetChunkLim("genI", poffs, offs, &AFr.ChunkInfo);

/* Skip to the start of data */
  poffs = offs;
  offs += RSKIP(fp, Fpreamb.Data_offset - offs);
  AFsetChunkLim("skip", poffs, offs, &AFr.ChunkInfo);

/* Error checks */
  if (FheadF.Type != ES_FT_FEA) {
    UTwarn("AFrdEShead - %s: \"%d\"", AFM_ES_UnsType, (int) FheadF.Type);
    return NULL;
  }
  if (!SAME_CSTR(FheadF.Magic, Fpreamb.Magic)) {
    UTwarn("AFrdEShead - %s", AFM_ES_IdMatch);
    return NULL;
  }
  if (FheadV.Fea_type != ES_FEA_SD) {
     UTwarn("AFrdEShead - %s: \"%d\"", AFM_ES_UnsFea, (int) FheadV.Fea_type);
     return NULL;
  }

/* Determine the data format */
  if (FheadF.Nshort != 0) {
    AFr.NData.Nchan = FheadF.Nshort;
    AFr.DFormat.Format = FD_INT16;
  }
  else if (FheadF.Nlong != 0) {
    AFr.NData.Nchan = FheadF.Nlong;
    AFr.DFormat.Format = FD_INT32;
  }
  else if (FheadF.Nfloat != 0) {
    AFr.NData.Nchan = FheadF.Nfloat;
    AFr.DFormat.Format = FD_FLOAT32;
  }
  else if (FheadF.Ndouble != 0) {
    AFr.NData.Nchan = FheadF.Ndouble;
    AFr.DFormat.Format = FD_FLOAT64;
  }
  else {
    UTwarn("AFrdEShead - %s", AFM_ES_UnsData);
    return NULL;
  }
  if (Fpreamb.Record_size != AF_DL[AFr.DFormat.Format] * AFr.NData.Nchan) {
    UTwarn("AFrdEShead - %s", AFM_ES_UnsEncod);
    return NULL;
  }

/* Get the sampling frequency */
  if (!AF_getGeneric(GenItems, NgI, "record_freq", AFr.DFormat.Swapb,
                     1, ES_DOUBLE, &AFr.Sfreq)) {
    UTwarn("AFrdEShead - %s", AFM_ES_NoSfreq);
    return NULL;
  }

  /* Other Generic Items */
  if (AF_getGeneric(GenItems, NgI, "start_time", AFr.DFormat.Swapb, 1,
                    ES_DOUBLE, &DTemp)) {
    Nv = sprintf(str, "%.7g", DTemp);
    AFaddInfoRec("start_time: ", str, Nv, &AFr.RInfo);
  }
  /* Pick up "max_value" only if it is a single value */
  if (AF_getGeneric(GenItems, NgI, "max_value", AFr.DFormat.Swapb, 1,
                    ES_DOUBLE, &DTemp)) {
    Nv = sprintf(str, "%.7g", DTemp);
    AFaddInfoRec("max_value: ", str, Nv, &AFr.RInfo);
  }

/* Set the parameters for file access */
  if (FheadF.Ndrec != 0) {
    AFr.NData.Nsamp = FheadF.Ndrec * AFr.NData.Nchan;
    Ldata = AFr.NData.Nsamp * AF_DL[AFr.DFormat.Format];
    AFsetChunkLim("data", offs, offs + Ldata, &AFr.ChunkInfo);
  }
  else                /* Ndrec = 0 may indicate unknown no. samples */
    AFsetChunkLim("data", offs, AF_EoF, &AFr.ChunkInfo);

  AFp = AFsetRead(fp, FT_ESPS, &AFr, AF_FIX_NSAMP_LOW | AF_FIX_NSAMP_HIGH);

  return AFp;
}

/* Get data from a Generic Item */


static int
AF_getGeneric(const char buff[], int Nbuff, const char ID[], int Fbo,
              int Nval, int Type, void *Val)

{
  int ncID, ncIDX, k, nS, Found, Lw, Nr;
  char sstr[2+2+ES_MAX_gID+4+2+1];
  char *p;
  struct ES_genItem gItem;

  ncID = (int) strlen(ID);
  ncIDX = RNDUPV(ncID + 1, 4);   /* Length of ID rounded up */
  assert(ncIDX <= ES_MAX_gID);

  /* Fill in a Generic Item structure */
  gItem.code = 13;
  gItem.ID_len = (UT_uint2_t) (ncIDX / 4);
  STcopyMax(ID, gItem.ID, ES_MAX_gID-1);
  for (k = ncID; k < ncIDX; ++k)
    gItem.ID[k] = '\0';
  gItem.count = (UT_uint4_t) Nval;
  gItem.data_code = (UT_uint2_t) Type;

  /* Form the search string */
  if (UTswapCode(Fbo) == DS_SWAP) {
    SWAPBXY(gItem.code, sstr[0]);
    SWAPBXY(gItem.ID_len, sstr[2]);
    memcpy(&sstr[4], gItem.ID, ncIDX);
    SWAPBXY(gItem.count, sstr[ncIDX + 4]);
    SWAPBXY(gItem.data_code, sstr[ncIDX + 8]);
  }
  else {
    memcpy(sstr, &gItem.code, 2);
    memcpy(&sstr[2], &gItem.ID_len, 2);
    memcpy(&sstr[4], &gItem.ID, ncIDX);
    memcpy(&sstr[ncIDX + 4], &gItem.count, 4);
    memcpy(&sstr[ncIDX + 8], &gItem.data_code, 2);
  }
  nS = ncIDX + 10;

  /* Search for the Generic Item in the buffer */
  p = STstrstrNM(buff, sstr, Nbuff, nS);
  Found = 0;
  if (p != NULL) {
    switch (Type) {
    case ES_DOUBLE:
      Lw = 8;
      break;
    case ES_FLOAT:
    case ES_LONG:
      Lw = 4;
      break;
    case ES_SHORT:
      Lw = 2;
      break;
    default:
      Lw = 0;
      assert(0);
      break;
    }
    p += nS;                  /* Point to values */
    Nr = Nbuff - (int) (p - buff);  /* No. bytes remaining in buffer */
    if (Nval * Lw <= Nr) {
      if (UTswapCode(Fbo) == DS_SWAP)
       VRswapBytes(p, Val, Lw, Nval);
      else
        memcpy(Val, p, Nval * Lw);
      Found = 1;
    }
  }

  return Found;
}
