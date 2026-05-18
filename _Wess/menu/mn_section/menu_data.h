//------------------------------------------------------------------------------------------------------------------------------
//  Copyright (C) 2021, WESS-Global Inc., All Rights Reserved.
//
//  The information contained herein is confidential property of WESS-Global Inc. The use, copying, transfer or disclosure of
//  such information is prohibited except by express written agreement with WESS-Global Co.,LTD
//
//  $ Date: 2020-07-13 12:00:00 +0900 $
//------------------------------------------------------------------------------------------------------------------------------

#ifndef ___MENU_DATA_H___
#define ___MENU_DATA_H___

//------------------------------------------------------------------------------------------------------------------------------
//  Includes
//------------------------------------------------------------------------------------------------------------------------------
#include "app_sys.h"
#include "app_main.h"

//------------------------------------------------------------------------------------------------------------------------------
//  Constant definitions
//------------------------------------------------------------------------------------------------------------------------------

enum {
	MnDAT_SUB_TREND = 0,
	MnDAT_SUB_COMM,
	MnDAT_SUB_ECHO,
	// Number Max
	MnDAT_SUB_NUM,
};
#define MnDAT_SUB_MIN		(MnDAT_SUB_TREND)
#define MnDAT_SUB_MAX		(MnDAT_SUB_NUM-1)

enum {
	MnDS0_OPT_INTERVAL = 0,
	MnDS0_OPT_DOWNLOAD,
	MnDS0_OPT_DELETE,
	MnDS0_OPT_DISPLAY_TERM,
	// Number Max
	MnDS0_OPT_NUM,
};

#define MnDS0_OPT_MIN		(MnDS0_OPT_INTERVAL)
#define MnDS0_OPT_MAX		(MnDS0_OPT_NUM-1)

enum {
	MnDS0_ASSIGN_CH1_LIGHT,
	MnDS0_ASSIGN_CH1_HEAVY,
	MnDS0_ASSIGN_CH2_LIGHT,
	MnDS0_ASSIGN_CH2_HEAVY,

	MnDS0_ASSIGN_NUM,
};


enum {
	MnDS0_INTV_01S = 0,
	MnDS0_INTV_10S,
	MnDS0_INTV_01M,
	MnDS0_INTV_10M,
	MnDS0_INTV_01H,
	// Number Max
	MnDS0_INTV_NUM,
};
	
#define MnDS0_INTV_MIN		(MnDS0_INTV_01S)
#define MnDS0_INTV_MAX		(MnDS0_INTV_01H)
#define MnDS0_INTV_DEF		(MnDS0_INTV_01M)

enum {
	MnDS0_DOWN_CH1_LIGHT = 0,
	MnDS0_DOWN_CH1_HEAVY,
	MnDS0_DOWN_CH2_LIGHT,
	MnDS0_DOWN_CH2_HEAVY,

	MnDS0_DOWN_NUM,
};

#define MnDS0_DOWN_MIN	(MnDS0_DOWN_CH1_LIGHT)
#define MnDS0_DOWN_MAX	(MnDS0_DOWN_NUM-1)
#define MnDS0_DOWN_SINGLE_MAX	(MnDS0_DOWN_CH1_HEAVY)

enum {
	MnDS0_DELETE_NO = 0,
	MnDS0_DELETE_YES,
	// Number Max
	MnDS0_DELETE_NUM,
};	
#define MnDS0_DELETE_MIN	(MnDS0_DELETE_NO)
#define MnDS0_DELETE_MAX	(MnDS0_DELETE_NUM-1)

enum {
	MnDS0_DISPLAY_TERM_1 = 0,
	MnDS0_DISPLAY_TERM_2,
	// Number Max
	MnDS0_DISPLAY_TERM_NUM,
};
	
#define MnDS0_DISPLAY_TERM_MIN		(MnDS0_DISPLAY_TERM_1)
#define MnDS0_DISPLAY_TERM_MAX		(MnDS0_DISPLAY_TERM_2)
#define MnDS0_DISPLAY_TERM_DEF		(MnDS0_DISPLAY_TERM_1)

#if 0
enum {
	MnDS1_OPT_SAVEMODE = 0,
	MnDS1_OPT_SAVEINTERVAL,
	MnDS1_OPT_ECHO_LOAD,
	MnDS1_OPT_DOWNLOAD,
	MnDS1_OPT_DELETE,

	MnDS1_OPT_NUM,
};

#define MnDS1_OPT_MIN	(MnDS1_OPT_SAVEMODE)
#define MnDS1_OPT_MAX	(MnDS1_OPT_NUM-1)
#endif


