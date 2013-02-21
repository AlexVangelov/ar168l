// SRAM
#define SRAM_BASE			0x4000

// 32k banks usage
#define BANK_BASE			0x8000

#define UDP_DEBUG_BANK_OFFSET	0
#define UI_BANK_OFFSET			1	
#define SERIAL_BANK_OFFSET		UI_BANK_OFFSET
#define NET_BANK_OFFSET			2
#define TCP_BANK_OFFSET			3
#define HTTP_BANK_OFFSET		3
#define MENU_BANK_OFFSET		4
#define RTP_BANK_OFFSET			5
#define PROTOCOL_BANK_OFFSET	5
#define AUTH_BANK_OFFSET		6
#define MD5_BANK_OFFSET			6
#define DIGITMAP_BANK_OFFSET	7
#define SIPR_BANK_OFFSET		7

// 64k pages usage
#define SYSTEM_BOOT_PAGE		0
#define SYSTEM_SETTINGS_PAGE	1
#define SYSTEM_RINGTONE_PAGE	2
#define SYSTEM_HOLDMUSIC_PAGE	5
#define SYSTEM_FONT_PAGE		8
#define SYSTEM_IVR_PAGE			8
#define SYSTEM_1ST_PAGE			12
#define SYSTEM_2ND_PAGE			22
#define SYSTEM_PROGRAMMER_PAGE	32		// for ar168d programmer
#define SYSTEM_INVALID_PAGE		255
#define SYSTEM_HTTP_PAGE_OFFSET	4
#define SYSTEM_DSP_PAGE			5	// Start of DSP page, details defined in dsp_page.h

#define IVR_UNIT_LENGTH			4640	// 0.58 seconds for each ivr unit
#define IVR_MAX_NUMBER			7		// every 7 units in a 32k page
#define SYSTEM_IVR_0_OFFSET		0
#define SYSTEM_IVR_1_OFFSET		1
#define SYSTEM_IVR_2_OFFSET		2
#define SYSTEM_IVR_3_OFFSET		3
#define SYSTEM_IVR_4_OFFSET		4
#define SYSTEM_IVR_5_OFFSET		5
#define SYSTEM_IVR_6_OFFSET		6
#define SYSTEM_IVR_7_OFFSET		7
#define SYSTEM_IVR_8_OFFSET		8
#define SYSTEM_IVR_9_OFFSET		9
#define SYSTEM_IVR_DOT_OFFSET	10
#define SYSTEM_IVR_IP_OFFSET	11
#define SYSTEM_IVR_LOCAL_OFFSET	12

#define SYSTEM_PROGRAM_PAGE_NUM		(SYSTEM_2ND_PAGE-SYSTEM_1ST_PAGE)
#define SYSTEM_RINGTONE_PAGE_NUM	(SYSTEM_HOLDMUSIC_PAGE-SYSTEM_RINGTONE_PAGE)
#define SYSTEM_HOLDMUSIC_PAGE_NUM	(SYSTEM_FONT_PAGE-SYSTEM_HOLDMUSIC_PAGE)
#define SYSTEM_FONT_PAGE_NUM		(SYSTEM_1ST_PAGE - SYSTEM_FONT_PAGE)
#define SYSTEM_DSP_PAGE_NUM			5

#define FILE_FULL_PAGE_SIZE		65536
#define FILE_HALF_PAGE_SIZE		32768
#define FILE_RING_PAGE_SIZE		0xff80
#define SETTINGS_FILE_SIZE		16384
//#define DIGITMAP_FILE_SIZE		8192
#define PHONEBOOK_FILE_SIZE		32768
#define FILE_FLAG_PAGE_SIZE		0xffd0

#define MAX_USER_NAME_LEN		32
#define MAX_USER_NUMBER_LEN		32

