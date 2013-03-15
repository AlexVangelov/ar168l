// main.c

extern TIME_T Sys_tmLocal;

void OptionsGetBuf(PCHAR pBuf, USHORT sOffset, UCHAR iLen);
UCHAR OptionsGetByte(USHORT sOffset);

// Task functions
void TaskStartProv();
void TaskStartApp();
void TaskOutgoingData(UCHAR iFrame1, PCHAR pBuf1, UCHAR iFrame2, PCHAR pBuf2);
void TaskIncomingData(USHORT sLen, PCHAR pBuf);
void TaskFrameTimer();

void TaskMiniRun();

#define MD5_SIGNATURE_SIZE	16
void TaskMD5GenValue(PCHAR pDst, PCHAR pSrc, UCHAR iLen);

#ifdef OEM_INNOMEDIA
void TaskRC4Init(PCHAR pKey, UCHAR iKeyLen);
void TaskRC4Calc(PCHAR pDst, PCHAR pSrc, USHORT sLen);
void TaskGenKey(PCHAR P, PCHAR S, USHORT c, PCHAR DK, UCHAR dkLen);
#endif

#define DIAL_TIMEOUT_START	30
#define DIAL_TIMEOUT_END	5
#define HANGUP_TIMEOUT		10
BOOLEAN TaskMapMatch(PCHAR pNumber);

#define STUN_TASK_NONE		0
#define STUN_TASK_LOGON		1
#define STUN_TASK_CALL		2
#define STUN_TASK_ANSWER	3
void TaskStunMap(UCHAR iTask, UDP_SOCKET pSocket);
void TaskStunDone(PCHAR pMappedAddr, USHORT sMappedPort, UCHAR iTask);
BOOLEAN TaskStunRun(UDP_SOCKET pUcb);

void TaskHttpDigest(PCHAR pDst, PCHAR pAuthData, PCHAR pUri, PCHAR pAccount, PCHAR pPassword, PCHAR pMethod);
void TaskDnsQuery(PCHAR pDomain, UCHAR iType, UCHAR iBankOffset, USHORT sCallBackAddr);
void TaskDnsDone(PCHAR pIP, USHORT sAddr, UCHAR iBank);
void TaskUdpRunData(UDP_SOCKET pUcb);
void TaskRunTcp(TCP_HANDLE h, USHORT sLen, PCHAR pBuf, USHORT sAddr, UCHAR iBank);

void TaskIpSendData(USHORT sLen, PCHAR pDstIP, UCHAR iProtocol);

void TaskTcpRun(PACKET_LIST * p);

#define TEST_ITEM_LCD	0
#define TEST_ITEM_LED	1
#define TEST_ITEM_KEY	2
#define TEST_ITEM_TIMER	3
#define TEST_ITEM_DONE	4
#define TEST_ITEM_NONE	5

// UI Event, from other bank to protocol bank
#define UI_EVENT_LOGON		0
#define UI_EVENT_LOGOFF		1
#define UI_EVENT_TIMER		2
#define UI_EVENT_TESTDONE	3

// All possible keys
#define UI_EVENT_HANDSET_UP		10
#define UI_EVENT_HANDSET_DOWN	11
#define UI_EVENT_CALL			12
#define UI_EVENT_FLASH			13
#define UI_EVENT_TRANSFER		14
#define UI_EVENT_HOLD			15
#define UI_EVENT_MEM_CALL		16
#define UI_EVENT_MENU			17
#define UI_EVENT_OK				18
#define UI_EVENT_CANCEL			19
#define UI_EVENT_LEFT			20
#define UI_EVENT_RIGHT			21
#define UI_EVENT_UP				22
#define UI_EVENT_DOWN			23
#define UI_EVENT_MUTE_DND		24
#define UI_EVENT_TEXT			25

