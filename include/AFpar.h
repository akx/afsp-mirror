/*------------ Telecommunications & Signal Processing Lab --------------
                         McGill University

Routine:
  AFpar.h

Description:
  Declarations for the AFsp audio file routines.

Author / revision:
  P. Kabal  Copyright (C) 2017
  $Revision: 1.117 $  $Date: 2017/09/22 00:30:47 $

----------------------------------------------------------------------*/

#ifndef AFpar_h_
#define AFpar_h_

#include <float.h>        /* DBL_MAX */
#include <limits.h>       /* LONG_MAX */
#include <stdio.h>        /* typedef for FILE */

#include <libtsp/UTpar.h> /* Byte swap codes DS_SWAP, etc. */

#ifndef AFILE_t_
# define  AFILE_t_
  typedef struct AF_filepar AFILE;  /* Audio file parameters */
#endif

/* ------ ------ ----- Loudspeaker location definitions */

#define AF_MAXN_SPKR        (50) /* Maximum number of speaker locations */
#define AF_MAXNC_SPKRNAMES (172) /* Max chars, 136 + 2*(N-32) */

/* AF_SPKR_AUX must be the last entry in the speaker name array */
#define AF_SPKR_AUX   ('\x21')  /* Location not specified */
                                /* Equal to AF_N_SPKR_NAMES, see assert in
                                   AFspeakerNames */
#define AF_N_SPKR_NAMES AF_SPKR_AUX  /* Number of speaker location names */

/* Declaration of loudspeaker location names */
/* Initialized in AFspeakerNames */
extern const char *AF_Spkr_Names[];

/* Declaration of loudspeaker keywords and indices */
/* Initialized in AFdecSpeaker */
extern const char *AF_Spkr_Key[];
extern const char *AF_Spkr_KeyVal[];

/* ------ ------ ----- Input audio file types */
/* Internal codes for input audio file types - must be sequential for the
   standard file types
*/
enum AF_FT_T {
  FT_UNKNOWN,     /* unknown audio file format */
  FT_NH,          /* headerless (non-standard or no header) audio file */
  FT_AU,          /* AU audio file */
  FT_WAVE,        /* WAVE file */
  FT_WAVE_EX,     /* WAVE (extensible format) file */
  FT_WAVE_BWF,    /* WAVE (BWF extension) */
  FT_WAVE_EX_BWF, /* WAVE (extensible format, BWF extension) */
  FT_AIFF,        /* AIFF sound file */
  FT_AIFF_C,      /* AIFF-C sound file */
  FT_SPHERE,      /* NIST SPHERE audio file */
  FT_ESPS,        /* ESPS sampled data feature file */
  FT_SF,          /* IRCAM soundfile */
  FT_SPPACK,      /* SPPACK file */
  FT_INRS,        /* INRS-Telecom audio file */
  FT_SPW,         /* Comdisco SPW Signal file */
  FT_NSP,         /* CSL NSP file */
  FT_TXAUD,       /* Text audio file (with header) */
  FT_ERROR = -1,  /* Error, file type cannot be determined */
  FT_AUTO  = 255, /* Automatic mode (check file header) */
  FT_UNSUP = 256  /* Unsupported audio file type */
};

#define AF_NFT (FT_TXAUD+1)

/* File type names */
/* Initialized in AFprintAFpar */
extern const char *AF_FTN[AF_NFT];

/* ------ ------ ----- Data format types */
/* Data format types - must be sequential */
enum AF_FD_T {
  FD_UNDEF,     /* undefined / unsupported file data format */
  FD_ALAW8,     /* A-law 8-bit data */
  FD_MULAW8,    /* mu-law 8-bit data */
  FD_MULAWR8,   /* mu-law bit-reversed 8-bit data */
  FD_UINT8,     /* offset binary integer 8-bit data */
  FD_INT8,      /* two's complement integer 8-bit data */
  FD_INT16,     /* two's complement integer 16-bit data */
  FD_INT24,     /* two's complement integer 24-bit data */
  FD_INT32,     /* two's complement integer 32-bit data */
  FD_FLOAT32,   /* 32-bit float data */
  FD_FLOAT64,   /* 64-bit float data */
  FD_TEXT16,    /* text data scaled as 16-bit integers */
  FD_TEXT       /* text data */
};
#define AF_NFD   (FD_TEXT+1)

/* Data type names */
/* Initialized in AFprintAFpar */
extern const char *AF_DTN[AF_NFD];

/* Data format sizes (bytes) */
enum AF_FDL_T {
  FDL_UNDEF    = 0,
  FDL_ALAW8    = 1,
  FDL_MULAW8   = 1,
  FDL_MULAWR8  = 1,
  FDL_UINT8    = 1,
  FDL_INT8     = 1,
  FDL_INT16    = 2,
  FDL_INT24    = 3,
  FDL_INT32    = 4,
  FDL_FLOAT32  = 4,
  FDL_FLOAT64  = 8,
  FDL_TEXT16   = 0,  /* Variable size */
  FDL_TEXT     = 0
};

