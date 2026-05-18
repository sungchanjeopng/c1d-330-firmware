//------------------------------------------------------------------------------------------------------------------------------
//  Copyright (C) 2021, WESS-Global Inc., All Rights Reserved.
//
//  The information contained herein is confidential property of WESS-Global Inc. The use, copying, transfer or disclosure of
//  such information is prohibited except by express written agreement with WESS-Global Co.,LTD
//
//  $ Date: 2020-07-13 12:00:00 +0900 $
//------------------------------------------------------------------------------------------------------------------------------

#ifndef ___SCREEN_MARQUEE_H___
#define ___SCREEN_MARQUEE_H___

//------------------------------------------------------------------------------------------------------------------------------
//  Includes
//------------------------------------------------------------------------------------------------------------------------------
#include "app_sys.h"


//------------------------------------------------------------------------------------------------------------------------------
//  Constant definitions
//------------------------------------------------------------------------------------------------------------------------------
#define MQE_LCD_WIDTH       800
#define MQE_LCD_HEIGHT      480

#define MQE_TEXT_MAX_LEN    64

#define MQE_SCROLL_SPEED    40      // pixels per frame
#define MQE_FRAME_DELAY     20      // ms between frames
#define MQE_TEXT_PROMO      "\x32\x33\x34\x35\x36\x37\x38\x20\x21\x22\x23\x24\x25\x3A\x30\x31\x39"   // encoded KOR glyph indexes for marquee promo text


//------------------------------------------------------------------------------------------------------------------------------
//  Type definitions
//------------------------------------------------------------------------------------------------------------------------------
typedef struct {
    I08 text[MQE_TEXT_MAX_LEN];     // Text to display
    S16 xPos;                        // Current X position
    U16 textWidth;                   // Calculated text width in pixels
    U08 isRunning;                   // Marquee running flag
    U32 lastUpdate;                  // Last update tick
    U32 fgColor;                     // Foreground color
    U32 bgColor;                     // Background color
} ScMQE_LS;


//------------------------------------------------------------------------------------------------------------------------------
//  Macros
//------------------------------------------------------------------------------------------------------------------------------


//------------------------------------------------------------------------------------------------------------------------------
//  Extern global variables
//------------------------------------------------------------------------------------------------------------------------------


//------------------------------------------------------------------------------------------------------------------------------
//  Extern global APIs
//------------------------------------------------------------------------------------------------------------------------------
// Get Local Variables
extern U08 ScMQE_IsRunning(void);

// Set Local Variables
extern void ScMQE_SetText(const I08* text);
extern void ScMQE_SetColors(U32 fgColor, U32 bgColor);
extern void ScMQE_SetSpeed(U08 speed);

// Operation
extern void ScMQE_InitMain(void);
extern void ScMQE_ProcMain(void);
extern void ScMQE_Start(void);
extern void ScMQE_Stop(void);
extern void ScMQE_EnterMode(const I08* text);  // Enter marquee mode with text


#endif // ___SCREEN_MARQUEE_H___
//------------------------------------------------------------------------------------------------------------------------------
//  EOF
//------------------------------------------------------------------------------------------------------------------------------
