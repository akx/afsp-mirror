/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  void AFaddInfoRec (const char Ident[], const char text[], int Size,
                     struct AF_info *AFInfo)

Purpose:
  Append a line of text as a named record in an information record structure

Description:
  If the input text is not empty (Size > 0), a record is added to the
  information record structure.  This record consists of an identification
  string followed by the text.  Additional editing of the text string is
  performed:
    - Trailing null characters are removed
    - Trailing white-space characters are removed
      The text will end up being null terminated.
    - Carriage return / newline combinations are replaced by a newline (WAVE
      DISP chunks can contain these)
    - Embedded nulls are replaced by newlines
    - Carriage returns are replaced by newlines
    - A space is inserted between the identifier and text if there is no
      white-space there
  If the text after editing is empty, only the identification string is added to
  the information structure.

Parameters:
  <-  void AFaddInfoRec
   -> const char Ident[]
      Identification string to be prefixed to the text. Standard identifying
      strings have a terminating ':' character.
   -> const char text[]
      Input text
   -> int Size
      Number of characters in the text string
  <-> struct AF_info *AFInfo
      AFsp information structure

Author / revision:
  P. Kabal  Copyright (C) 2017
  $Revision: 1.17 $  $Date: 2017/07/13 15:35:35 $

-------------------------------------------------------------------------*/

#include <ctype.h>
#include <string.h>

#include <libtsp.h>
#include <AFpar.h>
#include <libtsp/nucleus.h>
#include <libtsp/AFinfo.h>

#define NBUFF 512

/* Local functions */
static int
AF_trimTail (const char Si[], int nc, char So[]);
static int
AF_fixCR (const char Si[], int nc, char So[]);


void
AFaddInfoRec (const char Ident[], const char text[], int Size,
              struct AF_info *AFInfo)

{
  int Ni, Nt;
  char buff[NBUFF+1];
  char *t;

  if (text == NULL)
    Size = 0;
  Ni = (int) strlen (Ident);

  /* Allocate a buffer for long strings */
  if (Ni + 1 + Size > NBUFF)
    t = (char *) UTmalloc (Ni + Size + 2);  /* Leave room for blank and NULL */
  else
    t = buff;

  /* Insert the identifier */
  memcpy (t, Ident, Ni);
  if (Ni > 0   && ! isspace ((int) t[Ni-1]) &&
      Size > 0 && ! isspace ((int) text[0]))
    t[Ni++] = ' ';      /* Add a space between the identifier and the text */
  Nt = Ni;

  /* Append the text and sanitize the string */
  if (Size > 0)
    Nt += AF_trimTail (text, Size, &t[Ni]);
  Nt = AF_fixCR (t, Nt, t);             /* Nt now includes a trailing NUL) */

  /* Add the string to the info structure */
  AFaddInfoChunk (t, Nt, AFInfo);

  /* Release the buffer */
  if (Ni + 1 + Size > NBUFF)
    UTfree (t);

  return;
}

/*
  const char Si[] - String with nc characters, not including the trailing null
   - replace '\r' '\n' combinations with '\n'
   - replace '\r' with '\n'
   - replace an embedded '\0' with '\n'
   A trailing null character is added.

   The number of characters, including the trailing null, is returned.
*/


static int
AF_fixCR (const char Si[], int nc, char So[])

{
  int i, j;

  for (i = 0, j = 0; i < nc; ++i, ++j) {
    if (Si[i] == '\r' && Si[i+1] == '\n')
      So[j] = Si[++i];                        /* Skip over '\r' before '\n' */
    else if (Si[i] == '\0' || Si[i] == '\r')  /* '\0' or '\r' -> '\n' */
      So[j] = '\n';
    else
      So[j] = Si[i];
  }
  nc = j;

  /* Add a trailing null */
  So[nc++] = '\0';

  return nc;
}

/* Trim trailing NUL characters and then trim trailing white-space characters.
   The input and output string pointers may be the same.
*/

  /* Input string count nc may or may not include a trailing NUL
     The output character count does not include any trailing NUL */

static int
AF_trimTail (const char Si[], int nc, char So[])

{
  /* Remove trailing NUL characters */
  for (; nc > 0; --nc) {
    if (Si[nc-1] != '\0')
      break;
  }

  /* Trim trailing white-space */
  for (; nc > 0; --nc) {
    if (! isspace ((int) Si[nc-1]))
      break;
  }

  if (Si != So)
    memcpy (So, Si, nc);

  return nc;
}
