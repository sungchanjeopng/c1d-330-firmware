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
// api
#include "measure_main.h"
// bsp
#include "bsp_adc.h"
#include "bsp_sdram.h"
#include "bsp_nand.h"
#include "bsp_ram.h"

// app
#include "app_status.h"
#include "app_schd1sec.h"
// display
#include "disp_string.h"
#include "disp_figure.h"
#include "disp_color.h"
// screen
#include "screen_main.h"
#include "screen_hybrid.h"
#include "screen_echo.h"
#include "screen_calib.h"
#include "screen_eco1_list.h"
#include "screen_eco2_value.h"
// measure
#include "measure_calc.h"
// menu
#include "menu_engineer.h"
#include "menu_factory.h"
#include "menu_measure.h"
#include "menu_output.h"
#include "menu_data.h"
#include "menu_test.h"
// self
#include "measure_calc.h"
// function
#include "output_current.h"
#include "output_pcd.h"
#include "data_commu.h"

#include "app_main.h"

//------------------------------------------------------------------------------------------------------------------------------
//  Global variables
//------------------------------------------------------------------------------------------------------------------------------
U16 test_cnt = 0;


//------------------------------------------------------------------------------------------------------------------------------
//  Local variables
//------------------------------------------------------------------------------------------------------------------------------
MsCAL_LS lMsCal;

// SDRAM_Prof(0xD0400000~ 0xD0450000 /0xD0450000~ 0xD0500000 ) - / SDRAM_ADDR(0xD0400000~ 0xD0800000)
U16 *SDRAM_aEco_prof_S0 = (U16 *)SDRAM_PROFILE_S0_ADDR;
U16 *SDRAM_aEco_prof_S1 = (U16 *)SDRAM_PROFILE_S1_ADDR;
U16 *SDRAM_aEco_Damping = (U16 *)0xD0500000;

U16 *SDRAM_aEco_prof_S3 = (U16 *)0xD0620000;

U16 auto_gain_prof_cnt[2];


//------------------------------------------------------------------------------------------------------------------------------
//  Local Funtions
//------------------------------------------------------------------------------------------------------------------------------
void MsCAL_InitVari(void)
{
	U16 i, j;

	for(j=0; j<MsCAL_THR_TYPE_NUM; j++)
	{
		lMsCal.stp3_dist[MsCAL_CH_S0][j] = MnMSR_BaseGet_Value(MnMS0_OPT_CH1_EMPTY);
		lMsCal.stp3_sldg[MsCAL_CH_S0][j] = 0;
		lMsCal.rslt_dist[MsCAL_CH_S0][j] = MnMSR_BaseGet_Value(MnMS0_OPT_CH1_EMPTY);
		lMsCal.rslt_sldg[MsCAL_CH_S0][j] = 0;
		lMsCal.stp3_dist[MsCAL_CH_S1][j] = MnMSR_BaseGet_Value(MnMS0_OPT_CH2_EMPTY);
		lMsCal.stp3_sldg[MsCAL_CH_S1][j] = 0;
		lMsCal.rslt_dist[MsCAL_CH_S1][j] = MnMSR_BaseGet_Value(MnMS0_OPT_CH2_EMPTY);
		lMsCal.rslt_sldg[MsCAL_CH_S1][j] = 0;		
	}
	

	for(i=0; i<MsCAL_PROF_MAX; i++)
	{
		for(j=0; j<ADC_BUFF_MAX; j++)
		{
			SDRAM_aEco_prof_S0[j+(i*ADC_BUFF_MAX)] = 0;
			SDRAM_aEco_prof_S1[j+(i*ADC_BUFF_MAX)] = 0;
			if(i<5)
			{
				lMsCal.aEco_filed_prof[MsCAL_CH_S0][i][j] = 0;
				lMsCal.aEco_filed_prof[MsCAL_CH_S1][i][j] = 0;
			}
		}
	}


	lMsCal.fauto_gain[APP_CH_1] = 0;
	lMsCal.fauto_gain[APP_CH_2] = 0;

	auto_gain_prof_cnt[APP_CH_1] = 0;
	auto_gain_prof_cnt[APP_CH_2] = 0;


	for(i=0; i<MsCAL_CH_NUM; i++)
	{	
		lMsCal.csave_avg[i] = 0;
		lMsCal.asf_r[i] = 0;
		_MEMSET(lMsCal.aEco_rslt[i], 0, sizeof(lMsCal.aEco_rslt[i]));
		_MEMSET(lMsCal.dist_prof[i], 0, sizeof(lMsCal.dist_prof[i]));
		_MEMSET(lMsCal.aEco_real[i], 0, sizeof(lMsCal.aEco_real[i]));
		_MEMSET(lMsCal.save_real[i], 0, sizeof(lMsCal.aEco_real[i]));
		_MEMSET(lMsCal.save_rslt[i], 0, sizeof(lMsCal.aEco_rslt[i]));	
		_MEMSET(lMsCal.aEco_filed_rlst[i], 0, sizeof(lMsCal.aEco_filed_rlst[i]));	
		_MEMSET(lMsCal.aEco_field_real[i], 0, sizeof(lMsCal.aEco_field_real[i]));	
		_MEMSET(lMsCal.auto_gain_buffer[i], 0, sizeof(lMsCal.auto_gain_buffer[i]));
		for(j=0; j<MsCAL_THR_TYPE_NUM; j++)
		{	
			for(U08 k=0; k<MsCAL_DAMP_MAX; k++)
				SDRAM_aEco_Damping[(2*MsCAL_DAMP_MAX*i)+(MsCAL_DAMP_MAX*j)+k] = 0;
		}
	}

	lMsCal.diff_dist    = 0;
	lMsCal.diff_sldg    = 0;
	lMsCal.avrg_dist    = 0;
	lMsCal.avrg_sldg    = 0;
	lMsCal.auto_calib    = 1;

	lMsCal.auto_gain[APP_CH_1] = 0;
	lMsCal.auto_gain[APP_CH_2] = 0;	
	lMsCal.auto_gain_cnt[APP_CH_1] = 0;
	lMsCal.auto_gain_cnt[APP_CH_2] = 0;	
	for(i=0; i<MsCAL_CH_NUM; i++)
	{
		lMsCal.thr[i][MsCAL_THR_HEAVY] = ADC_VAL_MAX/2;
		lMsCal.thr[i][MsCAL_THR_LIGHT] = ADC_VAL_MAX/3;
		// Threshold
		for(j=0; j<MsCAL_THR_TYPE_NUM; j++)
		{	
			lMsCal.Refilter_cnt[i][j]  = 0;
			lMsCal.Tracking_dist[i][j] = 0;
			lMsCal.fRefilter[i][j]     = Tracking_OFF;
			lMsCal.NOT_Tracking_dist[i][j] = 0;
		}
		lMsCal.thr_max[i] = 0;
		lMsCal.thr_min[i] = 0;
		lMsCal.auto_gain_avg[i] = 0;
		// Tracking
		// Error
		lMsCal.f_run[i]		= ERROR_OFF;
		lMsCal.error_sec[i]		= 0;
		lMsCal.signal_ok_sec[i]	= 0;
		lMsCal.signal_ng_sec[i]	= 0;
		lMsCal.f_signal[i]		= SIGNAL_OK;	
		lMsCal.stt[i]		= ApSTT_MSR_IDLE;

		CalPrc_Threshold_Value(i);
		
		lMsCal.valu[i] = 0;

		lMsCal.field_sldg[i] = 0;
		lMsCal.field_dist[i] = 0; 
		lMsCal.thr_min[i] = 0;
		lMsCal.thr_max[i] = 0;
		lMsCal.auto_gain_count[i]  = 1;
		lMsCal.auto_gain_pass_cnt[i] = 0;
		lMsCal.auto_gain_fail_cnt[i] = 0;
		lMsCal.auto_gain_current_slope[i] =0;
	}
	lMsCal.mini_sens = APP_CH_1;
	lMsCal.cProf = 0;

	lMsCal.cnt = 0;
	lMsCal.step = 0;

	lMsCal.auto_slope_max[APP_CH_1] = 0;
	lMsCal.auto_slope_max[APP_CH_2] = 0;

	lMsCal.auto_gain_fslope[APP_CH_1] = MRM_RdByte(_mAUTO_GAIN_CH1_FSLOPE);
	lMsCal.auto_gain_fslope[APP_CH_2] = MRM_RdByte(_mAUTO_GAIN_CH2_FSLOPE);	
	
	lMsCal.auto_gain_before_slope[APP_CH_1] = MRM_RdWord(_mAUTO_GAIN_CH1_BEFORE_SLOPE_L);
	lMsCal.auto_gain_before_slope[APP_CH_2] = MRM_RdWord(_mAUTO_GAIN_CH2_BEFORE_SLOPE_L);

	lMsCal.auto_gain_before_xrange[APP_CH_1] = MRM_RdWord(_mAUTO_GAIN_CH1_SLOPE_XRANGE_L);
	lMsCal.auto_gain_before_xrange[APP_CH_2] = MRM_RdWord(_mAUTO_GAIN_CH2_SLOPE_XRANGE_L);		


	lMsCal.auto_gain_before_max[APP_CH_1][1] = MRM_RdWord(_mAUTO_GAIN_CH1_BEFORE_MAX_L);
	lMsCal.auto_gain_before_min[APP_CH_1][1] = MRM_RdWord(_mAUTO_GAIN_CH1_BEFORE_MIN_L);

	lMsCal.auto_gain_before_max[APP_CH_2][1] = MRM_RdWord(_mAUTO_GAIN_CH2_BEFORE_MAX_L);
	lMsCal.auto_gain_before_min[APP_CH_2][1] = MRM_RdWord(_mAUTO_GAIN_CH2_BEFORE_MIN_L);

}


void CalPrc_SttCheck(U08 iCh)
{
	U16 stt = ApSTT_GetStatus(iCh);
	U16 err_delay =  MnOUT_ErrPrGet_Value(MnOS3_OPT_ERR_DELAY);

	switch(lMsCal.stt[iCh])
	{
		case ApSTT_MSR_WEAK:
			switch(stt)
			{
				case ApSTT_MSR_TRAC:
				case ApSTT_MSR_OK:
				case ApSTT_MSR_IDLE:
					ApSTT_SetStatus(iCh, lMsCal.stt[iCh]);
					break;
				case ApSTT_MSR_NG:
				case ApSTT_TPR_NG:
					MsCAL1_InitVari();
					CalPrc_ResetFlag(iCh);
					ApSTT_SetStatus(iCh, lMsCal.stt[iCh]);
					break;
				default:
					break;
			}
			break;
		case ApSTT_MSR_TRAC:
			switch(stt)
			{
				case ApSTT_MSR_OK:	
				case ApSTT_MSR_WEAK:
				case ApSTT_MSR_IDLE:
					ApSTT_SetStatus(iCh, lMsCal.stt[iCh]);
					break;
				case ApSTT_MSR_NG:
				case ApSTT_TPR_NG:
					MsCAL1_InitVari();
					CalPrc_ResetFlag(iCh);
					ApSTT_SetStatus(iCh, lMsCal.stt[iCh]);
					break;
				default:
					break;
			}
			break;			
		case ApSTT_MSR_OK:
			switch(stt)
			{
				case ApSTT_MSR_OK:
					if(ApSCD_GetVl_Prof_Full(iCh)==FALSE)	ApSTT_SetStatus(iCh, ApSTT_MSR_TRAC);
					break;
				case ApSTT_MSR_WEAK:
				case ApSTT_MSR_TRAC:
				case ApSTT_MSR_IDLE:	
					if(ApSCD_GetVl_Prof_Full(iCh)==FALSE)	
					{
						ApSTT_SetStatus(iCh, ApSTT_MSR_TRAC);
						break;
					}
					lMsCal.error_sec[iCh] = 0;
					ApSTT_SetStatus(iCh, ApSTT_MSR_OK);
					break;
				case ApSTT_MSR_NG:
				case ApSTT_TPR_NG:	
					MsCAL1_InitVari();
					CalPrc_ResetFlag(iCh);
					ApSTT_SetStatus(iCh, lMsCal.stt[iCh]);
					break;
				default:
					break;
			}
			break;
		case ApSTT_MSR_NG:
			switch(stt)
			{
				case ApSTT_MSR_OK:				
				case ApSTT_MSR_WEAK:
				case ApSTT_MSR_TRAC:
					if(++lMsCal.error_sec[iCh]>err_delay)
					{	
						lMsCal.error_sec[iCh]=0;
						ApSTT_SetStatus(iCh, lMsCal.stt[iCh]);
					}	
					break;						
				case ApSTT_TPR_NG:
				case ApSTT_MSR_IDLE:	
					ApSTT_SetStatus(iCh, lMsCal.stt[iCh]);
					break;
				default:
					break;
			}
			break;			
		case ApSTT_TPR_NG:
			switch(stt)
			{
				case ApSTT_MSR_OK:				
				case ApSTT_MSR_WEAK:
				case ApSTT_MSR_TRAC:
					if(++lMsCal.error_sec[iCh]>err_delay)
					{	
						lMsCal.error_sec[iCh]=0;
						ApSTT_SetStatus(iCh, lMsCal.stt[iCh]);
					}	
					break;						
				case ApSTT_MSR_NG:
				case ApSTT_MSR_IDLE:	
					ApSTT_SetStatus(iCh, lMsCal.stt[iCh]);
					break;
				default:
					break;
			}
			break;
		default:
			break;
	}

}

void CalPrc_ResetFlag(U08 ch)
{
	U16 j,i;

	for(j=0; j<MsCAL_THR_TYPE_NUM; j++)
	{
		lMsCal.Refilter_cnt[ch][j]	= 0;
		lMsCal.Tracking_dist[ch][j] = 0;
		lMsCal.fRefilter[ch][j]	= Tracking_OFF;
	}

	for(i=0; i<MsCAL_PROF_MAX; i++)
	{
		for(j=0; j<ADC_BUFF_MAX; j++)
		{
			if(ch ==APP_CH_1)
				SDRAM_aEco_prof_S0[j+(i*ADC_BUFF_MAX)] = 0;
			if(ch ==APP_CH_2)
				SDRAM_aEco_prof_S1[j+(i*ADC_BUFF_MAX)] = 0;
			if(i<5)
			{
				lMsCal.aEco_filed_prof[ch][i][j] = 0;

			}
		}
	}

	auto_gain_prof_cnt[ch] = 0;


	_MEMSET(lMsCal.aEco_rslt[ch], 0, sizeof(lMsCal.aEco_rslt[ch]));
	_MEMSET(lMsCal.dist_prof[ch], 0, sizeof(lMsCal.dist_prof[ch]));
	_MEMSET(lMsCal.aEco_real[ch], 0, sizeof(lMsCal.aEco_real[ch]));
	_MEMSET(lMsCal.save_real[ch], 0, sizeof(lMsCal.aEco_real[ch]));
	_MEMSET(lMsCal.save_rslt[ch], 0, sizeof(lMsCal.aEco_rslt[ch]));	
	_MEMSET(lMsCal.aEco_filed_rlst[ch], 0, sizeof(lMsCal.aEco_filed_rlst[ch]));	
	_MEMSET(lMsCal.aEco_field_real[ch], 0, sizeof(lMsCal.aEco_field_real[ch]));	
	
	for(j=0; j<MsCAL_THR_TYPE_NUM; j++)
	{	
		for(U08 k=0; k<MsCAL_DAMP_MAX; k++)
			SDRAM_aEco_Damping[(2*MsCAL_DAMP_MAX*ch)+(MsCAL_DAMP_MAX*j)+k] = 0;
	}


	// Threshold
	for(j=0; j<MsCAL_THR_TYPE_NUM; j++)
	{	
		lMsCal.Refilter_cnt[ch][j]  = 0;
		lMsCal.Tracking_dist[ch][j] = 0;
		lMsCal.fRefilter[ch][j]	   = Tracking_OFF;
		lMsCal.NOT_Tracking_dist[ch][j] = 0;
		lMsCal.NOT_Tracking_dist[ch][j] = 0;
	}
	lMsCal.thr_max[ch] = 0;
	lMsCal.thr_min[ch] = 0;
	// Tracking
	// Error
	lMsCal.f_run[ch] 	= ERROR_OFF;
	lMsCal.error_sec[ch] 	= 0;
	lMsCal.signal_ok_sec[ch] = 0;
	lMsCal.signal_ng_sec[ch] = 0;
	lMsCal.f_signal[ch]		= SIGNAL_OK;	
	lMsCal.stt[ch]		= ApSTT_MSR_IDLE;
	
	CalPrc_Threshold_Value(ch);
	
	lMsCal.valu[ch] = 0;
	
	lMsCal.field_sldg[ch] = 0;
	lMsCal.field_dist[ch] = 0; 

	lMsCal.stt[ch] =  ApSTT_MSR_IDLE;

	lMsCal.auto_gain_pass_cnt[ch] = 0;
	lMsCal.auto_gain_fail_cnt[ch] = 0;
	lMsCal.auto_gain_current_slope[ch] = 0;

	ApSCD_Init1Sec(ch);
}

U16 CalPrc_GetAsfCnt(U08 ch, U16 st, U16 thd)
{
	U16 i;
	U16 c = 0;
	U16 empty = MnMSR_BaseGet_Ch_Value(ch, MnMS0_OPT_SINGLE_EMPTY);
	U16 signal_end = empty*1.1;
	U16 asf;
	U08 screen_type = ScECH_GetType();
	U08 ifn = ScEC1_GetIdxFunc();
	U16 val = ScEC2_GetValue();


	if((ScECH_GetLayer() == SCRN_L2_VALU) && (SCRN_GetPage() == SCRN_P2_ECHO))
	{
		switch(screen_type)
		{
			case ScECO_TYPE_REAL:	break;
			case ScECO_TYPE_AVG:
				switch(ifn)
				{
					case ScECO_AVG_THR_LIGHT:	break;
					case ScECO_AVG_THR_HEAVY:	break;
					case ScECO_AVG_ASF_LIGHT:
					case ScECO_AVG_ASF_HEAVY:
						signal_end = val;
						break;
					default:
						break;
				}
				break;
			default:
				break;
		}	
	}

	if(st<signal_end)
	{
		for(i=st; i<signal_end; i++)
		{
			if(thd <= lMsCal.aEco_rslt[ch][i])		c++;
			else									break;
		}
		asf = c*13.5;

		if(lMsCal.asf_r[ch] < asf)
			lMsCal.asf_r[ch] = asf;

		if(lMsCal.asf_h[ch] < lMsCal.asf_r[ch])
			lMsCal.asf_h[ch] = lMsCal.asf_r[ch];
	}
	
	return c;
}


U16 TPRT_Calib(U08 ch, U16 index)
{
	F32 c_factor;
	U16 return_dist;
	F32 val = MsTPR_GetTprt(ch) * 0.1;

	c_factor = (1410 + (val * 4.6) - (val * val * 0.055) + (val * val * val * 0.00029)) * 0.5;

	return_dist = ((index * 13.5 * 0.000001) * c_factor) * 100;

	return return_dist;
}


U16 TPRT_Calib_Reverse(U08 ch, U16 return_dist)
{
    F32 c_factor;
    U16 index;
    F32 val = MsTPR_GetTprt(ch) * 0.1;

    c_factor = (1410 + (val * 4.6) - (val * val * 0.055) + (val * val * val * 0.00029)) * 0.5;

    index = (U16)(return_dist / (0.00135 * c_factor));

    return index;
}




