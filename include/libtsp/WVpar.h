/*------------ Telecommunications & Signal Processing Lab --------------
                         McGill University

Routine:
  WVpar.h

Description:
  Declarations for WAVE file headers

Author / revision:
  P. Kabal  Copyright (C) 2020
  $Date: 2020/11/25 17:56:06 $

----------------------------------------------------------------------*/

#ifndef WVpar_h_
#define WVpar_h_

#include <libtsp/UTtypes.h>   /* typedef for UT_uint2_t, etc */

#define NELEM(array)  ((int) ((sizeof array) / (sizeof array[0])))

/* Supported format codes */
#define WAVE_FORMAT_UNKNOWN     0x0000
#define WAVE_FORMAT_PCM         0x0001
#define WAVE_FORMAT_IEEE_FLOAT  0x0003
  /*  IEEE754: range (+1, -1]  */
  /*  32-bit/64-bit format as defined by float/double type */
#define WAVE_FORMAT_ALAW        0x0006
#define WAVE_FORMAT_MULAW       0x0007

/* Special formats (*/
#define WAVE_FORMAT_EXTENSIBLE  0xFFFE  /* extensible format */
#define WAVE_FORMAT_DEVELOPMENT 0xFFFF

/* Compressed formats */
#define WAVE_FORMAT_ADPCM       0x0002    /* Microsoft */
#  define WV_FD_ADPCM           "Microsoft ADPCM"
#define WAVE_FORMAT_IMA_ADPCM   0x0011    /* Intel */
#  define WV_FD_IMA_ADPCM       "Intel IMA/DVI ADPCM"
#define WAVE_FORMAT_DSPGROUP_TRUESPEECH \
                                0x0022    /* DSP Group */
#  define WV_FD_DSPGROUP_TRUESPEECH \
                                "DSP Group TrueSpeech"
#define WAVE_FORMAT_GSM610      0x0031    /* Microsoft */
#  define WV_FD_GSM610          "Microsoft GSM 06.10"
#define WAVE_FORMAT_MSG723      0x0042    /* Microsoft */
#  define WV_FD_MSG723          "G.723.1 coding"
#define WAVE_FORMAT_MPEGLAYER3  0x0055    /* AT&T Labs */
#  define WV_FD_MPEGLAYER3      "ISO/MPEG-1 Layer 3"

/* Chunk names and identifiers in file byte order */
#define ckID_RIFF     "RIFF"
#  define FM_WAVE     "WAVE"
#define ckID_bext     "bext"
#define ckID_fmt      "fmt "
#define ckID_fact     "fact"
#define ckID_data     "data"
#define ckID_afsp     "afsp"
#define ckID_DISP     "DISP"
#define ckID_LIST     "LIST"
#  define FM_INFO     "INFO"

/* ----- ----- ----- ----- ----- ----- */
#ifdef WV_SPKR

#define WV_N_SPKR_MASK NELEM(WV_Spkr_Mask)
/* Speaker position masks */
static const unsigned long int WV_Spkr_Mask[] =
{
 (    0x1),  (    0x2),   /* FL   FR   */
 (    0x4),  (    0x8),   /* FC   LFE1 */
 (   0x10),  (   0x20),   /* BL   BR   */
 (   0x40),  (   0x80),   /* FLc  FRc  */
 (  0x100),  (  0x200),   /* BC   SiL  */
 (  0x400),  (  0x800),   /* SiR  TpC  */
 ( 0x1000),  ( 0x2000),   /* TpFL TpFC */
 ( 0x4000),  ( 0x8000),   /* TpFR TpBL */
 (0x10000),  (0x20000)    /* TpBC TpBR */
};
/* Reserved for future use */
#define WV_SPKR_RESERVED          0x7FFC0000
/* Any speaker configuration */
#define WV_SPKR_ALL               0x80000000
/* Logical OR masks */
#define WV_SPKR_KNOWN                0x3FFFF

/* Map WV speaker mask index to AF speaker location index (see AFpar.h) */
static const unsigned int WV_AF_Spkr_Index[] = {
   1,      2,       /* FL    FR   */
   3,      4,       /* FC    LFE1 */
   5,      6,       /* BL    BR   */
   7,      8,       /* FLc   FRc  */
   9,     11,       /* BC    SiL  Note change in order */
  12,     16,       /* SiR   TpC  */
  13,     15,       /* TpFL  TpFC */
  14,     17,       /* TpFR  TpBL */
  21,     18,       /* TpBC  TpBR */
};

