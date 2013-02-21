/*-------------------------------------------------------------------------
   AR1688 TFTP Interface function copy right information

   Copyright (c) 2006-2012. Tang, Li      <tangli@palmmicro.com>
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
#include "apps.h"
#include "core.h"
#include "tcpip.h"
#include "bank1.h"

// RFC 1350 - The TFTP Protocol (Revision 2)
#define TFTP_SERVER_PORT	69

#define TFTP_TIME_OUT		3
#define TFTP_MAX_RETRY		4
#define TFTP_BLOCK_SIZE		512

#define	TFTP_OPCODE_PTR			0
#define	TFTP_FILENAME_PTR		2
#define TFTP_BLOCK_PTR			2
#define TFTP_ERRORCODE_PTR		2
#define TFTP_ERRORMSG_PTR		4
#define	TFTP_DATA_PTR			4

#define	TFTP_OPCODE_READ		1
#define	TFTP_OPCODE_WRITE		2
#define	TFTP_OPCODE_DATA		3
#define	TFTP_OPCODE_ACK			4
#define	TFTP_OPCODE_ERROR		5
#define TFTP_OPCODE_IDLE		255

/*
 * Error codes.
 */
#define	EUNDEF		0		/* not defined */
#define	ENOTFOUND	1		/* file not found */
#define	EACCESS		2		/* access violation */
#define	ENOSPACE	3		/* disk full or allocation exceeded */
#define	EBADOP		4		/* illegal TFTP operation */
#define	EBADID		5		/* unknown transfer ID */
#define	EEXISTS		6		/* file already exists */
#define	ENOUSER		7		/* no such user */

void TftpRun(UDP_SOCKET pUcb);

UDP_SOCKET _pTftpSocket;

USHORT _sBlockNum;
UCHAR _iTimer, _iRetries;
UCHAR _iLastOpcode;
USHORT _sTotalLen;		// we only have 64k bytes buffer in total
USHORT _sFileLen;
UCHAR _iUpdatePage;
BOOLEAN _bReadDone;

void _CheckUpdatePage(PCHAR pCur)
{
	PCHAR p;
	USHORT sOffset;

//	UdpDebugString(pCur);	// pCur in send buffer, can not debug correctly here

	sOffset = 0;
	_sTotalLen = 0;
	_sFileLen = 0;
	
#ifdef SYS_PROGRAMMER
	if (!memcmp_lowercase(pCur, "all"))	// all.dat
	{
		_iUpdatePage = SYSTEM_PROGRAMMER_PAGE*2;
		goto End;
	}

	p = strchr(pCur, '.');
	if (p)
	{
		if (!memcmp_lowercase((PCHAR)(p-4), "_all"))	// ar168j_sip_us_017027_all.bin
		{
			_iUpdatePage = SYSTEM_PROGRAMMER_PAGE;
			goto End;
		}
	}
#endif

	if (!memcmp_lowercase(pCur, "settings"))
	{
		ReadFlashPage(SYSTEM_SETTINGS_PAGE);
		_iUpdatePage = SYSTEM_SETTINGS_PAGE;
		_sFileLen = SETTINGS_FILE_SIZE;
		goto End;
	}

	if (!memcmp_lowercase(pCur, "phonebook"))
	{
		_iUpdatePage = SYSTEM_SETTINGS_PAGE;
		_sFileLen = PHONEBOOK_FILE_SIZE;
		ReadFlashPage(SYSTEM_SETTINGS_PAGE);
		sOffset = OPT_PHONE_BOOK;
		goto End;
	}

/*	if (!memcmp_lowercase(pCur, "digitmap"))
	{
		_iUpdatePage = SYSTEM_SETTINGS_PAGE;
		_sFileLen = DIGITMAP_FILE_SIZE;
		ReadFlashPage(SYSTEM_SETTINGS_PAGE);
		sOffset = OPT_DIGIT_MAP;
		goto End;
	}
*/
	if (!memcmp_lowercase(pCur, "ringtone"))
	{
		_iUpdatePage = SYSTEM_RINGTONE_PAGE;
		goto End;
	}

	if (!memcmp_lowercase(pCur, "holdmusic"))
	{
		_iUpdatePage = SYSTEM_HOLDMUSIC_PAGE;
		goto End;
	}

	if (!memcmp_lowercase(pCur, "font"))
	{
		_iUpdatePage = SYSTEM_FONT_PAGE;
		goto End;
	}

#ifdef SYS_IVR
	if (!memcmp_lowercase(pCur, "ivr"))
	{
		_iUpdatePage = SYSTEM_IVR_PAGE;
		goto End;
	}
#endif

	p = SkipField(pCur, '_');
	if (p)
	{
		if (!memcmp_lowercase(p, "none"))
		{
			_iUpdatePage = SYSTEM_BOOT_PAGE;
		}
		else
		{
			_iUpdatePage = (Sys_iSystemPage == SYSTEM_1ST_PAGE) ? SYSTEM_2ND_PAGE : SYSTEM_1ST_PAGE;
		}
		goto End;
	}

	_iUpdatePage = SYSTEM_INVALID_PAGE;
	UdpDebugString("error file name");

End:
	BufSeek(sOffset);
}

