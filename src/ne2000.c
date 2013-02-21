/*-------------------------------------------------------------------------
   AR1688 Realtek RTL8019AS function copy right information

   Copyright (c) 2006-2013. Lin, Rongrong <woody@palmmicro.com>

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

#ifdef RTL8019AS

#include "sfr.h"
#include "ne2000.h"
#include "core.h"
#include "ar168.h"
#include "apps.h"
#include "tcpip.h"
#include "bank2.h"

//#define DEBUG_OVERFLOW

UCHAR _iCurrentPacket;

BOOLEAN _bAdapterOK;

extern USHORT _sLayer2Qos;
extern BOOLEAN _bVLan;
extern USHORT _sCurPacketAddr;

BOOLEAN _CardWaitStatus(UCHAR iStatus)
{
	PCHAR p;
	USHORT sCount;

	sCount = 0xffff;
	p = (PCHAR)BANK_BASE;
	while (sCount)
	{
		if (p[NIC_INTR_STATUS] & iStatus)
		{
			return TRUE;
		}
		else
		{
			sCount --;
		}
	}
	return FALSE;
}

BOOLEAN Ne2000Read(USHORT sAddr, PCHAR pBuf, USHORT sLen)
{
	UCHAR iDummy;
	PCHAR p;
	BOOLEAN bRet;

	p = (PCHAR)BANK_BASE;

	EnableCE1();
    // Read the Command Register, to make sure it is ready for a write
    iDummy = p[NIC_COMMAND];

    // Set iCount and destination address)
	p[NIC_COMMAND] = CR_PAGE0;
	USHORT2PCHAR_L(sAddr, (PCHAR)(BANK_BASE + NIC_RMT_ADDR_LSB));
//	p[NIC_RMT_ADDR_LSB] = (UCHAR)sAddr;
//	p[NIC_RMT_ADDR_MSB] = (UCHAR)(sAddr >> 8);
	USHORT2PCHAR_L(sLen, (PCHAR)(BANK_BASE + NIC_RMT_COUNT_LSB));
//	p[NIC_RMT_COUNT_LSB] = (UCHAR)sLen;
//	p[NIC_RMT_COUNT_MSB] = (UCHAR)(sLen >> 8);

    // Set direction (Read)
    p[NIC_COMMAND] = CR_START + CR_PAGE0 + CR_DMA_READ;
	fixed_input(pBuf, sLen);

    // Wait for DMA to complete
    bRet = _CardWaitStatus(ISR_DMA_DONE);
	DisableCEx();
	return bRet;
}

UCHAR _CardGetCurrent()
{
	UCHAR iCurrent;
	PCHAR p;

	p = (PCHAR)BANK_BASE;

	EnableCE1();
   	p[NIC_COMMAND] = CR_START + CR_NO_DMA + CR_PAGE1;	// Have to go to page 1 to read NIC_CURRENT register
	iCurrent = p[NIC_CURRENT];
	p[NIC_COMMAND] =  CR_START + CR_NO_DMA + CR_PAGE0;
	DisableCEx();

	return iCurrent;
}

void _CardSetBoundary(UCHAR iCur)
{
	PCHAR p;

	p = (PCHAR)BANK_BASE;

	// Sets the value of the "boundary" NIC register to one behind _iCurrentPacket, 
	// to prevent packets from being received on top of un-indicated ones.
	// Have to be careful with "one behind NicNextPacket" when NicNextPacket is the first buffer in receive area.
	EnableCE1();
	p[NIC_BOUNDARY] = (iCur == NE2000_NIC_PAGE_START) ? (NE2000_NIC_PAGE_STOP - 1) : (iCur - 1);
	DisableCEx();
}

#ifdef DEBUG_OVERFLOW
void _CardCheckInterrupt()
{
	PCHAR p;
	UCHAR iStatus;

	p = (PCHAR)BANK_BASE;

	EnableCE1();
	iStatus = p[NIC_INTR_STATUS];
	if (iStatus != ISR_EMPTY)
	{	// Acknowledge the interrupt
		p[NIC_INTR_STATUS] = iStatus;
		DisableCEx();
		if (iStatus & IMR_OVERFLOW)
		{
			UdpDebugString("NE2000 overflow");
		}
		if (iStatus & IMR_RCV_ERR)
		{
			UdpDebugString("NE2000 receive error");
		}
		if (iStatus & IMR_COUNTER)
		{
			UdpDebugString("NE2000 counter");
		}
	}
	else
	{
		DisableCEx();
	}
}
#endif

void Ne2000Init()
{
	PCHAR p;

	p = (PCHAR)BANK_BASE;

	EnableCE1();
    // Check that Command register reflects this last command
    if (!(p[NIC_COMMAND] & CR_STOP))
    {
		goto Error;
    }

    p[NIC_RMT_COUNT_LSB] = 55;				// Do initialization errata
    p[NIC_COMMAND] = CR_DMA_READ + CR_START;	// Setup for a read
    p[NIC_INTR_MASK] = 0x0;					// Mask Interrupts
    p[NIC_DATA_CONFIG] = DCR_FIFO_8_BYTE + DCR_NORMAL + DCR_BYTE_WIDE;    // Setup the Adapter for reading ram
    p[NIC_XMIT_CONFIG] = TCR_NO_LOOPBACK;	    // Clear transmit configuration.
    p[NIC_RCV_CONFIG] = 0;				    // Clear receive configuration.
    p[NIC_INTR_STATUS] = 0xff;			    // Clear any interrupts
    p[NIC_COMMAND] = CR_NO_DMA + CR_STOP;	    // Stop the chip
    p[NIC_RMT_COUNT_LSB] = 0;					// Clear any DMA values
    p[NIC_RMT_COUNT_MSB] = 0;				    // Clear any DMA values

    // Wait for the reset to complete.
	if (!_CardWaitStatus(ISR_RESET))
	{
		goto Error;
	}

    p[NIC_XMIT_CONFIG] = TCR_LOOPBACK;	    // Put card in loopback mode
    p[NIC_COMMAND] = CR_NO_DMA + CR_START;    // Start the chip.
    p[NIC_COMMAND] = CR_STOP + CR_NO_DMA;	    // Stop the chip

    // Setup the card based on the initialization information
    p[NIC_COMMAND] = CR_STOP + CR_NO_DMA + CR_PAGE0;	    // Write to and read from CR to make sure it is there.
    if ((p[NIC_COMMAND] & (CR_STOP + CR_NO_DMA + CR_PAGE0)) != (CR_STOP + CR_NO_DMA + CR_PAGE0))
    {
		goto Error;
    }

    // Set up the registers in the correct sequence, as defined by the 8390 specification.
    p[NIC_DATA_CONFIG] = DCR_BYTE_WIDE + DCR_NORMAL + DCR_FIFO_8_BYTE;
    p[NIC_RMT_COUNT_MSB] = 0;
    p[NIC_RMT_COUNT_LSB] = 0;
    p[NIC_RCV_CONFIG] = RCR_REJECT_ERR + RCR_BROADCAST;
    p[NIC_XMIT_CONFIG] = TCR_LOOPBACK;
    p[NIC_BOUNDARY] = NE2000_NIC_PAGE_START;
    p[NIC_PAGE_START] = NE2000_NIC_PAGE_START;
    p[NIC_PAGE_STOP] = NE2000_NIC_PAGE_STOP;

    p[NIC_INTR_STATUS] = 0xff;	    
#ifdef DEBUG_OVERFLOW
	p[NIC_INTR_MASK] = IMR_OVERFLOW + IMR_RCV_ERR + IMR_COUNTER;   // Enable overflow interrupt.
#else
    p[NIC_INTR_MASK] = 0;
#endif

/*  93c46 
	// Setup to read the ethernet address
    p[NIC_RMT_COUNT_LSB] = 12;
    p[NIC_RMT_COUNT_MSB] = 0;
    p[NIC_RMT_ADDR_LSB] = 0;
    p[NIC_RMT_ADDR_MSB] = 0;
    p[NIC_COMMAND] = CR_START | CR_DMA_READ;

    // Read in the station address. (We have to read words -- 2 * 6 -- bytes)
    for (i = 0; i < HW_ALEN; i ++)
    {
        Sys_pMacAddress[i] = p[NIC_RACK_NIC];
        Sys_pMacAddress[i] = p[NIC_RACK_NIC];
    }
*/

    // Move to page 1 to write the station address
	p[NIC_COMMAND] = CR_STOP + CR_NO_DMA + CR_PAGE1;
	memcpy((PCHAR)(BANK_BASE+NIC_PHYS_ADDR), Sys_pMacAddress, HW_ALEN);		// memcpy MUST be in common bank

    // Write out the current receive buffer to receive into
    _iCurrentPacket = NE2000_NIC_PAGE_START + 1;
    p[NIC_CURRENT] = _iCurrentPacket;

    // move back to page 0 and start the card...
    p[NIC_COMMAND] = CR_STOP + CR_NO_DMA + CR_PAGE0;
    p[NIC_COMMAND] = CR_START + CR_NO_DMA + CR_PAGE0;

    // ... but it is still in loopback mode.
    // Start up the adapter.
    p[NIC_XMIT_CONFIG] = TCR_NO_LOOPBACK;

	DisableCEx();
	_bAdapterOK = TRUE;

    return;

