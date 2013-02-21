
const UCHAR _cMainMenu[MAIN_MENU_MAX][DISPLAY_MAX_CHAR+1] = {
"Agenda", 
"Reg. de chamadas",
"Configurações",
"Info. do sist.",
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
"Cham. perdidas",
"Cham. recebidas",
"Núm. discados",
};

const UCHAR _cSettingsMenu[SETTINGS_MENU_MAX][DISPLAY_MAX_CHAR+1] = {
"Rede",
"Voz",
"Protocolo",
"Plano de disc.",
"Sistema",
"Config. padrão",
"Store Defaults",
};

const UCHAR _cInfoMenu[INFO_MENU_MAX][DISPLAY_MAX_CHAR+1] = {
"Endereço IP",
"Número. do tel.",
"Endereço MAC",
"Modelo do tel.",
"Protocolo",
"Linguagem",
"OEM Tag",
"Versão"
};

// Level-2 submenu
const UCHAR _cPbActionMenu[PB_ACTION_MENU_MAX][DISPLAY_MAX_CHAR+1] = {
"Chamar?",
"Editar?",
"Apagar?",
"Adicionar novo?",
"Apagar todos?",
};

const UCHAR _cChActionMenu[CH_ACTION_MENU_MAX][DISPLAY_MAX_CHAR+1] = {
"Chamar?",
"Adic. ?agenda?",
"Apagar todos?",
};

const UCHAR _cNetworkMenu[NETWORK_MENU_MAX][DISPLAY_MAX_CHAR+1] = {
"Configurar IP",
"Endereço IP",
"Másc. subrede",
"Gateway",
"PPPoE login",
"PPPoE senha",
"Configurar DNS",
"DNS IP1",
"DNS IP2",
"DSCP QoS",
"VLAN Tag",
"VLAN Prioridade"
};

const UCHAR _cVoiceMenu[VOICE_MENU_MAX][DISPLAY_MAX_CHAR+1] = {
"Codec 1",
"Quadros p/ TX 1",
"Codec 2",
"Quadros p/ TX 2",
"Voice Codec3",
"Quadros p/ TX3",
"Voice Codec4",
"Quadros p/ TX4",
"Voice Codec5",
"Quadros por TX5",
"Voice Codec6",
"Quadros p/ TX6",
"Voice Codec7",
"Quadros p/ TX7",
"Tam. quad. iLBC",
"Taxa Speex",
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
"Registrar SIP",
"Servidor SIP",
"Porta do serv.",
"Domínio SIP",
"Proxy de saída",
"DNS SRV",
"ID usuário" ,
"ID autorização",
"Senha",
"Nome de usuário",
"Porta local",
"Porta RTP",
"Expiração do reg.",
"Keep Alive",
"Enviar DTMF",
"DTMF Payload",
"G.726 Payload",
"iLBC Payload",
"Speex Payload",
"Num. Corr. voz",
"PRACK",
"Subescrever MWI",
"Requer proxy",
"NAT Traversal",
"NAT IP",
"Servidor STUN",
"Porta STUN",
};
#endif

const UCHAR _cDialplanMenu[DIALPLAN_MENU_MAX][DISPLAY_MAX_CHAR+1] = {
"Desviar para",
"Desviar sempre",
"Desviar ocupado",
"Desv.não atende",
"Tempo n. atende",
"Atender auto.",
"Cham. em espera",
"Pref. discagem",
"Número hotline",
"Map. dígitos",
"Tempo limite",
"# p/ discar",
};

const UCHAR _cSystemMenu[SYSTEM_MENU_MAX][DISPLAY_MAX_CHAR+1] = {
"Senha Admin",
"IP Syslog",
"Entr. monofone",
"Saída monofone",
"Saída viva-voz",
"Serv. de prov.",
"Porta de pro.",
"Interv. prov.",
"Servidor SNTP",
"Fuso horário",
"Hor. de verão",
};

const UCHAR _cSavePhonebook[] = "Salvar agenda?";
const UCHAR _cSaveSettings[] = "Salvar config.?";
const UCHAR _cConfirm[] = "Do you confirm?";
const UCHAR _cYesOrCancel[] = "<OK>|<CANCELAR>";
const UCHAR _cEndOfList[] = "--Fim da lista--";
const UCHAR _cSpaces[] = "espaços";
const UCHAR _cContacts[] = "contatos";
const UCHAR _cRecords[] = "registros";
const UCHAR _cEnterName[] = "Nome:";
const UCHAR _cEnterNumber[] = "Número:";
const UCHAR _cOKToEnter[] = "<OK> para entrar";
const UCHAR _cError[] = "ERRO!";
const UCHAR _cEmpty[] = "[vazio]";
const UCHAR _cWriting[] = "-GRAVANDO FLASH-";

const UCHAR _cCodecMark[VOICE_CODER_NUM+1][9] = {"Null", "PCMU", "PCMA", "G.726-32", "GSM 6.10", "iLBC", "Speex", "G.729"};
const UCHAR _cSpeexRateMark[8][9] = {"2.15kbps", "3.95kbps", "5.95kbps", "8kbps", "11kbps", "15kbps", "18.2kbps", "24.6kbps"};
const UCHAR _cIlbcFrameMark[2][7] = {"20ms", "30ms"};
const UCHAR _cYesNoMark[2][6] = {"No", "Yes"};
const UCHAR _cDtmfTypeMark[3][9] = {"Inband", "RFC2833", "SIP Info"};

const UCHAR _cIPTypeMark[IP_TYPE_MAX][DISPLAY_MAX_CHAR+1] = {"Estático", "DHCP", "PPPoE"};
const UCHAR _cDnsTypeMark[2][DISPLAY_MAX_CHAR+1] = {"Manual", "Automático"};
const UCHAR _cNatTraversalMark[3][DISPLAY_MAX_CHAR+1] = {"Desativado", "STUN", "NAT IP"};
