// chip.c
void ChipInit();

void KickDog();
void SetDog(UCHAR iWatchDog);	// 0[176ms], 1[352ms], 2[1.4s], 3[5.6s], 4[22.2s], 5[45s], 6[90s], 7[180s], >8[disable]

// AR1688  MIPS: 0[12], 1[24], 2[36], 3[48], 4[60], 5[72], 6[84], 7[30]
// AR1688+ MIPS: 2[6], 3[9], ..., 31[93]
void SetDSP(UCHAR iMIPS);

void Led_WP_On();
void Led_WP_Off();

void Led_PRE_On();
void Led_PRE_Off();

void InterruptInit();

// dsp_bank1.c
void DspInit();
void DspClearEncodeBuffer();
void DspClearDecodeBuffer();
void DspRun(BOOLEAN bMini);
void DspStart(UCHAR iType, BOOLEAN bVad, UCHAR iOperation);
void DspStop();
void DspSetOperation(UCHAR iCur, UCHAR iNext);
UCHAR DspGetOperation();
UCHAR DspGetType();
void DspLoad(UCHAR iType);

void DspHandleData(USHORT sLen, PCHAR pBuf);

// codec.c
BOOLEAN IsCodecRunning();
void CodecInit();
//void CodecStart(UCHAR iType);
void CodecStart();
void CodecStop();
void CodecMicMute(BOOLEAN bMute);
void CodecSetInputVol(UCHAR iVol);	// input vol: 0-7
void CodecSetVol(UCHAR iVol);		// output vol: 0-31
UCHAR CodecGetVol();

void CodecAnalogLoopback(BOOLEAN bOn);
void CodecTestStart();
void CodecTestStop();

void SpeakerRing();
void SpeakerOff();

void SideToneEnable(BOOLEAN bEnable);

// Functions in Dtmf.c
//#define DTMF_KEY_TIME		180
#define DTMF_KEY_TIME		90

#define DTMF_SIGNAL_DIAL	12
#define DTMF_SIGNAL_BUSY	13
#define DTMF_SIGNAL_WAITING	14
#define DTMF_SIGNAL_CONGESTION	15
#define DTMF_SIGNAL_RING	16

void DtmfPlayTone(UCHAR iType);
void DtmfPlayStutter();
void DtmfPlayKey(UCHAR iAsciiKey);

// Key functions in key.c
void KeyInit();
void KeyStart();

// adc_key.c
void ADC_KeyStart();
void ADC_KeyRun();

// ar168r.c
void RoIP_Start();
void RoIP_Run();
void RoIP_Timer();
//void RoIP_MinuteTimer();

void RoIP_Key(UCHAR iKey);

void RoIP_LogonOk();
void RoIP_Ringing();
void RoIP_Dial();
void RoIP_Connected();
void RoIP_IncomingData(USHORT sLen);
void RoIP_EndCall();

// hook.c
void HookHandleHandsetDown(UCHAR iCount);
void HookStart();
void HookRun();
void Gateway_KeyRun();

// gpio_key.c
void Key4x4_Start();
void Key4x4_Run();

void Key5x5_Start();
void Key5x5_Run();

void Key5x6_Start();
void Key5x6_Run();

void Key8x4_Start();
void Key8x4_Run();

void Key8x5_Start();
void Key8x5_Run();

void Key7x8_Start();
void Key7x8_Run();

void KeyC2_Start();
void KeyC2_Run();

// bootflag.c
#define BOOT_FLAG_LEN		48
#define BOOT_FLAG_ADDR		(FILE_FLAG_PAGE_SIZE - BANK_BASE)

BOOLEAN BootFlag(BOOLEAN bCheck);
void GenerateBootFlag(PCHAR pFlag, PCHAR pMac, PCHAR pProduct);
void CopyBootFlag(PCHAR pFlag, PCHAR pMac, PCHAR pProduct);

// program.c
void RunProgrammer();
void ProgrammerUpdate();
void ProgrammerModifyMac();
void ProgrammerModifyBootFlag();
void ProgrammerDebugMac();

// UI functions, function.c
void UI_Init();
void UI_Start();
void UI_Timer();
void UI_MinuteTimer();
void UI_Run();
void SpeakerPhone(BOOLEAN bHandfree);

void UI_StoreDefaults();
void UI_LoadDefaults();

void UI_StopAlarm();
void UI_EventHandler(UCHAR iType, USHORT sParam);

void UI_HandleKeys(UCHAR iKey);
void UI_HandlePeerKeys(UCHAR iKey);