Error:
	DisableCEx();
	_bAdapterOK = FALSE;
	LcdDisplay("Ne2000 error", 1);
}

#ifdef CALL_NONE

// Ne2000 functions
void Ne2000InitStep1()
{
	PCHAR p;

	p = (PCHAR)BANK_BASE;

#ifndef SYS_CHIP_PLUS
	// GPIO G3 as ethernet RESET
	rGPIO_G_Enable = (rGPIO_G_Enable & 0x7f) | 0x08;	// 01111111, 00001000
	rGPIO_G &= 0xf7;			// 11110111

	rGPIO_G |= 0x08;			// 00001000
	Delay(5);
	rGPIO_G &= 0xf7;			// 11110111
#elif defined VER_AR168MS || defined VER_AR168W
	// GPIO A2 as ethernet RESET
	rGPIO_A &= 0xfb;			// 11111011

	rGPIO_A |= 0x04;			// 00000100
	Delay(5);
	rGPIO_A &= 0xfb;			// 11111011
#elif defined VER_AR168M
	// GPIO G0 as ethernet RESET
	rGPIO_G_Enable |= 0x01;

	rGPIO_G &= 0xfe;

	rGPIO_G |= 0x01;
	Delay(5);
	rGPIO_G &= 0xfe;
#endif

#ifdef SYS_PROGRAMMER
	// set GPIO D4 (UART RX) as input
	rGPIO_D_Input_Enable |= 0x10;
	rGPIO_D &= 0xef;
#else
	// set GPIO C1 as input, IRQ
	rGPIO_C_Enable = (rGPIO_C_Enable & 0xfd) | 0x20;		// 11111101, 00100000
	rGPIO_C &= 0xfd;
#endif

	EnableCE1();

	// Initializes the card into a running state.
    p[NIC_INTR_MASK] = 0;						// Turn off interrupts first.
    p[NIC_COMMAND] = CR_STOP + CR_NO_DMA;		// Stop the card.
    p[NIC_DATA_CONFIG] = DCR_AUTO_INIT + DCR_FIFO_8_BYTE;	    // Initialize the Data Configuration register.
    p[NIC_XMIT_START] = 0xa0;					// Set Xmit start location
    p[NIC_XMIT_CONFIG] = TCR_NO_LOOPBACK;		// Set Xmit configuration
    p[NIC_RCV_CONFIG] = RCR_MONITOR;			// Set Receive configuration
    p[NIC_PAGE_START] = 0x4;					// Set Receive start
    p[NIC_PAGE_STOP] = 0xFF;					// Set Receive end
    p[NIC_BOUNDARY] = 0x4;					// Set Receive boundary
    p[NIC_XMIT_COUNT_LSB] = 0x3C;				// Set Xmit bytes
    p[NIC_XMIT_COUNT_MSB] = 0x0;
	DisableCEx();
}

