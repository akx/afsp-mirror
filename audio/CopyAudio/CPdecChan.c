/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  void CPdecChan (const char String, int Ichan, struct CP_Chgain Chgain)

Purpose:
  Decode channel gain specifications

Description:
  This routine decodes gain factor strings.  The gain factor string takes the
  form
    [+/-] [gain *] chan +/- [gain *] chan +/- gain ...)
  where gain is a number or ratio and chan is A, B, C, ... .

Parameters:
   -> const char String[]
      Input string
   -> int Ichan
      Output channel number
  <-> struct CP_Chgain Chgain
      Structure with channel gains and offsets

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.20 $  $Date: 2003/05/13 01:33:24 $

-------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>

#include <libtsp.h>
#include <libtsp/nucleus.h>
#include "CopyAudio.h"

#define MAXV(a, b)	(((a) > (b)) ? (a) : (b))

static void
CP_decChan (const char String[], int *NI, double Gain[MAXNI], double *Offset);
static char *
CP_chan (char String[], int *NI);


void
CPdecChan (const char String[], int Ichan, struct CP_Chgain *Chgain)

{
  assert (Ichan >= 0 && Ichan < MAXNO);

  CP_decChan (String, &Chgain->NI, Chgain->Gain[Ichan],
	      &Chgain->Offset[Ichan]);
  Chgain->NO = MAXV (Chgain->NO, Ichan+1);

  return;
}

/* Decode one channel expression */


static void
CP_decChan (const char String[], int *NI, double Gain[MAXNI], double *Offset)

{
  int nc, n, sign, nsign;
  const char *p;
  char *gp;
  char *token;
  double gain;
  double DN, DD;

/* Allocate temporary space */
  nc = strlen (String);
  if (nc == 0)
    UThalt ("%s: %s", PROGRAM, CPM_EmptyChan);
  token = (char *) UTmalloc (nc + 1);

/* Loop over subexpressions */
  VRdZero (Gain, MAXNI);
  *Offset = 0.0;
  p = String;
  nsign = 1;
  while (p != NULL) {

    /* Separate the string at + or - signs, p-1 points to the delimiter */
    p = STfindToken (p, "+-", "", token, 1, nc);

    /* Save the sign of the delimiter for the next iteration */
    sign = nsign;
    nsign = 1;
    if (p != NULL && *(p-1) == '-')
      nsign = -1;

    /* Special handling for a + or - followed by another + or - */
    if (token[0] == '\0') {
      if (p == NULL)
	UThalt ("%s: %s: \"%s\"", PROGRAM, CPM_BadChanEx, String);
      nsign = sign * nsign;
      continue;
    }

/* Decode the channel (n == MAXNI means the offset) */
    gp = CP_chan (token, &n);
    if (n < 0)
      UThalt ("%s: %s: \"%s\"", PROGRAM, CPM_BadChanEx, String);

/* Decode the gain */
    DN = 1.0;
    DD = 1.0;
    if (gp != NULL && STdecDfrac (gp, &DN, &DD))
      UThalt ("%s: %s: \"%s\"", PROGRAM, CPM_BadChanEx, String);
    gain = sign * DN / DD;
    if (n < MAXNI) {
      Gain[n] += gain;
      *NI = MAXV (*NI, n+1);
    }
    else
      *Offset += gain;
  }

  /* Deallocate the storage */
  UTfree (token);

  return;
}

static const char *Chtab[] = {
  "A", "B", "C", "D", "E", "F", "G", "H", "I", "J",
  "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", NULL };

/* Return a pointer to the gain string (NULL if none) */
/* Return the input channel number for the expression, MAXNI if none, -1
   for an error.
   The input string may be modified.
*/


static char *
CP_chan (char String[], int *NI)

{
  char *q, *gp, *cp;
  int n;

/* Allowable expressions:
   25/3 * A	gain followed by channel
   A            channel (assumes unity gain)
   25.2         gain (assumes an offset)
*/

  assert (MAXNI == (sizeof Chtab / sizeof Chtab[0] - 1));

  /* Set up the pointers to the gain and channel strings */
  q = strchr (String, '*');
  if (q != NULL) {
    *q = '\0';		/* Overwrite the '*' */
    gp = String;
    cp = STtrimIws (q+1);
    n = STkeyMatch (cp, Chtab);
  }
  else {
    gp = NULL;
    n = STkeyMatch (String, Chtab);
    if (n < 0) {	/* No match, must be an offset */
      gp = String;
      n = MAXNI;
    }
  }

  *NI = n;

  return gp;
}
