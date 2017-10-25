/*-------------- Telecommunications & Signal Processing Lab ---------------
                            McGill University

Routine:
  int AFgetDelInfoRec (const char *RecID[], struct AF_info *AFInfo, char text[],
                       int NcMax)

Purpose:
  Retrieve the text from an information record; delete the information record in
  the information record structure

Description:
  The input consists of a NULL terminated array of pointers to identifiers for
  information records.  The identifiers are used to find an information record.
  The text from the first match is returned. The information record is then
  deleted from the information record structure.

  The returned text skips a leading blank character.

Parameters:
   <- AFgetDelInfoRec
      Number of characters in the text string, not including the terminating
      null character
   -> const char *RecID[]
      List of pointers to identifier strings to be matched.  The list is
      terminated with a NULL pointer.
  <-> struct AF_info *AFInfo
      Information record structure
  <-  char text[]
      Text from the information record (null terminated)
   -> int NcMax
      Maximum length of the text (not including the terminating null)

Author / revision:
  P. Kabal  Copyright (C) 2017
  $Revision: 1.7 $  $Date: 2017/05/24 16:17:10 $

-------------------------------------------------------------------------*/

#include <ctype.h>
#include <string.h>

#include <libtsp.h>
#include <AFpar.h>
#include <libtsp/nucleus.h>
#include <libtsp/AFinfo.h>
#include <libtsp/AFmsg.h>

static const char *Ident[] = {NULL, NULL};


int
AFgetDelInfoRec (const char *RecID[],  struct AF_info *WInfo, char text[],
                 int NcMax)

{
  const char *p;
  int k, Nc;

  Nc = 0;
  p = NULL;
  for (k = 0; RecID[k] != NULL; ++k) {
     Ident[0] = RecID[k];
     p = AFgetInfoRec (Ident, WInfo);
     if (p != NULL) {
       Nc = (int) strlen (p);
       if (Nc <= NcMax) {
         STcopyMax (p, text, NcMax);
         AFdelInfoRec (Ident[0], WInfo);
       }
       else {
         UTwarn ("AFopnWrite/AFupdHeader - %s", AFM_LongInfo);
         text[0] = '\0';
         Nc = 0;
       }
       break;
    }
  }
  return Nc;
}
