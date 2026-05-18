//------------------------------------------------------------------------------------------------------------------------------
//  Copyright (C) 2021, WESS-Global Inc., All Rights Reserved.
//
//  The information contained herein is confidential property of WESS-Global Inc. The use, copying, transfer or disclosure of
//  such information is prohibited except by express written agreement with WESS-Global Co.,LTD
//
//  $ Date: 2020-07-13 12:00:00 +0900 $
//------------------------------------------------------------------------------------------------------------------------------


//------------------------------------------------------------------------------------------------------------------------------
//  Includes
//------------------------------------------------------------------------------------------------------------------------------
// bsp
#include "bsp_rtc.h"
#include "bsp_uart.h"
#include "Bsp_timer.h"
// app
#include "app_status.h"
#include "app_boot.h"
#include "app_schd1sec.h"
// function - input
#include "input_main.h"
#include "input_key.h"
// function - data
#include "data_main.h"
#include "data_save.h"
#include "data_commu.h"
// measure
#include "measure_temperature.h"
#include "measure_main.h"
#include "output_main.h"
// screen
#include "screen_main.h"
// menu
#include "menu_main.h"
#include "menu_factory.h"
#include "menu_measure.h"
#include "menu_test.h"
#include "menu_engineer.h"
#include "measure_cal1_field0.h"
#include "stm32f7xx_hal_gpio.h"
// display
#include "disp_main.h"
#include "disp_string.h"
#include "disp_boot.h"
#include "disp_color.h"
#include "disp_figure.h"
// self
#include "app_main.h"

#include "bsp_nand.h"
#include "menu_lyr2_item.h"
#include "screen_echo.h"

#include "measure_calc.h"
#include "bsp_ram.h"
#include "output_pcd.h"
#include "bsp_adc.h"
#include "bsp_ota.h"
//------------------------------------------------------------------------------------------------------------------------------
//  Global variables
//------------------------------------------------------------------------------------------------------------------------------


//------------------------------------------------------------------------------------------------------------------------------
//  Local variables
//------------------------------------------------------------------------------------------------------------------------------
APP_LS lApp;

extern U16 aaaa = 0;
//------------------------------------------------------------------------------------------------------------------------------
//  Local Funtions - RTC
//------------------------------------------------------------------------------------------------------------------------------
void ApRtc_FtrRst(void)
{
	lApp.tm.year  = 20;
	lApp.tm.month = 7;
	lApp.tm.day   = 13;
	lApp.tm.hours = 8;
	lApp.tm.minutes = 14;
	lApp.tm.seconds = 5;
	RTC_SetTime(lApp.tm);
}


//------------------------------------------------------------------------------------------------------------------------------
//  Local Funtions - Mode Proccessor
//------------------------------------------------------------------------------------------------------------------------------
void ApRun_ProcMode(void)
{
	switch(lApp.mode)
	{
		case APP_MD0_SCRN:		SCRN_ProcMain();					break;
		case APP_MD1_MENU:		MENU_ProcMain();					break;
		default:				lApp.mode = APP_MD0_SCRN;			break;
	}

	lApp.fRun = TRUE;
}

void ApRun_MsrStart(U08 iCh, U08 fClr)
{
	if((iCh == APP_CH_2) && (MnFTR_PrGet_SsChn() == MnFTR_SS_SINGLE))
		return;

	// Measure - Sensor Tx PWM & Rx ADC
	MsSEN_ProcMain(iCh);
	// Timer Enable
	TMR_HalEnable(_F_T, fClr);
}


void ApRun_SettingStart(U08 iCh, U08 fClr)
{
	if((iCh == APP_CH_2) && (MnFTR_PrGet_SsChn() == MnFTR_SS_SINGLE))
		return;

	// Measure - Sensor Tx PWM & Rx ADC
	MsSEN_ProcMain(iCh);
	// Timer Enable
	TMR_HalEnable(_F_T, fClr);
}


void ApRun_FieldStart(U08 iCh, U08 fClr)
{
	if((iCh == APP_CH_2) && (MnFTR_PrGet_SsChn() == MnFTR_SS_SINGLE))
		return;

	// Measure - Sensor Tx PWM & Rx ADC
	MsSEN_FieldProcMain(iCh);
	// Timer Enable
	TMR_HalEnable(_F_T, fClr);
}