void CalPrc_Tprt_Check(U08 ch)
{
	S16 tprt = MsTPR_GetTprt(ch);

	switch(lMsCal.stt[ch])
	{
		case ApSTT_TPR_NG:
			if((tprt < -200) || (tprt > 1000))
    		{
				lMsCal.stt[ch] = ApSTT_TPR_NG;
			}
			else
			{
				lMsCal.stt[ch] = ApSTT_MSR_IDLE;
			}
			break;
		case ApSTT_MSR_NG:
		case ApSTT_MSR_WEAK:
		case ApSTT_MSR_TRAC:
		case ApSTT_MSR_IDLE:
		case ApSTT_MSR_OK:
			if((tprt < -200) || (tprt > 1000))
			{
				lMsCal.stt[ch] = ApSTT_TPR_NG;
			}
			break;
		default:				break;
	}
}


void CalPrc_Threshold_Auto_Value(U16 thr_val, U08 type,U08 ch)
{
	U16 per_end;
	S16 range = 0;
	S16 thr;
	U16 i;
	U16 deadz = MnMSR_BaseGet_Ch_Value(ch, MnMS0_OPT_SINGLE_DEADZONE);
	U16 empty = MnMSR_BaseGet_Ch_Value(ch, MnMS0_OPT_SINGLE_EMPTY);
	U16 signal_range = empty*1.1;
	U16 dist;
	
	lMsCal.thr_min[ch] = MsCAL_THR_MAX;	
	lMsCal.thr_max[ch] = 0;


	switch(lMsCal.stt[ch])
	{
		case ApSTT_MSR_OK:
		case ApSTT_MSR_TRAC:
			if(lMsCal.Tracking_dist[ch][MsCAL_THR_HEAVY] >lMsCal.Tracking_dist[ch][MsCAL_THR_LIGHT])
			{
				dist = lMsCal.Tracking_dist[ch][MsCAL_THR_HEAVY];
				per_end = lMsCal.Tracking_dist[ch][MsCAL_THR_HEAVY] + MnTST_PrGet_CH_Value(ch, MnTST_OPT_SINGLE_THR_RANGE);
			}
			else
			{
				dist = lMsCal.Tracking_dist[ch][type];
				per_end = lMsCal.Tracking_dist[ch][type] + MnTST_PrGet_CH_Value(ch, MnTST_OPT_SINGLE_THR_RANGE);
			}
			if(per_end>=signal_range)		per_end = signal_range;

			for(i=deadz; i<=signal_range; i++)
			{
				if(i<=dist)
				{
					if(lMsCal.thr_min[ch]>lMsCal.aEco_rslt[ch][i])		
					{
						lMsCal.thr_min[ch] = lMsCal.aEco_rslt[ch][i];
						lMsCal.thr_min_index[ch] = i;
					}
				}
				else if(i<=per_end)
				{
					if(lMsCal.thr_max[ch]<lMsCal.aEco_rslt[ch][i])		
					{
						lMsCal.thr_max[ch] = lMsCal.aEco_rslt[ch][i];
						lMsCal.thr_max_index[ch] = i;
					}
				}
			}

			break;
		case ApSTT_MSR_WEAK:		
			per_end = empty*1.1;
			for(i=deadz; i<=signal_range; i++)
			{
				if(i<=per_end)
				{
					if(lMsCal.thr_min[ch]>lMsCal.aEco_rslt[ch][i])		
					{
						lMsCal.thr_min_index[ch] = i;
						lMsCal.thr_min[ch] = lMsCal.aEco_rslt[ch][i];
					}
					if(lMsCal.thr_max[ch]<lMsCal.aEco_rslt[ch][i])	
					{
						lMsCal.thr_max[ch] = lMsCal.aEco_rslt[ch][i];
						lMsCal.thr_max_index[ch] = i;
					}
				}
			}

			break;
		default:				
			return;
	}




	range = lMsCal.thr_max[ch]-lMsCal.thr_min[ch];
	if(range<=0)	range = 0;
	
	thr = (lMsCal.thr_min[ch]+(F32)((range*thr_val)/100));

	if(thr>lMsCal.thr_max[ch])	thr = lMsCal.thr_max[ch];
	if(thr<0)	thr=0;
	if(thr>4095)	thr=4095;

	lMsCal.thr[ch][type] = thr;


}
void CalPrc_Threshold_Value(U08 ch)
{
	U16 thr_light_val = MnMSR_Get_Threshold_Ch_Value(ch, MnMS1_OPT_SINGLE_THR_LIGHT);
	U16 thr_heavy_val = MnMSR_Get_Threshold_Ch_Value(ch, MnMS1_OPT_SINGLE_THR_HEAVY);
	U16 thr_light_mod = MnMSR_CalGet_Ch_Value(ch, MnMS1_OPT_SINGLE_THR_LIGHT);
	U16 thr_heavy_mod = MnMSR_CalGet_Ch_Value(ch, MnMS1_OPT_SINGLE_THR_HEAVY);

	

	switch(thr_light_mod)
	{
		case MnMS1_THRESHOLD_AUTO:		CalPrc_Threshold_Auto_Value(thr_light_val, MsCAL_THR_LIGHT,ch);							break;
		case MnMS1_THRESHOLD_MANUAL:	lMsCal.thr[ch][MsCAL_THR_LIGHT] = (thr_light_val*ADC_VAL_MAX)/MnMS1_THR_VAL_MANUAL_MAX;	break;
		default:						break;
	}


	switch(thr_heavy_mod)
	{
		case MnMS1_THRESHOLD_AUTO:		CalPrc_Threshold_Auto_Value(thr_heavy_val, MsCAL_THR_HEAVY,ch);							break;
		case MnMS1_THRESHOLD_MANUAL:	lMsCal.thr[ch][MsCAL_THR_HEAVY] = (thr_heavy_val*ADC_VAL_MAX)/MnMS1_THR_VAL_MANUAL_MAX;	break;
		default:						break;
	}



#if 0
	lMsCal.thr_min[ch] = MsCAL_THR_MAX;	
	lMsCal.thr_max[ch] = 0;

	switch(lMsCal.fRefilter[ch])
	{
		case Tracking_ON:	
			per_end = lMsCal.Tracking_dist[ch] + MsCAL_TRACK_RANGE;	
			if(per_end>=empty)		per_end = empty;
			break;
		case Tracking_OFF:	
			per_end = empty;						
			break;
		default:							
			break;
	}


	for(i=deadz; i<=empty; i++)
	{
		if(i<=per_end)
		{
			if(lMsCal.thr_min[ch]>lMsCal.aEco_rslt[ch][i])		lMsCal.thr_min[ch] = lMsCal.aEco_rslt[ch][i];
			if(lMsCal.thr_max[ch]<lMsCal.aEco_rslt[ch][i])		lMsCal.thr_max[ch] = lMsCal.aEco_rslt[ch][i];
		}
	}

	range = lMsCal.thr_max[ch]-lMsCal.thr_min[ch];
	if(range<=0)	range = 0;
	
	thr = (lMsCal.thr_min[ch]+(F32)((range*thr_sel)/100));

	if(thr>lMsCal.thr_max[ch])	thr = lMsCal.thr_max[ch];

	lMsCal.thr[ch] = thr;
#endif
}

U08 CalPrc_Flt3Stp(U08 ch,U08 type)
{
	U16 i;
	U16 asf_c = 0;
	U16 c=0;
	U16 asf_v;
	U16 deadz = MnMSR_BaseGet_Ch_Value(ch, MnMS0_OPT_SINGLE_DEADZONE);
	U16 empty = MnMSR_BaseGet_Ch_Value(ch, MnMS0_OPT_SINGLE_EMPTY);
	U16 signal_end = empty*1.1;
	U16 window_time = MnEGN_PrGet_CH_Item(ch, MnEGN_OPT_SINGLE_WINDOW_RESET_TIME);
	U16 window_range =  MnEGN_PrGet_CH_Item(ch, MnEGN_OPT_SINGLE_WINDOW_RANGE);
	U16 tracking_Hi;
	U16 tracking_Lo;
	U08 msr_chk;

	switch(type)
	{
		case MsCAL_THR_LIGHT:	asf_v = MnMSR_CalGet_Ch_Value(ch, MnMS1_OPT_SINGLE_ASF_LIGHT);	break;
		case MsCAL_THR_HEAVY:	asf_v = MnMSR_CalGet_Ch_Value(ch, MnMS1_OPT_SINGLE_ASF_HEAVY);	break;
	}


	if(asf_v>0) asf_v+=5;

	tracking_Hi = lMsCal.Tracking_dist[ch][type]+window_range;
	tracking_Lo = lMsCal.Tracking_dist[ch][type]-window_range;		
	msr_chk = ApSTT_MSR_WEAK;

	if(ApSCD_GetVl_Prof_Full(ch)==FALSE)	window_time = 0;
	//if(MnEGN_PrGet_Item(MnEGN_OPT_MSR)==MnEGN_MSR_ON)	window_time = 0;

	for(i=deadz; i<=signal_end; i++)
	{		
		if(lMsCal.thr[ch][type] <= lMsCal.aEco_rslt[ch][i])			//Threshold
		{
			c = CalPrc_GetAsfCnt(ch, i, lMsCal.thr[ch][type]);
			asf_c = (c*13.5);
			if(asf_c >= asf_v)									//ASF
			{
				msr_chk = ApSTT_MSR_TRAC;
				
				if(window_time>0)
				{
					switch(lMsCal.fRefilter[ch][type])				//Tracking
					{
						case Tracking_ON:
							if((i>=tracking_Lo) && (i<=tracking_Hi))
							{
								lMsCal.Refilter_cnt[ch][type]  = 0;
								lMsCal.Tracking_dist[ch][type] = i;
								lMsCal.stp3_dist[ch][type] = TPRT_Calib(ch, i);
								if(lMsCal.stp3_dist[ch][type]<=empty)
									lMsCal.stp3_sldg[ch][type] = empty - lMsCal.stp3_dist[ch][type];
								else
									lMsCal.stp3_sldg[ch][type] = 0;
								msr_chk = ApSTT_MSR_OK;
								return msr_chk;
							}
							break;
						case Tracking_OFF:
							if(i<=empty)	lMsCal.Tracking_dist[ch][type] = i;
							else			lMsCal.Tracking_dist[ch][type] = empty;
							lMsCal.stp3_dist[ch][type]     = TPRT_Calib(ch, i);
							if(lMsCal.stp3_dist[ch][type]<=empty)
								lMsCal.stp3_sldg[ch][type] = empty - lMsCal.stp3_dist[ch][type];
							else
								lMsCal.stp3_sldg[ch][type] = 0;

							lMsCal.fRefilter[ch][type]     	= Tracking_ON;
							msr_chk = ApSTT_MSR_OK;
							return msr_chk;
						default:		break;	
					}
				}
				else if(window_time ==0)
				{

					if(i<=empty)		lMsCal.Tracking_dist[ch][type] = i;
					else				lMsCal.Tracking_dist[ch][type] = empty;					
					lMsCal.stp3_dist[ch][type] = TPRT_Calib(ch, i);
					if(lMsCal.stp3_dist[ch][type]<=empty)
						lMsCal.stp3_sldg[ch][type] = empty - lMsCal.stp3_dist[ch][type];
					else
						lMsCal.stp3_sldg[ch][type] = 0;

					
					msr_chk = ApSTT_MSR_OK;	
					
					return msr_chk;
				}
			}
			i += c;
		}		
	}

	return msr_chk;
}


void CalPrc_3stp_dist(U08 ch)
{
	U16 window_time   = MnEGN_PrGet_CH_Item(ch, MnEGN_OPT_SINGLE_WINDOW_RESET_TIME);
	U08 msr_chk[MsCAL_THR_TYPE_NUM];
	U16 i,j;
	U16 min_x,max_x;
	U16 min_y=0xfff0;
	U16 max_y=0;

	
	CalPrc_Threshold_Value(ch);

	switch(lMsCal.stt[ch])
	{
		case ApSTT_MSR_OK:
		case ApSTT_MSR_WEAK:
		case ApSTT_MSR_TRAC:	break;
		default:				return;
	}


	for(j=0; j<2;	j++)
	{
		msr_chk[j] = CalPrc_Flt3Stp(ch,j);
		
		switch(msr_chk[j])
		{
			case ApSTT_MSR_OK: 		
				lMsCal.stt[ch] =  ApSTT_MSR_OK;
				break;
			case ApSTT_MSR_WEAK:	
				lMsCal.stt[ch] =  ApSTT_MSR_WEAK;
				//CalPrc_ResetFlag(ch);
				break;
			case ApSTT_MSR_TRAC:
				switch(lMsCal.fRefilter[ch][j])
				{
					case Tracking_ON:	
						for(i=0; i<MsCAL_THR_TYPE_NUM; i++)
						{
							if(++lMsCal.Refilter_cnt[ch][j]>window_time)
							{
								lMsCal.fRefilter[ch][j] = Tracking_OFF;
								lMsCal.Refilter_cnt[ch][j]=0;
								lMsCal.stt[ch] =  ApSTT_MSR_WEAK;
								if(MnEGN_PrGet_Item(MnEGN_OPT_MSR)==MnEGN_MSR_OFF)
									MsCAL_SaveEcho(ch);
								CalPrc_ResetFlag(ch);
							}
							else	
								lMsCal.stt[ch] =  ApSTT_MSR_TRAC;
						}
						break;
					case Tracking_OFF:	
						lMsCal.Refilter_cnt[ch][j]=0;
						lMsCal.stt[ch] =  ApSTT_MSR_WEAK;
						break;
					default:	
						break;
				}
				break;
			default:			
				break;
		}

	}


	if(ApSTT_GetStatus(ch)==ApSTT_MSR_OK)
	{
		U16 auto_range_x = MnTST_PrGet_Value(MnTST_OPT_AUTO_RANGE);
		(void)auto_range_x;
		U16 auto_volt_diff = MnTST_PrGet_Value(MnTST_OPT_AUTO_VOLT_DIFF);
		(void)auto_volt_diff;

		U08 buff[4096] = {0, };
		U16 cnt = 0;
		(void)buff;

		buff[cnt++] = 0x0d; // CR
		buff[cnt++] = 0x0a; // LF

		min_x = MsCAL_GetVl_TracDist(ch,MsCAL_THR_HEAVY)-MnTST_PrGet_Value(MnTST_OPT_AUTO_RANGE);

		if(min_x <MnMSR_BaseGet_Ch_Value(ch, MnMS0_OPT_SINGLE_DEADZONE))
			min_x = MnMSR_BaseGet_Ch_Value(ch, MnMS0_OPT_SINGLE_DEADZONE);

		max_x = MsCAL_GetVl_TracDist(ch,MsCAL_THR_HEAVY)+MnTST_PrGet_Value(MnTST_OPT_AUTO_RANGE);

		if(max_x >MnMSR_BaseGet_Ch_Value(ch, MnMS0_OPT_SINGLE_EMPTY))
			max_x = MnMSR_BaseGet_Ch_Value(ch, MnMS0_OPT_SINGLE_EMPTY);


		for(i=min_x; i<=MsCAL_GetVl_TracDist(ch,MsCAL_THR_HEAVY); i++)
		{
			if(MsCAL_GetVl_EchoBuff(ch,i)<min_y)
			{
				min_y = MsCAL_GetVl_EchoBuff(ch,i);
				lMsCal.auto_gain_current_min[ch][0] = i;
			}
		}

		lMsCal.auto_gain_current_min[ch][1] = min_y;


		for(i=MsCAL_GetVl_TracDist(ch,MsCAL_THR_HEAVY); i<=max_x; i++)
		{
			if(MsCAL_GetVl_EchoBuff(ch,i)>max_y)
			{
				max_y = MsCAL_GetVl_EchoBuff(ch,i);
				lMsCal.auto_gain_current_max[ch][0] = i;
			}
		}


		lMsCal.auto_gain_current_max[ch][1] = max_y;

		{
			S32 diff = abs(lMsCal.auto_gain_current_max[ch][0]-lMsCal.auto_gain_current_min[ch][0]);
			if(diff == 0) diff = 1;
			lMsCal.auto_gain_current_slope[ch] = (lMsCal.auto_gain_current_max[ch][1]-lMsCal.auto_gain_current_min[ch][1])/diff;
		}

		//before_volt_diff = lMsCal.auto_gain_before_slope[ch]*lMsCal.auto_gain_before_xrange[ch];
		//current_volt_diff = lMsCal.auto_gain_current_slope[ch]*(auto_range_x*2);
		//max_diff = lMsCal.auto_gain_current_max[ch][1] - lMsCal.auto_gain_before_max[ch][1];
		//min_diff = lMsCal.auto_gain_current_min[ch][1] - lMsCal.auto_gain_before_min[ch][1];


	}


}