void UI_StandbyDisplay();
void UI_CallDisplay(UCHAR iState);
void UI_HandsetUp(BOOLEAN bSpeaker);
BOOLEAN UI_IsCallMode();
void UI_ClearDisplay(UCHAR iStartLine);
void UI_UpdateProgress(UCHAR iPage);
void UI_MemCall(PCHAR pNameNumber);
void UI_BuildNumber(PCHAR pOldNumber);

void OnCall();
void OnFlash();
void OnRedial();
#if defined VER_AR168M && defined OEM_BT2008
void OnSpeaker(BOOLEAN bSpk);
#else
void OnSpeaker();
#endif

void OnHandsetDown();

#ifdef KEY_C2
void OnToggleCall();
#endif

void OnHook();
void OnHold();
void OnMute();
void OnMenu();
void OnVolDown();
void OnVolUp();
void OnLeft();
void OnRight();
void OnOK();
void OnCancel();
void OnMessage();
void OnTransfer();
void OnConf();
void OnWake();
void OnDND();
void OnMem(UCHAR iIndex);

void OnIpKey();

void DisplayVal(PCHAR pPrefix, USHORT iVal, UCHAR iLine);
void DisplayString(UCHAR iIDS, UCHAR iLine);
void DisplayProgress(UCHAR iLen, UCHAR iLine);
void DisplayClear(UCHAR iLine);

void KeyClear();

void SpeakerControlOn();
void SpeakerControlOff();

UCHAR GetDspPreprocess();

BOOLEAN GetPhoneBookNumber(UCHAR iIndex, PCHAR pBuf);
void GetPhoneBookName(UCHAR iIndex, PCHAR pBuf);
void UI_ChangeMode(UCHAR iMode);

#ifdef OEM_IP20
void OnWakeOK();
void LedTest(UCHAR chn1, UCHAR chn2);
void AllDispChar();
void UI_TestFunctionKeys(UCHAR iKey);
extern const UCHAR ledMap[15][2];
#endif

void ManagerReport();

// ivr.c
#ifdef SYS_IVR
#define IVR_TYPE_IP		0
void PrepareIVR(UCHAR iType);	// iType from IVR_TYPE_XXXX
#endif

// led.c
#define LED_LCD			0
#define LED_KEYPAD		1
#define LED_MESSAGE		2
#define LED_MUTE		3
#define LED_HOLD		4
#define LED_TRANSFER	5
#define LED_ACTIVE		6
#define LED_RINGING		7
#define LED_LOGON		8

void LED_Init();
void LED_On(UCHAR iIndex);
void LED_Off(UCHAR iIndex);
void LED_Blink(UCHAR iIndex);
void LED_SetState(UCHAR iIndex, BOOLEAN bOn);

void IP20_LED_On(PCHAR pMatchNumber);
void IP20_LED_Off();

// date and time functions, time.c
void ConvertTime(ULONG lTime, TIME_T * ptm);
//void DisplayTime(ULONG lTime, UCHAR iLine);
void DisplayTime(TIME_T * ptm, UCHAR iLine);
void DisplayCallTime(CALL_TIME * pct, UCHAR iLine);
ULONG CalcIax2Time(PCHAR pParam);

// test.c
void UI_Test(UCHAR iGroup, UCHAR iItem);

void LcdFactoryTest();
void DspTest(UCHAR iType);
void DspTestEcho();

// sys.c

#define SST_SECURITY_ID_LEN		0x10

void SystemModifyPage(UCHAR iPage);
void SystemUpdateFlag(UCHAR iPage);
void SystemCheckSecurityID();
void SystemCalcSecurityID(PCHAR pMac, PCHAR pID);

BOOLEAN SystemHandler(UCHAR iType, UCHAR iParam);

void OptionsInit();

void JumpMain();
BOOLEAN DetectSafeRecoveryMode(USHORT sMillSeconds);

// serial.c
void SerialInit();
void SerialRun();
void SerialSend(PCHAR pBuf, UCHAR iLen);
void SerialSendString(PCHAR pStr);

void SerialUI_Event(UCHAR iType);
void SerialUI_DisplayMsg(PCHAR pMsg);

// uart.c
void UART_Init(UCHAR iBaudRate, BOOLEAN bHalfFull);	// Baudrate table: 13[115200], 26[57600], 39[38400], 52[28800], 78[19200], 104[14400], 156[9600], 208[7200]
/*
The function checks if there is enough space in the transmit FIFO . 
If there is enough it puts the byte in the buffer and returns TRUE. Else it return FALSE. The byte should be transmitted immediately without using any interrupt.
*/
BOOLEAN UART_SendByte(UCHAR chr);
UCHAR UART_RecvByte(UCHAR * chr);


void SPI_Init();
BOOLEAN SPI_SendByte(UCHAR iByte);

