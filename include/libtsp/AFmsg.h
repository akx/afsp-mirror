/*------------ Telecommunications & Signal Processing Lab --------------
                         McGill University

Routine:
  AFmsg.h

Description:
  Message texts for the AFsp audio file routines.

Author / revision:
  P. Kabal  Copyright (C) 2017
  $Revision: 1.69 $  $Date: 2017/07/13 15:26:27 $

----------------------------------------------------------------------*/

#ifndef AFmsg_h_
#define AFmsg_h_

/* File parameter printout formats */
#define AFM_Headerless  "Headerless audio file"
#define AFM_NumFrameNA  "   Samples / channel : <N/A>    "
#define AFM_NumFrameUnk "   Samples / channel : <unknown>"
#define AFMF_Desc       "   Description: %s\n"
#define AFMF_NumChan    "   Number of channels: %ld"
#define AFMF_NumFrame   "   Samples / channel : %ld"
#define AFMF_Sfreq      "   Sampling frequency: %.6g Hz\n"

/* Error message text */

/* Warning messages for parameter setting routines */
/* AFsetFileType
   AFsetInfo
   AFsetNHpar
   AFsetSpeaker
*/
#define AFM_BadFormat   "Invalid format keyword"
#define AFM_BadFtype    "Invalid file type"
/*      AFM_BadNchan */
#define AFM_BadFullScale "Invalid full scale value"
#define AFM_BadSfreq    "Invalid sampling frequency"
#define AFM_BadSpkr     "Invalid loudspeaker location"
#define AFM_BadStart    "Invalid start value"
#define AFM_BadSwap     "Invalid swap keyword"
#define AFM_DupSpkr     "Duplicate loudspeaker position"
#define AFM_LongInfo    "Information string too long"
#define AFM_TooManyPar  "Too many parameters"
#define AFM_XSpkr       "Too many speaker locations"

/* Input audio file: Error messages */
#define AFM_BadLdata    "Invalid data length"
#define AFM_BadNchan    "Invalid number of channels"
#define AFM_BadNsamp    "Invalid number of samples"
/*      AFM_BadSpkr */
#define AFM_ErrHiLdata  "Error, data length greater than file length"
#define AFM_ErrHiNsamp  "Error, no. samples greater than data length"
#define AFM_MissingAFsp "Missing information record chunk identifier"
#define AFM_NoNsamp     "Unable to determine number of samples"
#define AFM_OpenRErr    "Error opening input audio file"
#define AFM_RAuto       "Input audio file must be random access for \"auto\""
#define AFM_RRAccess    "Input audio file must be random access"
#define AFM_UndSfreq    "Sampling rate is undefined"
#define AFM_UnkFtype    "Unknown audio file type"
#define AFM_UnsupFtype  "Unsupported audio file type"

/* Output audio file: Error messages */
#define AFM_BadDataC    "Invalid data format code"
#define AFM_BadFtypeC   "Invalid output audio file type"
/*      AFM_BadNchan */
#define AFM_BadNframe   "Invalid number of sample frames"
#define AFM_ExSpkr      "Extra speaker location(s) ignored"
#define AFM_OpenWErr    "Error opening output audio file"

/* I/O errors */
#define AFM_FilePosErr  "File positioning error"
#define AFM_LongLine    "Input line too long"
#define AFM_NoFilePos   "Cannot determine file position"
#define AFM_MoveBack    "Cannot move back in file; File must be random access"
#define AFM_ReadErr     "Read error"
#define AFM_ReadErrOffs "Error while reading, sample offset:"
#define AFM_TextErr     "Text line, invalid character"
#define AFM_UEoF        "Unexpected end-of-file"
#define AFM_UEoFOffs    "Unexpected end-of-file, sample offset:"
#define AFM_UpdHeadErr  "Error updating file header"
#define AFM_WriteErr    "Write error"

/* Warning messages */
#define AFM_BadNbS      "Invalid bits/sample"
#define AFM_Clip        "Output data clipped"
#define AFM_FixLoNsamp  "Fixup, no. samples less than data length"
#define AFM_FixHiNsamp  "Fixup, no. samples greater than data length"
#define AFM_FixHiLdata  "Fixup, data length greater than file length"
#define AFM_InaNbS      "Inappropriate bits/sample setting"
#define AFM_MisSRate    "Sampling frequency / \"sample_rate\" record mismatch"
#define AFM_NPSfreq     "Non-positive sampling frequency"
#define AFM_NsampNchan  "No. samples not a multiple of no. channels"
#define AFM_NsampMis    "No. samples different from declared value"
#define AFM_NsampUpd    "Updating no. samples in header"
#define AFM_NoIEEE      "Host does not use IEEE float format"
#define AFM_NoRBinMode  "Cannot set input to binary mode"
#define AFM_NoWBinMode  "Cannot set output to binary mode"
#define AFM_NonIntNsamp "Fixup, non-integer number of samples"
#define AFMF_ChunkGap   " Gap between chunks <%.4s> / <%.4s>\n"
#define AFMF_ChunkGapEoF " Gap after chunk <%.4s> at end-of-file\n"
#define AFMF_ChunkOvlp  " Overlapping chunks <%.4s> / <%.4s>\n"
#define AFMF_ChunkPastEoF " Chunk <%.4s> extends beyond end-of-file\n"

