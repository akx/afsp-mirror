/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  void LPlpcSyn (AFILE *AFpI, AFILE *AFpL, AFILE *AFpO,
                 double pre, int Lframe, int Np)

Purpose:
  Perform LPC synthesis from a residual signal

Description:
  This routine synthesizes the output data frame by frame from an input
  residual error signal file and an input LPC coefficient file.

Parameters:
   -> AFILE *AFpI
      Audio file pointer for the input residual file
   -> AFILE *AFpL
      Audio file pointer for the input LPC coefficient file
   -> AFILE *AFpO
      Audio file pointer for the output audio file.  If AFpO is NULL, the
      output signal is not written out.   
   -> double pre
      Preemphasis factor
   -> int Lframe
      Frame length
   -> int Np
      Number of LPC coefficients

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.3 $  $Date: 2003/05/13 01:18:00 $

-------------------------------------------------------------------------*/

#include <libtsp.h>
#include "LPsyn.h"


void
LSlpcSyn (AFILE *AFpI, AFILE *AFpL, AFILE *AFpO, double pre,
	  int Lframe, int Np)

{
  int Nout;
  float Rbuff[MAXFRAME];
  float Sbuff[MAXFRAME+MAXNP];
  float Dbuff[MAXFRAME];
  float pc[MAXNP];
  float ec[MAXNP+1];
  float Fmemd;
  long int offr, nf;
  int Nv;

/* Data access:
  This program is intended to be a skeleton for more complex processing.  It
  uses very simple buffering, processing only one frame at a time.
*/

  offr = 0;
  nf = 0;

/* Deemphasis memory */
  Fmemd = 0.0;

/* Filter memory */
  VRfZero (Sbuff, Np);

  while (1) {
    /* Read the residual signal */
    Nout = AFfReadData (AFpI, offr, Rbuff, Lframe);
    offr = offr + Lframe;
    if (Nout == 0)
      break;

    /* read the LPC coefficients */
    Nv = AFfReadData (AFpL, nf * Np, pc, Np);
    if (Nv != Np)
      UThalt ("%s: Unexpected end of LPC file", PROGRAM);
    nf = nf + 1;
    SPpcXec (pc, ec, Np);

    /* Filter the data */
    FIfFiltAP (Rbuff, Sbuff, Lframe, ec, Np+1);

    /* Deemphasize the signal */
    FIfDeem (pre, &Fmemd, &Sbuff[Np], Dbuff, Lframe);

    /* Write the reconstructed signal to an audio file */
    if (AFpO != NULL)
      AFfWriteData (AFpO, Dbuff, Lframe);

    /* Set up the LPC filter memory for the next frame */
    VRfShift (Sbuff, Np, Lframe);
  }

  return;
}
