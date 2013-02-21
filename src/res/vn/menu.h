
const UCHAR _cMainMenu[MAIN_MENU_MAX][DISPLAY_MAX_CHAR+1] = {
"Danh ba", 
"Lich su cuoc goi",
"Cai dat dine thoai",
"Thong tin he thong",
#ifdef SYS_TEST
"Kiem tra dien thoai",
#endif
};

#ifdef SYS_TEST
const UCHAR _cTestMenu[TEST_MENU_MAX][DISPLAY_MAX_CHAR+1] = {
"Kiem tra LCD",
"Kiem tra LED",
"Kiem tra ban phim",
};
#endif

const UCHAR _cCallMenu[CALL_MENU_MAX][DISPLAY_MAX_CHAR+1] = {
"Cac cuoc goi nho",
"Cac cuoc goi da nhan",
"Cac cuoc goi di",
};

const UCHAR _cSettingsMenu[SETTINGS_MENU_MAX][DISPLAY_MAX_CHAR+1] = {
"Mang",
"Thoai",
"Giao thuc",
"Quay so nhanh",
"He thong",
"Lay cau hinh mac dinh",
"Luu cau hinh mac dinh",
};

const UCHAR _cInfoMenu[INFO_MENU_MAX][DISPLAY_MAX_CHAR+1] = {
"Dia chi IP",
"So may dien thoai",
"Dia chi MAC",
"Loai may dien thoai",
"Giao thuc",
"Ngon ngu",
"Nhan OEM",
"Phien ban"
};

// Level-2 submenu
const UCHAR _cPbActionMenu[PB_ACTION_MENU_MAX][DISPLAY_MAX_CHAR+1] = {
"Goi khong?",
"Sua lai?",
"Xoa khong?",
"Them moi?",
"Xoa tat ca?",
};

const UCHAR _cChActionMenu[CH_ACTION_MENU_MAX][DISPLAY_MAX_CHAR+1] = {
"Goi di khong ?",
"Them vao danh ba",
"Xoa tat ca?",
};

const UCHAR _cNetworkMenu[NETWORK_MENU_MAX][DISPLAY_MAX_CHAR+1] = {
"Kieu IP",
"Dia chi IP",
"Mat na Subnet",
"Dia chi dinh tuyen",
"Ten PPPoE",
"Mat ma PPPoE",
"Kieu DNS",
"Dia chi IP DNS1",
"Dia chi IP DNS",
"Lay3 QoS",
"VLAN Tag",
"VLAN co uu tien"
};

const UCHAR _cVoiceMenu[VOICE_MENU_MAX][DISPLAY_MAX_CHAR+1] = {
"Ma hoa thoai 1",
"Khung TX1",
"Ma hoa thoai 2",
"Khung TX2",
"Ma hoa thoai 3",
"Khung TX3",
"Ma hoa thoai 4",
"Khung TX4",
"Ma hoa thoai 5",
"Khung TX5",
"Ma hoa thoai 6",
"Khung TX6",
"Ma hoa thoai 7",
"Khung TX7",
"Kich thuoc khung iLBC",
"Toc do Speex",
"VAD",
"AGC",
"AEC",
};

#ifdef CALL_IAX2
const UCHAR _cProtocolMenu[PROTOCOL_MENU_MAX][DISPLAY_MAX_CHAR+1] = {
"Dang ky IAX2",
"May chu IAX2",
"Cong may chu",
"ID nguoi dung",
"ID xac thuc",
"PIN xac thuc",
"Ten nguoi dung",
"Cong noi bo",
"Han dang ky",
"So dich vu nhan tin",
"Quay so nhanh",
};
#elif defined CALL_SIP
const UCHAR _cProtocolMenu[PROTOCOL_MENU_MAX][DISPLAY_MAX_CHAR+1] = {
"Dang ky SIP",
"May chu SIP",
"Cong may chu",
"Mien SIP",
"May chu Proxy",
"May chu DNS",
"ID nguoi dung",
"ID xac thuc",
"PIN xac thuc",
"Ten nguoi dung",
"Cong noi bo",
"Cong RTP",
"han dang ky",
"Thoi gian lam tuoi",
"Gui DTMF",
"Tai DTMF",
"Tai G.726-32",
"Tai iLBC",
"Tai Speex",
"So ban tin",
"PRACK",
"Thue bao MWI",
"Can may chu Proxy",
"Vuot NAT",
"Dia chi NAT",
"May chu STUN",
"Cong STUN",
};
#endif

const UCHAR _cDialplanMenu[DIALPLAN_MENU_MAX][DISPLAY_MAX_CHAR+1] = {
"So chuyen den",
"Luon chuyen",
"Khi ban",
"Khong tra loi",
"Thoi gian khong tra loi",
"Tu dong tra loi",
"Dang cho cuoc goi",
"Tien to quay so",
"So dien thoai nong",
"Dung chuc nang quay so nhanh",
"Chuc nang quay so nhanh da het",
"Dung phim '#' de goi",
};

const UCHAR _cSystemMenu[SYSTEM_MENU_MAX][DISPLAY_MAX_CHAR+1] = {
"Mat ma quan tri",
"Loc ban tin IP",
"Am luong MIC",
"Am luong tai nghe to hop",
"Am luong loa ngoai",
"Ring Volume",
"May chu cung cap cau hinh tu dong",
"Cong cung cap cau hinh tu dong",
"Thoi gian duy tri cau hinh tu dong",
"May chu SNTP",
"Vung thoi gian",
"Ban ngay",
};

const UCHAR _cSavePhonebook[] = "Luu danh ba?";
const UCHAR _cSaveSettings[] = "Luu cai dat?";
const UCHAR _cConfirm[] = "Ban co chac?";
const UCHAR _cYesOrCancel[] = "<OK>|<Huy>";
const UCHAR _cEndOfList[] = "--Het danh sach--";
const UCHAR _cSpaces[] = "trong";
const UCHAR _cContacts[] = "So lien he";
const UCHAR _cRecords[] = "Cac ban ghi";
const UCHAR _cEnterName[] = "Vao ten:";
const UCHAR _cEnterNumber[] = "vao so:";
const UCHAR _cOKToEnter[] = "<OK> de vao";
const UCHAR _cError[] = "Loi!";
const UCHAR _cEmpty[] = "[Trong]";
const UCHAR _cWriting[] = "-Dang ghi vao Flash-";

const UCHAR _cCodecMark[VOICE_CODER_NUM+1][9] = {"Null", "PCMU", "PCMA", "G.726-32", "GSM 6.10", "iLBC", "Speex", "G.729"};
const UCHAR _cSpeexRateMark[8][9] = {"2.15kbps", "3.95kbps", "5.95kbps", "8kbps", "11kbps", "15kbps", "18.2kbps", "24.6kbps"};
const UCHAR _cIlbcFrameMark[2][7] = {"20ms", "30ms"};
const UCHAR _cYesNoMark[2][6] = {"Khong", "Dong y"};
const UCHAR _cDtmfTypeMark[3][9] = {"Inband", "RFC2833", "SIP Info"};

const UCHAR _cIPTypeMark[IP_TYPE_MAX][DISPLAY_MAX_CHAR+1] = {"Static", "DHCP", "PPPoE"};
const UCHAR _cDnsTypeMark[2][DISPLAY_MAX_CHAR+1] = {"Bang tay", "Tu dong"};
const UCHAR _cNatTraversalMark[3][DISPLAY_MAX_CHAR+1] = {"Huy bo", "STUN", "NAT IP"};