#if 0
I08 sDbg[128];
_SPRINTF(sDbg, "%d ", gTmr.c.c100ms);
DpFIG_DrwRect(150, 60, 400, 25, _cBGD_SCRN, DpFIG_FILL);
DpSTR_GuiLeft(150, 60, _cECO_LVL_TITL, _cBGD_SCRN, _fE17HsB, sDbg); 	
#endif

//------------------------------------------------------------------------------------------------------------------------------
//  Local Funtions - Task
//------------------------------------------------------------------------------------------------------------------------------
void ApTsk_ProcT0(void)
{	
	U16 i,j;
	U08 ch= APP_CH_1;

	if(MnMSR_CalGet_Ch_Value(ch, MnMS1_OPT_SINGLE_AUTO_FAMP) == MnMS1_AUTO_FAMP_OFF)
	{
		lMsCal.fauto_gain[ch] = 0;
		
		ApRun_MsrStart(ch, _F_F);
		MsTPR_ProcCalc(ch);
	
		MsCAL_CheckWeak(ch);
	} 
	else if(MnMSR_CalGet_Ch_Value(ch, MnMS1_OPT_SINGLE_AUTO_FAMP) == MnMS1_AUTO_FAMP_ON) 
	{
		

		//U16 a=gTmr.c.c001ms_cnt;
		if(ApSTT_GetStatus(ch)==ApSTT_MSR_OK)
		{
			lMsCal.fauto_gain[ch] = 1;
		
			for(i=0; i<20; i++)
			{
				U16 gain = MnMSR_CalGet_Ch_Value(ch, MnMS1_OPT_SINGLE_ECHO_AMP);
				lMsCal.auto_calib = gain + ((i + 1) / 2) * (1 - 2 * (i & 1));  

				if(lMsCal.auto_calib>=50)
					lMsCal.auto_calib = 50;
				if(lMsCal.auto_calib<=1)
					lMsCal.auto_calib = 1;
				
				lMsCal.auto_calib_cnt = i;
				
				ApRun_MsrStart(ch, _F_F);		

				if(i==0)
				{
				
					MsTPR_ProcCalc(ch); 
					MsCAL_CheckWeak(ch);

				}
				
				MsCAL_Auto_Gain_Real(ch);
				HAL_Delay(1);
				DaMdb_ProcMain();
			}
			//ApSCD_Proc1Sec(APP_CH_1);
		}
		else
		{
			lMsCal.fauto_gain[ch] = 0;

			ApRun_MsrStart(ch, _F_F);
			MsTPR_ProcCalc(ch);
		
			MsCAL_CheckWeak(ch);
		
		}
	}

	DaMdb_ProcMain();
}

void ApTsk_ProcT1(void)
{	
#if 0
	if(MnFTR_PrGet_SsChn()==MnFTR_SS_DUAL)
	{
		ApSCD_Proc1Sec(APP_CH_2);
		CalPrc_SttCheck(APP_CH_2);
	}
#endif
	U16 i,j;
	U08 ch= APP_CH_2;

	if(MnMSR_CalGet_Ch_Value(ch, MnMS1_OPT_SINGLE_AUTO_FAMP) == MnMS1_AUTO_FAMP_OFF)
	{
		lMsCal.fauto_gain[ch] = 0;
		
		ApRun_MsrStart(ch, _F_F);
		MsTPR_ProcCalc(ch);
	
		MsCAL_CheckWeak(ch);
	} 
	else if(MnMSR_CalGet_Ch_Value(ch, MnMS1_OPT_SINGLE_AUTO_FAMP) == MnMS1_AUTO_FAMP_ON) 
	{
		

		//U16 a=gTmr.c.c001ms_cnt;
		if(ApSTT_GetStatus(ch)==ApSTT_MSR_OK)
		{
			lMsCal.fauto_gain[ch] = 1;
		
			for(i=0; i<20; i++)
			{
				U16 gain = MnMSR_CalGet_Ch_Value(ch, MnMS1_OPT_SINGLE_ECHO_AMP);
				lMsCal.auto_calib = gain + ((i + 1) / 2) * (1 - 2 * (i & 1));  

				if(lMsCal.auto_calib>=50)
					lMsCal.auto_calib = 50;
				if(lMsCal.auto_calib<=1)
					lMsCal.auto_calib = 1;
				
				lMsCal.auto_calib_cnt = i;
				
				ApRun_MsrStart(ch, _F_F);		

				if(i==0)
				{
				
					MsTPR_ProcCalc(ch); 
					MsCAL_CheckWeak(ch);

				}
				
				MsCAL_Auto_Gain_Real(ch);
				HAL_Delay(1);
				DaMdb_ProcMain();
			}
			//ApSCD_Proc1Sec(APP_CH_1);
		}
		else
		{
			lMsCal.fauto_gain[ch] = 0;

			ApRun_MsrStart(ch, _F_F);
			MsTPR_ProcCalc(ch);
		
			MsCAL_CheckWeak(ch);
		
		}
	}

	DaMdb_ProcMain();
}


