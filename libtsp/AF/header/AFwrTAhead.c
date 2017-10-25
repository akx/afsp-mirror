/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
AFILE *AFwrTAhead (FILE *fp, struct AF_write *AFw)

Purpose:
  Write header information to a text audio file

Description:
  This routine writes header information to a text audio file.  Each line of the
  file header starts with a '%' character.  The minimal information written is
  as follows.

    %//
    %
    %date: <YYYY-MM-DD HH:MM:SS UTC>
    %program: <program name>      if set using UTsetProg("<program name>")
    %channels: <no. channels>
    %full_scale: <full scale value>
    %sample_rate: <value>

  If the number of samples is known (AFopt.Nframe, where AFopt is defined in the
  AFpar.h header file), the number of samples per channel is written to the
  header
    %frames: <no. samples/channel>
  Additional user supplied information records are also written to the header.

Parameters:
  <-  AFILE *AFwrTAhead
      Audio file pointer for the audio file.  This routine allocates the
      space for this structure.  If an error is detected, a NULL pointer is
      returned.
   -> FILE *fp
      File pointer for the audio file
  <-> struct AF_write *AFw
      Structure containing file parameters

Author / revision:
  P. Kabal  Copyright (C) 2017
  $Revision: 1.2 $  $Date: 2017/05/15 15:43:17 $

-------------------------------------------------------------------------*/

#include <assert.h>
#include <setjmp.h>
#include <string.h>

#include <libtsp.h>
#include <AFpar.h>
#include <libtsp/nucleus.h>
#include <libtsp/AFheader.h>
#include <libtsp/AFmsg.h>


/* setjmp / longjmp environment */
extern jmp_buf AFW_JMPENV;


AFILE *
AFwrTAhead (FILE *fp, struct AF_write *AFw)

{
  AFILE *AFp;
  int N;
  char *Info;
  char *p, *pst, *pend;

  assert (AFw->DFormat.Format == FD_TEXT || AFw->DFormat.Format == FD_TEXT16);

/* Set the long jump environment; on error return a NULL */
  if (setjmp (AFW_JMPENV))
    return NULL; /* Return from a header write error */

/* Write the file ID and a following blank comment line */
  fprintf (fp, "%%//\n%%\n");

/* Write the information records */
  Info = AFw->WInfo.Info;
  N = AFw->WInfo.N;
  pst = &Info[0];
  pend = &Info[N - 1];
  while (pst <=  pend) {
    p = memchr (pst, '\0', pend - pst + 1);
    if (p == NULL)
      break;
    fprintf (fp, "%%%s\n", pst);
    pst = ++p;    /* Past the NUL character **/
  }

/* Write an end of header line */
  fprintf (fp, "%%//\n");

/* Set the parameters for file access */
  AFp = AFsetWrite (fp, FT_TXAUD, AFw);

  return AFp;
}
