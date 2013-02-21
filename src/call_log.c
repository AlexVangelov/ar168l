/*-------------------------------------------------------------------------
   AR1688 Call log function copy right information

   Copyright (c) 2006-2010. Tang, Li      <tangli@palmmicro.com>
                            Lin, Rongrong <woody@palmmicro.com>

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
#include "bank4.h"

#ifdef SYS_CHIP_PLUS
#define MAX_RECORD_NUM		20
#else
#define MAX_RECORD_NUM		10
#endif

#define RECORD_TYPE_NUM		3

#ifdef OEM_BT2008
extern UCHAR _iLastRecordTimer;
extern UCHAR _pLastRecord[MAX_USER_NUMBER_LEN];
#endif

UCHAR _pTotalRecords[RECORD_TYPE_NUM];
CALL_RECORD * _pp_record_heads[RECORD_TYPE_NUM];

void CallLogInit()
{
	UCHAR i;

	memset(_pTotalRecords, 0, RECORD_TYPE_NUM);
	for (i = 0; i < RECORD_TYPE_NUM; i ++)
	{
		_pp_record_heads[i] = NULL;
	}
}

#ifndef OEM_IP20
BOOLEAN FindPhoneName(PCHAR pSrcNumber, PCHAR pName)
{
	UCHAR i;
	USHORT sOffset;
	UCHAR pNumber[MAX_USER_NUMBER_LEN];

	for (i = 0; i < PHONEBOOK_ENTRY_NUM; i ++)
	{
		sOffset = (USHORT)(OPT_PHONE_BOOK + ((USHORT)i << PHONEBOOK_ENTRY_SHIFT));
		if (OptionsGetByte(sOffset))
		{
			OptionsGetString(pNumber, (USHORT)(sOffset + PHONEBOOK_DATA_NUMBER), MAX_USER_NUMBER_LEN);
			if (!strcmp(pNumber, pSrcNumber))
			{
				OptionsGetString(pName, (USHORT)(sOffset + PHONEBOOK_DATA_NAME), MAX_USER_NAME_LEN);
				return TRUE;
			}
		}
	}
	return FALSE;
}
#endif

void CallLogAdd(PCHAR pRealName, PCHAR pRealNumber, UCHAR iType)
{
	UCHAR iLen;
	UCHAR pTemp[MAX_USER_NUMBER_LEN];
	PCHAR pName;
	PCHAR pNumber;
	CALL_RECORD * p;
	CALL_RECORD * pCur;
	CALL_RECORD * pPrev;
	CALL_RECORD * pNext;

	OptionsGetString(pTemp, OPT_DIAL_PREFIX, MAX_USER_NUMBER_LEN);
	iLen = strlen(pTemp);
	pNumber = (iLen && !memcmp_str(pRealNumber, pTemp)) ? (PCHAR)(pRealNumber + iLen) : pRealNumber;
	pName = strcmp(pRealName, pRealNumber) ? pRealName : pNumber; 

#ifndef OEM_IP20
	if (FindPhoneName(pNumber, pTemp))
	{
		if (strlen(pTemp))
		{
			pName = pTemp;
		}
	}
#endif

#ifdef OEM_BT2008
	if (iType == CALL_LOG_TYPE_MISSED || iType == CALL_LOG_TYPE_RECVED)
	{
		strcpy(_pLastRecord, pNumber);
		_iLastRecordTimer = 0;
	}
#endif

	iLen = strlen(pNumber) + 1;
	p = (CALL_RECORD *)malloc(sizeof(CALL_RECORD) + iLen + strlen(pName));
	if (!p)	return;

	if (iType == CALL_LOG_TYPE_MISSED && Sys_iMissedCalls < 255)
	{
		Sys_iMissedCalls++;
	}

	p->lDatetime = Sys_lCurrentTime;
	strcpy(&p->iMem, pNumber);
	strcpy((PCHAR)(&p->iMem + iLen), pName);

	// add newest to head
	p->next = _pp_record_heads[iType];
	_pp_record_heads[iType] = p;

#ifndef OEM_IP20
	// remove repeat item
	pPrev = p;
	pCur = p->next;
	do
	{
		pNext = pCur->next;
		if (!strcmp(&p->iMem, &pCur->iMem))
		{
			pPrev->next = pNext;
			free(pCur);
			_pTotalRecords[iType] --;
			break;
		}
		else
		{
			pPrev = pCur;
		}
		pCur = pNext;
	} while (pNext);
#endif

	// limit item number
	if (_pTotalRecords[iType] == MAX_RECORD_NUM)
	{	// remove the oldest tail
		pPrev = pCur = p->next;
		do
		{
			pNext = pCur->next;
			if (!pNext)
			{
				pPrev->next = NULL;
				free(pCur);
				break;
			}
			else
			{
				pPrev = pCur;
			}
			pCur = pNext;
		} while (pNext);
	}
	else
	{
		_pTotalRecords[iType] ++;
	}
}

#ifdef OEM_BT2008
void CallLogTimer()
{
	UCHAR iType, iTimeOut;
	ULONG lCur;
	CALL_RECORD * pNext;
	CALL_RECORD * pCur;
	CALL_RECORD * pPrev;

	iTimeOut = OptionsGetByte(OPT_CALL_LOG_TIMEOUT);
	if (iTimeOut)
	{
		if (strlen(_pLastRecord))
		{
			_iLastRecordTimer ++;
			if (_iLastRecordTimer >= iTimeOut)
			{
				_pLastRecord[0] = 0;
				_iLastRecordTimer = 0;
			}
		}

		lCur = Sys_lCurrentTime - mul_8x8(iTimeOut, 60);
		for (iType = 0; iType < RECORD_TYPE_NUM; iType ++)
		{
			if (!_pp_record_heads[iType])	continue;
			pPrev = pCur = _pp_record_heads[iType];
			do
			{
				pNext = pCur->next;
				if (lCur >= pCur->lDatetime)
				{
					_pTotalRecords[iType] --;

					if (pCur == _pp_record_heads[iType])
					{	// make next as head
						_pp_record_heads[iType] = pNext;
					}
					else
					{	// link the chain
						pPrev->next = pNext;
					}
					free(pCur);
				}
				else
				{
					pPrev = pCur;
				}
				pCur = pNext;
			} while (pNext);
		}
	}
}
#endif

UCHAR CallLogGetNum(UCHAR iType)
{
	UCHAR iNum;
	CALL_RECORD * pCur;

	iNum = 0;
	pCur = _pp_record_heads[iType];
	while (pCur)
	{
		pCur = pCur->next;
		iNum ++;
	}

	return iNum;
}

void CallLogDeleteAll(UCHAR iType)
{
	CALL_RECORD * pCur;
	CALL_RECORD * pNext;

	pCur = _pp_record_heads[iType];
	while (pCur)
	{
		pNext = pCur->next;
		free(pCur);
		pCur = pNext;
	}
	_pp_record_heads[iType] = NULL;
	_pTotalRecords[iType]=0;
}

CALL_RECORD * CallLogRead(UCHAR iType, UCHAR iIndex)
{
	UCHAR iNum;
	CALL_RECORD * pCur;

	iNum = 0;
	pCur = _pp_record_heads[iType];
	while (pCur)
	{
		if (iNum == iIndex)	return pCur;
		pCur = pCur->next;
		iNum ++;
	}

	return NULL;
}


