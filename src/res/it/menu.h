
const UCHAR _cMainMenu[MAIN_MENU_MAX][DISPLAY_MAX_CHAR+1] = {
"Elenco telefono", 
"Lista chiamate",
"Impostazioni",
"Info sistema",
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
"Chiamate perse",
"Chiamate ricev.",
"Chiamate fatte",
};

const UCHAR _cSettingsMenu[SETTINGS_MENU_MAX][DISPLAY_MAX_CHAR+1] = {
"Rete",
"Voce",
"Protocollo",
"Opz. chiamata",
"Sistema",
"Load Defaults",
"Store Defaults",
};

const UCHAR _cInfoMenu[INFO_MENU_MAX][DISPLAY_MAX_CHAR+1] = {
"Indirizzo IP",
"Numero telefono",
"indirizzo MAC",
"Modello",
"Protocollo",
"Lingua",
"Tag OEM",
"Versione"
};

// Level-2 submenu
const UCHAR _cPbActionMenu[PB_ACTION_MENU_MAX][DISPLAY_MAX_CHAR+1] = {
"Chiamare?",
"Modificare?",
"Cancellare?",
"Aggiungere?",
"Elimina tutto?",
};

const UCHAR _cChActionMenu[CH_ACTION_MENU_MAX][DISPLAY_MAX_CHAR+1] = {
"Chiamare?",
"Agg. in elenco?",
"Elimina tutto?",
};

const UCHAR _cNetworkMenu[NETWORK_MENU_MAX][DISPLAY_MAX_CHAR+1] = {
"Assegnazione IP",
"Indirizzo IP",
"Maschera rete",
"Gateway IP",
"PPPoE ID",
"PPPoE PIN",
"Tipo DNS",
"DNS primario",
"DNS secondario",
"QoS - Layer 3",
"Tag VLAN",
"Priorit?VLAN"
};

const UCHAR _cVoiceMenu[VOICE_MENU_MAX][DISPLAY_MAX_CHAR+1] = {
"Codec voce 1",
"Pacchetti TX 1",
"Codec voce 2",
"Pacchetti TX 2",
"Codec voce 3",
"Pacchetti TX 3",
"Codec voce 4",
"Pacchetti TX 4",
"Codec voce 5",
"Pacchetti TX 5",
"Codec voce 6",
"Pacchetti TX 6",
"Codec voce 7",
"Pacchetti TX 7",
"Lunghezza iLBC",
"Banda Speex",
"VAD",
"AGC",
"AEC",
};

#ifdef CALL_IAX2
const UCHAR _cProtocolMenu[PROTOCOL_MENU_MAX][DISPLAY_MAX_CHAR+1] = {
"Registrazione",
"Server IAX2",
"Porta server",
"ID utente",
"ID per autent.",
"PIN per autent.",
"Nome completo",
"Porta locale",
"Rinnovo Reg.",
"Numero messaggi",
"Dialplan remoto",
};
#elif defined CALL_SIP
const UCHAR _cProtocolMenu[PROTOCOL_MENU_MAX][DISPLAY_MAX_CHAR+1] = {
"Registrazione",
"Server SIP",
"Porta server",
"Dominio SIP",
"Proxy uscita",
"Usa server DNS",
"ID utente",
"ID per autent.",
"PIN per autent.",
"Nome completo",
"Porta locale",
"Porta RTP",
"Rinnovo Reg.",
"Keep Alive",
"Invio DTMF",
"Payload DTMF",
"Payload G.726",
"Payload iLBC",
"Payload Speex",
"Numero messaggi",
"PRACK",
"Subsrcibe MWI",
"Proxy Require",
"NAT Traversal",
"IP del NAT",
"Server STUN",
"Porta STUN",
};
#endif

const UCHAR _cDialplanMenu[DIALPLAN_MENU_MAX][DISPLAY_MAX_CHAR+1] = {
"Devia vs numero",
"Devia sempre",
"Devia occupato",
"Devia no risp.",
"No-Answer Time",
"Auto rispondi",
"Metti in attesa",
"Digita Prefisso",
"Numero Hotline",
"Usa Digitmap",
"Timeout Digitmap",
"Use '#' To Call",
};

const UCHAR _cSystemMenu[SYSTEM_MENU_MAX][DISPLAY_MAX_CHAR+1] = {
"Ammin Password",
"Syslog IP",
"Vol. microfono",
"Vol. ricevitore",
"Vol. esterno",
"Server provis.",
"Porta provision",
"Intervallo prov",
"Server SNTP",
"Fuso orario",
"Usa ora legale",
};

const UCHAR _cSavePhonebook[] = "Salva Elenco?";
const UCHAR _cSaveSettings[] = "Salva config?";
const UCHAR _cConfirm[] = "Do you confirm?";
const UCHAR _cYesOrCancel[] = "<OK>|<ANNULLA>";
const UCHAR _cEndOfList[] = "--Fine lista--";
const UCHAR _cSpaces[] = "spazi";
const UCHAR _cContacts[] = "contatti";
const UCHAR _cRecords[] = "record";
const UCHAR _cEnterName[] = "Digita nome:";
const UCHAR _cEnterNumber[] = "Digita numero:";
const UCHAR _cOKToEnter[] = "<OK> per entrare";
const UCHAR _cError[] = "ERRORE!";
const UCHAR _cEmpty[] = "[vuoto]";
const UCHAR _cWriting[] = "-MEMORIZZANDO-";

const UCHAR _cIPTypeMark[IP_TYPE_MAX][DISPLAY_MAX_CHAR+1] = {"Statico", "DHCP", "PPPoE"};
const UCHAR _cDnsTypeMark[2][DISPLAY_MAX_CHAR+1] = {"Manuale", "Automatico"};
const UCHAR _cCodecMark[VOICE_CODER_NUM+1][9] = {"Nessuno", "PCMU", "PCMA", "G.726-32", "GSM 6.10", "iLBC", "Speex", "G.729"};
const UCHAR _cSpeexRateMark[8][9] = {"2.15kbps", "3.95kbps", "5.95kbps", "8kbps", "11kbps", "15kbps", "18.2kbps", "24.6kbps"};
const UCHAR _cIlbcFrameMark[2][7] = {"20ms", "30ms"};
const UCHAR _cYesNoMark[2][6] = {"No", "Si"};
const UCHAR _cDtmfTypeMark[3][9] = {"In banda", "RFC2833", "Info SIP"};
const UCHAR _cNatTraversalMark[3][DISPLAY_MAX_CHAR+1] = {"No", "STUN", "NAT IP"};
