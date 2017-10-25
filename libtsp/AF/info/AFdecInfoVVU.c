/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  int AFdecInfoVVU (const char *String, int Type, void *Val,
                    const char *UnitsTable[], int *Key)

Purpose:
  Decode numerical values and a corresponding units specification

Description:
  This routine decodes values of the following forms:
    (1) single value, e.g. "32000"
    (2) pair of values, e.g. "20000/2"
    (3) single value with a units specification, e.g. "19.98 kHz"
    (4) pair of values with a units specification, e.g. "20/3 kHz"
  This routine returns the number of values decoded, 0, 1, or 2.  The number of
  values decoded is set to zero if a decoding error occurs for either value, or
  if the units specification does not match one of a table of possible units
  specifications.

  The values can be of different numerical types (see the Type argument).

Parameters:
  <-  int AFdecInfoVVU
      The returned value gives the number of values successfully decoded.
        0 - A decoding error has occurred
        1 - A single value has been decoded
        2 - Two values have been decoded
   -> const char String[]
      Input string
   -> int Type
      Character code for the data type of the returned values: 'D' for double,
      'F' for float, 'I' for int, 'L' for long int
  <-  void *Val
      Point to the first of a two element array of the appropriate type as
      specified by the Type argument.  If the return value is positive, the
      corresponding number of elements of Val have been set.
   -> const char *UnitsTable[]
      Pointer array with pointers to the units specification keyword strings.
      The end of the keyword table is signalled with a NULL pointer.  Note that
      with ANSI C, if the actual parameter is not declared to have the const
      attribute, an explicit cast to (const char **) is required.  If UnitsTable
      itself is NULL, a no match condition is returned.  A valid units
      specification must be included as part of the input String unless
      UnitsTable is NULL or one of the units keyword entries is the empty
      string "".
  <-  int *Key
      Index of the matched units keyword.  This value is set to -1 if no match
      is found.  Key is not modified if the units keyword table is also NULL.
      Key can be NULL, otherwise the value is as follows.
       -1 - No match
        0 - Match to the first keyword
        1 - Match to the second keyword
       ...

Author / revision:
P. Kabal
$Revision: 1.5 $  $Date: 2017/06/22 11:39:45 $

-------------------------------------------------------------------------*/

#include <libtsp.h>
#include <libtsp/nucleus.h>
#include <libtsp/AFinfo.h>

#define WS_TRIM     1      /* Trim white-space */
#define WS_DELIM    2      /* Use white-space as a delimiter */
#define NCBUF       100

int
AFdecInfoVVU (const char *String, int Type, void *Val,
              const char *UnitsTable[], int *Key)

{
  const char *p;
  const char *Delim;
  char Token[NCBUF+1];
  int k, Nval, WSFlag, key;
  void *v[2];

  Nval = 0;             /* Number of values decoded */
  if (String == NULL)
    return Nval;

  /* Set up pointers to the two data elements */
  v[0] = Val;
  switch (Type) {
  case 'D':
    v[1] = (void *) (((double *) Val) + 1);
    break;
  case 'F':
    v[1] = (void *) (((float *) Val) + 1);
    break;
  case 'I':
    v[1] = (void *) (((int *) Val) + 1);
    break;
  case 'L':
    v[1] = (void *) (((long int *) Val) + 1);
    break;
  }

/* The decoding of the parameters is done using features of STfindToken().
   With a delimiter of "/" and WSFlag set to WS_TRIM:
   - The returned value of p (the next character to be scanned) is set in one
     of two ways.
     1. NULL. This occurs if there is no "/" character. Token will be the entire
        input string, trimmed of leading and trailing white-space (and with
        possible embedded white-space). Token is not of much use and will be
        ignored.
     2. Pointer to the first non-white-space character beyond the "/" character.
        The strategy is as follows.
        k == 1
          - Invoke AFfindToken with "/" and WS_TRIM
          - If "/" is present (returned p != NULL), decode the first value from
            Token
          - If "/" is absent, ignore Token, reset p to the beginning of the
            string
        k == 2
          - Invoke AFfindToken with "" and WS_DELIM (only white-space as a
            delimiter)
          - Decode the second value from Token
        k == 3
          - Invoke AFfindToken with "" and WS_DELIM
          - Check Token (units) with STkeyMatch
*/
  Delim = "/";
  WSFlag = WS_TRIM;
  if (UnitsTable != NULL)
    key = -1;          /* No match value */
  k = 0;
  p = String;

/* Decode the number of bits per sample values */
  while (p != NULL) {
    ++k;
    p = STfindToken (p, Delim, "", Token, WSFlag, NCBUF);
    if (k == 1) {               /* Possible first value before a '/' */
      if (p != NULL) {
        if (STdec1val (Token, Type, v[Nval++])) {
          Nval = 0;
          break;
        }
      }
      else
        p = String;             /* Reset to beginning of string */

      Delim = "";               /* Switch to white-space delimiters */
      WSFlag = WS_DELIM;
    }
    else if (k == 2) {          /* Value token */
      if (STdec1val (Token, Type, v[Nval++])) {
        Nval = 0;
        break;
      }
    }
    else if (k == 3 && UnitsTable != NULL) {      /* Units token */
      key = STkeyMatch (Token, UnitsTable);
      if (key < 0) {
        Nval = 0;
        break;
      }
    }
    else {                       /* Unexpected token: error */
      Nval = 0;
      break;
    }
  }

  /* No units token, check for the presence of a "" keyword, if so
     declare a match */
  if (UnitsTable != NULL && k == 2) {
    key = STkeyMatch ("", UnitsTable);
    if (key < 0)
      Nval = 0;
  }
  if (Key != NULL)
    *Key = key;

  return Nval;
}