void Ne2000InitStep2()
{
	PCHAR p;

	p = (PCHAR)BANK_BASE;

	EnableCE1();
    p[NIC_INTR_STATUS] = 0xff;				// Ack all interrupts that we might have produced
    p[NIC_COMMAND] = CR_PAGE1 + CR_STOP;		// Change to page 1
    p[NIC_CURRENT] = 0x4;						// Set current
    p[NIC_COMMAND] = CR_PAGE0 + CR_STOP;		// Back to page 0
	DisableCEx();
}
#else

void Ne2000MiniRun()
{
	UCHAR iCurrent, iNext;

#ifdef DEBUG_OVERFLOW
	_CardCheckInterrupt();
#endif

	if (!Ne2000Read(_sCurPacketAddr + HEADER_NEXT_PACKET, &iNext, 1))	return;
	while (1)
	{
		iCurrent = _CardGetCurrent();
		if (iCurrent != iNext)
		{
			iNext = ReadPacket(iNext, TRUE);
			if (!iNext)
			{
				break;
			}
		}
		else
		{
			break;
		}
	}
}

#endif

void Ne2000Run()
{
	UCHAR iCurrent, iNum;

	if (!_bAdapterOK)	return;

#ifdef DEBUG_OVERFLOW
	_CardCheckInterrupt();
#endif

	iNum = 0;
	do
	{
		iCurrent = _CardGetCurrent();
		if (iCurrent != _iCurrentPacket)
		{
			_iCurrentPacket = ReadPacket(_iCurrentPacket, FALSE);
			if (!_iCurrentPacket)	        // Read packet
			{
				_iCurrentPacket = iCurrent;
			}
			_CardSetBoundary(_iCurrentPacket);
		}
		else
		{
			break;
		}
		iNum ++;
	} while (iNum < ETHERNET_MAX_READ_PACKET);
}

