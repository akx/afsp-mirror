/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  int AFrdHinfo (FILE *fp, int Size, struct AF_infoX *Hinfo, int Align)

Purpose:
  Read AFsp information records 

Description:
  This routine reads AFsp information records from a file.  These records are
  prefixed by an identification string ("AFsp").  The information records from
  the file are appended to existing records in the AFsp information structure.

  The last information record is null terminated if the header information is
  not truncated.

Parameters:
  <-  int AFrdHinfo
      Number of characters read (including any skipped characters)
   -> FILE *fp
      File pointer for the audio file
   -> int Size
      Number of characters to be read
  <-> struct AF_infoX *Hinfo
      AFsp information structure
   -> int Align
      Alignment requirement.  If Size is not a multiple of Align, file data
      up to the next multiple of Align is skipped, leaving the file positioned
      at that point.

Author / revision:
  P. Kabal  Copyright (C) 2001
  $Revision: 1.8 $  $Date: 2001/07/02 19:05:20 $

-------------------------------------------------------------------------*/

static char rcsid[] = "$Id: AFrdHinfo.c 1.8 2001/07/02 AFsp-v6r8 $";

#include <string.h>

#include <libtsp.h>
#include <libtsp/AFheader.h>
#include <libtsp/AFmsg.h>
#include <libtsp/AFpar.h>

#define ICEILV(n,m)	(((n) + ((m) - 1)) / (m))	/* int n,m >= 0 */
#define RNDUPV(n,m)	((m) * ICEILV (n, m))		/* Round up */

#define SAME_CSTR(str,ref) 	(memcmp (str, ref, sizeof (str)) == 0)


int
AFrdHinfo (FILE *fp, int Size, struct AF_infoX *Hinfo, int Align)

{
  int N, offs;
  char ID[4];

  N = 0;
  offs = 0;
  if (Size > 4) {
    offs += RHEAD_S (fp, ID);

    /* Read the header information records */
    if (SAME_CSTR (ID, FM_AFSP)) {
      N = Size - offs;
      if (N > Hinfo->Nmax) {
	UTwarn ("AFrdHinfo - %s", AFM_TruncInfo);
	N = Hinfo->Nmax;
      }
      offs += RHEAD_SN (fp, Hinfo->Info, N);
    }
  }
  offs += RSKIP (fp, RNDUPV (Size, Align) - offs);

  /* Strip redundant nulls */
  for (; N > 0; --N) {
    if (Hinfo->Info[N-1] != '\0')
      break;
  }

  /* Add a single terminating null */
  if (N > 0 && N < Hinfo->Nmax) {
    Hinfo->Info[N] = '\0';
    ++N;
  }

  Hinfo->N = N;

  return offs;
}
