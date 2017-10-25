/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  int AFspeakerNames (int Nchan, const unsigned char *SpkrConfig,
                      char *SpkrNames, int MaxNc)

Purpose:
  List loudspeaker spatial positions

Description:
  The spatial positions of the loudspeakers are generated as a list of blank
  separated location names.  If the output string is not long enough to contain
  all of the names, the list is truncated and " ..." is added at the end of the
  string.

Parameters:
  <-  int AFspeakerNames
      Number of speaker positions listed in the output string
   -> const unsigned char *SpkrConfig
      Null-terminated list of speaker location codes. Code values are integer
      values starting at 1.
  <-  char *SpkrNames
      Null-terminated string containing the blank-separated list of speaker
      positions
   -> MaxNc
      Maximum number of characters in SpkrNames (not including a terminating
      null)

Author / revision:
  P. Kabal  Copyright (C) 2017
  $Revision: 1.4 $  $Date: 2017/05/24 16:17:41 $

-------------------------------------------------------------------------*/

#include <string.h>
#include <assert.h>

#include <libtsp.h>
#include <AFpar.h>
#include <libtsp/nucleus.h>
#include <libtsp/AFmsg.h>

#define NELEM(array)  ((int) ((sizeof array) / (sizeof array[0])))

/* Speaker names, indexed 0 : AF_N_SKPR_NAMES - 1 */
const char *AF_Spkr_Names[] = {
  "FL",    "FR",      /* Front Left            Front Right             */
  "FC",    "LFE1",    /* Front Centre          Low Frequency Effects-1 */
  "BL",    "BR",      /* Back Left             Back Right              */
  "FLc",   "FRc",     /* Front Left Centre     Front Right Centre      */
  "BC",    "LFE2",    /* Back Centre           Low Frequency Effects-2 */
  "SiL",   "SiR",     /* Side Left             Side Right              */
  "TpFL",  "TpFR",    /* Top Front Left        Top Front Right         */
  "TpFC",  "TpC",     /* Top Front Centre      Top Centre              */
  "TpBL",  "TpBR",    /* Top Back Left         Top Back Right          */
  "TpSiL", "TpSiR",   /* Top Side Left         Top Side Right          */
  "TpBC",  "BtFC",    /* Top Back Centre       Bottom Front Centre     */
  "BtFL",  "BtFR",    /* Bottom Front Left     Bottom Front Right      */
  "FLw",   "FRw",     /* Front Left wide       Front Right wide        */
  "LS",    "RS",      /* Left Surround         Right Surround          */
  "LSd",   "RSd",     /* Left Surround direct  Right Surround direct   */
  "TpLS",  "TpRS",    /* Top Left Surround     Top Right Surround      */
  "-",                /* Not associated with a named location          */
};
/* Length of "FL FR ... TpRS" is 136 (not including trailing null) */


/* Speaker configuration names */


int
AFspeakerNames (const unsigned char *SpkrConfig, char *SpkrNames, int MaxNc)

{
  int NO, i, Nspkr, n, nE, nc, m, ib;

  assert (AF_N_SPKR_NAMES == (NELEM(AF_Spkr_Names)) &&
          AF_SPKR_AUX == AF_N_SPKR_NAMES);

  NO = 0;             /* Number of speaker locations in the output string */
  SpkrNames[0] = '\0';
  if (SpkrConfig == NULL || SpkrConfig[0] == '\0')
    return NO;   /* Quick bypass */

  n = 0;
  nE = 0;             /* Position in the string for an ellipsis */
  ib = 0;             /* Insert blank flag, 0 or 1 */

  Nspkr = (int) strlen ((const char *) SpkrConfig);
  for (i = 0; i < Nspkr; ++i) {

    /* Get the speaker location code */
    m = SpkrConfig[i];
    assert (m >= 0 && m <= AF_N_SPKR_NAMES);

    /* Save the current position in the string */
    if (n + 4 <= MaxNc)
      nE = n;

    nc = (int) strlen ((const char *) AF_Spkr_Names[m-1]);
    if (n + ib + nc <= MaxNc) {
      if (ib) {
        SpkrNames[n] = ' ';
        ++n;
      }
      STcopyMax (AF_Spkr_Names[m-1], &SpkrNames[n], MaxNc-n);
      n += nc;
      ib = 1;
      ++NO;
    }
    else {
      /* Out of space, add " ..." to the end of the list */
      STcopyMax (" ...", &SpkrNames[n], MaxNc-nE);
      break;
    }
  }

  return NO;
}
