const UCHAR _cMainMenu[MAIN_MENU_MAX][DISPLAY_MAX_CHAR+1] = {
"�绰��", 
"ͨ����¼",
"��������",
"ϵͳ��Ϣ",
#ifdef SYS_TEST
"���Ի���",
#endif
};

#ifdef SYS_TEST
const UCHAR _cTestMenu[TEST_MENU_MAX][DISPLAY_MAX_CHAR+1] = {
"������ʾ��",
"����ָʾ��",
"���Լ���",
};
#endif

const UCHAR _cCallMenu[CALL_MENU_MAX][DISPLAY_MAX_CHAR+1] = {
"δ������",
"�ѽ�����",
"�Ѳ��绰",
};

const UCHAR _cSettingsMenu[SETTINGS_MENU_MAX][DISPLAY_MAX_CHAR+1] = {
"��������", 
"��������",
"Э������",
"��������",
"ϵͳ����",
"�ָ���������",
"�����������",
};

const UCHAR _cInfoMenu[INFO_MENU_MAX][DISPLAY_MAX_CHAR+1] = {
"IP��ַ",
"�绰����",
"MAC��ַ",
"�����ͺ�",
"ͨѶЭ��",
"����",
"���ұ�ʶ",
"�汾��"
};

// Level-2 submenu
const UCHAR _cPbActionMenu[PB_ACTION_MENU_MAX][DISPLAY_MAX_CHAR+1] = {
"����?",
"�༭?",
"ɾ��?",
"�½�?",
"ȫ��ɾ��?",
};

const UCHAR _cChActionMenu[CH_ACTION_MENU_MAX][DISPLAY_MAX_CHAR+1] = {
"����?",
"����绰��?",
"ȫ��ɾ��?",
};

const UCHAR _cNetworkMenu[NETWORK_MENU_MAX][DISPLAY_MAX_CHAR+1] = {
"���ӷ�ʽ",
"IP��ַ",
"��������",
"Ĭ������",
"PPPoE�û�ID",
"PPPoE�û�����",
"DNS��������ʽ",
"��ѡDNS������",
"����DNS������",
"3��Qos",
"Vlan��ʶ",
"Vlan���ȼ�"
};

const UCHAR _cVoiceMenu[VOICE_MENU_MAX][DISPLAY_MAX_CHAR+1] = {
"��������1",
"����֡��1",
"��������2",
"����֡��2",
"��������3",
"����֡��3",
"��������4",
"����֡��4",
"��������5",
"����֡��5",
"��������6",
"����֡��6",
"��������7",
"����֡��7",
"iLBC��֡ʱ��",
"Speex����",
"��������",
"�Զ��������",
"��������",
};

#ifdef CALL_IAX2
const UCHAR _cProtocolMenu[PROTOCOL_MENU_MAX][DISPLAY_MAX_CHAR+1] = {
"ʹ�õ�¼����",
"IAX2������",
"�������˿�",
"�û�ID",
"��֤ID",
"��֤����",
"�û�����",
"����IAX2�˿�",
"ע��ˢ�¼��",
"��Ϣ��ѯ����",
"Զ�̲��Ź���",
};
#elif defined CALL_SIP
const UCHAR _cProtocolMenu[PROTOCOL_MENU_MAX][DISPLAY_MAX_CHAR+1] = {
"ʹ�õ�¼����",
"SIP������",
"�������˿�",
"SIP����",
"���������",
"ʹ��DNS SRV",
"�û�ID",
"��֤ID",
"��֤����",
"�û�����",
"����SIP�˿�",
"����RTP�˿�",
"ע��ˢ�¼��",
"NAT��������",
"DTMF���ͷ�ʽ",
"DTMF��������",
"G.726-32��������",
"iLBC��������",
"Speex��������",
"��Ϣ��ѯ����",
"֧��SIP PRACK",
"Subsrcibe MWI",
"Proxy Require",
"NAT��͸��ʽ",
"NAT IP��ַ",
"STUN������",
"STUN�������˿�",
};
#endif

const UCHAR _cDialplanMenu[DIALPLAN_MENU_MAX][DISPLAY_MAX_CHAR+1] = {
"����ת�ƺ���",
"����������ת��",
"��æ����ת��",
"��Ӧ�����ת��",
"��Ӧ������ʱ��",
"�Զ�Ӧ��",
"���еȴ�",
"����ǰ׺",
"���ߺ���",
"ʹ�ú��й���",
"�����ȴ�ʱ��",
"ʹ��'#'������",
};

const UCHAR _cSystemMenu[SYSTEM_MENU_MAX][DISPLAY_MAX_CHAR+1] = {
"������������",
"Syslog IP ��ַ",
"��Ͳ��������",
"��Ͳ�������",
"�����������",
"��������",
"�Զ�����������",
"�Զ������˿�",
"�Զ��������",
"SNTP������",
"ʱ��",
"��ʱ���Զ�����",
};

const UCHAR _cSavePhonebook[] = "����绰��?";
const UCHAR _cSaveSettings[] = "���滰������?";
const UCHAR _cConfirm[] = "ȷ�ϼ���������";
const UCHAR _cYesOrCancel[] = "<ȷ��>|<ȡ��>";
const UCHAR _cEndOfList[] = "--End of List--";
const UCHAR _cSpaces[] = "���ռ�¼";
const UCHAR _cContacts[] = "����¼";
const UCHAR _cRecords[] = "����¼";
const UCHAR _cEnterName[] = "��������:";
const UCHAR _cEnterNumber[] = "�������:";
const UCHAR _cOKToEnter[] = "��<ȷ��>����";
const UCHAR _cError[] = "����!";
const UCHAR _cEmpty[] = "[�հ�]";
const UCHAR _cWriting[] = "���ڱ���...";

const UCHAR _cCodecMark[VOICE_CODER_NUM+1][9] = {"Null", "PCMU", "PCMA", "G.726-32", "GSM 6.10", "iLBC", "Speex", "G.729"};
const UCHAR _cSpeexRateMark[8][9] = {"2.15kbps", "3.95kbps", "5.95kbps", "8kbps", "11kbps", "15kbps", "18.2kbps", "24.6kbps"};
const UCHAR _cIlbcFrameMark[2][7] = {"20ms", "30ms"};
const UCHAR _cYesNoMark[2][6] = {"No", "Yes"};
const UCHAR _cDtmfTypeMark[3][9] = {"Inband", "RFC2833", "SIP Info"};

const UCHAR _cIPTypeMark[IP_TYPE_MAX][DISPLAY_MAX_CHAR+1] = {"Static", "DHCP", "PPPoE"};
const UCHAR _cDnsTypeMark[2][DISPLAY_MAX_CHAR+1] = {"Manual", "Automatic"};
const UCHAR _cNatTraversalMark[3][DISPLAY_MAX_CHAR+1] = {"Disabled", "STUN", "NAT IP"};
