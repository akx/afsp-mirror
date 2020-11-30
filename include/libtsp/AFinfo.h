/*------------ Telecommunications & Signal Processing Lab --------------
                         McGill University

Routine:
  AFinfo.h

Description:
  Information record identifiers

Author / revision:
  P. Kabal  Copyright (C) 2020
  $Revision: 2.15 $  $Date: 2020/11/25 17:56:06 $

----------------------------------------------------------------------*/

#ifndef AFinfo_h_
#define AFinfo_h_

#include <AFpar.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
  Info record ID's, canonical ID first, NULL terminated
  Info record Key's, NULL terminated
  Info record Key values
  Info record Units, use an empty last entry "" if the Units field is optional
  Info record Unit scaling values
*/

#define AF_INFOID_BYTES     {"bytes/sample:", "sample_n_bytes:", NULL}
#define AF_INFOID_DATE      {"date:", "creation_date:", "recording_date:", \
                             "originator_date:", NULL}
#define AF_INFOID_DESC      {"description:", NULL}
#define AF_INFOID_FULLSCALE {"full_scale:", NULL}
#define AF_INFOID_NBS       {"bits/sample:", "bits_per_sample:", \
                             "sample_sig_bits:", "A/D:", NULL}
#  define AF_INFOUNIT_NBS     {"bit*s", "b", "Bit*s", ""}
#define AF_INFOID_NCHAN     {"channels:", "channel_count:", NULL}
#define AF_INFOID_NFRAME    {"frames:", "sample_count:", "Number of points:", \
                             NULL}
#define AF_INFOID_PROG      {"program:", NULL}
#define AF_INFOID_PROD      {"product:", NULL}
#define AF_INFOID_SFREQ     {"sample_rate:", "sampling rate:", \
                             "Sampling Frequency:", NULL}
#  define AF_INFOUNIT_SFREQ   {"Hz", "kHz", "KHz", ""}
#  define AF_INFOSCALE_SFREQ  {1,    1000,  1000,  1}
#define AF_INFOID_SPKR      {"loudspeakers:", NULL}
#define AF_INFOID_SWAP      {"byte_order:", "sample_byte_format:", NULL}
#  define AF_INFOKEY_SWAP   {"n*ative", "N*ative", "1", "s*wap", \
                             "l*ittle-endian", "01", \
                             "b*ig-endian", "10", NULL}
#  define AF_INFOVAL_SWAP   {DS_NATIVE, DS_NATIVE, DS_NATIVE, DS_SWAP, \
                             DS_EL, DS_EL, \
                             DS_EB, DS_EB}
#define AF_INFOID_TITLE     {"title:", "name:", "comment:", "comments:", \
                             "utterance_id:", "display_text:", NULL}

/* AF/info function prototypes */
int
AFaddInfoChunk(const char Recs[], int Nc, struct AF_info *AFInfo);
void
AFaddInfoRec(const char Ident[], const char text[], int Size,
             struct AF_info *AFInfo);
int
AFdecSpeaker(const char String[], unsigned char *SpkrConfig, int MaxN);
int
AFdecInfoVVU(const char *String, int Type, void *Val, const char *UnitsTable[],
             int *Key);
int
AFdelInfoRec(const char Ident[], struct AF_info *AFInfo);
int
AFgetDelInfoRec(const char *Ident[],  struct AF_info *WInfo, char text[],
                int NcMax);
int
AFgetInfoBytes(const struct AF_info *AFInfo);
long int
AFgetInfoChan(const struct AF_info *AFInfo);
long int
AFgetInfoFrame(const struct AF_info *AFInfo);
double
AFgetInfoFullScale(const struct AF_info *AFInfo);
struct AF_NbS
AFgetInfoNbS(const struct AF_info *AFInfo);
const char *
AFgetInfoRec(const char *RecID[], const struct AF_info *AFInfo);
double
AFgetInfoSfreq(const struct AF_info *AFInfo);
enum UT_DS_T
AFgetInfoSwap(const struct AF_info *AFInfo);
int
AFspeakerNames(const unsigned char *SpkrConfig, char *SpkrNames, int MaxNc);
unsigned char *
AFgetInfoSpkr(const struct AF_info *AFInfo);

#ifdef __cplusplus
}
#endif

#endif /* AFinfo_h_ */
