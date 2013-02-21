// Port defines, * indicate difference between old and new ar1688, # indicate new position or new register
__sfr __at 0x00 rClockControl;					// *
__sfr __at 0x01 rEM_LowPage;
__sfr __at 0x02 rEM_HighPage;
__sfr __at 0x04 rA15Control;					// * bit 4:3 select DMA clock source
__sfr __at 0x05 rSRAM_Page;

__sfr __at 0x06 rDMA1_SrcAddr0;
__sfr __at 0x07 rDMA1_SrcAddr1;
__sfr __at 0x08 rDMA1_SrcAddr2;
__sfr __at 0x09 rDMA1_SrcAddr3;
__sfr __at 0x0a rDMA1_SrcAddrPage;
__sfr __at 0x0b rDMA1_DstAddr0;
__sfr __at 0x0c rDMA1_DstAddr1;
__sfr __at 0x0d rDMA1_DstAddr2;
__sfr __at 0x0e rDMA1_DstAddr3;
__sfr __at 0x0f rDMA1_DstAddrPage;
__sfr __at 0x10 rDMA1_CountLow;
__sfr __at 0x11 rDMA1_CountHigh;
__sfr __at 0x12 rDMA1_Mode;
__sfr __at 0x13 rDMA1_Command;

__sfr __at 0x14 rDMA2_SrcAddr0;
__sfr __at 0x15 rDMA2_SrcAddr1;
__sfr __at 0x16 rDMA2_SrcAddr2;					// *, bit 7 changed
__sfr __at 0x17 rDMA2_SrcAddr3;
__sfr __at 0x18 rDMA2_SrcAddrPage;
__sfr __at 0x19 rDMA2_DstAddr0;
__sfr __at 0x1a rDMA2_DstAddr1;
__sfr __at 0x1b rDMA2_DstAddr2;
__sfr __at 0x1c rDMA2_DstAddr3;
__sfr __at 0x1d rDMA2_DstAddrPage;
__sfr __at 0x1e rDMA2_CountLow;
__sfr __at 0x1f rDMA2_CountHigh;
__sfr __at 0x20 rDMA2_Mode;
__sfr __at 0x21 rDMA2_Command;

__sfr __at 0x22 rCTC_Prescale;
__sfr __at 0x23 rCTC_PeriodLow;
__sfr __at 0x24 rCTC_PeriodHigh;

__sfr __at 0x25 rDMA_CTC_IntStatus;
__sfr __at 0x26 rIntStatus;
__sfr __at 0x27 rIntEnable;

__sfr __at 0x2e rDSP_Control_Status;
__sfr __at 0x2f rDSP_BootMode;
__sfr __at 0x30 rDSP_HDR0;
__sfr __at 0x31 rDSP_HDR1;
__sfr __at 0x32 rDSP_HDR2;
__sfr __at 0x33 rDSP_HDR3;
__sfr __at 0x34 rDSP_HDR4;
__sfr __at 0x35 rDSP_HDR5;
__sfr __at 0x36 rDSP_HSR6;
__sfr __at 0x37 rDSP_HSR7;

__sfr __at 0x3d	tDMA_ClockControl;				// #

__sfr __at 0x40 rHighFrequencyCrystalControl;	// *
__sfr __at 0x42 rPLL_Control;					// *, bit 6:2 select DSP MIPS, High Frequency Crystal Oscillator enable changes to 0x40

__sfr __at 0x43 rRTC_Control;
__sfr __at 0x44 rRTC_IRQ_Status;
__sfr __at 0x4b rLOSC_DividerLow;
__sfr __at 0x4c rLOSC_DividerMiddle;
__sfr __at 0x4d rLOSC_DividerHigh;
__sfr __at 0x4e rWatchDog;

__sfr __at 0x4f rDC_Control;					// *, bit definition changed

__sfr __at 0x70 rClockControl2;					// *

__sfr __at 0x72 rUART_BaudRate;
__sfr __at 0x73 rUART_Control;
__sfr __at 0x74 rUART_Data;

__sfr __at 0x77 rLRADC2_Data;					// #, new reg

