
const UCHAR _cMainMenu[MAIN_MENU_MAX][DISPLAY_MAX_CHAR+1] = {
"Contacte", 
"Jurnal",
"Set�ri",
"Detalii",
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
"Apel. Nepreluate",
"Apeluri Primite",
"Numere Formate",
};

const UCHAR _cSettingsMenu[SETTINGS_MENU_MAX][DISPLAY_MAX_CHAR+1] = {
"Re�ea",
"Voce",
"Protocol",
"Plan Numerotare",
"Sistem",
"Load Defaults",
"Store Defaults",
};

const UCHAR _cInfoMenu[INFO_MENU_MAX][DISPLAY_MAX_CHAR+1] = {
"Adresa IP",
"Num�rul Telefon.",
"Adresa MAC",
"Modelul Telefon.",
"Protocol",
"Limba",
"OEM",
"Versiune"
};

// Level-2 submenu
const UCHAR _cPbActionMenu[PB_ACTION_MENU_MAX][DISPLAY_MAX_CHAR+1] = {
"Apelare?",
"Editare?",
"�tergere?",
"Ad�ugare?",
"�tergere toate?",
};

const UCHAR _cChActionMenu[CH_ACTION_MENU_MAX][DISPLAY_MAX_CHAR+1] = {
"Apel?",
"Ad�ug. Contacte?",
"�tergere toate?",
};

const UCHAR _cNetworkMenu[NETWORK_MENU_MAX][DISPLAY_MAX_CHAR+1] = {
"Tip IP",
"Adresa IP",
"Subre�ea",
"Router-ul IP",
"PPPoE ID",
"PPPoE PIN",
"Tip DNS",
"DNS IP1",
"DNS IP2",
"Lay3 QoS",
"VLAN",
"Prioritatea VLAN"
};

const UCHAR _cVoiceMenu[VOICE_MENU_MAX][DISPLAY_MAX_CHAR+1] = {
"Codec Vocal 1",
"Frames per TX1",
"Codec Vocal 2",
"Frames per TX2",
"Codec Vocal 3",
"Frames per TX3",
"Codec Vocal 4",
"Frames per TX4",
"Codec Vocal 5",
"Frames per TX5",
"Codec Vocal 6",
"Frames per TX6",
"Codec Vocal 7",
"Frames per TX7",
"iLBC Frame Size",
"Speex Rata",
"VAD",
"AGC",
"AEC",
};

#ifdef CALL_IAX2
const UCHAR _cProtocolMenu[PROTOCOL_MENU_MAX][DISPLAY_MAX_CHAR+1] = {
"IAX2 �nregistr.",
"IAX2 Server",
"Server Port",
"Num�r Telefon",
"Utilizator",
"Parol�",
"Numele",
"Port Local",
"Timpul registr.",
"Num. Mes.Vocale",
"Digit Dialing",
};
#elif defined CALL_SIP
const UCHAR _cProtocolMenu[PROTOCOL_MENU_MAX][DISPLAY_MAX_CHAR+1] = {
"SIP �nregistrare",
"SIP Server",
"Server Port",
"SIP Domeniu",
"Outbound Proxy",
"DNS SRV",
"Num�r Telefon",
"Utilizator",
"Parol�",
"Numele",
"Port Local",
"Port RTP",
"Timpul registr.",
"Keep Alive",
"Trimite DTMF",
"DTMF Payload",
"G.726-32 Payload",
"iLBC Payload",
"Speex Payload",
"Num. Mes.Vocale",
"PRACK",
"Subsrcibe MWI",
"Proxy Require",
"Traversare NAT",
"NAT IP",
"Server STUN",
"Port STUN",
};
#endif

const UCHAR _cDialplanMenu[DIALPLAN_MENU_MAX][DISPLAY_MAX_CHAR+1] = {
"Deviere la Num.",
"Deviere Toate",
"Deviere Ocupat",
"Deviere Nu R�sp.",
"No-Answer Timp",
"Auto R�spuns",
"Apel A�teptare",
"Prefix Apel",
"Num�r Hotline",
"Plan Numerotare",
"Plan Num Timeout",
"Use '#' To Call",
};

const UCHAR _cSystemMenu[SYSTEM_MENU_MAX][DISPLAY_MAX_CHAR+1] = {
"Parol� Admin.",
//"Folosire Debug",
"Syslog IP",
"Volum Microfon",
"Volum",
"Volum Difuzor",
"Auto-Conf Server",
"Auto-Conf Port",
"Auto-Conf Interval",
"SNTP Server",
"Fus Orar",
"Daylight",
};

const UCHAR _cSavePhonebook[] = "Salv. Contacte?";
const UCHAR _cSaveSettings[] = "Salv. Set�rilor?";
const UCHAR _cConfirm[] = "Do you confirm?";
const UCHAR _cYesOrCancel[] = "<OK>|<Anulare>";
const UCHAR _cEndOfList[] = "-- Sf�r�it --";
const UCHAR _cSpaces[] = "spa�ii";
const UCHAR _cContacts[] = "contacte";
const UCHAR _cRecords[] = "�nregistr�ri";
const UCHAR _cEnterName[] = "Numele:";
const UCHAR _cEnterNumber[] = "Num�rul:";
const UCHAR _cOKToEnter[] = "<OK> - selectare";
const UCHAR _cError[] = "ERROARE!";
const UCHAR _cEmpty[] = "[lips�]";
const UCHAR _cWriting[] = "-WRITING FLASH-";

const UCHAR _cCodecMark[VOICE_CODER_NUM+1][9] = {"Lips�", "PCMU", "PCMA", "G.726-32", "GSM 6.10", "iLBC", "Speex", "G.729"};
const UCHAR _cSpeexRateMark[8][9] = {"2.15kbps", "3.95kbps", "5.95kbps", "8kbps", "11kbps", "15kbps", "18.2kbps", "24.6kbps"};
const UCHAR _cIlbcFrameMark[2][7] = {"20ms", "30ms"};
const UCHAR _cYesNoMark[2][6] = {"Nu", "Da"};
const UCHAR _cDtmfTypeMark[3][9] = {"Inband", "RFC2833", "SIP Info"};

const UCHAR _cIPTypeMark[IP_TYPE_MAX][DISPLAY_MAX_CHAR+1] = {"Static", "DHCP", "PPPoE"};
const UCHAR _cDnsTypeMark[2][DISPLAY_MAX_CHAR+1] = {"Manual", "Automatic"};
const UCHAR _cNatTraversalMark[3][DISPLAY_MAX_CHAR+1] = {"Dezactivat", "STUN", "NAT IP"};
