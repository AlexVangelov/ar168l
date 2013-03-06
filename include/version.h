// General version config
//#define VER_AR168D			// AR1688 based programmer on July 28, 2006
//#define VER_AR168DS			// AR1688 based programmer for SST39VF1681 and other pin compatible flash
//#define VER_AR168F			// AR168E with only 29 keys, hardware version 4.03-4.05
//#define VER_GF302				// High-link AR168F special version
//#define VER_YWH201			// Yuxin AR168F special version
//#define VER_AR168G			// 37 keys AR168E with ST7565 dot matrix display
//#define VER_GP1266			// Digitmat extended GPIO key version with PoE support
//#define VER_BT2008			// BitCall extended GPIO key version with PoE support, 2x16 LCD
//#define VER_AR168J			// Hardware version 6.00
//#define VER_AR168K			// Hardware version 7.00, 2x16 LCD, 5x6 key
//#define VER_AR168KM			// 2x16 LCD, 5x6 key, Micrel KSZ8842, SST39VF1681
//#define VER_GP2266			// Digitmat AR168K with dot matrix display 
#define VER_AR168M			// AR1688 VoIP Module
//#define VER_AR168MK			// AR1688 VoIP Module with SST39VF1681, Micrel KSZ8842
//#define VER_AR168MS			// AR1688 VoIP Module with SST39VF1681
//#define VER_AR168MT			// AR1688 VoIP Module with Micrel KSZ8842
//#define VER_BT2008N			// BitCall SST39VF1681
//#define VER_BT2008O			// same as BitCall BT2008N, just different key and some functions
//#define VER_FWV2800			// FunctionGroup Voip 2800
//#define VER_DXDT				// Same as GP2266, ShenOu Communication, WuHan
//#define VER_AR168P			// GP2266 with KSZ8842
//#define VER_AR168R			// RoIP (Radio over IP) Module
//#define VER_AR168W			// Web only

// Protocols
#define CALL_SIP
//#define CALL_IAX2
//#define CALL_NONE			// for safe-recovery mode

// Country and District - according to http://www.iso.org/iso/country_codes
//#define RES_AR	// Argentina (Spanish)
//#define RES_AU	// Australia (English)
//#define RES_BE	// Belgium
//#define RES_BG	// Bulgaria (English)
//#define RES_BR	// Brazil (Portuguese)
//#define RES_CA	// Canada
//#define RES_CL	// Chile
//#define RES_CN		// China (Mandarin) ***
//#define RES_CZ	// Czech Republic
//#define RES_DE	// Germany
//#define RES_EG	// Egypt
//#define RES_ES	// Spain
//#define RES_FR	// France
//#define RES_GB	// United
//#define RES_GR	// Greece
//#define RES_HK	// China HongKong
//#define RES_HU	// Hungary
//#define RES_IN	// India
//#define RES_IT	// Italy
//#define RES_JP	// Japan
//#define RES_KR	// Korea
//#define RES_LK	// Sri Lanka (Singhalese)
//#define RES_MX	// Mexico
//#define RES_NL	// Netherlands
//#define RES_NO	// Norway
//#define RES_NZ	// New Zealand
//#define RES_PL	// Poland
//#define RES_PT	// Portugal
//#define RES_RO	// Romania  
//#define RES_RU	// Russia
//#define RES_SG	// Singapore
//#define RES_SK	// Slovakia
//#define RES_TR	// Turkey
//#define RES_TW	// China Taiwan, use Traditional Chinese.
#define RES_US	// United States (English) ***
//#define RES_VN	// Vietnam
//#define RES_ZA	// South Africa

// OEM defines
//#define OEM_2KLABS
//#define OEM_AEM
//#define OEM_AUTOTEST
//#define OEM_BIT2NET
//#define OEM_BROADCAST
//#define OEM_BT2008
//#define OEM_DEBUG
//#define OEM_INNOMEDIA
//#define OEM_IP20
//#define OEM_IVR
//#define OEM_IVRG729
//#define OEM_LCD4
//#define OEM_LCDTEST
//#define OEM_MAC
//#define OEM_REMOTA
//#define OEM_ROIP
//#define OEM_UART
//#define OEM_VLAN
//#define OEM_WEBNEED

// OEM options used in namebin.exe
//#define OEM_SIZE		// report total code size
//#define OEM_ZIP		// zip the output .bin file

