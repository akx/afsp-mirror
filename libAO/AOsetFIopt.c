/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  void AOsetFIopt (const struct AO_FIpar *FI, int NsampND, int RAccess)

Purpose:
  Set input file options

Description:
  This routine sets the input file options in the audio file options structure
  from the input file parameter structure.

  Calling this routine as
    AOsetFIopt (FI, 0, 0)
  will leave NsampND and RAccess at their default settings.

Parameters:
   -> const struct AO_FIpar *FI
      Input file parameters
   -> int NsampND
      Flag to determine if number of samples must be known
      0 - Nsamp must be known
      1 - Nsamp=AF_NSAMP_UNDEF allowed
   -> int RAccess
      Flag to determine if the file must be random access
      0 - File need not be random access, but an error will occur when an
          attempt is made to reposition the file
      1 - File must be random access

Author / revision:
  P. Kabal  Copyright (C) 2003
  $Revision: 1.3 $  $Date: 2003/05/09 12:32:37 $

-------------------------------------------------------------------------*/

#include <libtsp/AFpar.h>
#include <AO.h>


void
AOsetFIopt (const struct AO_FIpar *FI, int NsampND, int RAccess)

{
  struct AF_opt *AFopt;

  AFopt = AFoptions ();

  AFopt->NsampND = NsampND;
  AFopt->RAccess = RAccess;
  AFopt->FtypeI = FI->Ftype;
  AFopt->NHpar = FI->NHpar;

  return;
}