#if 1
enum {
	MnDS1_OPT_CH1_SAVEMODE = 0,
	MnDS1_OPT_CH1_SAVEINTERVAL,
	MnDS1_OPT_CH1_ECHO_LOAD,
	MnDS1_OPT_CH1_ECHO_VIEW_MODE,
	MnDS1_OPT_CH1_DELETE,
	MnDS1_OPT_CH2_SAVEMODE,
	MnDS1_OPT_CH2_SAVEINTERVAL,
	MnDS1_OPT_CH2_ECHO_LOAD,
	MnDS1_OPT_CH2_ECHO_VIEW_MODE,
	MnDS1_OPT_CH2_DELETE,

	MnDS1_OPT_NUM,
};

#define MnDS1_OPT_MIN	(MnDS1_OPT_CH1_SAVEMODE)
#define MnDS1_OPT_MAX	(MnDS1_OPT_NUM-1)

enum {
	MnDS1_OPT_SINGLE_SAVEMODE=0,
	MnDS1_OPT_SINGLE_SAVEINTERVAL,
	MnDS1_OPT_SINGLE_ECHO_LOAD,
	MnDS1_OPT_SINGLE_ECHO_VIEW_MODE,
	MnDS1_OPT_SINGLE_DELETE,

	MnDS1_OPT_SINGLE_NUM,
};

#define MnDS1_OPT_SINGLE_MIN (MnDS1_OPT_SINGLE_SAVEMODE)
#define MnDS1_OPT_SINGLE_MAX (MnDS1_OPT_SINGLE_NUM-1)
#endif



enum {
	MnDS1_SAVEMODE_REAL = 0,
	MnDS1_SAVEMODE_AVER,
	
	MnDS1_SAVEMODE_NUM,
};

#define MnDS1_SAVEMODE_MIN	(MnDS1_SAVEMODE_REAL)
#define MnDS1_SAVEMODE_MAX	(MnDS1_SAVEMODE_NUM-1)
#define MnDS1_SAVEMODE_DEF	(MnDS1_SAVEMODE_AVER)

enum {
	MnDS1_SAVEINTERVAL_01S = 0,
	MnDS1_SAVEINTERVAL_10S,
	MnDS1_SAVEINTERVAL_01M,
	MnDS1_SAVEINTERVAL_10M,
	MnDS1_SAVEINTERVAL_01H,

	MnDS1_SAVEINTERVAL_NUM,
};

#define MnDS1_SAVEINTERVAL_MIN	(MnDS1_SAVEINTERVAL_01S)
#define MnDS1_SAVEINTERVAL_MAX	(MnDS1_SAVEINTERVAL_NUM-1)
#define MnDS1_SAVEINTERVAL_DEF	(MnDS1_SAVEINTERVAL_01M)

enum {
	MnDS1_ECHO_VIEW_MODE_MSR = 0,
	MnDS1_ECHO_VIEW_MODE_AUTO_CAL,

	MnDS1_ECHO_VIEW_MODE_NUM,
};

#define MnDS1_ECHO_VIEW_MODE_MIN	(MnDS1_ECHO_VIEW_MODE_MSR)
#define MnDS1_ECHO_VIEW_MODE_MAX	(MnDS1_ECHO_VIEW_MODE_NUM-1)
#define MnDS1_ECHO_VIEW_MODE_DEF	(MnDS1_ECHO_VIEW_MODE_MSR)



enum {
	MnDS1_DOWNLOAD_NO = 0,
	MnDS1_DOWNLOAD_YES,

	MnDS1_DOWNLOAD_NUM,
};

#define MnDS1_DOWNLOAD_MIN	(MnDS1_DOWNLOAD_NO)
#define MnDS1_DOWNLOAD_MAX	(MnDS1_DOWNLOAD_NUM-1)

enum {
	MnDS1_DELETE_NO = 0,
	MnDS1_DELETE_YES,

	MnDS1_DELETE_NUM,
};

#define MnDS1_DELETE_MIN	(MnDS1_DELETE_NO)
#define MnDS1_DELETE_MAX	(MnDS1_DELETE_NUM-1)



// Item List - Sub-Section #1 (Communication)
enum {
	MnDS2_OPT_TYPE = 0,
	MnDS2_OPT_BAUD,
	MnDS2_OPT_ADDR,
	// Number Max
	MnDS2_OPT_NUM,
};

#define MnDS2_OPT_MIN		(MnDS2_OPT_TYPE)
#define MnDS2_OPT_MAX		(MnDS2_OPT_NUM-1)

// Item List - Sub-Section #1 (Communication)
enum {
	MnDS2_OPT_RF_TYPE = 0,
	MnDS2_OPT_RF_CH1_ASSIGN,
	MnDS2_OPT_RF_CH1_ADDRESS,
	MnDS2_OPT_RF_CH2_ASSIGN,
	MnDS2_OPT_RF_CH2_ADDRESS,
	// Number Max
	MnDS2_OPT_RF_NUM,
};

