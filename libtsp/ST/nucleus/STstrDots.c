/*-------------- Telecommunications & Signal Processing Lab -3-------------
                             McGill University

Routine:
  char *STstrDots (const char Si[], int Maxchar)

Purpose:
  Generate a line of text with at most Maxchar characters (terminated with ...)

Description:
  This routine copies an input string to an output string.  The output string
  is at most Maxchar characters long.  If the input string is longer than
  Maxchar characters long, the algorithm below tries to truncate the line at
  a blank or punctuation and add an ellipsis "...").

  The input string is processed to change each tab to a space; LF, CR, VT, and
  FF control mark the end of lines; non-printable characters (control characters
  and undefined characters as defined in the  Windows-1252 character map) are
  converted to the generic currency symbol (scarab).

  If the input has multiple lines, only the characters from the first line are
  returned, but an ellipsis is printed to indicate that characters in subsequent
  lines have been omitted.

  The algorithm to place the ellipsis for a string which exceeds Maxchar in
  length is as follows.  Let the length of the first line be M characters.
  2. The input has a single line and M <= Maxchar
     output: line[0:M-1]
  2. The input has multiple lines and M <= Maxchar-3,
     output: line[0:M-1], "..."
  3. If M > Maxchar-3, search for the last separator (space or punctuation
     character) in the line at positions [Maxchar-10:Maxchar-3].  If a
     separator is found, let the position be k.  Normally, the separator is
     part of the output, but if the character is a period, or k == Maxchar-3,
     k is reduced so that the separator is not included.
     (a) No separator found,
         output: line[0:Maxchar-4], "..."
     (b) Separator found,,
         output: line[0:k] , "..."

  Examples:
      input : "It was the best of times, it was the worst"  42
               012345678901234567890123456789012345678901
               0        10        20        30        40
      Maxchar = 60
      output: "It was the best of times, it was the worst"  42
      Maxchar = 40
      output: "It was the best of times, it was the ..."    40
      Maxchar = 39
      output: "It was the best of times, it was the..."     39
      Maxchar = 28
      output: "It was the best of times,..."                28
      Maxchar = 4
      output: "I..."                                         4

      input : "Address http://www.McGill.CA/software.html"  42
               012345678901234567890123456789012345678901
               0        10        20        30        40
      Maxchar = 42
      output: "Address http://www.McGill.CA/software.html"  42
      Maxchar = 41
      output: "Address http://www.McGill.CA/software..."    40
      Maxchar = 33
      output: "Address http://www.McGill.CA/..."            32

Parameters:
  <-  char *STstrDots
      Output string.  This string is in an internally allocated area and is
      overwritten each time this routine is called.
   -> const char Si[]
      Input character string
   -> int Maxchar
      Maximum number of characters (not including the trailing null character)
      for the output string (at least 4, at most 1000)

Author / revision:
  P. Kabal  Copyright (C) 2018
  $Revision: 1.14 $  $Date: 2018/11/14 14:21:19 $

-------------------------------------------------------------------------*/

#include <libtsp/sysOS.h>
#if (SY_OS == SY_OS_WINDOWS)
#  define _CRT_SECURE_NO_WARNINGS     /* Suppress warning on strcpy */
#endif

#include <string.h>

#include <libtsp.h>
#include <libtsp/nucleus.h>

#define MINV(a, b)  (((a) < (b)) ? (a) : (b))
#define MAXV(a, b)  (((a) > (b)) ? (a) : (b))
#define MAXM    100
#define MINM    4


char *
STstrDots (const char Si[], int Maxchar)