/* Data lengths */
/* Initialized in AFprintAFPar */
extern const enum AF_FDL_T AF_DL[AF_NFD];

/* Default full scale data values */
/* For the fixed-point formats, data is returned in the range [-1, +1). An
   exception is 8-bit data which gives values [-1/2, 1/2).
*/
#define AF_FULLSCALE_UNDEF   (-1)
#define AF_FULLSCALE_ALAW8   (32768.)
#define AF_FULLSCALE_MULAW8  (32768.)
#define AF_FULLSCALE_MULAWR8 (32768.)
#define AF_FULLSCALE_UINT8   (32768./128.)
#define AF_FULLSCALE_INT8    (32768./128.)
#define AF_FULLSCALE_INT16   (32768.)
#define AF_FULLSCALE_INT24   (256.*32768.)
#define AF_FULLSCALE_INT32   (65536.*32768.)
#define AF_FULLSCALE_FLOAT32 (1.)
#define AF_FULLSCALE_FLOAT64 (1.)
#define AF_FULLSCALE_TEXT16  (32768.)
#define AF_FULLSCALE_TEXT    (1.)
#define AF_FULLSCALE_DEFAULT (-(DBL_MAX))

/* FullScale values for different data types */
static const double AF_FULLSCALE[AF_NFD] = {
  AF_FULLSCALE_UNDEF,
  AF_FULLSCALE_ALAW8,
  AF_FULLSCALE_MULAW8,
  AF_FULLSCALE_MULAWR8,
  AF_FULLSCALE_UINT8,
  AF_FULLSCALE_INT8,
  AF_FULLSCALE_INT16,
  AF_FULLSCALE_INT24,
  AF_FULLSCALE_INT32,
  AF_FULLSCALE_FLOAT32,
  AF_FULLSCALE_FLOAT64,
  AF_FULLSCALE_TEXT16,
  AF_FULLSCALE_TEXT
};

/* ------ ------ ----- Output audio file types */
/* These codes are replaced in the audio file structure with the FT_xxx codes
*/
enum AF_FTW_T {
  FTW_UNDEF,        /* Undefined */
  FTW_AU,           /* AU audio file */
  FTW_WAVE,         /* WAVE file (automatic extension, if appropriate) */
  FTW_AIFF,         /* AIFF sound file */
  FTW_AIFF_C,       /* AIFF-C sound file */
  FTW_AIFF_C_SOWT,  /* AIFF-C sound file (data byte-swapped) */
  FTW_WAVE_EX,      /* WAVE file (with extension) */
  FTW_WAVE_NOEX,    /* WAVE file (no extension) */
  FTW_NH_NATIVE,    /* NH file, native byte order */
  FTW_NH_SWAP,      /* NH file, swap byte order */
  FTW_NH_EL,        /* NH file, little-endian data */
  FTW_NH_EB,        /* NH file, big-endian data */
  FTW_TXAUD         /* Text data (with header) */
};

/* ------ ------ ----- File operation types in the AFILE structure */
enum AF_FO_T {
  FO_NONE = 0,      /* closed */
  FO_RO   = 1,      /* read */
  FO_WO   = 2       /* write */
};

/* ------ ------ ----- Error codes in the AFILE structure */
enum AF_ERR_T {
  AF_NOERR  =  0,
  AF_UEOF   = -1,   /* Unexpected end-of-file on read */
  AF_IOERR  =  1,   /* Read or write error */
  AF_DECERR =  2    /* Data decoding error on read */
};

/* ------ ------ ----- Sub-structures */
/* Information structures */
#define FM_AFSP     "AFsp"
#define AF_MAXINFO  10240
struct AF_info {
  char *Info;   /* Pointer to string */
  int N;        /* Number of characters (includes nulls) */
  int Nmax;     /* Maximum number of characters (size of Info) */
};

/* Data format structure */
struct AF_dformat {
  enum AF_FD_T Format;  /* Data format, FD_INT16, etc. */
  enum UT_DS_T Swapb;   /* Swap code, DS_EB, DS_NATIVE, etc. */
  int NbS;              /* Number of bits per sample */
  double FullScale;     /* File data full scale value */
};

/* Data layout structure */
struct AF_ndata {
  long int Ldata;       /* Data space in bytes */
  long int Nsamp;       /* Number of samples (all channels) */
  long int Nchan;       /* Number of channels */
  unsigned char SpkrConfig[AF_MAXN_SPKR+1]; /* Speaker location list */
};

