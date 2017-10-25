/*-------------- Telecommunications & Signal Processing Lab ---------------
                             McGill University

Routine:
  struct AF_opt *AFoptions (unsigned int Cat)

Purpose:
  Reset all or part of the audio file options structure to its default setting
  and return a pointer to the structure.

Description:
  This routine returns a pointer to the audio file options structure.  The
  options structure is defined in the header file AFpar.h.  Routines are
  provided to set and access some of the fields.

  Note that the options for output files are reset to default values after
  opening an output file. Changes to other options persist until reset.  The
  routine AFresetOptions can be used to reset some or all of the options.

  Program Behaviour:
  Error Control (int ErrorHalt):
    The top level AF routines (AFopnRead, AFopnWrite, AFdReadData, etc.)
    normally print an error message and halt when an error is passed from a
    lower level routine.  Optionally, errors can also be signalled by a return
    value such as NULL file pointer or other error flag.  The error options are
    as follows.
      0 - Continue on error, returning an error code
      1 - Stop on error (default behaviour)
      2 - For input files, return an audio file parameter structure if possible
          for unsupported data formats.  Any attempt to read the data will
          generate an error.
  Program data full scale (double ScaleV):
    Data from input files is returned scaled to have a nominal full scale value
    ScaleV.  For a given value of ScaleV, integer file data on input will be
    scaled to the interval between -ScaleV and +ScaleV.  When writing to an
    output file, the inverse scaling is used.  The default value for ScaleV is
    1.

  Options for Input Files:
  Number of samples (long int NsampND):
    The number of samples in a speech file opened for read is normally returned
    by AFopnRead.  In some cases, the number of samples can only be determined
    from the actual number of records in the file.  This determination is not
    possible for input streams which are not random access.  For such cases, the
    number of samples value can be set to a value which indicates that this
    value is not available.  The NsampND options are as follows.
      0 - AFopnRead always return the number of samples in the file.
          An error condition exists if the number of samples cannot be
          determined (default behaviour).
      1 - AFopnRead returns the number of samples if the file is seekable or
          the number of is specified in the file header.  Otherwise return a
          value indicating that this value is undefined (AF_NSAMP_UNDEF).
  Random access requirement (int RAccess):
    Some programs require the ability to reposition the file to an earlier
    position (to reread data, for instance).  This parameter controls whether
    a file will be opened if it does not support random access (viz. file
    streams from a pipe are not random access).
      0 - Allow input files which are not random access (default behaviour).  If
          an attempt is made to reposition such a file during a read operation,
          an error condition will occur.
      1 - Do not open input audio files which are not random access.
  Input File Type (enum AF_FT_T FtypeI):
    Input audio file type (default FT_AUTO).  This parameter can be set with
    named parameters using the routine AFsetFileType.
  File parameters for input files (struct AF_InputPar AFInputPar):
    These parameters are used if the any of the parameters can not be discerned
    from the input file.  See the routine AFsetInputPar for a description of
    these parameters and their default settings.

  Options for Output Files:
  Number of frames (long int Nframe):
    This value specifies the number of sample frames in an output audio file.
    This value is used to set the number of frames information in a file header
    before writing audio data to the file.  The value specified is used for
    non-random access output files which cannot update the header after writing
    the audio data.  If this value is AF_NFRAME_UNDEF, the number of frames is
    not predefined.  For some types of output files, this setting means that
    the output file must be random access.
  Number of bits per sample (int NbS):
    This value affects a corresponding value in some file headers.  Each sample
    is in a container with a size which is a multiple of 8 bits.  The container
    size is determined from the data format.  The number of bits per sample
    indicates that some of the bits are not significant.  The number of bits per
    sample rounded up to a multiple of eight must equal the container size.  The
    default value of zero indicates that the number of bits is the same as the
    container size for the data.
  Speaker configuration (unsigned char *SpkrConfig):
    This null-terminated string contains the speaker location codes.  This
    string can be set with symbolic values using the routine AFsetSpeaker.
    This string is allocated as needed.  It can be deallocated by invoking
    AFsetSpeaker ("").  The default is that no speaker locations are specified.
  Standard information flag (enum AF_STDIN_T StdIF)
    This flag determines whether standard information records are to be
    generated. Default is allow.
  Header information structure (struct AF_info Uinfo):
    This structure contains information records supplied by the user via the
    routine AFsetInfo.  Storage is allocated as needed.  It can be deallocated
    by invoking AFsetInfo ("").  See AFsetInfo for more information.

Parameters:
  Routine AFoptions
  <-  struct AF_opt *AFoptions
      Pointer to the option structure
   -> unsigned int Cat
      Categories of options to be reset.  Cat is the sum
      of the following sub-categories:
        AF_OPT_NULL    - Do not reset any options, return a pointer to the
                         options structure
        AF_OPT_GENERAL - General options
        AF_OPT_INPUT   - Input file options
        AF_OPT_OUTPUT  - Output file options
        AF_OPT_ALL     - By itself, resets all options

Author / revision:
  P. Kabal  Copyright (C) 2017
  $Revision: 1.35 $  $Date: 2017/06/15 11:45:38 $

-------------------------------------------------------------------------*/

#include <libtsp.h>
#include <AFpar.h>
#include <libtsp/nucleus.h>

#define AF_INPUTPAR_DEFAULT \
  {FD_UNDEF, 0L, AF_SFREQ_DEFAULT, DS_NATIVE, 1L, AF_FULLSCALE_DEFAULT}
#define AF_UINFO_DEFAULT \
  {NULL, 0, 0}
#define AF_OPT_DEFAULT \
  {AF_ERRORHALT_DEFAULT, AF_SCALEV_DEFAULT, \
   AF_NSAMPND_DEFAULT, AF_RACCESS_DEFAULT, FT_AUTO, AF_INPUTPAR_DEFAULT, \
   AF_NFRAME_UNDEF, AF_NBS_DEFAULT, NULL, AF_STDINFO_DEFAULT, AF_UINFO_DEFAULT}

/* Initialization of the global options structure */
struct AF_opt AFopt = AF_OPT_DEFAULT;


struct AF_opt *
AFoptions (unsigned int Cat)

{
  /* Default values */
  static const struct AF_opt AFopt_def = AF_OPT_DEFAULT;

  if (Cat & AF_OPT_GENERAL) {
    AFopt.ErrorHalt = AFopt_def.ErrorHalt;
    AFopt.ScaleV = AFopt_def.ScaleV;
  }

  if (Cat & AF_OPT_INPUT) {
    AFopt.NsampND = AFopt_def.NsampND;
    AFopt.RAccess = AFopt_def.RAccess;
    AFopt.FtypeI = AFopt_def.FtypeI;
    AFopt.InputPar = AFopt_def.InputPar;
  }

  if (Cat & AF_OPT_OUTPUT) {
    AFopt.Nframe = AFopt_def.Nframe;
    AFopt.NbS = AFopt_def.NbS;
    UTfree (AFopt.SpkrConfig);
    AFopt.SpkrConfig = AFopt_def.SpkrConfig;
    UTfree (AFopt.UInfo.Info);
    AFopt.StdInfo = AFopt_def.StdInfo;
    AFopt.UInfo = AFopt_def.UInfo;
  }

  return &AFopt;
}
