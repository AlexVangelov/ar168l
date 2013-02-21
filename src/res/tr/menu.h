const UCHAR _cMainMenu[MAIN_MENU_MAX][DISPLAY_MAX_CHAR+1] = {
"Arama Kayd",
"Telefon Ayarlar",
"Telefon Defteri",
"Sistem Bilgisi",
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
"Cevapsýz Arama",
"Gelen Arama",
"Aranan Numaralar",
};

const UCHAR _cSettingsMenu[SETTINGS_MENU_MAX][DISPLAY_MAX_CHAR+1] = {
"Baðlant",
"Ses",
"Protokol",
"Arama",
"Sistem",
"Fabrika Ayar. Dön",
"Fabrika Ayar. Ata",
};

const UCHAR _cInfoMenu[INFO_MENU_MAX][DISPLAY_MAX_CHAR+1] = {
"IP Adres",
"Telefon No",
"MAC Adres",
"Telefon Model",
"Protokol",
"Dil",
"OEM Tag",
"Versiyon"
};

// Level-2 submenu
const UCHAR _cPbActionMenu[PB_ACTION_MENU_MAX][DISPLAY_MAX_CHAR+1] = {
"Ara?",
"Düzenle?",
"Sil?",
"Yeni Kayýt?",
"Hepsini Sil?",
};

const UCHAR _cChActionMenu[CH_ACTION_MENU_MAX][DISPLAY_MAX_CHAR+1] = {
"Ara ?",
"Rehbere Ekle ?",
"Hepsini Sil ?",
};

const UCHAR _cNetworkMenu[NETWORK_MENU_MAX][DISPLAY_MAX_CHAR+1] = {
"IP Tipi",
"IP Adresi",
"Alt A?Maskesi",
"Router IP",
"PPPoE ID",
"PPPoE PIN",
"DNS Tipi",
"DNS IP1",
"DNS IP2",
"Lay3 QoS",
"VLAN Etiket",
"VLAN öncelik"
};

const UCHAR _cVoiceMenu[VOICE_MENU_MAX][DISPLAY_MAX_CHAR+1] = {
"Ses Kodek1",
"Frames/TX1",
"Ses Kodek2",
"Frames/TX2",
"Ses Kodek3",
"Frames/TX3",
"Ses Kodek4",
"Frames/TX4",
"Ses Kodek5",
"Frames/TX5",
"Ses Kodek6",
"Frames/TX6",
"Ses Kodek7",
"Frames/TX7",
"iLBC Frame Size",
"Speex Oran",
"VAD",
"AGC",
"AEC",
};

#ifdef CALL_IAX2
const UCHAR _cProtocolMenu[PROTOCOL_MENU_MAX][DISPLAY_MAX_CHAR+1] = {
"IAX2 Kayd",
"IAX2 Sunucusu",
"Sunucu Portu",
"Kullanýc?ID",
"Servis ID",
"Servis PiN",
"Kullanýc?Ýsmi",
"Yerel Port",
"Kayýt Sonu",
"Mesaj Servisi No",
"Digit Map",
};
#elif defined CALL_SIP
const UCHAR _cProtocolMenu[PROTOCOL_MENU_MAX][DISPLAY_MAX_CHAR+1] = {
"SIP Kayd",
"SIP Sunucusu",
"Sunucu Portu",
"SIP Alan",
"Dýþ Proxy",
"DNS SRV",
"Kullanýc?ID",
"Servis ID",
"Servis PIN",
"Kullanýc?Ad",
"Yerel Port",
"RTP Port",
"Kayýt Sonu",
"Canl?Tut",
"DTMF Gönder",
"DTMF Yük",
"G.726-32 Yük",
"ÝLBC Yük",
"Speex Yük",
"Mesaj Numaras",
"PRACK",
"MWI Kayýt",
"Proxy Ýstemi",
"NAT Traversal",
"NAT IP",
"STUN Sunucu",
"STUN Port",
};


#endif

const UCHAR _cDialplanMenu[DIALPLAN_MENU_MAX][DISPLAY_MAX_CHAR+1] = {
"Yönlendir",
"Tüm Arama. Yön.",
"Meþgul. Yön.",
"Cevapsýz. Yön.",
"Bekleme Süresi",
"Otomatik Cevap",
"Arama Bekletme",
"Ön Numara ",
"Hotline Numaras",
"Digitmap Kullan",
"Digitmap Zaman",
"'#'ile Arama",
};


const UCHAR _cSystemMenu[SYSTEM_MENU_MAX][DISPLAY_MAX_CHAR+1] = {
"Yönetici þifre",
//"Debug Kullan",
"Syslog IP",
"Ahize Giden",
"Ahize Gelen",
"Hoparlor Çýkýþ",
"Prov. Sunucu",
"Prov. Port",
"Prov. Aralýk",
"SNTP Sunucu",
"Zaman Dilimi",
"Yaz Saati",
};

const UCHAR _cSavePhonebook[] = "  Rehbere Kaydet?";
const UCHAR _cSaveSettings[] = "  Ayarlar?Kaydet?";
const UCHAR _cConfirm[] = "Devam Edilsin mi?";
const UCHAR _cYesOrCancel[] = " ";//"<OK>|<Çýkýþ>";
const UCHAR _cEndOfList[] = "--Liste Sonu--";
const UCHAR _cSpaces[] = "bo";
const UCHAR _cContacts[] = "kayýtl";
const UCHAR _cRecords[] = "kayýtl";
const UCHAR _cEnterName[] = "isim:";
const UCHAR _cEnterNumber[] = "Numara:";
const UCHAR _cOKToEnter[] = "giriþiçin <OK>";
const UCHAR _cError[] = "HATA!";
const UCHAR _cEmpty[] = "[boþ]";
const UCHAR _cWriting[] = "Hafýzaya Yazýlýyor..";

const UCHAR _cCodecMark[VOICE_CODER_NUM+1][9] = {"Null", "PCMU", "PCMA", "G.726-32", "GSM 6.10", "iLBC", "Speex", "G.729"};
const UCHAR _cSpeexRateMark[8][9] = {"2.15kbps", "3.95kbps", "5.95kbps", "8kbps", "11kbps", "15kbps", "18.2kbps", "24.6kbps"};
const UCHAR _cIlbcFrameMark[2][7] = {"20ms", "30ms"};
const UCHAR _cYesNoMark[2][6] = {"Hayýr", "Evet"};
const UCHAR _cDtmfTypeMark[3][9] = {"Inband", "RFC2833", "SIP Info"};

const UCHAR _cIPTypeMark[IP_TYPE_MAX][DISPLAY_MAX_CHAR+1] = {"Statik", "DHCP", "PPPoE"};
const UCHAR _cDnsTypeMark[2][DISPLAY_MAX_CHAR+1] = {"Manuel", "Otomatik"};
const UCHAR _cNatTraversalMark[3][DISPLAY_MAX_CHAR+1] = {"Kaldýr", "STUN", "NAT IP"};
