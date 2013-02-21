/*-------------------------------------------------------------------------
   AR1688 Micrel KSZ8842 function copy right information

   Copyright (c) 2008-2010. Lin, Rongrong <woody@palmmicro.com>

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

#ifdef KSZ8842

#include "ksz8842.h"
#include "apps.h"
#include "core.h"
#include "ar168.h"
#include "tcpip.h"
#include "bank2.h"

#define REG_OFFSET	0x300

extern USHORT _sLayer2Qos;
extern BOOLEAN _bVLan;

UCHAR _iRegBank;
BOOLEAN _bReadLow;
BOOLEAN _bWriteLow;

void _set_reg_bank(UCHAR iBank)
{
	PCHAR p;

	if (iBank == _iRegBank)		return;

	p = (PCHAR)(BANK_BASE + REG_OFFSET + REG_BANK_SEL_OFFSET);
	*p = iBank;

	_iRegBank = iBank;
}

UCHAR _reg_read_byte(UCHAR iBank, UCHAR iOffset)
{
	PCHAR p;

	_set_reg_bank(iBank);

	p = (PCHAR)(BANK_BASE + REG_OFFSET + iOffset);
	return *p;
}

void _reg_write_byte(UCHAR iBank, UCHAR iOffset, UCHAR iVal)
{
	PCHAR p;

	_set_reg_bank(iBank);

	p = (PCHAR)(BANK_BASE + REG_OFFSET + iOffset);
	*p = iVal;
}

void _reg_or_byte(UCHAR iBank, UCHAR iOffset, UCHAR iOrVal)
{
	PCHAR p;

	_set_reg_bank(iBank);

	p = (PCHAR)(BANK_BASE + REG_OFFSET + iOffset);
	*p |= iOrVal;
}

void _reg_and_byte(UCHAR iBank, UCHAR iOffset, UCHAR iAndVal)
{
	PCHAR p;

	_set_reg_bank(iBank);

	p = (PCHAR)(BANK_BASE + REG_OFFSET + iOffset);
	*p &= iAndVal;
}

USHORT _reg_read_word(UCHAR iBank, UCHAR iOffset)
{
	PSHORT p;

	_set_reg_bank(iBank);

	p = (PSHORT)(BANK_BASE + REG_OFFSET + iOffset);
	return *p;
}

void _reg_write_word(UCHAR iBank, UCHAR iOffset, USHORT sVal)
{
	PSHORT p;

	_set_reg_bank(iBank);

	p = (PSHORT)(BANK_BASE + REG_OFFSET + iOffset);
	*p = sVal;
}

void _reg_or_word(UCHAR iBank, UCHAR iOffset, USHORT sOrVal)
{
	PSHORT p;

	_set_reg_bank(iBank);

	p = (PSHORT)(BANK_BASE + REG_OFFSET + iOffset);
	*p |= sOrVal;
}

void _reg_and_word(UCHAR iBank, UCHAR iOffset, USHORT sAndVal)
{
	PSHORT p;

	_set_reg_bank(iBank);

	p = (PSHORT)(BANK_BASE + REG_OFFSET + iOffset);
	*p &= sAndVal;
}

/*
void _ksz8842_write(PCHAR pBuf, USHORT sLen)
{
	USHORT s;
	PSHORT pDstLow;
	PSHORT pDstHigh;
	PSHORT pSrc;

	pSrc = (PSHORT)pBuf;
	pDstLow = (PSHORT)(BANK_BASE + REG_OFFSET + REG_DATA_OFFSET);
	pDstHigh = (PSHORT)(BANK_BASE + REG_OFFSET + REG_DATA_HI_OFFSET);

	for (s = 0; s < sLen; s += 2)
	{
		if (_bWriteLow)
		{
			*pDstLow = *pSrc;
			_bWriteLow = FALSE;
		}
		else
		{
			*pDstHigh = *pSrc;
			_bWriteLow = TRUE;
		}
		pSrc ++;
	}
}

void Ksz8842Write(PCHAR pBuf, USHORT sLen)
{
//	EnableCE1();
	_ksz8842_write(pBuf, sLen);
//	DisableCEx();
}

void _ksz8842_read(PCHAR pBuf, USHORT sLen)
{
	USHORT s;
	PSHORT pSrcLow;
	PSHORT pSrcHigh;
	PSHORT pDst;

	pDst = (PSHORT)pBuf;
	pSrcLow = (PSHORT)(BANK_BASE + REG_OFFSET + REG_DATA_OFFSET);
	pSrcHigh = (PSHORT)(BANK_BASE + REG_OFFSET + REG_DATA_HI_OFFSET);

//	if (sLen % 2)	sLen ++;

	for (s = 0; s < sLen; s += 2)
	{
		if (_bReadLow)
		{
			*pDst = *pSrcLow;
			_bReadLow = FALSE;
		}
		else
		{
			*pDst = *pSrcHigh;
			_bReadLow = TRUE;
		}
		pDst ++;
	}
}

void Ksz8842Read(PCHAR pBuf, USHORT sLen)
{
	EnableCE1();
	_ksz8842_read(pBuf, sLen);
	DisableCEx();
}
*/