/* Map AF speaker location index to WV mask index */
/* If n = SpkrConfig[i] => mask index (0 - 17) is AF_WV_ChannelMask[n-1] */
/* Unsupported values are set to WV_X */
#define WV_X  (255)
static const unsigned int AF_WV_ChannelMask[] = {
     0,      1,       /* FL    FR    */
     2,      3,       /* FC    LFE1  */
     4,      5,       /* BL    BR    */
     6,      7,       /* FLc   FRc   */
     8,   WV_X,       /* BC    LFE2  */
    10,     11,       /* SiL   SiR   */
    12,     13,       /* TpFL  TpFR  */
    14,     15,       /* TpFC  TpC   */
    16,     17,       /* TpBL  TpBR  */
  WV_X,   WV_X,       /* TpSiL TpSiR */
    18,   WV_X,       /* TpBC  BtFC  */
  WV_X,   WV_X,       /* BtFL  BtFR  */
  WV_X,   WV_X,       /* FLw   FRw   */
  WV_X,   WV_X,       /* LS    RS    */
  WV_X,   WV_X,       /* LSd   RSd   */
  WV_X,   WV_X,       /* TpLS  TpRS  */
};

#endif      /* WV_SPKR */

/* ----- ----- ----- ----- ----- ----- */
#ifdef  WV_INFO_REC
struct WV_LI {
  char ckID[4];
  const char *RecID[4];
};

/* Mapping between information record identifiers and LIST/INFO sub-chunks */
/* Input file: The first identifier is used on reading the corresponding chunk
     in the file
   Output file: Any of the identifiers if present are written as the
     corresponding record in the file
   The first 5 elements are in the order that they should appear in the
   LIST/INFO chunk (and will appear in that order on reading the file).
*/
static const struct WV_LI WV_LImap[] = {
  {"INAM", {"title:", "name:", NULL}},
  {"IART", {"artist:", "author:", NULL}},
  {"ICRD", {"creation_date:", "date:", "recording_date:", NULL}},
  {"ICMT", {"comment:", "comments:", NULL}},
  {"ISFT", {"software:", "program:", NULL}},
  {"IARL", {"archival_location:", NULL}},
  {"ICMS", {"commissioned:", NULL}},
  {"ICOP", {"copyright:", NULL}},
  {"ICRP", {"cropped:", NULL}},
  {"IDIT", {"digitization time", NULL}},    /* "Wed Jan 02 02:03:55 1990\n\0" */
  {"IENG", {"engineer:", NULL}},
  {"IGNR", {"genre:", NULL}},
  {"IKEY", {"keywords:", NULL}},
  {"IMED", {"medium:", NULL}},
  {"IPRD", {"product:", NULL}},
  {"ISBJ", {"subject:", NULL}},
  {"ISRC", {"source:", NULL}},
  {"ISMP", {"SMPTE time codes:", NULL}},    /* "HH:MM:SS.FF\0" */
  {"ITOC", {"table of contents:", NULL}},
  {"ITCH", {"technician:", NULL}},
  {"ITRK", {"track:", NULL}},
};
#define N_LIMAP       NELEM(WV_LImap)

/* DISP chunk and the corresponding information record identifiers */
static const char *WV_DISPid[] = {"display_text:", NULL};
/* Text flag used in the DISP chunk */
#define CF_TEXT   ((UT_uint4_t) 1)

#endif      /* WV_INFO_REC */
/* ----- ----- ----- ----- ----- ----- */

struct WV_GUID {
  UT_uint2_t wFormatTag;
  UT_uint1_t guidx[14];
};

static const struct WV_GUID WAVEFORMATEX_TEMPLATE =
{ 0x0000,
  { 0x00, 0x00,
    0x00, 0x00,
    0x10, 0x00,   /* In file byte order */
    0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71}
};

#define IS_VALID_WAVEFORMATEX_GUID(Guid) \
    (!memcmp(&WAVEFORMATEX_TEMPLATE.guidx, &Guid.guidx, 14))

/* WAVE chunk definitions */
#define ALIGN   2         /* Chunks padded out to a multiple of ALIGN */