__sfr __at 0x78 rUART_FIFO_Status;
__sfr __at 0x79 rUART_IRQ;

__sfr __at 0x7a rI2C_Control;
__sfr __at 0x7b rI2C_Status;
__sfr __at 0x7c rI2C_Data;

__sfr __at 0x7d rSPI_Control;
__sfr __at 0x7e rSPI_Clock;
__sfr __at 0x7f rSPI_Data;

__sfr __at 0x80 rDAC_Control;
__sfr __at 0x81 rDAC_SampleRateControl;			// *, sample rate bits definition changed

__sfr __at 0x82 tDAC_PCM_OutLow;				// #, new position
__sfr __at 0x83 tDAC_PCM_OutMid;				// #, new position
__sfr __at 0x84 tDAC_PCM_OutHigh;				// #, new position
__sfr __at 0x85 tDAC_FIFO_Status;				// #, new position
__sfr __at 0x86 tDAC_DitherControl;				// #, new position
__sfr __at 0x87 tAnalogOutputVolumeControl;		// #, new position	
__sfr __at 0x88 tDAC_AnalogBlockControl;		// #, new position

__sfr __at 0x88 rDAC_PCM_OutLow;				// *, position moved
__sfr __at 0x89 rDAC_PCM_OutMid;				// *, position moved
__sfr __at 0x8a rDAC_PCM_OutHigh;				// *, position moved
__sfr __at 0x8b rDAC_FIFO_Status;				// *, position moved

__sfr __at 0x8c tUSB_GlobalControl;				// #

__sfr __at 0x8c rDMA4_DstAddrHigh;
__sfr __at 0x8d rDMA4_Control_Status;

__sfr __at 0x8e rGPIO_K_Enable;
__sfr __at 0x8f rGPIO_K;

__sfr __at 0x91 rDAC_DitherControl;

__sfr __at 0x9a rAudioADC;
__sfr __at 0x9b rAudioADC_BusControl;			// *, added LRADC FS select
__sfr __at 0x9e rAnalogOutputVolumeControl;		// *, position moved	
__sfr __at 0x9f rAudioADC_Modulator;

__sfr __at 0xad tSerialLCM_Control;				// #

__sfr __at 0xc0 rKeyScan;
__sfr __at 0xc1 rKeyScanControl;

__sfr __at 0xd0 rLRADC_Control;					// *, all definition changed

__sfr __at 0xd1 rSARADC_Control;				// *, all definition changed
__sfr __at 0xd2 rTouchPanelEnable;

__sfr __at 0xd3 rAudioADC_Performance;
__sfr __at 0xd4 rAudioADC_FirstControl;
__sfr __at 0xd5 rAudioADC_SecondControl;
__sfr __at 0xd6 rAnalogInputGainControl;
__sfr __at 0xd7 rAudioADC_FIFO_Control;

__sfr __at 0xd8 rLRADC_Data;

__sfr __at 0xd9 tAD_LPF;						// #, new reg

__sfr __at 0xdd rDAC_AnalogBlockControl;		// *, position moved
__sfr __at 0xde rADC_IRQ_Control;
__sfr __at 0xdf rPowerControl;					// *

__sfr __at 0xee rGPIO_A;						// *, added GPIO_A3

__sfr __at 0xef rGPIO_B_Config;
__sfr __at 0xf0 rGPIO_B_Output_Enable;
__sfr __at 0xf1 rGPIO_B_Input_Enable;
__sfr __at 0xf2 rGPIO_B;

__sfr __at 0xf3 rGPIO_C_Enable;
__sfr __at 0xf4 rGPIO_C;

__sfr __at 0xf5 rGPIO_D_Output_Enable;
__sfr __at 0xf6 rGPIO_D_Input_Enable;
__sfr __at 0xf7 rGPIO_D;

__sfr __at 0xf8 rGPIO_E_Output_Enable;
__sfr __at 0xf9 rGPIO_E_Input_Enable;
__sfr __at 0xfa rGPIO_E;

__sfr __at 0xfe rGPIO_G_Enable;
__sfr __at 0xff rGPIO_G;


