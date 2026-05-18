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
// display
#include "disp_main.h"
#include "disp_color.h"
#include "disp_string.h"
#include "disp_figure.h"
#include "disp_titlebar.h"
#include "disp_button.h"
#include "disp_popup.h"
// menu
#include "menu_main.h"
#include "menu_test.h"
#include "menu_engineer.h"
#include "menu_lyr2_item.h"
#include "menu_lyr3_value.h"
#include "menu_lyr4_addition.h"
// disp - menu
#include "dp_menu_main.h"
// self
#include "dp_menu_test.h"


//------------------------------------------------------------------------------------------------------------------------------
//  Global variables
//------------------------------------------------------------------------------------------------------------------------------


//------------------------------------------------------------------------------------------------------------------------------
//  Local variables
//------------------------------------------------------------------------------------------------------------------------------
DpTST_LS lDpTst;


//------------------------------------------------------------------------------------------------------------------------------
//  Global APIs - Access Local Variables
//------------------------------------------------------------------------------------------------------------------------------
// Get (Set)
I08* DpTST_GetAddrItm(U08 idx)		{	return lDpTst.sItm[idx];			}
I08* DpTST_GetAddrVal(U08 idx)		{	return lDpTst.sVal[idx];			}


//------------------------------------------------------------------------------------------------------------------------------
//  Global APIs - Additional
//------------------------------------------------------------------------------------------------------------------------------
void DpTST_AddUpdat(void)
{

}

void DpTST_AddIntro(void)
{
	//DpTST_PopIntro();
}


