//------------------------------------------------------------------------------------------------------------------------------
//  Copyright (C) 2022, WESS-Global Inc., All Rights Reserved.
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
#include "bsp_ram.h"
// app
#include "app_main.h"
#include "app_schd1sec.h"
// 
#include "measure_calc.h"
// function
#include "input_main.h"
#include "input_key.h"
// menu
#include "menu_measure.h"
#include "menu_factory.h"
#include "menu_engineer.h"
// display
#include "disp_string.h"
#include "disp_button.h"
// disp - screen
#include "dp_screen_echo.h"
// screen
#include "screen_main.h"
#include "screen_eco0_func.h"
#include "screen_eco1_list.h"
#include "screen_eco2_value.h"
// self
#include "screen_echo.h"

#include "bsp_nand.h"

#include <stdio.h>
#include <time.h>

//------------------------------------------------------------------------------------------------------------------------------
//  Global variables
//------------------------------------------------------------------------------------------------------------------------------


//------------------------------------------------------------------------------------------------------------------------------
//  Local variables
//------------------------------------------------------------------------------------------------------------------------------
ScECO_LS lScEco;


//------------------------------------------------------------------------------------------------------------------------------
//  Global APIs - Parameters
//------------------------------------------------------------------------------------------------------------------------------
U08 ScECH_PrGet_Chnnl(void)			{	return lScEco.mPr.chnnl;			}
void ScECH_PrSet_Chnnl(U08 ch)			{	lScEco.mPr.chnnl=ch;			}

S32 ScECH_PrGet_Value(U08 iCh, U08 iFn)
{
	switch(lScEco.type)
	{
		case ScECO_TYPE_REAL:
			switch(iFn)
			{	// Page #0
				case ScECO_REAL_ECHO_AMP:		
					return MnMSR_CalGet_Ch_Value(iCh,MnMS1_OPT_SINGLE_ECHO_AMP);
				case ScECO_REAL_MEAS_RESET:
					return MENU_CHK_NO;
				case ScECO_REAL_FREQ:
					return MnMSR_CalGet_Ch_Value(iCh,MnMS1_OPT_SINGLE_FREQ);
				case ScECO_REAL_EMPTY:
					return MnMSR_BaseGet_Ch_Value(iCh,MnMS0_OPT_SINGLE_EMPTY);
				case ScECO_REAL_DEADZONE:
					return MnMSR_BaseGet_Ch_Value(iCh,MnMS0_OPT_SINGLE_DEADZONE);
				case ScECO_REAL_TVG:
					return MnEGN_PrGet_CH_Item(iCh, MnEGN_OPT_SINGLE_TVG);
				default:
					break;
			}
			break;
		case ScECO_TYPE_AVG:
			switch(iFn)
			{
				case ScECO_AVG_THR_LIGHT:
					return MnMSR_Get_Threshold_Ch_Value(iCh,MnMS1_OPT_SINGLE_THR_LIGHT);
				case ScECO_AVG_THR_HEAVY:
					return MnMSR_Get_Threshold_Ch_Value(iCh,MnMS1_OPT_SINGLE_THR_HEAVY);
				case ScECO_AVG_ASF_LIGHT:
				case ScECO_AVG_ASF_HEAVY:			
					return MnMSR_BaseGet_Ch_Value(iCh, MnMS0_OPT_SINGLE_DEADZONE);
				default:
					break;
			}
			break;
		case ScECO_TYPE_SAVE_ECHO:
			switch(iFn)
			{
				case ScECO_TIME_YY:
					return lScEco.yy;
				case ScECO_TIME_MM:
					return lScEco.mm;
				case ScECO_TIME_DD:
					return lScEco.dd;
				case ScECO_TIME_HH:
					return lScEco.hh;
				case ScECO_TIME_MN:
					return lScEco.mn;
			}
			break;
		default:				
			break;
	}


	return MENU_VAL_INVALID;
}

