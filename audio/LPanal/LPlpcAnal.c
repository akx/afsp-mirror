/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  void LPlpcAnal (AFILE *AFpI, AFILE *AFpL, AFILE *AFpO,
                  double pre, const float Win[], int Lwin, int Woffs,
		  int Lframe, int Np, double bwexp)

Purpose:
  Perform LPC analysis

Description:
  This routine analyzes the input data frame by frame to generate a set of LPC
  coefficients that minimize the residual error energy for the windowed data.
  This set of LPC coefficients is then used to filter the input data to form
  the residual signal.

Parameters:
   -> AFILE *AFpI
      Audio file pointer for the input data file
   -> AFILE *AFpL
      Audio file pointer for the output LPC coefficient file
   -> AFILE *AFpO
      Audio file pointer for the residual signal data file.  If AFpO is NULL,
      the residual signal is not written out.   
   -> int Fstats
      Statistics flag, set to 1 to get frame-by-frame information
   -> double pre
      Preemphasis factor
   -> const float Win[]
      Window data (Lwin values)
   -> int Lwin
      Window length
   -> int Woffs
      Offset of the center of the data frame to the center of the analysis
      window.
   -> int Lframe
      Frame length
   -> int Np
      Number of LPC coefficients
   -> float *bwexp
      Bandwidth expansion factor

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.11 $  $Date: 2003/05/13 01:18:26 $

-------------------------------------------------------------------------*/

#include <libtsp.h>
#include "LPanal.h"

void
LPlpcAnal (AFILE *AFpI, AFILE *AFpL, AFILE *AFpO, double pre,
	   const float Win[], int Lwin, int Woffs, int Lframe, int Np,
	   double bwexp)

{
  int Nout;
  double perr;
  float Wbuff[MAXWINDOW];
  float Dbuff[MAXFRAME+MAXNP];
  float Rbuff[MAXFRAME];
  float cor[MAXNP+1];
  float pc[MAXNP];
  float ec[MAXNP+1];
  float Fmemw, Fmemd;
  long int offd, offw;

/* Data access:
  This program is intended to be a skeleton for more complex processing.  As
  such, it uses very simple buffering, processing only one frame at a time.
  In fact we make use of random access to the input file, reading the data
  for the analysis and for the filtering separately.
*/
  offd = 0;
  offw = MSiFloor (Lframe - Lwin, 2) - Woffs + offd;

/* Preemphasis memory */
  AFfReadData (AFpI, offd-1, &Fmemd, 1);

/* Filter memory */
  AFfReadData (AFpI, offd-Np, Dbuff, Np);

  while (1) {

    /* Read the data for LPC analysis */
    AFfReadData (AFpI, offw, Wbuff, Lwin);

    /* Preemphasize the data */
    AFreadData (AFpI, offw-1, &Fmemw, 1);
    offw = offw + Lframe;
    FIfPreem (pre, &Fmemw, Wbuff, Wbuff, Lwin);

    /* Window the data */
    VRfMult (Wbuff, Win, Wbuff, Lwin);

    /* Read the data for filtering */
    Nout = AFfReadData (AFpI, offd, &Dbuff[Np], Lframe);
    offd = offd + Lframe;
    if (Nout == 0)
      break;
    FIfPreem (pre, &Fmemd, &Dbuff[Np], &Dbuff[Np], Lframe);

    /* LPC analysis */
    SPautoc (Wbuff, Lwin, cor, Np+1);
    perr = SPcorXpc (cor, pc, Np);

    /* Bandwidth expansion */
    SPpcBWexp (bwexp, pc, pc, Np);

    /* Filter the data */
    SPpcXec (pc, ec, Np);
    FIfConvol (Dbuff, Rbuff, Lframe, ec, Np+1);

    /* Write the LPC coefficients */
    AFfWriteData (AFpL, pc, Np);

    /* Write the residual to an audio file */
    if (AFpO != NULL)
      AFfWriteData (AFpO, Rbuff, Lframe);

    /* Set up the LPC filter memory */
    VRfShift (Dbuff, Np, Lframe);
  }

  return;
}
