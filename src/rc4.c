/*-------------------------------------------------------------------------
   AR1688 RC4 stream cipher function copy right information

   Copyright (c) 2006-2010. Tang, Li <tangli@palmmicro.com>

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

#include "version.h"
#include "type.h"
#ifdef OEM_INNOMEDIA

#include "core.h"
#include "bank3.h"


UCHAR state[256];
UCHAR x, y;

void swap(PCHAR pa, PCHAR pb)
{
	UCHAR iTemp;

	iTemp = *pa;
	*pa = *pb;
	*pb = iTemp;
}

void RC4Init(PCHAR pKey, UCHAR iKeyLen)
{
	UCHAR i;

	i = 0;
	do
	{
		state[i] = i;
		i ++;
	} while (i);

	x = 0;
	do
	{
		x += state[i] + pKey[mod_16x8(i, iKeyLen)];
		swap(&state[i], &state[x]);
		i ++;
	} while(i);
	x = 0;
	y = 0;
}

void RC4Calc(PCHAR pDst, PCHAR pSrc, USHORT sLen)
{
	USHORT k;
	UCHAR r;

	for (k = 0; k < sLen; k ++)
	{
		x ++;
		y += state[x];
		swap(&state[x], &state[y]);
		r = state[x] + state[y];
		*pDst++ = (*pSrc++) ^ state[r];
	}
}

#endif
