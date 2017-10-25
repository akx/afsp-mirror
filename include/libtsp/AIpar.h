/*------------ Telecommunications & Signal Processing Lab --------------
                         McGill University

Routine:
  AIpar.h

Description:
  Declarations for AIFF and AIFF-C sound file headers

Author / revision:
  P. Kabal  Copyright (C) 2017
  $Date: 2017/09/22 00:31:17 $

----------------------------------------------------------------------*/

#ifndef AIpar_h_
#define AIpar_h_

#include <libtsp/UTtypes.h>   /* typedef for uint2_t, etc */

/* AIFF or AIFF-C file layout
  FORM chunk preamble  8
    File ID            4
    FVER chunk         8 + 4  (AIFF-C files only)
    COMM chunk         8 + 18 + compressionType (4 bytes)
                         + compressionName (min length 2)
    SSND chunk         8 + 8 + data
*/

/* Chunk names and identifiers in file byte order */
#define FM_IFF        "FORM"
#  define FM_AIFF     "AIFF"
#  define FM_AIFF_C   "AIFC"
#define ckID_COMM     "COMM"
#define ckID_FVER     "FVER"
#define ckID_SSND     "SSND"
#define ckID_NAME     "NAME"
#define ckID_AUTH     "AUTH"
#define ckID_CPRT     "(c) "
#define ckID_ANNO     "ANNO"

#define CT_NONE       "NONE"
#define CT_SOWT       "sowt"
#define CT_ALAW       "alaw"
#define CT_ULAW       "ulaw"
#define CT_FLOAT32    "fl32"
#define CT_FLOAT64    "fl64"

#define CT_ADP4       "ADP4"  /* DVI ADPCM */
#define CT_DWVW       "DWVW"  /* TX16W Typhoon */
#define CT_FL32       "FL32"  /* Csound Float 32 */
#define CT_FL64       "FL64"
#define CT_IMA4       "ima4"  /* IMA */
#define CT_MAC3       "MAC3"  /* Apple MACE 3-to-1 */
#define CT_MAC6       "MAC6"  /* Appple MACE 6-to-1 */
#define CT_QDMC       "QDMC"  /* QDesign Music */
#define CT_QCLP       "Qclp"  /* Qualcomm PureVoice */
#define CT_RT24       "rt24"  /* Voxware RT24 */
#define CT_RT29       "rt29"  /* Voxware RT29 */

#define CT_X_ALAW     "ALAW"  /* "SGI CCITT G.711 A-law" */
#define CT_X_ULAW     "ULAW"  /* "SGI CCITT G.711 u-law" */
#define CT_G722       "G722"  /* "SGI CCITT G.722" */
#define CT_G726       "G726"  /* "CCITT G.726" */
#define CT_G728       "G728"  /* "CCITT G.728" */
#define CT_GSM        "GSM "  /* "GSM 06-10" */

/* Compression name strings (max CNAME_MAX characters) */
#define CNAME_MAX   32
#define CN_NONE     "not compressed"
#define CN_SOWT     "not compressed"
#define CN_ULAW     "\265law 2:1"
#define CN_ALAW     "Alaw 2:1"
#define CN_FLOAT32  "IEEE 32-bit float"
#define CN_FLOAT64  "IEEE 64-bit float"
#define CN_X_ALAW   "ITU-T G.711 A-law"
#define CN_X_ULAW   "ITU-T G.711 mu-law"

#define AIFCVersion1  0xA2805140  /* AIFF-C Version 1 identifier */

#define NANNO_MAX   12

struct AI_CkHead {
  char ckID[4];
  UT_uint4_t ckSize;
};

struct AI_CkFVER {
  char ckID[4];
  UT_uint4_t ckSize;
  UT_uint4_t timestamp;
};

struct AI_CkCOMM {
  char ckID[4];
  UT_uint4_t ckSize;
  UT_uint2_t numChannels;
  UT_uint4_t numSampleFrames;
  UT_uint2_t sampleSize;
  unsigned char sampleRate[10];
  char compressionType[4];
  char compressionName[CNAME_MAX+1];  /* Null terminated string */
};

struct AI_CkText {
  char ckID[4];
  UT_uint4_t ckSize;
  char *text;
};

struct AI_CkANNOx {
  char ckID[4];
  UT_uint4_t ckSize;
  char InfoID[4];
  char *text;
};

struct AI_CkSSND {
  char ckID[4];
  UT_uint4_t ckSize;
  UT_uint4_t offset;
  UT_uint4_t blockSize;
};

struct AI_CkFORM {
  char ckID[4];
  UT_uint4_t ckSize;
  char AIFFID[4];
  struct AI_CkCOMM CkCOMM;
  struct AI_CkFVER CkFVER;
  struct AI_CkText CkNAME;
  struct AI_CkText CkAUTH;
  struct AI_CkText CkCPRT;
  struct AI_CkText CkANNO[NANNO_MAX];
  struct AI_CkANNOx CkANNOx;
  struct AI_CkSSND CkSSND;
};

#define NELEM(array)  ((int) ((sizeof array) / (sizeof array[0])))

/* Text chunks and the corresponding information record identifiers */
/* Input file: The first identifier is used on reading the corresponding chunk
     in the file
   Output file: Any of the identifiers if present are written as the
     corresponding chunk in the file
*/
#ifdef AI_INFOREC

static const char *AI_NAMEid[] = {"title:", "name:", NULL};
static const char *AI_AUTHid[] = {"artist:","author:", NULL};
static const char *AI_CPRTid[] = {"copyright:", NULL};
static const char *AI_ANNOid[] = {"comment:", "comments:", "annotation:", NULL};

#endif

#endif  /* AIpar_h_ */
