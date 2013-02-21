const UCHAR _cMacAddress[] = "mac_address=";
const UCHAR _cSystemPage[] = "system_page=";
const UCHAR _cFlagProduct[] = "flag_product=";
const UCHAR _cFlagProtocol[] = "flag_protocol=";
const UCHAR _cFlagCountry[] = "flag_country=";
const UCHAR _cFlagOem[] = "flag_oem=";
const UCHAR _cFlagVersion[] = "flag_version=";
const UCHAR _cConfigID[] = "config_id=";
//network
const UCHAR _cIPtype[] = "ip_type=";
const UCHAR _cIPAddress[] = "ip_address="; 
const UCHAR _cSubMask[] = "subnet_mask=";
const UCHAR _cRouterIP[] = "router_ip=";
const UCHAR _cDnsType[] = "dns_type=";
const UCHAR _cDnsIP[] = "dns_ip=";
const UCHAR _cDnsIP2[] = "dns_ip2=";
const UCHAR _cLay3Qos[] = "lay3_qos=";
const UCHAR _cVlanTag[] = "vlan_tag=";
const UCHAR _cQosPriority[] = "qos_priority=";
const UCHAR _cPPPoeID[] = "pppoe_id=";
const UCHAR _cPPPoePIN[] = "pppoe_pin=";
const UCHAR _cSerialMethod[] = "serial_method=";
const UCHAR _cBaudRate[] = "baud_rate=";
const UCHAR _cVlanTag2[] = "vlan_tag2=";
const UCHAR _cQosPriority2[] = "qos_priority2=";
const UCHAR _cVlanTag3[] = "vlan_tag3=";
const UCHAR _cQosPriority3[] = "qos_priority3=";
//system
const UCHAR _cTimeZone[] = "time_zone=";
const UCHAR _cDayLight[] = "day_light=";
const UCHAR _cSntpDns[] = "sntp_dns=";
const UCHAR _cAdminPIN[] = "admin_pin=";
const UCHAR _cAdminUser[] = "admin_user=";
const UCHAR _cVolInput[] = "vol_input=";
const UCHAR _cVolOutput[] = "vol_output=";
const UCHAR _cSpeakerVol[] = "speaker_vol=";
const UCHAR _cProvisionServer[] = "provision_server=";
const UCHAR _cProvisionTTL[] = "provision_ttl=";
const UCHAR _cProvisionPort[] = "provision_port=";
const UCHAR _cProvisionFilename[] = "prov_filename=";
const UCHAR _cUdpDebug[] = "udp_debug=";
const UCHAR _cSyslogIP[] = "syslog_ip=";
const UCHAR _cTimeOut[] ="time_out=";
const UCHAR _cRingVolOutput[] = "ring_vol=";
#ifdef OEM_ROIP
const UCHAR _cPttControl[] = "ptt_ctype=";
#endif

//voice
const UCHAR _cVoiceCoder0[] = "voice_coder0=";
const UCHAR _cVoiceCoder1[] = "voice_coder1=";
const UCHAR _cVoiceCoder2[] = "voice_coder2=";
const UCHAR _cVoiceCoder3[] = "voice_coder3=";
const UCHAR _cVoiceCoder4[] = "voice_coder4=";
const UCHAR _cVoiceCoder5[] = "voice_coder5=";
const UCHAR _cVoiceCoder6[] = "voice_coder6=";
const UCHAR _cSpeexRate[] = "speex_rate=";
const UCHAR _cVad[] = "vad=";
const UCHAR _cAgc[] = "agc=";
const UCHAR _cAec[] = "aec=";
const UCHAR _cIlbcFrame[] = "ilbcframe=";
const UCHAR _cFrameNumber0[] = "frame_number0=";
const UCHAR _cFrameNumber1[] = "frame_number1=";
const UCHAR _cFrameNumber2[] = "frame_number2=";
const UCHAR _cFrameNumber3[] = "frame_number3=";
const UCHAR _cFrameNumber4[] = "frame_number4=";
const UCHAR _cFrameNumber5[] = "frame_number5=";
const UCHAR _cFrameNumber6[] = "frame_number6=";
//dialplan
const UCHAR _cFwdNumber[] = "fwd_number=";
const UCHAR _cFwdAlways[] = "fwd_always=";
const UCHAR _cFwdBusy[] = "fwd_busy=";
const UCHAR _cFwdNoAnswer[] = "fwd_noanswer=";
const UCHAR _cNoAnswerTimeout[] = "noanswer_timeout=";
const UCHAR _cCallWaiting[] = "call_waiting=";
const UCHAR _cDialPrefix[] = "dial_prefix=";
const UCHAR _cHotLineNumber[] = "hotline_number=";
const UCHAR _cUseDigitmap[] = "use_digitmap=";
const UCHAR _cDigitmapTimeout[] = "digitmap_timeout=";
const UCHAR _cDigitmapString[] = "digitmap_string=";
const UCHAR _cAutoAnswer[] = "auto_answer=";
const UCHAR _cRedialTimeout[] = "redial_timeout=";
const UCHAR _cCallLogTimeout[] = "callhis_timeout=";
const UCHAR _cRinging[] = "ringing=";
const UCHAR _cPoundAsCall[] = "pound_call=";
const UCHAR _cGetVoiceMsg[] = "get_voicemsg=";
const UCHAR _cGradualRinging[] = "gradual_ringing=";
//prtocol settings
//common
const UCHAR _cRegister[] = "register=";
const UCHAR _cLocalPort[] = "local_port=";
const UCHAR _cAuthID[] = "auth_id=";
const UCHAR _cAuthPIN[] = "auth_pin=";
const UCHAR _cUserName[] = "user_name=";
const UCHAR _cUserNumber[] = "user_number=";
const UCHAR _cDtmfType[] = "dtmf_type=";
const UCHAR _cRegisterTTL[] = "register_ttl=";
const UCHAR _cKeepAlive[] = "keep_alive=";
const UCHAR _cMessageNumber[] = "message_number=";
//sip
#ifdef CALL_SIP
const UCHAR _cSipProxy[] = "sip_proxy=";
const UCHAR _cSipPort[] = "sip_port=";
const UCHAR _cSipDomain[] = "sip_domain=";
const UCHAR _cOutBoundProxy[] = "outbound_proxy=";
const UCHAR _cRtpPort[] = "rtp_port=";
const UCHAR _cDtmfPayload[] = "dtmf_payload=";
const UCHAR _cDnsSrv[] = "dns_srv=";
const UCHAR _cPrack[] = "prack=";
const UCHAR _cSubscribeMWI[] = "subscribe_mwi=";
const UCHAR _cProxyRequire[] = "proxy_require=";
const UCHAR _cNatTraversal[] = "nat_traversal=";
const UCHAR _cNatIP[] = "nat_ip=";
const UCHAR _cStunServer[] = "stun_server=";
const UCHAR _cStunPort[] = "stun_port=";
const UCHAR _cIlbcPayload[] = "ilbcpayload=";
const UCHAR _cAdpcm32Payload[] = "adpcm32_payload=";
const UCHAR _cSpeexPayload[] = "speex_payload=";
#endif
// iax2
#ifdef CALL_IAX2
const UCHAR _cIax2Server[] = "iax2_server=";
const UCHAR _cIax2Port[] = "iax2_port=";
const UCHAR _cDigitDialing[] = "digit_dialing=";
#endif