/* Special values for Ldata, Nsamp and Nframe */
/* (There is a redundant relation ship between these values.
    Normally Nsamp = Nchan * Nframe and Ldata = Nsamp * LW where LW is the size
    of each sample in bytes.  The following flags determine which one of Ldata,
    Nsamp, and Nframe should be calculated from the others. */
#define AF_LDATA_UNDEF  -1
#define AF_NSAMP_UNDEF  -1
#define AF_NFRAME_UNDEF -1

/* Header data length fixup flags */
/* Flag to determine if fixes should be applied if Nsamp and Ldata are
   inconsistent.  The fixes used are file type specific - a warning is issued if
   the fix has to be applied.
*/
enum AF_FIX_T {
  AF_NOFIX                = 0,
  AF_FIX_NSAMP_HIGH       = 1,
  AF_FIX_NSAMP_LOW        = 2,
  AF_FIX_LDATA_HIGH       = 4
};

/* Chunk layout structures */
struct AF_chunkLim {
  char ID[4];
  long int Start;
  long int End;
};
struct AF_chunkInfo {
  struct AF_chunkLim *ChunkLim;
  int N;
  int Nmax;
};

/* Default values for AF_ChunkInfo structure */
#define AF_CHUNKINFO_INIT(x) static struct AF_chunkInfo x = {NULL, 0, 0}
#define AF_EoF LONG_MAX

/* Structure with read parameters used before AFILE structure is created */
struct AF_read {
  double Sfreq;
  struct AF_dformat DFormat;
  struct AF_ndata NData;
  struct AF_info RInfo;
  struct AF_chunkInfo ChunkInfo;
};

/* Default values for AF_read structure */
#define AF_READ_DEFAULT(x) \
  static const struct AF_read x = \
    {0.0, \
     {FD_UNDEF, DS_NATIVE, 0, AF_FULLSCALE_DEFAULT}, \
     {AF_LDATA_UNDEF, AF_NSAMP_UNDEF, 1L, {'\0'}}, \
     {NULL, 0, 0}, \
     {NULL, 0, 0}}

/* Structure with write parameters used before AFILE structure is created */
struct AF_write {
  double Sfreq;
  struct AF_dformat DFormat;
  long int Nchan;
  long int Nframe;        /* No. Frames: normally AF_NFRAME_UNDEF */
  enum AF_FTW_T FtypeW;   /* FTW_WAVE, etc. */
  unsigned char SpkrConfig[AF_MAXN_SPKR+1]; /* location list */
  struct AF_info WInfo;
};

/* Structure for NbS/Res bits/sample values */
/* Consider NbS = 12 bit samples (for instance from a 12-bit A/D) stored in a
   Res = 16 bit word.  By convention, the 12 bit samples occupy the most
   significant bits in a 16 bit container. */
struct AF_NbS {
  int NbS;                /* Number of (significant) bits per sample */
  int Res;                /* Number of bits in the sample container */
};
#define AF_RES_MAX (64)   /* Maximum number of bits/sample */

/* Default values for AF_NbS structure */
#define AF_NBS_INIT(x) static const struct AF_NbS x = {0, 0}

/* ------ ------ ----- AFILE audio file parameter structure */
struct AF_filepar {

  /* File pointers and parameters */
  FILE *fp;                 /* File pointer */
  enum AF_FO_T Op;          /* Operation (read, write) */
  enum UT_DS_T Swapb;       /* Swap code for file data (DS_NATIVE or DS_SWAP) */
  long int Start;           /* Start byte */
  long int Isamp;           /* Sample offset */

  /* Error indicators */
  enum AF_ERR_T Error;      /* Error flag (0 for no error) */
  long int Novld;           /* Number of points clipped */

  /* File data layout parameters */
  enum AF_FT_T Ftype;       /* File type */
  enum AF_FD_T Format;      /* Data format, FD_INT16, etc. */
  long int Nsamp;           /* Number of samples written (all channels) */
  long int Nchan;           /* Number of channels */
  long int Nframe;          /* Number of frames (to preset write header) */

  /* File data parameters */
  double Sfreq;             /* Sampling rate */
  int NbS;                  /* Number of significant bits per sample */
  double FullScale;         /* Full scale for file data */

  /* Program data parameters */
  double ScaleV;            /* Full scale for program sample data */
  double ScaleF;            /* Scale factor applied to file data */