/* Info record routines */
#define AFM_Info_DecBytes  "Decoding error, bytes/sample info record"
#define AFM_Info_DecNbS    "Decoding error, bits/sample info record"
#define AFM_Info_DecNchan  "Decoding error, channels info record"
#define AFM_Info_DecNframe "Decoding error, frames record"
#define AFM_Info_DecFullScale "Decoding error, full scale info record"
#define AFM_Info_DecSfreq  "Decoding error, sample rate info record"
#define AFM_Info_InvFullScale "Invalid full scale value in info record"
#define AFM_Info_InvBytes  "Invalid bytes/sample in info record"
#define AFM_Info_InvNbS    "Invalid bits/sample in info record"
#define AFM_Info_InvNchan  "Invalid number of channels in info record"
#define AFM_Info_InvNframe "Invalid number of frames in info record"
#define AFM_Info_InvSfreq  "Invalid sample rate in info record"

/* Informational message formats */
#define AFMF_FullScInPar  "%s Full scale value from AFsetInputPar: %g"
#define AFMF_InvNbS       "%s Invalid bits/sample: %d; using %d"
#define AFMF_NchanInPar   "%s No. channels from AFsetInputPar: %ld"
#define AFMF_NsampNchan   "%s %ld samples; %ld channels"
#define AFMF_NClip        "%s %ld data values clipped"
#define AFMF_SfreqInPar   "%s Sampling frequency from AFsetInputPar: %g"

/* Error/warning messages for different audio file types */
#define AFM_AIFFX_BadHead "Invalid AIFF/AIFF-C file header"
#define AFM_AIFFX_BadId   "Invalid AIFF/AIFF-C file identifier"
#define AFM_AIFFX_BadSpkr "AIFF/AIFF-C file: Non-standard" \
                          " loudspeaker configuration"
#define AFM_AIFFX_BadSize "AIFF/AIFF-C file: Header stucture size mismatch"
#define AFM_AIFFC_BadVer  "AIFF-C file: Unrecognized version number"
#define AFM_AIFFC_FixCOMM "AIFF-C file: Fixup for invalid COMM chunk size"
#define AFM_AIFFX_FixFORM "AIFF/AIFF-C file: Fixup for invalid FORM chunk size"
#define AFM_AIFFX_NoSSND  "AIFF/AIFF-C file: Missing SSND chunk"
#define AFM_AIFFC_UnsComp "AIFF-C file: Unsupported compression type"
#define AFM_AIFF_UnsData  "AIFF file: Unsupported data format"
#define AFM_AIFFC_UnsData "AIFF-C file: Unsupported data format"
#define AFM_AIFFC_X_UnsData "AIFF-C/sowt file: Unsupported data format"
#define AFM_AIFFX_UnsSSize "AIFF/AIFF-C file: Unsupported sample size"
#define AFM_AIFFX_WRAccess "AIFF/AIFF-C file: Output file must be random access"
#define AFMF_AIFFX_InvNbS  "%s AIFF/AIFF-C file: Invalid bits/sample: %d;" \
                           " using %d"

#define AFM_AU_BadId    "Invalid AU audio file identifier"
#define AFM_AU_UnsData  "AU audio file: Unsupported data format"

#define AFM_BL_BadId    "Invalid SPPACK file identifier"
#define AFM_BL_UnsComp  "SPPACK file: Unsupported companding option"
#define AFM_BL_UnsData  "SPPACK file: Unsupported data format"
#define AFM_BL_UnsDomain "SPPACK file: Unsupported domain"
#define AFM_BL_UnsWLen  "SPPACK file: Unsupported data word length"

#define AFM_ES_BadId    "Invalid ESPS file identifier"
#define AFM_ES_IdMatch  "ESPS file: Identifier values do not match"
#define AFM_ES_MixData  "ESPS file: Unsupported mixed data types"
#define AFM_ES_NoSfreq  "ESPS file: Missing sampling frequency"
#define AFM_ES_UnsData  "ESPS file: Unsupported data format"
#define AFM_ES_UnsEncod "ESPS file: Unsupported data encoding"
#define AFM_ES_UnsFea   "ESPS file: Unsupported feature file subtype"
#define AFM_ES_UnsType  "ESPS file: Unsupported file type code"

