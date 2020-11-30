/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  int AFrdInfoText(FILE *fp, int Size, const char Ident[],
                   struct AF_info *Info, int Align, int Mode)

Purpose:
  Read text and append as record(s) in an AFsp information structure

Description:
  This routine reads text (Size bytes) from a file. If the position is not
  already at a multiple of Align, it skips ahead in the file to the next
  multiple of the Align value.

  The tail end of the text read is trimmed to eliminate redundant trailing '\0'
  characters or '\377' fill characters (in old INRS-Telecom files). Then
  trailing white-space is trimmed. If the trimmed string is zero length, this
  routine does not add an information record to the information structure.

  Mode 0:
    This routine reads text from the file. It then creates a single info record
    with the given identifier, followed by the text read from the file. Any
    nulls in the text will be converted to newline characters.
  Mode 1:
    This routine reads text from the file. It then checks for an "AFsp" tag.
    1. "AFsp" tag found. The rest of the text will be treated as a number of
       information records, each normally with an identifier, text, and
       terminated by a null character.
    2. "AFsp" tag not found. The whole text is processed as a single record
       with the given identifier. Any nulls in the text will be converted to
       newline characters.
  Mode 2:
    This routine reads text from the file. It then checks for an "AFsp" tag.
    1. "AFsp" tag found. The rest of the text will be treated as a number of
       information records, each normally with an identifier, text, and
       terminated by  a null character.
    2. "AFsp" tag not found. A warning message is printed.

  The following macros are available for the three modes.
    Mode 0: AFrdInfoIdentText (fp, Size, Ident, Info, Align)
    Mode 1: AFrdInfoAFspIdentText (fp, Size, Ident, Info, Align)
    Mode 2: AFrdInfoAFspText (fp, Size, Info, Align)

Parameters:
  <-  int AFrdInfoText
      Number of bytes advanced in the file
   -> FILE *fp
      File pointer to the audio file
   -> int Size
      Number of characters to be read
   -> const char Ident[]
      Information record identifier
   -> struct AF_info *Info
      AFsp information record structure
   -> int Align
      Alignment requirement. If Size is not a multiple of Align, file data up
      to the next multiple of Align is skipped, leaving the file positioned at
      that point.
   -> int Mode
      For Mode = 0, the text is processed as a single information record with
      the given record identifier.
      For Mode = 1, if the text has an "AFsp" tag, the remaining text is treated
      as a number of information records. If the "AFsp" tag is missing, a single
      record with the Ident identifier is created.
      For Mode = 2, if the text has an "AFsp" tag, the remaining text is treated
      as a number of information records. If the "AFsp" tag is missing, a
      warning message is printed.

Author / revision:
  P. Kabal  Copyright (C) 2020
  $Revision: 1.13 $  $Date: 2020/11/26 11:29:47 $

-------------------------------------------------------------------------*/

#include <ctype.h>
#include <string.h>

#include <libtsp.h>
#include <AFpar.h>
#include <libtsp/AFheader.h>
#include <libtsp/AFinfo.h>
#include <libtsp/AFmsg.h>

#define ICEILV(n,m) (((n) + ((m) - 1)) / (m)) /* int n,m >= 0 */

#define SSIZE(str)  (sizeof(str) - 1)        /* ignore null */

#define NBUFF 512

/* Local functions */
static int
AF_trimTail(const char Si[], int nc);


int
AFrdInfoText(FILE *fp, int Size, const char Ident[], struct AF_info *Info,
             int Align, int Mode)

{
  int offs;
  int nc, NcAFsp;
  char buff[NBUFF+1];
  char *t;

  /* Allocate a buffer for long strings */
  if (Size > NBUFF)
    t = (char *) UTmalloc(Size + 1);
  else
    t = buff;

  /* Read the text; move ahead in the file to a multiple of Align */
  offs  = RHEAD_SN(fp, t, Size);
  offs += RSKIP(fp, RNDUPV(Size, Align) - offs);

  /* Trim the text */
  nc = AF_trimTail(t, Size);

  /* Check the mode */
  if (Mode == 0) {
    /* AFrdInfoText mode */
    if (nc > 0)
      AFaddInfoRec(Ident, t, nc, Info);
  }
  else if (Mode > 0) {
    /* Looking for an "AFsp" tag */
    NcAFsp = SSIZE(FM_AFSP);
    if (nc > NcAFsp && memcmp(t, FM_AFSP, NcAFsp) == 0) {
      /* Found AFsp tag, add pre-formatted information records */
      AFaddInfoChunk(t + NcAFsp, nc - NcAFsp, Info);
    }
    else if (Mode == 2) {
    /* Required AFsp identifier missing */
      UTwarn("AFaddInfoText - %s", AFM_MissingAFsp);
    }
    else {
      /* Add the Ident / text record */
      if (nc > 0)
        AFaddInfoRec(Ident, t, nc, Info);
    }
  }

  /* Release the buffer */
  if (Size > NBUFF)
    UTfree(t);

  return offs;
}

/* Trim trailing nulls or trailing '\377' characters; then trim trailing
   white-space.
   - The input string is left untouched
   - The returned value is the number of characters after trailing characters
     are ignored.
*/

static int
AF_trimTail(const char Si[], int nc)

{
  int Size;

  /* Find trailing NUL characters */
  Size = nc;
  for (; nc > 0; --nc) {
    if (Si[nc-1] != '\0')
      break;
  }
  /* Find trailing '\377' characters */
  if (Size == nc) {
    for (; nc > 0; --nc) {
      if (Si[nc-1] != '\377')
        break;
    }
  }
  /* Trim trailing white-space */
  for (; nc > 0; --nc) {
    if (!isspace((int) Si[nc-1]))
      break;
  }

  return nc;
}