struct WV_Ckpreamb {
  char ckID[4];
  UT_uint4_t ckSize;
};

struct WV_Ckfmt {
  char ckID[4];
  UT_uint4_t ckSize;
  UT_uint2_t wFormatTag;      /* Format type */
  UT_uint2_t nChannels;       /* Number of channels */
  UT_uint4_t nSamplesPerSec;  /* Sample rate */
  UT_uint4_t nAvgBytesPerSec; /* nBlockAlign * nSamplesPerSec */
  UT_uint2_t nBlockAlign;     /* Block size (bytes), multiple of nChannels */
/* End of the basic fmt chunk */
/* Begin format specific fields */
  UT_uint2_t wBitsPerSample;  /* Bits/sample: for WAVE_FORMAT_EXTENSIBLE,
                                 this is the sample container size */
/* Start of EXTENSIBLE part */
  UT_uint2_t cbSize;          /* Number of bytes following */
  UT_uint2_t wValidBitsPerSample; /* Actual number of bits in each sample */
  UT_uint4_t dwChannelMask;   /* Channel assignment mask */
  struct WV_GUID SubFormat;
};

struct WV_Ckfact {
  char ckID[4];
  UT_uint4_t ckSize;
  UT_uint4_t dwSampleLength;
};

struct WV_Ckdata {
  char ckID[4];
  UT_uint4_t ckSize;
};

#ifdef WV_INFO_REC
struct WV_CkDISP {
  char ckID[4];
  UT_uint4_t ckSize;
  UT_uint4_t CFtype;
  const char *text;
};

/* INFO chunk used inside LIST chunk */
struct WV_CkINFO {
  char ckID[4];
  UT_uint4_t ckSize;
  const char *text;
};

/* LIST/INFO structure */
struct WV_CkLIST {
  char ckID[4];
  UT_uint4_t ckSize;
  char INFOID[4];
  int N;
  struct WV_CkINFO CkINFO[N_LIMAP];
};

/* afsp chunk for info records */
struct WV_Ckafsp {
  char ckID[4];
  UT_uint4_t ckSize;
  char AFspID[4];
  const char *text;
};

struct WV_Ckbext {
  char ckID[4];
  UT_uint4_t ckSize;
  const char Description[256];
  const char Originator[32];
  const char OriginatorReference[32];
  const char OriginatorDate[10];
  const char OriginatorTime[8];
  UT_uint4_t TimeReferenceLow;
  UT_uint4_t TimeReferenceHigh;
  UT_uint2_t Version;
  const char UMID[64];
  UT_int2_t LoudnessValue;
  UT_int2_t LoudnessRange;
  UT_int2_t MaxTruePeakLevel;
  UT_int2_t MaxMomentaryLoudness;
  UT_int2_t MaxShortTermLoudness;
  const char Reserved[180];
  const char CodingHistory[];
};
#endif    /* WV_INFO_REC */

/* RIFF chunk with subchunks used to assemble the header for writing */
struct WV_CkRIFF {
  char ckID[4];
  UT_uint4_t ckSize;
  char WAVEID[4];
  struct WV_Ckfmt Ckfmt;
  struct WV_Ckfact Ckfact;
#ifdef WV_INFO_REC
  struct WV_CkDISP CkDISP;
  struct WV_CkLIST CkLIST;
  struct WV_Ckafsp Ckafsp;
#endif    /* WV_INFO_REC */
  struct WV_Ckpreamb Ckdata;  /* Not including audio data */
};

/* Information record ID's for bext chunk */
#define BW_RecID_Description           "description:"
#define BW_RecID_Originator            "originator:"
#define BW_RecID_OriginatorDate        "originator_date:"
#define BW_RecID_OriginatorTime        "originator_time:"
#define BW_RecID_Version               "BWF_version:"
#define BW_RecID_UMID                  "SMPTE_UMID:"
#define BW_RecID_LoudnessValue         "loudness_value:"
#define BW_RecID_LoudnessRange         "loudness_range:"
#define BW_RecID_MaxTruePeakLevel      "max_true_peak_level:"
#define BW_RecID_MaxMomentaryLoudness  "max_momentary_loudness:"
#define BW_RecID_MaxShortTermLoudness  "max_short_term_loudness:"

#endif    /* WVpar_h_ */
