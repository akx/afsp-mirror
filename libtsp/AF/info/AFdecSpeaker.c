/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  int AFdecSpeaker (const char String[], unsigned char *SpkrConfig, int MaxN)

Purpose:
  Decode loudspeaker spatial positions
  List loudspeaker spatial positions

Description:
  This routine decodes a list of loudspeaker locations.  The spatial positions
  of the loudspeakers are specified as a list of comma and/or white-space
  separated locations.  See the description of routine AFsetSpeaker for more
  information.

Parameters:
  <-  int AFdecSpeaker
      Error flag, zero for no error
   -> const char String[]
      String containing the list comma and/or white-space separated list of
      speaker positions and or configuration names.
  <-  unsigned char *SpkrConfig
      Null-terminated list of speaker location codes.  Code values are integer
      values starting at 1.  SpkrConfig must be of size MaxN+1.  In case of an
      error, SpkrConfig is empty.
   -> int MaxN
      Maximum number of speaker locations.  This value should normally be equal
      to or larger than the number of channels.

Author / revision:
  P. Kabal  Copyright (C) 2020
  $Revision: 1.25 $  $Date: 2020/09/17 20:01:48 $

-------------------------------------------------------------------------*/

#include <libtsp/sysOS.h>
#if (SY_OS == SY_OS_WINDOWS)
#  define _CRT_SECURE_NO_WARNINGS     /* Allow strcpy */
#endif

#include <string.h>

#include <libtsp.h>
#include <AFpar.h>
#include <libtsp/nucleus.h>
#include <libtsp/AFmsg.h>

#define NCBUF     (100)

/* Speaker keywords */
const char *AF_Spkr_Key[] = {
  "FL",             "FR",             /* \x01      \x02               */
  "FC",             "LF*E1",          /* \x03      \x04               */
  "BL",             "BR",             /* \x05      \x06               */
  "FLc",   "FLC",   "FRc",   "FRC",   /* \x07 \x07 \x08 \x08          */
  "BC",             "LFE2",           /* \x09      \x0A               */
  "SiL",   "SL",    "SiR",   "SR",    /* \x0B \x0B \x0C \x0C          */
  "TpFL",  "TFL",   "TpFR",  "TFR",   /* \x0D \x0D \x0E \x0E          */
  "TpFC",  "TFC",   "TpC",   "TC",    /* \x0F \x0F \x10 \x10          */
  "TpBL",  "TBL",   "TpBR",  "TBR",   /* \x11 \x11 \x12 \x12          */
  "TpSiL",          "TpSiR",          /* \x13      \x14               */
  "TpBC",  "TBC",   "BtFC",           /* \x15 \x15 \x16               */
  "BtFL",           "BtFR",           /* \x17      \x18               */
  "FLw",            "FRw",            /* \x19      \x1A               */
  "LS",             "RS",             /* \x1B      \x1C               */
  "LSd",            "RSd",            /* \x1D      \x1E               */
  "TpLS",           "TpRS",           /* \x1F      \x20               */
  "*-",                               /* \x21 (AF_SPKR_AUX)               */
  "stereo*phonic",                    /* \x01\x02                         */
  "quad*raphonic",                    /* \x01\x02\x05\x06                 */
  "5.1",                              /* \x01\x02\x03\x04\x05\x06         */
  "7.1",                              /* \x01\x02\x03\x04\x05\x06\x0B\x0C */
  NULL  /* Terminating null string */
};
/* Map from keywords to speaker location index, values 1 : AF_N_SPKRNAMES */
/* Speaker names are given by the index-1 */
const char *AF_Spkr_KeyVal[] = {
  "\x01",         "\x02",
  "\x03",         "\x04",
  "\x05",         "\x06",
  "\x07", "\x07", "\x08", "\x08",
  "\x09",         "\x0A",
  "\x0B", "\x0B", "\x0C", "\x0C",
  "\x0D", "\x0D", "\x0E", "\x0E",
  "\x0F", "\x0F", "\x10", "\x10",
  "\x11", "\x11", "\x12", "\x12",
  "\x13",         "\x14",
  "\x15", "\x15", "\x16",
  "\x17",         "\x18",
  "\x19",         "\x1A",
  "\x1B",         "\x1C",
  "\x1D",         "\x1E",
  "\x1F",         "\x20",
  "\x21",
  "\x01\x02",
  "\x01\x02\x05\x06",
  "\x01\x02\x03\x04\x05\x06",
  "\x01\x02\x03\x04\x05\x06\x0B\x0C"
};

