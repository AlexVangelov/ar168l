
const UCHAR _cMainMenu[MAIN_MENU_MAX][DISPLAY_MAX_CHAR+1] = {
"Phone Book", 
"Call History",
"Phone Settings",
"System Info",
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
"Missed Calls",
"Received Calls",
"Dialed Numbers",
};

const UCHAR _cSettingsMenu[SETTINGS_MENU_MAX][DISPLAY_MAX_CHAR+1] = {
"Network",
"Voice",
"Protocol",
"Dialplan",
"System",
"Load Defaults",
"Store Defaults",
};

const UCHAR _cInfoMenu[INFO_MENU_MAX][DISPLAY_MAX_CHAR+1] = {
"IP Address",
"Phone Number",
"MAC Address",
"Phone Model",
"Protocol",
"Language",
"OEM Tag",
"Version"
};

// Level-2 submenu
const UCHAR _cPbActionMenu[PB_ACTION_MENU_MAX][DISPLAY_MAX_CHAR+1] = {
"Call?",
"Edit?",
"Delete?",
"Add Newly?",
"Delete All?",
};

const UCHAR _cChActionMenu[CH_ACTION_MENU_MAX][DISPLAY_MAX_CHAR+1] = {
"Call ?",
"Add to Phonebook",
"Delete All?",
};

const UCHAR _cNetworkMenu[NETWORK_MENU_MAX][DISPLAY_MAX_CHAR+1] = {
"IP Type",
"IP Address",
"Subnet Mask",
"Router IP",
"PPPoE ID",
"PPPoE PIN",
"DNS Type",
"DNS IP1",
"DNS IP2",
"Lay3 QoS",
"VLAN Tag",
"VLAN Priority"
};

const UCHAR _cVoiceMenu[VOICE_MENU_MAX][DISPLAY_MAX_CHAR+1] = {
"Voice Codec1",
"Frames per TX1",
"Voice Codec2",
"Frames per TX2",
"Voice Codec3",
"Frames per TX3",
"Voice Codec4",
"Frames per TX4",
"Voice Codec5",
"Frames per TX5",
"Voice Codec6",
"Frames per TX6",
"Voice Codec7",
"Frames per TX7",
"iLBC Frame Size",
"Speex Rate",
"VAD",
"AGC",
"AEC",
};

#ifdef CALL_IAX2
const UCHAR _cProtocolMenu[PROTOCOL_MENU_MAX][DISPLAY_MAX_CHAR+1] = {
"IAX2 Register",
"IAX2 Server",
"Server Port",
"User ID",
"Auth ID",
"Auth PIN",
"User Name",
"Local Port",
"Reg Expire",
"Msg Number",
"Digit Dialing",
};
#elif defined CALL_SIP
const UCHAR _cProtocolMenu[PROTOCOL_MENU_MAX][DISPLAY_MAX_CHAR+1] = {
"SIP Register",
"SIP Server",
"Server Port",
"SIP Domain",
"Outbound Proxy",
"DNS SRV",
"User ID",
"Auth ID",
"Auth PIN",
"User Name",
"Local Port",
"RTP Port",
"Reg Expire",
"Keep Alive",
"Send DTMF",
"DTMF Payload",
"G.726-32 Payload",
"iLBC Payload",
"Speex Payload",
"Msg Number",
"PRACK",
"Subsrcibe MWI",
"Proxy Require",
"NAT Traversal",
"NAT IP",
"STUN Server",
"STUN Port",
};
#endif

const UCHAR _cDialplanMenu[DIALPLAN_MENU_MAX][DISPLAY_MAX_CHAR+1] = {
"Fwd-to Number",
"Fwd Always",
"Fwd Busy",
"Fwd No Answer",
"No-Answer Time",
"Auto Answer",
"Call Waiting",
"Dial Prefix",
"Hotline Number",
"Use Digitmap",
"Digitmap Timeout",
"Use '#' To Call",
};

const UCHAR _cSystemMenu[SYSTEM_MENU_MAX][DISPLAY_MAX_CHAR+1] = {
"Admin Password",
"Syslog IP",
"Handset Input",
"Handset Output",
"Speaker Output",
"Ring Volume",
"Prov. Server",
"Prov. Port",
"Prov. Interval",
"SNTP Server",
"Time Zone",
"Daylight",
};

const UCHAR _cSavePhonebook[] = "Save Phonebook?";
const UCHAR _cSaveSettings[] = "Save Settings?";
const UCHAR _cConfirm[] = "Do you confirm?";
const UCHAR _cYesOrCancel[] = "<OK>|<CANCEL>";
const UCHAR _cEndOfList[] = "--End of List--";
const UCHAR _cSpaces[] = "spaces";
const UCHAR _cContacts[] = "contacts";
const UCHAR _cRecords[] = "records";
const UCHAR _cEnterName[] = "Enter name:";
const UCHAR _cEnterNumber[] = "Enter number:";
const UCHAR _cOKToEnter[] = "<OK> to enter";
const UCHAR _cError[] = "ERROR!";
const UCHAR _cEmpty[] = "[empty]";
const UCHAR _cWriting[] = "-WRITING FLASH-";

const UCHAR _cCodecMark[VOICE_CODER_NUM+1][9] = {"Null", "PCMU", "PCMA", "G.726-32", "GSM 6.10", "iLBC", "Speex", "G.729"};
const UCHAR _cSpeexRateMark[8][9] = {"2.15kbps", "3.95kbps", "5.95kbps", "8kbps", "11kbps", "15kbps", "18.2kbps", "24.6kbps"};
const UCHAR _cIlbcFrameMark[2][7] = {"20ms", "30ms"};
const UCHAR _cYesNoMark[2][6] = {"No", "Yes"};
const UCHAR _cDtmfTypeMark[3][9] = {"Inband", "RFC2833", "SIP Info"};

const UCHAR _cIPTypeMark[IP_TYPE_MAX][DISPLAY_MAX_CHAR+1] = {"Static", "DHCP", "PPPoE"};
const UCHAR _cDnsTypeMark[2][DISPLAY_MAX_CHAR+1] = {"Manual", "Automatic"};
const UCHAR _cNatTraversalMark[3][DISPLAY_MAX_CHAR+1] = {"Disabled", "STUN", "NAT IP"};