void _CheckReadPage(PCHAR pCur)
{
	PCHAR p;
	UCHAR iPage;
	USHORT sOffset;

//	UdpDebugString(pCur); 	// pCur in send buffer, can not debug correctly here

	sOffset = 0;
	if (!strcmp_lowercase(pCur, "pm.dat"))
	{
		_sTotalLen = 0xc000;
	}
	else if (!strcmp_lowercase(pCur, "dm.dat"))
	{
		_sTotalLen = 0xc000;
		BufCopyDM2PM();
	}
	else if (!strcmp_lowercase(pCur, "phonebook.dat"))
	{
		UdpDebugString("phonebook");
		ReadFlashPage(SYSTEM_SETTINGS_PAGE);
		_sTotalLen = PHONEBOOK_FILE_SIZE;
		sOffset = OPT_PHONE_BOOK;
	}
/*	else if (!strcmp_lowercase(pCur, "digitmap.dat"))
	{
		UdpDebugString("digitmap");
		ReadFlashPage(SYSTEM_SETTINGS_PAGE);
		_sTotalLen = DIGITMAP_FILE_SIZE;
		sOffset = OPT_DIGIT_MAP;
	}
*/	else if (!memcmp_lowercase(pCur, "page"))
	{
		pCur += 4;
		p = SkipField(pCur, '.');
		iPage = (p) ? atoi(pCur, 10) : SYSTEM_SETTINGS_PAGE;
		UdpDebugVal(iPage, 10);
		ReadFlashPage(iPage);
		_sTotalLen = 0;		// set total length of the file here, 0 == 65536
	}
	else
	{
		UdpDebugString("settings");
		ReadFlashPage(SYSTEM_SETTINGS_PAGE);
		TaskSystemHandler(SYS_OPTIONS_SET_NETWORK, 0);
		_sTotalLen = SETTINGS_FILE_SIZE;
	}
	BufSeek(sOffset);
}

void _SendAck(USHORT sBlock)
{
	PCHAR pCur;

	pCur = Adapter_pUdpBuf;
	USHORT2PCHAR(TFTP_OPCODE_ACK, pCur);
	USHORT2PCHAR(sBlock, (PCHAR)(pCur + TFTP_BLOCK_PTR));

	UdpSend(_pTftpSocket, 4);
	_iTimer = 0;
	_iLastOpcode = TFTP_OPCODE_ACK;
}

