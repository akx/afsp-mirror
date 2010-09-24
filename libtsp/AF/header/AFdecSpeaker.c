/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  int AFdecSpeaker (const char String[], unsigned char *SpkrConfig, int MaxN)
  int AFcheckSpeakers (const unsigned char *SpkrConfig);
  int AFspeakerNames (const unsigned char *SpkrConfig, char *SpkrNames,
                      int MaxNc)

Purpose:
  Decode, check, list loudspeaker spatial positions

Description:
  These routines decode or generate the mapping of output channels to speaker
  positions. The spacial positions of the loudspeakers are specified as a list
  of comma and/or white-space separated locations from the list below.
    "FL"  - Front Left
    "FR"  - Front Right
    "FC"  - Front Center
    "LF"  - Low Frequency
    "BL"  - Back Left
    "BR"  - Back Right
    "FLC" - Front Left of Center
    "FRC" - Front Right of Center
    "BC"  - Back Center
    "SL"  - Side Left
    "SR"  - Side Right
    "TC"  - Top Center
    "TFL" - Top Front Left
    "TFC" - Top Front Center
    "TFR" - Top Front Right
    "TBL" - Top Back Lefty
    "TBC" - Top Back Center
    "TBR" - Top Back Right
    "-"   - none
  Each speaker location is associated with an integer code: "FL" is speaker
  location code code 1, "FR" is speaker location code 2, and so on.

  A speaker position can be associated with only one channel.  Only WAVE files
  store the speaker locations in the file header.  In the case of WAVE files,
  the subset of spatial positions associated with the channels must appear in
  the order given above.

Parameters:
  <-  int AFdecSpeaker
      Error flag, zero for no error
   -> const char String[]
      String containing the list comma or whitespace separated list of speaker
      positions
  <-  unsigned char *SpkrConfig
      Null-terminated list of speaker location codes. Code values are integer
      values starting at 1. SpkrConfig must be of size MaxN+1.
   -> int MaxN
      Maximum number of speaker locations. This value should normally be equal
      to or larger than the number of channels.

  <-  int AFspeakerNames
      Error flag, zero for no error
   -> int Nchan
      Number of channels
   -> const unsigned char *SpkrConfig
      Null-terminated list of speaker location codes. Code values are integer
      values starting at 1.
  <-  char *SpkrNames
      String containing the blank-separated list of speaker positions (size
      MaxNc+1)
   -> MaxNc
      Maximum number of characters in SpkrNames (not including a terminating
      null)

  <-  int AFcheckSpeakers
      Error flag, zero for no error
   -> const unsigned char *SpkrConfig
      Null-terminatedlist of speaker location codes.

Author / revision:
  P. Kabal  Copyright (C) 2009
  $Revision: 1.9 $  $Date: 2009/02/28 23:36:04 $

-------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>

#include <libtsp.h>
#include <libtsp/AFheader.h>
#include <libtsp/AFmsg.h>
#define AF_SPKR_NAMES
#include <libtsp/AFpar.h>

#define NCBUF           512


int
AFdecSpeaker(const char String[], unsigned char *SpkrConfig, int MaxN)

{
  char cbuf[NCBUF+1];
  const char *p;
  char *token;
  int n, k, nc, ErrCode;

  p = String;

/* Set up the token buffer */
  nc = strlen (p);
  if (nc <= NCBUF)
    token = cbuf;
  else
    token = (char *) UTmalloc (nc + 1);

  ErrCode = 0;
  k = 0;
  while (p != NULL && k < MaxN) {
    p = STfindToken (p, ",", "", token, 2, nc);
    n = STkeyMatch (token, AF_Spkr_Names);
    if (n < 0) {
      ErrCode = 1;
      UTwarn ("AFdecSpeaker - %s: \"%.10s\"", AFM_BadSpkr, token);
      break;
    }
    SpkrConfig[k] = n+1;
    ++k;
  }

  if (k == MaxN) {
    ErrCode = 1;
    UTwarn ("AFdecSpeaker - %s", AFM_XSpkr);
    k = 0;
  }
  SpkrConfig[k] = '\0';

  /* Check for duplicate speakers */
  if (! ErrCode)
    ErrCode = AFcheckSpeakers (SpkrConfig);

/* Deallocate the buffer */
  if (nc > NCBUF)
    UTfree ((void *) token);

  if (ErrCode)
    SpkrConfig[0] = '\0';

  return ErrCode;
}

/* Check for duplicate speaker names */


int
AFcheckSpeakers (const unsigned char *SpkrConfig)

{
  int ErrCode, Nspkr, i, n;
  unsigned char SpkrCode[AF_N_SPKR_NAMES];

  ErrCode = 0;
  if (SpkrConfig == NULL || SpkrConfig[0] == AF_X_SPKR)
    Nspkr = 0;
  else
    Nspkr = strlen ((const char *) SpkrConfig);

  for (i = 0; i < AF_N_SPKR_NAMES; ++i)
    SpkrCode[i] = 0;

  for (i = 0; i < Nspkr; ++i) {
    n = SpkrConfig[i];
    assert (n >= 1 && n <= AF_N_SPKR_NAMES);
    if (n != AF_SPKR_X) {
      if (SpkrCode[n-1] > 0) {
	ErrCode = 1;
	UTwarn ("AFcheckSpeakers - %s: \"%.10s\"", AFM_DupSpkr,
		AF_Spkr_Names[n-1]);
	break;
      }
    }
    ++SpkrCode[n-1];
  }

  return ErrCode;
}

/* Speaker configuration names */
/* The output is a list of blank separated speaker location names. Extra
   speakers are designated as "-". If the output string is not long enough
   to contain all of the names, the list is truncated and " ..." is added
   at the end of the string.
*/


int
AFspeakerNames (int Nchan, const unsigned char *SpkrConfig, char *SpkrNames,
		int MaxNc)

{
  int ErrCode;
  int i, Nspkr, nc, ncE, ncS, n, nb;

  ErrCode = 0;

  SpkrNames[0] = '\0';
  if (SpkrConfig == NULL || SpkrConfig[0] == AF_X_SPKR ||
      SpkrConfig[0] == '\0')
    return ErrCode;		/* Quick bypass */

  /* Check for duplicate speakers */
  ErrCode = AFcheckSpeakers (SpkrConfig);
  if (ErrCode)
    return ErrCode;

  Nspkr = strlen ((const char *) SpkrConfig);
  SpkrNames[0] = '\0';
  nc = 0;
  ncE = 0;             /* Last place in the string for an ellipsis */
  nb = 0;              /* Insert blank flag */
  for (i = 0; i < Nchan; ++i) {

    /* Get the speaker location code */
    if (i < Nspkr)
      n = SpkrConfig[i];
    else
      n = AF_SPKR_X;

    /* Save the last position in the string for an ellipsis */
    if (nc + 4 <= MaxNc)
      ncE = nc;

    ncS = strlen ((const char *) AF_Spkr_Names[n-1]);
    if (nc + nb + ncS <= MaxNc) {
      if (nb > 0)
	SpkrNames[nc] = ' ';
      nc = nc + nb;
      STcopyMax (AF_Spkr_Names[n-1], &SpkrNames[nc], MaxNc-nc);
      nc = nc + ncS;
      nb = 1;
    }
    else {
      /* Out of space, add " ..." to the end of the list */
      STcopyMax (" ...", &SpkrNames[nc], MaxNc-ncE);

      break;
    }
  }

  return ErrCode;
}
