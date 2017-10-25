/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  int STtrimTail (char So[])

Purpose:
  Trim trailing white-space

Description:
  This finds the last non-white-space character and writes a NUL character
  after that position if one is not already there.

Parameters:
  <-  int STtrimTail
      Number of characters in the trimmed string (not including the trailing
      NUL character)
  <-> char Si[]
      Input/Output character string (NUL terminated)

Author / revision:
  P. Kabal  Copyright (C) 2017
  $Revision: 1.2 $  $Date: 2017/07/12 17:35:19 $

-------------------------------------------------------------------------*/

#include <ctype.h>
#include <string.h>

#include <libtsp.h>


int
STtrimTail (char Si[])

{
  char *q;
  char *r;
  int nc;

  /* Find the end of the string */
  for (q = Si; *q != '\0'; ++q)
    ;
  /* q points to the NUL character */

  /* Find the last non-white-space character  */
  r = q;
  for (r = q; r > Si && isspace ((int) *(r-1)); --r)
    ;
  /* r points to the position that a NUL should be placed to trim the end */

/* 1. String contains at least one character
   - Si points to first character
   - q points to the NUL in the input string
   - r points to the position that a NUL should appear to trim the end
     - Si < r <= q
     - r < q indicates that trailing white-space will be trimmed
   2. String contains no non-NUL characters
     - Si points to a NUL character
       q = r = Si

*/
  nc = (int) (r - Si);

/* If trailing white-space needs to be trimmed, only the NUL character marking
   the end of the string needs to be written; otherwise, nothing needs to be
   done
*/

/* Write a NUL terminator if trailing white-space was found */
  if (r < q)
    *r = '\0';

  return nc;
}