// Common settings, starting from 0x0000 to 0x0fff
#define OPT_MAC_ADDRESS			0x0000
#define OPT_SYSTEM_PAGE			0x0006
#define OPT_FLAG_PRODUCT		0x0007
#define MAX_FLAG_PRODUCT_LEN	16
#define OPT_FLAG_PROTOCOL		0x0017
#define MAX_FLAG_PROTOCOL		5
#define OPT_FLAG_COUNTRY		0x001c
#define MAX_FLAG_COUNTRY		3
#define OPT_FLAG_OEM			0x001f
#define MAX_FLAG_OEM_LEN		16
#define OPT_FLAG_VERSION		0x002f
#define MAX_FLAG_VERSION		7
#define OPT_CONFIG_ID			0x0036
#define MAX_CONFIG_ID			11
//Network settings, starting from 0x0050 to 0x00cf
#define OPT_IP_TYPE				0x0050
#define OPT_IP_ADDRESS			0x0051
#define OPT_SUBNET_MASK			0x0055
#define OPT_ROUTER_IP			0x0059
#define OPT_DNS_TYPE			0x005d
#define OPT_DNS_IP				0x005e
#define OPT_DNS_IP2				0x0062
#define OPT_LAY3_QOS			0x0066	//0x0067 reserved
#define OPT_VLAN_TAG			0x0068
#define OPT_QOS_PRIORITY		0x006a	//0x006b reserved
#define OPT_PPPOE_ID			0x006c	//0x006c-0x009b
#define MAX_PPPOE_ID_LEN		48
#define OPT_PPPOE_PIN			0x009c	// 0x009c-0x00bb
#define MAX_PPPOE_PIN_LEN		32
#ifndef OEM_IP20
#define OPT_SERIAL_METHOD		0x00bc
#define OPT_BAUD_RATE			0x00bd
#endif
#define OPT_VLAN_TAG2			0x00be
#define OPT_QOS_PRIORITY2		0x00c0	//0x00c1 reserved
#define OPT_VLAN_TAG3			0x00c2
#define OPT_QOS_PRIORITY3		0x00c4	//0x00c5 reserved

//System settings, starting from 0x00d0 to 0x034f
#define OPT_TIME_ZONE			0x00d0
#define OPT_DAY_LIGHT			0x00d1
#define OPT_SNTP_DNS			0x00d2	// 0x00d2-0x0111, 0x0112-0x01d0 reserved
#define MAX_DNS_LEN				64
#define OPT_ADMIN_PIN			0x01d1
#define MAX_ADMIN_PIN_LEN		32
#define OPT_VOL_INPUT			0x01f1
#define OPT_VOL_OUTPUT			0x01f2
#define OPT_SPEAKER_VOL			0x01f3
#define OPT_PROVISION_SERVER	0x01f4	// 0x01f4-0x0233, 0x0234-0x0273 reserved
#define OPT_PROV_FILENAME		0x0274	// 0x0274-0x02b3, 0x02b4-0x02f2 reserved
#define OPT_PROVISION_TTL		0x02f3
#define OPT_PROVISION_PORT		0x02f5	// 0x02f5-0x02f6
#define	OPT_UDP_DEBUG			0x02f7
#define OPT_SYSLOG_IP			0x02f8	// 0x02f8-0x02fb
#define OPT_TIME_OUT			0x02fc	// oem_ip20
#define OPT_RING_VOL_OUTPUT		0x02fd
#define OPT_ADMIN_USER			0x0300	// 0x0300-0x0320, oem_innomedia
#define MAX_ADMIN_USER_LEN		32

//Voice settings, starting from 0x0350 to 0x03cf
#define OPT_VOICE_CODER0		0x0350	//0x0350 -- 0x035f, all kinds of codec
#define OPT_VOICE_CODER1		0x0351	
#define OPT_VOICE_CODER2		0x0352
#define OPT_VOICE_CODER3		0x0353		
#define OPT_VOICE_CODER4		0x0354		
#define OPT_VOICE_CODER5		0x0355
#define OPT_VOICE_CODER6		0x0356		
#define OPT_VAD					0x0361
#define OPT_AGC					0x0362
#define OPT_AEC					0x0363
#define OPT_ILBC_FRAME			0x0364	
#define OPT_FRAME_NUMBER		0x0366
#define OPT_SPEEX_RATE			0x0367
#define OPT_FRAME_NUMBER0		0x0368
#define OPT_FRAME_NUMBER1		0x0369
#define OPT_FRAME_NUMBER2		0x036a
#define OPT_FRAME_NUMBER3		0x036b
#define OPT_FRAME_NUMBER4		0x036c
#define OPT_FRAME_NUMBER5		0x036d
#define OPT_FRAME_NUMBER6		0x036e
//Dialplan settings, starting from 0x03d0
#define OPT_FWD_NUMBER			0x03d0	//0x03d0-0x03ef, 0x03f0-0x040f reserved
#define OPT_FWD_ALWAYS			0x0410
#define OPT_FWD_BUSY			0x0411
#define OPT_FWD_NOANSWER		0x0412
#define OPT_NOANSWER_TIMEOUT	0x0413
#define OPT_CALL_WAITING		0x0414
#define OPT_DIAL_PREFIX			0x0415	//0x0415-0x0424, 0x0425 - 0x0434 reserved
#define MAX_DIAL_PREFIX_LEN		16
#define OPT_HOTLINE_NUMBER		0x0435  //0x0435-0x0474
#define OPT_USE_DIGITMAP		0x475
#define OPT_DIGITMAP_TIMEOUT	0x476
#define OPT_AUTO_ANSWER			0x477
#define OPT_REDIAL_TIMEOUT		0x478	// oem_bt2008	
#define OPT_CALL_LOG_TIMEOUT	0x479	// oem_bt2008
#define OPT_RINGING				0x47a	// oem_bt2008
#define OPT_POUND_AS_CALL		0x47b
#define OPT_GET_VOICE_MSG		0x047c	// oem_bt2008
#define OPT_GRADUAL_RINGING		0x047d	// oem_bt2008
#define OPT_PTT_CONTROL			0x047c	// oem_roip