  /* Data descriptors */
  struct AF_info AFInfo;    /* Information structure */
  struct AF_chunkInfo ChunkInfo;  /* Chunk layout parameters */
  unsigned char *SpkrConfig;  /* Speaker location list */
};
/* Scaling values:
Input files:
  File data in the range [-FullScale, +FullScale] are scaled by ScaleF so that
  the program data occupy the range [-ScaleV, +ScaleV] (normally [-1, +1]).
    ScaleF = ScaleV / FullScale
  So for example 16-bit file data has FullScale = 32768. Then to map to [-1, +1]
  ScaleF = 1/32768.  Note however, that ScaleF may differ from this if the input
  values have a gain applied to them.
Output files:
  Program data in the range [-ScaleV, +ScaleV] are scaled by ScaleF so that the
  file data occupy [-FullScale, +FullScale].
    ScaleF = FullScale / ScaleV
  So for example program data in the interval [-1, +1] are scaled to the
  FullScale of 32768 (corresponding to 16-bit file data), by using
  ScaleF = 32768.
*/

/* ------ ------ ----- AF options */
/* Sub-structure with parameters for headerless files */
struct AF_InputPar {
  enum AF_FD_T Format;      /* Data format */
  long int Start;           /* Offset in bytes to the start of data */
  double Sfreq;             /* Sampling frequency */
  enum UT_DS_T Swapb;       /* Byte swap flag */
  long int Nchan;           /* Number of channels */
  double FullScale;         /* Full scale for the file data */
};

/* Values for UInfo */
enum AF_STDINFO_T {
  AF_STDINFO_OFF,           /* Standard info records will not be used */
  AF_STDINFO_ON,            /* Standard info records will be used */
};

/* Default parameter values for headerless or non-standard files */
#define AF_SFREQ_DEFAULT  8000.0     /* Default sampling frequency */
#define AF_SFREQ_UNDEF    -1

/* Structure for AF routine options */
struct AF_opt {
/* General options */
  int ErrorHalt;              /* Error handling:
                                 0 - continue on error,
                                 1 - halt on error */
  double ScaleV;              /* Full scale value for program sample data */
/* Input file options */
  int NsampND;                /* Number of samples requirement
                                 0 - Nsamp must be known
                                 1 - Nsamp=AF_NSAMP_UNDEF allowed */
  int RAccess;                /* Random access requirement:
                                 0 - input file can be sequential or random
                                     access
                                 1 - input file must be random access */
  enum AF_FT_T FtypeI;        /* Input file type, FT_AUTO, FT_AU, etc. */
  struct AF_InputPar InputPar;  /* Default input audio file parameters */
/* Output file options */
  long int Nframe;            /* Number of frames
                                AF_NFRAME_UNDEF means this value is undefined */
  int NbS;                    /* Number of bits per sample
                                 0 means use full precision of the data */
  unsigned char *SpkrConfig;  /* Speaker configuration */
  enum AF_STDINFO_T StdInfo;  /* Flag whether to use standard info records */
  struct AF_info UInfo;       /* User supplied information string */
};
#define AF_ERRORHALT_DEFAULT  1       /* Halt on error */
#define AF_SCALEV_DEFAULT     1.0     /* Program sample data full scale */
#define AF_NSAMPND_DEFAULT    0       /* Number of samples must be known */
#define AF_NBS_DEFAULT        0       /* Bits/Sample unknown */
#define AF_RACCESS_DEFAULT    0       /* Allow non-random access files */
#define AF_STDINFO_DEFAULT  AF_STDINFO_ON   /* Generate standard info records */

/* Declaration of the options structure */
/* Initialized in AFoptions */
extern struct AF_opt AFopt;

/* Values for AFOptions reset categories */
enum AF_OPT_T {
  AF_OPT_NULL     = 0,
  AF_OPT_GENERAL  = 1,
  AF_OPT_INPUT    = 2,
  AF_OPT_OUTPUT   = 4,
  AF_OPT_ALL      = 7
};

/* ------ ------ ----- Byte swap macros */
#define BSWAP2(x) { \
  unsigned char *cp; \
  unsigned char t; \
  cp = (unsigned char *) x; \
  t = cp[1]; cp[1] = cp[0]; cp[0] = t; \
  }
#define BSWAP3(x) { \
  unsigned char *cp; \
  unsigned char t; \
  cp = (unsigned char *) x; \
  t = cp[2]; cp[2] = cp[0]; cp[0] = t; \
  }
#define BSWAP4(x) { \
  unsigned char *cp; \
  unsigned char t; \
  cp = (unsigned char *) x; \
  t = cp[3]; cp[3] = cp[0]; cp[0] = t; \
  t = cp[2]; cp[2] = cp[1]; cp[1] = t; \
  }
#define BSWAP8(x) { \
  unsigned char *cp; \
  unsigned char t; \
  cp = (unsigned char *) x; \
  t = cp[7]; cp[7] = cp[0]; cp[0] = t; \
  t = cp[6]; cp[6] = cp[1]; cp[1] = t; \
  t = cp[5]; cp[5] = cp[2]; cp[2] = t; \
  t = cp[4]; cp[4] = cp[3]; cp[3] = t; \
  }

#endif  /* AFpar_h_ */