/*************************** End of automatic change, DO NOT CHANGE this line! */

#ifdef VER_AR168D
#define MT28F016S5
#define SYS_PROGRAMMER
#endif

#ifdef VER_AR168DS
#define SST39VF1681
#define SYS_PROGRAMMER
#endif

#ifdef SYS_PROGRAMMER
#define RTL8019AS
#define KEY_4X4				// Basic 16 keys by GPIO_B(8)
#define LCD_HY1602
#define VERIFY_FLASH
#endif

#if defined VER_GF302 || defined VER_YWH201
#define VER_AR168F
#endif

#ifdef VER_AR168F
#ifdef OEM_UART
#define SERIAL_UI
#define MT28F016S5
#define RTL8019AS
#else
#define KEY_ADC_2X7			// Extended 14 keys by ADC
#endif
#define LCD_HY1602
#endif

#ifdef VER_AR168G
#ifdef OEM_UART
#define SERIAL_UI
#define MT28F016S5
#define RTL8019AS
#else
#define KEY_ADC_2X11		// Extended 22 keys by ADC
#define SYS_TEST
#endif
#define LCD_ST7565
#endif

#if defined KEY_ADC_2X7 || defined KEY_ADC_2X11
#define KEY_4X4				// Basic 16 keys by GPIO_B(8)
#define KEY_ADC
#define MT28F016S5
#define RTL8019AS
#endif

#ifdef VER_GP1266
#ifdef OEM_UART
#define SERIAL_UI
#define HOOK_GPIO			// GPIO_C2
#define MT28F016S5
#define RTL8019AS
#define SYS_DUAL_MIC
#else
#define KEY_5X5				// GPIO_K(8)&D(2)
#define SYS_TEST
#endif
#define LCD_ST7565
#endif

#ifdef VER_BT2008
#define KEY_5X5				// GPIO_K(8)&D(2)
#define LCD_HY1602
#define MT28F016S5
#define OEM_BT2008
#endif

#ifdef KEY_5X5
#define KEY_4X4				// Basic 16 keys by GPIO_B(8)
#define HOOK_GPIO			// GPIO_C2
#define MT28F016S5
#define RTL8019AS
#define SYS_DUAL_MIC
#endif

#ifdef VER_AR168J
#define KEY_7X8				// GPIO_B(8)
#define HOOK_GPIO			// GPIO_C2
#ifdef OEM_IP20
#define LCD_HY2004
#else
#define LCD_HY1602
#endif
#define MT28F016S5
#define RTL8019AS
#define SYS_DUAL_MIC
#endif

#ifdef VER_AR168K
#define KEY_5X6				// GPIO_B(4)&D(2)
#define YNTX_KEYPAD
#define HOOK_GPIO			// GPIO_C2
#define LCD_HY1602
#define MT28F016S5
#define RTL8019AS
#endif

#ifdef VER_AR168KM
#define KEY_5X6				// GPIO_B(4)&D(2)
#define HOOK_GPIO			// GPIO_C2
#define LCD_HY1602
#define SST39VF1681
#define KSZ8842
#endif

#ifdef KEY_5X6
#define SYS_CHIP_PLUS
#endif

#ifdef VER_AR168M
#ifdef OEM_ROIP
#define SYS_IVR_G729
#else
#define SERIAL_UI			// do not define this if you like to control serial communications totaly by yourself and do not use our standard 8051 UI
#endif
#define MT28F016S5
#define RTL8019AS
#define SYS_CHIP_PLUS
#ifdef OEM_IVR
#define IP_KEY				// use safe mode jumper "key" to read IVR if OEM_IVR defined
#endif
#ifdef OEM_LCD4
#define LCD_ST7565
//#undef SERIAL_UI
//#define SERIAL_LOOP
#endif

#endif

#ifdef VER_AR168MK
#define SERIAL_LOOP
#define LCD_HY1602
#define SST39VF1681
#define KSZ8842
#define SYS_CHIP_PLUS
#endif

#ifdef VER_AR168MS
#define SERIAL_LOOP
#define LCD_HY1602
#define SST39VF1681
#define RTL8019AS
#define SYS_CHIP_PLUS
#endif

#ifdef VER_AR168MT
#define SERIAL_UI
#define MT28F016S5
#define KSZ8842
#define SYS_CHIP_PLUS
#endif

