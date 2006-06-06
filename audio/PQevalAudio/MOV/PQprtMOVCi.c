/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  void PQprtMOVCi (int Nchan, int i, const struct PQ_MOVBC *MOVC)

Purpose:
  Print MOV precursor information for a frame

Description:
  Print MOV precursor information for a frame.

Parameters:
   -> int Nchan
      Number of channels
   -> int i
      Frame number
   -> const struct PQ_MOVBC *MOVC
      Structure containing the MOV precursors

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.5 $  $Date: 2003/05/13 01:15:13 $

-------------------------------------------------------------------------*/

#include <assert.h>
#include <stdio.h>

#include "PQevalAudio.h"


void
PQprtMOVCi (int Nchan, int i, const struct PQ_MOVBC *MOVC)

{
  printf ("Frame: %d\n", i);

  if (Nchan == 1) {
    printf ("  Ntot   : %g %g\n",
	    MOVC->Loud.NRef[0][i], MOVC->Loud.NTest[0][i]);
    printf ("  ModDiff: %g %g %g\n",
	    MOVC->MDiff.Mt1B[0][i], MOVC->MDiff.Mt2B[0][i],
	    MOVC->MDiff.Wt[0][i]);
    printf ("  NL     : %g\n", MOVC->NLoud.NL[0][i]);
    printf ("  BW     : %g %g\n",
	    MOVC->BW.BWRef[0][i], MOVC->BW.BWTest[0][i]);
    printf ("  NMR    : %g %g\n",
	    MOVC->NMR.NMRavg[0][i], MOVC->NMR.NMRmax[0][i]);
    printf ("  PD     : %g %g\n", MOVC->PD.Pc[i], MOVC->PD.Qc[i]);
    printf ("  EHS    : %g\n", 1000 * MOVC->EHS.EHS[0][i]);
  }
  else if (Nchan == 2) {
    printf ("  Ntot   : %g %g // %g %g\n",
	    MOVC->Loud.NRef[0][i], MOVC->Loud.NTest[0][i],
	    MOVC->Loud.NRef[1][i], MOVC->Loud.NTest[1][i]);
    printf ("  ModDiff: %g %g %g // %g %g %g\n",
	    MOVC->MDiff.Mt1B[0][i], MOVC->MDiff.Mt2B[0][i],
	    MOVC->MDiff.Wt[0][i],
	    MOVC->MDiff.Mt1B[1][i], MOVC->MDiff.Mt2B[1][i],
	    MOVC->MDiff.Wt[1][i]);
    printf ("  NL     : %g // %g\n",
	    MOVC->NLoud.NL[0][i],
	    MOVC->NLoud.NL[1][i]);
    printf ("  BW     : %g %g // %g %g\n",
	    MOVC->BW.BWRef[0][i], MOVC->BW.BWTest[0][i],
	    MOVC->BW.BWRef[1][i], MOVC->BW.BWTest[1][i]);
    printf ("  NMR    : %g %g // %g %g\n",
	    MOVC->NMR.NMRavg[0][i], MOVC->NMR.NMRmax[0][i],
	    MOVC->NMR.NMRavg[1][i], MOVC->NMR.NMRmax[1][i]);
    printf ("  PD     : %g %g\n", MOVC->PD.Pc[i], MOVC->PD.Qc[i]);
    printf ("  EHS    : %g // %g\n",
	    1000 * MOVC->EHS.EHS[0][i],
	    1000 * MOVC->EHS.EHS[1][i]);
  }
  else
    assert (0);

  return;
}
