//------------------------------------------------------------------------------------------------------------------------------
//  Copyright (C) 2022, WESS-Global Inc., All Rights Reserved.
//
//  The information contained herein is confidential property of WESS-Global Inc. The use, copying, transfer or disclosure of
//  such information is prohibited except by express written agreement with WESS-Global Co.,LTD
//
//  $ Date: 2022-01-20 12:00:00 +0900 $ (From 2020-07-13)
//------------------------------------------------------------------------------------------------------------------------------


//------------------------------------------------------------------------------------------------------------------------------
//  Includes
//------------------------------------------------------------------------------------------------------------------------------
// app
#include "app_main.h"
// input
#include "input_main.h"
#include "input_key.h"
// display
#include "disp_string.h"
// disp - screen
#include "dp_screen_main.h"
#include "dp_screen_echo.h"
// screen
#include "screen_main.h"
#include "screen_echo.h"
// menu
#include "menu_engineer.h"
// self
#include "screen_eco0_func.h"

#include "bsp_nand.h"
#include "dp_menu_main.h"
#include "menu_lyr3_value.h"
#include "disp_titlebar.h"
#include "disp_color.h"
#include "menu_data.h"
#include "disp_popup.h"
//------------------------------------------------------------------------------------------------------------------------------
//  Global variables
//------------------------------------------------------------------------------------------------------------------------------


//------------------------------------------------------------------------------------------------------------------------------
//  Local variables
//------------------------------------------------------------------------------------------------------------------------------
ScL0F_LS lScEc0;


//------------------------------------------------------------------------------------------------------------------------------
//  Local Functions - Event - Key Action
//------------------------------------------------------------------------------------------------------------------------------
void ScEc0Evt_KeyMenu(U08 evt)
{
	DpSTR_DebugExp(1, 1, "[ScEC0] Goto Menu");
	INPU_ClrKeyEvt(evt);


	switch(ScECH_GetType())
	{
		case ScECO_TYPE_MSR_ECHO:
		case ScECO_TYPE_SAVE_ECHO:
			MsCAL_SetFAutoGain(0);
			DpM0_IntroSct();
			DpM1_IntroSub();
			//DpM2_IntroItm();
			MENU_SetLayer(MENU_L2_ITEM);
			MnLY3_InitValue();
			ScECH_SetType(ScECO_TYPE_REAL);	
			lScEc0.stt = SCRN_S0_INTRO;
			DpTTB_UdtIntro(TEXT_LIST_MENU, 300, _cTTB_ST_TITLE);
			break;
		default:
			lScEc0.stt = SCRN_S0_INTRO;
			APP_SetMode(APP_MD1_MENU);			
			break;
	}
}

void ScEc0Evt_KeyPrev(U08 evt)
{
	DpSTR_DebugExp(1, 1, "[ScEC0] Goto ScHYB");
	INPU_ClrKeyEvt(evt);

	switch(ScECH_GetType())
	{
		case ScECO_TYPE_MSR_ECHO:		lScEc0.stt = SCRN_S1_STDBY;		break;
		case ScECO_TYPE_REAL:			SCRN_SetPage(SCRN_P1_HYBRID);	break;
		case ScECO_TYPE_AVG:			ScECH_SetType(ScECO_TYPE_REAL);	break;
		case ScECO_TYPE_SAVE_ECHO:
			if(evt == KEY_EVT_REPEAT)
			{
				if(nnd_echo.num <= 9) nnd_echo.num = NND_Meta_GetEchocnt(nnd_echo.ch)-1;
				else				  nnd_echo.num -= 10;
			}
			else
			{
				if(nnd_echo.num <= 0) nnd_echo.num = NND_Meta_GetEchocnt(nnd_echo.ch)-1;
				else				  nnd_echo.num -= 1;
			}
			if(MnDAT_EchoSavePrGet_SingleValue(nnd_echo.ch,MnDS1_OPT_SINGLE_ECHO_VIEW_MODE) == MnDS1_ECHO_VIEW_MODE_AUTO_CAL)
				NND_auto_calib_buffer_Read(nnd_echo.num);
			else 
				NND_Echo_buffer_Read(nnd_echo.num);
			break;
		default:						break;
	}
	lScEc0.stt = SCRN_S0_INTRO;
}

