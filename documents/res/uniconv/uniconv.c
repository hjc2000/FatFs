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

#include "uniconv.h"

#define ERR_CHAR	'?'		/* Replacement of error character (0:discard it) */



/*---------------------------------------*/
/* Get an OEM character from the string  */
/*---------------------------------------*/
static WORD get_oem (	/* Returns a character (>=0x100: DBC, 0xFFFF:broken DBC) */
	WORD oem,			/* [IN] Input OEM code page */
	const char** src	/* [IN/OUT] Pointer to pointer to the input string */
)
{
	/* DBCS 1st byte range */
	static const BYTE dc932[] = {0x9F, 0xE0, 0xFC, 0x40, 0x7E, 0x80, 0xFC, 0x00, 0x00};
	static const BYTE dc936[] = {0xFE, 0x00, 0x00, 0x40, 0x7E, 0x80, 0xFE, 0x00, 0x00};
	static const BYTE dc949[] = {0xFE, 0x00, 0x00, 0x41, 0x5A, 0x61, 0x7A, 0x81, 0xFE};
	static const BYTE dc950[] = {0xFE, 0x00, 0x00, 0x40, 0x7E, 0xA1, 0xFE, 0x00, 0x00};
	const BYTE *dc, *p = (const BYTE*)*src;
	WORD chr;


	chr = *p++;	/* Get a character */
	if (chr >= 0x81) {	/* Might be DBC 1st byte */
		switch (oem) {
		case OEM_932: dc = dc932; break;
		case OEM_936: dc = dc936; break;
		case OEM_949: dc = dc949; break;
		case OEM_950: dc = dc950; break;
		default: dc = 0;
		}
		if (dc && (chr <= dc[0] || (chr >= dc[1] && chr <= dc[2])) && *p) {	/* Is DBC 1st byte? */
			if ((*p >= dc[3] && *p <= dc[4]) || (*p >= dc[5] && *p <= dc[6]) || (*p >= dc[7] && *p <= dc[8])) {	/* Is DBC 2nd byte? */
				chr = chr << 8 | *p++;	/* Get DBC 2nd byte */
			} else {
				chr = 0xFFFF;	/* Broken DBC */
			}
		}
	}
	*src = (const char*)p;	/* Return next location */

	return chr;
}



/*-----------------------------------------*/
/* Get a Unicode character from the string */
/*-----------------------------------------*/
static DWORD get_uni (	/* Returns code point (0xFFFFFFFF:decode error) */
	WORD utf,			/* [IN] Input encoding */
	const void** src	/* [IN/OUT] Pointer to pointer to the string */
)
{
	const BYTE *bp;
	const WCHAR *wp;
	const DWORD *dp;
	DWORD chr = 0;
	int nf;


	switch (utf) {
	case UNI_UTF8:
		bp = (const BYTE*)*src;
		chr = *bp++;		/* Get a unit */
		if (chr >= 0x80) {	/* Multi-byte? */
			if        ((chr & 0xE0) == 0xC0) {	/* 2-byte sequence? */
				chr &= 0x1F; nf = 1;
			} else if ((chr & 0xF0) == 0xE0) {	/* 3-byte sequence? */
				chr &= 0x0F; nf = 2;
			} else if ((chr & 0xF8) == 0xF0) {	/* 4-byte sequence? */
				chr &= 0x07; nf = 3;
			} else {							/* Wrong code */
				nf = 4;
			}
			if (nf != 4) {
				do {	/* Get trailing bytes */
					if ((*bp & 0xC0) != 0x80) break;	/* Broken sequence? */
					chr = chr << 6 | (*bp++ & 0x3F);
				} while (--nf);
			}
			if (nf || chr < 0x80 || chr >= 0x110000 || (chr >= 0xD800 && chr <= 0xDFFF)) chr = 0xFFFFFFFF;	/* Wrong code? */
		}
		*src = bp;	/* Return next locaiton */
		break;

	case UNI_UTF16:
		wp = (const WCHAR*)*src;
		chr = *wp++;	/* Get a unit */
		if (chr >= 0xD800 && chr <= 0xDFFF) {	/* Surrogate? */
			if (chr <= 0xDBFF && *wp >= 0xDC00 && *wp <= 0xDFFF) {	/* Valid surrogate? */
				chr = ((chr & 0x3FF) << 10) + (*wp++ & 0x3FF) + 0x10000;
			} else {
				chr = 0xFFFFFFFF;
			}
		}
		*src = wp;	/* Return next locaiton */
		break;

	case UNI_UTF32:
		dp = (const DWORD*)*src;
		chr = *dp++;	/* Get a unit */
		if (chr >= 0x110000 || (chr >= 0xD800 && chr <= 0xDFFF)) chr = 0xFFFFFFFF;	/* Valid code? */
		*src = dp;	/* Return next locaiton */
		break;
	}
	return chr;
}