#if 0
void MsCAL_ProcGain(U08 ch)
{
	if(MnMSR_CalGet_Ch_Value(APP_CH_1, MnMS1_OPT_SINGLE_AUTO_FAMP) == MnMS1_AUTO_FAMP_OFF)
		return; 	


	if(ApSTT_GetStatus(ch)==ApSTT_MSR_OK)
	{

#if 0
		U16 auto_range_x = MnTST_PrGet_Value(MnTST_OPT_AUTO_RANGE);
		U16 auto_volt_diff = MnTST_PrGet_Value(MnTST_OPT_AUTO_VOLT_DIFF);
		U16 slope_sel = (auto_volt_diff*100)/(auto_range_x*2);
		U32 auto_volt_peak = MnTST_PrGet_Value(MnTST_OPT_AUTO_VOLT_PEAK)*1240;
		U16 value_range =MnTST_PrGet_Value(MnTST_OPT_AUTO_LEVEL_RANGE);
		
		U16 val = MsCAL_GetVl_TracDist(ch, MsCAL_THR_HEAVY);
		U08 echo_amp;
#endif
#if 0
		lMsCal.auto_gain_before_max[ch][0] = MsCAL_GetVl_TracDist(ch,MsCAL_THR_HEAVY) + MnTST_PrGet_Value(MnTST_OPT_AUTO_RANGE);
		lMsCal.auto_gain_before_max[ch][1] = MsCAL_GetVl_EchoBuff(ch,MsCAL_GetVl_TracDist(ch,MsCAL_THR_HEAVY)+MnTST_PrGet_Value(MnTST_OPT_AUTO_RANGE));
		lMsCal.auto_gain_before_min[ch][0] = MsCAL_GetVl_TracDist(ch,MsCAL_THR_HEAVY) - MnTST_PrGet_Value(MnTST_OPT_AUTO_RANGE);
		lMsCal.auto_gain_before_min[ch][1] = MsCAL_GetVl_EchoBuff(ch,MsCAL_GetVl_TracDist(ch,MsCAL_THR_HEAVY)-MnTST_PrGet_Value(MnTST_OPT_AUTO_RANGE));
		
		lMsCal.auto_gain_before_slope[ch] = (lMsCal.auto_gain_before_max[ch][1]-lMsCal.auto_gain_before_min[ch][1])/abs(lMsCal.auto_gain_before_max[ch][0]-lMsCal.auto_gain_before_min[ch][0]);		
		
		
		if(lMsCal.auto_gain_before_max[ch][1]*100<auto_volt_peak)
		{
			if(++lMsCal.auto_gain_fail_cnt[ch]>50)
			{
				echo_amp = MnMSR_CalGet_Ch_Value(ch,MnMS1_OPT_SINGLE_ECHO_AMP)+1;
				if(echo_amp>=50)	echo_amp = 50;
				else if(echo_amp<=1) echo_amp = 1;

				MnMSR_CalSet_Ch_Value(ch, MnMS1_OPT_SINGLE_ECHO_AMP, echo_amp);				
			}
		}
		else
		{
			lMsCal.auto_gain_fail_cnt[ch] = 0;

			if(lMsCal.auto_gain_before_slope[ch]*8.05)<slope_sel)
			{
				if(++lMsCal.auto_gain_pass_cnt[ch]>50)
				{
					echo_amp = MnMSR_CalGet_Ch_Value(ch,MnMS1_OPT_SINGLE_ECHO_AMP)-1;
					if(echo_amp>=50)	echo_amp = 50;
					else if(echo_amp<=1) echo_amp = 1;

					MnMSR_CalSet_Ch_Value(ch, MnMS1_OPT_SINGLE_ECHO_AMP, echo_amp);				
				}
			}
			else
				lMsCal.auto_gain_pass_cnt[ch] = 0;
		}
#endif
#if 0
		U08 buff[4096] = {0, };
		U16 cnt = 0;

		buff[cnt++] = 0x0d; // CR
		buff[cnt++] = 0x0a; // LF

		buff[cnt++] = '/';					   // 'A'

		buff[cnt++] = (lMsCal.auto_gain_before_slope[ch] / 1000) % 10 + '0';
		buff[cnt++] = (lMsCal.auto_gain_before_slope[ch] / 100) % 10 + '0';
		buff[cnt++] = (lMsCal.auto_gain_before_slope[ch] / 10)	% 10 + '0';
		buff[cnt++] =  lMsCal.auto_gain_before_slope[ch]		% 10 + '0';		
#endif	

#if 0
		lMsCal.auto_gain_before_max[ch][0] = MsCAL_GetVl_TracDist(ch,MsCAL_THR_HEAVY) + MnTST_PrGet_Value(MnTST_OPT_AUTO_RANGE);
		lMsCal.auto_gain_before_max[ch][1] = MsCAL_GetVl_EchoBuff(ch,MsCAL_GetVl_TracDist(ch,MsCAL_THR_HEAVY)+MnTST_PrGet_Value(MnTST_OPT_AUTO_RANGE));
		lMsCal.auto_gain_before_min[ch][0] = MsCAL_GetVl_TracDist(ch,MsCAL_THR_HEAVY) - MnTST_PrGet_Value(MnTST_OPT_AUTO_RANGE);
		lMsCal.auto_gain_before_min[ch][1] = MsCAL_GetVl_EchoBuff(ch,MsCAL_GetVl_TracDist(ch,MsCAL_THR_HEAVY)-MnTST_PrGet_Value(MnTST_OPT_AUTO_RANGE));
		
		lMsCal.auto_gain_before_slope[ch] = (lMsCal.auto_gain_before_max[ch][1]-lMsCal.auto_gain_before_min[ch][1])/abs(lMsCal.auto_gain_before_max[ch][0]-lMsCal.auto_gain_before_min[ch][0]);

		
		if(lMsCal.auto_gain_before_max[ch][1]*100<auto_volt_peak)
		{

		}

		if((lMsCal.auto_gain_before_slope[ch]*8.05)>slope_sel)
		{

		}
#endif

#if 
		if(lMsCal.auto_gain_fslope[ch]==1)
		{
			lMsCal.auto_gain_before_max[ch][0] = MsCAL_GetVl_TracDist(ch,MsCAL_THR_HEAVY) + MnTST_PrGet_Value(MnTST_OPT_AUTO_RANGE);
			lMsCal.auto_gain_before_max[ch][1] = MsCAL_GetVl_EchoBuff(ch,MsCAL_GetVl_TracDist(ch,MsCAL_THR_HEAVY)+MnTST_PrGet_Value(MnTST_OPT_AUTO_RANGE));
			lMsCal.auto_gain_before_min[ch][0] = MsCAL_GetVl_TracDist(ch,MsCAL_THR_HEAVY) - MnTST_PrGet_Value(MnTST_OPT_AUTO_RANGE);
			lMsCal.auto_gain_before_min[ch][1] = MsCAL_GetVl_EchoBuff(ch,MsCAL_GetVl_TracDist(ch,MsCAL_THR_HEAVY)-MnTST_PrGet_Value(MnTST_OPT_AUTO_RANGE));

			lMsCal.auto_gain_before_slope[ch] = (lMsCal.auto_gain_before_max[ch][1]-lMsCal.auto_gain_before_min[ch][1])/abs(lMsCal.auto_gain_before_max[ch][0]-lMsCal.auto_gain_before_min[ch][0]);

			lMsCal.auto_gain_fslope[ch] = 0;
		}
#endif
#if 0

		if(lMsCal.auto_gain_fslope[ch]==1)
		{
			lMsCal.auto_gain_before_max[ch][0] = MsCAL_GetVl_TracDist(ch,MsCAL_THR_HEAVY) + MnTST_PrGet_Value(MnTST_OPT_AUTO_RANGE);
			lMsCal.auto_gain_before_max[ch][1] = MsCAL_GetVl_EchoBuff(ch,MsCAL_GetVl_TracDist(ch,MsCAL_THR_HEAVY)+MnTST_PrGet_Value(MnTST_OPT_AUTO_RANGE));
			lMsCal.auto_gain_before_min[ch][0] = MsCAL_GetVl_TracDist(ch,MsCAL_THR_HEAVY) - MnTST_PrGet_Value(MnTST_OPT_AUTO_RANGE);
			lMsCal.auto_gain_before_min[ch][1] = MsCAL_GetVl_EchoBuff(ch,MsCAL_GetVl_TracDist(ch,MsCAL_THR_HEAVY)-MnTST_PrGet_Value(MnTST_OPT_AUTO_RANGE));

			lMsCal.auto_gain_before_slope[ch] = (lMsCal.auto_gain_before_max[ch][1]-lMsCal.auto_gain_before_min[ch][1])/abs(lMsCal.auto_gain_before_max[ch][0]-lMsCal.auto_gain_before_min[ch][0]);

			lMsCal.auto_gain_fslope[ch] = 0;

		}
		else
		{

		
			lMsCal.auto_gain_current_max[ch][0] = MsCAL_GetVl_TracDist(ch,MsCAL_THR_HEAVY) + MnTST_PrGet_Value(MnTST_OPT_AUTO_RANGE);
			lMsCal.auto_gain_current_max[ch][1] = MsCAL_GetVl_EchoBuff(ch,MsCAL_GetVl_TracDist(ch,MsCAL_THR_HEAVY)+MnTST_PrGet_Value(MnTST_OPT_AUTO_RANGE));
			lMsCal.auto_gain_current_min[ch][0] = MsCAL_GetVl_TracDist(ch,MsCAL_THR_HEAVY) - MnTST_PrGet_Value(MnTST_OPT_AUTO_RANGE);
			lMsCal.auto_gain_current_min[ch][1] = MsCAL_GetVl_EchoBuff(ch,MsCAL_GetVl_TracDist(ch,MsCAL_THR_HEAVY)-MnTST_PrGet_Value(MnTST_OPT_AUTO_RANGE));

			lMsCal.auto_gain_current_slope[ch] = (lMsCal.auto_gain_current_max[ch][1]-lMsCal.auto_gain_current_min[ch][1])/abs(lMsCal.auto_gain_current_max[ch][0]-lMsCal.auto_gain_current_min[ch][0]);

			before_volt_diff = lMsCal.auto_gain_before_slope[ch]*auto_range_x*2;
			current_volt_diff = lMsCal.auto_gain_current_slope[ch]*auto_range_x*2;

			min_volt_diff = lMsCal.auto_gain_before_min[ch][1] - lMsCal.auto_gain_current_min[ch][1];
			max_volt_diff = lMsCal.auto_gain_before_max[ch][1] - lMsCal.auto_gain_current_max[ch][1];

			buff[cnt++] = (before_volt_diff / 1000) % 10 + '0';
			buff[cnt++] = (before_volt_diff / 100)	% 10 + '0';
			buff[cnt++] = (before_volt_diff / 10)	% 10 + '0';
			buff[cnt++] =  before_volt_diff 	% 10 + '0';

			buff[cnt++] = '/';					   // 'A'

			buff[cnt++] = (current_volt_diff / 1000) % 10 + '0';
			buff[cnt++] = (current_volt_diff / 100) % 10 + '0';
			buff[cnt++] = (current_volt_diff / 10)	% 10 + '0';
			buff[cnt++] =  current_volt_diff		% 10 + '0';

			buff[cnt++] = '/';					   // 'A'


			buff[cnt++] = (abs(min_volt_diff) / 1000) % 10 + '0';
			buff[cnt++] = (abs(min_volt_diff) / 100)	% 10 + '0';
			buff[cnt++] = (abs(min_volt_diff) / 10) % 10 + '0';
			buff[cnt++] =  abs(min_volt_diff)		% 10 + '0';

			buff[cnt++] = '/';					   // 'A'


			buff[cnt++] = (abs(max_volt_diff)/ 1000) % 10 + '0';
			buff[cnt++] = (abs(max_volt_diff)/ 100) % 10 + '0';
			buff[cnt++] = (abs(max_volt_diff)/ 10)	% 10 + '0';
			buff[cnt++] =  abs(max_volt_diff)	% 10 + '0';


			I08 sDbg[128];
			_SPRINTF(sDbg, "%d %d %d %d", before_volt_diff,current_volt_diff,min_volt_diff,max_volt_diff);
			DpFIG_DrwRect(150, 30, 400, 25, _cBGD_SCRN, DpFIG_FILL);
			DpSTR_GuiLeft(150, 30, _cECO_LVL_TITL, _cBGD_SCRN, _fE17HsB, sDbg); 


			if((lMsCal.thr_max[ch]*100<auto_volt_peak) || ((lMsCal.auto_gain_current_slope[ch]*8.05)<=slope_sel))
			{	
				if(++lMsCal.auto_gain_fail_cnt[ch]>10)
				{
					echo_amp = MnMSR_CalGet_Ch_Value(ch,MnMS1_OPT_SINGLE_ECHO_AMP)+1;
					if(echo_amp>=50)	echo_amp = 50;

					MnMSR_CalSet_Ch_Value(ch, MnMS1_OPT_SINGLE_ECHO_AMP, echo_amp);
					lMsCal.auto_gain_fail_cnt[ch] =0;
					buff[cnt++] = '/';					   // 'A'
					buff[cnt++] = 'F';					   // 'A'
					URT_TxPkt(URT_IDX_1, buff, cnt);
					_SPRINTF(sDbg, "f");
					DpFIG_DrwRect(150, 60, 400, 25, _cBGD_SCRN, DpFIG_FILL);
					DpSTR_GuiLeft(150, 60, _cECO_LVL_TITL, _cBGD_SCRN, _fE17HsB, sDbg); 

					return;
				}

			}
			else
				lMsCal.auto_gain_fail_cnt[ch] = 0;

			if(abs(before_volt_diff-current_volt_diff)>=125)
			{
				if(++lMsCal.auto_gain_pass_cnt[ch]>50)
				{
					if((before_volt_diff-current_volt_diff)>0)
						echo_amp = MnMSR_CalGet_Ch_Value(ch,MnMS1_OPT_SINGLE_ECHO_AMP)+1;
					else
						echo_amp = MnMSR_CalGet_Ch_Value(ch,MnMS1_OPT_SINGLE_ECHO_AMP)-1;

					if(echo_amp>=50)	echo_amp = 50;
					else if(echo_amp<=1) echo_amp = 1;

					MnMSR_CalSet_Ch_Value(ch, MnMS1_OPT_SINGLE_ECHO_AMP, echo_amp);

					buff[cnt++] = '/';					   // 'A'
					buff[cnt++] = 'P';					   // 'A'

					buff[cnt++] = '/';					   // 'A'
					
					
					buff[cnt++] = (echo_amp / 1000) % 10 + '0';
					buff[cnt++] = (echo_amp / 100) % 10 + '0';
					buff[cnt++] = (echo_amp / 10)	% 10 + '0';
					buff[cnt++] =  echo_amp 	% 10 + '0';
					
					I08 sDbg[128];
					_SPRINTF(sDbg, "%d ", echo_amp);
					DpFIG_DrwRect(150, 60, 400, 25, _cBGD_SCRN, DpFIG_FILL);
					DpSTR_GuiLeft(150, 60, _cECO_LVL_TITL, _cBGD_SCRN, _fE17HsB, sDbg); 

					lMsCal.auto_gain_pass_cnt[ch]=0;
				}	
			}
			else
				lMsCal.auto_gain_pass_cnt[ch] = 0;

			URT_TxPkt(URT_IDX_1, buff, cnt);

		}
		
		if(ch==APP_CH_1)
		{
			MRM_WrByte(_mAUTO_GAIN_CH1_FSLOPE,	lMsCal.auto_gain_fslope[ch]);
			MRM_WrWord(_mAUTO_GAIN_CH1_BEFORE_SLOPE_L,	lMsCal.auto_gain_before_slope[ch]);
		}
		else
		{
			MRM_WrByte(_mAUTO_GAIN_CH2_FSLOPE,	lMsCal.auto_gain_fslope[ch]);
			MRM_WrWord(_mAUTO_GAIN_CH2_BEFORE_SLOPE_L,	lMsCal.auto_gain_before_slope[ch]);
		
		}

#endif
	}


}
#endif
//------------------------------------------------------------------------------------------------------------------------------
//  Global APIs - Variables
//------------------------------------------------------------------------------------------------------------------------------
// Get Result
U16 MsCAL_GetVl_TracDist(U08 iCh,U08 type)					{	return lMsCal.Tracking_dist[iCh][type];		}
U16 MsCAL_GetVl_RsltDist(U08 iCh,U08 type)					{	return lMsCal.rslt_dist[iCh][type];			}
U16 MsCAL_GetVl_RsltSldg(U08 iCh,U08 type)					{	return lMsCal.rslt_sldg[iCh][type];			}
U16 MsCAL_GetVl_Stp3Dist(U08 iCh,U08 type)					{	return lMsCal.stp3_dist[iCh][type];			}
U16 MsCAL_GetVl_Stp3Sldg(U08 iCh,U08 type)					{	return lMsCal.stp3_sldg[iCh][type];			}
U16 MsCAL_GetVl_EchoBuff(U08 iCh, U16 idx)					{	return lMsCal.aEco_rslt[iCh][idx];			}
U16* MsCAL_GetVl_Echo(U08 iCh)							{	return lMsCal.aEco_rslt[iCh];				}
U16 MsCAL_GetVl_EchoReal(U08 iCh, U16 idx)					{	return lMsCal.aEco_real[iCh][idx];			}
U16 MsCAL_GetVl_SaveBuff(U08 iCh, U16 idx)					{	return lMsCal.save_rslt[iCh][idx];			}
U16 MsCAL_GetVl_SaveReal(U08 iCh, U16 idx)					{	return lMsCal.save_real[iCh][idx];			}
U16 MsCAL_GetVl_FieldBuff(U08 iCh, U16 idx)					{	return lMsCal.aEco_filed_rlst[iCh][idx];	}
U16 MsCAL_GetVl_FieldReal(U08 iCh, U16 idx)					{	return lMsCal.aEco_field_real[iCh][idx];	}
U16 MsCAL_GetVl_DiffDist(void)								{	return lMsCal.diff_dist;				}
U16 MsCAL_GetVl_DiffSldg(void)								{	return lMsCal.diff_sldg;				}
U16 MsCAL_GetVl_AvrgDist(void)								{	return lMsCal.avrg_dist;				}
U16 MsCAL_GetVl_AvrgSldg(void)								{	return lMsCal.avrg_sldg;				}
U16 MsCAL_GetVl_MiniDist(void)								{ 	return lMsCal.mini_dist;				}
U16 MsCAL_GetVl_MiniSldg(void)								{ 	return lMsCal.mini_sldg;				}
U08 MsCAL_GetVl_MiniSens(void)								{	return lMsCal.mini_sens;				}
U32 MsCAL_GetVl_Threshold(U08 iCh,U08 type)					{	return lMsCal.thr[iCh][type];				}
U32 MsCAL_GetVl_Threshold_Max(U08 iCh)						{	return lMsCal.thr_max[iCh];				}
U32 MsCAL_GetVl_Threshold_Min(U08 iCh)						{	return lMsCal.thr_min[iCh];				}

U16 MsCAL_GetVl_ASF_R(U08 iCh)								{	return lMsCal.asf_r[iCh];				}
U16 MsCAL_GetVl_ASF_H(U08 iCh)								{	return lMsCal.asf_h[iCh];				}

U16 MsCAL_GetVl_FieldDist(U08 iCh)							{	return lMsCal.field_dist[iCh];			}
U16 MsCAL_GetVl_FieldSldg(U08 iCh)							{	return lMsCal.field_sldg[iCh];			}


U08 MsCAL_Get_Stt(U08 iCh)									{	return lMsCal.stt[iCh];					}

U16 MsCAL_GetAutoGain(void)									{	return lMsCal.auto_calib;				}
U08 MsCAL_GetFAutoGain(void)								{	return lMsCal.fauto_calib;				}



// Set
void MsCAL_SetVl_ASF_R(U08 iCh,U16 val)						{	lMsCal.asf_r[iCh] = val;				}
void MsCAL_SetVl_ASF_H(U08 iCh,U16 val)						{	lMsCal.asf_h[iCh] = val;				}
void MsCAL_SetAutoGain(U08 val)								{ 	lMsCal.auto_calib = val;					}
void MsCAL_SetFAutoGain(U08 sel)							{	lMsCal.fauto_calib = sel;				}

//------------------------------------------------------------------------------------------------------------------------------
//  Global APIs - Converter Unit
//------------------------------------------------------------------------------------------------------------------------------
F32 MsCAL_Feet2Meter(U16 feet)		{		return (feet  * MsCAL_CNST_FT2MT) + 0.5;		}
F32 MsCAL_Meter2Feet(U16 meter)		{		return (meter * MsCAL_CNST_MT2FT) + 0.5;		}

void MsCAL_SetVl_RsltDist(U08 iCh,U08 type, U16 val)					{	lMsCal.rslt_dist[iCh][type] = val;			}
void MsCAL_SetVl_RsltSldg(U08 iCh,U08 type, U16 val)					{	lMsCal.rslt_sldg[iCh][type] = val;			}

//------------------------------------------------------------------------------------------------------------------------------
//  Global APIs - Basic
//------------------------------------------------------------------------------------------------------------------------------



void MsCAL_CheckWeak(U08 iCh)
{
	U32 i;
	S16 thr_max = 0;
	U16 thr_min = (MnTST_PrGet_CH_Value(iCh, MnTST_OPT_SINGLE_THR_MIN)*4096)/33;
	U16 deadz = MnMSR_BaseGet_Ch_Value(iCh, MnMS0_OPT_SINGLE_DEADZONE);
	U16 empty = MnMSR_BaseGet_Ch_Value(iCh, MnMS0_OPT_SINGLE_EMPTY);
	
	U16 msr_range= (empty)*1.1;

	if((ScECH_GetLayer() == SCRN_L2_VALU) || (ScCLB_GetLayer() == SCRN_L2_VALU) || (MENU_GetLayer() >= MENU_L3_VALUE)) 
		return;

	switch(OuCUR_GetPCD_fRun())
	{
		case PCD_OFF:	break;
		case PCD_ON:	
		case PCD_TEST:	return;
		default:
			return;	
	}


	for(i=deadz; i<ADC_BUFF_MAX; i++)
	{
		if(i<=msr_range)
		{	
			if(ADC_GetEchoVal(iCh, i)>thr_min)		thr_max = ADC_GetEchoVal(iCh, i);
			if(thr_max<=0)							thr_max = 0;
		}
	}

	
	if(thr_max<thr_min)
	{
		switch(lMsCal.stt[iCh])
		{
			case ApSTT_TPR_NG:		return;
			case ApSTT_MSR_NG:		
				if(++lMsCal.signal_ng_sec[iCh]>MsCAL_SIGNAL_NG_TIME)
				{
					lMsCal.f_signal[iCh] = SIGNAL_NG;	
					lMsCal.signal_ok_sec[iCh] = 0;
					lMsCal.signal_ng_sec[iCh] = 0;
				}
				return;
			case ApSTT_MSR_OK:
			case ApSTT_MSR_TRAC:
			case ApSTT_MSR_IDLE:
			case ApSTT_MSR_WEAK:	lMsCal.stt[iCh] =  ApSTT_MSR_NG;
									return;
			default:				return;
		}
	}
	else if (thr_max>=thr_min)
	{
		switch(lMsCal.stt[iCh])
		{
			case ApSTT_MSR_IDLE:	lMsCal.stt[iCh] =  ApSTT_MSR_WEAK;	break;
			case ApSTT_MSR_NG:	
				lMsCal.error_sec[iCh]=0;
				switch(lMsCal.f_signal[iCh])
				{
					case SIGNAL_NG:
						if(++lMsCal.signal_ok_sec[iCh]>MsCAL_SIGNAL_OK_TIME)
						{	
							lMsCal.stt[iCh] =  ApSTT_MSR_WEAK;
							lMsCal.f_signal[iCh] = SIGNAL_OK;
							lMsCal.signal_ok_sec[iCh] = 0;
						}	
						return;
					case SIGNAL_OK:			
						lMsCal.stt[iCh] =  ApSTT_MSR_WEAK;
						break;
					default:			return;
				}
				break;
			case ApSTT_TPR_NG:		return;
			case ApSTT_MSR_OK:
			case ApSTT_MSR_TRAC:
			case ApSTT_MSR_WEAK:	break;			
			default:				return;
		}
	}

}


