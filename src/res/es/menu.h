
const UCHAR _cMainMenu[MAIN_MENU_MAX][DISPLAY_MAX_CHAR+1] = {
"Agenda", 
"Hist.de Llamadas",
"Configuración",
"Información",
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
"Llam.Perdidas",
"Llam.Recibidas",
"Llam.Realizadas",
};

const UCHAR _cSettingsMenu[SETTINGS_MENU_MAX][DISPLAY_MAX_CHAR+1] = {
"Red",
"Voz",
"Protocolo",
"Dialplan",
"Sistema",
"Load Defaults",
"Store Defaults",
};

const UCHAR _cInfoMenu[INFO_MENU_MAX][DISPLAY_MAX_CHAR+1] = {
"Direccion IP",
"Num.de teléfono",
"MAC Address",
"Modelo de tel.",
"Protocolo",
"Lenguage",
"OEM Tag",
"Version"
};

// Level-2 submenu
const UCHAR _cPbActionMenu[PB_ACTION_MENU_MAX][DISPLAY_MAX_CHAR+1] = {
"Llamar?",
"Editar?",
"Borrar?",
"Agregar nuevo?",
"Borrar todo?",
};

const UCHAR _cChActionMenu[CH_ACTION_MENU_MAX][DISPLAY_MAX_CHAR+1] = {
"Llamar ?",
"Agregar a agenda",
"Borrar todo?",
};

const UCHAR _cNetworkMenu[NETWORK_MENU_MAX][DISPLAY_MAX_CHAR+1] = {
"Tipo de IP",
"IP Address",
"Subnet Mask",
"Router IP",
"PPPoE ID",
"PPPoE PIN",
"Tipo de DNS",
"DNS IP1",
"DNS IP2",
"Lay3 QoS",
"VLAN Tag",
"Prioridad VLAN"
};

const UCHAR _cVoiceMenu[VOICE_MENU_MAX][DISPLAY_MAX_CHAR+1] = {
"Voice Codec1",
"Frames por TX1",
"Voice Codec2",
"Frames por TX2",
"Voice Codec3",
"Frames por TX3",
"Voice Codec4",
"Frames por TX4",
"Voice Codec5",
"Frames por TX5",
"Voice Codec6",
"Frames por TX6",
"Voice Codec7",
"Frames por TX7",
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
"Dominio SIP",
"Outbound Proxy",
"DNS SRV",
"User ID",
"Auth ID",
"Auth PIN",
"Nombre de usuario",
"Puerto Local",
"Puerto RTP",
"Reg Expire",
"Keep Alive",
"Enviar DTMF",
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
"Desv.a número",
"Desv.siempre",
"Desv.si ocupado",
"Desv.sin atender",
"Sin-Answer Time",
"Auto atenter",
"Call Waiting",
"Dial Prefix",
"Numero preferido",
"Usar Digitmap",
"Digitmap Timeout",
"Use '#' To Call",
};

const UCHAR _cSystemMenu[SYSTEM_MENU_MAX][DISPLAY_MAX_CHAR+1] = {
"Admin Password",
"Syslog IP",
"Handset Input",
"Handset Output",
"Speaker Output",
"Prov. Server",
"Prov. Port",
"Prov. Interval",
"SNTP Server",
"Time Zone",
"Daylight",
};


const UCHAR _cSavePhonebook[] = "Grabar agenda?";
const UCHAR _cSaveSettings[] = "Grabar Settings?";
const UCHAR _cConfirm[] = "Do you confirm?";
const UCHAR _cYesOrCancel[] = "<OK>|<CANCELAR>";
const UCHAR _cEndOfList[] = "--Fin de Lista--";
const UCHAR _cSpaces[] = "espacios";
const UCHAR _cContacts[] = "contactos";
const UCHAR _cRecords[] = "registros";
const UCHAR _cEnterName[] = "Ingresa nombre:";
const UCHAR _cEnterNumber[] = "Ingresa número:";
const UCHAR _cOKToEnter[] = "<OK> confirma";
const UCHAR _cError[] = "ERROR!";
const UCHAR _cEmpty[] = "[empty]";
const UCHAR _cWriting[] = "-Escribiendo-"; //Writing FLASH

const UCHAR _cCodecMark[VOICE_CODER_NUM+1][9] = {"Null", "PCMU", "PCMA", "G.726-32", "GSM 6.10", "iLBC", "Speex", "G.729"};
const UCHAR _cSpeexRateMark[8][9] = {"2.15kbps", "3.95kbps", "5.95kbps", "8kbps", "11kbps", "15kbps", "18.2kbps", "24.6kbps"};
const UCHAR _cIlbcFrameMark[2][7] = {"20ms", "30ms"};
const UCHAR _cYesNoMark[2][6] = {"No", "Si"};
const UCHAR _cDtmfTypeMark[3][9] = {"Inband", "RFC2833", "SIP Info"};

const UCHAR _cIPTypeMark[IP_TYPE_MAX][DISPLAY_MAX_CHAR+1] = {"Estática", "DHCP", "PPPoE"};
const UCHAR _cDnsTypeMark[2][DISPLAY_MAX_CHAR+1] = {"Manual", "Automática"};
const UCHAR _cNatTraversalMark[3][DISPLAY_MAX_CHAR+1] = {"Deshabilitado", "STUN", "NAT IP"};

