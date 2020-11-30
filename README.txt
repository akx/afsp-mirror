November 2020

Audio File Programs and Routines
  The AFsp package contains audio file utility programs and a library of
  routines for reading and writing audio files.

Audio File Utility Programs:
  InfoAudio - display information about an audio file (data format, header/data
              layout, information records)
  CompAudio - compare audio files, producing statistics and signal-to-noise
              ratio figures.
  CopyAudio - copy audio files.  This program combines samples from input audio
              files (an arbitrary linear combination) and writes them to the
              output file in a user selectable format. One application is to
              provide format conversion for an audio file; another is to
              manipulate samples from multi-channel files.
  FiltAudio - filter audio files. This program filters an audio file with an
              FIR, IIR or all-pole filter.
  GenNoise  - generate noise. This program generates an audio file containing
              Gaussian white noise.
  GenTone   - generae a tone. This program generates an audio file containing
              samples from a sinusoid.
  ResampAudio - resample data from an audio file. This process involves
              interpolating between the samples in the original file to create
              a new sequence of samples with a new spacing (sampling rate).

Windows
  Windows executables (32-bit and 64-bit) which can be run from a Windows
  command prompt are included in the distribution (directory bin/Win32 or
  bin/Win64). These were built using Microsoft Visual Study Community 2019
  (available for free).

Linux
  Linux binaries (64-bit) are available in the bin/Linux folder. These were
  built using the Windows 10 Subsystem for Linux.

Cygwin
  Cygwin binaries (64-bit) are available in the bin/Cygwin folder.

Filters:
  The package includes the filters from the ITU-T Software Tool Library (G.191)
  in a form that can be used directly with the FiltAudio program.

Audio File I/O Routines:
  The library supports reading and writing of audio files. The routines have
  been designed to be easy to use, yet provide transparent support the reading
  of several audio file formats. The audio file open routine automatically
  determines the input file type. Based on the file format, the audio file
  reading routine does byte swapping and format conversion on the fly as the
  file is read. The user sees double data without needing to worry about the
  underlying data format.

Audio File Formats:

  The following file formats are supported for reading.
  - Headerless audio files
  - AU audio files
  - WAVE files
  - AIFF/AIFF-C/AIFF-C-sowt sound files
  - NIST SPHERE audio files
  - IRCAM soundfiles
  - INRS-Telecom audio files
  - ESPS sampled data feature files
  - CSL / NSP audio files
  - SPPACK sampled data files
  - Cadence SPW signal files
  - Text audio files (with or without a header)

  The following file formats are supported for writing.
  - Headerless audio files
  - AU audio files
  - WAVE files
  - AIFF sound files
  - AIFF-C sound files
  - Text audio files (with header)

The AFsp routines are covered by copyright, see the file "Licence" for details
of the distribution conditions.

AFsp-v10r3.tar.gz
http://www-MMSP.ECE.McGill.CA/MMSP/Documents/

=============
Changes in v10r3
- CompAudio allows more channels to be displayed. Option to try a range of
  delays has been revamped.
- Apply a fix to Traktion AIFF files with a misformed header
Changes in v10r2
- FiltAudio updated to show the number of samples in the output file header
- Executables for Cygwin, Linux, Win32, Win64
Changes in v9r1
- Simplified BSD License
- Fix Makefile for PQevalAudio to apply mostlyclean to subdirectories
Changes in v10r0
- Added new audio file types and data
  - AIFF-C-sowt PCM sound files for reading and writing
  - Read/Write bit-reversed mu-law format
  - Text data file (with header) for importing / exporting data
- InfoAudio prints the header/data structure for audio files
- Executables built using Cygwin, Linux, and Microsoft Visual Studio 2017 are
  included
- CopyAudio supports a larger number of channels
- Information record subsystem is more consistent across file types. Additional
  text fields in file headers are now returned as information records (includes
  text information from WAVE files with a Broadcast Wave Format chunk).
- Information chunks for WAVE files now stored in the header (rather than after
  the data)
- Internal data scaling rationalized, can be changed with an optional
  parameter
Changes in v9r1
- Invoke the Simplified BSD License
- Fix Makefile for PQevalAudio to apply mostlyclean to subdirectories
Changes in v9r0
- MSVC files updated for Visual C++ 2008
- Use #def to avoid warnings with MSVC compiler
- Use local typedef names to avoid conflicts with standard names
- Fix for parameter scanning (STdec1val)
Changes in v8r2
- User identity in Information fields has been removed
Changes in v8r1
- FiltAudio and ResampAudio use double precision internally
Changes in v8r0
- Add PQevalAudio
Changes in v7r2
- Internal changes, fix documentation on GenTone/GenNoise to reflect
  new normalization.
Changes in v7r1
 - Fix for Compaudio for multi-channel files
Changes in v7r0
 - Support for double (and float) buffers (new open and read routines)
 - Standard normalization for data is now -1 to +1.  The old open and read
   routines are still supported, giving the old normalization (returned data
   values between -32768 and +32767).
 - Audio programs now use the standard normalization.  This change is mainly
   transparent to users, though the output printout for CompAudio now gives
   both absolute and normalized values.
Changes in v6r8
 - Default scaling for headerless files changed
Changes in v6r7a
 - Support for writing AIFF (in addition to AIFF-C) files
 - Support for writing WAVE files that do not use the extensible data types
Changes in v6r6a
 - Fix for mu-law and A-law multi-channel WAVE files (fact chunk value
   is now samples per channel)
 - All utility programs report "samples" (meaning samples per channel)
   instead of "frames" for multi-channel files
 - Fix incomplete struct definition (AFheader.h)
Changes in v6r5a
 - FiltAudio updated (new behaviour for even length symmetric filters)
Changes in v6r4
 - Plug a memory leak (Thanks to RS @ FhG)
 - More type casts for fussy compilers
Changes in v6r3
 - Support for CSL / NSP files
 - Support for WAVE extended files (more than 2 channels, speaker positions)
Changes in v5r2
- Non-PCM WAVE file headers are "more" compliant
- FiltAudio supports subsampling for all filter types
Changes in v4r3
- Microsoft Visual C project files included
Changes in v4r2
- Audio utilities: output file type taken from the file name extension
- Source files compile with MS Visual C
- ANSI C source code
- Fix to AIFF-C COMM chunk size
- Extra information written to WAVE files
- Support for 24-bit and 32-bit data types
- On non-Unix systems, WAVE files are the default output file type
Changes in v4r0
- Windows executables
- ITU filter coefficients
- Utility routines support multi-channel data
- CompAudio implements the ITU-T speech voltmeter (speech activity factor)
- InfoAudio prints header information fields
Changes in V3R2
- ConcatAudio eliminated; CopyAudio can do concatenation
- Output file can be standard output
Changes in V3R1b
- Identify various unsupported formats
- Support for Comdisco SPW files
- Support for 64-bit float data type
- SunAudioPort, SunPlay added
Changes in V2R2
- Revised default data types
Changes in V2R1a
- Support for 8-bit data (Sun, WAVE, AIFF-C files)
Changes in V2R0
- ResampAudio
- Numerous support routine changes

=============
Peter Kabal
Electrical & Computer Engineering
McGill University
Peter.Kabal@McGill.CA

$Id: ReadMe.txt 1.5 2020/11/30 AFsp-v10r3 $
