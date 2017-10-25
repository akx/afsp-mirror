/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  int STtrim (const char Si[], char So[])

Purpose:
  Copy a string, trimming leading and trailing white-space

Description:
  This routine copies characters from the input string to the output string.
  Leading white-space characters and trailing white-space characters are
  omitted.  The input and output strings are terminated by NUL characters.

Parameters:
  <-  int STtrim
      Number of characters in the output string (not including the trailing
      NUL character)
   -> const char Si[]
      Input character string
  <-  char So[]
      Output character string.  The output string pointer can be the same as
      the input string pointer.  If So is different from Si, then So should
      be at a minimum, the same size as Si in case no white-space is trimmed.

Author / revision:
  P. Kabal  Copyright (C) 2017
  $Revision: 1.13 $  $Date: 2017/05/24 16:54:43 $

-------------------------------------------------------------------------*/

#include <ctype.h>
#include <string.h>

#include <libtsp.h>


int
STtrim (const char Si[], char So[])

{
  const char *p;
  const char *q;
  const char *r;
  char *s;
  int nc;

  /* Find the first non-white-space character */
  for (p = Si; isspace ((int) *p); ++p)
    ;
  /* p points to the first non-white-space character (could be NUL) */

  /* Find the end of the string */
  for (q = p; *q != '\0'; ++q)
    ;
  /* q points to the NUL character */

  /* Find the last non-white-space character  */
  r = q;
  for (r = q; r > p && isspace ((int) *(r-1)); --r)
    ;
  /* r points to the position that a NUl should be placed to trim the end */

/* 1. String contains at least one non-white-space character
   - p points to first non-white-space character
     - p > Si means that initial white-space will be trimmed
   - q points to the NUL in the input string
   - r points to the position that a NUL should appear to trim the end
     - p < r <= q
     - r < q indicates that trailing white-space will be trimmed
   2. String contains no non-white-space characters
     - p points to a NUL character
       p = q = r
     - p > Si means that initial white-space will be trimmed
*/
  nc = (int) (r - p);

/* In the case that the input and output string pointers are the same, it may
   not be necessary to copy the string.
   - The body of the string needs to be copied only if i) the string pointers
     are different (So != Si) OR ii) the initial white-space needs to be trimmed
     (p > Si)
   - Otherwise, we have the case that the input and output string pointers are
     the same AND there is no initial white-space to be trimmed
     - If trailing white-space needs to be trimmed, only the NUL character
       marking the end of the string needs to be written
     - Otherwise, nothing needs to be done
*/
  if (p > Si || So != Si) {
    /* Copy the trimmed string to the output string */
    for (; p < r; )
      *(So++) = *(p++);
    *So = '\0';     /* Add a trailing null */
  } else if (r < q) {
    s = (char *) r;
    *s = '\0';
  }

  return nc;
}