//------------------------------------------------------------------------------------------------------------------------------
//  Global APIs - Pop-Up
//------------------------------------------------------------------------------------------------------------------------------
#if 1
void DpTST_PopUpdat(void)
{
	U16 x0 = DpPOP_CT1_X0;
	U16 y0 = DpPOP_CT1_Y0;
	U16 y1 = DpPOP_CT2_Y0;
	U32 cBg = _cPOP_BG_WND;
	I08 pSt0[16] = {0, };
	I08 pSt1[16] = {0, };

	U08 iIt = MnLY2_GetIdxItem();
	S32 val = MnLY3_GetValue();
	S32 dig = MnLY3_GetUpdnDig();
	S32 updn_mod = MnLY3_GetUpdnMod();
	S32 max  = MnLY3_GetMax();
	U32 slope;

	DpFIG_DrwRect(x0, y0, 200, 30, cBg, DpFIG_FILL);
	DpFIG_DrwRect(x0, y1, 200, 30, cBg, DpFIG_FILL);
	



	switch(iIt)
	{
		case MnTST_OPT_HW_RX_AMP:
			switch(val)
			{
				case MnTST_HW_RX_AMP_47:		_SPRINTF(pSt0, "x47");		break;
				case MnTST_HW_RX_AMP_100:		_SPRINTF(pSt0, "x100");		break;
				default:
					break;
			}			
			break;
		case MnTST_OPT_CH1_SMOOTH_NO:		
		case MnTST_OPT_CH2_SMOOTH_NO:			_SPRINTF(pSt0, "%d", val);									break;
		case MnTST_OPT_CH1_SMOOTH_RANGE:	
		case MnTST_OPT_CH2_SMOOTH_RANGE:		_SPRINTF(pSt0, "%d", val);									break;
		case MnTST_OPT_CH2_THR_RANGE:
		case MnTST_OPT_CH1_THR_RANGE:			_SPRINTF(pSt0, "%01d.%02d", (U16)(val/100), (U16)(val%100));	break;
		case MnTST_OPT_CH1_THR_MIN:				
		case MnTST_OPT_CH2_THR_MIN:				_SPRINTF(pSt0, "%01d.%01d", val/10,val%10);					break;
			case MnTST_OPT_CH1_PULSE_NO:			
			case MnTST_OPT_CH2_PULSE_NO:			_SPRINTF(pSt0, "%d", val);	break;
			case MnTST_OPT_CH1_ECHO_AMP_B:			
			case MnTST_OPT_CH2_ECHO_AMP_B:			_SPRINTF(pSt0, "%04d", val);	break;
				break;
			case MnTST_OPT_CH1_AUTO_CAL:
			case MnTST_OPT_CH2_AUTO_CAL:			_SPRINTF(pSt0, "%02d.%02d", (U16)(val/100), (U16)(val%100));	break;
			case MnTST_OPT_FILTER_RANGE:			_SPRINTF(pSt0, "%02d.%02d", (U16)(val/100), (U16)(val%100));	break;
			case MnTST_OPT_FILTER_TIME:				_SPRINTF(pSt0, "%04d", val);									break;
			case MnTST_OPT_SENSOR_DISTANCE:			_SPRINTF(pSt0, "%02d.%02d", (U16)(val/100), (U16)(val%100));	break;
		case MnTST_OPT_DEMO_MODE:				_SPRINTF(pSt0, "%d", val);										break;
		case MnTST_OPT_DEBUG_MODE:
			DpPOP_UdtValue(0,   _F_F, gDpStr.sOnF[0]);
			DpPOP_UdtValue(1,   _F_F, gDpStr.sOnF[1]);
			DpPOP_UdtValue(val, _F_T, gDpStr.sOnF[val]);
			return;
		case MnTST_OPT_AUTO_RANGE:			_SPRINTF(pSt0, "%02d.%02d", (U16)(val/100), (U16)(val%100));	break;
		case MnTST_OPT_AUTO_THR_MIN:		_SPRINTF(pSt0, "%03d %c", val,37);					break;
		case MnTST_OPT_AUTO_THR_MAX:		_SPRINTF(pSt0, "%03d %c", val,37);					break;
		case MnTST_OPT_AUTO_VOLT_DIFF:
		case MnTST_OPT_AUTO_VOLT_MIN:
		case MnTST_OPT_AUTO_VOLT_PEAK:		_SPRINTF(pSt0, "%01d.%02d", (U16)(val/100), (U16)(val%100));	break;
		case MnTST_OPT_AUTO_LEVEL_RANGE:	_SPRINTF(pSt0, "%01d.%02d", (U16)(val/100), (U16)(val%100));	break;
		default:
			break;
	}

	switch(iIt)
	{
		case MnTST_OPT_AUTO_VOLT_DIFF:
			slope = (val*100)/(MnTST_PrGet_Value(MnTST_OPT_AUTO_RANGE)*2);
			
			DpFIG_DrwRect(DpPOP_EXP_X0, DpPOP_EXP_Y0, 300, 30, cBg, DpFIG_FILL);
			_SPRINTF(pSt1, "Slope:");	
			DpSTR_GuiLeft(DpPOP_EXP_X0, DpPOP_EXP_Y0, _cPOP_ST_EXP, _cPOP_BG_WND, _fE17HsB, pSt1);	
			if(slope>=100)
				_SPRINTF(pSt1, "%d.%02d",slope/100,slope%100);	
			else 
				_SPRINTF(pSt1, "%d.%02d",slope/100,slope%100);	
			DpSTR_GuiLeft(DpPOP_EXP_X0+100, DpPOP_EXP_Y0, _cPOP_ST_EXP, _cPOP_BG_WND, _fE17HsB, pSt1);				
			break;			

		case MnTST_OPT_AUTO_RANGE:
			slope = (MnTST_PrGet_Value(MnTST_OPT_AUTO_VOLT_DIFF)*100)/(val*2);
			
			DpFIG_DrwRect(DpPOP_EXP_X0, DpPOP_EXP_Y0, 300, 30, cBg, DpFIG_FILL);
			_SPRINTF(pSt1, "Slope:");	
			DpSTR_GuiLeft(DpPOP_EXP_X0, DpPOP_EXP_Y0, _cPOP_ST_EXP, _cPOP_BG_WND, _fE17HsB, pSt1);	
			if(slope>=100)
				_SPRINTF(pSt1, "%d.%02d",slope/100,slope%100);		
			else 
				_SPRINTF(pSt1, "%d.%02d",slope/100,slope%100);	

			DpSTR_GuiLeft(DpPOP_EXP_X0+100, DpPOP_EXP_Y0, _cPOP_ST_EXP, _cPOP_BG_WND, _fE17HsB, pSt1);				
			break;				
		default:
			break;
	}
	
	


	switch(updn_mod)
	{
		case MENU_V3_UPDN_IDLE:			DpSTR_GuiLeft(x0, y0, _cPOP_ST_VAL_SEL, cBg, _fE22HsB, pSt0);	break;
		case MENU_V3_UPDN_DIG:
		case MENU_V3_UPDN_DIG_VALUE:	DpSTR_GuiLeftCursor(x0, y0, _cPOP_ST_VAL_SEL, _cMNU_SEL_SCT, cBg, (U08)log10(max)-dig, _fE22HsB, pSt0);	break;
		default:	break;
	}

	if(MnLY3_GetUpdnMod() != MENU_V3_UPDN_DIG_VALUE)
		return;
	DpFIG_DrwRect(x0, y1, 500, 30, cBg, DpFIG_FILL);	

	switch(dig)
	{
		case 4:	
			_SPRINTF(pSt0, "%d", (val/10000)%10);	DpSTR_GuiLeft(x0, y1, _cMNU_SEL_SCT, cBg, _fE22HsB, pSt0);
			break;
		case 3:
			_SPRINTF(pSt0, "%d", (val/1000)%10); DpSTR_GuiLeft(x0, y1, _cMNU_SEL_SCT, cBg, _fE22HsB, pSt0);
			break;
		case 2:
			_SPRINTF(pSt0, "%d", (val/100)%10); DpSTR_GuiLeft(x0, y1, _cMNU_SEL_SCT, cBg, _fE22HsB, pSt0);
			break;
		case 1:
			_SPRINTF(pSt0, "%d", (val/10)%10); DpSTR_GuiLeft(x0, y1, _cMNU_SEL_SCT, cBg, _fE22HsB, pSt0);
			break;
		case 0:
			_SPRINTF(pSt0, "%d", val%10); DpSTR_GuiLeft(x0, y1, _cMNU_SEL_SCT, cBg, _fE22HsB, pSt0);
			break;
	}





#if 0
	switch(MnLY2_GetIdxItem())
	{
		case MnEGN_I00_THRESHOLD_MIN:
		case MnEGN_I01_SMOOTH_NUM:
		case MnEGN_I02_SMOOTH_RANGE:	_SPRINTF(pSt0, "%d", val);		break;
		case MnEGN_I03_fDP_DBG:			
			DpPOP_UdtValue(0,   _F_F, gDpStr.sOnF[0]);
			DpPOP_UdtValue(1,   _F_F, gDpStr.sOnF[1]);
			DpPOP_UdtValue(val, _F_T, gDpStr.sOnF[val]);
			return;
		case MnEGN_I04_fDP_ECHO:
			DpPOP_UdtValue(0,   _F_F, gDpStr.sOnF[0]);
			DpPOP_UdtValue(1,   _F_F, gDpStr.sOnF[1]);
			DpPOP_UdtValue(val, _F_T, gDpStr.sOnF[val]);
			return;
		case MnEGN_I05_RX_BPF:			_SPRINTF(pSt0, "%d", val);		break;
		default:
			DpPOP_UdtValue(val, _F_F, _sNG);
			return;
	}
#endif
}

