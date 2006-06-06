/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  int PQloudTest (const struct MOVC_Loud *Loud, int Nchan, int Np)

Purpose:
  Loudness test

Description:
  This routine returns the PEAQ frame number of the first frame that
  has a loudness which exceeds 0.1 sone.

Parameters:
  <-  int PQloudTest
      Frame offset
   -> const struct MOVC_Loud
      Structure containing the loudness values
   -> int Nchan
      Number of channels
   -> int Np
      Number of PEAQ frames

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.4 $  $Date: 2003/05/13 01:15:12 $

-------------------------------------------------------------------------*/

#include <math.h>

#include "PQevalAudio.h"

#define MINV(a, b)	(((a) < (b)) ? (a) : (b))

static int
PQ_LThresh (double Thr, const double NRef[], const double NTest[],
	    int Np);


int
PQloudTest (const struct MOVC_Loud *Loud, int Nchan, int Np)

{
  int j, Ndel;
  static const double Thr = 0.1;	/* Threshold: 0.1 sone */

  /* Loudness threshold */
  Ndel = Np;
  for (j = 0; j < Nchan; ++j)
    Ndel = MINV (Ndel,
		 PQ_LThresh (Thr, Loud->NRef[j], Loud->NTest[j], Np));

  return Ndel;
}

/* Loudness check: Look for the first time that loudness exceeds a
   threshold for both the test and reference signals */

static int
PQ_LThresh (double Thr, const double NRef[], const double NTest[],
	    int Np)

{
  int i;

  for (i = 0; i < Np; ++i) {
    if (NRef[i] > Thr && NTest[i] > Thr)
      break;
  }

  return i;
}