BOOLEAN Ne2000Write(USHORT sAddr, PCHAR pBuf, USHORT sLen)
{
	UCHAR iDummy;
	PCHAR p;
	BOOLEAN bRet;

//	if (!_bAdapterOK)	return;

	p = (PCHAR)BANK_BASE;

	EnableCE1();

    // Read the Command Register, to make sure it is ready for a write
    iDummy = p[NIC_COMMAND];
/*
    // Do Write errata as described on pages 1-143 and 1-144 of the 1992 LAN databook
	p[NIC_COMMAND] = CR_PAGE0;
	USHORT2PCHAR_L(sAddr, (PCHAR)(BANK_BASE+NIC_RMT_ADDR_LSB));
    p[NIC_RMT_COUNT_LSB] = 0x2;
    p[NIC_RMT_COUNT_MSB] = 0x0;

    // Set direction (Read)
    p[NIC_COMMAND] = CR_START + CR_PAGE0 + CR_DMA_READ;

    // Read from port
	iDummy = p[NIC_RACK_NIC];
    iDummy = p[NIC_RACK_NIC];

    // Wait for addr to change
	while ((p[NIC_CRDA_LSB] == (UCHAR)sAddr) && (p[NIC_CRDA_MSB] == (UCHAR)(sAddr >> 8)));
*/
    // Set sCount and destination address
	p[NIC_COMMAND] = CR_PAGE0;
	USHORT2PCHAR_L(sAddr, (PCHAR)(BANK_BASE + NIC_RMT_ADDR_LSB));
//	p[NIC_RMT_ADDR_LSB] = (UCHAR)sAddr;
//	p[NIC_RMT_ADDR_MSB] = (UCHAR)(sAddr >> 8);
	USHORT2PCHAR_L(sLen, (PCHAR)(BANK_BASE + NIC_RMT_COUNT_LSB));
//	p[NIC_RMT_COUNT_LSB] = (UCHAR)sLen;
//	p[NIC_RMT_COUNT_MSB] = (UCHAR)(sLen >> 8);

    // Set direction (Write)
    p[NIC_COMMAND] = CR_START + CR_PAGE0 + CR_DMA_WRITE;
	fixed_output(pBuf, sLen);

    // Wait for DMA to complete
	bRet = _CardWaitStatus(ISR_DMA_DONE);
	DisableCEx();
	return bRet;
}

