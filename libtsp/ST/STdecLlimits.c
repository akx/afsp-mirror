/*-------------- Telecom ht munications & Signal Processing Lab ---------------
                             McGill University

Routine:
  int STdecLlimits (const char String[], long int Lim[2], int Mode)

Purpose:
  Decode a pair of limits for long integer values

Description:
  This routine decodes a string specifying a pair of long integer values.
  THe limits are specified in the form "n" or "n:m", for example "-23 : 45".
  Optional white-space (as defined by isspace) can surround the values.  Default
  values for the limits can be given.

  The table below shows how the processing carried out in the different modes.
  The initial values of Lim[0]:Lim[1] are indicated across the top.  Table
  entries marked with an X are not allowed and generate an error.
                        Mode
              0       1       2       3       4
    input    x:x     x:x     N:U     L:M     L:U
    --------------------------------------------
    "   "     X       X       X       X       X
    "n  "     X      n:n     N:N+n-1  X       X
    " : "     X       X       X       X       X
    "n: "     X       X      n:U     n:M     n:U
    " :m"     X       X       X      L:m     L:m
    "n:m"    n:m     n:m     n:m     n:m     n:m

  Mode 0: The initial values of Lim are ignored.  Only an input of the form
  n:m is allowed.

  Mode 1: The initial values of Lim are ignored.  The special case of an input
  of the form "n" (no colon or second value) is interpreted to be "n:n".

  Mode 2: The initial values of Lim are used.  The special case of an input of
  the form "n" (no colon or second value) is interpreted as N:N+n-1, where N
  is the initial value of Lim[0].  An input of the form "n:" is interpreted as
  "n:U" where U is the default value of Lim[1].  In practice U can be a special
  value that indicates that the upper limit is yet to be determined.

  Mode 3: The initial values of Lim are used.  An input of the form "n:" is
  interpreted to be "n:M" where M is the initial value of Lim[1].  An input of
  the form ":m" is interpreted as "L:m" where L is the initial value of Lim[0].
  In practice L can be a special value that indicates that the lower limit is
  yet to be determined.

  Mode 4: The initial values of Lim are used.  An input of the form "n:" is
  interpreted as "n:U" where U is the initial value of Lim[1].  An input of the
  form ":m" is interpreted as "L:m".  Both L and U can be a special value that
  indicates that these limits have yet to be determined.

  In all modes, an input of the form "n:m" ignores the initIal values set in
  Lim.

Parameters:
  <-  int STdecLimits
      Error status, 0 for no error, 1 for a decoding or format error
   -> const char String[]
      Input string
  <-> long int Lim[2]
      Initial limits modified by the decoded limits.  If an error is detected,
      the initial limits are not changed.
   -> int Mode
      This flag determines if and how the initial values of Lim[] are used to
      determine the output values.

Author / revision:
  P. Kabal  Copyright (C) 2018
  $Revision: 1.3 $  $Date: 2018/11/15 16:11:54 $

-------------------------------------------------------------------------*/

#include <assert.h>

#include <libtsp.h>
#include <libtsp/nucleus.h>
#include <libtsp/STmsg.h>

#define MAXC  22
#define DP_EMPTY    0
#define DP_LVAL     1
#define DP_DELIM    2
#define DP_RVAL     4

#define NCOLUMN(array)  ((int) ((sizeof array) / (sizeof array[0][0])))

/*                              Mode
                      0       1       2       3       4
    status  input    x:x     x:x     N:U     L:M     L:U
    -----------------------------------------------------
       0    "   "     X       X       X       X       X
       1    "n  "     X       2       3       X       X
       2    " : "     X       X       X       X       X
       3    "n: "     X       X       1       1       1
       4              X       X       X       X       X
       5              X       X       X       X       X
       6    " :m"     X       X       X       1       1
       7    "n:m"     1       1       1       1       1

    X codes to 0, normal cases to 1, special cases to 2 and 3
*/

/* Table of not allowed, allowed, special 1, special 2 */
static int Tallow[8][5] =
        { {0, 0, 0, 0, 0},     /* status = 0 */
          {0, 2, 3, 0, 0},     /* status = 1 */
          {0, 0, 0, 0, 0},
          {0, 0, 1, 1, 1},
          {0, 0, 0, 0, 0},
          {0, 0, 0, 0, 0},
          {0, 0, 0, 1, 1},
          {1, 1, 1, 1, 1} };


int
STdecLlimits (const char String[], long int Lim[2], int Mode)

{
  int status, Allow;
  long int DLim[2];

  /* Save the initial values of Lim */
  DLim[0] = Lim[0];
  DLim[1] = Lim[1];

  assert (Mode >= 0 && Mode <= 4);

  /* Decode the range values */
  status = STdecPair (String, ":", 'L', (void *) (&DLim[0]),
                                        (void *) (&DLim[1]));
  if (status < 0)
    return 1;

  /* Index into the allowed table */
  assert (status >= 0 && status < NCOLUMN (Tallow) / 4);
  Allow = Tallow[status][Mode];

  switch (Allow) {
    case 1:
      Lim[0] = DLim[0];
      Lim[1] = DLim[1];
      break;
    case 2:
      Lim[0] = DLim[0];
      Lim[1] = DLim[0];
      break;
    case 3:
      Lim[1] = Lim[0] + DLim[0] - 1;
      break;
    default:
      UTwarn ("STdecLlimits - %s: \"%s\"", STM_DataErr,
              STstrDots (String, MAXC));
      return 1;
      break;
  }

  return 0;
}
