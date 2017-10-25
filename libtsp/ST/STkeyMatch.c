/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  int STkeyMatch (const char String[], const char *KeyTable[])

Purpose:
  Match a string to keyword strings

Description:
  This subroutine searches a table for a match to a character string.  The table
  of keywords is searched for the first match.  The keyword table can optionally
  specify the minimum length for a match and a length beyond which characters
  need no longer match.  The keywords are searched in order.

  Each entry of the keyword table specifies the character string to be matched.
  An optional asterisk ('*') is used to indicate the minimum number of
  characters needed for a match.  A second asterisk can be used to indicate
  that characters after this point need not match.

  Example:
    Keyword table entry "abc*de*". Input strings "abc", "abcd", "abcde", and
    "abcdex" will match this entry.  Input strings "ab", "abC" " abc" and
    "abcx" do not match this entry.
  Special cases:
  - Only an empty string will match a keyword that is empty
  - An empty string will match a keyword that starts with an asterisk
  - A NULL pointer will not match any keyword
  - Any string will match a keyword that starts with two asterisks

Parameters:
  <-  int STkeyMatch
      Index of the matched keyword.  This value is set to -1 if no match is
      found.
       -1 - No match
        0 - Match to the first keyword
        1 - Match to the second keyword
      ...
   -> const char String[]
      Input character string.  If String is the NULL pointer, a no match
      condition is returned.
  ->  const char *KeyTable[]
      Pointer array with pointers to the keyword strings.  The end of the
      keyword table is signalled with a NULL pointer.  Note that with ANSI C,
      if the actual parameter is not declared to have the const attribute, an
      explicit cast to (const char **) is required.  If KeyTable itself is NULL,
      a no match condition is returned.

Author / revision:
  P. Kabal  Copyright (C) 2017
  $Revision: 1.27 $  $Date: 2017/06/02 10:56:50 $

-------------------------------------------------------------------------*/

#include <libtsp.h>

#define NOSTAR  0
#define STAR    1
#define NOMATCH -1


int
STkeyMatch (const char String[], const char *KeyTable[])

{
  int i;
  const char *kt;
  const char *s;
  int state;

/* Check for NULL String or NULL KeyTable */
  if (String == NULL || KeyTable == NULL)
    return NOMATCH;

  for (i = 0; KeyTable[i] != NULL; ++i) {

    /* Compare characters */
    state = NOSTAR;
    for (kt = KeyTable[i], s = String; *kt != '\0' && *s != '\0'; ++kt) {

      if (*kt == '*') {
        if (state == NOSTAR)
          state = STAR;     /* First asterisk */
        else
          return i;         /* Second asterisk */
      }
      else if (*s != *kt)
        goto Nexti;
      else
        ++s;
    }
/* Each keyword can be considered to have an implicit asterisk following the
   keyword if one has not been encountered yet */
/*
Check for:
(1) all characters matched (*s == '\0' && *kt == '\0')
(2) all characters up to this point matched, string finished (*s == '\0')
and at or past an asterisk)
*/
    if (*s == '\0' && (*kt == '\0' || state == STAR || *kt == '*'))
      return i;

Nexti:
    continue;
  }

return NOMATCH;
}
