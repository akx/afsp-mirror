/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  void UTsetProg (const char Program[]);
  char *UTgetProg (void)

Purpose:
  Set the program name for messages
  Get the program name for messages

Description:
  This routine sets a program name string.  This string is used by error
  reporting routines as an identifier in error message strings.

Parameters:
  UTsetProg:
   -> const char Program[]
      Program name, normally without a path
  UTgetProg:
  <-  char *UTgetProg
      Pointer to the program name.  If the program name has not been set, this
      is a pointer to an empty string.

Author / revision:
  P. Kabal  Copyright (C) 2009
  $Revision: 1.14 $  $Date: 2009/03/01 20:59:56 $

-------------------------------------------------------------------------*/

#include <string.h>

#include <libtsp.h>

static char *Pgm = NULL;


void
UTsetProg (const char Program[])

{
  int nc;

  UTfree ((void *) Pgm);
  nc = strlen (Program);
  if (nc > 0) {
    Pgm = (char *) UTmalloc (nc + 1);
	STcopyMax (Program, Pgm, nc);
  }
  else
    Pgm = NULL;
}



char *
UTgetProg (void)

{
  static char Empty[] = "";

  if (Pgm == NULL)
    return Empty;
  else
    return Pgm;
}