void _SendError(USHORT iErrorCode)
{
	PCHAR pCur;

	pCur = Adapter_pUdpBuf;
	USHORT2PCHAR(TFTP_OPCODE_ERROR, pCur);
	USHORT2PCHAR(iErrorCode, (PCHAR)(pCur + TFTP_ERRORCODE_PTR));
	pCur[TFTP_ERRORMSG_PTR] = 0;

	UdpSend(_pTftpSocket, 5);
}

void _SendData()
{
	USHORT sLen;
	PCHAR pCur;

	sLen = ((!_sTotalLen && !_bReadDone) || _sTotalLen >= TFTP_BLOCK_SIZE) ? TFTP_BLOCK_SIZE : _sTotalLen;
	pCur = Adapter_pUdpBuf;
	USHORT2PCHAR(TFTP_OPCODE_DATA, pCur);
	USHORT2PCHAR(_sBlockNum, (PCHAR)(pCur + TFTP_BLOCK_PTR));

	// fill data you want to send here
	// _sBlockNum can tell you which block of data you should send
//	memcpy((PCHAR)(pCur + TFTP_DATA_PTR), "hello world!", sLen);
	BufRead((PCHAR)(pCur + TFTP_DATA_PTR), sLen);
	sLen += TFTP_DATA_PTR;

	UdpSend(_pTftpSocket, sLen);
	_iTimer = 0;
	_iLastOpcode = TFTP_OPCODE_DATA;
}

void TftpInit()
{
	_pTftpSocket = UdpListen(TFTP_SERVER_PORT, NET_BANK_OFFSET, (USHORT)TftpRun);
	_iTimer = 0;
	_iRetries = 0;
	_sBlockNum = 0;
	_iLastOpcode = TFTP_OPCODE_IDLE;
	_iUpdatePage = SYSTEM_INVALID_PAGE;
}

void TftpClose()
{
	TaskUIHandler(UI_CHANGE_MODE, SYS_MODE_STANDBY);
	UdpClose(_pTftpSocket);
}

