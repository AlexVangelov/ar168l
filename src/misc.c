/*-------------------------------------------------------------------------
   AR1688 Misc function copy right information

   Copyright (c) 2006-2012. Lin, Rongrong <woody@palmmicro.com>
                            Tang, Li      <tangli@palmmicro.com>

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

// Misc functions

void itoa(USHORT sVal, PCHAR pBuf, UCHAR iRadix)
{
    PCHAR p;            // pointer to traverse string 
    PCHAR pFirstDigit;  // pointer to first digit 
	UCHAR iTmp;          // temp UCHAR 

    p = pBuf;
    pFirstDigit = p;        // save pointer to first digit 

    do 
    {
		iTmp = mod_16x8(sVal, iRadix);
		sVal = div_16x8(sVal, iRadix);

        // convert to ascii and store 
		*p++ = (iTmp > 9) ? iTmp - 10 + 'A' : iTmp + '0';  // a letter or a digit
    } while (sVal > 0);

	// We now have the digit of the number in the buffer, but in reverse order.  Thus we reverse them now. 
	
    *p-- = '\0';            // terminate string; p points to last digit 

    do 
    {
		iTmp = *p;
        *p = *pFirstDigit;
        *pFirstDigit = iTmp;   // swap *p and *pFirstDigit 
        --p;
        ++pFirstDigit;         // advance to next two digits 
    } while (pFirstDigit < p); // repeat until halfway 
}

USHORT atoi(PCHAR pBuf, UCHAR iRadix)
{
	UCHAR iPos, iVal;
	USHORT sTotal;

	sTotal = 0;
	iPos = 0;
	do
	{
		iVal = pBuf[iPos];
		if (iVal >= (UCHAR)'0' && iVal <= (UCHAR)'9')
		{
			iVal -= '0';
		}
		else 
		{
			if (iRadix == 16)
			{
				if (iVal >= (UCHAR)'a' && iVal <= (UCHAR)'f')
				{
					iVal -= 'a' - 10;
				}
				else if (iVal >= (UCHAR)'A' && iVal <= (UCHAR)'F')
				{
					iVal -= 'A' - 10;
				}
				else
				{
					break;
				}
			}
			else
			{
				break;
			}
		}
		sTotal = (USHORT)mul_16x8(sTotal, iRadix);
		sTotal += iVal;
		iPos ++;
	} while (1);

	return sTotal;
}

void ltoa(ULONG lVal, PCHAR pBuf, UCHAR iRadix)
{
    PCHAR p;            // pointer to traverse string 
    PCHAR pFirstDigit;  // pointer to first digit 
	UCHAR iTmp;          // temp UCHAR 

    p = pBuf;
    pFirstDigit = p;        // save pointer to first digit 

    do 
    {
		iTmp = mod_32x8(lVal, iRadix);
		lVal = div_32x8(lVal, iRadix);

        // convert to ascii and store 
		*p++ = (iTmp > 9) ? iTmp - 10 + 'A' : iTmp + '0';  // a letter or a digit
    } while (lVal);

	// We now have the digit of the number in the buffer, but in reverse order.  Thus we reverse them now. 
	
    *p-- = '\0';            // terminate string; p points to last digit 

    do 
    {
		iTmp = *p;
        *p = *pFirstDigit;
        *pFirstDigit = iTmp;   // swap *p and *pFirstDigit 
        --p;
        ++pFirstDigit;         // advance to next two digits 
    } while (pFirstDigit < p); // repeat until halfway 
}

ULONG atol(PCHAR pBuf, UCHAR iRadix)
{
	UCHAR iPos, iVal;
	ULONG lTotal;

	lTotal = 0;
	iPos = 0;
	do
	{
		iVal = pBuf[iPos];
		if (iVal >= (UCHAR)'0' && iVal <= (UCHAR)'9')
		{
			iVal -= '0';
		}
		else 
		{
			if (iRadix == 16)
			{
				if (iVal >= (UCHAR)'a' && iVal <= (UCHAR)'f')
				{
					iVal -= 'a' - 10;
				}
				else if (iVal >= (UCHAR)'A' && iVal <= (UCHAR)'F')
				{
					iVal -= 'A' - 10;
				}
				else
				{
					break;
				}
			}
			else
			{
				break;
			}
		}
		lTotal = mul_32x8(lTotal, iRadix);
		lTotal += iVal;
		iPos ++;
	} while (1);

	return lTotal;
}

void ip2str(PCHAR pIP, PCHAR pStr)
{
	UCHAR i;

	pStr[0] = 0;
	for (i = 0; i < IP_ALEN; i ++)
	{
		itoa(pIP[i], (PCHAR)(pStr + strlen(pStr)), 10);
		if (i < IP_ALEN-1)
		{
			strcat_char(pStr, '.');
		}
	}
}

BOOLEAN str2ip(PCHAR pStr, PCHAR pDstIP)
{
	UCHAR iVal, iCount;
	USHORT iTotal;
	UCHAR pIp[IP_ALEN];

	iTotal = 0;
	iCount = 0;
	do
	{
		iVal = *pStr;
		pStr ++;
		if (iVal >= (UCHAR)'0' && iVal <= (UCHAR)'9')
		{
//			iTotal *= 10;
			iTotal = (USHORT)mul_16x8(iTotal, 10);
			iTotal += iVal - '0';
			if (iTotal >= 256)
			{
				return FALSE;
			}
		}
		else
		{
			pIp[iCount] = (UCHAR)iTotal;
			iTotal = 0;
			iCount ++;
			if (iCount < IP_ALEN)
			{
				if (iVal != (UCHAR)'.' && iVal != (UCHAR)'*')
				{
					return FALSE;
				}
			}
			else
			{
				break;
			}
		}
	} while (iVal);

	memcpy4(pDstIP, pIp);
	return TRUE;
}

void mac2str(PCHAR pMac, PCHAR pString)
{
	UCHAR i, iVal;
	PCHAR p;

	p = pString;
	for (i = 0; i < HW_ALEN; i ++)
	{
		iVal = pMac[i];
		if (iVal < 0x10)
		{
			*p ++ = '0';
			itoa(iVal, p, 16);
			p ++;
		}
		else
		{
			itoa(iVal, p, 16);
			p += 2;
		}
		*p ++ = '-';
	}
	p --;
	*p = 0;
}

const UCHAR _cUpAscMapping[] = "0123456789ABCDEF";
const UCHAR _cLowAscMapping[] = "0123456789abcdef";

UCHAR char2asc(UCHAR iChar, BOOLEAN bUpper)
{
	iChar &= 0x0f;
	return (bUpper) ? _cUpAscMapping[iChar] : _cLowAscMapping[iChar];
}

void char2asc_str(PCHAR pDst, PCHAR pSrc, UCHAR iLen, BOOLEAN bUpper)
{
	UCHAR i, iVal;
	PCHAR p;

	p = pDst;
	for (i = 0; i < iLen; i ++)
	{
		iVal = pSrc[i];
		*p ++ = char2asc((UCHAR)(iVal>>4), bUpper);
		*p ++ = char2asc(iVal, bUpper);
	}
	*p = 0;
}

const UCHAR _cDtmfKeys[MAP_KEY_NUM] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '*', '#', 'T'};

UCHAR ascii2keyid(UCHAR iAscii)
{
	UCHAR i;

	for (i = 0; i < MAP_KEY_NUM; i ++)
	{
		if (iAscii == _cDtmfKeys[i])
		{		
			return i;
		}
	}

	return MAP_KEY_UNKNOWN;
}

UCHAR keyid2ascii(UCHAR iKeyid)
{
	return (iKeyid < MAP_KEY_NUM) ? (UCHAR)(_cDtmfKeys[iKeyid]) : 0;
}

BOOLEAN IsValidIP(PCHAR pIP)
{
	UCHAR iFirst, iSecond;

	iFirst = pIP[0];
	iSecond = pIP[1];

	if (iFirst == 127 || iFirst >= 224)
	{
		return FALSE;
	}
	else if (!iFirst)
	{
		if (!iSecond)
		{
			return FALSE;
		}
	}

	return TRUE;
}

BOOLEAN IsPrivateIP(PCHAR pIP)
{
	UCHAR iFirst, iSecond;

	iFirst = pIP[0];
	iSecond = pIP[1];

	if (iFirst == 10)
	{
		return TRUE;
	}
	else if (iFirst == 192 )
	{
  		if (iSecond == 168)
		{
			return TRUE;
		}
	}
	else if (iFirst == 172)
	{
		if (iSecond >= 16 && iSecond <= 31)
		{
			return TRUE;
		}
	}
	return FALSE;
}

void Fill2Digits(PCHAR pBuf, UCHAR iVal)
{
	pBuf[0] = div_16x8(iVal, 10) + '0';
	pBuf[1] = mod_16x8(iVal, 10) + '0';
}

PCHAR SkipField(PCHAR pBuf, UCHAR iChar)
{
	PCHAR pCur;

	pCur = strchr(pBuf, iChar);
	if (pCur)
	{
		*pCur = 0;
		pCur ++;
	}
	return pCur;
}

