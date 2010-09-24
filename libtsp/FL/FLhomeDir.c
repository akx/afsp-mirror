/*------------- Telecommunications & Signal Processing Lab -------------
                          McGill University

Routine:
  int FLhomeDir (const char User[], char Home[])

Purpose:
  Get the home directory for a user

Description:
  This routine returns the path name corresponding to the home directory for a
  user.  When the User argument is empty, the current user is assumed. There
  are several cases. The search order is: environment variables HOMEDRIVE and
  HOMEPATH (current user on Windows systems), home directory in the password
  entry (Posix systems), and finally the environment variable HOME (current
  user). If the home directory cannot be obtained (for instance, the user is
  not known) the home directory string is returned as the empty string.

Parameters:
  <-  int FLhomeDir
      Number of characters in the output string
   -> const char User[]
      Input character string specifying the user.  If this string is empty, the
      home directory for the current user is returned.
  <-  char Home[]
      Output string with the home directory.  Except for the case of the root
      directory "/", the directory name does not have a trailing '/' character.
      This string is at most FILENAME_MAX characters long including the
      terminating null character.

Author / revision:
  P. Kabal  Copyright (C) 2009
  $Revision: 1.26 $  $Date: 2009/03/01 21:12:47 $

----------------------------------------------------------------------*/

#include <libtsp/sysOS.h>
#ifdef SY_OS_WINDOWS
#  define _CRT_NONSTDC_NO_DEPRECATE   /* Allow Posix names */
#  define _CRT_SECURE_NO_WARNINGS     /* Allow getenv */
#endif

#include <stdlib.h>		/* getenv prototype */
#include <string.h>

#include <libtsp.h>
#include <libtsp/nucleus.h>

#if (SY_POSIX)
#  include <pwd.h>	/* struct passwd, getpwnam prototype */
#  include <unistd.h>	/* getuid definitions */
#endif

#if (SY_FILENAME_SPEC == SY_FNS_UNIX)
#  define DIR_SEP_STR	"/"
#elif (SY_FILENAME_SPEC == SY_FNS_WINDOWS)
#  define DIR_SEP_STR	"\\"
#else
#  error "Bad SY_FILENAME_SPEC value"
#endif
#define DIR_SEP_CHAR	((DIR_SEP_STR)[0])


int
FLhomeDir (const char User[], char Home[])

{
  char *h, *p;
  int n, nc;
#if (SY_OS == SY_OS_WINDOWS)
  char temp[FILENAME_MAX-1];
  char *q;
#endif
#if (SY_POSIX)
  struct passwd *pwd;
#endif

  h = NULL;
  if (User[0] == '\0' || strcmp (User, UTgetUser()) == 0) {

    /* No user name specified or same as logged in user */
    /* Try the environment variable HOME 
       - On Cygwin, HOME is translated to Unix file conventions
    */
    p = getenv ("HOME");
    if (p != NULL && *p != '\0')
      h = p;

#if (SY_OS == SY_OS_WINDOWS)
    /* Get the home directory from the environment variables
       HOMEDRIVE and HOMEPATH.
    */
    p = getenv ("HOMEDRIVE");
    q = getenv ("HOMEPATH");
    if (p != NULL && *p != '\0' && q != NULL && q!= '\0') {
      STcopyMax (p, temp, FILENAME_MAX-1);
      STcatMax (q, temp, FILENAME_MAX-1);
      h = temp;
    }
#endif

#if (SY_POSIX)
    /* Try the password entry */
    else {
      pwd = getpwuid (getuid ());
      if (pwd != NULL)
	h = pwd->pw_dir;
    }
#endif
  }

#if (SY_POSIX)
  else {

/* User specified, try the password entry */
    pwd = getpwnam (User);
    if (pwd != NULL)
      h = pwd->pw_dir;
  }
#endif

  if (h != NULL) {

/* Found a home directory */
    nc = strlen (h);
    if (nc > 1 && h[nc-1] == DIR_SEP_CHAR)
      --nc;
    n = STcopyNMax (h, Home, nc, FILENAME_MAX-1);
  }

  else {

/* Unsuccessful */
    Home[0] = '\0';
    n = 0;
  }

  return n;
}
