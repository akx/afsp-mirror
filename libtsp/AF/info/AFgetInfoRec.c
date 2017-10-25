/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  const char *AFgetInfoRec (const char *RecID[], const struct AF_info *AFInfo)

Purpose:
  Find a named record in an information record structure

Description:
  This routine searches through records in an AFsp information structure,
  looking for a match to a record with a given record identifier.  The input is
  a list of potential record identifiers.  These are checked one by one with all
  the information records.  For the first record identifier found, a pointer to
  the data value for that record is returned.

  The records are separated by nulls.  If an information record does not have
  a terminating null, the last unterminated record will not be checked.

  AFsp Information Records:
    The header information records are separated by null characters.  The name
    field is the first part of the record and the value field is the remainder.
    Standard names are terminated by a ':' character, which delimits the name
    field from the value field.

Parameters:
  <-  const char *AFgetInfoRec
      Pointer to a null terminated value string in the information string.
      Initial white-space has been skipped over.  If no record identifier
      matches, a NULL pointer is returned.
   -> const char *RecID[]
      List of pointers to identifier strings to be matched.  The list is
      terminated with a NULL pointer.  Each record identifier normally includes
      a terminating ':' character.  An empty identifier string will match the
      first record.
   -> const struct AF_info *AFInfo
      AFsp information structure

Author / revision:
  P. Kabal  Copyright (C) 2017
  $Revision: 1.39 $  $Date: 2017/05/24 16:17:28 $

-------------------------------------------------------------------------*/

#include <string.h> /* memchr definition */

#include <AFpar.h>
#include <libtsp/nucleus.h>

/* Local function */
static const char *
AF_getInfoRec (const char Ident[], const struct AF_info *AFInfo);


const char *AFgetInfoRec (const char *RecID[], const struct AF_info *AFInfo)

{
  const char *p;
  int i;

/* Search for named records in an AFsp information structure */
  for (i = 0; RecID[i] != NULL; ++i) {
    p = AF_getInfoRec (RecID[i], AFInfo);
    if (p != NULL) {
      p = STtrimIws (p);
      break;
    }
  }

  return p;
}

static const char *
AF_getInfoRec (const char Ident[], const struct AF_info *AFInfo)

{
  const char *p, *q, *t;
  int NI, NID, Nr;

  if (AFInfo == NULL || AFInfo->Info == NULL || AFInfo->N <= 0)
    return NULL;

  NI = AFInfo->N;
  t = &AFInfo->Info[NI-1];
  p = AFInfo->Info;
  NID = (int) strlen (Ident);
  while (p <= t) {

    /* Find the record terminator */
    q = (char *) memchr (p, '\0', (int) (t - p) + 1);     /* Look for '\0' */
    if (q == NULL)
      break;            /* Terminating '\0' not found */

    /* Check for a name match */
    Nr = (int) (q - p) + 1;     /* Record length, including null */
    if (Nr > NID && memcmp (p, Ident, NID) == 0)
      return (p + NID);

    p = q + 1;
  }

/* No match to the name */
  return NULL;
}