void DpTST_PopIntro(void)
{
	U08 fDp = _F_T;
	U08 iIt = MnLY2_GetIdxItem();
	S32 val = MnTST_PrGet_Value(iIt);
	S32 min = -1, max = -1;


	_SPRINTF(lDpTst.pStr,lDpTst.sItm[iIt]);



	DpPOP_UdtTitle(lDpTst.pStr);

	// Old
	fDp = _F_T;

	switch(iIt)
	{
		case MnTST_OPT_HW_RX_AMP:
			switch(val)
			{
				case MnTST_HW_RX_AMP_47:		_SPRINTF(lDpTst.pStr, "x47");		break;
				case MnTST_HW_RX_AMP_100:		_SPRINTF(lDpTst.pStr, "x100");		break;
				default:
					break;
			}			
			break;
		case MnTST_OPT_CH1_SMOOTH_NO:		
		case MnTST_OPT_CH2_SMOOTH_NO:			_SPRINTF(lDpTst.pStr, "%d", val);									break;
		case MnTST_OPT_CH1_SMOOTH_RANGE:	
		case MnTST_OPT_CH2_SMOOTH_RANGE:		_SPRINTF(lDpTst.pStr, "%d", val);									break;
		case MnTST_OPT_CH2_THR_RANGE:
		case MnTST_OPT_CH1_THR_RANGE:			_SPRINTF(lDpTst.pStr, "%1d.%02d", (U16)(val/100), (U16)(val%100));	break;
		case MnTST_OPT_CH1_THR_MIN:				
		case MnTST_OPT_CH2_THR_MIN:				_SPRINTF(lDpTst.pStr, "%1d.%01d", val/10,val%10);					break;
			case MnTST_OPT_CH1_PULSE_NO:			
			case MnTST_OPT_CH2_PULSE_NO:			_SPRINTF(lDpTst.pStr, "%d", val);	break;
			case MnTST_OPT_CH1_ECHO_AMP_B:			
			case MnTST_OPT_CH2_ECHO_AMP_B:			_SPRINTF(lDpTst.pStr, "%d", val);	break;
			case MnTST_OPT_CH1_AUTO_CAL:
			case MnTST_OPT_CH2_AUTO_CAL:			_SPRINTF(lDpTst.pStr, "%2d.%02d", (U16)(val/100), (U16)(val%100));		break;
			case MnTST_OPT_FILTER_RANGE:			_SPRINTF(lDpTst.pStr, "%2d.%02d", (U16)(val/100), (U16)(val%100));	break;
			case MnTST_OPT_FILTER_TIME:				_SPRINTF(lDpTst.pStr, "%d", val);									break;
			case MnTST_OPT_SENSOR_DISTANCE:			_SPRINTF(lDpTst.pStr, "%2d.%02d", (U16)(val/100), (U16)(val%100));	break;
		case MnTST_OPT_DEMO_MODE:				_SPRINTF(lDpTst.pStr, "%d", val);	break;
		case MnTST_OPT_DEBUG_MODE:
			switch(val)
			{
				case MnTST_DEBUG_MODE_OFF:		_SPRINTF(lDpTst.pStr,_sOFF); break;
				case MnTST_DEBUG_MODE_ON:		_SPRINTF(lDpTst.pStr,_sON);  break;
			}
			break;
		case MnTST_OPT_AUTO_RANGE:			_SPRINTF(lDpTst.pStr, "%2d.%02d", (U16)(val/100), (U16)(val%100));		break;
		case MnTST_OPT_AUTO_THR_MIN:		_SPRINTF(lDpTst.pStr, "%d", val);										break;
		case MnTST_OPT_AUTO_THR_MAX:		_SPRINTF(lDpTst.pStr, "%d", val);										break;
		case MnTST_OPT_AUTO_VOLT_DIFF:
		case MnTST_OPT_AUTO_VOLT_MIN:
		case MnTST_OPT_AUTO_VOLT_PEAK:		_SPRINTF(lDpTst.pStr, "%1d.%02d", (U16)(val/100), (U16)(val%100));		break;
		case MnTST_OPT_AUTO_LEVEL_RANGE:	_SPRINTF(lDpTst.pStr, "%1d.%02d", (U16)(val/100), (U16)(val%100));		break;
		default:
			break;
	}


	if(fDp)
		DpPOP_UdtOldVl(lDpTst.pStr);


	fDp = _F_T;

	// Range (Min. / Max.)

	switch(iIt)
	{
		case MnTST_OPT_HW_RX_AMP:			min = MnTST_HW_RX_AMP_MIN;	max = MnTST_HW_RX_AMP_MAX;	break;
		case MnTST_OPT_CH1_SMOOTH_NO:		
		case MnTST_OPT_CH2_SMOOTH_NO:		min = MnTST_SMOOTH_NO_MIN;	max = MnTST_SMOOTH_NO_MAX;	break;
		case MnTST_OPT_CH1_SMOOTH_RANGE:	
		case MnTST_OPT_CH2_SMOOTH_RANGE:	min = MnTST_SMOOTH_RANGE_MIN;	max = MnTST_SMOOTH_RANGE_MAX;	break;
		case MnTST_OPT_CH1_THR_RANGE:		
		case MnTST_OPT_CH2_THR_RANGE:		min = MnTST_THR_RANGE_MIN;	max = MnTST_THR_RANGE_MAX;break;
		case MnTST_OPT_CH1_THR_MIN:			
		case MnTST_OPT_CH2_THR_MIN:			min = MnTST_THR_MIN_MIN;	max = MnTST_THR_MIN_MAX;	break;
			case MnTST_OPT_CH1_PULSE_NO:		
			case MnTST_OPT_CH2_PULSE_NO:		min = MnTST_PULSE_NO_MIN;	max = MnTST_PULSE_NO_MAX;	break;
			case MnTST_OPT_CH1_ECHO_AMP_B:		
			case MnTST_OPT_CH2_ECHO_AMP_B:		min = MnTST_ECHO_AMP_B_MIN;	max = MnTST_ECHO_AMP_B_MAX;	 break;
			case MnTST_OPT_CH1_AUTO_CAL:
			case MnTST_OPT_CH2_AUTO_CAL:		min = MnTST_AUTO_CAL_MIN;	max = MnTST_AUTO_CAL_MAX;	break;
			case MnTST_OPT_FILTER_RANGE:		min = MnTST_FILTER_RANGE_MIN;	max = MnTST_FILTER_RANGE_MAX;  break;
			case MnTST_OPT_FILTER_TIME:			min = MnTST_FILTER_TIME_MIN;	max = MnTST_FILTER_TIME_MAX;	 break;
			case MnTST_OPT_SENSOR_DISTANCE:		min = MnTST_SENSOR_DISTANCE_MIN;	max = MnTST_SENSOR_DISTANCE_MAX;	 break;
		case MnTST_OPT_DEMO_MODE:			min = MnTST_DEMO_MODE_MIN;	max = MnTST_DEMO_MODE_MAX;	break;
		case MnTST_OPT_DEBUG_MODE:			min = MnTST_DEBUG_MODE_MIN;	max = MnTST_DEBUG_MODE_MAX;	break;
		case MnTST_OPT_AUTO_RANGE:			min = MnTST_AUTO_RANGE_MIN;	max = MnTST_AUTO_RANGE_MAX;	break;
		case MnTST_OPT_AUTO_THR_MIN:		min = MnTST_AUTO_THR_MIN;	max = MnTST_AUTO_THR_MAX;	break;
		case MnTST_OPT_AUTO_THR_MAX:		min = MnTST_AUTO_THR_MIN;	max = MnTST_AUTO_THR_MAX;	break;
		case MnTST_OPT_AUTO_VOLT_DIFF:
		case MnTST_OPT_AUTO_VOLT_MIN:
		case MnTST_OPT_AUTO_VOLT_PEAK:		min = MnTST_AUTO_VOLT_PEAK_MIN;	max = MnTST_AUTO_VOLT_PEAK_MAX;	break;
		case MnTST_OPT_AUTO_LEVEL_RANGE:	min = MnTST_AUTO_LEVEL_RANGE_MIN; max = MnTST_AUTO_LEVEL_RANGE_MAX;	break;
		default:
			fDp = _F_F;	
			break;
	}



	switch(iIt)
	{
		case MnTST_OPT_HW_RX_AMP:				fDp = _F_F;	break;
		case MnTST_OPT_CH1_SMOOTH_NO:		
		case MnTST_OPT_CH2_SMOOTH_NO:			_SPRINTF(lDpTst.pStr, "%d", min);									break;
		case MnTST_OPT_CH1_SMOOTH_RANGE:	
		case MnTST_OPT_CH2_SMOOTH_RANGE:		_SPRINTF(lDpTst.pStr, "%d", min);									break;
		case MnTST_OPT_CH2_THR_RANGE:
		case MnTST_OPT_CH1_THR_RANGE:			_SPRINTF(lDpTst.pStr, "%1d.%02d", (U16)(min/100), (U16)(min%100));	break;
		case MnTST_OPT_CH1_THR_MIN:				
		case MnTST_OPT_CH2_THR_MIN:				_SPRINTF(lDpTst.pStr, "%1d.%01d", min/10,min%10);					break;
			case MnTST_OPT_CH1_PULSE_NO:			
			case MnTST_OPT_CH2_PULSE_NO:			_SPRINTF(lDpTst.pStr, "%d", min);	break;
			case MnTST_OPT_CH1_ECHO_AMP_B:			
			case MnTST_OPT_CH2_ECHO_AMP_B:			_SPRINTF(lDpTst.pStr, "%d", min);	break;
			case MnTST_OPT_CH1_AUTO_CAL:
			case MnTST_OPT_CH2_AUTO_CAL:			_SPRINTF(lDpTst.pStr, "%2d.%02d", (U16)(min/100), (U16)(min%100));	break;
			case MnTST_OPT_FILTER_RANGE:			_SPRINTF(lDpTst.pStr, "%2d.%02d", (U16)(min/100), (U16)(min%100));	break;
			case MnTST_OPT_FILTER_TIME:				_SPRINTF(lDpTst.pStr, "%d", min);									break;
			case MnTST_OPT_SENSOR_DISTANCE:			_SPRINTF(lDpTst.pStr, "%2d.%02d", (U16)(min/100), (U16)(min%100));	break;
		case MnTST_OPT_DEMO_MODE:				fDp = _F_F;	break;
		case MnTST_OPT_DEBUG_MODE:				fDp = _F_F;	break;
		case MnTST_OPT_AUTO_RANGE:				_SPRINTF(lDpTst.pStr, "%2d.%02d", (U16)(min/100), (U16)(min%100));	break;
		case MnTST_OPT_AUTO_THR_MIN:			_SPRINTF(lDpTst.pStr, "%3d", min);									break;
		case MnTST_OPT_AUTO_THR_MAX:			_SPRINTF(lDpTst.pStr, "%3d", min);										break;
		case MnTST_OPT_AUTO_VOLT_DIFF:
		case MnTST_OPT_AUTO_VOLT_MIN:
		case MnTST_OPT_AUTO_VOLT_PEAK:			_SPRINTF(lDpTst.pStr, "%1d.%02d", (U16)(min/100), (U16)(min%100));	break;
		case MnTST_OPT_AUTO_LEVEL_RANGE:		_SPRINTF(lDpTst.pStr, "%1d.%02d", (U16)(min/100), (U16)(min%100));	break;
		default:
			break;
	}



	if(fDp)	DpPOP_UdtRangeMin(lDpTst.pStr);

	fDp = _F_T;

	switch(iIt)
	{
		case MnTST_OPT_HW_RX_AMP:				fDp = _F_F;	break;
		case MnTST_OPT_CH1_SMOOTH_NO:		
		case MnTST_OPT_CH2_SMOOTH_NO:			_SPRINTF(lDpTst.pStr, "%d", max);									break;
		case MnTST_OPT_CH1_SMOOTH_RANGE:	
		case MnTST_OPT_CH2_SMOOTH_RANGE:		_SPRINTF(lDpTst.pStr, "%d", max);									break;
		case MnTST_OPT_CH2_THR_RANGE:
		case MnTST_OPT_CH1_THR_RANGE:			_SPRINTF(lDpTst.pStr, "%1d.%02d", (U16)(max/100), (U16)(max%100));	break;
		case MnTST_OPT_CH1_THR_MIN:				
		case MnTST_OPT_CH2_THR_MIN:				_SPRINTF(lDpTst.pStr, "%1d.%01d", max/10,max%10);					break;
			case MnTST_OPT_CH1_PULSE_NO:			
			case MnTST_OPT_CH2_PULSE_NO:			_SPRINTF(lDpTst.pStr, "%d", max);	break;
			case MnTST_OPT_CH1_ECHO_AMP_B:			
			case MnTST_OPT_CH2_ECHO_AMP_B:			_SPRINTF(lDpTst.pStr, "%d", max);	break;
			case MnTST_OPT_CH1_AUTO_CAL:
			case MnTST_OPT_CH2_AUTO_CAL:			_SPRINTF(lDpTst.pStr, "%2d.%02d", (U16)(max/100), (U16)(max%100));	break;
			case MnTST_OPT_FILTER_RANGE:			_SPRINTF(lDpTst.pStr, "%2d.%02d", (U16)(max/100), (U16)(max%100));	break;
			case MnTST_OPT_FILTER_TIME:				_SPRINTF(lDpTst.pStr, "%d", max);									break;
			case MnTST_OPT_SENSOR_DISTANCE:			_SPRINTF(lDpTst.pStr, "%2d.%02d", (U16)(max/100), (U16)(max%100));	break;
		case MnTST_OPT_DEMO_MODE:				fDp = _F_F;	break;
		case MnTST_OPT_DEBUG_MODE:				fDp = _F_F;	break;
		case MnTST_OPT_AUTO_RANGE:				_SPRINTF(lDpTst.pStr, "%2d.%02d", (U16)(max/100), (U16)(max%100));	break;
		case MnTST_OPT_AUTO_THR_MAX:			_SPRINTF(lDpTst.pStr, "%3d", max);										break;
		case MnTST_OPT_AUTO_THR_MIN:			_SPRINTF(lDpTst.pStr, "%3d", min);										break;		
		case MnTST_OPT_AUTO_VOLT_DIFF:
		case MnTST_OPT_AUTO_VOLT_MIN:
		case MnTST_OPT_AUTO_VOLT_PEAK:			_SPRINTF(lDpTst.pStr, "%1d.%02d", (U16)(max/100), (U16)(max%100));	break;
		case MnTST_OPT_AUTO_LEVEL_RANGE:		_SPRINTF(lDpTst.pStr, "%1d.%02d", (U16)(max/100), (U16)(max%100));	break;

		default:
			break;
	}




	if(fDp)	DpPOP_UdtRangeMax(lDpTst.pStr);
	fDp = _F_T;

#if 0 
	// Value
	switch(MnLY2_GetIdxItem())
	{
		case MnEGN_I00_THRESHOLD_MIN:
		case MnEGN_I01_SMOOTH_NUM:
		case MnEGN_I02_SMOOTH_RANGE:	break;
		case MnEGN_I03_fDP_DBG:			DpPOP_UdtValue(0, _F_F, gDpStr.sOnF[0]);
										DpPOP_UdtValue(1, _F_F, gDpStr.sOnF[1]);		break;
		case MnEGN_I04_fDP_ECHO:		DpPOP_UdtValue(0, _F_F, gDpStr.sOnF[0]);
										DpPOP_UdtValue(1, _F_F, gDpStr.sOnF[1]);		break;
		case MnEGN_I05_RX_BPF:			break;
		default:						DpPOP_UdtValue(0, _F_F, _sNG);					return;
	}
#endif
}