void MsCAL_SaveEcho(U08 ch)
{
	U16 i;

	for(i=0; i<ADC_BUFF_MAX; i++)
	{
		lMsCal.save_real[ch][i] = lMsCal.aEco_real[ch][i];
		lMsCal.save_rslt[ch][i] = lMsCal.aEco_rslt[ch][i];
	}
}



void MsCAL_FieldAProf(U08 iCh,U08 idx)
{
	U32 i;
	S32 j;
	U64 sum[ADC_BUFF_MAX] = {0, };
	U16 deadz = MnMSR_BaseGet_Ch_Value(iCh, MnMS0_OPT_SINGLE_DEADZONE);
	U16 empty = MnMSR_BaseGet_Ch_Value(iCh, MnMS0_OPT_SINGLE_EMPTY);

	U16 empty_range = empty*1.1;
	
	if(idx>=5)
		idx=5;

	// Buffer Shift Save
	for(j=idx-1; j>0; j--)
	{
		if(idx<=1)	break;
		for(i=0; i<ADC_BUFF_MAX; i++)
		{
			switch(iCh)
			{
				case MsCAL_CH_S0 : lMsCal.aEco_filed_prof[iCh][j][i] = lMsCal.aEco_filed_prof[iCh][j-1][i]; break;
				case MsCAL_CH_S1 : lMsCal.aEco_filed_prof[iCh][j][i] = lMsCal.aEco_filed_prof[iCh][j-1][i]; break;
			}
		}
		DaMdb_ProcMain();
	}

	DaMdb_ProcMain();

	// Add Data
	for(i=0; i<ADC_BUFF_MAX; i++)
	{
		switch(iCh)
			{
			case MsCAL_CH_S0 : lMsCal.aEco_filed_prof[iCh][0][i] = ADC_GetEchoVal(iCh, i); break;
			case MsCAL_CH_S1 : lMsCal.aEco_filed_prof[iCh][0][i] = ADC_GetEchoVal(iCh, i); break;
		}
	}

	DaMdb_ProcMain();

	// Moving Average
	for(i=0; i<ADC_BUFF_MAX; i++)
	{
		for(j=0; j<idx; j++)
		{
			switch(iCh)
			{
				case MsCAL_CH_S0 : sum[i] += lMsCal.aEco_filed_prof[iCh][j][i]; break;
				case MsCAL_CH_S1 : sum[i] += lMsCal.aEco_filed_prof[iCh][j][i]; break;
			}
		}

		switch(iCh)
		{
			case MsCAL_CH_S0:	lMsCal.aEco_filed_rlst[MsCAL_CH_S0][i] = sum[i]/idx;	break;
			case MsCAL_CH_S1:	lMsCal.aEco_filed_rlst[MsCAL_CH_S1][i] = sum[i]/idx;	break;
		}
	}

	DaMdb_ProcMain();

	if(ApSCD_GetVl_Prof_Full(iCh))
	{
		for(i=deadz+50; i<empty_range; i++)
		{
			if(lMsCal.aEco_filed_rlst[iCh][i]>MsCAL_FIELD_THR)
			{
				lMsCal.field_dist[iCh] = i;
				if(lMsCal.field_dist[iCh]>=empty)
					lMsCal.field_dist[iCh] = empty;
				lMsCal.field_sldg[iCh] = empty-lMsCal.field_dist[iCh];

#if 0
				if((lMsCal.field_sldg[iCh]+offs)<=0)
					lMsCal.field_sldg[iCh] = 0;
				else
					lMsCal.field_sldg[iCh] = lMsCal.field_sldg[iCh]+offs;
#endif

				if((lMsCal.field_sldg[iCh])>=empty)
					lMsCal.field_sldg[iCh] = empty;					
				i=empty;
				break;
			}	
			else
			{
				lMsCal.field_dist[iCh] = empty;
				lMsCal.field_sldg[iCh] = 0;
			}
		}
	}
}


void MsCal_fieldReal(U08 iCh)
{
	U16 i;

	for(i=0; i<ADC_BUFF_MAX; i++)
	{
		lMsCal.aEco_field_real[iCh][i] = ADC_GetEchoVal(iCh, i);
	}


}

void MsCAL_ProcReal(U08 iCh)
{
	U16 i;

	if(MnMSR_CalGet_Ch_Value(iCh, MnMS1_OPT_SINGLE_AUTO_FAMP) == MnMS1_AUTO_FAMP_OFF)
	{
		for(i=0; i<ADC_BUFF_MAX; i++)
		{
			lMsCal.aEco_real[iCh][i] = ADC_GetEchoVal(iCh, i);
		}
	}
	else
	{
		if(ScECH_GetLayer() == SCRN_L2_VALU)
		{
			for(i=0; i<ADC_BUFF_MAX; i++)
			{
				lMsCal.aEco_real[iCh][i] = ADC_GetEchoVal(iCh, i);
			}

		}
		else
		{
			if(lMsCal.fauto_gain[iCh]==1)
			{
				for(i=0; i<ADC_BUFF_MAX; i++)
				{
					lMsCal.aEco_real[iCh][i] =SDRAM_aEco_prof_S3[(((40*iCh)+lMsCal.auto_gain_cnt[iCh])*ADC_BUFF_MAX)+i];
				}
			}		
			else
			{
				for(i=0; i<ADC_BUFF_MAX; i++)
				{
					lMsCal.aEco_real[iCh][i] = ADC_GetEchoVal(iCh, i);
				}

			}
		}
	}

}

void MsCAL_Auto_Gain_Real(U08 ch)
{
	U16 i;

	for(i=0; i<ADC_BUFF_MAX; i++)
	{
		SDRAM_aEco_prof_S3[(((40*ch)+lMsCal.auto_calib_cnt)*ADC_BUFF_MAX)+i] =ADC_GetEchoVal(ch, i);
	}
}

 
#if 1
void MsCAL_Auto_Gain_test(U08 ch)
{
	U16 i,j,k;
	U16 min_x;
	U16 max_x;
	U16 ref_min_volt;
	U16 ref_max_volt;
	U16 ref_min_x;
	U16 ref_max_x = 0;
	U16 min_volt[20];
	U16 max_volt[20];
	U16 slope[20];
	U16 max_slope = 0;
	U16 prof = MnEGN_PrGet_CH_Item(ch, MnEGN_OPT_SINGLE_PROFILE);
	U16 sum=0;
	U16 gain;

	if(++auto_gain_prof_cnt[ch] >= prof)
		auto_gain_prof_cnt[ch] = prof;

	min_x = MsCAL_GetVl_TracDist(ch,MsCAL_THR_HEAVY)-MnTST_PrGet_Value(MnTST_OPT_AUTO_RANGE);
	if(min_x <MnMSR_BaseGet_Ch_Value(ch, MnMS0_OPT_SINGLE_DEADZONE))
		min_x = MnMSR_BaseGet_Ch_Value(ch, MnMS0_OPT_SINGLE_DEADZONE);

	// max_x: Heavy 임계점 + AUTO_RANGE
	// 단, 슬러지 높이(=empty - TracDist) 0.50m(=50) 이하면 Auto Gain 적용 안 함
	{
		U16 trac_heavy  = MsCAL_GetVl_TracDist(ch, MsCAL_THR_HEAVY);
		U16 empty_val   = MnMSR_BaseGet_Ch_Value(ch, MnMS0_OPT_SINGLE_EMPTY);
		U16 sldg_height;
		if (empty_val > trac_heavy)  sldg_height = empty_val - trac_heavy;
		else                          sldg_height = 0;

		if (sldg_height <= 50)
		{
			lMsCal.auto_gain[ch] = MnMSR_CalGet_Ch_Value(ch, MnMS1_OPT_SINGLE_ECHO_AMP);
			lMsCal.auto_gain_cnt[ch] = 0;
			lMsCal.auto_slope_max[ch] = 0;

			if(auto_gain_prof_cnt[ch] == 0)
				auto_gain_prof_cnt[ch] = 1;

			for(j=auto_gain_prof_cnt[ch]-1; j>0; j--)
				lMsCal.auto_gain_buffer[ch][j] =  lMsCal.auto_gain_buffer[ch][j-1];

			lMsCal.auto_gain_buffer[ch][0]= lMsCal.auto_gain[ch];

			for(i=0; i<auto_gain_prof_cnt[ch]; i++)
				sum += lMsCal.auto_gain_buffer[ch][i];
			lMsCal.auto_gain_avg[ch] = sum/auto_gain_prof_cnt[ch];

			return;
		}
		else
			max_x = trac_heavy + MnTST_PrGet_Value(MnTST_OPT_AUTO_RANGE);
	}

	if(max_x >MnMSR_BaseGet_Ch_Value(ch, MnMS0_OPT_SINGLE_EMPTY))
		max_x = MnMSR_BaseGet_Ch_Value(ch, MnMS0_OPT_SINGLE_EMPTY);

	for(j=0;j<19; j++)
	{
		ref_min_volt = 4095;
		ref_max_volt = 0;

		
		for(i=min_x; i<=MsCAL_GetVl_TracDist(ch,MsCAL_THR_HEAVY); i++)
		{
			if(SDRAM_aEco_prof_S3[(((40*ch)+20+j)*ADC_BUFF_MAX)+i]<ref_min_volt)
			{
				ref_min_volt = SDRAM_aEco_prof_S3[(((40*ch)+20+j)*ADC_BUFF_MAX)+i];
				ref_min_x = i;
			}	
		}

		for(i=MsCAL_GetVl_TracDist(ch,MsCAL_THR_HEAVY); i<=max_x; i++)
		{
			if(SDRAM_aEco_prof_S3[(((40*ch)+20+j)*ADC_BUFF_MAX)+i]>ref_max_volt)
			{
				ref_max_volt = SDRAM_aEco_prof_S3[(((40*ch)+20+j)*ADC_BUFF_MAX)+i];
				ref_max_x = i;
			}
		}



		min_volt[j] = ref_min_volt;
		max_volt[j] = ref_max_volt;

		if(max_volt[j]>2000)
		{
			if(max_volt[j]>min_volt[j])
			{
				slope[j] = max_volt[j] - min_volt[j];
				if(slope[j]<=992)
					slope[j] = 0;
			}
			else
				slope[j] = 0;
		}
		else
			slope[j] =0;

#if 0
		if((ref_min_x<MsCAL_GetVl_TracDist(ch,MsCAL_THR_HEAVY)) &&(ref_max_x>MsCAL_GetVl_TracDist(ch,MsCAL_THR_HEAVY)))
		{
			if(max_volt[j]>min_volt[j])
				slope[j] = max_volt[j] - min_volt[j];
			else
				slope[j] = 0;
		}
		else
			slope[j] = 0;
#endif
	}

	j=0;

	for(i=0; i<19; i++)
	{
		if(max_slope<slope[i])
		{
			max_slope = slope[i];
			j=i;
		}
	}

	gain = MnMSR_CalGet_Ch_Value(ch, MnMS1_OPT_SINGLE_ECHO_AMP);


	if((gain + ((j + 1) / 2) * (1 - 2 * (j & 1)))>50)
		lMsCal.auto_gain[ch] = 50;
	else if((gain + ((j + 1) / 2) * (1 - 2 * (j & 1)))<0)
		lMsCal.auto_gain[ch]=1;
	else
		lMsCal.auto_gain[ch] = gain + ((j + 1) / 2) * (1 - 2 * (j & 1));
	

	if(max_slope ==0)
	{
		lMsCal.auto_gain[ch] = MnMSR_CalGet_Ch_Value(ch, MnMS1_OPT_SINGLE_ECHO_AMP);
		lMsCal.auto_gain_cnt[ch] =0;
	}
	else
		lMsCal.auto_gain_cnt[ch] =j;
	
	lMsCal.auto_slope_max[ch] = max_slope;

	for(j=auto_gain_prof_cnt[ch]-1; j>0; j--)
		lMsCal.auto_gain_buffer[ch][j] =  lMsCal.auto_gain_buffer[ch][j-1];

	lMsCal.auto_gain_buffer[ch][0]= lMsCal.auto_gain[ch];

	for(i=0; i<auto_gain_prof_cnt[ch]; i++)
		sum += lMsCal.auto_gain_buffer[ch][i];
	lMsCal.auto_gain_avg[ch] = sum/auto_gain_prof_cnt[ch];
}
#endif



#if 0
		if(MnMSR_CalGet_Ch_Value(ch, MnMS1_OPT_SINGLE_AUTO_FAMP) == MnMS1_AUTO_FAMP_OFF)
			return; 	
	

	if(ApSTT_GetStatus(ch)==ApSTT_MSR_OK)
	{
		S16 before_volt_diff;
		S16 current_volt_diff;
		S16 max_diff;
		S16 min_diff;
		U32 auto_volt_peak = MnTST_PrGet_Value(MnTST_OPT_AUTO_VOLT_PEAK)*1240;
		U32 auto_volt_min = MnTST_PrGet_Value(MnTST_OPT_AUTO_VOLT_MIN)*1240;
		U08 echo_amp;
		U16 before_slope;
		U16 auto_range_x = MnTST_PrGet_Value(MnTST_OPT_AUTO_RANGE);

		U16 auto_volt_diff = MnTST_PrGet_Value(MnTST_OPT_AUTO_VOLT_DIFF);
		U16 slope_sel = (auto_volt_diff*100)/(auto_range_x*2);	
#endif
#if 0
	

		U08 buff[4096] = {0, };
		U16 cnt = 0;
		
		buff[cnt++] = 0x0d; // CR
		buff[cnt++] = 0x0a; // LF

		min_x = MsCAL_GetVl_TracDist(ch,MsCAL_THR_HEAVY)-MnTST_PrGet_Value(MnTST_OPT_AUTO_RANGE);

		if(min_x <MnMSR_BaseGet_Ch_Value(ch, MnMS0_OPT_SINGLE_DEADZONE))
			min_x = MnMSR_BaseGet_Ch_Value(ch, MnMS0_OPT_SINGLE_DEADZONE);
		
		max_x = MsCAL_GetVl_TracDist(ch,MsCAL_THR_HEAVY)+MnTST_PrGet_Value(MnTST_OPT_AUTO_RANGE);

		if(max_x >MnMSR_BaseGet_Ch_Value(ch, MnMS0_OPT_SINGLE_EMPTY))
			max_x = MnMSR_BaseGet_Ch_Value(ch, MnMS0_OPT_SINGLE_EMPTY);


		for(i=min_x; i<=MsCAL_GetVl_TracDist(ch,MsCAL_THR_HEAVY); i++)
		{
			if(MsCAL_GetVl_EchoBuff(ch,i)<min_y)
			{
				min_y = MsCAL_GetVl_EchoBuff(ch,i);
				lMsCal.auto_gain_current_min[ch][0] = i;
			}
		}

		lMsCal.auto_gain_current_min[ch][1] = min_y;


		for(i=MsCAL_GetVl_TracDist(ch,MsCAL_THR_HEAVY); i<=max_x; i++)
		{
			if(MsCAL_GetVl_EchoBuff(ch,i)>max_y)
			{
				max_y = MsCAL_GetVl_EchoBuff(ch,i);
				lMsCal.auto_gain_current_max[ch][0] = i;
			}
		}


		lMsCal.auto_gain_current_max[ch][1] = max_y;


		lMsCal.auto_gain_current_slope[ch] = (lMsCal.auto_gain_current_max[ch][1]-lMsCal.auto_gain_current_min[ch][1])/abs(lMsCal.auto_gain_current_max[ch][0]-lMsCal.auto_gain_current_min[ch][0]);


		//before_volt_diff = lMsCal.auto_gain_before_slope[ch]*lMsCal.auto_gain_before_xrange[ch];
		current_volt_diff = lMsCal.auto_gain_current_slope[ch]*(auto_range_x*2);
		//max_diff = lMsCal.auto_gain_current_max[ch][1] - lMsCal.auto_gain_before_max[ch][1];
		//min_diff = lMsCal.auto_gain_current_min[ch][1] - lMsCal.auto_gain_before_min[ch][1];

		
		if(lMsCal.auto_gain_current_min[ch][1]*100>auto_volt_min)
		{
			if(lMsCal.auto_gain_current_max[ch][1]*100<auto_volt_peak)
			{
				if(++lMsCal.auto_gain_fail_cnt[ch]>50)
				{
					echo_amp = MnMSR_CalGet_Ch_Value(ch,MnMS1_OPT_SINGLE_ECHO_AMP)+1;
				
					if(echo_amp>=50)	echo_amp = 50;
					else if(echo_amp<=1) echo_amp =1;


					buff[cnt++] = 'f';					   // 'A'
					
					buff[cnt++] = (echo_amp / 1000) % 10 + '0';
					buff[cnt++] = (echo_amp / 100) % 10 + '0';
					buff[cnt++] = (echo_amp / 10)	% 10 + '0';
					buff[cnt++] =  echo_amp 	% 10 + '0'; 


					MnMSR_CalSet_Ch_Value(ch, MnMS1_OPT_SINGLE_ECHO_AMP, echo_amp); 		
					lMsCal.auto_gain_fail_cnt[ch] =0;
				}
			}	
			else
			{
				if(++lMsCal.auto_gain_pass_cnt[ch]>50)
				{
					echo_amp = MnMSR_CalGet_Ch_Value(ch,MnMS1_OPT_SINGLE_ECHO_AMP)-1;
					if(echo_amp>=50)	echo_amp = 50;
					else if(echo_amp<=1) echo_amp =1;
					
					MnMSR_CalSet_Ch_Value(ch, MnMS1_OPT_SINGLE_ECHO_AMP, echo_amp);
					lMsCal.auto_gain_pass_cnt[ch] = 0;
				
					buff[cnt++] = 'b';					   // 'A'
					
					buff[cnt++] = (echo_amp / 1000) % 10 + '0';
					buff[cnt++] = (echo_amp / 100) % 10 + '0';
					buff[cnt++] = (echo_amp / 10)	% 10 + '0';
					buff[cnt++] =  echo_amp 	% 10 + '0'; 
				
				}
			}
		}
		else
		{
			if(lMsCal.auto_gain_current_max[ch][1]*100<auto_volt_peak)
			{
				if(++lMsCal.auto_gain_fail_cnt[ch]>50)
				{
					echo_amp = MnMSR_CalGet_Ch_Value(ch,MnMS1_OPT_SINGLE_ECHO_AMP)+1;
				
					if(echo_amp>=50)	echo_amp = 50;
					else if(echo_amp<=1) echo_amp =1;
			
			
					buff[cnt++] = 'f';					   // 'A'
					
					buff[cnt++] = (echo_amp / 1000) % 10 + '0';
					buff[cnt++] = (echo_amp / 100) % 10 + '0';
					buff[cnt++] = (echo_amp / 10)	% 10 + '0';
					buff[cnt++] =  echo_amp 	% 10 + '0'; 
			
			
					MnMSR_CalSet_Ch_Value(ch, MnMS1_OPT_SINGLE_ECHO_AMP, echo_amp); 		
					lMsCal.auto_gain_fail_cnt[ch] =0;
				}
			}	
		}
	}
