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
  P. Kabal  Copyright (C) 2001
  $Revision: 1.3 $  $Date: 2001/10/16 13:49:45 $

-------------------------------------------------------------------------*/

static char rcsid[] = "$Id: AOsetFOopt.c 1.3 2001/10/16 AFsp-v6r8 $";

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

  AFsetHinfo (NULL);	/* Reset the info string */
  AFsetHinfo (FO->Hinfo);

  return;
}