void ApTsk_ProcT2(void)
{
	if(MnEGN_PrGet_Item(MnEGN_OPT_MSR)==MnEGN_MSR_ON)
	{
		ApRun_FieldStart(APP_CH_1, _F_F);
		ApSCD_ProcFieldA1Sec(APP_CH_1);
	}
}

void ApTsk_ProcT3(void)
{
	if(MnEGN_PrGet_Item(MnEGN_OPT_MSR)==MnEGN_MSR_ON)
	{
		ApRun_FieldStart(APP_CH_2, _F_F);
		ApSCD_ProcFieldA1Sec(APP_CH_2);
	}
}

void ApTsk_ProcT4(void)
{
	if(MnEGN_PrGet_Item(MnEGN_OPT_MSR)==MnEGN_MSR_ON)
	{
		MsCal1_ProcDetect(APP_CH_1);
		MsCal1_ProcDetect(APP_CH_2);

		MsCal1_Hunting_Proc();

		MsCal1_ProcJudge();
		MsCal1_CalcResult();
		//MsCalPrc_DualParam();	
	}
}

void ApTsk_ProcT5(void)
{
	 U16 a=gTmr.c.c001ms_cnt;

	 ApSCD_Proc1Sec(APP_CH_1);
	 CalPrc_SttCheck(APP_CH_1);


	 aaaa = gTmr.c.c001ms_cnt-a;


	#if 0
	 if((MnMSR_CalGet_Ch_Value(APP_CH_1, MnMS1_OPT_SINGLE_AUTO_FAMP)== MnMS1_AUTO_FAMP_ON) &&(ApSTT_GetStatus(APP_CH_1)==ApSTT_MSR_OK))
	 {
		ApSCD_Proc1Sec(APP_CH_1);
		CalPrc_SttCheck(APP_CH_1);
	 }
	 #endif

}

void ApTsk_ProcT6(void)
{
	ApSCD_Proc1Sec(APP_CH_2);
	CalPrc_SttCheck(APP_CH_2);
	#if 0
	 if((MnMSR_CalGet_Ch_Value(APP_CH_2, MnMS1_OPT_SINGLE_AUTO_FAMP)== MnMS1_AUTO_FAMP_ON) &&(ApSTT_GetStatus(APP_CH_2)==ApSTT_MSR_OK))
	 {
		ApSCD_Proc1Sec(APP_CH_2);
		CalPrc_SttCheck(APP_CH_2);

	 }
	 else
	 {
	 	ApSCD_Proc1Sec(APP_CH_2);
	 	CalPrc_SttCheck(APP_CH_2);

	 }
	 #endif
	ApSCD_Intv(APP_CH_1);
	ApSCD_Intv(APP_CH_2);



}

void ApTsk_ProcT7(void)
{

}

void ApTsk_ProcT8(void)
{
}

void ApTsk_ProcT9(void)
{

}

void ApTsk_ProcAdd1(void)
{
	U08 iIt = MnLY2_GetIdxItem(); 
	U08 ch;

	if(iIt == MnTST_OPT_CH1_AUTO_CAL)
		ch = APP_CH_1;
	else if(iIt == MnTST_OPT_CH2_AUTO_CAL)
		ch = APP_CH_2;
	else
		return;

	switch(OuCUR_GetPCD_fRun())
	{
		case PCD_OFF:	break;
		case PCD_ON:	
		case PCD_TEST:	return;
		default:
			return;	
	}


	ApRun_MsrStart(ch, _F_F);
	DaMdb_ProcMain();
	//MsCAL_ProcReal(ch);
	MsCAL_ProcAutoCalib(ch);
	DISP_ProcFlipFrm();	
}

//------------------------------------------------------------------------------------------------------------------------------
//  Local Funtions - Scheduler
//------------------------------------------------------------------------------------------------------------------------------
void AppRun_010ms(void)
{

}

void AppRun_050ms(void)
{
	INPU_ProcMain();
}

