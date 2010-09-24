/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  char *UTgetUser (void)

Purpose:
  Get the user name

Description:
  This routine returns the user name.  The user name is determined by using
  the user id to find the name in the password database with this uid.  For
  systems that do not support the password database, the user name is taken
  from the environment variables USER, LOGNAME or USERNAME.

Parameters:
  <-  char UTgetUser[]
      Pointer to a character string containing the user name.  This string is
      null terminated.  This is a pointer to an internal static storage area;
      each call to this routine overlays this storage.

Author / revision:
  P. Kabal  Copyright (C) 2009
  $Revision: 1.22 $  $Date: 2009/03/01 21:02:19 $

-------------------------------------------------------------------------*/

#include <libtsp/sysOS.h>
#ifdef SY_OS_WINDOWS
#  define _CRT_NONSTDC_NO_DEPRECATE   /* Allow Posix names */
#  define _CRT_SECURE_NO_WARNINGS     /* Allow getenv */
#endif

#include <stdlib.h>	/* getenv */

#include <libtsp.h>
#include <libtsp/nucleus.h>
#include <libtsp/UTmsg.h>

#if (SY_POSIX)
#  include <unistd.h>		/* getlogin */
#endif

#define NELEM(array)	((int) ((sizeof array) / (sizeof array[0])))

#include <limits.h>
#ifndef LOGIN_NAME_MAX
#  define LOGIN_NAME_MAX	17
#endif

static const char *UserEnv[] = { "USER", "LOGNAME", "USERNAME" };
#define NENV	NELEM(UserEnv)


char *
UTgetUser (void)

{
  static char User[LOGIN_NAME_MAX];
  char *p;
  int i;

  User[0] = '\0';

#if (SY_POSIX)
  /* Try getlogin first */
  p = getlogin ();
  if (p != NULL && p[0] != '\0')
    STcopyMax (p, User, LOGIN_NAME_MAX-1);
#endif

  for (i = 0; i < NENV && User[0] == '\0'; ++i) {
    p = getenv (UserEnv[i]);
    if (p != NULL)
      STcopyMax (p, User, LOGIN_NAME_MAX-1);
  }

  return User;
}