// Protocol settings, starting from 0x1000 to 0x2000. Common part, starting from 0x1000 to 0x11ff
#define OPT_REGISTER			0x1000
#define OPT_LOCAL_PORT			0x1001
#define OPT_AUTH_ID				0x1003  //0x1003-0x1022, 0x1023-0x1042 reserved
#define MAX_AUTH_ID_LEN			32
#define OPT_AUTH_PIN			0x1043	//0x1043-0x1063, 0x1064-0x1082 reserved
#define MAX_AUTH_PIN_LEN		33
#define OPT_USER_NAME			0x1083	
#define OPT_USER_NUMBER			0x10a3	//0x10a3-0x10c2, 0x10c3-0x10e2 reserved
#define OPT_REGISTER_TTL		0x10e3	//0x10e3-0x10e4, 0x10e5 reserved
#define OPT_MESSAGE_NUMBER		0x10e6	//0x10e6-0x1105
//iax2 specific part, starting from 0x1200
#define OPT_IAX2_SERVER			0x1200	// 0x1200-0x1239, 0x1240-0x12fe reserved
#define OPT_IAX2_PORT			0x12ff
#define OPT_DIGIT_DIALING		0x1301	// 1 byte
//sip specific part, starting from 0x1200
#define OPT_SIP_PROXY			0x1200	//0x1200-0x1239, 0x1240-0x12fe reserved
#define OPT_SIP_PORT			0x12ff
#define OPT_SIP_DOMAIN			0x1301	//0x1301-0x1340, 0x1341-0x13ff reserved; temporarily set it as the same as OPT_SERVER_DNS
#define OPT_OUTBOUND_PROXY		0x1400
#define OPT_RTP_PORT			0x1401
#define OPT_DTMF_PAYLOAD		0x1403
#define OPT_DNS_SRV				0x1404	//0X1405 reserved
#define OPT_PRACK				0x1406
#define OPT_PROXY_REQUIRE		0x1407	//0x1407-0x1446, 0x1447-0x1505 reserved
#define OPT_NAT_TRAVERSAL		0x1506
#define OPT_NAT_IP				0x1507
#define OPT_STUN_SERVER			0x150b	//0x150b-0x154a, 0x154b-0x1609 reserved
#define OPT_STUN_PORT			0x160a
#define OPT_DTMF_TYPE			0x160c
#define OPT_KEEP_ALIVE			0x160d
#define OPT_ILBC_PAYLOAD		0x160e
#define OPT_ADPCM32_PAYLOAD		0x160f
#define OPT_SPEEX_PAYLOAD		0x1610
#define OPT_SUBSCRIBE_MWI		0x1611

//#ifdef NETCB
#define OPT_NETCB_SERVER	0x1700
#define OPT_NETCB_TMPADDR	0x1740
#define OPT_NETCB_CALLER	0x1780
#define OPT_NETCB_CALLEE	0x17a0
#define OPT_NETCB_PIN			0x17c0
#define OPT_NETCB_TYPE		0x17e0
//#endif

// Digit Maps
#define OPT_DIGIT_MAP			0x3000
#define DIGITMAP_ENTRY_LEN		32
#define DIGITMAP_ENTRY_SHIFT	5
#define DIGITMAP_ENTRY_NUM		128

// Default settings
#define OPT_DEFAULT_SETTINGS	0x4000

// Phone book start at 0x8000 and occupies the last 32k bytes of the settings page
#define OPT_PHONE_BOOK			0x8000
// Each entry is 256 bytes long, and are allocated as following:
// 0-31: hold information about this entry, now only first byte is meaningful. If 1, this entry is used; if 0, it's empty
// 32-63: name
// 64-96: number
#define PHONEBOOK_DATA_INFO		0
#define PHONEBOOK_DATA_NAME		32
#define PHONEBOOK_DATA_NUMBER	(PHONEBOOK_DATA_NAME + MAX_USER_NAME_LEN)
#define PHONEBOOK_DATA_LEN		(PHONEBOOK_DATA_NUMBER + MAX_USER_NUMBER_LEN)

#define PHONEBOOK_ENTRY_NUM		128
#define PHONEBOOK_ENTRY_LEN		256
#define PHONEBOOK_ENTRY_SHIFT	8

#ifdef OEM_IP20
#define __ADSL (1)
#endif
