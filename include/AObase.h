/*------------ Telecommunications & Signal Processing Lab --------------
                         McGill University

Routine:
  AObase.h

Description:
  Declarations for audio utility program options routines

Author / revision:
  P. Kabal  Copyright (C) 2017
  $Revision: 1.3 $  $Date: 2017/03/28 00:24:12 $

----------------------------------------------------------------------*/

#ifndef AObase_h_
#define AObase_h_

/* Option parameters */
struct AO_CmdArg {
  const char **Argv;  /* Argument pointer */
  int Argc;           /* Argument count */
  int Index;          /* Argument index */
  int EndOptions;     /* End-of-options indicator */
};

#ifdef __cplusplus
extern "C" {
#endif

/* Prototypes */
struct AO_CmdArg *
AOArgs (void);
int
AOdecHelp (const char Version[], const char Usage[]);
int
AOdecOpt (const char *OptTable[], const char **OptArg);
void
AOinitOpt (const int argc, const char *argv[]);

#ifdef __cplusplus
}
#endif

#endif /* AObase_h_ */
