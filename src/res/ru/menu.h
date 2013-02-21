
const UCHAR _cMainMenu[MAIN_MENU_MAX][DISPLAY_MAX_CHAR+1] = {
"Адресная книга", 
"История звонков",
"Настройки телеф.",
"Сист. информация",
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
"Пропущен. звонки",
"Принятые звонки",
"Набранные звонки",
};

const UCHAR _cSettingsMenu[SETTINGS_MENU_MAX][DISPLAY_MAX_CHAR+1] = {
"TCP/IP сеть",
"Голос",
"Протокол",
"План набора",
"Система",
"Load Defaults",
"Store Defaults",
};

const UCHAR _cInfoMenu[INFO_MENU_MAX][DISPLAY_MAX_CHAR+1] = {
"IP-адрес",
"Номер телефона",
"MAC-адрес",
"Модель телефона",
"Протокол",
"Язык",
"OEM",
"Версия"
};

// Level-2 submenu
const UCHAR _cPbActionMenu[PB_ACTION_MENU_MAX][DISPLAY_MAX_CHAR+1] = {
"Позвонить?",
"Редактировать?",
"Удалить?",
"Добавить новую?",
"Удалить ВСЕ?",
};

const UCHAR _cChActionMenu[CH_ACTION_MENU_MAX][DISPLAY_MAX_CHAR+1] = {
"Позвонить?",
"Добавить в книгу",
"Удалить ВСЕ?",
};

const UCHAR _cNetworkMenu[NETWORK_MENU_MAX][DISPLAY_MAX_CHAR+1] = {
"Тип IP подключ.",
"IP-адрес",
"Маска сети",
"IP-адрес шлюза",
"PPPoE ID",
"PPPoE PIN",
"Тип DNS",
"1ый ip-адрес DNS",
"2ой ip-адрес DNS",
"Lay3 QoS",
"VLAN метка",
"VLAN приоритет"
};

const UCHAR _cVoiceMenu[VOICE_MENU_MAX][DISPLAY_MAX_CHAR+1] = {
"Голосовой кодек1",
"Frames per TX1",
"Голосовой кодек2",
"Frames per TX2",
"Голосовой кодек3",
"Frames per TX3",
"Голосовой кодек4",
"Frames per TX4",
"Голосовой кодек5",
"Frames per TX5",
"Голосовой кодек6",
"Frames per TX6",
"Голосовой кодек7",
"Frames per TX7",
"iLBC Frame Size",
"Speex rate",
"VAD",
"AGC",
"AEC",
};

#ifdef CALL_IAX2
const UCHAR _cProtocolMenu[PROTOCOL_MENU_MAX][DISPLAY_MAX_CHAR+1] = {
"IAX2 регистратор",
"IAX2 сервер",
"Порт сервера",
"Номер телефона",
"Логин",
"Пароль",
"Имя",
"Локальный порт",
"Время регистрац.",
"Номер ГолосПочты",
"Digit Dialing",
};
#elif defined CALL_SIP
const UCHAR _cProtocolMenu[PROTOCOL_MENU_MAX][DISPLAY_MAX_CHAR+1] = {
"SIP регистратор",
"SIP сервер",
"Порт сервера",
"SIP домен",
"Outbound Proxy",
"Применять DnsSrv",
"Номер телефона",
"Логин",
"Пароль",
"Имя",
"Локальный порт",
"RTP порт",
"Время регистрац.",
"Keep Alive",
"Отправлять DTMF",
"DTMF Payload",
"G.726-32 Payload",
"iLBC Payload",
"Speex Payload",
"Номер ГолосПочты",
"PRACK",
"Подписка на MWI",
"Proxy Require",
"Прохождение NAT",
"NAT ip-адрес",
"STUN сервер",
"STUN порт",
};
#endif

const UCHAR _cDialplanMenu[DIALPLAN_MENU_MAX][DISPLAY_MAX_CHAR+1] = {
"Форвард на Номер",
"Форвард-Всегда",
"Форвард-Занято",
"Форвард-Нет Отв.",
"Время неответа",
"Авто-ответ",
"Удержание звонка",
"Префикс набора",
"Номер Hotline",
"Примен. ПланНум.",
"Таймаут ПланНум.",
"Use '#' To Call",
};

const UCHAR _cSystemMenu[SYSTEM_MENU_MAX][DISPLAY_MAX_CHAR+1] = {
"Системный пароль",
//"Включить отладку",
"Syslog сервер",
"Громк. микрофона",
"Громк. динамика",
"Громк. спикера",
"Prov. сервер",
"Prov. порт",
"Prov. интервал",
"SNTP сервер",
"Временная зона",
"Летнее время",
};

const UCHAR _cSavePhonebook[] = "Сохр. зап.книжку?";
const UCHAR _cSaveSettings[] =  "Сохр. настройки?";
const UCHAR _cConfirm[] = "Do you confirm?";
const UCHAR _cYesOrCancel[] = "<OK>|<CANCEL>";
const UCHAR _cEndOfList[] = "--Конец--";
const UCHAR _cSpaces[] = "ячеек";
const UCHAR _cContacts[] = "контактов";
const UCHAR _cRecords[] = "записей";
const UCHAR _cEnterName[] = "Введите имя:";
const UCHAR _cEnterNumber[] = "Введите номер:";
const UCHAR _cOKToEnter[] = "<OK> для ввода";
const UCHAR _cError[] = "ОШИБКА!";
const UCHAR _cEmpty[] = "[пусто]";
const UCHAR _cWriting[] = "-ЗАПИСЬ FLASH-";

const UCHAR _cCodecMark[VOICE_CODER_NUM+1][9] = {"Нет", "PCMU", "PCMA", "G.726-32", "GSM 6.10", "iLBC", "Speex", "G.729"};
const UCHAR _cSpeexRateMark[8][9] = {"2.15kbps", "3.95kbps", "5.95kbps", "8kbps", "11kbps", "15kbps", "18.2kbps", "24.6kbps"};
const UCHAR _cIlbcFrameMark[2][7] = {"20ms", "30ms"};
const UCHAR _cYesNoMark[2][6] = {"Нет", "Да"};
const UCHAR _cDtmfTypeMark[3][9] = {"Inband", "RFC2833", "SIP Info"};

const UCHAR _cIPTypeMark[IP_TYPE_MAX][DISPLAY_MAX_CHAR+1] = {"Постоянный", "DHCP", "PPPoE"};
const UCHAR _cDnsTypeMark[2][DISPLAY_MAX_CHAR+1] = {"Вручную", "Автоматически"};
const UCHAR _cNatTraversalMark[3][DISPLAY_MAX_CHAR+1] = {"Выключено", "STUN", "NAT IP"};