#define MnDS2_OPT_RF_MIN		(MnDS2_OPT_RF_TYPE)
#define MnDS2_OPT_RF_MAX		(MnDS2_OPT_RF_NUM-1)
#define MnDS2_OPT_RF_SINGLE_MAX	(MnDS2_OPT_RF_CH1_ADDRESS)

// Value - Comm. Item #0 (Type)
enum {
	MnDS2_TYPE_MDB = 0,
	MnDS2_TYPE_RF,
	// Number Max
	MnDS2_TYPE_NUM,
};

#define MnDS2_TYPE_MIN		(MnDS2_TYPE_MDB)
#define MnDS2_TYPE_MAX		(MnDS2_TYPE_NUM-1)
#define MnDS2_TYPE_DEF		(MnDS2_TYPE_MDB)
// Value - Comm. Item #1 (Baudrate)
enum {
	MnDS2_BAUDRATE_9600 = 0,
	MnDS2_BAUDRATE_19200,
	MnDS2_BAUDRATE_115200,
	// Number Max
	MnDS2_BAUDRATE_NUM,
};

#define MnDS2_BAUDRATE_MIN		(MnDS2_BAUDRATE_9600)
#define MnDS2_BAUDRATE_MAX		(MnDS2_BAUDRATE_NUM-1)
#define MnDS2_BAUDRATE_DEF		(MnDS2_BAUDRATE_19200)

// Value - Comm. Item #2 (Address)
#define MnDS2_ADDRESS_MIN		(1)
#define MnDS2_ADDRESS_MAX		(255)
#define MnDS2_ADDRESS_DEF		(MnDS2_ADDRESS_MIN)


#define MnDS2_RF_ADDRESS_MIN		(1)
#define MnDS2_RF_ADDRESS_MAX		(4)
#define MnDS2_RF_ADDRESS_DEF		(MnDS2_RF_ADDRESS_MIN)

// Value - Comm. Item #3 (Assign)
enum {
	MnDS2_ASSIGN_LIGHT = 0,
	MnDS2_ASSIGN_HEAVY,

	MnDS2_ASSIGN_NUM,
};

#define MnDS2_ASSIGN_MIN	(MnDS2_ASSIGN_LIGHT)
#define MnDS2_ASSIGN_MAX	(MnDS2_ASSIGN_NUM-1)
#define MnDS2_ASSIGN_DEF	(MnDS2_ASSIGN_HEAVY)

#if 0
// Sub-Section List
enum {
	MnDAT_S00_SAVE = 0,
	MnDAT_S01_COMM,
	// Number Max
	MnDAT_SCT_NUM,
};
#define MnDAT_SCT_I00		(MnDAT_S00_SAVE)

// Item List - Sub-Section #0 (Save)
enum {
	MnDS0_I00_fEN = 0,
	MnDS0_I01_ASSIGN,
	MnDS0_I02_INTV,
	MnDS0_I03_DOWN,
	MnDS0_I04_DEL,
	// Number Max
	MnDS0_ITM_NUM,
};
#define MnDS0_ITM_I00		(MnDS0_I00_fEN)

// Value - Save Item #0 (Enable Flag)
enum {
	MnDS0_IV0_OFF = 0,
	MnDS0_IV0_ON,
	// Number Max
	MnDS0_IV0_VAL_NUM,
};
#define MnDS0_IV0_VAL_DEF	(MnDS0_IV0_ON)

// Value - Save Item #1 (Data Assign)
enum {
	MnDS0_IV1_CH1 =0,
	MnDS0_IV1_CH2 ,
	MnDS0_IV1_DIFF,
	MnDS0_IV1_AVG,
	// Number Max
	MnDS0_IV1_VAL_NUM,
};

#define MnDS0_IV1_VAL_DEF	(MnDS0_IV1_CH1)



// Value - Save Item #2 (Interval Time)
enum {
	MnDS0_IV2_01S = 0,
	MnDS0_IV2_10S,
	MnDS0_IV2_01M,
	MnDS0_IV2_10M,
	MnDS0_IV2_01H,
	MnDS0_IV2_24H,
	// Number Max
	MnDS0_IV2_VAL_NUM,
};
#define MnDS0_IV2_DEF		(MnDS0_IV2_01M)

// Value - Save Item #3 (Download)
enum {
	MnDS0_IV3_OFF = 0,
	MnDS0_IV3_ON,
	// Number Max
	MnDS0_IV3_VAL_NUM,
};
#define MnDS0_IV3_VAL_DEF	(MnDS0_IV3_ON)

