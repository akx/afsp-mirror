/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  void AOsetFOopt(const struct AO_FOpar *FO)

Purpose:
  Set output file options

Description:
  This routine sets the output file options in the audio file options structure
  from the output file parameter structure.

Parameters:
  <-  void AOsetFOopt
   -> const struct AO_FOpar *FO
      Output file parameters

Author / revision:
  P. Kabal  Copyright (C) 2020
  $Revision: 1.14 $  $Date: 2020/11/26 11:55:45 $

-------------------------------------------------------------------------*/

#include <string.h>

#include <AO.h>


void
AOsetFOopt(const struct AO_FOpar *FO)

{
  int N;

  AFopt.Nframe = FO->Nframe;
  AFopt.NbS = FO->DFormat.NbS;

  UTfree(AFopt.SpkrConfig);
  if (FO->SpkrConfig != NULL) {
    N = (int) strlen((const char *) FO->SpkrConfig);
    AFopt.SpkrConfig = (unsigned char *) UTmalloc(N+1);
    STcopyMax((const char *) FO->SpkrConfig, (char *) AFopt.SpkrConfig, N);
  }
}
