// call_log.c

typedef struct _CALL_RECORD CALL_RECORD;
struct _CALL_RECORD
{
	CALL_RECORD *	next;
	ULONG			lDatetime;
	UCHAR			iMem;
};

void CallLogInit();
void CallLogAdd(PCHAR pName, PCHAR pNumber, UCHAR iType);
void CallLogTimer();
UCHAR CallLogGetNum(UCHAR iType);
void CallLogDeleteAll(UCHAR iType);
CALL_RECORD * CallLogRead(UCHAR iType, UCHAR iIndex);

// menu.c
#define MENU_TYPE_MAIN		0
#define MENU_TYPE_SUB1		1
#define MENU_TYPE_SUB2		2
#define MENU_TYPE_SUB3		3
#define MENU_TYPE_EDIT		4
#define MENU_TYPE_SAVEPB	5
#define MENU_TYPE_SAVECFG	6
#define MENU_TYPE_LOADDFT	7
#define MENU_TYPE_STOREDFT	8
#define MENU_TYPE_TEST		9
#ifdef OEM_IP20
#define MENU_TYPE_ALR		10
#endif

// Main menu
#ifdef OEM_IP20

#define MAIN_MENU_PHONEBOOK		2
#define MAIN_MENU_CALL			0
#define MAIN_MENU_SETTINGS		1
#define MAIN_MENU_INFO			3
#ifdef SYS_TEST
#define MAIN_MENU_TEST			4
#define MAIN_MENU_MAX			5
#else
#define MAIN_MENU_MAX			4
#endif

#else

#define MAIN_MENU_PHONEBOOK		0
#define MAIN_MENU_CALL			1
#define MAIN_MENU_SETTINGS		2
#define MAIN_MENU_INFO			3
#ifdef SYS_TEST
#define MAIN_MENU_TEST			4
#define MAIN_MENU_MAX			5
#else
#define MAIN_MENU_MAX			4
#endif

#endif

// Level-1 submenu
#define PHONEBOOK_MENU_MAX		PHONEBOOK_ENTRY_NUM

#define TEST_MENU_LCD	0
#define TEST_MENU_LED	1
#define TEST_MENU_KEY	2
#define TEST_MENU_MAX	3

#define CALL_MENU_MISSED	0
#define CALL_MENU_RECVED	1
#define CALL_MENU_DIALED	2
#define CALL_MENU_MAX		3

#define SETTINGS_MENU_NETWORK	0
#define SETTINGS_MENU_VOICE		1
#define SETTINGS_MENU_PROTOCOL	2
#define SETTINGS_MENU_DIALPLAN	3
#define SETTINGS_MENU_SYSTEM	4
#define SETTINGS_MENU_LOADDFT	5
#define SETTINGS_MENU_STOREDFT	6
#ifdef OEM_IP20
#define SETTINGS_ALARM		    7
#define SETTINGS_MENU_MAX		8
#else
#define SETTINGS_MENU_MAX		7
#endif

#define INFO_MENU_IP		0
#define INFO_MENU_NUMBER	1
#define INFO_MENU_MAC		2
#define INFO_MENU_MODEL		3
#define INFO_MENU_PROTOCOL	4
#define INFO_MENU_LANGUAGE	5
#define INFO_MENU_OEM		6
#define INFO_MENU_VERSION	7
#define INFO_MENU_MAX		8

// Level-2 submenu
#define PB_ACTION_MENU_CALL		0
#define PB_ACTION_MENU_EDIT		1
#define PB_ACTION_MENU_DELETE	2
#define PB_ACTION_MENU_ADD		3
#define PB_ACTION_MENU_DELALL	4
#define PB_ACTION_MENU_MAX		5

#define CH_ACTION_MENU_CALL		0
#define CH_ACTION_MENU_ADD		1
#define CH_ACTION_MENU_DELALL	2
#define CH_ACTION_MENU_MAX		3

// Edit menu
#define EDIT_PB_ADD_NAME		0
#define EDIT_PB_ADD_NUMBER		1
#define EDIT_PB_EDIT_NAME		2
#define EDIT_PB_EDIT_NUMBER		3
#define EDIT_PHONE_SETTINGS		4

#define NETWORK_MENU_MAX		12

#ifdef OEM_BT2008
#define DIALPLAN_MENU_MAX		17
#elif defined OEM_IP20
#define DIALPLAN_MENU_MAX		10
#else
#define DIALPLAN_MENU_MAX		12
#endif

#define VOICE_MENU_MAX			19