void Ne2000Send(USHORT sLen, UCHAR iType)
{
	UCHAR iDummy;
	PCHAR p;
	USHORT sCount;
#ifndef CALL_NONE
	UCHAR pHead[EP_DATA + VLAN_HEADER_SIZE];
#endif

	if (!_bAdapterOK)	return;

	p = (PCHAR)BANK_BASE;

#ifndef CALL_NONE
	if (_bVLan)
	{
		memcpy(pHead, Adapter_pPacketBuf, EP_TYPE);
		USHORT2PCHAR(EPT_VLAN, (PCHAR)(pHead + EP_TYPE));
		USHORT2PCHAR(_sLayer2Qos, (PCHAR)(pHead + EP_TYPE + VLAN_TCI));
		memcpy((PCHAR)(pHead + EP_TYPE + VLAN_HEADER_SIZE), (PCHAR)(Adapter_pPacketBuf + EP_TYPE), 2);
		if (!Ne2000Write(NE2000_RAM_BASE, pHead, EP_DATA + VLAN_HEADER_SIZE))	return;
	}
#endif

	if (iType == NET_SEND_NORMAL)
	{
		if (_bVLan)
		{
			if (!Ne2000Write(NE2000_RAM_BASE + EP_DATA + VLAN_HEADER_SIZE, (PCHAR)(Adapter_pPacketBuf + EP_DATA), sLen - EP_DATA))	return;
			sLen += VLAN_HEADER_SIZE;
		}
		else
		{
			if (!Ne2000Write(NE2000_RAM_BASE, Adapter_pPacketBuf, sLen))	return;
		}
	}
#ifndef CALL_NONE
	else if (iType == NET_SEND_PPPOE)
	{
		if (_bVLan)
		{
			if (!Ne2000Write(NE2000_RAM_BASE + EP_DATA + VLAN_HEADER_SIZE, PPPoE_pSendHead, PPPOE_INFO))	return;
			if (!Ne2000Write(NE2000_RAM_BASE + EP_DATA + VLAN_HEADER_SIZE + PPPOE_INFO, (PCHAR)(Adapter_pPacketBuf + EP_DATA), sLen - EP_DATA))	return;
			sLen += VLAN_HEADER_SIZE + PPPOE_INFO;
		}
		else
		{
			if (!Ne2000Write(NE2000_RAM_BASE, Adapter_pPacketBuf, EP_DATA))	return;
			if (!Ne2000Write(NE2000_RAM_BASE + EP_DATA, PPPoE_pSendHead, PPPOE_INFO))	return;
			if (!Ne2000Write(NE2000_RAM_BASE + EP_DATA + PPPOE_INFO, (PCHAR)(Adapter_pPacketBuf + EP_DATA), sLen - EP_DATA))	return;
			sLen += PPPOE_INFO;
		}
	}
#endif

	EnableCE1();

#ifdef DEBUG_OVERFLOW
	p[NIC_INTR_MASK] |= IMR_XMIT + IMR_XMIT_ERR;   // Enable transmit interrupts.
#else
	p[NIC_INTR_MASK] = IMR_XMIT + IMR_XMIT_ERR;   // Enable transmit interrupts.
#endif

    // Prepare the NIC registers for transmission.
    p[NIC_XMIT_START] = NE2000_NIC_XMIT_START;
	USHORT2PCHAR_L((sLen < ETHERNET_MIN_SIZE) ? ETHERNET_MIN_SIZE : sLen, (PCHAR)(BANK_BASE+NIC_XMIT_COUNT_LSB));
/*	if (sLen < ETHERNET_MIN_SIZE)		sLen = ETHERNET_MIN_SIZE;
	p[NIC_XMIT_COUNT_LSB] = (UCHAR)sLen;
	p[NIC_XMIT_COUNT_MSB] = (UCHAR)(sLen >> 8);
*/
    // Start transmission, check for power failure first.
//    iDummy = p[NIC_COMMAND];
    p[NIC_COMMAND] = CR_START + CR_XMIT + CR_NO_DMA;

	sCount = 0xffff;
	while (sCount)
	{
#ifdef SYS_PROGRAMMER
		if (rGPIO_D & 0x10)			// Wait for the current send to complete, GPIO D4 to high
#else
		if (rGPIO_C & 0x02)			// Wait for the current send to complete, GPIO C1 to high
#endif
		{
		    // Get any new interrupts
			iDummy = p[NIC_INTR_STATUS];
//		    if (iDummy != ISR_EMPTY)
		    if (iDummy)
			{
				// Acknowledge the interrupt
				p[NIC_INTR_STATUS] = iDummy;
		        if (iDummy & (ISR_XMIT + ISR_XMIT_ERR))
			    {
					break;
				}
			}
		}
		else
		{
			sCount --;
		}
	}
#ifdef DEBUG_OVERFLOW
	p[NIC_INTR_MASK] &= ~(IMR_XMIT + IMR_XMIT_ERR);
#else
    p[NIC_INTR_MASK] = 0;
#endif

	DisableCEx();
}

#endif