void ScECH_PrSet_Value(S32 val)
{
	U08 iCh = lScEco.mPr.chnnl;
	U08 iFn = ScEC1_GetIdxFunc();

	switch(lScEco.type)
	{
		case ScECO_TYPE_REAL:		
			switch(iFn)
			{
				case ScECO_REAL_CH_SEL:		lScEco.mPr.chnnl = val;										break;
				case ScECO_REAL_ECHO_AMP:	MnMSR_CalSet_Ch_Value(iCh,MnMS1_OPT_SINGLE_ECHO_AMP, val);	CalPrc_ResetFlag(iCh);	lMsCal.auto_gain_fslope[iCh] = 1;break;
				case ScECO_REAL_MEAS_RESET:	if(val==MENU_CHK_YES)	CalPrc_ResetFlag(iCh);				lMsCal.auto_gain_fslope[iCh] = 1;break;
				case ScECO_REAL_FREQ:		MnMSR_CalSet_Ch_Value(iCh, MnMS1_OPT_SINGLE_FREQ, val);		break;
				case ScECO_REAL_EMPTY:		MnMSR_BaseSet_Ch_Value(iCh, MnMS0_OPT_SINGLE_EMPTY, val);	break;
				case ScECO_REAL_DEADZONE:	MnMSR_BaseSet_Ch_Value(iCh, MnMS0_OPT_SINGLE_DEADZONE,val);	break;
				case ScECO_REAL_TVG:		MnEGN_PrSet_Ch_Value(iCh, MnEGN_OPT_SINGLE_TVG,val);		break;
				default:
					break;
			}
			break;
		case ScECO_TYPE_AVG:
			switch(iFn)
			{
				case ScECO_REAL_CH_SEL:		lScEco.mPr.chnnl = val;		break;
				case ScECO_AVG_THR_LIGHT:	MnMSR_Set_Threshold_Ch_Value(iCh,MnMS1_OPT_SINGLE_THR_LIGHT,val);	break;
				case ScECO_AVG_THR_HEAVY:	MnMSR_Set_Threshold_Ch_Value(iCh,MnMS1_OPT_SINGLE_THR_HEAVY,val);	break;
				case ScECO_AVG_ASF_LIGHT:	MnMSR_CalSet_Ch_Value(iCh,MnMS1_OPT_SINGLE_ASF_LIGHT, val);			break;
				case ScECO_AVG_ASF_HEAVY:	MnMSR_CalSet_Ch_Value(iCh,MnMS1_OPT_SINGLE_ASF_HEAVY, val);			break;				
				default:
					break;
			}
			break;
		case ScECO_TYPE_SAVE_ECHO:
			switch(ScEC2_Getupdn_dig())
			{
				case ScECO_TIME_YY:
					lScEco.yy = val;
					break;
				case ScECO_TIME_MM:
					lScEco.mm =val;
					break;
				case ScECO_TIME_DD:
					lScEco.dd =val;
					break;
				case ScECO_TIME_HH:
					lScEco.hh =val;
					break;
				case ScECO_TIME_MN:
					lScEco.mn =val;
					break;
			}
			break;
		default:				
			break;
	}

}

void ScECH_PrSet_ECHO_SAVE_TIME(void)
{
	lScEco.yy = nnd_echo.echo_main[0];
	lScEco.mm = nnd_echo.echo_main[1];	
	lScEco.dd = nnd_echo.echo_main[2];	
	lScEco.hh = nnd_echo.echo_main[3];
	lScEco.mn = nnd_echo.echo_main[4];	
}


U32 unix_time_in_sec(U08 min, U08 hrs, U08 day, U08 mon,U08 year)
{
	struct tm t;
	t.tm_sec = 0;
	t.tm_min = min;
	t.tm_hour = hrs;
	t.tm_mday = day;
	t.tm_mon = mon -1;
	t.tm_year = year+100;
	t.tm_isdst = -1;

	return (U32)mktime(&t);



}

void ScECH_PrSet_Time(void)
{
	U16 cnt = NND_Meta_GetEchocnt(nnd_echo.ch)-1;
	U16 i;
	U32 min =  0xfffffff0;
	U16 val = 0;
	U32 sel_totaldays;
	U32 dump_totaldays;

	sel_totaldays = unix_time_in_sec(lScEco.mn,lScEco.hh,lScEco.dd,lScEco.mm,lScEco.yy);

	
	for(i = cnt; i>0; i--)
	{
		dump_totaldays =  unix_time_in_sec(NND_Meta_Getmain(nnd_echo.ch,i,4),NND_Meta_Getmain(nnd_echo.ch,i,3),NND_Meta_Getmain(nnd_echo.ch,i,2),
						NND_Meta_Getmain(nnd_echo.ch,i,1),NND_Meta_Getmain(nnd_echo.ch,i,0));

		if((abs(dump_totaldays-sel_totaldays)) <min)
		{
			min = abs(dump_totaldays-sel_totaldays);
			val = i;
		}
	}

	nnd_echo.num = val;



	NND_Echo_buffer_Read(nnd_echo.num);
	
}
//------------------------------------------------------------------------------------------------------------------------------
//  Global APIs - Access Local Variables - Basic
//------------------------------------------------------------------------------------------------------------------------------
// Get
U08 ScECH_GetLayer(void)			{	return lScEco.lyr;					}
U08 ScECH_GetType(void)				{	return lScEco.type;					}
U08 ScECH_GetSave(void)				{	return lScEco.save;					}
// Set
void ScECH_SetLayer(U08 lyr)		{	lScEco.lyr = lyr;					}
void ScECH_SetType(U08 sel)			{	lScEco.type = sel;					}
void ScECH_SetSave(U08 sel)			{	lScEco.save = sel;					}


//------------------------------------------------------------------------------------------------------------------------------
//  Global APIs - Basic Operation
//------------------------------------------------------------------------------------------------------------------------------
void ScECH_InitMain(void)
{
	lScEco.lyr = SCRN_L0_FUNC;
	ScEC0_InitFunc();
	ScEC1_InitList();
	ScEC2_InitValu();

}

void ScECH_ProcMain(void)
{
	switch(lScEco.lyr)
	{
		case SCRN_L0_FUNC:		ScEC0_ProcFunc();			break;
		case SCRN_L1_LIST:		ScEC1_ProcList();			break;
		case SCRN_L2_VALU:		ScEC2_ProcValu();			break;
	}
}


//------------------------------------------------------------------------------------------------------------------------------
//  EOF
//------------------------------------------------------------------------------------------------------------------------------

