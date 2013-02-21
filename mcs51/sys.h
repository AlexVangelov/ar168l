//#define SYS_DEBUG
#define SYS_WINBOND

//#define OSCILLATOR	11059200
#define OSCILLATOR		22118400
//#define BAUD			57600
#define BAUD			19200
//#define BAUD			9600

// timer 1 used for baudrate generator
#define TIMER1_VALUE	-2*OSCILLATOR/32/12/BAUD

// Hook.c
void HookISR() __using 1;
void HookInit();
void HookRun();

// Key.c
void KeyISR() __using 1;
void KeyInit();
void KeyRun();

// Lcd.c
#define DISPLAY_MAX_LINE	2
#define DISPLAY_MAX_CHAR	16
void LcdInit();
void LcdCursor(UCHAR iLine, UCHAR iPos, BOOLEAN bShow);
void LcdDisplay(PDCHAR pBuf, UCHAR iLine);
void LcdDisplayC(PCCHAR pBuf, UCHAR iLine);

// Led.c
void LED_On(UCHAR iIndex);
void LED_Off(UCHAR iIndex);
void LED_Blink(UCHAR iIndex);

// String.c
void strcpy_c2d(PDCHAR pDst, PCCHAR pSrc);
void memcpy_c2d(PDCHAR pDst, PCCHAR pSrc, UCHAR iCount);
UCHAR memcmp_c2d(PDCHAR pDst, const PCCHAR pSrc, UCHAR iCount);
void memset_d(PDCHAR pDst, UCHAR iVal, UCHAR iCount);

// UART.c
void SerialInterrupt() __interrupt 4 __using 2;
void UART_Init();
void UART_Run();
void UART_SendChar(UCHAR iVal);

// UI.c
void UI_Init();
void UI_HandleKey(UCHAR iKey);
void UI_HandleSerialChar(UCHAR iVal);