#ifdef VER_GP2266
#define RTL8019AS
#ifdef OEM_UART
#define SERIAL_UI
#define HOOK_GPIO			// GPIO_C2
#define SST39VF1681
#define SYS_CHIP_PLUS
#else
#define KEY_8X5				// GPIO_B(4)&D4
#define SYS_TEST
#endif
#define LCD_ST7565
#endif

#if defined VER_BT2008N || defined VER_BT2008O
#define KEY_8X5				// GPIO_B(4)&D4
#ifndef OEM_IVR
#define LCD_ST7565
#endif
#define RTL8019AS
#define OEM_BT2008
#endif

#ifdef VER_FWV2800
#define KEY_8X5				// GPIO_B(4)&D4
#define LCD_ST7565
#define RTL8019AS
#endif

#ifdef VER_DXDT
#define KEY_8X5				// GPIO_B(4)&D4
#define LCD_ST7565
#define RTL8019AS
#endif

#ifdef VER_AR168P
#define KEY_8X5				// GPIO_B(4)&D4
#define LCD_ST7565
#define KSZ8842
#define SYS_TEST
#endif

#ifdef KEY_8X5
#define HOOK_GPIO			// GPIO_C2
#define SST39VF1681
#define SYS_CHIP_PLUS
#endif

#ifdef VER_AR168R
#define RTL8019AS
#ifdef OEM_UART
#define SERIAL_UI
//#define SERIAL_LOOP
//#define SERIAL_WDCB
//#define SYS_WEBCALL
#elif defined OEM_REMOTA
#define SERIAL_UI
#define OEM_ROIP
#else
#define OEM_ROIP
#endif
#define LCD_HY1602
#define KEY_C2				// GPIO_C2 as key
#define MT28F016S5
#define SYS_CHIP_PLUS
#endif

#ifdef VER_AR168W
#define SERIAL_WDCB
#define SYS_WEBCALL
#define MT28F016S5
#define RTL8019AS
#define SYS_CHIP_PLUS
#endif

#ifdef OEM_IVRG729
#define SYS_IVR_G729
#endif

#if defined SERIAL_UI || defined SERIAL_LOOP || defined SERIAL_WDCB
#define SYS_UART
#endif

#ifdef IP_KEY
#define KEY_C2				// GPIO_C2 as key
#endif

#ifdef CALL_NONE
#undef SYS_UART
#undef SERIAL_UI
#undef SERIAL_LOOP
#undef SERIAL_WDCB
#undef KEY_ADC
#undef KEY_5X5
#undef KEY_C2
#define SYS_DEBUG
#else
#if defined OEM_IVR || defined IP_KEY
#define SYS_IVR
#endif
#endif

#ifdef LCD_HY1602
#define DISPLAY_MAX_CHAR	16
#define DISPLAY_MAX_LINE	2
#define SYS_LCD
#endif

#ifdef LCD_HY2004
#define DISPLAY_MAX_CHAR	20
#define DISPLAY_MAX_LINE	4
#define UI_4_LINES
#define SYS_LCD
#endif

#ifdef LCD_ST7565
#define DISPLAY_DOT
#define DISPLAY_MAX_CHAR	16
#if defined VER_BT2008N || defined VER_BT2008O
#define DISPLAY_MAX_LINE	2
#else
#define DISPLAY_MAX_LINE	4
#define UI_4_LINES
#endif	// VER_BT2008N
#define SYS_LCD
#endif

#ifndef SYS_LCD
#ifdef SERIAL_UI
#define SYS_LCD
#endif
#define DISPLAY_MAX_CHAR	16
#define DISPLAY_MAX_LINE	2
#endif

#ifdef SYS_CHIP_PLUS
#define SYS_DUAL_MIC
#endif

#ifdef OEM_DEBUG
#define SYS_DEBUG
#endif

#ifdef OEM_BT2008
#define MAX_VOLOUT	29
#endif

#ifdef SYS_DEBUG
#define VERIFY_CHECKSUM
#define VERIFY_FLASH
//#define LCD_DEBUG
//#define TEST_MD5
#endif

/*############################ Begin of automatic find software version, DO NOT CHANGE this line! */

#define SOFT_VER_HIGH		0
#define SOFT_VER_LOW		6
#define SOFT_VER_BUILD		1
#define ENG_BUILD_HIGH		0
#define ENG_BUILD_MID		1
#define ENG_BUILD_LOW		3