#endif



#if 0
void MsCAL_ProcRealSave380(U32 idx)
{
	U32 i;
	U32 j;

	RTC_TM curr_tm = RTC_GetTime();

	for(j=idx-1; j>0; j--)
	{
		if(idx<=1)	break;
		for(i=0; i<500; i++)
		{
			SDRAM_aEco_prof_S2[((j)*500)+i] = SDRAM_aEco_prof_S2[((j-1)*500)+i];
		}
	}	

	// Add Data
	for(i=0; i<500; i++)
	{
		SDRAM_aEco_prof_S2[i] = ADC_GetEchoVal(APP_CH_1, i);
		SDRAM_aEco_prof_S2[0] = curr_tm.day;
		SDRAM_aEco_prof_S2[1] = curr_tm.hours;
		SDRAM_aEco_prof_S2[2] = curr_tm.minutes;
	}

}




void MsCAL_ProcRealSave160(U32 idx)
{
	U32 i;
	U32 j;

	RTC_TM curr_tm = RTC_GetTime();

	for(j=idx-1; j>0; j--)
	{
		if(idx<=1)	break;
		for(i=0; i<500; i++)
		{
			SDRAM_aEco_prof_S3[((j)*500)+i] = SDRAM_aEco_prof_S3[((j-1)*500)+i];
		}
	}	

	// Add Data
	for(i=0; i<500; i++)
	{
		SDRAM_aEco_prof_S3[i] = ADC_GetEchoVal(APP_CH_1, i);
		SDRAM_aEco_prof_S3[0] = curr_tm.day;
		SDRAM_aEco_prof_S3[1] = curr_tm.hours;
		SDRAM_aEco_prof_S3[2] = curr_tm.minutes;
	}

}
#endif




void MsCAL_ProcProf(U08 iCh,U32 idx)
{
	U32 i;
	S32 j;
	U64 sum[ADC_BUFF_MAX] = {0, };
	U16 empty_range = MnMSR_BaseGet_Ch_Value(iCh, MnMS0_OPT_SINGLE_EMPTY)*1.1;
	U16 auto_gain_sum;


#if 0
	switch(OuCUR_GetPCD_fRun())
	{
		case PCD_OFF:	break;
		case PCD_ON:	return;
		default:
			return;	
	}
#endif

	

	// Buffer Shift Save
	for(j=idx-1; j>0; j--)
	{
		//if(idx<=1)	break;
		for(i=0; i<empty_range; i++)
		{
			switch(iCh)
			{
				case MsCAL_CH_S0 : SDRAM_aEco_prof_S0[((j)*ADC_BUFF_MAX)+i] = SDRAM_aEco_prof_S0[((j-1)*ADC_BUFF_MAX)+i]; break;
				case MsCAL_CH_S1 : SDRAM_aEco_prof_S1[((j)*ADC_BUFF_MAX)+i] = SDRAM_aEco_prof_S1[((j-1)*ADC_BUFF_MAX)+i]; break;
			}
		}

		DaMdb_ProcMain();
	}
	
	DaMdb_ProcMain();

	if(lMsCal.fauto_gain[iCh]==1)
	{
		// Add Data
		for(i=0; i<empty_range; i++)
		{
			switch(iCh)
			{
				case MsCAL_CH_S0 : SDRAM_aEco_prof_S0[i] = SDRAM_aEco_prof_S3[(((40*iCh)+lMsCal.auto_gain_cnt[iCh])*ADC_BUFF_MAX)+i];	break;
				case MsCAL_CH_S1 : SDRAM_aEco_prof_S1[i] = SDRAM_aEco_prof_S3[(((40*iCh)+lMsCal.auto_gain_cnt[iCh])*ADC_BUFF_MAX)+i];	break;
			}
		}
	}
	else
	{
		// Add Data
		for(i=0; i<empty_range; i++)
		{
			switch(iCh)
			{
				case MsCAL_CH_S0 : SDRAM_aEco_prof_S0[i] = ADC_GetEchoVal(iCh, i); break;
				case MsCAL_CH_S1 : SDRAM_aEco_prof_S1[i] = ADC_GetEchoVal(iCh, i); break;
			}
		}
	}

	// Moving Average
	for(i=0; i<empty_range; i++)
	{
		for(j=0; j<idx; j++)
		{
			switch(iCh)
			{
				case MsCAL_CH_S0 : sum[i] += SDRAM_aEco_prof_S0[(j*ADC_BUFF_MAX)+i]; break;
				case MsCAL_CH_S1 : sum[i] += SDRAM_aEco_prof_S1[(j*ADC_BUFF_MAX)+i]; break;
			}
		}

		DaMdb_ProcMain();

		switch(iCh)
		{
			case MsCAL_CH_S0:	lMsCal.aEco_rslt[MsCAL_CH_S0][i] = sum[i]/idx;	break;
			case MsCAL_CH_S1:	lMsCal.aEco_rslt[MsCAL_CH_S1][i] = sum[i]/idx;	break;
		}
	}



	DaMdb_ProcMain();


#if 0
	I08 sDbg[32];
	U08 a;
	_SPRINTF(sDbg, "%d %d %d %d", ADC_GetEchoVal(MsCAL_CH_S0, 100),lMsCal.aEco_real[MsCAL_CH_S0][100],lMsCal.aEco_rslt[MsCAL_CH_S0][100],sum[100]);
	DpFIG_DrwRect(150, 30, 400, 25, _cBGD_SCRN, DpFIG_FILL);
	DpSTR_GuiLeft(150, 30, _cECO_LVL_TITL, _cBGD_SCRN, _fE17HsB, sDbg);

#endif
}



void MsCAL_CalcSmooth(U08 ch, U08 type)
{
	U32 i,j,k;
	U16 num = MnTST_PrGet_CH_Value(ch,MnTST_OPT_SINGLE_SMOOTH_NO);
	U16 size = MnTST_PrGet_CH_Value(ch,MnTST_OPT_SINGLE_SMOOTH_RANGE);
	U64 sum = 0;
	U16 deadz = MnMSR_BaseGet_Ch_Value(ch, MnMS0_OPT_SINGLE_DEADZONE);
	U16 echo_smooth[MsCAL_CH_NUM][ADC_BUFF_MAX];
	U16 empyt_range = MnMSR_BaseGet_Ch_Value(ch, MnMS0_OPT_SINGLE_EMPTY)*1.1;
	U16 min_x,max_x;

	if(type == 1)
	{
		min_x = MsCAL_GetVl_TracDist(ch,MsCAL_THR_HEAVY)-MnTST_PrGet_Value(MnTST_OPT_AUTO_RANGE);
		if(min_x <MnMSR_BaseGet_Ch_Value(ch, MnMS0_OPT_SINGLE_DEADZONE))
			min_x = MnMSR_BaseGet_Ch_Value(ch, MnMS0_OPT_SINGLE_DEADZONE);	

		max_x = MsCAL_GetVl_TracDist(ch,MsCAL_THR_HEAVY)+MnTST_PrGet_Value(MnTST_OPT_AUTO_RANGE);
		
		if(max_x >MnMSR_BaseGet_Ch_Value(ch, MnMS0_OPT_SINGLE_EMPTY))
			max_x = MnMSR_BaseGet_Ch_Value(ch, MnMS0_OPT_SINGLE_EMPTY);

		if(deadz<min_x-20)
			deadz = min_x-20;
		if(empyt_range>max_x+50)
			empyt_range=max_x+50;
	}

	if((num==0) || (size ==0))
	{
		return;
	}

	for(k=0; k<num;	k++)
	{
		for(i=deadz; i<empyt_range; i++)
		{
			for(j=0; j<=size; j++)
			{
				if(j==0)
				{
					if(type>=1)
					{
						if(k==0)	echo_smooth[ch][i] = SDRAM_aEco_prof_S3[(((40*ch)+lMsCal.auto_calib_cnt)*ADC_BUFF_MAX)+(ADC_BUFF_MAX+i)];
					}
					else
					{
						if(k==0)	echo_smooth[ch][i] = lMsCal.aEco_rslt[ch][i];
					}
					sum = echo_smooth[ch][i]; 
				}
				else
				{
					if(type>=1)
					{
						if(i>=j)						sum+= SDRAM_aEco_prof_S3[(((40*ch)+lMsCal.auto_calib_cnt)*ADC_BUFF_MAX)+i-j];
						if(i+j<empyt_range)				sum+= SDRAM_aEco_prof_S3[(((40*ch)+lMsCal.auto_calib_cnt)*ADC_BUFF_MAX)+i+j];
					}
					else
					{
						if(i>=j)						sum+= lMsCal.aEco_rslt[ch][i-j];	
						if(i+j<empyt_range) 			sum+= lMsCal.aEco_rslt[ch][i+j];		
					}
				}
			}
			if(i<size)								echo_smooth[ch][i] = sum / (size+i+1);
			else if(i+size>=empyt_range)			echo_smooth[ch][i] = sum / (size+i+1);
			else echo_smooth[ch][i] = (sum / ((size*2)+1));
		}
		DaMdb_ProcMain();

	}

	if(type>=1)
	{
#if 0
		for(i=0; i<deadz; i++)
			SDRAM_aEco_prof_S3[(((20*ch)+20+lMsCal.auto_calib_cnt)*ADC_BUFF_MAX)+i]=SDRAM_aEco_prof_S3[(((20*ch)+lMsCal.auto_calib_cnt)*ADC_BUFF_MAX)+i];
#endif
		for(i=deadz; i<empyt_range; i++)
			SDRAM_aEco_prof_S3[(((40*ch)+20+lMsCal.auto_calib_cnt)*ADC_BUFF_MAX)+i]= echo_smooth[ch][i];
#if 0
		for(i=empyt_range; i<ADC_BUFF_MAX; i++)
			SDRAM_aEco_prof_S3[(((20*ch)+20+lMsCal.auto_calib_cnt)*ADC_BUFF_MAX)+i]=SDRAM_aEco_prof_S3[(((20*ch)+lMsCal.auto_calib_cnt)*ADC_BUFF_MAX)+i];
#endif
	}
	else
	{
		for(i=deadz; i<empyt_range; i++)
			lMsCal.aEco_rslt[ch][i] = echo_smooth[ch][i];
	}

}


void MsCAL_ProcSmooth(U08 ch)
{
	if((ScECH_GetLayer() == SCRN_L2_VALU) || (ScCLB_GetLayer() == SCRN_L2_VALU) || (MENU_GetLayer() >= MENU_L3_VALUE)) 
		return;

	switch(MsCAL_Get_Stt(ch))
	{
		case ApSTT_TPR_NG:	
		case ApSTT_MSR_NG:		return;
		case ApSTT_MSR_OK:
		case ApSTT_MSR_TRAC:
		case ApSTT_MSR_IDLE:
		case ApSTT_MSR_WEAK:	break;
		default:				return;
	}


	MsCAL_CalcSmooth(ch,0);
}


void MsCAL_Proc3Stp(U08 iCh)
{
	if((ScECH_GetLayer() == SCRN_L2_VALU) || (ScCLB_GetLayer() == SCRN_L2_VALU) || (MENU_GetLayer() >= MENU_L3_VALUE)) 
		return;

	
	CalPrc_3stp_dist(iCh);

}

void MsCAL_ProcDamp(U08 iCh, U16 idx)
{
	//I08 sDbg[32];
	U16 i,j;
	U64 sum[MsCAL_THR_TYPE_NUM];
	S16 dist[MsCAL_THR_TYPE_NUM];
	U16 empty = MnMSR_BaseGet_Ch_Value(iCh, MnMS0_OPT_SINGLE_EMPTY);
	S16 offs = MnMSR_CalGet_Ch_Value(iCh,MnMS1_OPT_SINGLE_OFFSET);
	if((ScECH_GetLayer() == SCRN_L2_VALU) || (ScCLB_GetLayer() == SCRN_L2_VALU) || (MENU_GetLayer() >= MENU_L3_VALUE)) 
		return;

	// Buffer Shift Save
	for(i=idx-1; i>0; i--)
	{
		for(j=0; j<MsCAL_THR_TYPE_NUM; j++)
			SDRAM_aEco_Damping[(2*MsCAL_DAMP_MAX*iCh)+(MsCAL_DAMP_MAX*j)+i] =  SDRAM_aEco_Damping[(2*MsCAL_DAMP_MAX*iCh)+(MsCAL_DAMP_MAX*j)+i-1];
	}
	

	// Save Step3 Data
	for(j=0; j<MsCAL_THR_TYPE_NUM; j++)
	{
		SDRAM_aEco_Damping[(2*MsCAL_DAMP_MAX*iCh)+(MsCAL_DAMP_MAX*j)] = lMsCal.stp3_dist[iCh][j];
		sum[j] = 0;
	}
	// Sum Data
	for(i=0; i<idx; i++)
	{
		for(j=0; j<MsCAL_THR_TYPE_NUM; j++)
			sum[j] += SDRAM_aEco_Damping[(2*MsCAL_DAMP_MAX*iCh)+(MsCAL_DAMP_MAX*j)+i];
	}
	
	// Offset
	for(j=0; j<MsCAL_THR_TYPE_NUM; j++)
	{
		switch(MnMSR_BaseGet_Ch_Value(iCh, MnMS0_OPT_SINGLE_OPERATION))
		{
			case MnMS0_OPERATION_DISTANCE:	dist[j] = sum[j]/idx + offs;  	break;
			case MnMS0_OPERATION_SLUDGE:	dist[j] = sum[j]/idx - offs;	if(dist[j]<= 0) dist[j] =0;  break;
			default:						break;
		}
	}


	for(j=0; j<MsCAL_THR_TYPE_NUM; j++)
	{
		lMsCal.rslt_dist[iCh][j] = dist[j];

		// Final Result
		if(lMsCal.rslt_dist[iCh][j] > empty)	
		{
			lMsCal.rslt_dist[iCh][j] = empty;
			lMsCal.rslt_sldg[iCh][j] = 0;
		}
		else								
		{
			lMsCal.rslt_sldg[iCh][j] = empty  - lMsCal.rslt_dist[iCh][j];
		}	

		if(lMsCal.rslt_sldg[iCh][j] > empty)	lMsCal.rslt_sldg[iCh][j] = empty;
	}


#if 0
	if(!MnEGN_PrGet_Item(MnEGN_I03_fDP_DBG))
		return;
#endif
#if 0
	_SPRINTF(sDbg, "%d   %d %d", idx, lMsCal.rslt_dist[0][1],lMsCal.stp3_dist[0][1]);
	DpFIG_DrwRect(150, 30, 400, 25, _cBGD_SCRN, DpFIG_FILL);
	DpSTR_GuiLeft(150, 30, _cECO_LVL_TITL, _cBGD_SCRN, _fE17HsB, sDbg);	
#endif
}


#if 0
void MsCAL_Sigmoid(F32 x_start, F32 val, F32 x_end)
{
	S16 i;
	S16 x;
	S16 y;
	F32 w;
	F32 z; 
	S16 L = lMsCal.auto_calib_val_max[lMsCal.auto_calib][1];
	F32 sum_wz = 0;
	F32 sum_w2 = 0;
	U16 count = 0;
	for(i=x_start; i<=x_end; i++)
	{
		x = i;
		y = SDRAM_aEco_prof_S3[(lMsCal.auto_calib*ADC_BUFF_MAX*6)+5*ADC_BUFF_MAX+i];
		w = i-val;

		if(y<=0 || y>= L)
			continue;

		z = log((L / y)-1.0);
		sum_wz += w*z;
		sum_w2 += w*w;
		
		count++;
	}

	lMsCal.auto_calib_slope2[lMsCal.auto_calib] = -(((sum_wz*100)/sum_w2));

}
#endif
void MsCal_AutoGainProf(U08 ch,U08 type)
{
	U16 i;
	U16 empty = MnMSR_BaseGet_Ch_Value(ch, MnMS0_OPT_SINGLE_EMPTY);
	U16 empty_range = empty*1.1;	


	for(i=0; i<empty_range; i++)
	{
		SDRAM_aEco_prof_S3[(lMsCal.auto_calib*ADC_BUFF_MAX*6)+(5*ADC_BUFF_MAX)+i] = 
			(
			SDRAM_aEco_prof_S3[(lMsCal.auto_calib*ADC_BUFF_MAX*6)+(4*ADC_BUFF_MAX)+i]+
			SDRAM_aEco_prof_S3[(lMsCal.auto_calib*ADC_BUFF_MAX*6)+(3*ADC_BUFF_MAX)+i]+
			SDRAM_aEco_prof_S3[(lMsCal.auto_calib*ADC_BUFF_MAX*6)+(2*ADC_BUFF_MAX)+i]+
			SDRAM_aEco_prof_S3[(lMsCal.auto_calib*ADC_BUFF_MAX*6)+(1*ADC_BUFF_MAX)+i]+
			SDRAM_aEco_prof_S3[(lMsCal.auto_calib*ADC_BUFF_MAX*6)+(0*ADC_BUFF_MAX)+i])/5;

		//lMsCal.aEco_real[ch][i] =SDRAM_aEco_prof_S3[(lMsCal.auto_calib*ADC_BUFF_MAX*6)+5*ADC_BUFF_MAX+i];
	}
	
	MsCAL_CalcSmooth(ch,type);


}




void MsCAL_AutoGainINF(U08 ch,U16 cnt,S16 amp)
{

	U16 val = MnMSR_CalGet_Ch_Value(ch, MnMS1_OPT_SINGLE_AUTO_CAL);



	U16 auto_range_x = MnTST_PrGet_Value(MnTST_OPT_AUTO_RANGE);
	U16 auto_volt_diff = MnTST_PrGet_Value(MnTST_OPT_AUTO_VOLT_DIFF);
	U16 slope_sel = (auto_volt_diff*100)/(auto_range_x*2);
	U32 auto_volt_peak = MnTST_PrGet_Value(MnTST_OPT_AUTO_VOLT_PEAK)*1240;
	U16 value_range =MnTST_PrGet_Value(MnTST_OPT_AUTO_LEVEL_RANGE);

	val = MsCAL_GetVl_TracDist(ch, MsCAL_THR_HEAVY);
	



	if(lMsCal.auto_calib_val_max[amp][1]*100>auto_volt_peak)
	{
		if(lMsCal.auto_slope_max[ch] <=lMsCal.auto_calib_slope[amp])
		{
			lMsCal.auto_slope_max[ch] = lMsCal.auto_calib_slope[amp];
			lMsCal.auto_calib_result_thr = lMsCal.auto_calib_thr[amp];
			lMsCal.auto_calib_result_val = val - (value_range)+cnt;
			lMsCal.auto_calib_result_count = cnt;
			lMsCal.auto_calib_result_echo_amp = amp;

			lMsCal.auto_calib_result_thr_max[1] = lMsCal.auto_calib_thr_max[amp][1];
			lMsCal.auto_calib_result_thr_max[0] =lMsCal.auto_calib_thr_max[amp][0];
			lMsCal.auto_calib_result_thr_min[1] =lMsCal.auto_calib_thr_min[amp][1];
			lMsCal.auto_calib_result_thr_min[0] =lMsCal.auto_calib_thr_min[amp][0];
			lMsCal.auto_calib_result_val_max[1] =lMsCal.auto_calib_val_max[amp][1];
			lMsCal.auto_calib_result_val_max[0] =lMsCal.auto_calib_val_max[amp][0];
			lMsCal.auto_calib_result_val_min[1] =lMsCal.auto_calib_val_min[amp][1];
			lMsCal.auto_calib_result_val_min[0] =lMsCal.auto_calib_val_min[amp][0];
			lMsCal.auto_calib_result_volt = lMsCal.auto_calib_val_volt[amp];
			lMsCal.auto_cal_sel_cnt++;
		}
	}




#if 0
	I08 sDbg[128];


 
	_SPRINTF(sDbg, "%d %d %d %d",lMsCal.auto_calib_result_thr,lMsCal.auto_calib_result_val,lMsCal.auto_calib_result_echo_amp,val_final);
	DpFIG_DrwRect(150, 120, 400, 25, _cBGD_SCRN, DpFIG_FILL);
	DpSTR_GuiLeft(150, 120, _cECO_LVL_TITL, _cBGD_SCRN, _fE17HsB, sDbg);		
#endif


	//lMsCal.auto_calib = val;
	//ApRun_ProcMode();

}	




