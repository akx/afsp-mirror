/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  void PQprtMOV (const double MOV[], int N, double DI, double ODG)

Purpose:
  Print model output values

Description:
  Print model output values

Parameters:
   -> const double MOV[]
      Input model output variables
   -> int N
      Number of model output variables
   -> double DI
      Distortion index
   -> double ODG
      Objective difference grade

Author / revision:
  P. Kabal  Copyright (C) 2009
  $Revision: 1.5 $  $Date: 2009/03/12 18:29:33 $

-------------------------------------------------------------------------*/

#include <assert.h>
#include <stdio.h>

#include "PQevalAudio.h"


void
PQprtMOV (const double MOV[], int N, double DI, double ODG)

{
  printf ("Model Output Variables:\n");

  if (N == PQ_NMOV_B) {
    printf ("   BandwidthRefB: %g\n", MOV[0]);
    printf ("  BandwidthTestB: %g\n", MOV[1]);
    printf ("      Total NMRB: %g\n", MOV[2]);
    printf ("    WinModDiff1B: %g\n", MOV[3]);
    printf ("            ADBB: %g\n", MOV[4]);
    printf ("            EHSB: %g\n", MOV[5]);
    printf ("    AvgModDiff1B: %g\n", MOV[6]);
    printf ("    AvgModDiff2B: %g\n", MOV[7]);
    printf ("   RmsNoiseLoudB: %g\n", MOV[8]);                 
    printf ("           MFPDB: %g\n", MOV[9]);
    printf ("  RelDistFramesB: %g\n", MOV[10]);
  }
  else if (N == PQ_NMOV_A) {
    printf ("        RmsModDiffA: %g\n", MOV[0]);
    printf ("  RmsNoiseLoudAsymA: %g\n", MOV[1]);
    printf ("     Segmental NMRB: %g\n", MOV[2]);
    printf ("               EHSB: %g\n", MOV[3]);
    printf ("        AvgLinDistA: %g\n", MOV[4]);
  }
  else
    assert (0);

  printf ("Distortion Index: %.3f\n", DI);
  printf ("Objective Difference Grade: %.3f\n", ODG);
  return;
};
