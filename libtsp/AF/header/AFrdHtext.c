/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  int AFrdHtext (FILE *fp, int Size, const char Ident[],
                 struct AF_infoX *Hinfo, int Align)
  void AFaddHtext (const char Ident[], const char line[], int Size,
                   struct AF_infoX *Hinfo)

Purpose:
  Read text and append as a record in an AFsp information structure
  Append a line of text as a record in an AFsp information structure

Description:
  These routines get a line of text, either from a file or from input.  If the
  text after trimming for white space, null characters and '377' characters, is
  not empty, a record is added to the AFsp information structure.  This record
  consists of an identification string followed by the text. Additional editing
  of the text string is performed:
    - Carriage return / newline combinations are replaced by a newline
    - Nulls are replaced by newlines
    - Carriage returns are replaced by newlines

Parameters:
  <-  int AFrdHtext
      Number of characters read (including any skipped characters)
   -> FILE *fp
      File pointer to the audio file
   -> int Size
      Number of characters in the text string
   -> const char Ident[]
      Identification string to be prefixed to the text
  <-> struct AF_infoX *Hinfo
      AFsp information structure
   -> int Align
      Alignment requirement.  If Size is not a multiple of Align, file data
      up to the next multiple of Align is skipped, leaving the file positioned
      at that point.

   -> const char Ident[]
      Identification string to be prefixed to the text
   -> const char line[]
      Input text line
   -> int Size
      Number of characters in the text string
  <-> struct AF_infoX *Hinfo
      AFsp information structure   

Author / revision:
  P. Kabal  Copyright (C) 2001
  $Revision: 1.15 $  $Date: 2001/11/10 05:04:52 $

-------------------------------------------------------------------------*/

static char rcsid[] = "$Id: AFrdHtext.c 1.15 2001/11/10 AFsp-v6r8 $";

#include <ctype.h>
#include <string.h>

#include <libtsp.h>
#include <libtsp/AFheader.h>
#include <libtsp/AFpar.h>

#define ICEILV(n,m)	(((n) + ((m) - 1)) / (m))	/* int n,m >= 0 */
#define RNDUPV(n,m)	((m) * ICEILV (n, m))		/* Round up */

#define NBUFF 511

static int
AF_trimSC (const char Si[], int nc, char So[]);


int
AFrdHtext (FILE *fp, int Size, const char Ident[], struct AF_infoX *Hinfo,
	   int Align)

{
  int offs;
  char buff[NBUFF+1];
  char *t;

  /* Allocate a buffer for long strings */
  if (Size > NBUFF)
    t = (char *) UTmalloc (Size + 1);
  else
    t = buff;

  /* Read the text */
  offs = RHEAD_SN (fp, t, Size);

  /* Add the text to the info structure */
  AFaddHtext (Ident, t, Size, Hinfo);

  /* Release the buffer */
  if (Size > NBUFF)
    UTfree (t);

  /* Move ahead in the file to a multiple of Align */
  offs += RSKIP (fp, RNDUPV (Size, Align) - offs);

  return offs;
}

/* Place a line into the info string */


void
AFaddHtext (const char Ident[], const char line[], int Size,
	    struct AF_infoX *Hinfo)

{
  int Nh, Ni, Nt;
  char buff[NBUFF+1];
  char *t;

  /* Allocate a buffer for long strings */
  if (Size > NBUFF)
    t = (char *) UTmalloc (Size + 1);
  else
    t = buff;

  /* Sanitize the string */
  Nt = AF_trimSC (line, Size, t);

  /* Add the string to the info structure */
  Nh = Hinfo->N;
  Ni = strlen (Ident);
  if (Nt > 0 && Nh + Ni + Nt + 1 <= Hinfo->Nmax) {
    strcpy (&Hinfo->Info[Nh], Ident);
    strcpy (&Hinfo->Info[Nh+Ni], t);
    Hinfo->N = Nh + Ni + Nt + 1;
  }

  /* Release the buffer */
  if (Size > NBUFF)
    UTfree (t);

  return;
}

/* Trim trailing white space, null characters and '\377' characters.
   (The latter occur as padding in some old audio files.)
   - replace \r\n combinations with \n (occur in some WAVE files)
   - replace \r with \n
   - replace \0 with \n
   A trailing null character is added.

   The number of characters, not including the trailing null, is returned.
*/


static int
AF_trimSC (const char Si[], int nc, char So[])

{
  int i, j;

  for (i = nc-1; i >= 0; --i) {
    if (! (isspace ((int)(Si[i])) || Si[i] == '\0' || Si[i] == '\377'))
      break;
  }
  nc = i + 1;

  for (i = 0, j = 0; i < nc; ++i, ++j) {
    if (Si[i] == '\r' && Si[i+1] == '\n') {
      ++i;
      So[j] = Si[i];
    }
    else if (Si[i] == '\0' || Si[i] == '\r')
      So[j] = '\n';
    else
      So[j] = Si[i];
  }
  nc = j;

  So[nc] = '\0';

  return nc;
}
