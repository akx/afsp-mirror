/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  void AOstdin (const struct AO_FIpar FI[], int N)

Purpose:
  Check for more than one standard input file specification

Description:
  This routine checks an array of structures containing file names to determine
  if more than one of them specifies standard input ("-").  If more than one
  is found, an error message is printed and execution is halted.

Parameters:
   -> const struct AO_FIpar *FI
      Array of input file parameter structures
   -> int N
      Number of input file structures

Author / revision:
  P. Kabal  Copyright (C) 1999
  $Revision: 1.5 $  $Date: 1999/06/05 02:36:25 $

-------------------------------------------------------------------------*/

static char rcsid[] = "$Id: AOstdin.c 1.5 1999/06/05 AFsp-v6r8 $";

#include <string.h>

#include <libtsp.h>
#include <AO.h>

#define ROUTINE		"AOstdin"
#define PGM		((UTgetProg ())[0] == '\0' ? ROUTINE : UTgetProg ())


void
AOstdin (const struct AO_FIpar FI[], int N)

{
  int i, Ni;

  Ni = 0;
  for (i = 0; i < N; ++i) {
    if (strcmp (FI[i].Fname, "-") == 0)
      ++Ni;
  }
  if (Ni > 1)
    UThalt ("%s: %s", PGM, AOM_stdin1);

  return;
}