#define UI_EVENT_DIGIT			30
#define UI_EVENT_DIGIT0			30
#define UI_EVENT_DIGIT1			31
#define UI_EVENT_DIGIT2			32
#define UI_EVENT_DIGIT3			33
#define UI_EVENT_DIGIT4			34
#define UI_EVENT_DIGIT5			35
#define UI_EVENT_DIGIT6			36
#define UI_EVENT_DIGIT7			37
#define UI_EVENT_DIGIT8			38
#define UI_EVENT_DIGIT9			39
#define UI_EVENT_STAR			40	
#define UI_EVENT_POUND			41	// Only for AR168a/b/c, use '#' to replace hook

#define SIPR_EVENT_LOGON_OK			100
#define SIPR_EVENT_LOGON_FAILED		101
#define SIPR_EVENT_SEND_REQ			102
#define SIPR_EVENT_SEND_RESP		103
#define SIPR_EVENT_CALL_CANCELED	104
#define SIPR_EVENT_AUTO_ANSWER		105
#define SIPR_EVENT_LINE_FREE		106
#define SIPR_EVENT_CALL_FORWARDED	107
#define SIPR_EVENT_XFEREE_DONE		108
#define SIPR_EVENT_NEW_TAG			109
#define SIPR_EVENT_SUB_FAILED		110
#define SIPR_EVENT_MODIFY_RTPPORT	111

void TaskMenuEvent(UCHAR iEvent);
void TaskHandleEvent(UCHAR iEvent, USHORT sParam);

// Call state
#define CALL_STATE_FREE			0	// IDLE
#define CALL_STATE_DIALING		1	// CALLER STATE, We stay in this state until enough digits are collected from user and then enter calling state	
#define CALL_STATE_CALLING		2	// CALLER STATE Step1, We have pressed 'Call' key and waiting for dns to resolve
#define CALL_STATE_CALLING2		3	// CALLER STATE Step2, We have sent out the number we want to call and waiting for the other party's reponse
#define CALL_STATE_RINGING		4	// CALLEE STATE, We are ringing
#define CALL_STATE_TALKING		5	// BOTH CALLER AND CALLEE
#define CALL_STATE_HANGUP		6

#define CALL_INFO_STATE			0
#define CALL_INFO_IS_CALLEE		1
#define CALL_INFO_PEER_NAME		2
#define CALL_INFO_PEER_NUMBER	3
#define CALL_INFO_CALL_TIME		4

USHORT TaskGetCallInfo(UCHAR iType);

#define CALL_LOG_TYPE_MISSED	0
#define CALL_LOG_TYPE_RECVED	1	
#define CALL_LOG_TYPE_DIALED	2

void TaskCallLogAdd(PCHAR pName, PCHAR pNumber, UCHAR iType);

#define HTTP_GET_SETTINGS_LEN	0
#define HTTP_GET_PB_LEN			1
#define HTTP_GET_DM_LEN			2
#define HTTP_UPDATE_SETTINGS	3
#define HTTP_UPDATE_PB			4
#define HTTP_UPDATE_DM			5
#define HTTP_WRITE_SETTINGS		6
#define HTTP_WRITE_PB			7
#define HTTP_WRITE_DM			8
#define HTTP_WRITE_APDM			9

USHORT TaskHttpEvent(UCHAR iEvent, USHORT sParam);

// UI message, for functions in other bank to call UI functions
#define UI_NET_CONNECTED			0
#define UI_SNTP_DONE				1
#define UI_SNTP_FAILED				2
#define UI_PROV_DONE				3
#define UI_CHANGE_MODE				4
#define UI_UPDATE_PROGRESS			5
#define UI_IAX2_TIME				6
#define UI_MENU_EXIT				7
#define UI_MENU_CALL				8
#define UI_MENU_DISPLAY_TIME		9
#define UI_LOGON_OK					10
#define UI_LOGON_FAILED				11
#define UI_LOAD_DEFAULTS			12
#define UI_STORE_DEFAULTS			13
#define UI_TEST_PHONE				14