void MsCAL_AutoCalibINF(U08 ch,U16 cnt,S16 amp,U08 m)
{
	
	U16 val = MnMSR_CalGet_Ch_Value(ch, MnMS1_OPT_SINGLE_AUTO_CAL);

	U08 thr_min = MnTST_PrGet_Value(MnTST_OPT_AUTO_THR_MIN);
	U08 thr_max = MnTST_PrGet_Value(MnTST_OPT_AUTO_THR_MAX);

	U16 auto_range_x = MnTST_PrGet_Value(MnTST_OPT_AUTO_RANGE);
	U16 auto_volt_diff = MnTST_PrGet_Value(MnTST_OPT_AUTO_VOLT_DIFF);
	U16 slope_sel = (auto_volt_diff*100)/(auto_range_x*2);
	U32 auto_volt_peak = MnTST_PrGet_Value(MnTST_OPT_AUTO_VOLT_PEAK)*1240;
	U16 value_range =MnTST_PrGet_Value(MnTST_OPT_AUTO_LEVEL_RANGE);

#if 1
	if((m==0) || (m==2))
	{
		thr_min=50;
		thr_max=50;
	}
#endif

	if(lMsCal.auto_calib_val_max[amp][1]*100>auto_volt_peak)
	{
		if((lMsCal.auto_calib_thr[amp]<=thr_max) && (lMsCal.auto_calib_thr[amp]>=thr_min))
		{
			if((lMsCal.auto_calib_slope[amp]*8.05)>slope_sel)
			{
				if(lMsCal.auto_slope_max[ch] <=lMsCal.auto_calib_slope[amp])
				{

					lMsCal.auto_slope_max[ch] = lMsCal.auto_calib_slope[amp];
					lMsCal.auto_calib_result_thr = lMsCal.auto_calib_thr[amp];
					lMsCal.auto_calib_result_val = val - (value_range)+cnt;
					lMsCal.auto_calib_result_count = cnt;
					lMsCal.auto_calib_result_echo_amp = amp;

					lMsCal.auto_calib_result_thr_max[1] = lMsCal.auto_calib_thr_max[amp][1];
					lMsCal.auto_calib_result_thr_max[0] =lMsCal.auto_calib_thr_max[amp][0];
					lMsCal.auto_calib_result_thr_min[1] =lMsCal.auto_calib_thr_min[amp][1];
					lMsCal.auto_calib_result_thr_min[0] =lMsCal.auto_calib_thr_min[amp][0];
					lMsCal.auto_calib_result_val_max[1] =lMsCal.auto_calib_val_max[amp][1];
					lMsCal.auto_calib_result_val_max[0] =lMsCal.auto_calib_val_max[amp][0];
					lMsCal.auto_calib_result_val_min[1] =lMsCal.auto_calib_val_min[amp][1];
					lMsCal.auto_calib_result_val_min[0] =lMsCal.auto_calib_val_min[amp][0];
					lMsCal.auto_calib_result_volt = lMsCal.auto_calib_val_volt[amp];
					lMsCal.auto_cal_sel_cnt++;
				}
			}
		}
	}



#if 0
	I08 sDbg[128];


 
	_SPRINTF(sDbg, "%d %d %d %d",lMsCal.auto_calib_result_thr,lMsCal.auto_calib_result_val,lMsCal.auto_calib_result_echo_amp,val_final);
	DpFIG_DrwRect(150, 120, 400, 25, _cBGD_SCRN, DpFIG_FILL);
	DpSTR_GuiLeft(150, 120, _cECO_LVL_TITL, _cBGD_SCRN, _fE17HsB, sDbg);		
#endif


	//lMsCal.auto_calib = val;
	//ApRun_ProcMode();

}		



#if 0
void MsCAL_AutoCalibBuffer(U08 ch)
{
	U16 i,j,k;
	U16 min_y = 0xffff;
	U16 max_y = 0;
	U16 max_x = 0;
	U16 min_x = 0;
	S16 slope;
	U16 max_range= MnTST_PrGet_CH_Value(ch, MnTST_OPT_SINGLE_THR_RANGE);
	U16 empty = MnMSR_BaseGet_Ch_Value(ch, MnMS0_OPT_SINGLE_EMPTY);
	U16 empty_range = empty*1.1;
	U16 dead = MnMSR_BaseGet_Ch_Value(ch, MnMS0_OPT_SINGLE_DEADZONE);
	U16 x_end;
	U16 x_start;
	U16 thr_range;
	U16 thr_min_y = 0xffff;
	U16 thr_max_y = 0;
	U16 thr_min_x = 0;
	U16 thr_max_x = 0;

	U16 val_volt[2];
	U16 val_thr[2];
	U16 thr;
	S16 val;
	S16 val2;
	S16 sel;

	U16 auto_range_x = MnTST_PrGet_Value(MnTST_OPT_AUTO_RANGE);

	if(MnMSR_CalGet_Ch_Value(ch, MnMS1_OPT_SINGLE_AUTO_FAMP) == MnMS1_AUTO_FAMP_ON)
		sel = MsCAL_GetVl_RsltSldg(ch, MsCAL_THR_HEAVY);
	else
		sel  = MnMSR_CalGet_Ch_Value(ch, MnMS1_OPT_SINGLE_AUTO_CAL);

	U16 value_range =100;



	for(k=1; k<51; k++)
	{
	
		val2 = sel-(value_range/2)+lMsCal.auto_calib_result_count;

		if(val2>=(empty-dead))
			val = empty-dead;
		else
			val = empty -val2;

		if(val2<=0)
			val = 0;
		else
			val = empty-val2;


		//val = TPRT_Calib_Reverse(ch,val);

		if((val-auto_range_x) <= dead)
			x_start = dead;
		else
			x_start = val-auto_range_x;
		
		if((val+auto_range_x) >= empty)
			x_end = empty;
		else
			x_end = val+auto_range_x;

#if 1
		if((val+max_range) >= empty)
			thr_range = empty;
		else
			thr_range = val+max_range; 
#endif	
		

		for(i=dead; i<empty_range; i++)
		{
			#if 0
			if(j==0)
				lMsCal.aEco_real[ch][i] =SDRAM_aEco_prof_S3[(lMsCal.auto_calib*ADC_BUFF_MAX*6)+5*ADC_BUFF_MAX+i];
			#endif
			if(i==val)
			{
				#if 0
				val_volt[lMsCal.auto_calib][j] = 
				((SDRAM_aEco_prof_S3[(lMsCal.auto_calib*ADC_BUFF_MAX*6)+5*ADC_BUFF_MAX+i]+SDRAM_aEco_prof_S3[(lMsCal.auto_calib*ADC_BUFF_MAX*6)+5*ADC_BUFF_MAX+i-1])/2)-5;
				#endif
				val_volt[1] = SDRAM_aEco_prof_S3[(k*ADC_BUFF_MAX*6)+5*ADC_BUFF_MAX+i];
				val_volt[0] = SDRAM_aEco_prof_S3[(k*ADC_BUFF_MAX*6)+5*ADC_BUFF_MAX+i-1];
				lMsCal.auto_calib_val_volt[k] = val_volt[1];
			}

			if(i==x_start)
			{
				if(SDRAM_aEco_prof_S3[(k*ADC_BUFF_MAX*6)+5*ADC_BUFF_MAX+i]<min_y)
				{
					min_y = SDRAM_aEco_prof_S3[(k*ADC_BUFF_MAX*6)+5*ADC_BUFF_MAX+i];
					min_x = i;
				}
			}
			else if(i<=x_end)
			{	
				if(SDRAM_aEco_prof_S3[(lMsCal.auto_calib*ADC_BUFF_MAX*6)+5*ADC_BUFF_MAX+i]>=max_y)
				{
					max_y = SDRAM_aEco_prof_S3[(k*ADC_BUFF_MAX*6)+5*ADC_BUFF_MAX+i];
					max_x = i;
				}
			}

			if(i<=val)
			{
				if(SDRAM_aEco_prof_S3[(k*ADC_BUFF_MAX*6)+5*ADC_BUFF_MAX+i]<thr_min_y)
				{
					thr_min_y =  SDRAM_aEco_prof_S3[(k*ADC_BUFF_MAX*6)+5*ADC_BUFF_MAX+i];
					thr_min_x = i;				
				}
			}
			else if(i<thr_range)
			{
				if(SDRAM_aEco_prof_S3[(k*ADC_BUFF_MAX*6)+5*ADC_BUFF_MAX+i]>thr_max_y)
				{
					thr_max_y = SDRAM_aEco_prof_S3[(k*ADC_BUFF_MAX*6)+5*ADC_BUFF_MAX+i];
					thr_max_x = i;
				}	
			}
			
			if(thr_max_y<=thr_min_y)
				thr_min_y = thr_max_y;

		}
		
		if(min_y == 0xffff || max_y == 0 || max_x ==0 || min_x ==0)
		{
			lMsCal.auto_calib_slope[k] = 0;
			lMsCal.auto_calib_thr[k] = 0;
		}
		else
		{
			lMsCal.auto_calib_slope[k] = (max_y- min_y) / (max_x-min_x);		

			val_thr[1] = ((val_volt[1] - thr_min_y)*100) / (thr_max_y-thr_min_y);
			val_thr[0] = ((val_volt[0] - thr_min_y)*100) / (thr_max_y-thr_min_y);

			if(val_thr[1]>val_thr[0])
			{
				if(val_thr[0]>50)
					lMsCal.auto_calib_thr[k] = val_thr[0];
				else if(val_thr[1]<50)
					lMsCal.auto_calib_thr[k] = val_thr[1];
				else
					lMsCal.auto_calib_thr[k] = 50;
					
			}
			else
			{
				if(val_thr[1]>50)
					lMsCal.auto_calib_thr[k] = val_thr[1];
				else if(val_thr[0]<50)
					lMsCal.auto_calib_thr[k] = val_thr[0];
				else
					lMsCal.auto_calib_thr[k] = 50;
			}
			//lMsCal.auto_calib_thr[lMsCal.auto_calib][j] = ((val_volt[lMsCal.auto_calib][j] - thr_min_y)*100) / (thr_max_y-thr_min_y);

		}

		lMsCal.auto_calib_thr_max[k][1] = thr_max_y;
		lMsCal.auto_calib_thr_max[k][0] = thr_max_x;
		lMsCal.auto_calib_thr_min[k][1]= lMsCal.auto_calib_result_count;
		lMsCal.auto_calib_thr_min[k][0] = val;
		lMsCal.auto_calib_val_max[k][1]= max_y;
		lMsCal.auto_calib_val_max[k][0] = max_x;
		lMsCal.auto_calib_val_min[k][1] = min_y;
		lMsCal.auto_calib_val_min[k][0] = min_x;		

		min_y = 0xffff;
		max_y = 0;
		max_x = 0;
		min_x = 0;
		thr_min_y = 0xffff;
		thr_max_y = 0;
		thr_max_x = 0;
		thr_min_x = 0;

	}






#if 1

#endif	

}
#endif


