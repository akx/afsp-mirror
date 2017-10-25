/*-------------- Telecommunications & Signal Processing Lab ---------------
                            McGill University

Routine:
  int AFdelInfoRec (const char Ident[], struct AF_info *AFInfo)

Purpose:
  Delete an information record from an information record structure

Description:
  The input consists of an identifier for an information record.  The
  information record structure consists of null terminated records.  An
  information record starting with the given identifier is deleted.  The
  information is compacted to recover the space used by the deleted record.

Parameters:
   <- AFdelInfoRec
      Number of characters deleted in the information record structure
   -> const char Ident[]
      Identifier string
  <-> struct AF_info *AFInfo
      Information record structure

Author / revision:
  P. Kabal  Copyright (C) 2017
  $Revision: 1.3 $  $Date: 2017/05/24 16:17:00 $

-------------------------------------------------------------------------*/

#include <string.h>

#include <AFpar.h>
#include <libtsp/AFmsg.h>
#include <libtsp/nucleus.h>


int
AFdelInfoRec (const char Ident[], struct AF_info *AFInfo)

{
  char *r_st;
  char *r_term;
  char *h_end;
  int NI, NID, Nr, Nc;

  if (AFInfo == NULL || AFInfo->Info == NULL || AFInfo->N <= 0)
    return 0;

  NI = AFInfo->N;
  h_end = &AFInfo->Info[NI-1];     /* Last character in AFInfo->Info */
  r_st = AFInfo->Info;             /* First character in AFInfo->Info */
  NID = (int) strlen (Ident);
  while (r_st <= h_end) {

    /* Find the record terminator */
    r_term = (char *) memchr (r_st, '\0', h_end-r_st+1);  /* Look for '\0' */
    if (r_term == NULL)
      return 0;    /* Terminating '\0' not found */

    /* Check for a name match */
    Nr = (int) (r_term - r_st) + 1;     /* Record length, including null */
    if (Nr > NID && memcmp (r_st, Ident, NID) == 0 ) {
      /* Found a match, compact the information record structure */
      Nc = (int) (r_term - r_st) + 1;
      memmove (r_st, r_term + 1, (int) (h_end - r_term));
                                                  /* (h_end - (r_term+1) + 1 */
      AFInfo->N = NI - Nc;
      return Nc;
    }

    r_st = r_term + 1;
  }

/* No match to the name */
  return 0;
}
