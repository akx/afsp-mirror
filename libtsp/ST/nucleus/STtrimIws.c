/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  char *STtrimIws (const char Si[])

Purpose:
  Trim leading white-space

Description:
  This routine returns a pointer to the first character in a string that is not
  white-space (as defined by isspace).  If the input string consists entirely
  of white-space, this routine returns a pointer to the terminating null
  character.

Parameters:
  <-  char *STrimIws
      Pointer to the first non-white-space character
   -> const char Si[]
      Input character string

Author / revision:
  P. Kabal  Copyright (C) 2001
  $Revision: 1.7 $  $Date: 2001/11/30 13:38:24 $

-------------------------------------------------------------------------*/

static char rcsid[] = "$Id: STtrimIws.c 1.7 2001/11/30 AFsp-v6r8 $";

#include <ctype.h>

#include <libtsp/nucleus.h>


char *
STtrimIws (const char Si[])

{
  /* Find the first non-white-space character */
  for (; isspace ((int) *Si); ++Si)
    ;

  return ((char *) Si);
}
