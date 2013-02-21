
const UCHAR _cMainMenu[MAIN_MENU_MAX][DISPLAY_MAX_CHAR+1] = {
"Annuaire", 
"Historique",
"Paramétrage",
"Informations",
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
"Manqués",
"Reçus",
"Composés",
};

const UCHAR _cSettingsMenu[SETTINGS_MENU_MAX][DISPLAY_MAX_CHAR+1] = {
"Réseau",
"Voix",
"Protocole",
"Numérotation",
"Système",
"Load Defaults",
"Store Defaults",
};

const UCHAR _cInfoMenu[INFO_MENU_MAX][DISPLAY_MAX_CHAR+1] = {
"Adresse IP",
"Numéro assigné",
"Adresse MAC",
"Modèle",
"Protocole",
"Langage",
"Tag OEM",
"Version"
};

// Level-2 submenu
const UCHAR _cPbActionMenu[PB_ACTION_MENU_MAX][DISPLAY_MAX_CHAR+1] = {
"Appeler ?",
"Modifier ?",
"Effacer ?",
"Ajouter ?",
"Tout effacer ?",
};

const UCHAR _cChActionMenu[CH_ACTION_MENU_MAX][DISPLAY_MAX_CHAR+1] = {
"Appeler ?",
"Ajout annuaire ?",
"Tout effacer ?",
};

const UCHAR _cNetworkMenu[NETWORK_MENU_MAX][DISPLAY_MAX_CHAR+1] = {
"Assignation IP",
"Addresse IP",
"Masque réseau",
"IP Passerelle",
"PPPoE ID",
"PPPoE PIN",
"Type DNS",
"IP Serveur DNS 1",
"IP Serveur DNS 2",
"QoS - Layer 3",
"VLAN Tag",
"VLAN Priorité"
};

const UCHAR _cVoiceMenu[VOICE_MENU_MAX][DISPLAY_MAX_CHAR+1] = {
"Codec voix 1",
"Frames per TX1",
"Codec voix 2",
"Frames per TX2",
"Codec voix 3",
"Frames per TX3",
"Codec voix 4",
"Frames per TX4",
"Codec voix 5",
"Frames per TX5",
"Codec voix 6",
"Frames per TX6",
"Codec voix 7",
"Frames per TX7",
"iLBC frame size",
"Speex rate",
"Activ. a la voix",
"Gain automatique",
"Annulation echo",
};

#ifdef CALL_IAX2
const UCHAR _cProtocolMenu[PROTOCOL_MENU_MAX][DISPLAY_MAX_CHAR+1] = {
"Activer IAX2",
"Serveur IAX2",
"Port serveur",
"Utilisateur",
"Identifiant",
"Mot de passe",
"Nom complet",
"Port local",
"Expiration auth.",
"Num. messagerie",
"Plan num. serv.",
};
#elif defined CALL_SIP
const UCHAR _cProtocolMenu[PROTOCOL_MENU_MAX][DISPLAY_MAX_CHAR+1] = {
"Activer SIP",
"Serveur SIP",
"Port serveur",
"Domaine SIP",
"Proxy de sortie",
"DNS SRV",
"Utilisateur",
"Identifiant",
"Mot de passe",
"Nom complet",
"Port local",
"Port RTP",
"Expiration auth.",
"Keep Alive",
"Envoi DTMF",
"DTMF Payload",
"G.726-32 Payload",
"iLBC Payload",
"Speex Payload",
"Num. messagerie",
"PRACK",
"Subsrcibe MWI",
"Proxy requis",
"NAT Traversal",
"IP NAT",
"Serveur STUN",
"Port STUN",
};
#endif

const UCHAR _cDialplanMenu[DIALPLAN_MENU_MAX][DISPLAY_MAX_CHAR+1] = {
"Num. transfert",
"Tout transférer",
"Trans. si occupé",
"Trans. si absent",
"Délai pr absence",
"Réponse auto",
"Mise en attente",
"Préfixe",
"Numéro support",
"Plan num. local",
"Délai numérot.",
"Use '#' To Call",
};

const UCHAR _cSystemMenu[SYSTEM_MENU_MAX][DISPLAY_MAX_CHAR+1] = {
"Mot de passe",
//"Activer Debug",
"Syslog IP",
"Volume micro",
"Volume écouteur",
"Vol. main libre",
"Serveur approv.",
"Port appprov.",
"Intervalle appr.",
"Serveur SNTP",
"Faisceau horaire",
"Heure d'été auto",
};

const UCHAR _cSavePhonebook[] = "Sauve annuaire ?";
const UCHAR _cSaveSettings[] = "Sauve config ?";
const UCHAR _cConfirm[] = "Do you confirm?";
const UCHAR _cYesOrCancel[] = "<OK>|<CANCEL>";
const UCHAR _cEndOfList[] = "--Fin de liste--";
const UCHAR _cSpaces[] = "libres";
const UCHAR _cContacts[] = "contacts";
const UCHAR _cRecords[] = "appels";
const UCHAR _cEnterName[] = "Entrer nom :";
const UCHAR _cEnterNumber[] = "Entrer numero :";
const UCHAR _cOKToEnter[] = "<OK> pour entrer";
const UCHAR _cError[] = "ERREUR !";
const UCHAR _cEmpty[] = "[vide]";
const UCHAR _cWriting[] = "- MAJ MEMOIRE -";

const UCHAR _cCodecMark[VOICE_CODER_NUM+1][9] = {"Aucun", "PCMU", "PCMA", "G.726-32", "GSM 6.10", "iLBC", "Speex", "G.729"};
const UCHAR _cSpeexRateMark[8][9] = {"2.15kbps", "3.95kbps", "5.95kbps", "8kbps", "11kbps", "15kbps", "18.2kbps", "24.6kbps"};
const UCHAR _cIlbcFrameMark[2][7] = {"20ms", "30ms"};
const UCHAR _cYesNoMark[2][6] = {"Non", "Oui"};
const UCHAR _cDtmfTypeMark[3][9] = {"Inband", "RFC2833", "SIP Info"};

const UCHAR _cIPTypeMark[IP_TYPE_MAX][DISPLAY_MAX_CHAR+1] = {"Static", "DHCP", "PPPoE"};
const UCHAR _cDnsTypeMark[2][DISPLAY_MAX_CHAR+1] = {"Manuel", "Auto"};
const UCHAR _cNatTraversalMark[3][DISPLAY_MAX_CHAR+1] = {"Désactivé", "STUN", "NAT IP"};
