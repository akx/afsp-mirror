/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  void AOsetFOopt (const struct AO_FOpar *FO)

Purpose:
  Set output file options

Description:
  This routine sets the input file options in the audio file options structure
  from the input file parameter structure.

Parameters:
   -> const struct AO_FOpar *FO
      Output file parameters

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.5 $  $Date: 2003/11/04 12:50:46 $

-------------------------------------------------------------------------*/

#include <string.h>

#include <libtsp.h>
#include <libtsp/AFpar.h>
#include <AO.h>


void
AOsetFOopt (const struct AO_FOpar *FO)

{
  int N;
  struct AF_opt *AFopt;

  AFopt = AFoptions ();

  AFopt->Nframe = FO->Nframe;
  AFopt->NbS = FO->DFormat.NbS;

  UTfree (AFopt->SpkrConfig);
  if (FO->SpkrConfig != NULL) {
    N = strlen ((const char *) FO->SpkrConfig);
    AFopt->SpkrConfig = (unsigned char *) UTmalloc (N+1);
    strcpy ((char *) AFopt->SpkrConfig, (const char *) FO->SpkrConfig);
  }

  AFsetInfo (NULL);	/* Reset the info string */
  AFsetInfo (FO->Info);

  return;
}