#define UI_STBY_DISPLAY_LINE0		20
#define UI_STBY_DISPLAY_LINE1		21
#define UI_STBY_DISPLAY_LINE2		22
#define UI_STBY_DISPLAY_LINE3		23
#define UI_STBY_DISPLAY_IDS0		24
#define UI_STBY_DISPLAY_IDS1		25
#define UI_STBY_DISPLAY_IDS2		26
#define UI_STBY_DISPLAY_IDS3		27
#define UI_CALL_DISPLAY_LINE0		28
#define UI_CALL_DISPLAY_LINE1		29
#define UI_CALL_DISPLAY_LINE2		30
#define UI_CALL_DISPLAY_LINE3		31
#define UI_CALL_DISPLAY_IDS0		32
#define UI_CALL_DISPLAY_IDS1		33
#define UI_CALL_DISPLAY_IDS2		34
#define UI_CALL_DISPLAY_IDS3		35

#define UI_CALL_CONNECTED			36
#define UI_CALL_UPDATE_CODEC		37
#define UI_CALL_DECODE_ENCODE		38
#define UI_CALL_FINISHED			39
#define UI_CALL_RINGING				40
#define UI_CALL_HOLD				41
#define UI_CALL_ENDED				42
#define UI_CALL_CALLING				43
#define UI_CALL_DIAL				44
#define UI_CALL_INCOMING_DTMF		45
#define UI_CALL_AUTOANSWER			46
#define UI_CALL_PLAYKEYS			47
#define UI_CALL_RINGBACK			48
#define UI_CALL_TIMER				49
#define UI_CALL_PLAYKEY				50
#define UI_CALL_HANGUP				51
#define UI_CALL_XFERFAIL			52
#define UI_CALL_DISPLAY_MSG			53
#define UI_VOICE_MESSAGE			54
#define UI_CALL_WAITING				55
#define UI_CALL_RINGBACK_REMOTE		56

#define UI_PPP_DEBUG				60

void TaskUIHandler(UCHAR iType, USHORT sParam);

void TaskWebcallStart();

void TaskSerialSendString(PCHAR pStr);

/*
BOOLEAN UpgradeCheckFile();
void UpgradeWriteFlag(UCHAR iUpdatePage);
void OptionsKeepFlag();
void OptionsSetNetwork();
*/

#define SYS_UPGRADE_CHECK_FILE		0
#define SYS_UPGRADE_CHECK_STATUS	1
#define SYS_OPTIONS_SET_NETWORK		3
#define SYS_MODIFY_PAGE				5
#define SYS_UPDATE_FLAG				6

BOOLEAN TaskSystemHandler(UCHAR iType, UCHAR iParam);

#define IDS_VERSION				0
#define IDS_DHCP_START			1
#define IDS_DHCP_FAILED			2
#define IDS_LOGON_OK			3
#define IDS_LOGON_FAILED		4
#define IDS_MISSED_CALLS		5
#define IDS_PPPOE_START			6
#define IDS_PHONE_READY			7
#define IDS_TFTPD_MODE			8
#define IDS_HTTPD_MODE			9
#define IDS_AUTOPROV_MODE		10
#define IDS_NEW_MESSAGE			11
#define IDS_ENTER_NUMBER		12
#define IDS_CALLING				13	
#define IDS_RINGING				14	
#define IDS_CALL_WAITING		15
#define IDS_CONNECTED			16	
#define IDS_PLEASE_HANGUP		17	
#define IDS_UNKNOWN_REASON		18		// From here to IDS_XFER_DONE must be conitues
#define IDS_NOT_LOGON			19	
#define IDS_EMPTY_NUMBER		20		
#define IDS_WRONG_NUMBER		21
#define IDS_PEER_BUSY			22
#define IDS_NO_ANSWER			23
#define IDS_PEER_REJECT			24
#define IDS_PEER_HANGUP			25
#define IDS_MSG_TIMEOUT			26
#define IDS_CODER_MISMATCH		27
#define IDS_RES_LIMITED			28
#define IDS_BAD_SERVERIP		29
#define IDS_XFER_FAILED			30
#define IDS_XFER_DONE			31
#define IDS_CALL_ONHOLD			32
#define IDS_CALL_XFER			33
#define IDS_GET_DATETIME		34
#define IDS_GET_SETTINGS		35
#define IDS_GET_FIRMWARE		36	
#define IDS_VOLUME_INC			37
#define IDS_VOLUME_DEC			38
#define IDS_MEMORY_DIAL			39
#define IDS_CALL_MUTED			40
#define IDS_DO_NOT_DISTURB		41
#define IDS_SNTP_FAILED			42
#define IDS_CALL_FORWARDED		43
#define IDS_PPPOE_FAILED		44
#define IDS_PORT1_LINKED		45
#define IDS_PORT1_UNPLUGGED		46
#define IDS_PORT2_LINKED		47
#define IDS_PORT2_UNPLUGGED		48
#define IDS_EMPTY_STRING		49
#define IDS_BATTERY_LOW			50
#define IDS_ON_BATTERY			51
#define IDS_KEY_OK				52
#define IDS_LCD_OK				53
#define IDS_LED_OK				54