{
  int i, k, N, M;
  int MultiLine;
  const char FillChar = '\xA4';   /* Generic currency symbol */
  static char Line[MAXM+1];

  enum CharType {
    CT_NL,    /* Control character = > newline */
    CT_CTL,   /* Control character, undefined character => FillChar */
      CT_HT,  /* Horizontal tab => space */
      CT_CR,  /* Carriage control => newline */
      CT_LF,  /* Line feed, omitted if follows CT_CR */
    CT_CHR,   /* Alphanumeric character, includes accented characters */
    CT_PUN,   /* Punctuation, includes spaces and extended punctuation
                characters */
  };
  enum CharType CType;

  /* Character types, as per Window-1252 character table
     - Some control characters are specifically noted
     - Some punctuation characters are specifically noted
  */
  const enum CharType CTypeTab[256] = {
  /* 0/8     1/9     2/A     3/B     4/C     5/D     6/E     7/F */
  CT_NL,  CT_CTL, CT_CTL, CT_CTL, CT_CTL, CT_CTL, CT_CTL, CT_CTL, /* 00 - 07 */
  CT_CTL, CT_HT,  CT_NL,  CT_NL,  CT_NL,  CT_NL,  CT_CTL, CT_CTL, /* 08 - 0F */
  CT_CTL, CT_CTL, CT_CTL, CT_CTL, CT_CTL, CT_CTL, CT_CTL, CT_CTL, /* 10 - 17 */
  CT_CTL, CT_CTL, CT_CTL, CT_CTL, CT_CTL, CT_CTL, CT_CTL, CT_CTL, /* 18 - 1F */

  CT_PUN, CT_PUN, CT_PUN, CT_PUN, CT_PUN, CT_PUN, CT_PUN, CT_PUN, /* 20 - 27 */
  CT_PUN, CT_PUN, CT_PUN, CT_PUN, CT_PUN, CT_PUN, CT_PUN, CT_PUN, /* 28 - 2F */
  CT_CHR, CT_CHR, CT_CHR, CT_CHR, CT_CHR, CT_CHR, CT_CHR, CT_CHR, /* 30 - 37 */
  CT_CHR, CT_CHR, CT_PUN, CT_PUN, CT_PUN, CT_PUN, CT_PUN, CT_PUN, /* 38 - 3F */
  CT_PUN, CT_CHR, CT_CHR, CT_CHR, CT_CHR, CT_CHR, CT_CHR, CT_CHR, /* 40 - 47 */
  CT_CHR, CT_CHR, CT_CHR, CT_CHR, CT_CHR, CT_CHR, CT_CHR, CT_CHR, /* 48 - 4F */
  CT_CHR, CT_CHR, CT_CHR, CT_CHR, CT_CHR, CT_CHR, CT_CHR, CT_CHR, /* 50 - 57 */
  CT_CHR, CT_CHR, CT_CHR, CT_PUN, CT_PUN, CT_PUN, CT_PUN, CT_PUN, /* 58 - 5F */
  CT_PUN, CT_CHR, CT_CHR, CT_CHR, CT_CHR, CT_CHR, CT_CHR, CT_CHR, /* 60 - 67 */
  CT_CHR, CT_CHR, CT_CHR, CT_CHR, CT_CHR, CT_CHR, CT_CHR, CT_CHR, /* 68 - 6F */
  CT_CHR, CT_CHR, CT_CHR, CT_CHR, CT_CHR, CT_CHR, CT_CHR, CT_CHR, /* 70 - 77 */
  CT_CHR, CT_CHR, CT_CHR, CT_PUN, CT_PUN, CT_PUN, CT_PUN, CT_CTL, /* 78 - 7F */

  CT_PUN, CT_CTL, CT_PUN, CT_CHR, CT_PUN, CT_PUN, CT_PUN, CT_PUN, /* 80 - 87 */
  CT_PUN, CT_PUN, CT_CHR, CT_PUN, CT_CHR, CT_CTL, CT_CHR, CT_CTL, /* 88 - 8F */
  CT_CTL, CT_PUN, CT_PUN, CT_PUN, CT_PUN, CT_PUN, CT_PUN, CT_PUN, /* 90 - 97 */
  CT_PUN, CT_PUN, CT_CHR, CT_PUN, CT_CHR, CT_CTL, CT_CHR, CT_CHR, /* 98 - 9F */

  CT_PUN, CT_PUN, CT_PUN, CT_PUN, CT_PUN, CT_PUN, CT_PUN, CT_PUN, /* A0 - A7 */
  CT_PUN, CT_PUN, CT_CHR, CT_PUN, CT_PUN, CT_PUN, CT_PUN, CT_PUN, /* A8 - AF */
  CT_PUN, CT_PUN, CT_CHR, CT_CHR, CT_PUN, CT_PUN, CT_PUN, CT_PUN, /* B0 - B7 */
  CT_PUN, CT_CHR, CT_CHR, CT_PUN, CT_PUN, CT_PUN, CT_PUN, CT_PUN, /* B8 - BF */
  CT_CHR, CT_CHR, CT_CHR, CT_CHR, CT_CHR, CT_CHR, CT_CHR, CT_CHR, /* C0 - C7 */
  CT_CHR, CT_CHR, CT_CHR, CT_CHR, CT_CHR, CT_CHR, CT_CHR, CT_CHR, /* C8 - CF */
  CT_CHR, CT_CHR, CT_CHR, CT_CHR, CT_CHR, CT_CHR, CT_CHR, CT_PUN, /* D0 - D7 */
  CT_CHR, CT_CHR, CT_CHR, CT_CHR, CT_CHR, CT_CHR, CT_CHR, CT_CHR, /* D8 - DF */
  CT_CHR, CT_CHR, CT_CHR, CT_CHR, CT_CHR, CT_CHR, CT_CHR, CT_CHR, /* E0 - E7 */
  CT_CHR, CT_CHR, CT_CHR, CT_CHR, CT_CHR, CT_CHR, CT_CHR, CT_CHR, /* E8 - EF */
  CT_CHR, CT_CHR, CT_CHR, CT_CHR, CT_CHR, CT_CHR, CT_CHR, CT_PUN, /* F0 - F7 */
  CT_CHR, CT_CHR, CT_CHR, CT_CHR, CT_CHR, CT_CHR, CT_CHR, CT_CHR, /* F6 - FF */
  };

  N = (int) strlen (Si);
  Maxchar = MINV(MAXM, Maxchar);
  Maxchar = MAXV(MINM, Maxchar);

  /* Clean up the non-printing characters, search for an end-of-line */
  MultiLine = 0;
  k = 0;
  for (i = 0; i < N; ++i) {
    CType = CTypeTab[(int) ((unsigned char) Si[i])];

    if (CType == CT_NL) {
      MultiLine = 1;
      break;
    }
    else if (CType == CT_HT)
      Line[k++] = ' ';
    else if (CType == CT_CTL)
      Line[k++] = FillChar;
    else
      Line[k++] = Si[i];
  }
  Line[k] = '\0';
  M = k;

  /* No truncation of the first line */
  if (! MultiLine && M <= Maxchar)
    return Line;
  else if (MultiLine && M <= Maxchar-3) {
    strcpy (&Line[M], "...");
    return Line;
  }

  /* Ellipsis required */
  for (k = Maxchar-3; k >= MAXV(Maxchar-10, 0); --k) {
    CType = CTypeTab[(int) ((unsigned char) Line[k])];

    /* Separator found */
    if (CType == CT_PUN) {
      if (k == Maxchar-3 || Line[k] == '.')
        --k;
      strcpy (&Line[k+1], "...");
      return Line;
    }
  }

  /* No separator found */
  strcpy (&Line[Maxchar-3], "...");
  return Line;

}