#define AFM_NH_NoFormat "Headerless file: Data format not specified"
#define AFM_NH_UnsData  "Headerless file: Unsupported data type"

#define AFM_NS_BadHead  "Invalid CSL NSP file header"
#define AFM_NS_BadId    "Invalid CSL NSP file identifier"
#define AFM_NS_BadHEDR  "CSL NSP file: Invalid HEDR/HDR8 chunk size"
#define AFM_NS_BadFORM  "CSL NSP file: Invalid FORM-DS16 block size"
#define AFM_NS_FixFORM  "CSL NSP file: Fixup for invalid FORM-DS16 block size"

#define AFM_INRS_BadId  "Invalid INRS-Telecom file identifier"

#define AFM_SF_BadId    "Invalid IRCAM soundfile identifier"
#define AFM_SF_UnsData  "IRCAM soundfile: Unsupported data format"

#define AFM_SP_BadId    "Invalid NIST SPHERE audio file identifier"
#define AFM_SP_NoInter  "NIST SPHERE audio file: Channels must be interleaved"
#define AFM_SP_UnsData  "NIST SPHERE audio file: Unsupported data format"
#define AFM_SP_UnsMulaw "NIST SPHERE audio file: Invalid mu-law word length"
#define AFM_SP_UnsPCM   "NIST SPHERE audio file: Unsupported PCM word length"
#define AFM_SP_BadHead  "NIST SPHERE audio file: Malformed file header"
#define AFM_SP_IncFmt   "NIST SPHERE audio file: Missing format information"

#define AFM_SW_BadId    "Invalid SPW Signal file identifier"
#define AFM_SW_PosErr   "File position error"
#define AFM_SW_UnsHead  "Unsupported SPW header format"
#define AFM_SW_UnsOpt   "Unsupported SPW data option"
#define AFMF_SW_UnsFixFormat  "%s Unsupported fixed point format \"%.10s\""
#define AFMF_SW_UnsSigType    "%s Unsupported signal type \"%.20s\""
#define AFMF_SW_UnsSys  "%s Unsupported system type \"%.10s\""

#define AFM_TA_BadId    "Invalid Text file identifier"
#define AFM_TA_PosErr   "File position error"
#define AFM_TA_UnsHead  "Unsupported header format"

#define AFM_WV_BadAlaw  "WAVE file: Invalid A-law data size"
#define AFM_WV_BadBlock "WAVE file: Invalid block align value"
#define AFM_WV_BadBytesSec \
    "        WAVE file: Inconsistent header value AvgBytesPerSec ignored"
#define AFM_WV_BadFACT  "WAVE file: Invalid fact chunk size"
#define AFM_WV_BadFloat "WAVE file: Invalid float data size"
#define AFM_WV_BadId    "Invalid WAVE file identifier"
#define AFM_WV_BadfmtEx "WAVE file: Invalid fmt (EXTENSIBLE) chunk"
#define AFM_WV_BadfmtSize "WAVE file: Invalid fmt chunk size"
#define AFM_WV_BadHead  "Invalid WAVE file header"
#define AFM_WV_BadMulaw "WAVE file: Invalid mu-law data size"
#define AFM_WV_BadNbS   "WAVE file: Invalid bits/sample"
#define AFM_WV_BadNchan "WAVE file: Invalid number of channels"
#define AFM_WV_BadRIFF  "WAVE file: Invalid RIFF chunk size"
#define AFM_WV_BadSize  "WAVE file: Header structure size mismatch"
#define AFM_WV_BadSS    "WAVE file: Inconsistent sample size"
#define AFM_WV_FixRIFF  "WAVE file: Fixup for invalid RIFF chunk size"
#define AFM_WV_NSSpkr   "WAVE file: Non-standard speaker locations"
#define AFM_WV_NSSpkrO  "WAVE file: Non-standard speaker order"
#define AFM_WV_UnkChannel "WAVE file: Unknown Channel/Speaker combination"
#define AFM_WV_UnsDSize "WAVE file: Unsupported PCM data size"
#define AFM_WV_UnsData  "WAVE file: Unsupported data format"
#define AFM_WV_WRAccess "WAVE file: Output file must be random access"
#define AFMF_WV_InvNbS  "%s WAVE file: Invalid bits/sample: %d; using %d"

#endif  /* AFmsg_h_ */