void TftpRun(UDP_SOCKET pUcb)
{
	USHORT iBlock;
	PCHAR pBuf;

	if (Sys_iCurMode != SYS_MODE_STANDBY && Sys_iCurMode != SYS_MODE_TFTPD)
	{
		UdpDebugString("Not in standby or tftpd mode, TFTP blocked");
		return;
	}

	if (pUcb != _pTftpSocket)	return;

	pBuf = Adapter_pUdpBuf;
	if (pBuf[0])
	{
		_SendError(EBADOP);
		return;
	}

	iBlock = PCHAR2USHORT((PCHAR)(pBuf + TFTP_BLOCK_PTR));
	switch (pBuf[1])
	{
	case TFTP_OPCODE_DATA:
		if (_sBlockNum == iBlock)
		{
			_sBlockNum ++;
			_iRetries = 0;
			// check if transfer is completed
			if (pUcb->sLen  < TFTP_BLOCK_SIZE + TFTP_DATA_PTR)
			{	// TFTP transfer is completed, do something here
				UdpDebugString("TFTP done");
				if (_sTotalLen == _sFileLen && pUcb->sLen == TFTP_DATA_PTR)
				{	// update OPT_SYSTEM_PAGE and other information about the upgrade
					TaskSystemHandler(SYS_UPDATE_FLAG, _iUpdatePage);
				}
				else
				{
					if (_iUpdatePage >= SYSTEM_FONT_PAGE && _iUpdatePage < SYSTEM_1ST_PAGE)
					{	// font/ivr length can be any value
						FlashWritePage(_iUpdatePage);
					}
					else
					{
						UdpDebugString("TFTP length error");
						UdpDebugVal(_sTotalLen, 16);
					}
				}
				UdpDebugString("rebooting ...");
				_SendAck(iBlock);
				FlashReboot();
			}
			else
			{
				BufWrite((PCHAR)(pBuf + TFTP_DATA_PTR), TFTP_BLOCK_SIZE);
				BufForward(TFTP_BLOCK_SIZE);
				_sTotalLen += TFTP_BLOCK_SIZE;
			}

			if (_sTotalLen == _sFileLen)	// enough data to write
			{
				TaskUIHandler(UI_UPDATE_PROGRESS, (USHORT)_iUpdatePage);
				TaskSystemHandler(SYS_MODIFY_PAGE, _iUpdatePage);

				if (_iUpdatePage == SYSTEM_INVALID_PAGE)
				{	// invalid file name, use to test tftp performance and network
				}
				else if (_iUpdatePage > SYSTEM_SETTINGS_PAGE)	
				{	// SYSTEM_1ST_PAGE, SYSTEM_2ND_PAGE, SYSTEM_RINGTONE_PAGE, SYSTEM_HOLDMUSIC_PAGE, SYSTEM_PROGRAMMER_PAGE
					if (FlashWritePage(_iUpdatePage))
					{
						_iUpdatePage = SYSTEM_INVALID_PAGE;
					}
					else
					{
						_iUpdatePage ++;
					}
				}
				else
				{	// SYSTEM_BOOT_PAGE or SYSTEM_SETTINGS_PAGE
					FlashWritePage(_iUpdatePage);
				}
				BufSeek(0);
			}
		}
		if (iBlock <= _sBlockNum)			// Ack this data packet
		{
			_SendAck(iBlock);
		}
		break;

	case TFTP_OPCODE_ACK:
		if (_sBlockNum == iBlock)
		{
			if ((!_sTotalLen && !_bReadDone) || _sTotalLen >= TFTP_BLOCK_SIZE)
			{
				BufForward(TFTP_BLOCK_SIZE);
				_sTotalLen -= TFTP_BLOCK_SIZE;
				if (_sTotalLen < TFTP_BLOCK_SIZE)
				{
					_bReadDone = TRUE;
				}
				_sBlockNum ++;
				_SendData();
				_iRetries = 0;
			}
			else
			{
				// file transfer done
				UdpDebugString("TFTP done");
				TftpClose();
				TftpInit();
			}
		}
		break;

	case TFTP_OPCODE_READ:
		TaskUIHandler(UI_CHANGE_MODE, SYS_MODE_TFTPD);
		UdpConnect(_pTftpSocket);
		UdpDebugString("TFTP read ...");
		_CheckReadPage((PCHAR)(pBuf + TFTP_FILENAME_PTR));
		_sBlockNum = 1;
		_bReadDone = FALSE;
		_SendData();
		_iRetries = 0;
		break;

	case TFTP_OPCODE_WRITE:
		TaskUIHandler(UI_CHANGE_MODE, SYS_MODE_TFTPD);
		UdpConnect(_pTftpSocket);
//		_SendAck(0);
		_sBlockNum = 1;
		_iRetries = 0;
		UdpDebugString("TFTP write ...");
		_CheckUpdatePage((PCHAR)(pBuf + TFTP_FILENAME_PTR));
		UdpDebugVal(_iUpdatePage, 10);
		_SendAck(0);
		break;

	case TFTP_OPCODE_ERROR:
		UdpDebugString("ERROR...");
		TftpClose();
		TftpInit();
		break;

	default:
		_SendError(EBADOP);
		break;
	}
}

void TftpTimer()
{
	if (Sys_iCurMode != SYS_MODE_TFTPD)	return;

	_iTimer ++;
	if (_iTimer > (1 << _iRetries))
	{
		_iRetries ++;
		_iTimer = 0;
		if (_iRetries >= TFTP_MAX_RETRY)
		{
			UdpDebugString("TFTP failed");
			TftpClose();
			TftpInit();
		}
		else
		{
			UdpDebugString("TFTP retry");
			if (_iLastOpcode == TFTP_OPCODE_ACK)
			{
				_SendAck((USHORT)(_sBlockNum - 1));
			}
			else if (_iLastOpcode == TFTP_OPCODE_DATA)
			{
				_SendData();
			}
		}
	}

}
