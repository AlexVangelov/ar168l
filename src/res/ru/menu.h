
const UCHAR _cMainMenu[MAIN_MENU_MAX][DISPLAY_MAX_CHAR+1] = {
"�������� �����", 
"������� �������",
"��������� �����.",
"����. ����������",
#ifdef SYS_TEST
"Test phone",
#endif
};

#ifdef SYS_TEST
const UCHAR _cTestMenu[TEST_MENU_MAX][DISPLAY_MAX_CHAR+1] = {
"Test LCD",
"Test LED",
"Test keypad",
};
#endif

const UCHAR _cCallMenu[CALL_MENU_MAX][DISPLAY_MAX_CHAR+1] = {
"��������. ������",
"�������� ������",
"��������� ������",
};

const UCHAR _cSettingsMenu[SETTINGS_MENU_MAX][DISPLAY_MAX_CHAR+1] = {
"TCP/IP ����",
"�����",
"��������",
"���� ������",
"�������",
"Load Defaults",
"Store Defaults",
};

const UCHAR _cInfoMenu[INFO_MENU_MAX][DISPLAY_MAX_CHAR+1] = {
"IP-�����",
"����� ��������",
"MAC-�����",
"������ ��������",
"��������",
"����",
"OEM",
"������"
};

// Level-2 submenu
const UCHAR _cPbActionMenu[PB_ACTION_MENU_MAX][DISPLAY_MAX_CHAR+1] = {
"���������?",
"�������������?",
"�������?",
"�������� �����?",
"������� ���?",
};

const UCHAR _cChActionMenu[CH_ACTION_MENU_MAX][DISPLAY_MAX_CHAR+1] = {
"���������?",
"�������� � �����",
"������� ���?",
};

const UCHAR _cNetworkMenu[NETWORK_MENU_MAX][DISPLAY_MAX_CHAR+1] = {
"��� IP �������.",
"IP-�����",
"����� ����",
"IP-����� �����",
"PPPoE ID",
"PPPoE PIN",
"��� DNS",
"1�� ip-����� DNS",
"2�� ip-����� DNS",
"Lay3 QoS",
"VLAN �����",
"VLAN ���������"
};

const UCHAR _cVoiceMenu[VOICE_MENU_MAX][DISPLAY_MAX_CHAR+1] = {
"��������� �����1",
"Frames per TX1",
"��������� �����2",
"Frames per TX2",
"��������� �����3",
"Frames per TX3",
"��������� �����4",
"Frames per TX4",
"��������� �����5",
"Frames per TX5",
"��������� �����6",
"Frames per TX6",
"��������� �����7",
"Frames per TX7",
"iLBC Frame Size",
"Speex rate",
"VAD",
"AGC",
"AEC",
};

#ifdef CALL_IAX2
const UCHAR _cProtocolMenu[PROTOCOL_MENU_MAX][DISPLAY_MAX_CHAR+1] = {
"IAX2 �����������",
"IAX2 ������",
"���� �������",
"����� ��������",
"�����",
"������",
"���",
"��������� ����",
"����� ���������.",
"����� ����������",
"Digit Dialing",
};
#elif defined CALL_SIP
const UCHAR _cProtocolMenu[PROTOCOL_MENU_MAX][DISPLAY_MAX_CHAR+1] = {
"SIP �����������",
"SIP ������",
"���� �������",
"SIP �����",
"Outbound Proxy",
"��������� DnsSrv",
"����� ��������",
"�����",
"������",
"���",
"��������� ����",
"RTP ����",
"����� ���������.",
"Keep Alive",
"���������� DTMF",
"DTMF Payload",
"G.726-32 Payload",
"iLBC Payload",
"Speex Payload",
"����� ����������",
"PRACK",
"�������� �� MWI",
"Proxy Require",
"����������� NAT",
"NAT ip-�����",
"STUN ������",
"STUN ����",
};
#endif

const UCHAR _cDialplanMenu[DIALPLAN_MENU_MAX][DISPLAY_MAX_CHAR+1] = {
"������� �� �����",
"�������-������",
"�������-������",
"�������-��� ���.",
"����� ��������",
"����-�����",
"��������� ������",
"������� ������",
"����� Hotline",
"������. �������.",
"������� �������.",
"Use '#' To Call",
};

const UCHAR _cSystemMenu[SYSTEM_MENU_MAX][DISPLAY_MAX_CHAR+1] = {
"��������� ������",
//"�������� �������",
"Syslog ������",
"�����. ���������",
"�����. ��������",
"�����. �������",
"Prov. ������",
"Prov. ����",
"Prov. ��������",
"SNTP ������",
"��������� ����",
"������ �����",
};

const UCHAR _cSavePhonebook[] = "����. ���.������?";
const UCHAR _cSaveSettings[] =  "����. ���������?";
const UCHAR _cConfirm[] = "Do you confirm?";
const UCHAR _cYesOrCancel[] = "<OK>|<CANCEL>";
const UCHAR _cEndOfList[] = "--�����--";
const UCHAR _cSpaces[] = "�����";
const UCHAR _cContacts[] = "���������";
const UCHAR _cRecords[] = "�������";
const UCHAR _cEnterName[] = "������� ���:";
const UCHAR _cEnterNumber[] = "������� �����:";
const UCHAR _cOKToEnter[] = "<OK> ��� �����";
const UCHAR _cError[] = "������!";
const UCHAR _cEmpty[] = "[�����]";
const UCHAR _cWriting[] = "-������ FLASH-";

const UCHAR _cCodecMark[VOICE_CODER_NUM+1][9] = {"���", "PCMU", "PCMA", "G.726-32", "GSM 6.10", "iLBC", "Speex", "G.729"};
const UCHAR _cSpeexRateMark[8][9] = {"2.15kbps", "3.95kbps", "5.95kbps", "8kbps", "11kbps", "15kbps", "18.2kbps", "24.6kbps"};
const UCHAR _cIlbcFrameMark[2][7] = {"20ms", "30ms"};
const UCHAR _cYesNoMark[2][6] = {"���", "��"};
const UCHAR _cDtmfTypeMark[3][9] = {"Inband", "RFC2833", "SIP Info"};

const UCHAR _cIPTypeMark[IP_TYPE_MAX][DISPLAY_MAX_CHAR+1] = {"����������", "DHCP", "PPPoE"};
const UCHAR _cDnsTypeMark[2][DISPLAY_MAX_CHAR+1] = {"�������", "�������������"};
const UCHAR _cNatTraversalMark[3][DISPLAY_MAX_CHAR+1] = {"���������", "STUN", "NAT IP"};