void Ksz8842Send(USHORT sLen, UCHAR iType)
{
	UCHAR pHead[4];

#ifndef CALL_NONE
	if (iType == NET_SEND_PPPOE)
	{
		sLen += PPPOE_INFO;
	}
#endif

	EnableCE1();

	// wait TXQCR to clear
	while (_reg_read_byte(REG_TXQ_CMD_BANK, REG_TXQ_CMD_OFFSET));

	// read value from TXMIR to check if QMU TXQ has enough amount of memory for the ethernet packet data.
	// compare the read value with (sLen+4), if less than (sLen+4), Exit. TXMIR[12-0]
	if ((_reg_read_word(REG_TX_MEM_INFO_BANK, REG_TX_MEM_INFO_OFFSET) & MEM_AVAILABLE_MASK) < sLen + 4)	goto End;

	_set_reg_bank(REG_DATA_BANK);
//	DisableCEx();
	_bWriteLow = TRUE;

	USHORT2PCHAR(0, pHead);
	USHORT2PCHAR_L(sLen, (PCHAR)(pHead + 2));
	Ksz8842Write(pHead, 4);
	if (iType == NET_SEND_NORMAL)
	{
		Ksz8842Write(Adapter_pPacketBuf, sLen);
	}
#ifndef CALL_NONE
	else if (iType == NET_SEND_PPPOE)
	{
		Ksz8842Write(Adapter_pPacketBuf, EP_DATA);
		Ksz8842Write(PPPoE_pSendHead, PPPOE_INFO);
		Ksz8842Write((PCHAR)(Adapter_pPacketBuf + EP_DATA), sLen - EP_DATA - PPPOE_INFO);
	}
#endif

//	EnableCE1();
	// issue the ENQUEUE transmits command for the device to transmit the ethernet packet to the network. Exit. TXQCR
	_reg_write_byte(REG_TXQ_CMD_BANK, REG_TXQ_CMD_OFFSET, 0x01);	

End:
	DisableCEx();
}

void Ksz8842Run()
{
	USHORT sVal;
	UCHAR iNum;

	EnableCE1();

	// receive data

	// read value from ISR to check if RXIS receive interrupt is set, if not set, exit, ISR[13]
	sVal = _reg_read_word(REG_INT_STATUS_BANK, REG_INT_STATUS_OFFSET);
	if (!(sVal & 0x2000))	goto End;

	// acknowledge (clear) RXIS receive interrupt bit, ISR[13]
	_reg_write_word(REG_INT_STATUS_BANK, REG_INT_STATUS_OFFSET, 0x2000);

	iNum = 0;
	do
	{
		// read value from RXMIR to check if QMU RXQ still has more packet data to be read, if read value <= 0, exit, RXMIR[12-0]
		sVal = _reg_read_word(REG_RX_MEM_INFO_BANK, REG_RX_MEM_INFO_OFFSET) & MEM_AVAILABLE_MASK;
		if (!sVal)	break;

		_set_reg_bank(REG_DATA_BANK);
		DisableCEx();
		_bReadLow = TRUE;

		HandlePacket();
		EnableCE1();

		// reset QMU receive high water mark to 3Kbytes, QRFCR[12]
//		sVal = _reg_read_word(REG_RX_WATERMARK_BANK, REG_RX_WATERMARK_OFFSET);
//		_reg_write_word(REG_RX_WATERMARK_BANK, REG_RX_WATERMARK_OFFSET, sVal & (~RX_HIGH_WATERMARK_2KB));

		// issue the RELEASE frame command for the device to release this frame buffer memory space from QMU RXQ, RXQCR[0]
		_reg_or_byte(REG_RXQ_CMD_BANK, REG_RXQ_CMD_OFFSET, 0x01);
		// wait RXQCR to clear
		while (_reg_read_byte(REG_RXQ_CMD_BANK, REG_RXQ_CMD_OFFSET) & 0x01);

		// set QMU receive high water mark to 2Kbytes, QRFCR[12]
//		sVal = _reg_read_word(REG_RX_WATERMARK_BANK, REG_RX_WATERMARK_OFFSET);
//		_reg_write_word(REG_RX_WATERMARK_BANK, REG_RX_WATERMARK_OFFSET, sVal | RX_HIGH_WATERMARK_2KB);

		iNum ++;
	} while (iNum < ETHERNET_MAX_READ_PACKET);
End: 
	DisableCEx();
}

UCHAR ksz8842_read_reg_byte(UCHAR iBank, UCHAR iOffset)
{
	UCHAR iVal;

	EnableCE1();
	iVal = _reg_read_byte(iBank, iOffset);
	DisableCEx();

	return iVal;
}

void ksz8842_or_reg_byte(UCHAR iBank, UCHAR iOffset, UCHAR iOrVal)
{
	EnableCE1();
	_reg_or_byte(iBank, iOffset, iOrVal);
	DisableCEx();
}

void ksz8842_and_reg_byte(UCHAR iBank, UCHAR iOffset, UCHAR iAndVal)
{
	EnableCE1();
	_reg_and_byte(iBank, iOffset, iAndVal);
	DisableCEx();
}

void ksz8842_write_reg_word(UCHAR iBank, UCHAR iOffset, USHORT sVal)
{
	EnableCE1();
	_reg_write_word(iBank, iOffset, sVal);
	DisableCEx();
}

void ksz8842_or_reg_word(UCHAR iBank, UCHAR iOffset, USHORT sOrVal)
{
	EnableCE1();
	_reg_or_word(iBank, iOffset, sOrVal);
	DisableCEx();
}

void ksz8842_and_reg_word(UCHAR iBank, UCHAR iOffset, USHORT sAndVal)
{
	EnableCE1();
	_reg_and_word(iBank, iOffset, sAndVal);
	DisableCEx();
}

#endif