#endif
//------------------------------------------------------------------------------------------------------------------------------
//  Global APIs - Basic
//------------------------------------------------------------------------------------------------------------------------------
void DpTST_InitVari(void)
{
	S32 val = 0;

	_SPRINTF(lDpTst.sItm[MnTST_OPT_HW_RX_AMP], _sHW_RX_AMP);
	_SPRINTF(lDpTst.sItm[MnTST_OPT_CH1_SMOOTH_NO], _sCH1_SMOOTH_NUM);
	_SPRINTF(lDpTst.sItm[MnTST_OPT_CH1_SMOOTH_RANGE], _sCH1_SMOOTH_RANGE);
	_SPRINTF(lDpTst.sItm[MnTST_OPT_CH1_THR_RANGE], _sCH1_THRESHOLD_RANGE);
	_SPRINTF(lDpTst.sItm[MnTST_OPT_CH1_THR_MIN],_sCH1_THRESHOLD_MIN);
	_SPRINTF(lDpTst.sItm[MnTST_OPT_CH1_PULSE_NO], _sCH1_PULSE_NUM);
	_SPRINTF(lDpTst.sItm[MnTST_OPT_CH1_ECHO_AMP_B], _sCH1_ECHO_AMP_B);
	_SPRINTF(lDpTst.sItm[MnTST_OPT_CH1_AUTO_CAL], _sCH1_AUTO_CAL);
	_SPRINTF(lDpTst.sItm[MnTST_OPT_CH2_SMOOTH_NO], _sCH2_SMOOTH_NUM);
	_SPRINTF(lDpTst.sItm[MnTST_OPT_CH2_SMOOTH_RANGE],_sCH2_SMOOTH_RANGE);	
	_SPRINTF(lDpTst.sItm[MnTST_OPT_CH2_THR_RANGE], _sCH2_THRESHOLD_RANGE);
	_SPRINTF(lDpTst.sItm[MnTST_OPT_CH2_THR_MIN],_sCH2_THRESHOLD_MIN);
	_SPRINTF(lDpTst.sItm[MnTST_OPT_CH2_PULSE_NO], _sCH2_PULSE_NUM);
	_SPRINTF(lDpTst.sItm[MnTST_OPT_CH2_ECHO_AMP_B], _sCH2_ECHO_AMP_B);
	_SPRINTF(lDpTst.sItm[MnTST_OPT_CH2_AUTO_CAL], _sCH2_AUTO_CAL);
	_SPRINTF(lDpTst.sItm[MnTST_OPT_FILTER_RANGE],_filter_range);	
	_SPRINTF(lDpTst.sItm[MnTST_OPT_FILTER_TIME], _filter_time);
	_SPRINTF(lDpTst.sItm[MnTST_OPT_SENSOR_DISTANCE], _sSensor_Distance);
	_SPRINTF(lDpTst.sItm[MnTST_OPT_DEMO_MODE],_sEcho_Download);	
	_SPRINTF(lDpTst.sItm[MnTST_OPT_DEBUG_MODE], _sDebug_Mode);
	_SPRINTF(lDpTst.sItm[MnTST_OPT_AUTO_RANGE], "Auto Cal Window Range");
	_SPRINTF(lDpTst.sItm[MnTST_OPT_AUTO_THR_MIN], "Auto Cal Thr Min.");
	_SPRINTF(lDpTst.sItm[MnTST_OPT_AUTO_THR_MAX], "Auto Cal Thr Max.");
	_SPRINTF(lDpTst.sItm[MnTST_OPT_AUTO_VOLT_DIFF],"Auto Cal Volt Diff");	
	_SPRINTF(lDpTst.sItm[MnTST_OPT_AUTO_VOLT_PEAK], "Auto Cal Volt Peak");
	_SPRINTF(lDpTst.sItm[MnTST_OPT_AUTO_LEVEL_RANGE], "Auto Cal Level Range");
	_SPRINTF(lDpTst.sItm[MnTST_OPT_AUTO_VOLT_MIN], "Auto Cal Volt Min");
	_SPRINTF(lDpTst.sItm[MnTST_OPT_MARQUEE], "Marquee");

	
	val = MnTST_PrGet_Value(MnTST_OPT_HW_RX_AMP);
	switch(val)
	{
		case MnTST_HW_RX_AMP_47:		_SPRINTF(lDpTst.sVal[MnTST_OPT_HW_RX_AMP], "x47");		break;
		case MnTST_HW_RX_AMP_100:		_SPRINTF(lDpTst.sVal[MnTST_OPT_HW_RX_AMP], "x100");		break;
		default:
			break;
	}

	val = MnTST_PrGet_Value(MnTST_OPT_CH1_SMOOTH_NO);
	_SPRINTF(lDpTst.sVal[MnTST_OPT_CH1_SMOOTH_NO], " %d", val);

	val = MnTST_PrGet_Value(MnTST_OPT_CH1_SMOOTH_RANGE);
	_SPRINTF(lDpTst.sVal[MnTST_OPT_CH1_SMOOTH_RANGE], " %d", val);	

	val = MnTST_PrGet_Value(MnTST_OPT_CH1_THR_RANGE);
	_SPRINTF(lDpTst.sVal[MnTST_OPT_CH1_THR_RANGE], "%1d.%02d", (U16)(val/100), (U16)(val%100));

	val = MnTST_PrGet_Value(MnTST_OPT_CH1_THR_MIN);	
	_SPRINTF(lDpTst.sVal[MnTST_OPT_CH1_THR_MIN], "%1d.%01d", val/10,val%10);	

	val = MnTST_PrGet_Value(MnTST_OPT_CH1_PULSE_NO);
	_SPRINTF(lDpTst.sVal[MnTST_OPT_CH1_PULSE_NO], " %d", val);

	val = MnTST_PrGet_Value(MnTST_OPT_CH1_ECHO_AMP_B);
	_SPRINTF(lDpTst.sVal[MnTST_OPT_CH1_ECHO_AMP_B], " %d", val);	

	val = MnTST_PrGet_Value(MnTST_OPT_CH1_AUTO_CAL);
	_SPRINTF(lDpTst.sVal[MnTST_OPT_CH1_AUTO_CAL], "%2d.%02d", (U16)(val/100), (U16)(val%100));

	val = MnTST_PrGet_Value(MnTST_OPT_CH2_SMOOTH_NO);
	_SPRINTF(lDpTst.sVal[MnTST_OPT_CH2_SMOOTH_NO], " %d", val);

	val = MnTST_PrGet_Value(MnTST_OPT_CH2_SMOOTH_RANGE);
	_SPRINTF(lDpTst.sVal[MnTST_OPT_CH2_SMOOTH_RANGE], " %d", val);	

	val = MnTST_PrGet_Value(MnTST_OPT_CH2_THR_RANGE);
	_SPRINTF(lDpTst.sVal[MnTST_OPT_CH2_THR_RANGE], "%1d.%02d", (U16)(val/100), (U16)(val%100));

	val = MnTST_PrGet_Value(MnTST_OPT_CH2_THR_MIN);	
	_SPRINTF(lDpTst.sVal[MnTST_OPT_CH2_THR_MIN], "%1d.%01d", val/10,val%10);	

	val = MnTST_PrGet_Value(MnTST_OPT_CH2_PULSE_NO);
	_SPRINTF(lDpTst.sVal[MnTST_OPT_CH2_PULSE_NO], " %d", val);

	val = MnTST_PrGet_Value(MnTST_OPT_CH2_ECHO_AMP_B);
	_SPRINTF(lDpTst.sVal[MnTST_OPT_CH2_ECHO_AMP_B], " %d", val);	

	val = MnTST_PrGet_Value(MnTST_OPT_CH2_AUTO_CAL);
	_SPRINTF(lDpTst.sVal[MnTST_OPT_CH2_AUTO_CAL], "%2d.%02d", (U16)(val/100), (U16)(val%100));

	val = MnTST_PrGet_Value(MnTST_OPT_FILTER_RANGE);
	_SPRINTF(lDpTst.sVal[MnTST_OPT_FILTER_RANGE], "%2d.%02d", (U16)(val/100), (U16)(val%100));


	val = MnTST_PrGet_Value(MnTST_OPT_FILTER_TIME);
	_SPRINTF(lDpTst.sVal[MnTST_OPT_FILTER_TIME], " %d", val);	

	val = MnTST_PrGet_Value(MnTST_OPT_SENSOR_DISTANCE);
	_SPRINTF(lDpTst.sVal[MnTST_OPT_SENSOR_DISTANCE], "%2d.%02d", (U16)(val/100), (U16)(val%100));


	val = MnTST_PrGet_Value(MnTST_OPT_DEMO_MODE);

	_SPRINTF(lDpTst.sVal[MnTST_OPT_DEMO_MODE], " %d", val);	

	
	val = MnTST_PrGet_Value(MnTST_OPT_AUTO_RANGE);
	_SPRINTF(lDpTst.sVal[MnTST_OPT_AUTO_RANGE], "%2d.%02d", (U16)(val/100), (U16)(val%100));

	val = MnTST_PrGet_Value(MnTST_OPT_AUTO_THR_MAX);
	_SPRINTF(lDpTst.sVal[MnTST_OPT_AUTO_THR_MAX], "%d", val);

	val = MnTST_PrGet_Value(MnTST_OPT_AUTO_THR_MIN);
	_SPRINTF(lDpTst.sVal[MnTST_OPT_AUTO_THR_MIN], "%d", val);

	val = MnTST_PrGet_Value(MnTST_OPT_AUTO_VOLT_DIFF);
	_SPRINTF(lDpTst.sVal[MnTST_OPT_AUTO_VOLT_DIFF], "%1d.%02d", (U16)(val/100), (U16)(val%100));

	val = MnTST_PrGet_Value(MnTST_OPT_AUTO_VOLT_PEAK);
	_SPRINTF(lDpTst.sVal[MnTST_OPT_AUTO_VOLT_PEAK], "%1d.%02d", (U16)(val/100), (U16)(val%100));

	val = MnTST_PrGet_Value(MnTST_OPT_AUTO_LEVEL_RANGE);
	_SPRINTF(lDpTst.sVal[MnTST_OPT_AUTO_LEVEL_RANGE], "%1d.%02d", (U16)(val/100), (U16)(val%100));

	val = MnTST_PrGet_Value(MnTST_OPT_DEBUG_MODE);

	switch(val)
	{
		case MnTST_DEBUG_MODE_OFF:		_SPRINTF(lDpTst.sVal[MnTST_OPT_DEBUG_MODE],_sOFF); break;
		case MnTST_DEBUG_MODE_ON:		_SPRINTF(lDpTst.sVal[MnTST_OPT_DEBUG_MODE],_sON);  break;
	}

	val = MnTST_PrGet_Value(MnTST_OPT_AUTO_VOLT_MIN);
	_SPRINTF(lDpTst.sVal[MnTST_OPT_AUTO_VOLT_MIN], "%1d.%02d", (U16)(val/100), (U16)(val%100));

	_SPRINTF(lDpTst.sVal[MnTST_OPT_MARQUEE], "Enter");

	_MEMSET(lDpTst.pStr, 0, sizeof(lDpTst.pStr));
}

