/*-------------------------------------------------------------------------
   AR1688 Protocol function copy right information

   Copyright (c) 2007-2010. Lin, Rongrong <woody@palmmicro.com>

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
#include "apps.h"
#include "bank5.h"

// Put SIP/RTP and IAX2 common functions (NOT version.h related) here

void CallTimeZero(CALL_TIME * p)
{
	memset((PCHAR)p, 0, sizeof(CALL_TIME));
}

void CallTimeTick(CALL_TIME * p)
{
	p->iSec ++;
	if (p->iSec == 60)
	{
		p->iSec -= 60;
		p->iMin ++;
		if (p->iMin == 60)
		{
			p->iMin -= 60;
			p->iHour ++;
		}
	}
}

const UCHAR _cFrameTime[VOICE_CODER_NUM] = {G711_FRAME_TIME, G711_FRAME_TIME, G726_FRAME_TIME, GSM610_FRAME_TIME, ILBC_30MS_TIME, SPEEX_FRAME_TIME, G729_FRAME_TIME};

UCHAR GetFrameTime(UCHAR iVoiceCoder)
{
	if (iVoiceCoder == VOICE_CODER_ILBC)
	{
		return (Sys_iIlbcFrame) ? ILBC_30MS_TIME : ILBC_20MS_TIME;
	}
	else
	{
		return _cFrameTime[iVoiceCoder-1];
	}
}