#define IDS_COMMON_END			54

#ifdef OEM_IP20
#define IDS_OK					(IDS_COMMON_END + 1)
#define IDS_KAREL				(IDS_COMMON_END + 2)
#define IDS_SIL_ARA				(IDS_COMMON_END + 3)
//#define IDS_ARAYAN			(IDS_COMMON_END + 4)
#define IDS_DETAY				(IDS_COMMON_END + 4)
#define IDS_SESSIZ_REDDET		(IDS_COMMON_END + 5)
#define IDS_KAPAT				(IDS_COMMON_END + 6)
#define IDS_DINLE_1				(IDS_COMMON_END + 7)
#define IDS_DINLE_2				(IDS_COMMON_END + 8)
#define IDS_LEDLER				(IDS_COMMON_END + 9)
#define IDS_YAN_OR				(IDS_COMMON_END + 10)
#define IDS_EVET				(IDS_COMMON_END + 11)
#define IDS_EKRANDAKI			(IDS_COMMON_END + 12)
#define IDS_KARAKTERLER_DOLU	(IDS_COMMON_END + 13)
#define IDS_1E_DE				(IDS_COMMON_END + 14)
#define IDS_ILERI				(IDS_COMMON_END + 15)
#define IDS_ALARM_ZAMAN			(IDS_COMMON_END + 16)
#define IDS_PARK_PBOOK			(IDS_COMMON_END + 17)
#define IDS_TRANS_REDIAL		(IDS_COMMON_END + 18)
#define IDS_VOLDOWN_VOLUP		(IDS_COMMON_END + 19)
#define IDS_ALARM_KAPAT			(IDS_COMMON_END + 20)
#define IDS_TRANSFER_YAP		(IDS_COMMON_END + 21)
#define IDS_FEN_KAPAT			(IDS_COMMON_END + 22)
#define IDS_ALARM_CANCELLED		(IDS_COMMON_END + 23)
#define IDS_ALARM_AT			(IDS_COMMON_END + 24)
#define IDS_ALARM_BUF			(IDS_COMMON_END + 25)
#define IDS_T_K					(IDS_COMMON_END + 26)
#define IDS_IP20_V_AAT			(IDS_COMMON_END + 27)
#define IDS_TRUE				(IDS_COMMON_END + 28)
#define IDS_FALSE				(IDS_COMMON_END + 29)
#define IDS_VAR					(IDS_COMMON_END + 30)
#define IDS_SESLI_MESAJ			(IDS_COMMON_END + 31)
#define IDS_BA_TEST			(IDS_COMMON_END + 32)
#endif

#ifdef OEM_INNOMEDIA
#define IDS_PROVISION_FAILED	(IDS_COMMON_END + 1)
#endif

#ifdef OEM_BT2008
#define IDS_ALARM_TIME			(IDS_COMMON_END + 1)
#define IDS_ALARM_CANCELLED		(IDS_COMMON_END + 2)
#define IDS_ALARM_AT			(IDS_COMMON_END + 3)
#define IDS_ALARM_BUF			(IDS_COMMON_END + 4)
#endif

#define IDS_DISPLAY_MODEL		255

void TaskLoadString(UCHAR iIDS, PCHAR pBuf);

void delay1ms()  __naked;
void Delay(USHORT sMilliSecond);

#ifdef SYS_UART
void UART_Isr(UCHAR iVal);
#endif

