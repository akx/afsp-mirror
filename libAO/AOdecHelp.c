/*------------- Telecommunications & Signal Processing Lab -------------
                           McGill University

Routine:
  int AOdecHelp (const char Version[], const char Usage[])

Purpose:
  Decode help and version options for audio utilities

Description:
  This routine decodes a help or version option.  The routine AOinitOpt must be
  called first to initialize the option arguments.  If this routine fails to
  find an input audio file option, the argument pointer is reset to allow
  another routine to try to decode the option.  For the help or version
  options, a message is printed and execution is halted.

Parameters:
  <-  int AOdecHelp
         -1 - End of arguments/options
          0 - Option not found
          1 - Option found
   -> const char Version[]
      Program verion identification
   -> const char Usage[]
      Usage message for the program.  This is a format that normally contains
      a "%s" for substitution of the program name.

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.5 $  $Date: 2003/05/09 12:32:37 $

----------------------------------------------------------------------*/

#include <stdlib.h>   /* EXIT_SUCCESS */

#include <libtsp.h>
#include <AObase.h>

#define ROUTINE		"AOdecHelp"
#define PGM		((UTgetProg ())[0] == '\0' ? ROUTINE : UTgetProg ())
#define ERRSTOP(text,par)	UThalt ("%s: %s: \"%s\"", PGM, text, par)

#define RET_ERROR	-2
#define RET_END		-1

static const char *OThelp[] = {
  "-h",  "--h*elp",
  "-v",  "--v*ersion",
  "--",
  "**",
  NULL
};


int
AOdecHelp (const char Version[], const char Usage[])

{
  const char *OptArg;
  int n, Sindex;
  struct AO_CmdArg *Carg;

  /* Get the argument pointers */
  Carg = AOArgs ();
  
  if (Carg->EndOptions)
    return 0;

  /* Decode help options */
  Sindex = Carg->Index;
  n = UTgetOption (&Carg->Index, Carg->Argc, Carg->Argv, OThelp, &OptArg);

  switch (n) {
  case RET_END:
    break;
  case 1:
  case 2:
    /* Help */
    UTwarn (Usage, PGM);
    exit (EXIT_SUCCESS);
    break;
  case 3:
  case 4:
    /* Version */
    printf ("%s: %s\n", PGM, Version);
    exit (EXIT_SUCCESS);
    break;
  case 5:
    /* End of options */
    Carg->EndOptions = 1;
    break;
  default:
    Carg->Index = Sindex;		/* Reset the index */
    n = 0;
    break;
  }

  return n;
}
