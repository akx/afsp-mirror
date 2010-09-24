/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  double PQnNet (const double MOV[], const struct Par_NNet *NNet, double *DIp)

Purpose:
  Process the MOV's with a neural network to get the ODG

Description:
  This routine processes the model output variables through a neural
  network to obtain the objective difference grade.

Parameters:
  <-  double PQnNet
      Output objective difference grade
   -> const double MOV[]
      Input model output variables
   -> const struct Par_NNet *NNet
      Neural net parameters
  <-  double *DIp
      Distortion index

Author / revision:
  P. Kabal  Copyright (C) 2009
  $Revision: 1.9 $  $Date: 2009/03/12 18:28:32 $

-------------------------------------------------------------------------*/

#include <math.h>

#include "PQevalAudio.h"

#define SIG(x)	(1. / (1. + exp(-(x))))


double
PQnNet (const double MOV[], const struct Par_NNet *NNet, double *DIp)

{
  int i, j, Nclip;
  double MOVs[PQ_NMOV_MAX];
  double arg, DI, ODG;

  const int I        = NNet->I;
  const int J        = NNet->J;
  const double *amin = NNet->amin;
  const double *amax = NNet->amax;
  const double * const *wx  = NNet->wx;
  const double *wxb  = NNet->wxb;
  const double *wy   = NNet->wy;
  const double wyb   = NNet->wyb;
  const double bmin  = NNet->bmin;
  const double bmax  = NNet->bmax;

  /* Scale the MOV's */
  Nclip = 0;
  for (i = 0; i < I; ++i) {
    MOVs[i] = (MOV[i] - amin[i]) / (amax[i] - amin[i]);
    if (NNet->ClipMOV) {	/* Optional clipping */
      if (MOVs[i] < 0) {
	MOVs[i] = 0;
	++Nclip;
      }
      else if (MOVs[i] > 1) {
	MOVs[i] = 1;
	++Nclip;
      }
    }
  }
  if (Nclip > 0)
    printf (">>> %d MOVs clipped\n", Nclip);

  /* Neural network */
  DI = wyb;
  for (j = 0; j < J; ++j) {
    arg = wxb[j];
    for (i = 0; i < I; ++i)
      arg += wx[i][j] * MOVs[i];
    DI += wy[j] * SIG (arg);
  }
  *DIp = DI;
  ODG = bmin + (bmax - bmin) * SIG (DI);

  return ODG;
}