// Interrupt functions, isr.c
#define KEY_ADC_STAGE	5
#define KEY_4X4_SIZE	4
#define KEY_5X5_SIZE	5
#define KEY_5X6_SIZE	12
#define KEY_8X4_SIZE	8
#define KEY_8X5_SIZE	8
#define KEY_7X8_SIZE	8

#ifdef SERIAL_UI
#define SERIAL_BUF_SIZE_TX	255
#define SERIAL_BUF_SIZE_RX	255 
#elif defined SERIAL_LOOP
#define SERIAL_BUF_SIZE_TX	24
#define SERIAL_BUF_SIZE_RX	32 
#elif defined SERIAL_WDCB
#define SERIAL_BUF_SIZE_TX	32
#define SERIAL_BUF_SIZE_RX	32 
#elif defined SERIAL_CARD_READER
#define SERIAL_BUF_SIZE_RX      32
#endif

void isr() __interrupt __naked;

// isr_gpio.c
extern volatile BOOLEAN _bTimer;
extern volatile ULONG Sys_lTicker;
extern volatile ULONG Sys_lCurrentTime;		// current time (in seconds)
extern volatile UCHAR Sys_iDebugVal;		// temp debug use, report value as soon as none zero in UI_Run();
extern volatile USHORT Sys_sDebug;			// temp debug use, report non zero value in whole second timer UI_Timer();
extern volatile USHORT Sys_sMinuteDebug;	// temp debug use, report non zero value in whole minute timer UI_MinuteTimer();

UCHAR _Check_B_By_B(UCHAR iAndMask);
void _10msCounter();

// ce3.c
void IP20_LedControl(UCHAR ch1, UCHAR ch2);

#define NET_SEND_NORMAL		0
#define NET_SEND_PPPOE		1
#define NET_SEND_EAP		2	// for 802.1X

// ksz8842.c
#ifdef KSZ8842
void Ksz8842Init();
void Ksz8842Run();
void Ksz8842Read(PCHAR pBuf, USHORT sLen);
void Ksz8842ReadBypass(USHORT sLen);
void Ksz8842Send(USHORT sLen, UCHAR iType);		// iType in NET_SEND_XXXX
void Ksz8842Write(PCHAR pBuf, USHORT sLen);

UCHAR ksz8842_read_reg_byte(UCHAR iBank, UCHAR iOffset);
void ksz8842_or_reg_byte(UCHAR iBank, UCHAR iOffset, UCHAR iOrVal);
void ksz8842_and_reg_byte(UCHAR iBank, UCHAR iOffset, UCHAR iAndVal);
void ksz8842_write_reg_word(UCHAR iBank, UCHAR iOffset, USHORT sVal);
void ksz8842_or_reg_word(UCHAR iBank, UCHAR iOffset, USHORT sOrVal);
void ksz8842_and_reg_word(UCHAR iBank, UCHAR iOffset, USHORT sAndVal);

#define NetSend			Ksz8842Send
#define NetRead			Ksz8842Read
#define NetReadBypass	Ksz8842ReadBypass
#endif

// Lcd functions
#define LCD_STATUS_CHAR			0x1f
#define EXTENDED_CHAR_START		0xa0

void LcdInit();
void LcdTest4();
void LcdTest5();
void LcdStart();
void LcdDisplay(PCHAR pBuf, UCHAR iLine);
void LcdCursor(UCHAR iLine, UCHAR iPos, BOOLEAN bShow);

void LcdLight(UCHAR iRow, UCHAR iData);
void LcdDisplayBMP(PCHAR pBuf, BOOLEAN bReverse);

void LcdDebugVal(USHORT sVal, UCHAR iRadix, UCHAR iLine);
void LcdDebugVals(PCHAR pVal, UCHAR iLen, UCHAR iLine);

void LcdCenterDisplay(PCHAR pString, UCHAR iLine);

void EnableLCD();