void DpTS2_StrCntts(void)
{
	U08 i,n,page,idx,iN,realIdx;
	U32 cStr =_cMNU_STR_IDLE;
	U08 iIt = MnLY2_GetIdxItem();
	U08 displayIt;
	U16 box_itm_x0 = DpMNU_STR_X0_ITM;
	U16 box_val_x0 = DpMNU_STR_X0_VAL;
	U16 str_y0 = DpMNU_STR_Y0;
	U16 str_yg = DpMNU_STR_YG;
	U08 sel;
	U32 back_color = _cMNU_BOX_BG;
	U32 text_color = _cMNU_STR_IDLE;

	// Hide items 20~28: remap MARQUEE(29) to display position 20
	n = MnTST_OPT_SENSOR_DISTANCE + 2;	// 21 visible items (0~19 + MARQUEE)
	displayIt = iIt;
	if(iIt == MnTST_OPT_MARQUEE) displayIt = MnTST_OPT_SENSOR_DISTANCE + 1;

	if 	(displayIt >= (DpMNU_ITM_PAGE*5)) 	page = DpMNU_PG_5;
	else if	(displayIt >= (DpMNU_ITM_PAGE*4))	page = DpMNU_PG_4;
	else if (displayIt >= (DpMNU_ITM_PAGE*3))	page = DpMNU_PG_3;
	else if	(displayIt >= (DpMNU_ITM_PAGE*2))	page = DpMNU_PG_2;
	else if (displayIt >= (DpMNU_ITM_PAGE*1))	page = DpMNU_PG_1;
	else										page = DpMNU_PG_0;

	sel = (displayIt%DpMNU_ITM_PAGE);

	switch(page)
	{
		case DpMNU_PG_0:
			if(n>DpMNU_ITM_PAGE)		iN=DpMNU_ITM_PAGE;
			else						iN=n;
			break;
		case DpMNU_PG_1:
			if(n>(DpMNU_ITM_PAGE*2))	iN=DpMNU_ITM_PAGE;
			else						iN=n-DpMNU_ITM_PAGE;
			break;
		case DpMNU_PG_2:
			if(n>(DpMNU_ITM_PAGE*3))	iN=DpMNU_ITM_PAGE;
			else						iN=n-(DpMNU_ITM_PAGE*2);
			break;
		case DpMNU_PG_3:
			if(n>(DpMNU_ITM_PAGE*4))	iN=DpMNU_ITM_PAGE;
			else						iN=n-(DpMNU_ITM_PAGE*3);
			break;
		case DpMNU_PG_4:
			if(n>(DpMNU_ITM_PAGE*5))	iN=DpMNU_ITM_PAGE;
			else						iN=n-(DpMNU_ITM_PAGE*4);
			break;
		case DpMNU_PG_5:
			if(n>(DpMNU_ITM_PAGE*6))	iN=DpMNU_ITM_PAGE;
			else						iN=n-(DpMNU_ITM_PAGE*5);
			break;
		default:			return;
	}


	// String
	for(i=0; i<iN; i++)
	{
		if(MENU_GetLayer()>=MENU_L2_ITEM)
		{
			if(i==sel)
			{
				back_color = _cMNU_SEL_ITM;
				text_color = _cSD_DARKGRAY;
			}
			else
			{
				back_color = _cMNU_BOX_BG;
				text_color = _cMNU_STR_IDLE;
			}
		}

		DpFIG_DrwRect(DpMNU_BOX_X0_VAL+60, str_y0+(str_yg*i)+15, DpMNU_BOX_WID-30-60, 40, _cMNU_BOX_BG,	DpFIG_FILL);
		idx = i+(DpMNU_ITM_PAGE*page);

		// Remap display position 20 back to actual MARQUEE index
		realIdx = idx;
		if(idx == MnTST_OPT_SENSOR_DISTANCE + 1) realIdx = MnTST_OPT_MARQUEE;

		DpSTR_GuiLeft(box_itm_x0, str_y0+(str_yg*i), text_color, back_color, _fE17HsB, lDpTst.sItm[realIdx]);
		DpSTR_GuiLeft(box_val_x0, str_y0+(str_yg*i), cStr,           _cTTB_BG_MID, _fE17HsB, lDpTst.sVal[realIdx]);
	}
}

//------------------------------------------------------------------------------------------------------------------------------
//  EOF
//------------------------------------------------------------------------------------------------------------------------------

