/*-------------------------------------------------------------------------
   AR1688 Digitmap function copy right information

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

#include "type.h"
#include "ar168.h"
#include "core.h"
#include "apps.h"

#define DIGITMAP_NO_MATCH		0	/* no match within the map */
#define DIGITMAP_PARTIAL_MATCH	1	/* possible match with more keys pressed */
#define DIGITMAP_FULL_MATCH		2	/* exact match with one item in the map */

#define DIGITMAP_MASK_X		0x03ff
const USHORT _cDigitMapMask[MAP_KEY_NUM] = 
{
	0x0001, 0x0002, 0x0004, 0x0008, 0x0010, 0x0020, 0x0040, 0x0080,
	0x0100, 0x0200, 0x0400, 0x0800, 0x1000
};

BOOLEAN _IsMatched(PCHAR pEvent, PSHORT pMap, UCHAR iNum)
{
	UCHAR i;

	for (i = 0; i < iNum; i ++)
	{
		if ((_cDigitMapMask[pEvent[i]] & pMap[i]) == 0)
		{
			return FALSE;
		}
	}
	return TRUE;
}

BOOLEAN _IsPeriodMatched(PCHAR pEvent, UCHAR iEventNum, PSHORT pMap, UCHAR iMapLen)
{
	UCHAR i;

	for (i = 0; i < iMapLen; i ++)
	{
		if ((_cDigitMapMask[pEvent[i]] & pMap[i]) == 0)
		{
			return FALSE;
		}
	}
	for (i = iMapLen; i < iEventNum; i ++)
	{
		if ((_cDigitMapMask[pEvent[i]] & pMap[iMapLen-1]) == 0)
			return FALSE;
	}
	return TRUE;
}

BOOLEAN map_convert(PSHORT pMap, PCHAR pBuf, PCHAR pLen)
{
	UCHAR i, iLen;
	USHORT sVal;
	BOOLEAN bVarLen;

	iLen = 0;
	bVarLen = FALSE;
	while (*pBuf)
	{
		sVal = 0;
		if (*pBuf == (UCHAR)'[')
		{
			pBuf ++;
			while (*pBuf != (UCHAR)']')
			{
				if (pBuf[1] == (UCHAR)'-')
				{
					for (i = (UCHAR)(*pBuf - '0'); i <= (UCHAR)(pBuf[2] - '0'); i ++)
					{
						sVal |= _cDigitMapMask[i];
					}
					pBuf += 3;
				}
				else
				{
					i = ascii2keyid(*pBuf);
					if (i != MAP_KEY_UNKNOWN)
					{
						sVal |= _cDigitMapMask[i];
					}
					pBuf ++;
				}
			}
		}
		else if (*pBuf == (UCHAR)'x' || *pBuf == (UCHAR)'X')
		{
			sVal = DIGITMAP_MASK_X;
		}
		else if (*pBuf == (UCHAR)'.')
		{
			bVarLen = TRUE;
			pBuf ++;
			continue;
		}
		else
		{
			i = ascii2keyid(*pBuf);
			if (i != MAP_KEY_UNKNOWN)
			{
				sVal = _cDigitMapMask[i];
			}
			else
			{
				pBuf ++;
				continue;
			}
		}
		*pMap = sVal;
		pMap ++;
		pBuf ++;
		iLen ++;
	}

	*pLen = iLen;
	return bVarLen;
}

UCHAR map_match(PCHAR pEvent, UCHAR iEventNum)
{
	USHORT pMap[DIGITMAP_ENTRY_LEN];
	UCHAR pBuf[DIGITMAP_ENTRY_LEN];
	UCHAR i, iRtVal, iLen;
	USHORT sOffset;
	BOOLEAN bVarLen;

	iRtVal = DIGITMAP_NO_MATCH;
	sOffset = OPT_DIGIT_MAP;
	for (i = 0; i < DIGITMAP_ENTRY_NUM; i ++, sOffset += DIGITMAP_ENTRY_LEN)
	{
		OptionsGetString(pBuf, sOffset, DIGITMAP_ENTRY_LEN);
		if (!strlen(pBuf))	continue;
		bVarLen = map_convert(pMap, pBuf, &iLen);
		if (bVarLen)
		{
			//this entry has a dot in it, so it's a variable length map
			iLen --;
			if (iEventNum > iLen)
			{
				if (_cDigitMapMask[pEvent[iEventNum-1]] & pMap[iLen])
				{
					if (_IsPeriodMatched(pEvent, (UCHAR)(iEventNum - 1), pMap, iLen))
					{
						return DIGITMAP_FULL_MATCH;
					}
				}
				else
				{
					if (_IsPeriodMatched(pEvent, iEventNum, pMap, iLen))
					{
						iRtVal = DIGITMAP_PARTIAL_MATCH;
					}
				}
			}
			else if (iEventNum == iLen)
			{
				if (_IsMatched(pEvent, pMap, iEventNum))
				{
					iRtVal = DIGITMAP_PARTIAL_MATCH;
				}
			}
			else
			{
				if (_IsMatched(pEvent, pMap, iEventNum))
				{
					iRtVal = DIGITMAP_PARTIAL_MATCH;
				}
			}
		}
		else
		{
			if (iEventNum < iLen)
			{
				if (_IsMatched(pEvent, pMap, iEventNum))
				{
					iRtVal = DIGITMAP_PARTIAL_MATCH;				
				}
			}
			else if (iEventNum == iLen)
			{
				if (_IsMatched(pEvent, pMap, iEventNum))
				{
					return DIGITMAP_FULL_MATCH;
				}
			}
		}
	}
	return iRtVal;
}

BOOLEAN MapMatch(PCHAR pNumber)
{
	UCHAR pEvent[MAX_USER_NUMBER_LEN];
	UCHAR iNum;
	
	for (iNum = 0; iNum < strlen(pNumber); iNum ++)
	{
		pEvent[iNum] = ascii2keyid(pNumber[iNum]);
	}
	return (map_match(pEvent, iNum) == DIGITMAP_PARTIAL_MATCH) ? FALSE : TRUE;
}