// Ne2000 functions
#ifdef RTL8019AS
void Ne2000InitStep1();
void Ne2000InitStep2();
void Ne2000Init();
void Ne2000Run();
void Ne2000MiniRun();
void Ne2000Send(USHORT sLen, UCHAR iType);		// iType in NET_SEND_XXXX
BOOLEAN Ne2000Read(USHORT sAddr, PCHAR pBuf, USHORT sLen);
BOOLEAN Ne2000Write(USHORT sAddr, PCHAR pBuf, USHORT sLen);

#define NetSend			Ne2000Send
#endif

// Program flash functions
#define BUF_PAGE_NUM	4
extern const UCHAR _cMemoryPages[BUF_PAGE_NUM+2];
#define DEFAULT_MEMORY_PAGE		0x07
//#define DSP_LOW_8K_PAGE			0x2c
#define DSP_LOW_8K_PAGE			0x24
#define DSP_LOW_8K_PAGE2		0x25
#define DSP_LOW_8K_PAGE3		0x26

void FlashInit();
UCHAR FlashSetBankOffset(UCHAR iBankOffset);
void FlashSetBank(UCHAR iBank);
UCHAR FlashWritePage(UCHAR iPage);
void FlashWriteSettingsSectors(UCHAR iNumber);
void FlashReboot();

void FlashEraseChip(UCHAR iFirstPage);

#define QUERY_MANUFACTURER_ID	0x00
#define QUERY_DEVICE_ID			0x01
#define QUERY_DEVICE_ID2		0x02	// for MXIC
#define QUERY_SECURITY_START	0x10
#define QUERY_LOCK_STATUS		0xff

#define SST_ID					0xbf
#define SST39VF1681_ID			0xc8
#define SST39VF1682_ID			0xc9

#define MXIC_ID					0xc2
#define MX29LV160CB_ID			0x49
#define MX29LV160CT_ID			0xc4

UCHAR FlashQueryData(UCHAR iFirstPage, UCHAR iType);

void FlashWriteSecurityID(UCHAR iFirstPage, PCHAR pID);

BOOLEAN FlashCompare(UCHAR iPage);

void EnableCE1();
void EnableCE2();
void EnableCE3();
void DisableCEx();

UCHAR GetExtPage();
void SetExtPage(UCHAR iPage);

void SetMemoryPage(UCHAR iPage);

void ReadFlashPage(UCHAR iPage);	// Uses DMA, faster than FlashReadPage
void ReadFlashData(USHORT sDstAddr, USHORT sSrcAddr, USHORT sLen, UCHAR iPage);		// Data will not cross 0x8000 boundary, others same as above

void SetHighSpeed();
void SetNormalSpeed();
void SetZ80Speed(UCHAR iSpeed);		// AR1688+ Speed: (27 + iSpeed * 3)Mhz, 0[27], 2[33], 6[45], 7[48], 8[51], 15[72]
BOOLEAN IsHighSpeed();

// DSP functions, dsp.c
#define DSP_OP_TEST_ENCODE			1
#define DSP_OP_TEST_DECODE			2
#define DSP_OP_DECODEPLAY_ENCODE	3
#define DSP_OP_DTMFPLAY_ENCODE		4
#define DSP_OP_DTMFPLAY_DTMFENCODE	5
#define DSP_OP_VADPLAY_ENCODE		6
#define DSP_OP_PLCPLAY_ENCODE		7
#define DSP_OP_RINGPLAY_RINGENCODE	8
#define DSP_OP_RINGPLAY_DTMFENCODE	9
#define DSP_OP_VADPLAY_HOLDENCODE	10
#define DSP_OP_DTMFPLAY				11
#define DSP_OP_CLEAR_BUFFER			12
#define DSP_OP_IVRPLAY				13
#define DSP_OP_TEST_ECHO			14
#define DSP_OP_CID_GENERATE			15

#define MAX_DSP_DURATION		8
#define DTMF_TONE_DSP_DURATION		4
#define STUTTER_TONE_DSP_DURATION	8

#define G711_FRAME_TIME		10
#define G726_FRAME_TIME		10
#define G729_FRAME_TIME		10
#define GSM610_FRAME_TIME	20
#define ILBC_30MS_TIME		30
#define ILBC_20MS_TIME		20
#define SPEEX_FRAME_TIME	20

