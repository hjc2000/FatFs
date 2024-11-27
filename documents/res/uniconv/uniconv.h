/*------------------------------------------------------------------------*/
/* Embedded Unicode Utilities                                             */
/*------------------------------------------------------------------------*/
/* This module provides Unicode utility functions work with FatFs Unicode  /
/  module, ff_unicode.c.                                                   /
/-------------------------------------------------------------------------*/
/*
/ Copyright (C) 2021, ChaN, all right reserved.
/
/ This software, Embedded Unicode Utilities, is an open source software.
/ Redistribution and use of this module in source and binary forms,
/ with or without modification, are permitted provided that the following
/ condition is met:
/
/ 1. Redistributions of source code must retain the above copyright notice,
/    this condition and the following disclaimer.
/
/ This software is provided by the copyright holder and contributors "AS IS"
/ and any warranties related to this software are DISCLAIMED.
/ The copyright owner or contributors be NOT LIABLE for any damages caused
/ by use of this software.
*/

#include "ff.h"

#ifndef UNICONV_DEF
#define UNICONV_DEF	1

#ifdef __cplusplus
extern "C" {
#endif

/*-----------------------------------------------------------------*/
/* Definitions                                                     */
/*-----------------------------------------------------------------*/

/* Code page identifiers */
#define OEM_437		437
#define OEM_720		720
#define OEM_737		737
#define OEM_771		771
#define OEM_775		775
#define OEM_850		850
#define OEM_852		852
#define OEM_855		855
#define OEM_857		857
#define OEM_860		860
#define OEM_861		861
#define OEM_862		862
#define OEM_863		863
#define OEM_864		864
#define OEM_865		865
#define OEM_866		866
#define OEM_869		869
#define OEM_932		932
#define OEM_936		936
#define OEM_949		949
#define OEM_950		950

/* Unicode encoding identifiers */
#define UNI_UTF16	1200
#define UNI_UTF32	12000
#define UNI_UTF8	65001


/*-----------------------------------------------------------------*/
/* Convert OEM code string to Unicode string                       */
/*-----------------------------------------------------------------*/
UINT oem2uni (WORD oem, const char* src, WORD utf, void* dst, UINT* szb);
/*
/ Arguments
/  oem  [IN] Input OEM code page. If FF_CODE_PAGE != 0, only
/       the code page is valid. If FF_CODE_PAGE == 0, any code
/       page supported can be specified.
/
/  src  [IN] Pointer to the nul terminated input string.
/
/  utf  [IN] Output Unicode encoding.
/
/  dst  [OUT] Buffer to store the output Unicode string without
/       terminator. Pointer type is assumed as char, WCHAR, DWORD
/       for UTF-8, UTF-16, UTF-32 respectively.
/
/  szb  [IN/OUT] Pointer to the variable to input size of output
/       buffer and to return length of output string. The unit is
/       byte regardless of encoding.
/
/ Return
/  Return value indicates the conversion status. 0 on no error and
/  status flag will be set if any error occured in the conversion.
/   bit0: Truncation. Conversion terminated due to buffer full.
/   bit1: Conversion error. Error character is replaced with a '?'.
/
*/


/*-----------------------------------------------------------------*/
/* Convert Unicode string to OEM code string                       */
/*-----------------------------------------------------------------*/
UINT uni2oem (WORD utf, const void* src, WORD oem, char* dst, UINT* szb);
/*
/ Arguments
/  utf  [IN] Input Unicode encoding.
/
/  src  [IN] Pointer to the nul terminated input Unicode string.
/       Pointer type is assumed as char, WCHAR, DWORD for UTF-8,
/       UTF-16, UTF-32 respectively.
/
/  oem  [IN] Output OEM code page. If FF_CODE_PAGE != 0, only
/       the code page is valid. If FF_CODE_PAGE == 0, any code
/       page supported can be specified.
/
/  dst  [OUT] Buffer to store the output OEM code string without
/       terminator.
/
/  szb  [IN/OUT] Pointer to the variable to input the size of
/       buffer and to return the length of output string. The unit
/       is always byte.
/
/ Return
/  Return value indicates the conversion status. 0 on no error and
/  status flag will be set if any error occured in the conversion.
/   bit0: Truncation. Conversion terminated due to buffer full.
/   bit1: Conversion error. Error character is replaced with a '?'.
/   bit2: Input decode error. Error character is replaced with a '?'.
/
*/


/*-----------------------------------------------------------------*/
/* Convert encoding of Unicode string                              */
/*-----------------------------------------------------------------*/
UINT uni2uni (WORD utfs, const void* src, WORD utfd, void* dst, UINT* szb);
/*
/ Arguments
/  utfs [IN] Input Unicode encoding.
/
/  src  [IN] Pointer to the nul terminated input Unicode string.
/       Pointer type is assumed as char, WCHAR, DWORD for UTF-8,
/       UTF-16, UTF-32 respectively.
/
/  utfd [IN] Output Unicode encoding.
/
/  dst  [OUT] Buffer to store the output Unicode string without
/       terminator. Pointer type is assumed as char, WCHAR, DWORD
/       for UTF-8, UTF-16, UTF-32 respectively.
/
/  szb  [IN/OUT] Pointer to the variable to input the size of
/       buffer and to return the length of output string. The unit
/       is always byte.
/
/ Return
/  Return value indicates the conversion status. 0 on no error and
/  status flag will be set if any error occured in the conversion.
/   bit0: Truncation. Conversion terminated due to buffer full.
/   bit2: Input decode error. Error character is replaced with a '?'.
/
*/


#ifdef __cplusplus
}
#endif

#endif	/* UNICONV_DEF */