#if 0
void MsCAL_AutoGainResult(U08 ch)
{
	U16 i,j,k;
	U16 min_y = 0xffff;
	U16 max_y = 0;
	U16 max_x = 0;
	U16 min_x = 0;
	U16 max_range= MnTST_PrGet_CH_Value(ch, MnTST_OPT_SINGLE_THR_RANGE);
	U16 empty = MnMSR_BaseGet_Ch_Value(ch, MnMS0_OPT_SINGLE_EMPTY);
	U16 empty_range = empty*1.1;
	U16 dead = MnMSR_BaseGet_Ch_Value(ch, MnMS0_OPT_SINGLE_DEADZONE);
	U16 x_end;
	U16 x_start;
	U16 thr_range;
	U32 thr_min_y = 0xffff;
	U32 thr_max_y = 0;
	U32 thr_min_x = 0;
	U32 thr_max_x = 0;

	U16 val_volt[201][2];
	U16 val_thr[2];
	S16 val;
	S16 val2;
	S16 sel;
	U16 before_volt_diff;
	U16 final_volt_diff;
	U16 auto_range_x = MnTST_PrGet_Value(MnTST_OPT_AUTO_RANGE);
	S16 t;
	S16 current_slope;
	S16 current_slope_max;
	S16 current_slope_min;
	U32 auto_volt_peak = MnTST_PrGet_Value(MnTST_OPT_AUTO_VOLT_PEAK)*1240;

	U16 auto_volt_diff = MnTST_PrGet_Value(MnTST_OPT_AUTO_VOLT_DIFF);
	U16 slope_sel = (auto_volt_diff*100)/(auto_range_x*2);

	sel  = MsCAL_GetVl_TracDist(ch, MsCAL_THR_HEAVY);

	U16 value_range =MnTST_PrGet_Value(MnTST_OPT_AUTO_LEVEL_RANGE);


	lMsCal.auto_cal_sel_cnt = 0;
	lMsCal.auto_slope_max = 0;
	lMsCal.auto_slope_min = 0xffff;

	U08 buff[10000] = {0, };
	U16 cnt = 0;

	buff[cnt++] = 0x0d;	// CR
	buff[cnt++] = 0x0a;	// LF


	for(j=0; j<(value_range*2)+1; j++)
	{	
		t = (value_range)+((j % 2 == 0) ? (j / 2) : -(j + 1) / 2);

		if(j!=0)
			continue;
		
		for(k=1; k<8; k++)
		{		

			min_y = 0xffff;
			max_y = 0;
			max_x = 0;
			min_x = 0;
			thr_min_y = 0xffff;
			thr_max_y = 0;
			thr_max_x = 0;
			thr_min_x = 0;
#if 1
			if(k==1)
				lMsCal.auto_calib = MnMSR_CalGet_Ch_Value(ch, MnMS1_OPT_SINGLE_ECHO_AMP);
			else if(k==2)
				lMsCal.auto_calib = MnMSR_CalGet_Ch_Value(ch, MnMS1_OPT_SINGLE_ECHO_AMP)-1;
			else if(k==3)
				lMsCal.auto_calib = MnMSR_CalGet_Ch_Value(ch, MnMS1_OPT_SINGLE_ECHO_AMP)+1;
			else if(k==4)
				lMsCal.auto_calib = MnMSR_CalGet_Ch_Value(ch, MnMS1_OPT_SINGLE_ECHO_AMP)-2;
			else if(k==5)
				lMsCal.auto_calib = MnMSR_CalGet_Ch_Value(ch, MnMS1_OPT_SINGLE_ECHO_AMP)+2;
			else if(k==6)
				lMsCal.auto_calib = MnMSR_CalGet_Ch_Value(ch, MnMS1_OPT_SINGLE_ECHO_AMP)-3;
			else if(k==7)
				lMsCal.auto_calib = MnMSR_CalGet_Ch_Value(ch, MnMS1_OPT_SINGLE_ECHO_AMP)+3;
#endif

			if(lMsCal.auto_calib<=1)
				lMsCal.auto_calib =1;
			else if (lMsCal.auto_calib>=50)
				lMsCal.auto_calib =50;



			val2 = sel-(value_range)+t;

			if(val2>=empty)
				val = empty;
			else
				val = val2;

			if(val2<=dead)
				val = dead;
			else
				val = val2;

			//val = TPRT_Calib_Reverse(ch,val);

			if((val-auto_range_x) <= dead)
				x_start = dead;
			else
				x_start = val-auto_range_x;
			
			if((val+auto_range_x) >= empty)
				x_end = empty;
			else
				x_end = val+auto_range_x;

#if 1
			if((val+max_range) >= empty)
				thr_range = empty_range;
			else
				thr_range = val+max_range; 
#endif	
			

			for(i=dead; i<empty_range; i++)
			{
				#if 0
				if(j==0)
					lMsCal.aEco_real[ch][i] =SDRAM_aEco_prof_S3[(lMsCal.auto_calib*ADC_BUFF_MAX*6)+5*ADC_BUFF_MAX+i];
				#endif
				if(i==val)
				{
					#if 0
					val_volt[lMsCal.auto_calib][j] = 
					((SDRAM_aEco_prof_S3[(lMsCal.auto_calib*ADC_BUFF_MAX*6)+5*ADC_BUFF_MAX+i]+SDRAM_aEco_prof_S3[(lMsCal.auto_calib*ADC_BUFF_MAX*6)+5*ADC_BUFF_MAX+i-1])/2)-5;
					#endif
					val_volt[j][1] = SDRAM_aEco_prof_S3[(lMsCal.auto_calib*ADC_BUFF_MAX*6)+5*ADC_BUFF_MAX+i];
					val_volt[j][0] = SDRAM_aEco_prof_S3[(lMsCal.auto_calib*ADC_BUFF_MAX*6)+5*ADC_BUFF_MAX+i-1];
					lMsCal.auto_calib_val_volt[lMsCal.auto_calib] = val_volt[j][1];
				}

				if(i==x_start)
				{
					if(SDRAM_aEco_prof_S3[(lMsCal.auto_calib*ADC_BUFF_MAX*6)+5*ADC_BUFF_MAX+i]<min_y)
					{
						min_y = SDRAM_aEco_prof_S3[(lMsCal.auto_calib*ADC_BUFF_MAX*6)+5*ADC_BUFF_MAX+i];
						min_x = i;
					}
				}
				else if(i==x_end)
				{	
					if(SDRAM_aEco_prof_S3[(lMsCal.auto_calib*ADC_BUFF_MAX*6)+5*ADC_BUFF_MAX+i]>=max_y)
					{
						max_y = SDRAM_aEco_prof_S3[(lMsCal.auto_calib*ADC_BUFF_MAX*6)+5*ADC_BUFF_MAX+i];
						max_x = i;
					}
				}

				if(i<=val)
				{
					if(SDRAM_aEco_prof_S3[(lMsCal.auto_calib*ADC_BUFF_MAX*6)+5*ADC_BUFF_MAX+i]<thr_min_y)
					{
						thr_min_y =  SDRAM_aEco_prof_S3[(lMsCal.auto_calib*ADC_BUFF_MAX*6)+5*ADC_BUFF_MAX+i];
						thr_min_x = i;				
					}
				}
				else if(i<thr_range)		
				{
					if(SDRAM_aEco_prof_S3[(lMsCal.auto_calib*ADC_BUFF_MAX*6)+5*ADC_BUFF_MAX+i]>=thr_max_y)
					{
						thr_max_y = SDRAM_aEco_prof_S3[(lMsCal.auto_calib*ADC_BUFF_MAX*6)+5*ADC_BUFF_MAX+i];
						thr_max_x = i;
					}	
				}
				if(thr_max_y<=thr_min_y)
					thr_min_y = thr_max_y;

			}
			
			if(min_y == 0xffff || max_y == 0 || max_x ==0 || min_x ==0)
			{
				lMsCal.auto_calib_slope[lMsCal.auto_calib] = 0;
				lMsCal.auto_calib_thr[lMsCal.auto_calib] = 0;
			}
			else
			{
				if(max_y>=min_y)
					lMsCal.auto_calib_slope[lMsCal.auto_calib] = (max_y- min_y) / (max_x-min_x);		
				else
					lMsCal.auto_calib_slope[lMsCal.auto_calib] = 0;
				val_thr[1] = ((val_volt[j][1] - thr_min_y)*100) / (thr_max_y-thr_min_y);
				val_thr[0] = ((val_volt[j][0] - thr_min_y)*100) / (thr_max_y-thr_min_y);

#if 1
				if(val_thr[1]>val_thr[0])
				{
					if(val_thr[0]>MnMSR_Get_Threshold_Ch_Value(ch, MnMS1_OPT_SINGLE_THR_HEAVY))
						lMsCal.auto_calib_thr[lMsCal.auto_calib] = val_thr[0];
					else if(val_thr[1]<MnMSR_Get_Threshold_Ch_Value(ch, MnMS1_OPT_SINGLE_THR_HEAVY))
						lMsCal.auto_calib_thr[lMsCal.auto_calib] = val_thr[1];
					else
						lMsCal.auto_calib_thr[lMsCal.auto_calib] = MnMSR_Get_Threshold_Ch_Value(ch, MnMS1_OPT_SINGLE_THR_HEAVY);
						
				}
				else
				{
					if(val_thr[1]>MnMSR_Get_Threshold_Ch_Value(ch, MnMS1_OPT_SINGLE_THR_HEAVY))
						lMsCal.auto_calib_thr[lMsCal.auto_calib] = val_thr[1];
					else if(val_thr[0]<MnMSR_Get_Threshold_Ch_Value(ch, MnMS1_OPT_SINGLE_THR_HEAVY))
						lMsCal.auto_calib_thr[lMsCal.auto_calib] = val_thr[0];
					else
						lMsCal.auto_calib_thr[lMsCal.auto_calib] = MnMSR_Get_Threshold_Ch_Value(ch, MnMS1_OPT_SINGLE_THR_HEAVY);
				}
				//lMsCal.auto_calib_thr[lMsCal.auto_calib][j] = ((val_volt[lMsCal.auto_calib][j] - thr_min_y)*100) / (thr_max_y-thr_min_y);
#endif
			}

			lMsCal.auto_calib_thr_max[lMsCal.auto_calib][1] = thr_max_y;
			lMsCal.auto_calib_thr_max[lMsCal.auto_calib][0] = thr_max_x;
			lMsCal.auto_calib_thr_min[lMsCal.auto_calib][1]= thr_min_y;
			lMsCal.auto_calib_thr_min[lMsCal.auto_calib][0] = thr_min_x;
			lMsCal.auto_calib_val_max[lMsCal.auto_calib][1]= max_y;
			lMsCal.auto_calib_val_max[lMsCal.auto_calib][0] = max_x;
			lMsCal.auto_calib_val_min[lMsCal.auto_calib][1] = min_y;
			lMsCal.auto_calib_val_min[lMsCal.auto_calib][0] = min_x;		



			MsCAL_AutoGainINF(ch,t,lMsCal.auto_calib);


			min_y = 0xffff;
			max_y = 0;
			max_x = 0;
			min_x = 0;
			thr_min_y = 0xffff;
			thr_max_y = 0;
			thr_max_x = 0;
			thr_min_x = 0;

#if 1
			buff[cnt++] = 0x0d; // CR
			buff[cnt++] = 0x0a; // LF

			
			buff[cnt++] = (val / 1000) % 10 + '0';
			buff[cnt++] = (val / 100)	% 10 + '0';
			buff[cnt++] = (val / 10)	% 10 + '0';
			buff[cnt++] =  val 		% 10 + '0';


			buff[cnt++] = '/';					   // 'A'
			
			buff[cnt++] = (lMsCal.auto_calib / 1000) % 10 + '0';
			buff[cnt++] = (lMsCal.auto_calib / 100)	% 10 + '0';
			buff[cnt++] = (lMsCal.auto_calib / 10)	% 10 + '0';
			buff[cnt++] =  lMsCal.auto_calib 		% 10 + '0';


			buff[cnt++] = '-';					   // 'A'
			
			buff[cnt++] = (lMsCal.auto_calib_slope[lMsCal.auto_calib] / 1000) % 10 + '0';
			buff[cnt++] = (lMsCal.auto_calib_slope[lMsCal.auto_calib] / 100)	% 10 + '0';
			buff[cnt++] = (lMsCal.auto_calib_slope[lMsCal.auto_calib] / 10)	% 10 + '0';
			buff[cnt++] =  lMsCal.auto_calib_slope[lMsCal.auto_calib] 		% 10 + '0';

			buff[cnt++] = '-';					   // 'A'
			
			buff[cnt++] = (lMsCal.auto_calib_thr[lMsCal.auto_calib] / 1000) % 10 + '0';
			buff[cnt++] = (lMsCal.auto_calib_thr[lMsCal.auto_calib] / 100)	% 10 + '0';
			buff[cnt++] = (lMsCal.auto_calib_thr[lMsCal.auto_calib] / 10)	% 10 + '0';
			buff[cnt++] = lMsCal.auto_calib_thr[lMsCal.auto_calib] 		% 10 + '0';

			buff[cnt++] = '-';					   // 'A'

			buff[cnt++] = (lMsCal.auto_calib_val_max[lMsCal.auto_calib][1] / 1000) % 10 + '0';
			buff[cnt++] = (lMsCal.auto_calib_val_max[lMsCal.auto_calib][1] / 100)	% 10 + '0';
			buff[cnt++] = (lMsCal.auto_calib_val_max[lMsCal.auto_calib][1] / 10) % 10 + '0';
			buff[cnt++] =  lMsCal.auto_calib_val_max[lMsCal.auto_calib][1]		% 10 + '0';

			buff[cnt++] = '-';					   // 'A'

			buff[cnt++] = (val_volt[j][1] / 1000) % 10 + '0';
			buff[cnt++] = (val_volt[j][1] / 100)	% 10 + '0';
			buff[cnt++] = (val_volt[j][1] / 10)	% 10 + '0';
			buff[cnt++] =  val_volt[j][1] 		% 10 + '0';

			buff[cnt++] = '-';					   // 'A'
			
			buff[cnt++] = (val_volt[j][0] / 1000) % 10 + '0';
			buff[cnt++] = (val_volt[j][0] / 100)	% 10 + '0';
			buff[cnt++] = (val_volt[j][0] / 10) % 10 + '0';
			buff[cnt++] =  val_volt[j][0]		% 10 + '0';
#endif

		}

	}


	

#if 1

	if(lMsCal.auto_cal_sel_cnt>0)
	{
		lMsCal.fauto_calib = 0;
		lMsCal.auto_calib_result_slope = lMsCal.auto_slope_max;

		before_volt_diff = lMsCal.auto_gain_before_slope[ch]*(auto_range_x*2);
		final_volt_diff = lMsCal.auto_gain_current_slope[ch]*(auto_range_x*2);

		buff[cnt++] = 0x0d; // CR
		buff[cnt++] = 0x0a; // LF
		
		buff[cnt++] = (before_volt_diff / 1000) % 10 + '0';
		buff[cnt++] = (before_volt_diff / 100) % 10 + '0';
		buff[cnt++] = (before_volt_diff / 10)	% 10 + '0';
		buff[cnt++] =  before_volt_diff		% 10 + '0';

		buff[cnt++] = '/';					   // 'A'

		buff[cnt++] = (final_volt_diff / 1000) % 10 + '0';
		buff[cnt++] = (final_volt_diff / 100) % 10 + '0';
		buff[cnt++] = (final_volt_diff / 10)	% 10 + '0';
		buff[cnt++] =  final_volt_diff		% 10 + '0';
		buff[cnt++] = '/';					   // 'A'

		buff[cnt++] = ((final_volt_diff-before_volt_diff) / 1000) % 10 + '0';
		buff[cnt++] = ((final_volt_diff-before_volt_diff)/ 100) % 10 + '0';
		buff[cnt++] = ((final_volt_diff-before_volt_diff) / 10)	% 10 + '0';
		buff[cnt++] = (final_volt_diff-before_volt_diff)		% 10 + '0';



		if(abs(final_volt_diff-before_volt_diff)>=125)
		{
			buff[cnt++] = '/';					   // 'A'
			buff[cnt++] = (lMsCal.auto_calib_result_echo_amp / 1000) % 10 + '0';
			buff[cnt++] = (lMsCal.auto_calib_result_echo_amp / 100) % 10 + '0';
			buff[cnt++] = (lMsCal.auto_calib_result_echo_amp / 10)	% 10 + '0';
			buff[cnt++] =  lMsCal.auto_calib_result_echo_amp		% 10 + '0';		
			buff[cnt++] = '/';					   // 'A'
			buff[cnt++] = (lMsCal.auto_calib_result_thr/ 1000) % 10 + '0';
			buff[cnt++] = (lMsCal.auto_calib_result_thr / 100) % 10 + '0';
			buff[cnt++] = (lMsCal.auto_calib_result_thr / 10)	% 10 + '0';
			buff[cnt++] =  lMsCal.auto_calib_result_thr		% 10 + '0';		

			buff[cnt++] = '/';					   // 'A'
			buff[cnt++] = ( lMsCal.auto_calib_result_val/ 1000) % 10 + '0';
			buff[cnt++] = (lMsCal.auto_calib_result_val / 100) % 10 + '0';
			buff[cnt++] = (lMsCal.auto_calib_result_val / 10)	% 10 + '0';
			buff[cnt++] =  lMsCal.auto_calib_result_val		% 10 + '0';						
			if(++lMsCal.auto_gain_pass_cnt[ch]>30)
			{
				buff[cnt++] = '/';	

				if(lMsCal.auto_calib_result_echo_amp>=50)
					lMsCal.auto_calib_result_echo_amp = 50;
				else if(lMsCal.auto_calib_result_echo_amp<=1)
					lMsCal.auto_calib_result_echo_amp = 1;

				MnMSR_CalSet_Ch_Value(ch, MnMS1_OPT_SINGLE_ECHO_AMP, lMsCal.auto_calib_result_echo_amp );
				MnMSR_CalSet_Ch_Value(ch, MnMS1_OPT_SINGLE_THR_HEAVY,MnMS1_THRESHOLD_AUTO);
				MnMSR_CalSet_Ch_Value(ch, MnMS1_OPT_SINGLE_THR_LIGHT,MnMS1_THRESHOLD_AUTO);
				//MnMSR_Set_Threshold_Ch_Value(ch, MnMS1_OPT_SINGLE_THR_LIGHT,lMsCal.auto_calib_result_thr);
				//MnMSR_Set_Threshold_Ch_Value(ch, MnMS1_OPT_SINGLE_THR_HEAVY,lMsCal.auto_calib_result_thr);
				lMsCal.auto_gain_pass_cnt[ch] = 0;
			}
		}
		else
			lMsCal.auto_gain_pass_cnt[ch] = 0;
		lMsCal.auto_slope_max = 0;
		lMsCal.auto_gain_fail_cnt[ch] = 0;
	}
	else
	{
		if(++lMsCal.auto_gain_fail_cnt[ch]>30)
		{
			lMsCal.auto_calib_result_echo_amp = MnMSR_CalGet_Ch_Value(ch,MnMS1_OPT_SINGLE_ECHO_AMP)+1;
			if(lMsCal.auto_calib_result_echo_amp>=50)
				lMsCal.auto_calib_result_echo_amp = 50;
			else if(lMsCal.auto_calib_result_echo_amp<=1)
				lMsCal.auto_calib_result_echo_amp = 1;
			
			MnMSR_CalSet_Ch_Value(ch, MnMS1_OPT_SINGLE_ECHO_AMP, lMsCal.auto_calib_result_echo_amp);
			lMsCal.auto_gain_fail_cnt[ch] = 0;
		}
		//lMsCal.fauto_calib = 2;
		//lMsCal.auto_calib_fresult = 1;
		//lMsCal.auto_calib_result_count = 50;
	}

#endif

#if 0
	buff[cnt++] = 0x0d; // CR
	buff[cnt++] = 0x0a; // LF
	buff[cnt++] = '/';					   // 'A'
	buff[cnt++] =  lMsCal.fauto_calib         % 10 + '0';

	buff[cnt++] = '/';					   // 'A'

	buff[cnt++] = (lMsCal.auto_calib_result_echo_amp / 10)   % 10 + '0';
	buff[cnt++] =  lMsCal.auto_calib_result_echo_amp         % 10 + '0';
	buff[cnt++] = (lMsCal.auto_calib_result_echo_amp / 10)   % 10 + '0';
	buff[cnt++] =  lMsCal.auto_calib_result_echo_amp         % 10 + '0';

	buff[cnt++] = '/';					   // 'A'

	buff[cnt++] = (lMsCal.auto_calib_result_thr / 10)   % 10 + '0';
	buff[cnt++] =  lMsCal.auto_calib_result_thr         % 10 + '0';

#if 1
	buff[cnt++] = '/';					   // 'A'
	buff[cnt++] =  lMsCal.fauto_calib         % 10 + '0';
	buff[cnt++] = '/';					   // 'A'

	buff[cnt++] = (lMsCal.auto_calib_result_slope / 1000) % 10 + '0';
	buff[cnt++] = (lMsCal.auto_calib_result_slope / 100)  % 10 + '0';
	buff[cnt++] = (lMsCal.auto_calib_result_slope / 10)   % 10 + '0';
	buff[cnt++] =  lMsCal.auto_calib_result_slope         % 10 + '0';

	buff[cnt++] = '/';					   // 'A'

	buff[cnt++] = (lMsCal.auto_calib_result_echo_amp / 10)   % 10 + '0';
	buff[cnt++] =  lMsCal.auto_calib_result_echo_amp         % 10 + '0';
	buff[cnt++] = (lMsCal.auto_calib_result_echo_amp / 10)   % 10 + '0';
	buff[cnt++] =  lMsCal.auto_calib_result_echo_amp         % 10 + '0';

	buff[cnt++] = '/';					   // 'A'

	buff[cnt++] = (lMsCal.auto_calib_result_val / 1000) % 10 + '0';
	buff[cnt++] = (lMsCal.auto_calib_result_val / 100)  % 10 + '0';
	buff[cnt++] = (lMsCal.auto_calib_result_val / 10)   % 10 + '0';
	buff[cnt++] =  lMsCal.auto_calib_result_val         % 10 + '0';


	buff[cnt++] = '/';					   // 'A'


	buff[cnt++] = (lMsCal.auto_calib_result_thr / 10)   % 10 + '0';
	buff[cnt++] =  lMsCal.auto_calib_result_thr         % 10 + '0';
#endif

	URT_TxPkt(URT_IDX_1, buff, cnt);
#endif
	URT_TxPkt(URT_IDX_1, buff, cnt);

#if 1

#endif	

}


#endif


void MsCAL_AutoCalibResult(U08 ch)
{
	U16 i,j,k,m;
	U16 min_y = 0xffff;
	U16 max_y = 0;
	U16 max_x = 0;
	U16 min_x = 0;
	U16 max_range= MnTST_PrGet_CH_Value(ch, MnTST_OPT_SINGLE_THR_RANGE);
	U16 empty = MnMSR_BaseGet_Ch_Value(ch, MnMS0_OPT_SINGLE_EMPTY);
	U16 empty_range = empty*1.1;
	U16 dead = MnMSR_BaseGet_Ch_Value(ch, MnMS0_OPT_SINGLE_DEADZONE);
	U16 x_end;
	U16 x_start;
	U16 thr_range;
	U32 thr_min_y = 0xffff;
	U32 thr_max_y = 0;
	U32 thr_min_x = 0;
	U32 thr_max_x = 0;

	U16 val_volt[201][2];
	U16 val_thr[2];
	S16 val;
	S16 val2;
	S16 sel;

	U16 auto_range_x = MnTST_PrGet_Value(MnTST_OPT_AUTO_RANGE);
	S16 t;

	sel  = MnMSR_CalGet_Ch_Value(ch, MnMS1_OPT_SINGLE_AUTO_CAL);

	U16 value_range =MnTST_PrGet_Value(MnTST_OPT_AUTO_LEVEL_RANGE);


	lMsCal.auto_cal_sel_cnt = 0;
	lMsCal.auto_slope_max[ch] = 0;

	for(m=0; m<5; m++)
	{
		if((m==1) && (lMsCal.auto_cal_sel_cnt>0))
			continue;

		if((m==2) && (lMsCal.auto_cal_sel_cnt>0))
			continue;	

		if((m==3) && (lMsCal.auto_cal_sel_cnt>0))
			continue;	
	
		for(j=0; j<(value_range*2)+1; j++)
		{
				
			t = (value_range)+((j % 2 == 0) ? (j / 2) : -(j + 1) / 2);

			if((m==4) && (lMsCal.auto_calib_result_count!=t))
				continue;

		
			for(k=1; k<51; k++)
			{
				if((m==0) && (k<20 || k>30))
					continue;
				if((m==1) && (k<20 || k>30))
					continue;
				if((m==2) && (k>=20 && k<=30))
					continue;
				if((m==3) && (k>=20 && k<=30))
					continue;

			
				val2 = sel-(value_range)+t;

				if(val2>=(empty-dead))
					val = empty-dead;
				else
					val = empty -val2;

				if(val2<=0)
					val = 0;
				else
					val = empty-val2;


				//val = TPRT_Calib_Reverse(ch,val);

				if((val-auto_range_x) <= dead)
					x_start = dead;
				else
					x_start = val-auto_range_x;
				
				if((val+auto_range_x) >= empty)
					x_end = empty;
				else
					x_end = val+auto_range_x;

#if 1
				if((val+max_range) >= empty)
					thr_range = empty;
				else
					thr_range = val+max_range; 
#endif	
				

				for(i=dead; i<empty_range; i++)
				{
					#if 0
					if(j==0)
						lMsCal.aEco_real[ch][i] =SDRAM_aEco_prof_S3[(lMsCal.auto_calib*ADC_BUFF_MAX*6)+5*ADC_BUFF_MAX+i];
					#endif
					if(i==val)
					{
						#if 0
						val_volt[lMsCal.auto_calib][j] = 
						((SDRAM_aEco_prof_S3[(lMsCal.auto_calib*ADC_BUFF_MAX*6)+5*ADC_BUFF_MAX+i]+SDRAM_aEco_prof_S3[(lMsCal.auto_calib*ADC_BUFF_MAX*6)+5*ADC_BUFF_MAX+i-1])/2)-5;
						#endif
						val_volt[j][1] = SDRAM_aEco_prof_S3[(k*ADC_BUFF_MAX*6)+5*ADC_BUFF_MAX+i];
						val_volt[j][0] = SDRAM_aEco_prof_S3[(k*ADC_BUFF_MAX*6)+5*ADC_BUFF_MAX+i-1];
						lMsCal.auto_calib_val_volt[k] = val_volt[j][1];
					}

					if(i==x_start)
					{
						if(SDRAM_aEco_prof_S3[(k*ADC_BUFF_MAX*6)+5*ADC_BUFF_MAX+i]<min_y)
						{
							min_y = SDRAM_aEco_prof_S3[(k*ADC_BUFF_MAX*6)+5*ADC_BUFF_MAX+i];
							min_x = i;
						}
					}
					else if(i==x_end)
					{	
						if(SDRAM_aEco_prof_S3[(k*ADC_BUFF_MAX*6)+5*ADC_BUFF_MAX+i]>=max_y)
						{
							max_y = SDRAM_aEco_prof_S3[(k*ADC_BUFF_MAX*6)+5*ADC_BUFF_MAX+i];
							max_x = i;
						}
					}

					if(i<=val)
					{
						if(SDRAM_aEco_prof_S3[(k*ADC_BUFF_MAX*6)+5*ADC_BUFF_MAX+i]<thr_min_y)
						{
							thr_min_y =  SDRAM_aEco_prof_S3[(k*ADC_BUFF_MAX*6)+5*ADC_BUFF_MAX+i];
							thr_min_x = i;				
						}
					}
					else if(i<thr_range)
					{
						if(SDRAM_aEco_prof_S3[(k*ADC_BUFF_MAX*6)+5*ADC_BUFF_MAX+i]>thr_max_y)
						{
							thr_max_y = SDRAM_aEco_prof_S3[(k*ADC_BUFF_MAX*6)+5*ADC_BUFF_MAX+i];
							thr_max_x = i;
						}	
					}
					
					if(thr_max_y<=thr_min_y)
						thr_min_y = thr_max_y;

				}
				
				if(min_y == 0xffff || max_y == 0 || max_x ==0 || min_x ==0)
				{
					lMsCal.auto_calib_slope[k] = 0;
					lMsCal.auto_calib_thr[k] = 0;
				}
				else
				{
					if(max_y>=min_y)
						lMsCal.auto_calib_slope[k] = (max_y- min_y) / (max_x-min_x);		
					else
						lMsCal.auto_calib_slope[k] = 0;
					val_thr[1] = ((val_volt[j][1] - thr_min_y)*100) / (thr_max_y-thr_min_y);
					val_thr[0] = ((val_volt[j][0] - thr_min_y)*100) / (thr_max_y-thr_min_y);

					if(val_thr[1]>val_thr[0])
					{
						if(val_thr[0]>50)
							lMsCal.auto_calib_thr[k] = val_thr[0];
						else if(val_thr[1]<50)
							lMsCal.auto_calib_thr[k] = val_thr[1];
						else
							lMsCal.auto_calib_thr[k] = 50;
							
					}
					else
					{
						if(val_thr[1]>50)
							lMsCal.auto_calib_thr[k] = val_thr[1];
						else if(val_thr[0]<50)
							lMsCal.auto_calib_thr[k] = val_thr[0];
						else
							lMsCal.auto_calib_thr[k] = 50;
					}
					//lMsCal.auto_calib_thr[lMsCal.auto_calib][j] = ((val_volt[lMsCal.auto_calib][j] - thr_min_y)*100) / (thr_max_y-thr_min_y);

				}

				lMsCal.auto_calib_thr_max[k][1] = thr_max_y;
				lMsCal.auto_calib_thr_max[k][0] = thr_max_x;
				lMsCal.auto_calib_thr_min[k][1]= thr_min_y;
				lMsCal.auto_calib_thr_min[k][0] = thr_min_x;
				lMsCal.auto_calib_val_max[k][1]= max_y;
				lMsCal.auto_calib_val_max[k][0] = max_x;
				lMsCal.auto_calib_val_min[k][1] = min_y;
				lMsCal.auto_calib_val_min[k][0] = min_x;		

				min_y = 0xffff;
				max_y = 0;
				max_x = 0;
				min_x = 0;
				thr_min_y = 0xffff;
				thr_max_y = 0;
				thr_max_x = 0;
				thr_min_x = 0;


				if(m<4)
					MsCAL_AutoCalibINF(ch,t,k,m);
			}

		}
	}


	if(lMsCal.auto_cal_sel_cnt>0)
	{
		lMsCal.fauto_calib = 0;
		lMsCal.auto_calib_result_slope = lMsCal.auto_slope_max[ch];
		MnMSR_CalSet_Ch_Value(ch, MnMS1_OPT_SINGLE_ECHO_AMP, lMsCal.auto_calib_result_echo_amp );
		MnMSR_CalSet_Ch_Value(ch, MnMS1_OPT_SINGLE_THR_HEAVY,MnMS1_THRESHOLD_AUTO);
		MnMSR_CalSet_Ch_Value(ch, MnMS1_OPT_SINGLE_THR_LIGHT,MnMS1_THRESHOLD_AUTO);
		MnMSR_Set_Threshold_Ch_Value(ch, MnMS1_OPT_SINGLE_THR_LIGHT,lMsCal.auto_calib_result_thr);
		MnMSR_Set_Threshold_Ch_Value(ch, MnMS1_OPT_SINGLE_THR_HEAVY,lMsCal.auto_calib_result_thr);
		lMsCal.auto_slope_max[ch] = 0;
		CalPrc_ResetFlag(ch);
		lMsCal.auto_gain_fslope[ch] = 1;
	}
	else
	{

		lMsCal.fauto_calib = 2;
		lMsCal.auto_calib_fresult = 1;
		lMsCal.auto_calib_result_count = 50;
	}




#if 1

#endif	

}

