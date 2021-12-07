This repository mirrors the AFsp files from
http://www-mmsp.ece.mcgill.ca/Documents/Downloads/AFsp/index.html .

The contents of the site at the time of the mirroring follow.

---

# AFsp Package

The _AFsp_ package provides audio file utility programs and a library of routines for reading and writing audio files.

## Audio File Utility Programs

Detailed documentation for the utility programs is available on-line: [Audio File Programs and Routines](../../Software/Packages/AFsp/AFsp/AFsp.html).

Utility Program Source Code:

C-anguage source code for the utility programs is in the audio/xxxAudio directories

libAO Source Code:

C-language source code for the libAO library is in the libAO directory. These routines provide option processing for the utility programs. The include files for libAO library and for the libtsp library are in the include directory.

Documentation:

Documentation for the utility programs is in audio/html and audio/man directories

Executables:

Pre-built executables are included in the bin directory. Windows (command line) 64-bit executables are in the bin/Windows directory; Linux 64-bit executables built using the Windows subsystem for Linux are in the bin/Linux directory; Cygwin (64-bit) executables are in the bin/Cygwin directory.

MS Visual Studio

A 2017 MS Visual Studio C project to build both the utility programs and the libraries is in the MSVC directory.

Test Script

The test directory contains a Bash script for testing the utility programs in a Unix environment. The test can be initiated by running Make in the test directory. The Makefile assumes that the executables are in the main bin directory. The Makefile runs the test script and compares the output with a reference file.

### Filters:

The AFsp package includes the filters from the ITU-T Software Tool Library (G.191) in a form that can be used directly with the FiltAudio program. These filter files are in the filters directory.

### Audio File Routines Library:

Detailed documentation for the audio file routines is available on-line: [Audio File Programs and Routines](../../Software/Packages/AFsp/AFsp/AFsp.html).

Audio File Routines:

C-anguage source code for the audio file processing routines is in the libtsp directory. The AF subdirectory contains the main routines for processing audio files. The other subdirectories folders routines that are used by the audio file routines.

Documentation:

Documentation for the audio file routines is in libtsp/html and libtsp/man directories.

## Copyright Notice

Copyright © 2018 Peter Kabal\
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1.  Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
2.  Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS \"AS IS\" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

## Downloads

[AFsp-v10r3.tar.gz](AFsp-v10r3.tar.gz) 2020-11

[AFsp-v10r2.tar.gz](AFsp-v10r2.tar.gz) 2018-11

[AFsp-v10r1a.tar.gz](AFsp-v10r1a.tar.gz) 2017-10

[AFsp-v9r0.tar.gz](AFsp-v9r0.tar.gz) 2010-09

[AFsp-v8r2.tar.gz](AFsp-v8r2.tar.gz) 2006-06

[AFsp-v7r2.tar.gz](AFsp-v7r2.tar.gz) 2003-11

[AFsp-v6r8.tar.gz](AFsp-v6r8.tar.gz) 2003-05

## Version Changes

### Changes in V10r3

- CompAudio allows more channels to be displayed. Option to try a range of delays has been revamped.
- Apply a fix to Traktion AIFF files with a malformed header.

### Changes in V10r2

- FiltAudio updated to show the number of samples in the output file header
- Executables for Cygwin, Linux, Win32, Win64

### Changes in V10r1a

- Changes to information record printing to allow 8-bit characters
- Info record names more consistent for different file types
- Added new audio file types and data

  - AIFF-C-sowt PCM sound files for reading and writing
  - Read/Write bit-reversed mu-law format
  - Text data file (with header) for importing / exporting data

- InfoAudio prints the header/data structure for audio files
- Executables built using Cygwin, Linux, and Microsoft Visual Studio 2017 are included
- CopyAudio supports a larger number of channels
- Information records gathered from more file types.
- More text fields in file headers are now returned as information records (includes text information from WAVE files with a Broadcast Wave Format chunk)
- Information chunks for WAVE files now stored in the header (rather than after the data)
- Internal data scaling rationalized - it can be changed with an optional parameter

### Changes in v9r0

- MSVC files updated for Visual C++ 2008
- Use #def to avoid warnings with MSVC compiler
- Use local typedef names to avoid conflicts with standard names
- Fix for parameter scanning (STdec1val)

### Changes in v8r2

- User identity in default information fields removed
- FiltAudio and ResampAudio use double precision internally

### Changes in v7r2

- Internal changes, fix documentation on GenTone/GenNoise to reflect the new normalization
- Fix for Compaudio for multi-channel files
- Support for double (and float) buffers (new open and read routines)
- Standard normalization for internal data is now -1 to +1
- The old open and read routines are still supported, giving the old normalization (returned data values between -32768 and +32767)
- Audio programs now use the standard normalization. This change is mainly transparent to users, though the output printout for CompAudio now gives both absolute and normalized values.

### Changes in v6r8

- Default scaling for headerless files changed