void ScEc0Evt_KeyNext(U08 evt)
{
	DpSTR_DebugExp(1, 1, "[ScEC0] Goto ScINF");
	INPU_ClrKeyEvt(evt);

	switch(ScECH_GetType())
	{
		case ScECO_TYPE_MSR_ECHO:	lScEc0.stt = SCRN_S1_STDBY;		break;
		case ScECO_TYPE_REAL:		ScECH_SetType(ScECO_TYPE_AVG);	break;
		case ScECO_TYPE_AVG:		SCRN_SetPage(SCRN_P3_INFO);		break;
		case ScECO_TYPE_SAVE_ECHO:	
			if(evt == KEY_EVT_REPEAT)
			{
				if(nnd_echo.num >= NND_Meta_GetEchocnt(nnd_echo.ch)-10) nnd_echo.num = 0;
				else				   								 nnd_echo.num += 10;				
			}
			else
			{
				if(nnd_echo.num >= NND_Meta_GetEchocnt(nnd_echo.ch)-1) nnd_echo.num = 0;
				else				   								 nnd_echo.num += 1;
			}
			if(MnDAT_EchoSavePrGet_SingleValue(nnd_echo.ch,MnDS1_OPT_SINGLE_ECHO_VIEW_MODE) == MnDS1_ECHO_VIEW_MODE_AUTO_CAL)
				NND_auto_calib_buffer_Read(nnd_echo.num);
			else 
				NND_Echo_buffer_Read(nnd_echo.num);

			break;
		default:				break;
	}

	lScEc0.stt = SCRN_S0_INTRO;
}

void ScEc0Evt_KeyEnter(U08 evt)
{
	DpSTR_DebugExp(1, 1, "[ScEC0] Goto L2");
	INPU_ClrKeyEvt(evt);

	if(ScECH_GetType() == ScECO_TYPE_MSR_ECHO)
	{
		MsCAL_SetFAutoGain(0);
		DpM0_IntroSct();
		DpM1_IntroSub();
		//DpM2_IntroItm();
		MENU_SetLayer(MENU_L2_ITEM);
		MnLY3_InitValue();
		ScECH_SetType(ScECO_TYPE_REAL); 
		lScEc0.stt = SCRN_S0_INTRO;
		DpTTB_UdtIntro(TEXT_LIST_MENU, 300, _cTTB_ST_TITLE);
	}
	else if(ScECH_GetType() == ScECO_TYPE_SAVE_ECHO)
	{
		if(MnDAT_EchoSavePrGet_SingleValue(nnd_echo.ch,MnDS1_OPT_SINGLE_ECHO_VIEW_MODE) == MnDS1_ECHO_VIEW_MODE_AUTO_CAL)
		{
			nand_auto_calib_sel[nnd_echo.ch]++;
			if(nand_auto_calib_sel[nnd_echo.ch]==nand_auto_calib_num[nnd_echo.ch]/300)
				nand_auto_calib_sel[nnd_echo.ch]=0;
			NND_auto_calib_Read(nnd_echo.ch,nand_auto_calib_sel[nnd_echo.ch]);
			nand_auto_calib_sel_cnt[nnd_echo.ch]++;
			if(nand_auto_calib_sel_cnt[nnd_echo.ch]==nand_auto_calib_num[nnd_echo.ch]/300)
				nand_auto_calib_sel_cnt[nnd_echo.ch] = 0;
			lScEc0.stt = SCRN_S0_INTRO;
		}
		else
		{
			lScEc0.stt = SCRN_S0_INTRO;
			ScECH_SetLayer(SCRN_L2_VALU);
		}
		//DpTTB_UdtIntro(TEXT_LIST_MENU, 300, _cTTB_ST_TITLE);
	}
	else
	{
		lScEc0.stt = SCRN_S0_INTRO;
		ScECH_SetLayer(SCRN_L1_LIST);
	}
}

void ScEc0Evt_KeyInvalid(U08 evt)
{
	DpSTR_DebugExp(1, 1, "[ScEC0] EVT (Invalid)");
	INPU_ClrKeyEvt(evt);

	lScEc0.stt = SCRN_S1_STDBY;
}