/*---------------------------------------*/
/* Put a Unicode character on the memory */
/*---------------------------------------*/
static UINT put_uni (	/* Returns number of bytes written (0:not enough buffer) */
	DWORD chr,			/* [IN] Code point to store (0..0x10FFFF) */
	WORD utf,			/* [IN] Output encoding */
	void* dst,			/* [OUT] Buffer to store the encoded character */
	UINT szb			/* [IN] Size of output buffer */
)
{
	char *bp;
	WCHAR *wp;
	UINT bw = 0;


	switch (utf) {
	case UNI_UTF8:
		bp = (char*)dst;
		if (chr < 0x80) {	/* Single byte? (0..7F) */
			if (szb < 1) break;
			*bp = (char)chr;
			bw = 1;
			break;
		}
		if (chr < 0x800) {	/* 2-byte? (80..7FF) */
			if (szb < 2) break;
			*bp++ = (char)(0xC0 | (chr >> 6 & 0x1F));
			*bp   = (char)(0x80 | (chr >> 0 & 0x3F));
			bw = 2;
			break;
		}
		if (chr < 0x10000) {	/* 3-byte? (800..FFFF) */
			if (szb < 3) break;
			*bp++ = (char)(0xE0 | (chr >> 12 & 0x0F));
			*bp++ = (char)(0x80 | (chr >> 6 & 0x3F));
			*bp   = (char)(0x80 | (chr >> 0 & 0x3F));
			bw = 3;
			break;
		}
		/* 4-byte (10000..10FFFF) */
		if (szb < 4) break;
		*bp++ = (char)(0xF0 | (chr >> 18 & 0x07));
		*bp++ = (char)(0x80 | (chr >> 12 & 0x3F));
		*bp++ = (char)(0x80 | (chr >> 6 & 0x3F));
		*bp   = (char)(0x80 | (chr >> 0 & 0x3F));
		bw = 4;
		break;

	case UNI_UTF16:
		wp = (WCHAR*)dst;
		if (chr < 0x10000) {	/* BMP? */
			if (szb < 2) break;
			*wp = (WCHAR)chr;
			bw = 2;
		} else {				/* Make surrogate pair */
			if (szb < 4) break;
			chr -= 0x10000;
			*wp++ = (WCHAR)(chr >> 10 | 0xD800);
			*wp   = (WCHAR)((chr & 0x3FF) | 0xDC00);
			bw = 4;
		}
		break;

	case UNI_UTF32:
		if (szb < 4) break;
		*(DWORD*)dst = chr;
		bw = 4;
		break;
	}

	return bw;
}



/*-----------------------------------------------------------------*/
/* Convert OEM code string to Unicode string                       */
/*-----------------------------------------------------------------*/

UINT oem2uni (WORD oem, const char* src, WORD utf, void* dst, UINT* szb)
{
	UINT nw, i, rv;
	WORD chr;
	BYTE *outbuf = (BYTE*)dst;


	i = rv = 0;
	for (;;) {
		chr = get_oem(oem, &src);		/* Get an OEM character */
		if (chr == 0) break;			/* End of string? */
		chr = ff_oem2uni(chr, oem);		/* Convert it into Unicode */
		if (chr == 0) {					/* Conversion error? */
			rv |= 2; chr = ERR_CHAR;	/* Replace it with a replacement */
			if (chr == 0) continue;		/* Discard error char? */
		}
		nw = put_uni(chr, utf, &outbuf[i], *szb);	/* Put the converted character */
		if (nw == 0) {					/* Buffer overflow? */
			rv |= 1; break;
		}
		i += nw; *szb -= nw;
	}

	*szb = i;
	return rv;
}



/*-----------------------------------------------------------------*/
/* Convert Unicode string to OEM code string                       */
/*-----------------------------------------------------------------*/

UINT uni2oem (WORD utf, const void* src, WORD oem, char* dst, UINT* szb)
{
	UINT nw, i, rv;
	DWORD chr;


	i = rv = 0;
	for (;;) {
		chr = get_uni(utf, &src);		/* Get a Unicode character */
		if (chr == 0) break;			/* End of string? */
		if (chr == 0xFFFFFFFF) {		/* Decode error? */
			rv |= 4; chr = ERR_CHAR;	/* Replace it with a replacement */
			if (chr == 0) continue;		/* Discard error char? */
		}
		chr = ff_uni2oem(chr, oem);		/* Convert it */
		if (chr == 0) {					/* Conversion error? */
			rv |= 2; chr = ERR_CHAR;	/* Replace it with a replacement */
			if (chr == 0) continue;		/* Discard error char? */
		}
		nw = 0;
		if (chr >= 0x100) {		/* DBC? */
			if (*szb >= 2) {	/* Sufficient buffer? */
				nw = 2;			/* Byte to write */
				*dst++ = (char)(chr >> 8);	/* Put DBC 1st byte */
			}
		} else {				/* SBC */
			if (*szb >= 1) nw = 1;	/* Sufficient buffer? */
		}
		if (nw == 0) {			/* Buffer overflow? */
			rv |= 1; break;
		}
		*dst++ = (char)(chr & 0xFF);	/* Put SBC or DBC 2nd byte */
		i += nw; *szb -= nw;
	}

	*szb = i;
	return rv;

}



/*-----------------------------------------------------------------*/
/* Convert encoding of Unicode string (independent of ffunicode.c) */
/*-----------------------------------------------------------------*/

UINT uni2uni (WORD utfs, const void* src, WORD utfd, void* dst, UINT* szb)
{
	UINT szd, nw, i, rv;
	DWORD chr;
	BYTE *outbuf = (BYTE*)dst;


	szd = *szb;
	i = rv = 0;
	for (;;) {
		chr = get_uni(utfs, &src);		/* Get a character */
		if (chr == 0) break;			/* End of string? */
		if (chr == 0xFFFFFFFF) {		/* Decode error? */
			rv |= 4; chr = ERR_CHAR;	/* Replace it with a replacement */
			if (chr == 0) continue;		/* Discard error char? */
		}
		nw = put_uni(chr, utfd, &outbuf[i], szd);	/* Put it */
		if (nw == 0) {			/* Buffer overflow? */
			rv |= 1; break;
		}
		i += nw; szd -= nw;
	}

	*szb = i;
	return rv;
}