/* Local function */
static int
AF_checkSpeakers (unsigned char *SpkrConfig);


int
AFdecSpeaker(const char String[], unsigned char *SpkrConfig, int MaxN)

{
  char cbuf[NCBUF+1];
  const char *p;
  char *token;
  int n, k, nc, m, Err;

/* Set up the token buffer */
  p = String;
  nc = (int) strlen (p);
  if (nc <= NCBUF)
    token = cbuf;
  else
    token = (char *) UTmalloc (nc + 1);

  Err = 0;
  k = 0;
  while (p != NULL && k < MaxN) {
    p = STfindToken (p, ",", "", token, 2, nc);
    n = STkeyMatch (token, AF_Spkr_Key);
    if (n < 0) {
      Err = 1;
      UTwarn ("AFdecSpeaker - %s: \"%.10s\"", AFM_BadSpkr, token);
      break;
    }

    m = (int) strlen (AF_Spkr_KeyVal[n]);
    if (k + m < MaxN) {    /* Make sure there is enough space */
      strcpy ((char *) &SpkrConfig[k], AF_Spkr_KeyVal[n]);
      k = k + m;
    }
    else {
      Err = 1;
      UTwarn ("AFdecSpeaker - %s", AFM_XSpkr);
      break;
    }
  }

/* Null termination */
  SpkrConfig[k] = '\0';

/* Check for duplicate speaker locations */
/* Remove trailing AF_SPKR_AUX values */
  if (! Err)
    Err = AF_checkSpeakers (SpkrConfig);

/* Deallocate the buffer */
  if (nc > NCBUF)
    UTfree ((void *) token);

  if (Err)
    SpkrConfig[0] = '\0';

  return Err;
}

/* Check for duplicate speaker names */
/* Remove trailing AF_SPKR_AUX values (modifies SpkrConfig) */


static int
AF_checkSpeakers (unsigned char *SpkrConfig)

{
  int Err, Nspkr, i, m;
  int SpkrCodeCount[AF_N_SPKR_NAMES];

  Err = 0;
  if (SpkrConfig == NULL || SpkrConfig[0] == '\0') {
    Nspkr = 0;
    return Err;
  }
  else
    Nspkr = (int) strlen ((const char *) SpkrConfig);

/* Set up the speaker location index count */
  for (i = 0; i < AF_N_SPKR_NAMES; ++i)
    SpkrCodeCount[i] = 0;

  for (i = 0; i < Nspkr; ++i) {
    m = SpkrConfig[i];    /* Values 1 to AF_N_SPKR_NAMES-1 */
    if (m > AF_N_SPKR_NAMES || m < 0) {
      UTwarn ("AFdecSpeaker: %s", AFM_BadSpkr);
      Nspkr = 0;
      break;
    }

    if (m != AF_SPKR_AUX) {
      if (SpkrCodeCount[m] > 0) {
        Err = 1;
        UTwarn ("AFcheckSpeakers - %s: \"%.10s\"", AFM_DupSpkr,
                AF_Spkr_Names[m-1]);
        break;
      }
      ++SpkrCodeCount[m];
    }
  }

  /* Remove trailing AF_SPKR_AUX values */
  for (i = Nspkr-1; i >= 0; --i) {
    if (SpkrConfig[i] != AF_SPKR_AUX)
      break;
    else
      SpkrConfig[i] = '\0';
  }

  return Err;
}
