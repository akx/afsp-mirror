/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  void UTfree (void *ptr)

Purpose:
  Deallocate a block of memory

Description:
  This routine deallocates a block of memory allocated by UTmalloc (or malloc).

Parameters:
  <-  void UTfree
   -> void *ptr
      Pointer to the memory to be deallocated.  If ptr is NULL, no action is
      taken.

Author / revision:
  P. Kabal  Copyright (C) 2017
  $Revision: 1.17 $  $Date: 2017/05/05 00:21:40 $

-------------------------------------------------------------------------*/

#include <errno.h>
#include <stdlib.h>   /* free */

#include <libtsp.h>


void
UTfree (void *ptr)

{
  if (ptr != NULL) {    /* For non-ANSI compliant versions of free() */
    errno = 0;
    free (ptr);
    if (errno != 0)
      UTerror ("UTfree: Error detected in free()");
  }
  return;
}
