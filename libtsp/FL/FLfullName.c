/*------------- Telecommunications & Signal Processing Lab -------------
                          McGill University

Routine:
  int FLfullName (const char Fname[], char Fullname[])

Purpose:
  Expand a file name path

Description:
  This routine takes an input file name with path and tries to expand it to
  form a file name with an absolute path.  The typical use of this routine is
  to log the full file name of a file that has already been opened, and hence
  for which the path is well defined and readable.

  The basic strategy is to take the path and try to temporarily change the
  current working directory to this path.  If this is unsuccessful, the
  rightmost component of the path is removed and the process is repeated.
  When (if) successful, the leftmost components are replaced by the working
  directory name.  This results in a resolved, cleaned up path name.  The
  rightmost components are appended to this to form the full path name.

Parameters:
  <-  int FLfullName
      Number of characters in the output string
   -> const char Fname[]
      Input character string with the file or directory name.  An empty string
      gives the current working directory.
  <-  char Fullname[]
      Output string with the file name with resolved path name.  This string
      is at most FILENAME_MAX characters long including the terminating null
      character.  This string will be the same as the input file name if the
      path cannot be resolved.

Author / revision:
  P. Kabal  Copyright (C) 2009
  $Revision: 1.44 $  $Date: 2009/03/01 21:12:08 $

----------------------------------------------------------------------*/

#include <libtsp/sysOS.h>
#ifdef SY_OS_WINDOWS
#  define _CRT_NONSTDC_NO_DEPRECATE   /* Allow Posix names */
#endif

#include <string.h>

#if (SY_OS == SY_OS_UNIX || SY_OS == SY_OS_CYGWIN)
#  include <sys/types.h>
#  include <sys/stat.h>
#endif

#include <libtsp.h>
#include <libtsp/nucleus.h>
#include <libtsp/FLmsg.h>

#if (SY_POSIX)
#  include <unistd.h>	/* chdir and getcwd */
#else
#  include <direct.h>
#endif

static int
FL_resDir (char Dname[]);
static int
FL_saveDir (char Dname[]);
static int
FL_chDir (const char Dname[]);


int
FLfullName (const char Fname[], char Fullname[])

{
  char Btemp[FILENAME_MAX];
  char Bname[FILENAME_MAX];
  char Dname[FILENAME_MAX];
  char Dprev[FILENAME_MAX];
  int n;

/*
   Try to "resolve" the path name by doing a chdir to the directory and then
   getting the resulting path name with a getcwd.  In this way, we follow
   symbolic links.  The resulting directory string from getcwd hopefully has
   been cleaned up of things like imbedded "./" and "../" strings.

   If the directory is not readable (or does not exist), an error results.  In
   that case, peel off one level of directory and try again.  This should
   eventually give a readable directory: the path name has a leading "/" or
   ends up being ".".  Note that with these peeled off directories we do not
   get the benefit of having getcwd clean up the directory string.
*/
  Bname[0] = '\0';
  STcopyMax (Fname, Dname, FILENAME_MAX-1);
  Dprev[0] = '\0';
  while (FL_resDir (Dname) != 0 && strcmp (Dname, Dprev) != 0) {
    STcopyMax (Dname, Dprev, FILENAME_MAX-1);
    FLbaseName (Dprev, Btemp);
    FLjoinNames (Btemp, Bname, Bname);
    FLdirName (Dprev, Dname);
  }

/* Form the output name */
  n = FLjoinNames (Dname, Bname, Fullname);

  return n;
}


static int
FL_resDir (char Dname[])

{
  char Dsave[FILENAME_MAX];
  char Dtemp[FILENAME_MAX];
  int status;

/* Change to Dname and get its "cleaned up" name */
/* An empty name is interpreted to mean the current directory */
  if (Dname[0] != '\0') {

    /* Save the current directory */
    status = 1;
    if (FL_saveDir (Dsave) != 0)
      return status;
    if (FL_chDir (Dname))
      return status;	/* Assume that the directory did not change */
  }

/* Get the "cleaned up" directory name */
  status = (getcwd (Dtemp, FILENAME_MAX) == NULL);

/* Restore the current directory */
  if (Dname[0] != '\0') {
    if (FL_chDir (Dsave) != 0) {
      UTwarn ("FLfullName - %s", FLM_CWDErr);
      status = 1;
    }
  }

/* Return the "cleaned up" name */
  if (! status)
    STcopyMax (Dtemp, Dname, FILENAME_MAX-1);

  return status;
}

/* Save the current directory and check that we can get back to it */


static int
FL_saveDir (char Dname[])

{
  int status;

  status = 1;
  if (getcwd (Dname, FILENAME_MAX) != NULL)
    status = FL_chDir (Dname);

  return status;
}

#if (SY_FILENAME_SPEC == SY_FNS_UNIX)
#  define DIR_SEP_STR	"/"
#elif (SY_FILENAME_SPEC == SY_FNS_WINDOWS)
#  define DIR_SEP_STR	"\\"
#else
#  error "Bad SY_FILENAME_SPEC value"
#endif
#define DIR_SEP_CHAR	((DIR_SEP_STR)[0])


static int
FL_chDir (const char Dname[])

{
  int status, n;
  char Dtemp[FILENAME_MAX+1];

/*  Workaround for Windows-specific behaviour.  In Windows, chdir("C:")
    changes the disk but not the directory, while chdir("C:\") also sets the
    directory.  Here we make sure that the directory name is always terminatd
    by a directory separator.
*/

  if (Dname[0] == '\0')
    status = 0;		/* Current directory */
  else {
    n = strlen (Dname);
    if (Dname[n] != DIR_SEP_CHAR) {
      n = STcopyNMax (Dname, Dtemp, n, FILENAME_MAX-1);
      Dtemp[n] = DIR_SEP_CHAR;
      Dtemp[n+1] = '\0';
      status = chdir (Dtemp);
    }
    else
      status = chdir (Dname);
  }

  return status;
}