//------------------------------------------------------------------------------------------------------------------------------
//  Local Funtions - Status Machine
//------------------------------------------------------------------------------------------------------------------------------
void ScEc0Stt_S0Intro(U08 evt)
{
	INPU_ClrKeyEvt(evt);
	ScECH_InitMain();
	DpEC0_FncIntro();

	if(ScECH_GetType()==ScECO_TYPE_SAVE_ECHO)
		DpSCR_UpdtBttn(SCRN_L0_FUNC);
	else
		DpSCR_UpdtBttn(SCRN_L0_FUNC);

	lScEc0.stt = SCRN_S1_STDBY;
}

void ScEc0Stt_S1StdBy(U08 evt)
{
	switch(evt)
	{
		case KEY_EVT_PUSH:		DpSTR_DebugExp(0, 1, "[ScEC0] Key - PUSH");			break;
		case KEY_EVT_DTT_L:		DpSTR_DebugExp(0, 1, "[ScEC0] Key - DTT_L");		break;
		case KEY_EVT_SHORT:		DpSTR_DebugExp(0, 1, "[ScEC0] Key - SHORT");		break;
		case KEY_EVT_REPEAT:	DpSTR_DebugExp(0, 1, "[ScEC0] Key - REPEAT");		break;
		case KEY_EVT_LONG:		DpSTR_DebugExp(0, 1, "[ScEC0] Key - LONG");			break;
		default:
			DpEC0_FncStdBy(0);
			if(lScEc0.fSb)
				return;
			lScEc0.fSb = _F_T;

			DpSTR_DebugExp(0, 1, "[ScEC0] Key - Pending");
			return;
	}

	lScEc0.stt = SCRN_S2_DTT_E;
	lScEc0.fSb = _F_F;
}

void ScEc0Stt_S2DttEv(U08 evt, U08 key)
{
	switch(evt)
	{
		case KEY_EVT_REPEAT:
			switch(key)
			{
				case KEY_IDX_PREV:		ScEc0Evt_KeyPrev(evt);			break;
				case KEY_IDX_NEXT:		ScEc0Evt_KeyNext(evt);			break;
				default:				ScEc0Evt_KeyInvalid(evt);		break;
			}
			break;
		case KEY_EVT_LONG:
		case KEY_EVT_SHORT:
			switch(key)
			{
				case KEY_IDX_MENU:		ScEc0Evt_KeyMenu(evt);			break;
				case KEY_IDX_PREV:		ScEc0Evt_KeyPrev(evt);			break;
				case KEY_IDX_NEXT:		ScEc0Evt_KeyNext(evt);			break;
				case KEY_IDX_ENTER:		ScEc0Evt_KeyEnter(evt);			break;
				default:				ScEc0Evt_KeyInvalid(evt);		break;
			}
			break;

		case KEY_EVT_NONE:		DpSTR_DebugExp(1, 1, "[ScEC0] Evt - NONE");			break;
		default:				DpSTR_DebugExp(1, 1, "[ScEC0] Evt - Default");		break;
	}
}


//------------------------------------------------------------------------------------------------------------------------------
//  Global APIs - Access Local Variables
//------------------------------------------------------------------------------------------------------------------------------
// Get
// Set


//------------------------------------------------------------------------------------------------------------------------------
//  Global APIs - Operation
//------------------------------------------------------------------------------------------------------------------------------
void ScEC0_InitFunc(void)
{
	lScEc0.stt = SCRN_S0_INTRO;
	lScEc0.fSb = _F_F;
}

void ScEC0_ProcFunc(void)
{
	U08 evt = INPU_GetKeyEvt();
	U08 key = INPU_GetKeyIdx();

	switch(lScEc0.stt)
	{
		case SCRN_S0_INTRO:		ScEc0Stt_S0Intro(evt);				break;
		case SCRN_S1_STDBY:		ScEc0Stt_S1StdBy(evt);				break;
		case SCRN_S2_DTT_E:		ScEc0Stt_S2DttEv(evt, key); 		break;
		default:				lScEc0.stt = SCRN_S0_INTRO; 		break;
	}
}


//------------------------------------------------------------------------------------------------------------------------------
//  EOF
//------------------------------------------------------------------------------------------------------------------------------

