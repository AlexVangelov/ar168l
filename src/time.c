/*-------------------------------------------------------------------------
   AR1688 Time function copy right information

   Copyright (c) 2006-2012. Lin, Rongrong <woody@palmmicro.com>
                            Tang, Li      <tangli@palmmicro.com>

   Alex Gradinar <Alex.Gradinar at cartel.md> added Europe Daylight Saving time
   Sebastien RAILLARD <sebastien.raillard at gmail.com> added French support
   Alfredo Sorbello <sorbello at antek.it> added Italian support
   Geisom <geisom at canall.com.br> added Brasil Portuguese support

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


   Add Australian Time zones. Fred Villella Feb 2011

	In Australia, Daylight Saving Time is observed in New South Wales, Victoria, South Australia,
	Tasmania, and the Australian Capital Territory.
	Daylight Saving Time begins at 2am (AEST) on the first Sunday in October and ends at 2am (AEST)
	(which is 3am Australian Eastern Daylight Time) on the first Sunday in April.

	Where Daylight Saving Time is observed:
	NSW, ACT,Vic and Tas move from AEST to Australian Eastern Daylight Time (AEDT),
	and clocks are advanced to UTC +11.

	SA and the NSW town of Broken Hill move from ACST to Australian Central Daylight Time (ACDT),
	and clocks are advanced to UTC +10 ?

	Daylight Saving Time is not observed in Queensland, the Northern Territory or Western Australia.

-------------------------------------------------------------------------*/

#include "version.h"
#include "type.h"
#include "ar168.h"
#include "core.h"
#include "apps.h"

#include "bank1.h"

// swisstime.ethz.ch
// time.windows.com
// pool.ntp.org
// au.pool.ntp.org

#define MONTHS_PER_YEAR			12
#define DAYS_PER_WEEK			7
#define DAYS_PER_YEAR			365
#define DAYS_PER_LEAP_YEAR		366
#define MINUTES_PER_HOUR		60
#define	SECONDS_PER_MINUTE		60
#define	SECONDS_PER_HOUR		3600
#define SECONDS_PER_DAY			86400
#define SECONDS_PER_YEAR		(DAYS_PER_YEAR * SECONDS_PER_DAY)
#define SECONDS_PER_LEAP_YEAR	(DAYS_PER_LEAP_YEAR * SECONDS_PER_DAY)

#define HOURS_PER_DAY			24


#ifdef RES_BR // Brasil
#include "res\\br\\time.h"
#elif defined RES_CN && defined DISPLAY_DOT
#elif defined RES_ES // Español
#include "res\\es\\time.h"
#elif defined RES_FR // French
#include "res\\fr\\time.h"
#elif defined RES_IT // Italy
#include "res\\it\\time.h"
#elif defined RES_KR && defined DISPLAY_DOT // Korean
#include "res\\kr\\time.h"
#elif defined RES_RO // Romanian
#include "res\\ro\\time.h"
#elif defined RES_RU && defined DISPLAY_DOT // Russian
#include "res\\ru\\time.h"
#elif defined RES_TR // Turkish
#ifdef OEM_IP20
#include "res\\ip20\\tr\\time.h"
#else
#include "res\\tr\\time.h"
#endif
#elif defined RES_VN // Vietnam
#include "res\\vn\\time.h"
#else // Default = REG_US
#include "res\\us\\time.h"
#endif

