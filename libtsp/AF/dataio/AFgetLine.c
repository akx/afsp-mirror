/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  char *AFgetLine (FILE *fp, enum AF_ERR_T *ErrCode);

Purpose:
  Read a line of text from a file

Description:
  This routine retrieves a line of input from a file.  This routine reads
  characters until a newline is encountered.  The newline is not returned.
  The return value is a pointer to the text string or NULL if end-of-file is
  encountered.

  If the text file has Windows line endings CR/LF, the carriage control
  character will be left intact.  Note that the CR character is considered a
  white-space character.

Parameters:
  <-  char *AFgetLine
      Pointer to the text string.  This is NULL if end-of-file is encountered
      and the line is empty.  Otherwise, when end-of-file is encountered, the
      line is treated as if it were terminated with a newline.  The text string
      is in an internal storage area; each call to this routine overlays this
      storage. The pointer is also NULL if Errcode is set.
   -> FILE *fp
      File pointer to the file
  <-> enum AF_ERR_T ErrCode
      Error code.  This value is set if an I/O error is detected, but otherwise
      remains unchanged.  If ErrCode is set on input, this routine is a no-op.

Author / revision:
  P. Kabal  Copyright (C) 2017
  $Revision: 1.15 $  $Date: 2017/06/28 23:51:11 $

-------------------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>

#include <libtsp.h>
#include <AFpar.h>
#include <libtsp/AFdataio.h>
#include <libtsp/AFmsg.h>

#define NBUF  256     /* buffer size */


char *
AFgetLine (FILE *fp, enum AF_ERR_T *ErrCode)

{
  char *p;
  int nc;
  static char line[NBUF];

  if (*ErrCode)
    return NULL;

  /* Read a line or partial line */
  line[0] = '\0';
  p = fgets (line, NBUF, fp);

  /* Check for error or empty line at end-of-file */
  if (p == NULL) {
    if (ferror (fp)) {
      UTsysMsg ("AFgetLine: %s", AFM_ReadErr);
      *ErrCode = AF_IOERR;
    }
    return NULL;
  }

  /* Check that end-of-line has been reached */
  /* Find the number of characters up to the terminating '\0'
     - If the last character is a newline
         - Successful read
         - Delete the newline
     - If the last character is not a newline
         - If the number of characters is equal to the buffer size
           - Declare a "line too long error"
           If the number of characters is smaller than the buffer size
           - If at end-of-file
             - Continue, the next read will return a NULL line, signalling
               the end-of-file
           - If not at end-of-file
             - Possible '\0' character was encountered messing up the line
               length count
             - Declare this case a decoding error
  */
  nc = (int) strlen (line);
  if (line[nc-1] == '\n')
    line[--nc] = '\0';        /* Replace '\n' by '\0' */
  else if (nc >= NBUF-1) {
    UTwarn ("AFgetLine - %s", AFM_LongLine);
    *ErrCode = AF_DECERR;
  }
  else if (! feof (fp)) {
    UTwarn ("AFgetLine - %s", AFM_TextErr);
    *ErrCode = AF_DECERR;
  }

  return line;
}
