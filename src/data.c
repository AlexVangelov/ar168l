/*-------------------------------------------------------------------------
   AR1688 Data function copy right information

   Copyright (c) 2006-2010. Lin, Rongrong <woody@palmmicro.com>

   All rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of Palmmicro nor the names of its contributors may be
	  used to endorse or promote products derived from this software without
	  specific prior written permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
   ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
   DISCLAIMED. IN NO EVENT SHALL COPYRIGHT OWNER BE LIABLE FOR ANY
   DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
   (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
   ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
-------------------------------------------------------------------------*/

#include "type.h"
#include "core.h"

// those are the c version of functions in datatype.s, for program reference only

/*
UCHAR P_HIBYTE(USHORT worddata)
{
	return (UCHAR)(worddata >> 8);
}

UCHAR P_LOBYTE(USHORT worddata)
{
	return (UCHAR)(worddata & 0x00ff);
}

USHORT P_LOWORD(ULONG longdata)
{
	return (USHORT)(longdata & 0x0000ffff);
}

USHORT P_HIWORD(ULONG longdata)
{
	return (USHORT)((longdata & 0xffff0000) >> 16);
}

USHORT P_MAKEWORD(UCHAR lowbyte, UCHAR highbyte)
{
	USHORT rt;
	rt = highbyte;
	rt <<= 8;
	rt += lowbyte;
	return rt;
}

ULONG P_MAKELONG(USHORT lowword, USHORT highword)
{
	ULONG rt;
	rt = highword;
	rt <<= 16;
	rt += lowword;
	return rt;
}

USHORT PCHAR2USHORT_L(PCHAR src)
{
	USHORT dst;
	dst = P_MAKEWORD(src[0], src[1]);
	return dst;
}

USHORT PCHAR2USHORT(PCHAR src)
{
	USHORT dst;
	dst = P_MAKEWORD(src[1], src[0]);
	return dst;
}

void USHORT2PCHAR(USHORT src, PCHAR dst)
{
	dst[0] = P_HIBYTE(src);
	dst[1] = (UCHAR)src;
}

void USHORT2PCHAR_L(USHORT src, PCHAR dst)
{
	dst[0] = (UCHAR)src;
	dst[1] = P_HIBYTE(src);
}

ULONG PCHAR2ULONG(PCHAR src)
{
	ULONG dst;
	dst = P_MAKELONG(P_MAKEWORD(src[3], src[2]), P_MAKEWORD(src[1], src[0]));
	return dst;
}

ULONG PCHAR2ULONG_L(PCHAR src)
{
	ULONG dst;
	dst = P_MAKELONG(P_MAKEWORD(src[0], src[1]), P_MAKEWORD(src[2], src[3]));
	return dst;
}

void ULONG2PCHAR(ULONG src, PCHAR dst)
{
	dst[0] = P_HIBYTE(P_HIWORD(src));
	dst[1] = P_LOBYTE(P_HIWORD(src));
	dst[2] = P_HIBYTE(P_LOWORD(src));
	dst[3] = P_LOBYTE(P_LOWORD(src));
}

void ULONG2PCHAR_L(ULONG src, PCHAR dst)
{
	dst[3] = P_HIBYTE(P_HIWORD(src));
	dst[2] = P_LOBYTE(P_HIWORD(src));
	dst[1] = P_HIBYTE(P_LOWORD(src));
	dst[0] = P_LOBYTE(P_LOWORD(src));
}

PCHAR strchr(PCHAR string, UCHAR ch)
{
	while (*string && *string != ch)
		string++;

	if (*string == ch)
		return(string);
	return ( NULL );
}

USHORT strlen(PCHAR pStr) 
{
	int i;

	i = 0;
	while (*pStr++) 
		i++ ;

	return i;
}

USHORT strcmp (PCHAR dst, PCHAR src)
{
	while( ! (*src - *dst) && *dst)
		++src, ++dst;

	return *src - *dst;
}

void strcpy(PCHAR dst, PCHAR src)
{
    while (*dst++ = *src++);
}

  
void strcat (PCHAR dst, PCHAR src)
{
        while( *dst )
                dst++;                   // find end of dst

        while( *dst++ = *src++ ) ;       // Copy src to end of dst
}

void strcat_char(PCHAR dst, UCHAR ch)
{
	while( *dst )
		dst++;                   // find end of dst
	*dst ++ = ch;
	*dst = 0;
}


void memset(PCHAR dst, UCHAR ch, USHORT len)
{
	do
	{
		*dst ++ = ch;
		len --;
	} while (len);
}

USHORT memcmp(PCHAR buf1, PCHAR buf2, USHORT len)
{
	UCHAR iVal;

	do
	{
		iVal = *buf1++;
		iVal -= *buf2++;
		len --;
	} while (len && !iVal);

	return iVal;
}

void memcpy(PCHAR dst, PCHAR src, USHORT len)
{
	do
	{
		*dst++ = *src++;
		len--;
	} while (len); 
}

void memmove(PCHAR dst, PCHAR src, USHORT len)
{
	dst += len - 1;
	src += len - 1;
	do
	{
		*dst-- = *src--;
		len--;
	} while (len); 
}

void fixed_input(PCHAR dst, PCHAR src, USHORT len)
{
	do
	{
		*dst++ = *src;
		len --;
	} while (len);
}

void fixed_output(PCHAR dst, PCHAR src, USHORT len)
{
	do
	{
		*dst = *src++;
		len --;
	} while (len);
}

ULONG short_sum(PCHAR pVal, USHORT iLen)
{
	ULONG lSum;
	USHORT * p;

	if (iLen & 1)
	{
		pVal[iLen] = 0;
	}
	iLen ++;
	iLen >>= 1;

	p = (USHORT *)pVal;
	lSum = 0;
	do
	{
		lSum += *p ++;
		iLen --;
	} while (iLen);
	return lSum;
}

USHORT ip_checksum(ULONG lSum)
{
    lSum = (lSum >> 16) + (lSum & 0xffff);
    lSum += (lSum >> 16);
    return ~((USHORT)lSum);
}

ULONG mul_16x8(USHORT i16, UCHAR i8)
{
	ULONG l;
	USHORT s;
	UCHAR c;

	c = P_HIBYTE(i16);
	s = mul_8x8(c, i8);
	l = s;
	l <<= 8;

	c = P_LOBYTE(i16);
	s = mul_8x8(c, i8);
	l += s;
	return l;
}

ULONG mul_32x8(ULONG l32, UCHAR i8)
{
	ULONG l1, l2;
	USHORT s;

	s = P_HIWORD(l32);
	s = (USHORT)mul_16x8(s, i8);
	l1 = P_MAKELONG(0, s);

	s = P_LOWORD(l32);
	l2 = mul_16x8(s, i8);
	l2 += l1;
	return l2;
}
*/

UCHAR mod_32x8(ULONG l32, UCHAR i8)
{
	UCHAR p[4];
	UCHAR i, iRemain;

	ULONG2PCHAR(l32, p);
	iRemain = 0;
	for (i = 0; i < 4; i ++)
	{
		iRemain = mod_16x8(P_MAKEWORD(p[i], iRemain), i8);
	}
	return iRemain;
}

ULONG div_32x8(ULONG l32, UCHAR i8)
{
	UCHAR p[4];
	UCHAR i, iRemain, iVal;

	ULONG2PCHAR(l32, p);
	iRemain = 0;
	for (i = 0; i < 4; i ++)
	{
		iVal = mod_16x8(P_MAKEWORD(p[i], iRemain), i8);
		p[i] = div_16x8(P_MAKEWORD(p[i], iRemain), i8);
		iRemain = iVal;
	}
	return PCHAR2ULONG(p);
}
