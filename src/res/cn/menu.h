const UCHAR _cMainMenu[MAIN_MENU_MAX][DISPLAY_MAX_CHAR+1] = {
"电话本", 
"通话记录",
"话机设置",
"系统信息",
#ifdef SYS_TEST
"测试话机",
#endif
};

#ifdef SYS_TEST
const UCHAR _cTestMenu[TEST_MENU_MAX][DISPLAY_MAX_CHAR+1] = {
"测试显示屏",
"测试指示灯",
"测试键盘",
};
#endif

const UCHAR _cCallMenu[CALL_MENU_MAX][DISPLAY_MAX_CHAR+1] = {
"未接来电",
"已接来电",
"已拨电话",
};

const UCHAR _cSettingsMenu[SETTINGS_MENU_MAX][DISPLAY_MAX_CHAR+1] = {
"网络设置", 
"语音设置",
"协议设置",
"呼叫设置",
"系统设置",
"恢复出厂设置",
"保存出厂设置",
};

const UCHAR _cInfoMenu[INFO_MENU_MAX][DISPLAY_MAX_CHAR+1] = {
"IP地址",
"电话号码",
"MAC地址",
"话机型号",
"通讯协议",
"语言",
"厂家标识",
"版本号"
};

// Level-2 submenu
const UCHAR _cPbActionMenu[PB_ACTION_MENU_MAX][DISPLAY_MAX_CHAR+1] = {
"呼叫?",
"编辑?",
"删除?",
"新建?",
"全部删除?",
};

const UCHAR _cChActionMenu[CH_ACTION_MENU_MAX][DISPLAY_MAX_CHAR+1] = {
"呼叫?",
"存入电话本?",
"全部删除?",
};

const UCHAR _cNetworkMenu[NETWORK_MENU_MAX][DISPLAY_MAX_CHAR+1] = {
"连接方式",
"IP地址",
"子网掩码",
"默认网关",
"PPPoE用户ID",
"PPPoE用户密码",
"DNS服务器方式",
"首选DNS服务器",
"备用DNS服务器",
"3层Qos",
"Vlan标识",
"Vlan优先级"
};

const UCHAR _cVoiceMenu[VOICE_MENU_MAX][DISPLAY_MAX_CHAR+1] = {
"语音编码1",
"语音帧数1",
"语音编码2",
"语音帧数2",
"语音编码3",
"语音帧数3",
"语音编码4",
"语音帧数4",
"语音编码5",
"语音帧数5",
"语音编码6",
"语音帧数6",
"语音编码7",
"语音帧数7",
"iLBC单帧时长",
"Speex码率",
"活动语音检测",
"自动增益控制",
"回声抵消",
};

#ifdef CALL_IAX2
const UCHAR _cProtocolMenu[PROTOCOL_MENU_MAX][DISPLAY_MAX_CHAR+1] = {
"使用登录服务",
"IAX2服务器",
"服务器端口",
"用户ID",
"认证ID",
"认证密码",
"用户名称",
"本地IAX2端口",
"注册刷新间隔",
"信息查询号码",
"远程拨号规则",
};
#elif defined CALL_SIP
const UCHAR _cProtocolMenu[PROTOCOL_MENU_MAX][DISPLAY_MAX_CHAR+1] = {
"使用登录服务",
"SIP服务器",
"服务器端口",
"SIP域名",
"代理服务器",
"使用DNS SRV",
"用户ID",
"认证ID",
"认证密码",
"用户名称",
"本地SIP端口",
"本地RTP端口",
"注册刷新间隔",
"NAT生存周期",
"DTMF发送方式",
"DTMF净荷类型",
"G.726-32净荷类型",
"iLBC净荷类型",
"Speex净荷类型",
"信息查询号码",
"支持SIP PRACK",
"Subsrcibe MWI",
"Proxy Require",
"NAT穿透方式",
"NAT IP地址",
"STUN服务器",
"STUN服务器端口",
};
#endif

const UCHAR _cDialplanMenu[DIALPLAN_MENU_MAX][DISPLAY_MAX_CHAR+1] = {
"呼叫转移号码",
"无条件呼叫转移",
"遇忙呼叫转移",
"无应答呼叫转移",
"无应答振铃时长",
"自动应答",
"呼叫等待",
"呼叫前缀",
"热线号码",
"使用呼叫规则",
"呼出等待时长",
"使用'#'键呼出",
};

const UCHAR _cSystemMenu[SYSTEM_MENU_MAX][DISPLAY_MAX_CHAR+1] = {
"话机管理密码",
"Syslog IP 地址",
"听筒输入音量",
"听筒输出音量",
"免提输出音量",
"振铃音量",
"自动升级服务器",
"自动升级端口",
"自动升级间隔",
"SNTP服务器",
"时区",
"夏时制自动调节",
};

const UCHAR _cSavePhonebook[] = "保存电话本?";
const UCHAR _cSaveSettings[] = "保存话机设置?";
const UCHAR _cConfirm[] = "确认继续操作？";
const UCHAR _cYesOrCancel[] = "<确定>|<取消>";
const UCHAR _cEndOfList[] = "--End of List--";
const UCHAR _cSpaces[] = "条空记录";
const UCHAR _cContacts[] = "条记录";
const UCHAR _cRecords[] = "条记录";
const UCHAR _cEnterName[] = "输入姓名:";
const UCHAR _cEnterNumber[] = "输入号码:";
const UCHAR _cOKToEnter[] = "按<确认>进入";
const UCHAR _cError[] = "错误!";
const UCHAR _cEmpty[] = "[空白]";
const UCHAR _cWriting[] = "正在保存...";

const UCHAR _cCodecMark[VOICE_CODER_NUM+1][9] = {"Null", "PCMU", "PCMA", "G.726-32", "GSM 6.10", "iLBC", "Speex", "G.729"};
const UCHAR _cSpeexRateMark[8][9] = {"2.15kbps", "3.95kbps", "5.95kbps", "8kbps", "11kbps", "15kbps", "18.2kbps", "24.6kbps"};
const UCHAR _cIlbcFrameMark[2][7] = {"20ms", "30ms"};
const UCHAR _cYesNoMark[2][6] = {"No", "Yes"};
const UCHAR _cDtmfTypeMark[3][9] = {"Inband", "RFC2833", "SIP Info"};

const UCHAR _cIPTypeMark[IP_TYPE_MAX][DISPLAY_MAX_CHAR+1] = {"Static", "DHCP", "PPPoE"};
const UCHAR _cDnsTypeMark[2][DISPLAY_MAX_CHAR+1] = {"Manual", "Automatic"};
const UCHAR _cNatTraversalMark[3][DISPLAY_MAX_CHAR+1] = {"Disabled", "STUN", "NAT IP"};