#ifdef CALL_IAX2
#define PROTOCOL_MENU_MAX		11
#else
#define PROTOCOL_MENU_MAX		27
#endif

#ifdef OEM_INNOMEDIA
#define SYSTEM_MENU_MAX			11
#else
#define SYSTEM_MENU_MAX			12
#endif

#define IP_TYPE_MAX				3


void MenuHandler(UCHAR iType);

// settings.c
// Option type
#define	OPTION_TYPE_SELECT		0
#define OPTION_TYPE_RADIO		1
#define OPTION_TYPE_BYTE		2
#define OPTION_TYPE_SHORT		3
#define OPTION_TYPE_IPADDR		4
#define OPTION_TYPE_STRING		5
#define OPTION_TYPE_UNKNOWN		0xff

USHORT OptGetIndex(PCHAR pName);
UCHAR OptGetType(USHORT sIndex);
UCHAR OptGetMaxlen(USHORT sIndex);
void OptWriteStr(PCHAR pString, USHORT sIndex, UCHAR iMaxLen);

USHORT HttpHandleEvent(UCHAR iEvent, USHORT sParam);

// ui_str.c
extern const UCHAR _cMainMenu[MAIN_MENU_MAX][DISPLAY_MAX_CHAR+1];
extern const UCHAR _cTestMenu[TEST_MENU_MAX][DISPLAY_MAX_CHAR+1];
extern const UCHAR _cCallMenu[CALL_MENU_MAX][DISPLAY_MAX_CHAR+1];
extern const UCHAR _cSettingsMenu[SETTINGS_MENU_MAX][DISPLAY_MAX_CHAR+1];
extern const UCHAR _cInfoMenu[INFO_MENU_MAX][DISPLAY_MAX_CHAR+1];
extern const UCHAR _cPbActionMenu[PB_ACTION_MENU_MAX][DISPLAY_MAX_CHAR+1];
extern const UCHAR _cChActionMenu[CH_ACTION_MENU_MAX][DISPLAY_MAX_CHAR+1];
extern const UCHAR _cNetworkMenu[NETWORK_MENU_MAX][DISPLAY_MAX_CHAR+1];
extern const UCHAR _cVoiceMenu[VOICE_MENU_MAX][DISPLAY_MAX_CHAR+1];
extern const UCHAR _cProtocolMenu[PROTOCOL_MENU_MAX][DISPLAY_MAX_CHAR+1];
extern const UCHAR _cDialplanMenu[DIALPLAN_MENU_MAX][DISPLAY_MAX_CHAR+1];
extern const UCHAR _cSystemMenu[SYSTEM_MENU_MAX][DISPLAY_MAX_CHAR+1];

extern const UCHAR _cSavePhonebook[];
extern const UCHAR _cSaveSettings[];
extern const UCHAR _cConfirm[];
extern const UCHAR _cYesOrCancel[];
extern const UCHAR _cEndOfList[];
extern const UCHAR _cSpaces[];
extern const UCHAR _cContacts[];
extern const UCHAR _cRecords[];
extern const UCHAR _cEnterName[];
extern const UCHAR _cEnterNumber[];
extern const UCHAR _cOKToEnter[];
#ifndef OEM_IP20
extern const UCHAR _cError[];
#endif
extern const UCHAR _cEmpty[];
extern const UCHAR _cWriting[];

#ifdef OEM_IP20
extern const UCHAR _cCancelOK[];
extern const UCHAR _cCancel[];
extern const UCHAR _cGeriSec[];
extern const UCHAR _cCiKaydet[];
extern const UCHAR _cYesNo[];
#endif

extern const UCHAR _cCodecMark[VOICE_CODER_NUM+1][9];
extern const UCHAR _cSpeexRateMark[8][9];
extern const UCHAR _cIlbcFrameMark[2][7];
extern const UCHAR _cYesNoMark[2][6];
extern const UCHAR _cDtmfTypeMark[3][9];
extern const UCHAR _cGetVoiceMsgMark[2][13];
extern const UCHAR _cOnOffMark[2][6];

extern const UCHAR _cIPTypeMark[IP_TYPE_MAX][DISPLAY_MAX_CHAR+1];
extern const UCHAR _cDnsTypeMark[2][DISPLAY_MAX_CHAR+1];
extern const UCHAR _cNatTraversalMark[3][DISPLAY_MAX_CHAR+1];

extern const UCHAR _cIDS[][DISPLAY_MAX_CHAR+1];

void LoadString(UCHAR iIDS, PCHAR pBuf);