extern volatile USHORT Dsp_sOpDuration;
extern volatile USHORT Dsp_sDurations[MAX_DSP_DURATION];
extern volatile UCHAR Dsp_iMaxDuration;
extern volatile UCHAR Dsp_iDurationIndex;
extern volatile USHORT Dsp_sDtmf[2];

#ifdef SYS_IVR
#define IVR_END_CHAR	'z'
#define MAX_IVR_STRING	32
extern UCHAR IVR_pString[MAX_IVR_STRING];
extern UCHAR IVR_iIndex;
extern USHORT IVR_iLength;
extern BOOLEAN IVR_bRepeat;
extern UCHAR IVR_iPage;
#endif

void DspBeginFrame(BOOLEAN bHalfFull);
void DspEndFrame();

// datatype.s functions
UCHAR P_HIBYTE(USHORT worddata);
UCHAR P_LOBYTE(USHORT worddata);
USHORT P_XCHGBYTE(USHORT worddata);
USHORT P_LOWORD(ULONG longdata);
USHORT P_HIWORD(ULONG longdata);
USHORT P_MAKEWORD(UCHAR lowbyte, UCHAR highbyte);
ULONG P_MAKELONG(USHORT lowword, USHORT highword);
USHORT PCHAR2USHORT_L(PCHAR src);
USHORT PCHAR2USHORT(PCHAR src);
void USHORT2PCHAR(USHORT src, PCHAR dst);
void USHORT2PCHAR_L(USHORT src, PCHAR dst);
ULONG PCHAR2ULONG(PCHAR src);
ULONG PCHAR2ULONG_L(PCHAR src);
void ULONG2PCHAR(ULONG src, PCHAR dst);
//void ULONG2PCHAR_L(ULONG src, PCHAR dst);		// not necessary for now, change datatype.s to use it in the future if needed

PCHAR strchr(PCHAR string, UCHAR ch);
USHORT strlen(PCHAR pStr);
UCHAR strcmp(PCHAR dst, PCHAR src);
UCHAR strcmp_lowercase(PCHAR dst, PCHAR src);
void strcpy(PCHAR dst, PCHAR src);
void strcat(PCHAR dst, const PCHAR src);
void strcat_char(PCHAR dst, UCHAR ch);
void memset(PCHAR dst, UCHAR ch, USHORT len);
UCHAR memcmp(PCHAR buf1, PCHAR buf2, USHORT len);
UCHAR memcmp_lowercase(PCHAR buf1, PCHAR buf2);		// buf2 must be an all lower case const string ended with 0
UCHAR memcmp_str(PCHAR buf1, PCHAR buf2);			// buf2 must be a string ended with 0
void memcpy(PCHAR dst, PCHAR src, USHORT len);
void memcpy4(PCHAR dst, PCHAR src);
// memmove and memcpy is NOT ANSI compatible, use memmove when copy data from tail to head, use memcpy from head to tail instead
// see data.c for C implementation example
void memmove(PCHAR dst, PCHAR src, USHORT len);

// math.s functions
USHORT mul_8x8(UCHAR i8, UCHAR i8);
ULONG mul_16x8(USHORT s16, UCHAR i8);
ULONG mul_32x8(ULONG l32, UCHAR i8);
USHORT div_16x8(USHORT s16, UCHAR i8);
UCHAR mod_16x8(USHORT s16, UCHAR i8);
ULONG rr_32x8(ULONG l32, UCHAR i8);
ULONG rl_32x8(ULONG l32, UCHAR i8);
ULONG rlc_32x8(ULONG l32, UCHAR i8);	// for md5 use only

// network.s functions
USHORT ip_checksum(ULONG lCheckSum);
ULONG short_sum(PCHAR pVal, USHORT sLen);
void fixed_input(PCHAR dst, USHORT len);
void fixed_output(PCHAR src, USHORT len);

void fixed_input_bypass(USHORT len);

#define NOP		__asm	nop	__endasm
#define EI		__asm	ei	__endasm
#define DI		__asm	di	__endasm
