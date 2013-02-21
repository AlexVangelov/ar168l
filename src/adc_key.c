/*-------------------------------------------------------------------------
   AR1688 ADC key function copy right information

   Copyright (c) 2006-2010. Lin, Rongrong <woody@palmmicro.com>
                            Lin, Guofeng  <alanda@palmmicro.com>

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
#include "core.h"
#include "apps.h"

#include "bank1.h"

#ifdef KEY_ADC

extern UCHAR _iHandsetDownCounter;

volatile UCHAR _pADC_KeyVal[KEY_ADC_STAGE];
volatile UCHAR _pADC_KeyVal2[KEY_ADC_STAGE];
volatile UCHAR _iADC_KeyStage, _iADC_KeyStage2;

UCHAR _iADC_OldKey, _iADC_OldKey2;

void ADC_KeyStart()
{
	_iADC_KeyStage = 0;
	_iADC_KeyStage2 = 0;
	_iADC_OldKey = 0;
	_iADC_OldKey2 = 0;
	memset(_pADC_KeyVal, 0, KEY_ADC_STAGE);
	memset(_pADC_KeyVal2, 0, KEY_ADC_STAGE);

	_iHandsetDownCounter = 0;
}

#ifdef KEY_ADC_2X7	//	7x2 ADC keys
#define KEY_ADC_MAX		7
const UCHAR _cADC_KeyLimits[] = {0x00, 0x01, 0x34, 0x5c, 0x7f, 0xa2, 0xca, 0xff};
#else				//	11x2 ADC keys
#define KEY_ADC_MAX		11
const UCHAR _cADC_KeyLimits[] = {0x00, 0x01, 0x22, 0x41, 0x59, 0x6f, 0x81, 0x94, 0xa9, 0xc1, 0xdd, 0xff};
#endif

UCHAR _ADC_GetKey(UCHAR iVal)
{
	UCHAR i;

	iVal -= 0x7f;
	for (i = 0; i < KEY_ADC_MAX; i ++)
	{
		if (iVal > _cADC_KeyLimits[i] && iVal <= _cADC_KeyLimits[i+1])		break;
	}
	return i;
}

UCHAR _ADC_JudgeKey(PCHAR pVals)
{
	UCHAR i, iVal, iKey;
	UCHAR pArray[KEY_ADC_MAX+1];

	memset(pArray, 0, KEY_ADC_MAX+1);
	for (i = 0; i < KEY_ADC_STAGE; i ++)
	{
		iKey = _ADC_GetKey(pVals[i]);
		pArray[iKey] ++;
	}

	iKey = 0;
	iVal = 0;
	for (i = 0; i <= KEY_ADC_MAX; i ++)
	{
		if (pArray[i] > iVal)
		{
			iVal = pArray[i];
			iKey = i;
		}
	}
	return iKey;
}

BOOLEAN _bIsADC_HandsetKey(UCHAR iKey)
{
#ifdef KEY_ADC_2X7	//	7x2 ADC keys
	if (iKey == (UCHAR)'v')	return TRUE;
#else				//	7x11 ADC keys
	if (iKey == (UCHAR)'z')	return TRUE;
#endif
	return FALSE;
}

void ADC_KeyRun()
{
	UCHAR iKey;

	if (_iADC_KeyStage == KEY_ADC_STAGE)
	{
		_iADC_KeyStage = 0;
		iKey = _ADC_JudgeKey(_pADC_KeyVal);
		if (iKey == KEY_ADC_MAX)
		{
			_iADC_OldKey = 0;
		}
		else
		{
			iKey += 'e';
			if (_iADC_OldKey != iKey)
			{
				_iADC_OldKey = iKey;
				UI_HandleKeys(iKey);
			}
		}
	}
	if (_iADC_KeyStage2 == KEY_ADC_STAGE)
	{
		_iADC_KeyStage2 = 0;
		iKey = _ADC_JudgeKey(_pADC_KeyVal2);

		if (iKey == KEY_ADC_MAX)
		{
			HookHandleHandsetDown(2);
			_iADC_OldKey2 = 0;
		}
		else
		{
			_iHandsetDownCounter = 0;
			iKey += 'p';
			if (_bIsADC_HandsetKey(iKey))
			{
				OnHook();
				_iADC_OldKey2 = 0;
			}
			else
			{
				if (_iADC_OldKey2 != iKey)
				{
					_iADC_OldKey2 = iKey;
					UI_HandleKeys(iKey);
				}
			}
		}
	}
}

#endif	// ADC_KEY