void AppRun_100ms(void)
{		
	if(ScECH_GetType() == ScECO_TYPE_MSR_ECHO)
		ApTsk_ProcAdd1();
	else
	{
		#if 0
		switch(gTmr.c.c100ms)
		{
			case	APP_SC0_START:		ApTsk_ProcT0(); 		break;
			case	APP_SC1_:			ApTsk_ProcT1(); 		break;
			case	APP_SC2_:			ApTsk_ProcT2(); 		break;
			case	APP_SC3_:			ApTsk_ProcT3(); 		break;
			case	APP_SC4_:			ApTsk_ProcT4(); 		break;
			case	APP_SC5_S1_MEAS:	ApTsk_ProcT5();			break;
			case	APP_SC6_:			break;
			case	APP_SC7_:			break;
			case	APP_SC8_:			break;
			case	APP_SC9_:			break;
			case	APP_SCD_NUM:		break;
		}
		#endif
		switch(gTmr.c.c100ms)
		{
			case	APP_SC0_START:		ApTsk_ProcT0(); 		 break;
			case	APP_SC1_:			ApRun_ProcMode();		DISP_ProcFlipFrm();		break;
			case	APP_SC2_:			ApTsk_ProcT1(); 		break;
			case 	APP_SC3_:			ApRun_ProcMode();		DISP_ProcFlipFrm();		break;
			case	APP_SC4_:			ApTsk_ProcT2(); 		ApTsk_ProcT3();  break;
			case	APP_SC5_S1_MEAS:	ApRun_ProcMode();		DISP_ProcFlipFrm();		break;
			case	APP_SC6_:			ApTsk_ProcT4(); 		ApTsk_ProcT5();	break;
			case	APP_SC7_:			ApRun_ProcMode();		DISP_ProcFlipFrm();		break;
			case	APP_SC8_:			ApTsk_ProcT6();			break;
			case	APP_SC9_:			ApRun_ProcMode();		DISP_ProcFlipFrm();		break;
			case	APP_SCD_NUM:		break;
		}		
	}

	//for(U08 i=0; i<10; i++)
	//	NND_Echo_Save(APP_CH_2);

	gTmr.c.c100ms++;
	
	DaMdb_ProcMain();
	DaBT_ProcMain();
	INPU_ProcMain();

	if(gTmr.f.b.f100ms_01sec==TRUE)
	{
		if(gTmr.c.c100ms >= APP_SCD_NUM)
		{
			gTmr.c.c100ms = APP_SC0_START;
			gTmr.f.b.f100ms_01sec= FALSE;
		}
	}
}

void AppRun_200ms(void)
{
	

}

void AppRun_500ms(void)
{
	ApSTT_ProcMain();
	DaCOM_ProcMain();
}

void AppRun_01sec(void)
{

	DaSAV_ProcSave();
	DaMdb_ProcMain();
	OUTP_ProcMain();
	Scd1Sec_ScrnRtnTime();

	NND_Echo_ProcSave();




	//NND_Echo_Save(APP_CH_1);
	//NND_Echo_Save(APP_CH_2);
	//NND_Delete_Main();
	//NND_Echo_Read(APP_CH_2);

	//NND_Echo_Read(APP_CH_2);


}

void AppRun_Every(void)
{
	RTC_TM rtc = RTC_GetTime();

	if(lApp.tm.seconds == rtc.seconds)
		return;
	lApp.tm = rtc;
	if(++gTmr.c.c01sec>0xfffffff0)
		gTmr.c.c01sec = 0;
	U64 cyc = MsCAL1_Getrst_cyc();
	
	gTmr.c.timer1[APP_CH_1]++;
	gTmr.c.timer1[APP_CH_2]++;

	cyc++;
	MsCAL1_Setrst_cyc(cyc);
	
	TMR_HalEnable(_F_T, _F_F);
}


//------------------------------------------------------------------------------------------------------------------------------
//  Local Funtions - Processor
//------------------------------------------------------------------------------------------------------------------------------
void AppProc_Boot(void)
{
	lApp.fRun = TRUE;
	INPU_ProcMain();

	U08 evt = INPU_GetKeyEvt();
	U08 key = INPU_GetKeyIdx();

	if(BOOT_ProcMain()==BOOT_ST02_DONE)
	{		
		switch(evt)
		{
			case KEY_EVT_PUSH: 
			case KEY_EVT_DTT_L: 
			case KEY_EVT_SHORT: 
			case KEY_EVT_REPEAT: 
			case KEY_EVT_LONG: 					
				if(key==(KEY_IDX_PREV|KEY_IDX_MENU|KEY_IDX_NEXT))
					APP_FtrReset();				
				break;
			default:
				break;
		}

		HAL_Delay(1000);   // BLE 모듈 부팅 대기
		DaBT_InItMain();   // BLE module initialization (AT commands)
		lApp.proc = APP_PR1_RUN_;
		lApp.mode = APP_MD0_SCRN;
	}

	//lApp.fRun = FALSE;
}

