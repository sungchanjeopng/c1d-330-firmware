//------------------------------------------------------------------------------------------------------------------------------
//  Copyright (C) 2021, WESS-Global Inc., All Rights Reserved.
//
//  The information contained herein is confidential property of WESS-Global Inc. The use, copying, transfer or disclosure of
//  such information is prohibited except by express written agreement with WESS-Global Co.,LTD
//
//  $ Date: 2020-07-13 12:00:00 +0900 $
//------------------------------------------------------------------------------------------------------------------------------

#ifndef ___APP_MAIN_H___
#define ___APP_MAIN_H___

//------------------------------------------------------------------------------------------------------------------------------
//  Includes
//------------------------------------------------------------------------------------------------------------------------------
#include "bsp_rtc.h"
#include "app_sys.h"


//------------------------------------------------------------------------------------------------------------------------------
//  Constant definitions
//------------------------------------------------------------------------------------------------------------------------------
// MODE - Index
enum {
	APP_MD0_SCRN = 0,
	APP_MD1_MENU,
	APP_MDn_NUM,
};

// Processor
enum {
	APP_PR0_BOOT = 0,
	APP_PR1_RUN_,

	APP_PRn_NUM,
};

// Scheduler (100ms)
enum {
	APP_SC0_START = 0,
	APP_SC1_,
	APP_SC2_,
	APP_SC3_,
	APP_SC4_,
	APP_SC5_S1_MEAS,
	APP_SC6_,
	APP_SC7_,
	APP_SC8_,
	APP_SC9_,
	APP_SCD_NUM,
};

// Secsor Index
enum {
	APP_CH_1 = 0,
	APP_CH_2,
	APP_CH_NUM,
};

enum {
	APP_INTERVAL_WAIT=0,
	APP_INTERVAL_RUN,

	APP_INTERVAL_NUM,
};

//------------------------------------------------------------------------------------------------------------------------------
//  Type definitions
//------------------------------------------------------------------------------------------------------------------------------
typedef struct {
	RTC_TM tm;			// Time

	U08 fRun;			// Flag: Run

	U08 mode;			// Window or Menu
	U08 proc;			// Processor

	U16 cInterval[APP_CH_NUM];

	U08 fInterval[APP_CH_NUM];
} APP_LS;


//------------------------------------------------------------------------------------------------------------------------------
//  Macros
//------------------------------------------------------------------------------------------------------------------------------
//  Firmware version — the numeric components below are the single source of
//  truth. The string form (_APP_VERISON), the on-screen version, and the BLE
//  device-info payload are all derived from these, so they can never drift.
//  NOTE: BLE firmware-version reporting was introduced in v1.1.2. A device
//        that does not report a version is therefore v1.1.1 or older.
#define _APP_VER_MAJOR		1
#define _APP_VER_MINOR		1
#define _APP_VER_PATCH		2

#define _APP_VER_STR2(x)	#x
#define _APP_VER_STR(x)		_APP_VER_STR2(x)
#define _APP_VERISON		"v" _APP_VER_STR(_APP_VER_MAJOR) "." _APP_VER_STR(_APP_VER_MINOR) "." _APP_VER_STR(_APP_VER_PATCH)

extern U16 aaaa;

//------------------------------------------------------------------------------------------------------------------------------
//  Extern global variables
//------------------------------------------------------------------------------------------------------------------------------


//------------------------------------------------------------------------------------------------------------------------------
//  Global APIs - Access Local Variables
//------------------------------------------------------------------------------------------------------------------------------
// Get Local Variables
extern U08 APP_GetMode(void);
extern U08 APP_GetFgRun(void);
// Set Local Variables
extern void APP_SetMode(U08 mode);


//------------------------------------------------------------------------------------------------------------------------------
//  Global APIs - RTC Time
//------------------------------------------------------------------------------------------------------------------------------
// Get
extern RTC_TM APP_RtcGetTime(void);
// Set
extern void APP_RtcSetTime(RTC_TM tm);


//------------------------------------------------------------------------------------------------------------------------------
//  Global APIs - Basic
//------------------------------------------------------------------------------------------------------------------------------
extern void APP_InitMain(void);
extern void APP_FtrReset(void);
extern void APP_LoopMain(void);
extern void AppProc_Run(void);
extern void ApRun_ProcMode(void);
extern void ApRun_MsrStart(U08 iCh, U08 fClr);

#endif // ___APP_MAIN_H___
//------------------------------------------------------------------------------------------------------------------------------
//  EOF
//------------------------------------------------------------------------------------------------------------------------------

