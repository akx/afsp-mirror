/*------------- Telecommunications & Signal Processing Lab -------------
                           McGill University

Routine:
  void AOinitOpt (const int argc, const char *argv[])
  int AOdecOpt (const char *OptTable[], const char **OptArg)
  struct AO_CmdArg *AOArgs (void)

Purpose:
  Decode options for audio utilities

Description:
  These routines decode options for audio utilties.  The routine AOinitOpt is
  used to initialize the argument array.  The routine AOdecOpt decodes options
  using a user supplied option table

Parameters:
  void AOinitOpt
   -> int argc
      Number of command line arguments
   -> const char *argv[]
      Array of pointers to argument strings

  <-  int  AOdecOpt
      Status code,
         -2 - Error, invalid option or missing value
         -1 - End of arguments/options
          0 - Not an option, argument is returned
        >=1 - Option code
   -> const char *OptTable[]
      Pointers to the option keywords.  The end of the keyword table is
      signalled by a NULL pointer.  These keyword strings are pased to
      UTgetOption.
  <-  const char *OptArg[]
      Argument/option string.  For an argument or an option taking a value,
      this is a pointer to a null terminated substring string in argv.  If the
      decoded option does not take a value, this pointer will be NULL.

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.11 $  $Date: 2003/05/09 12:32:37 $

----------------------------------------------------------------------*/

#include <libtsp.h>
#include <AObase.h>

static const char *nullTable[] = { NULL };

/* Initialize the command line argument structure */
static struct AO_CmdArg AOArg = {NULL, 0, 1, 0};


void
AOinitOpt (const int argc, const char *argv[])

{
/* Set the argument pointer structure */
  AOArg.Argv = argv;
  AOArg.Argc = argc;
  AOArg.Index = 1;
  AOArg.EndOptions = 0;

  return;
}

/* Return a pointer to the argument pointer structure */


struct AO_CmdArg *
AOArgs (void)

{
  return &AOArg;
}

/* Decode user options */


int
AOdecOpt (const char *OptTable[], const char **OptArg)

{
  int n;
  struct AO_CmdArg *Carg;

  /* Get the argument pointers */
  Carg = AOArgs ();
  
  /* Decode the option */
  if (Carg->EndOptions)
    n = UTgetOption (&Carg->Index, Carg->Argc, Carg->Argv, nullTable, OptArg);
  else
    n = UTgetOption (&Carg->Index, Carg->Argc, Carg->Argv, OptTable, OptArg);

  return n;
}