void AppProc_Run(void)
{
	AppRun_Every();
	DaMdb_ProcMain();

	if(gTmr.f.b.f010ms)		{	gTmr.f.b.f010ms = FALSE;	AppRun_010ms();		}
	if(gTmr.f.b.f050ms)		{	gTmr.f.b.f050ms = FALSE;	AppRun_050ms();		}
	if(gTmr.f.b.f100ms)		{	gTmr.f.b.f100ms = FALSE;	AppRun_100ms();		}
	if(gTmr.f.b.f500ms)		{	gTmr.f.b.f500ms = FALSE;	AppRun_500ms();		}
	if(gTmr.f.b.f01sec)		{	gTmr.f.b.f01sec = FALSE;	AppRun_01sec();		}
}


//------------------------------------------------------------------------------------------------------------------------------
//  Global APIs - RTC Time
//------------------------------------------------------------------------------------------------------------------------------
// Get
RTC_TM APP_RtcGetTime(void)			{	return RTC_GetTime();			}
// Set
void APP_RtcSetTime(RTC_TM tm)		{	RTC_SetTime(tm);				}


//------------------------------------------------------------------------------------------------------------------------------
//  Global APIs - Access Local Variables
//------------------------------------------------------------------------------------------------------------------------------
// Get
U08 APP_GetMode(void)			{	return lApp.mode;			}
U08 APP_GetFgRun(void)			{	return lApp.fRun;			}
// Set
void APP_SetMode(U08 mode)		{	lApp.mode = mode;			}


//------------------------------------------------------------------------------------------------------------------------------
//  Global APIs - Basic
//------------------------------------------------------------------------------------------------------------------------------
void APP_InitMain(void)
{
	lApp.proc = APP_PR0_BOOT;
	lApp.mode = APP_MD0_SCRN;

	lApp.fRun = FALSE;
	lApp.tm.seconds = 0;

	ApSTT_InitMain();

	INPU_InitMain();
	MnFTR_InitMain(); 
	OUTP_InitMain();
	DATA_InitMain();
	DaSAV_InitVari();
	DISP_InitMain();
	SCRN_InitMain();
	MENU_InitMain();
	MEAS_InitMain();
	DAC_InitVari();
	DaCOM_InitMain();
		ota_init();


	ApSCD_Init1Sec(APP_CH_1);
	ApSCD_Init1Sec(APP_CH_2);

}

void APP_FtrReset(void)
{
	DpBOO_FtrResetDisp();

	ApRtc_FtrRst();

	SCRN_FtrReset();
	MENU_PrRst_Factory();
	CalPrc_ResetFlag(APP_CH_1);
	CalPrc_ResetFlag(APP_CH_2);
	DaSAV_DeleteData();
	
	MRM_WrByte(_mAUTO_GAIN_CH1_FSLOPE,	1);
	MRM_WrWord(_mAUTO_GAIN_CH1_BEFORE_SLOPE_L,	0);
	MRM_WrWord(_mAUTO_GAIN_CH1_SLOPE_XRANGE_L,	0);
	
	MRM_WrByte(_mAUTO_GAIN_CH2_FSLOPE,	1);
	MRM_WrWord(_mAUTO_GAIN_CH2_BEFORE_SLOPE_L,	0);
	MRM_WrWord(_mAUTO_GAIN_CH2_SLOPE_XRANGE_L,	0);

	NND_Delete_Main(APP_CH_1);
	NND_Delete_Main(APP_CH_2);
}

void APP_LoopMain(void)
{
	switch(lApp.proc)
	{
		case APP_PR0_BOOT:		AppProc_Boot();					break;
		case APP_PR1_RUN_:		AppProc_Run();					break;
		default:				lApp.proc = APP_PR0_BOOT;		break;
	}
}


//------------------------------------------------------------------------------------------------------------------------------
//  EOF
//------------------------------------------------------------------------------------------------------------------------------