// Value - Save Item #4 (Delete)
enum {
	MnDS0_IV4_OFF = 0,
	MnDS0_IV4_ON,
	// Number Max
	MnDS0_IV4_VAL_NUM,
};
#define MnDS0_IV4_VAL_DEF	(MnDS0_IV4_OFF)

// Item List - Sub-Section #1 (Communication)
enum {
	MnDS2_I00_TYPE = 0,
	MnDS2_I01_BAUD,
	MnDS2_I02_ADDR,
	// Number Max
	MnDS2_ITM_NUM,
};
	
#define MnDS2_ITM_I00		(MnDS2_I00_TYPE)
// Value - Comm. Item #0 (Type)
enum {
	MnDS2_IV0_STD = 0,
	MnDS2_IV0_MDB,
	MnDS2_IV0_RFM,
	MnDS2_IV0_FidA,
	// Number Max
	MnDS2_IV0_VAL_NUM,
};
#define MnDS2_IV0_DEF		(MnDS2_IV0_STD)
// Value - Comm. Item #1 (Baudrate)
enum {
	MnDS2_IV1_4800 = 0,
	MnDS2_IV1_9600,
	MnDS2_IV1_19200,
	MnDS2_IV1_115200,
	// Number Max
	MnDS2_IV1_VAL_NUM,
};
#define MnDS2_IV1_DEF		(MnDS2_IV1_9600)
// Value - Comm. Item #2 (Address)
#define MnDS2_IV2_MIN		(1)
#define MnDS2_IV2_MAX		(247)
#define MnDS2_IV2_DEF		(MnDS2_IV2_MIN)

#endif
//------------------------------------------------------------------------------------------------------------------------------
//  Type definitions
//------------------------------------------------------------------------------------------------------------------------------
// Memory Parameter
typedef struct {
	U08 fEn;
	U08 assign;
	U08 intv;
	U08 display_term;
} MnDAT_mSavPr;


typedef struct {
	U08 savemode[APP_CH_NUM];
	U08 saveinterval[APP_CH_NUM];
	U08 delete[APP_CH_NUM];
	U08 view_mode[APP_CH_NUM];
} MnDAT_mEchoSavPr;


typedef struct {
	U08 type;
	U08 baud;
	U16 addr;

	U08 rf_assign[APP_CH_NUM];
	U16 rf_addr[APP_CH_NUM];
} MnDAT_mComPr;

// Local Struct
typedef struct {
	MnDAT_mSavPr mSavPr;
	MnDAT_mComPr mComPr;
	MnDAT_mEchoSavPr mEchoSavpr;

	U08 f_save;
	U08 prg;		// Progress Value (Percent)
	U08 lyr;		// Layer Index
} MnDAT_LS;


//------------------------------------------------------------------------------------------------------------------------------
//  Macros
//------------------------------------------------------------------------------------------------------------------------------


//------------------------------------------------------------------------------------------------------------------------------
//  Extern Global APIs - Parameters - Get
//------------------------------------------------------------------------------------------------------------------------------
extern S32 MnDAT_SavPrGet_Value(U08 iIt);
extern S32 MnDAT_ComPrGet_Value(U08 iIt);
extern S32 MnDAT_EchoSavePrGet_Value(U08 iIt);
extern S32 MnDAT_EchoSavePrGet_SingleValue(U08 ch,U08 iIt);
#if 1
extern U08 MnDAT_PrGet_fSaveEn(void);
extern U08 MnDAT_PrGet_SavIntv(void);

#endif

//------------------------------------------------------------------------------------------------------------------------------
//  Extern global APIs - Parameters - Set
//------------------------------------------------------------------------------------------------------------------------------
extern void MnDAT_SavPrSet_Value(U08 iIt, S32 val);
extern void MnDAT_ComPrSet_Value(U08 iIt, S32 val);
extern void MnDAT_EchoSavePrSet_Value(U08 iIt, S32 val);
#if 1
extern void MnDAT_PrSet_Value(S32 val);
#endif
extern void MnDAT_PrRst_Factory(void);
extern void MnDAT_Set_f_save(U08 sel);


//------------------------------------------------------------------------------------------------------------------------------
//  Extern global APIs - Varables
//------------------------------------------------------------------------------------------------------------------------------
// Get
extern U08 MnDAT_GetLayer(void);
// Set
extern void MnDAT_SetLayer(U08 lyr);


//------------------------------------------------------------------------------------------------------------------------------
//  Extern global APIs - Basic
//------------------------------------------------------------------------------------------------------------------------------
extern void MnDAT_PrInitMain(void);
extern void DatComPr_BaudInit(U08 val);
extern void MnDAT_InitMain(void);



#endif // ___MENU_DATA_H___
//------------------------------------------------------------------------------------------------------------------------------
//  EOF
//------------------------------------------------------------------------------------------------------------------------------