void MsCAL_ProcAutoCalib(U08 ch)
{
	U16 i;
	U16 empyt = MnMSR_BaseGet_Ch_Value(ch, MnMS0_OPT_SINGLE_EMPTY);
	U16 dead = MnMSR_BaseGet_Ch_Value(ch, MnMS0_OPT_SINGLE_DEADZONE);
	U08 buff[2048] = {0, };
	U16 buff_cnt = 0;

	if(lMsCal.fauto_calib!=1)
	{
		ApRun_ProcMode();
		return;
	}

	buff[buff_cnt++] = 0x30;
	buff[buff_cnt++] = 0x30;
	buff[buff_cnt++] = 0x30;
	buff[buff_cnt++] = 0x30;

	HAL_Delay(10);
	URT_TxPkt(URT_IDX_1, buff, buff_cnt);
	buff_cnt = 0;

	for(i=0; i<600; i++)
	{
		lMsCal.aEco_real[ch][i] = ADC_GetEchoVal(ch, i);
		buff[buff_cnt++] = (U08)((lMsCal.aEco_real[ch][i]>>8) & 0x00ff);
		buff[buff_cnt++] = (U08)((lMsCal.aEco_real[ch][i]>>0) & 0x00ff);
	}

	buff[0] = (U08)((empyt>>8) & 0x00ff);
	buff[1] = (U08)((empyt>>0) & 0x00ff);
	buff[2] = (U08)((dead>>8) & 0x00ff);
	buff[3] = (U08)((dead>>0) & 0x00ff);
	buff[4] = (U08)(((lMsCal.auto_calib+1)>>8) & 0x00ff);
	buff[5] = (U08)(((lMsCal.auto_calib+1)>>0) & 0x00ff);

	URT_TxPkt(URT_IDX_1, buff, buff_cnt);
	HAL_Delay(10);

	if(++lMsCal.auto_calib>=50)
		lMsCal.auto_calib = 0;

	ApRun_ProcMode();
}


#if 0
void MsCAL_ProcAutoCalib(U08 ch)
{
	U16 i;
	U32 j;
	U16 empyt_range = MnMSR_BaseGet_Ch_Value(ch, MnMS0_OPT_SINGLE_EMPTY)*1.1;

	//SDRAM_aEco_prof_S0[j+(i*ADC_BUFF_MAX)] = 0;
	//SDRAM_aEco_prof_S1[j+(i*ADC_BUFF_MAX)] = 0;

	if(lMsCal.fauto_calib!=1)
	{
		ApRun_ProcMode();
		return;
	}

	if(nand_auto_calib_cnt[ch]==4)
		nand_auto_calib_cnt[ch] =0;

	if(nand_meta_calib_block[ch]==451+(ch*10)+4)
		nand_meta_calib_block[ch] = 451+(ch*10);

	for(i=0; i<empyt_range; i++)
	{
		j = (lMsCal.auto_calib*ADC_BUFF_MAX*6)+lMsCal.auto_calib_cnt*ADC_BUFF_MAX+i;
	
		SDRAM_aEco_prof_S3[j] = ADC_GetEchoVal(ch, i);
	}

	

	
	if(++lMsCal.auto_calib_cnt>=5)
	{
		lMsCal.auto_calib_cnt = 0;
		
		MsCal_AutoGainProf(ch,1);
	
		for(i=0; i<empyt_range; i++)
		{
			lMsCal.aEco_real[ch][i] = SDRAM_aEco_prof_S3[(lMsCal.auto_calib*ADC_BUFF_MAX*6)+(5*ADC_BUFF_MAX)+i];
		}


		if(++lMsCal.auto_calib>=51)
		{
			MsCAL_AutoCalibResult(ch);
			
			for(i=1; i<51; i++)
			{
				nand_auto_calib_page[ch] = i-1;
				for(j=0; j<5; j++)
				{
					nand_auto_calib_block[ch] = 400+(24*ch)+(nand_auto_calib_cnt[ch]*6)+j;
					NND_Echo_AutoGain_RealSave(ch,i,j);
				}
				nand_auto_calib_block[ch] = 400+(24*ch)+(nand_auto_calib_cnt[ch]*6)+5;		
				nand_meta_calib_block[ch] = 451+(10*ch)+nand_auto_calib_cnt[ch];
				NND_Echo_AutoGain_ProfSave(ch,i);
			}
	
			nand_auto_calib_cnt[ch]++;

			nand_auto_calib_block[ch] = 400+(24*ch)+(nand_auto_calib_cnt[ch]*6);
			if(nand_auto_calib_block[ch] >= (400+(24*ch)+24))
				nand_auto_calib_block[ch] = 400+(24*ch);

			nand_meta_calib_block[ch] = 451+(10*ch)+nand_auto_calib_cnt[ch];
			
			nand_auto_calib_page[ch] = 0;


			if(ch==APP_CH_1)
			{
				MRM_WrWord(_mNAND_CH1_auto_calib_NUM_L,nand_auto_calib_num[ch]);
				MRM_WrWord(_mNAND_CH1_auto_calib_CNT_L,nand_auto_calib_cnt[ch]);
				MRM_WrWord(_mNAND_CH1_AUTO_BLOCK_L,nand_auto_calib_block[ch]);
				MRM_WrByte(_mNAND_CH1_AUTO_PAGE,nand_auto_calib_page[ch]);
				MRM_WrWord(_mNAND_CH1_CALIB_META_BLOCK_L,nand_meta_calib_block[ch]);
				//MRM_WrByte(_mNAND_CH1_CALIB_META_PAGE,nand_meta_calib_page[ch]);				
			}
			else if(ch==APP_CH_2)
			{
				MRM_WrWord(_mNAND_CH2_auto_calib_NUM_L,nand_auto_calib_num[ch]);
				MRM_WrWord(_mNAND_CH2_auto_calib_CNT_L,nand_auto_calib_cnt[ch]);
				MRM_WrWord(_mNAND_CH2_AUTO_BLOCK_L,nand_auto_calib_block[ch]);
				MRM_WrByte(_mNAND_CH2_AUTO_PAGE,nand_auto_calib_page[ch]);
				MRM_WrWord(_mNAND_CH2_CALIB_META_BLOCK_L,nand_meta_calib_block[ch]);
				//MRM_WrByte(_mNAND_CH2_CALIB_META_PAGE,nand_meta_calib_page[ch]);

			}
			lMsCal.auto_calib = 0;
			lMsCal.auto_slope_max = 0;
		}
		else
			ApRun_ProcMode();

	}
}
#endif

#if 0

void MsCAL_ProcAutoGain(U08 ch)
{
	U16 i;

	U16 a=gTmr.c.c001ms_cnt;


	if(MnMSR_CalGet_Ch_Value(ch, MnMS1_OPT_SINGLE_AUTO_FAMP) == MnMS1_AUTO_FAMP_OFF)
		return; 	
	
	switch(MsCAL_Get_Stt(ch))
	{
		case ApSTT_TPR_NG:	
		case ApSTT_MSR_NG:		return;
		case ApSTT_MSR_OK:		break;
		case ApSTT_MSR_TRAC:
		case ApSTT_MSR_IDLE:
		case ApSTT_MSR_WEAK:	return;
		default:				return;
	}

	if(ApSTT_GetStatus(ch)!=ApSTT_MSR_OK)
		return;

	if(lMsCal.auto_gain_fslope[ch]==1)
		return;


	lMsCal.fauto_calib = 1;

#if 1

	if(lMsCal.auto_gain_cnt>=5)
	{
		lMsCal.auto_gain_cnt = 0;
		lMsCal.fauto_calib = 0;
		return;
	}
	else
	{
		for(i=1; i<6; i++)
		{
			if(i==1)
				lMsCal.auto_calib = MnMSR_CalGet_Ch_Value(ch, MnMS1_OPT_SINGLE_ECHO_AMP);
			else if(i==2)
				lMsCal.auto_calib = MnMSR_CalGet_Ch_Value(ch, MnMS1_OPT_SINGLE_ECHO_AMP)-1;
			else if(i==3)
				lMsCal.auto_calib = MnMSR_CalGet_Ch_Value(ch, MnMS1_OPT_SINGLE_ECHO_AMP)+1;
			else if(i==4)
				lMsCal.auto_calib = MnMSR_CalGet_Ch_Value(ch, MnMS1_OPT_SINGLE_ECHO_AMP)-2;
			else if(i==5)
				lMsCal.auto_calib = MnMSR_CalGet_Ch_Value(ch, MnMS1_OPT_SINGLE_ECHO_AMP)+2;


			ApRun_MsrStart(ch, _F_F);		

			MsTPR_ProcCalc(ch); 

			MsCAL_Auto_Gain_Real(ch);
		}
	}
	
#endif

	lMsCal.fauto_calib = 0;
}

#if 0
	//I08 sDbg[128];
	_SPRINTF(sDbg, "%d", gTmr.c.c001ms_cnt-a);
	DpFIG_DrwRect(150, 60, 400, 25, _cBGD_SCRN, DpFIG_FILL);
	DpSTR_GuiLeft(150, 60, _cECO_LVL_TITL, _cBGD_SCRN, _fE17HsB, sDbg); 
#endif
#if 0
	for(i=0; i<ADC_BUFF_MAX; i++)
	{
		lMsCal.aEco_real[APP_CH_1][i] = SDRAM_aEco_prof_S3[(MnMSR_CalGet_Ch_Value(APP_CH_1, MnMS1_OPT_SINGLE_ECHO_AMP)*ADC_BUFF_MAX*6)+5*ADC_BUFF_MAX+i];
	}
#endif	

#endif







#if 0
void MsCAL_ProcCycle(U08 iCh, U16 cnt)
{
	U16 t_dep   = ScHYB_PrGet_Value(iCh, ScHYB_F04_T_DEP);
	U16 s_dep   = ScHYB_PrGet_Value(iCh, ScHYB_F02_S_DEP);
	U16 cpt_cyc = MnMSR_BaseGet_Value(MnMS0_I00_CH0_THR_MOD);
	U08 cpt_typ = MnMSR_BaseGet_Value(MnMS0_I01_CH1_THR_MOD);
	

	lMsCal.dist_cycl[iCh][cnt] = lMsCal.stp3_dist[iCh];
	lMsCal.dist_cycl[iCh][0] +=lMsCal.dist_cycl[iCh][cnt];

	if(cnt==cpt_cyc)
	{	
		switch(cpt_typ)
		{
			case MnMS0_IV1_AVRG:
				// Sum Data
				lMsCal.stp3_dist[iCh] = lMsCal.dist_cycl[iCh][0]/cnt;
				if(lMsCal.stp3_dist[iCh]==t_dep)	lMsCal.stp3_sldg[iCh]=t_dep-lMsCal.stp3_dist[iCh];
				else 								lMsCal.stp3_sldg[iCh] = t_dep - s_dep - lMsCal.stp3_dist[iCh];
				break;
			case MnMS0_IV1_LAST:
				lMsCal.stp3_dist[iCh]=lMsCal.dist_cycl[iCh][cpt_cyc];
				if(lMsCal.stp3_dist[iCh]==t_dep)	lMsCal.stp3_sldg[iCh]=t_dep-lMsCal.stp3_dist[iCh];
				else 								lMsCal.stp3_sldg[iCh] = t_dep - s_dep - lMsCal.stp3_dist[iCh];
				break;
			default:		break;
		}
		_MEMSET(lMsCal.dist_cycl[iCh], 0, sizeof(lMsCal.dist_cycl[iCh]));
	}
}
#endif

#if 0
void MsCAL_ProcAvgSave(U08 ch)
{
	U32 i;
	U32 j;
	U16 idx;
	U16 idx_max;
	RTC_TM curr_tm = RTC_GetTime();

	U16 empty = MnMSR_BaseGet_Ch_Value(ch, MnMS0_OPT_SINGLE_EMPTY);
	U16 range = empty*1.1;

	lMsCal.csave_avg[ch]++;

	if(range>600)
		idx_max = 1000000/(2*range);
	else 
		idx_max = 800;

	if(lMsCal.csave_avg[ch]>idx_max)
		lMsCal.csave_avg[ch] = idx_max;

	if(ch == APP_CH_1)
	{
		for(j=lMsCal.csave_avg[ch]-1; j>0; j--)
		{
			if(lMsCal.csave_avg[ch]<=1)	break;
			for(i=0; i<range; i++)
			{
				SDRAM_aEco_prof_S2[((j)*range)+i] = SDRAM_aEco_prof_S2[((j-1)*range)+i];
			}
		}	

		// Add Data
		for(i=0; i<range; i++)
		{
			SDRAM_aEco_prof_S2[i] = lMsCal.aEco_rslt[APP_CH_1][i];
		}
		SDRAM_aEco_prof_S2[0] = curr_tm.day;
		SDRAM_aEco_prof_S2[1] = curr_tm.hours;
		SDRAM_aEco_prof_S2[2] = curr_tm.minutes;
		SDRAM_aEco_prof_S2[3] = empty;
		SDRAM_aEco_prof_S2[4] = range;
		SDRAM_aEco_prof_S2[5] = (U16)MsCAL_GetVl_Threshold(ch, MsCAL_THR_LIGHT);
		SDRAM_aEco_prof_S2[6] = (U16)MsCAL_GetVl_Threshold(ch, MsCAL_THR_HEAVY);
		SDRAM_aEco_prof_S2[7] = lMsCal.Tracking_dist[ch][MsCAL_THR_LIGHT];
		SDRAM_aEco_prof_S2[8] = lMsCal.Tracking_dist[ch][MsCAL_THR_HEAVY];
		SDRAM_aEco_prof_S2[9] = lMsCal.rslt_sldg[ch][MsCAL_THR_LIGHT];
		SDRAM_aEco_prof_S2[10] = lMsCal.rslt_sldg[ch][MsCAL_THR_HEAVY];

	}
	else if(ch == APP_CH_2)
	{
		for(j=lMsCal.csave_avg[ch]-1; j>0; j--)
		{
			if(lMsCal.csave_avg[ch]<=1)	break;
			for(i=0; i<range; i++)
			{
				SDRAM_aEco_prof_S3[((j)*range)+i] = SDRAM_aEco_prof_S3[((j-1)*range)+i];
			}
		}	
		
		// Add Data
		for(i=0; i<range; i++)
		{
			SDRAM_aEco_prof_S3[i] = lMsCal.aEco_rslt[APP_CH_2][i];
		}
		SDRAM_aEco_prof_S3[0] = curr_tm.day;
		SDRAM_aEco_prof_S3[1] = curr_tm.hours;
		SDRAM_aEco_prof_S3[2] = curr_tm.minutes;
		SDRAM_aEco_prof_S3[3] = empty;
		SDRAM_aEco_prof_S3[4] = range;
		SDRAM_aEco_prof_S3[5] = (U16)MsCAL_GetVl_Threshold(ch, MsCAL_THR_LIGHT);
		SDRAM_aEco_prof_S3[6] = (U16)MsCAL_GetVl_Threshold(ch, MsCAL_THR_HEAVY);
		SDRAM_aEco_prof_S3[7] = lMsCal.Tracking_dist[ch][MsCAL_THR_LIGHT];
		SDRAM_aEco_prof_S3[8] = lMsCal.Tracking_dist[ch][MsCAL_THR_HEAVY];
		SDRAM_aEco_prof_S3[9] = lMsCal.rslt_sldg[ch][MsCAL_THR_LIGHT];
		SDRAM_aEco_prof_S3[10] = lMsCal.rslt_sldg[ch][MsCAL_THR_HEAVY];
	}
}
#endif
//------------------------------------------------------------------------------------------------------------------------------
//  EOF
//------------------------------------------------------------------------------------------------------------------------------