const UCHAR _cMonthDay[MONTHS_PER_YEAR] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
const UCHAR _cMonthDayLeap[MONTHS_PER_YEAR] = {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

#define SECONDS_PER_Q_HOUR	900
#define TIME_ZONE_MAX	76	
#define TIME_OFFSET_SECONDS		43200	// 48 * 900 = 43200

const UCHAR _cTimeZoneTable[TIME_ZONE_MAX] = {
0,  4,  8,  12, 16, 20, 20, 20, 24, 24, 24, 24, 28, 28, 28, 30, 32, 32, 32, 34, 36, 36, 36, 40, 
44, 44, 48, 48, 52, 52, 52, 52, 52, 56, 56, 56, 56, 56, 56, 60, 60, 60, 60, 62, 64, 64, 66, 68, 
68, 70, 71, 72, 72, 72, 74, 76, 76, 80, 80, 80, 80, 80, 84, 84, 84, 86, 86, 88, 88, 88, 88, 88, 
92, 96, 96, 100};

ULONG _TimeZoneOffset()
{
	ULONG lTime;

	lTime = mul_16x8(SECONDS_PER_Q_HOUR, _cTimeZoneTable[Sys_iTimeZone]);
	lTime -= TIME_OFFSET_SECONDS;
	return lTime;
}

BOOLEAN _DayLightOffset(UCHAR iWeek, UCHAR iMonth, UCHAR iDay, UCHAR iHour)
{
	UCHAR iChangeHour;

	if (Sys_iTimeZone == 4 || Sys_iTimeZone == 5 || Sys_iTimeZone == 9 || Sys_iTimeZone == 13)
	{	//	4 time zones in continental USA
		//	Rule	US	2007	max	-	Mar	Sun>=8	2:00	1:00	D
		//	Rule	US	2007	max	-	Nov	Sun>=1	2:00	0	S
		if (iMonth < 3)		return FALSE;
		else if (iMonth == 3)
		{
			if (iDay < 8)	return FALSE;
			else if (iDay < 15)
			{
				if (!iWeek)	// Sun
				{
					if (iHour < 2)	return FALSE;
					return TRUE;
				}
				else
				{
					if (iDay - 8 < iWeek)	return FALSE;
					return TRUE;
				}
			}
			else	return TRUE;
		}
		else if (iMonth < 11)	return TRUE;
		else if (iMonth == 11)	
		{
			if (iDay < 8)
			{
				if (!iWeek)	// Sun
				{
					if (iHour < 2) return TRUE;
					return FALSE;
				}
				else
				{
					if (iDay - 1 < iWeek)	return TRUE;
					else	return FALSE;
				}
			}
			else	return FALSE;
		}
		else	return FALSE;
	}
	else if (Sys_iTimeZone == 26 || Sys_iTimeZone == 28 || Sys_iTimeZone == 29 || Sys_iTimeZone == 30 || Sys_iTimeZone == 31 || Sys_iTimeZone == 33 || Sys_iTimeZone == 35 || Sys_iTimeZone == 37 || Sys_iTimeZone == 41)
	{	//	EU	 	on	Mar lastSun	1:00UTC	1:00
		//	EU	 	off	Oct lastSun	1:00UTC	0
		//	Zone 41
		//  Rule	Russia	1993	max	-	Mar	lastSun	 2:00s	1:00	S
		//	Rule	Russia	1996	max	-	Oct	lastSun	 2:00s	0	-
		if (Sys_iTimeZone == 26)		iChangeHour = 0;
		else if (Sys_iTimeZone <= 31)	iChangeHour = 1;
		else					iChangeHour = 2;

		if (iMonth < 3)		return FALSE;
		else if (iMonth == 3)
		{
			if (iDay < 25)	return FALSE;
			else
			{
				if (!iWeek)	// Sun
				{
					if (iHour < iChangeHour)	return FALSE;
					else	return TRUE;
				}
				else
				{
					if (iDay - 25 < iWeek)	return FALSE;
					else	return TRUE;
				}
			}
		}
		else if (iMonth < 10)	return TRUE;
		else if (iMonth == 10)
		{
			if (iDay < 25)	return TRUE;
			else
			{
				if (!iWeek)	// Sun
				{
					if (iHour < iChangeHour)	return TRUE;
					else	return FALSE;
				}
				else
				{
					if (iDay - 25 < iWeek)	return TRUE;
					else 	return FALSE;
				}
			}
		}
		else	return FALSE;
	}
	else if (Sys_iTimeZone == 73)	// New Zealand
	{	//	Rule	NZ	2007	max	-	Sep	lastSun	2:00s	1:00	D
		//	Rule	NZ	2008	max	-	Apr	Sun>=1	2:00s	0	S
		if (iMonth < 4)		return TRUE;
		else if (iMonth == 4)	
		{
			if (iDay < 8)
			{
				if (!iWeek)	// Sun
				{
					if (iHour < 2) return TRUE;
					return FALSE;
				}
				else
				{
					if (iDay - 1 < iWeek)	return TRUE;
					else	return FALSE;
				}
			}
		}
		else if (iMonth < 9)	return FALSE;
		else if (iMonth == 9)
		{
			if (iDay < 24)	return FALSE;
			else
			{
				if (!iWeek)	// Sun
				{
					if (iHour < 2)	return FALSE;
					else	return TRUE;
				}
				else
				{
					if (iDay - 24 < iWeek)	return FALSE;
					else	return TRUE;
				}
			}
		}
		else return TRUE;
	}

//fv  Aust zones: Melb=71, SA=65, Tas=70 fv
	else if (Sys_iTimeZone == 65 || Sys_iTimeZone == 70 || Sys_iTimeZone == 71)	// Australia
	{	//		 Oct	First Sun	at 2:00am DLS start
		//		 Apr	First Sun	at 2:00am DLS end
		if (iMonth < 4)	return TRUE;
		else if (iMonth == 4)
		{
			if (iDay < 8)
			{
				if (iWeek==0)	//Sunday
				{
					if (iHour < 2) return TRUE;
					else return FALSE;
				}
				else
				{
					if (iDay - 1 < iWeek)	{UdpDebugString("time1"); return TRUE;}
					else	{UdpDebugString("time2"); return FALSE;}
				}
			}
		}
		else if (iMonth < 10)	return FALSE;
		else if (iMonth == 10)	//October, switch over month
		{
			if (iDay > 7)	return TRUE; //first Sunday of the month (always between 1st & 7th Oct)
			else
			{
				if (iWeek==0)	// Sun=0, Mon=1, Thur=4 etc
				{		//this happens on a Sunday (iWeek=0)
					if (iHour < 2)	return FALSE;
					else	return TRUE;	//sunday after 2am DLS starts
				}

				else
				{		// any other day of week
					if (iDay - 1 < iWeek)
						return FALSE;
							//
					else
						return TRUE;
				}
			}
		}
	}

	return FALSE;
}

BOOLEAN _IsLeapYear(USHORT sYear)
{
	if (!mod_16x8(sYear, 100))
	{
		sYear = div_16x8(sYear, 100);
		if (!mod_16x8(sYear, 4))
		{
			return TRUE;
		}
	}
	else
	{
		if (!mod_16x8(sYear, 4))
		{
			return TRUE;
		}
	}

	return FALSE;
}

void ConvertTime(ULONG lTime, TIME_T * ptm)
{
	USHORT sTotalDays, sVal, sYear;
	UCHAR iWeek, iMonth, iDay, iHour, iMinute, iSecond;
	const PCHAR pCur;
	ULONG lVal;

	lTime += _TimeZoneOffset();

	sTotalDays = 0;
	sYear = 1900;
	iMonth = 1;
	iDay = 1;

	// check year
	do
	{
		if (_IsLeapYear(sYear))
		{
			lVal = SECONDS_PER_LEAP_YEAR;
			sVal = DAYS_PER_LEAP_YEAR;
		}
		else
		{
			lVal = SECONDS_PER_YEAR;
			sVal = DAYS_PER_YEAR;
		}
		if (lTime >= lVal)
		{
			lTime -= lVal;
			sTotalDays += sVal;
			sYear ++;
		}
		else
		{
			break;
		}
	} while (1);

	// check month
	pCur = _IsLeapYear(sYear) ? _cMonthDayLeap : _cMonthDay;
	do
	{
		sVal = pCur[iMonth-1];		// days in month
		lVal = mul_32x8(SECONDS_PER_DAY, sVal);
		if (lTime >= lVal)
		{
			lTime -= lVal;
			sTotalDays += sVal;
			iMonth ++;
		}
		else
		{
			break;
		}
	} while (1);

	// check day, avoid div to 16 or 32 bit number
	do
	{
		if (lTime >= SECONDS_PER_DAY) 
		{
			lTime -= SECONDS_PER_DAY;
			iDay ++;
			sTotalDays ++;
		}
		else
		{
			break;
		}
	} while (1);

	iWeek = mod_16x8(sTotalDays + 1, DAYS_PER_WEEK);
	// check hour
	iHour = 0;
	do
	{
		if (lTime >= SECONDS_PER_HOUR)
		{
			lTime -= SECONDS_PER_HOUR;
			iHour ++;
		}
		else
		{
			break;
		}
	} while (1);
	sVal = (USHORT)lTime;
	iMinute = div_16x8(sVal, SECONDS_PER_MINUTE);
	iSecond = mod_16x8(sVal, SECONDS_PER_MINUTE);

	// Automatically adjust daylight
	if (Sys_bDayLight)
	{
		if (_DayLightOffset(iWeek, iMonth, iDay, iHour))
		{
			iHour ++;
			if (iHour == HOURS_PER_DAY)
			{
				iHour = 0;
				if (iDay == pCur[iMonth-1])
				{
					iMonth ++;
					iDay = 1;
				}
				else
				{
					iDay ++;
				}

				if (iWeek == DAYS_PER_WEEK - 1)
				{
					iWeek = 0;
					UdpDebugVal(iWeek, 10);
				}
				else
				{
					iWeek ++;
				}
			}
		}
	}

	ptm->iSec = iSecond;
	ptm->iMin = iMinute;
	ptm->iHour = iHour;
	ptm->iMday = iDay;
	ptm->iMon = iMonth;
	ptm->sYear = sYear;
	ptm->iWday = iWeek;
}

const UCHAR _cHourMin[] = " 00:00";

void _BulidHourMin(PCHAR pBuf, TIME_T * ptm)
{
	strcpy(pBuf, _cHourMin);
	Fill2Digits((PCHAR)(pBuf + 1), ptm->iHour);
	Fill2Digits((PCHAR)(pBuf + 4), ptm->iMin);
}

// lTime is the seconds passed since 0h Jan 1, 1900
// this calculation is only correct for lTime elapsed since the very beginning moment of a year, that
// is the 00:00:00, Jan 1
void DisplayTime(TIME_T * ptm, UCHAR iLine)
{
	UCHAR pBuf[DISPLAY_MAX_CHAR+1];

#if defined RES_CN && defined DISPLAY_DOT
	Fill2Digits(pBuf, ptm->iMon);
	memcpy((PCHAR)(pBuf + 2), "ÔÂ", 2);
	Fill2Digits((PCHAR)(pBuf + 4), ptm->iMday);
	memcpy((PCHAR)(pBuf + 6), "ÈÕ", 2);
	_BulidHourMin((PCHAR)(pBuf + 8), ptm);
#elif defined RES_FR // French
	memcpy(pBuf, g_cWeekDay[ptm->iWday], 3);
	pBuf[3] = '-';
	Fill2Digits((PCHAR)(pBuf + 4), ptm->iMday);
	pBuf[6] = '-';
	memcpy((PCHAR)(pBuf + 7), g_cMonth[ptm->iMon - 1], 3);
	_BulidHourMin((PCHAR)(pBuf + 10), ptm);
#elif defined RES_IT || defined RES_RO
	memcpy(pBuf, g_cWeekDay[ptm->iWday], 3);
	pBuf[3] = ' ';
	Fill2Digits((PCHAR)(pBuf + 4), ptm->iMday);
	pBuf[6] = ' ';
	memcpy((PCHAR)(pBuf + 7), g_cMonth[ptm->iMon - 1], 3);
	_BulidHourMin((PCHAR)(pBuf + 10), ptm);
#elif defined RES_RU && defined DISPLAY_DOT // Russian
	memcpy(pBuf, g_cWeekDay[ptm->iWday], 2);
	pBuf[2] = ',';
	pBuf[3] = ' ';
	Fill2Digits((PCHAR)(pBuf + 4), ptm->iMday);
	pBuf[6] = ' ';
	memcpy((PCHAR)(pBuf + 7), g_cMonth[ptm->iMon - 1], 3);
	_BulidHourMin((PCHAR)(pBuf + 10), ptm);
#elif defined RES_TR // Türkiye
	memcpy(pBuf, g_cWeekDay[ptm->iWday], 3);
	pBuf[3] = ' ';
	Fill2Digits((PCHAR)(pBuf + 4), ptm->iMday);
	pBuf[6] = ' ';
	memcpy((PCHAR)(pBuf + 7), g_cMonth[ptm->iMon - 1], 3);
#ifdef OEM_IP20
	memset((PCHAR)(pBuf + 10), ' ', 4);
	_BulidHourMin((PCHAR)(pBuf + 14), ptm);
#else
	_BulidHourMin((PCHAR)(pBuf + 10), ptm);
#endif
#else // Default = REG_US
	memcpy(pBuf, g_cWeekDay[ptm->iWday], 3);
	pBuf[3] = ' ';
	memcpy((PCHAR)(pBuf + 4), g_cMonth[ptm->iMon - 1], 3);
	pBuf[7] = ' ';
	Fill2Digits((PCHAR)(pBuf + 8), ptm->iMday);
	_BulidHourMin((PCHAR)(pBuf + 10), ptm);
#endif

	LcdDisplay(pBuf, iLine);
#ifdef SERIAL_UI
	// for UART test only
	SerialSend("LOOP", 4);
	SerialSendString(pBuf);
#endif
}

const UCHAR _cTimeStart[] = "00:00:00";

// lTime is the seconds passed since 0h Jan 1, 1900
void DisplayCallTime(CALL_TIME * pct, UCHAR iLine)
{
	UCHAR pBuf[DISPLAY_MAX_CHAR];
	UCHAR iHour;
	PCHAR pCur;

	pCur = pBuf;
	iHour = pct->iHour;
	if (iHour >= 200)
	{
		pBuf[0] = '2';
		pCur ++;
		iHour -= 200;
	}
	else if (iHour >= 100)
	{
		pBuf[0] = '1';
		pCur ++;
		iHour -= 100;
	}
	strcpy(pCur, _cTimeStart);
	Fill2Digits(pCur, iHour);
	Fill2Digits((PCHAR)(pCur + 3), pct->iMin);
	Fill2Digits((PCHAR)(pCur + 6), pct->iSec);

	LcdDisplay(pBuf, iLine);
}

#ifdef CALL_IAX2
ULONG CalcIax2Time(PCHAR pParam)
{
	USHORT s, sYear;
	UCHAR i, iMonth, iDay, iHour, iMinute, iSecond;
	ULONG lVal;
	const PCHAR pCur;

	sYear = ((pParam[0] & 0xfe) >> 1) + 2000;
	iMonth = ((pParam[0] & 1) << 3) + ((pParam[1] & 0xe0) >> 5);
	iDay = pParam[1] & 0x1f;
	iHour = (pParam[2] & 0xf8) >> 3;
	iMinute = ((pParam[2] & 0x07) << 3) + ((pParam[3] & 0xe0) >> 5);
	iSecond = (pParam[3] & 0x1f) << 1;

	lVal = 0;
	for (s = 1900; s < sYear; s ++)
	{
		lVal += _IsLeapYear(s) ? SECONDS_PER_LEAP_YEAR : SECONDS_PER_YEAR;
	}

	pCur = _IsLeapYear(sYear) ? _cMonthDayLeap : _cMonthDay;
	for (i = 0; i < iMonth-1; i ++)
	{
		lVal += mul_32x8(SECONDS_PER_DAY, pCur[i]);
	}
	if (iDay > 1)
	{
		lVal += mul_32x8(SECONDS_PER_DAY, iDay-1);
	}
	if (iHour)
	{
		lVal += mul_16x8(SECONDS_PER_HOUR, iHour);
	}
	if (iMinute)
	{
		lVal += mul_8x8(SECONDS_PER_MINUTE, iMinute);
	}
	lVal += iSecond;
	lVal -= _TimeZoneOffset();

	return lVal;
}
#endif

