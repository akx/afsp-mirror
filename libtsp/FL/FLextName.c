/*------------- Telecommunications & Signal Processing Lab -------------
                          McGill University

Routine:
  int FLextName (const char Fname[], char Ext[])

Purpose:
  Return the extension component of a file name

Description:
  This routine takes a filename and returns the filename extension.  The
  extension is the part of the last component of the path name beginning with
  (and including) a period.  The extension for the path name "/abc/def.gh.ij"
  is ".ij".

Parameters:
  <-  int FLextName
      Number of characters in the output string
   -> const char Fname[]
      Input character string with the file name
  <-  char Ext[]
      Output string with the extension.  This string is at most FILENAME_MAX
      characters long including the terminating null character.

Author / revision:
  P. Kabal  Copyright (C) 2001
  $Revision: 1.7 $  $Date: 2001/08/28 21:19:11 $

----------------------------------------------------------------------*/

static char rcsid[] = "$Id: FLextName.c 1.7 2001/08/28 AFsp-v6r8 $";

#include <string.h>

#include <libtsp.h>


int
FLextName (const char Fname[], char Ext[])

{
  char Bname[FILENAME_MAX];
  char *p;
  int n;

  FLbaseName (Fname, Bname);
  p = strrchr (Bname, '.');
  if (p != NULL)
    n = STcopyMax (p, Ext, FILENAME_MAX-1);
  else {
    Ext[0] = '\0';
    n = 0;
  }

  return n;
}
