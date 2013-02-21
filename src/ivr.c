/*-------------------------------------------------------------------------
   AR1688 IVR function copy right information

   Copyright (c) 2006-2011. Lin, Rongrong <woody@palmmicro.com>

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
#include "ar168.h"
#include "core.h"
#include "apps.h"

#include "bank1.h"

#ifdef SYS_IVR

UCHAR IVR_pString[MAX_IVR_STRING];
UCHAR IVR_iIndex;
UCHAR IVR_iPage;
USHORT IVR_iLength;
BOOLEAN IVR_bRepeat;

void _PrepareIPIVR()
{
	UCHAR i, iTmp;
	UCHAR pString[16];
	
	IVR_iLength = 0;
	IVR_iIndex = 0;

	IVR_pString[0] = SYSTEM_IVR_LOCAL_OFFSET;
	IVR_pString[1] = SYSTEM_IVR_IP_OFFSET;

	ip2str(Sys_pIpAddress, pString);
	i = 0;
	while (1)
	{
		iTmp = pString[i];
		if (iTmp)
		{
			if (iTmp >= (UCHAR)'0' && iTmp <= (UCHAR)'9')
			{
				IVR_pString[i+2] = iTmp - '0';
			}
			else if (iTmp == (UCHAR)'*' || iTmp == (UCHAR)'.')
			{
				IVR_pString[i+2] = SYSTEM_IVR_DOT_OFFSET;
			}
		}
		else
		{
			IVR_pString[i+2] = IVR_END_CHAR;
			return;
		}
		i ++;
	}
}

void PrepareIVR(UCHAR iType)
{
	switch (iType)
	{
	case IVR_TYPE_IP:
//		IVR_bRepeat = TRUE;
		IVR_bRepeat = FALSE;
		IVR_iPage = SYSTEM_IVR_PAGE;
		_PrepareIPIVR();
		break;
	}
}

#endif
