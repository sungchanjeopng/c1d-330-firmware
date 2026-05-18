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
// hal
#include "stm32f7xx_hal.h"
// bsp
#include "bsp_lcd.h"
// function
#include "input_key.h"
// display
#include "disp_color.h"
#include "fonts.h"
// screen
#include "screen_main.h"
// self
#include "screen_marquee.h"
#include <string.h>


//------------------------------------------------------------------------------------------------------------------------------
//  Global variables
//------------------------------------------------------------------------------------------------------------------------------


//------------------------------------------------------------------------------------------------------------------------------
//  Local variables
//------------------------------------------------------------------------------------------------------------------------------
static ScMQE_LS lScMqe;
static U08 lMqe_scrollSpeed = MQE_SCROLL_SPEED;
static U08 lMqe_needRedraw = TRUE;
#define MQE_CHAR_SPACE      20
#define MQE_WRAP_GAP        120
#define MQE_KOR_GLYPH_COUNT  27U
extern void LcdDraw_Pixel(U16 Xpos, U16 Ypos, U32 RGB_Code);


//------------------------------------------------------------------------------------------------------------------------------
//  Local Functions
//------------------------------------------------------------------------------------------------------------------------------
static U16 MQE_CalcTextWidth(const I08* text)
{
    U16 width = 0;
    U08 ch;
    S16 idx;
    const sFONT* font = &Ft75_HanSansBold_KOR;

    if (text == NULL)
    {
        return 0;
    }

    while ((ch = (U08)*text++) != '\0')
    {
        if (ch >= 0x20 && ch < (0x20 + MQE_KOR_GLYPH_COUNT))
        {
            idx = (S16)(ch - 0x20);
        }
        else
        {
            idx = 0;
        }

        width += (U16)font->info[idx].Width;
        width += MQE_CHAR_SPACE;
    }

    return width;
}

static U16 MQE_GetCharIndex(U08 ch)
{
    if (ch >= 0x20 && ch < (0x20 + MQE_KOR_GLYPH_COUNT))
    {
        return (U16)(ch - 0x20);
    }
    return 0;
}

static U08 MQE_IsGlyphPixelOn(const U08* glyph, U16 glyphWidth, U16 x, U16 y)
{
    U16 bytesPerRow = (U16)((glyphWidth + 7U) / 8U);
    U16 offset = (U16)(bytesPerRow * 8U - glyphWidth);
    U16 bitIndex = (U16)(x + offset);
    U16 byteIndex = (U16)(y * bytesPerRow + (bitIndex / 8U));
    U08 mask = (U08)(1U << (7U - (bitIndex % 8U)));

    return (glyph[byteIndex] & mask) ? TRUE : FALSE;
}

static void MQE_DrawPixelClipped(S16 x, S16 y, U32 color)
{
    if (x < 0 || y < 0 || x >= MQE_LCD_WIDTH || y >= MQE_LCD_HEIGHT)
    {
        return;
    }

    LcdDraw_Pixel((U16)(MQE_LCD_WIDTH - 1 - x), (U16)(MQE_LCD_HEIGHT - 1 - y), color);
}

static void MQE_DrawTextLine(S16 xPos, U16 yPos)
{
    const sFONT* font = &Ft75_HanSansBold_KOR;
    S16 penX = xPos;
    const U08* pText = (const U08*)lScMqe.text;
    U08 ch;
    U16 idx;
    U16 glyphWidth;
    U16 glyphHeight;
    U32 glyphTablePos;
    const U08* glyph;
    U16 colStart;
    U16 colEnd;
    U16 col;
    U16 row;
    S16 glyphStart;
    S16 glyphEnd;
    S16 px;
    S16 py;

    // Skip if completely off screen (left or right)
    if (xPos >= MQE_LCD_WIDTH || xPos + (S16)lScMqe.textWidth <= 0)
    {
        return;
    }

    while ((ch = *pText++) != '\0')
    {
        idx = MQE_GetCharIndex(ch);
        glyphWidth = (U16)font->info[idx].Width;
        glyphHeight = (U16)font->info[idx].Height;
        glyphTablePos = (U32)font->info[idx].pt;
        glyph = &font->table[glyphTablePos];

        glyphStart = penX;
        glyphEnd = penX + (S16)glyphWidth;

        if (glyphEnd > 0 && glyphStart < MQE_LCD_WIDTH)
        {
            colStart = (glyphStart < 0) ? (U16)(0 - glyphStart) : 0;
            colEnd = glyphWidth;
            if (glyphEnd > MQE_LCD_WIDTH)
            {
                S16 visible = (S16)MQE_LCD_WIDTH - glyphStart;
                if (visible < 0)
                {
                    visible = 0;
                }
                if ((U16)visible < colEnd)
                {
                    colEnd = (U16)visible;
                }
            }
            if (colStart >= colEnd)
            {
                penX += (S16)glyphWidth + MQE_CHAR_SPACE;
                continue;
            }

            for (row = 0; row < glyphHeight; row++)
            {
                py = (S16)yPos + (S16)row;
                if (py >= MQE_LCD_HEIGHT)
                {
                    break;
                }

                for (col = colStart; col < colEnd; col++)
                {
                    if (MQE_IsGlyphPixelOn(glyph, glyphWidth, col, row))
                    {
                        px = glyphStart + (S16)col;
                        MQE_DrawPixelClipped(px, py, lScMqe.fgColor);
                    }
                }
            }
        }

        penX += (S16)glyphWidth + MQE_CHAR_SPACE;
        if (penX >= MQE_LCD_WIDTH && glyphStart >= MQE_LCD_WIDTH)
        {
            break;
        }
    }
}

static void MQE_DrawScreen(void)
{
    S16 wrapDistance = (S16)lScMqe.textWidth + MQE_WRAP_GAP;
    S16 xPos1 = lScMqe.xPos;
    S16 xPos2 = lScMqe.xPos - wrapDistance;

    // Clear screen with background color
    LCD_ClearScrn(lScMqe.bgColor);

    // === 75pt KOR font ===
    MQE_DrawTextLine(xPos1, 165);
    MQE_DrawTextLine(xPos2, 165);
}

static U08 MQE_UpdatePosition(void)
{
    U32 currentTick = HAL_GetTick();
    S16 wrapStart = 0 - (S16)lScMqe.textWidth;

    if ((currentTick - lScMqe.lastUpdate) < MQE_FRAME_DELAY)
    {
        return FALSE;
    }

    // Move from left to right
    lScMqe.xPos += lMqe_scrollSpeed;

    // Wrap when text exits right edge
    if (lScMqe.xPos >= MQE_LCD_WIDTH)
    {
        lScMqe.xPos = wrapStart;
    }

    lScMqe.lastUpdate = currentTick;
    return TRUE;
}

static void MQE_HandleKeyInput(void)
{
    U08 keyIdx = InKEY_GetIdx();
    U08 keyEvt = InKEY_GetEvt();

    // Exit marquee mode on any key press (short press)
    if (keyEvt == KEY_EVT_SHORT)
    {
        if (keyIdx == KEY_IDX_MENU || keyIdx == KEY_IDX_ENTER ||
            keyIdx == KEY_IDX_PREV || keyIdx == KEY_IDX_NEXT)
        {
            ScMQE_Stop();
            // Return to previous screen (FRONT)
            SCRN_SetPage(SCRN_P0_FRONT);
        }
    }    
}


//------------------------------------------------------------------------------------------------------------------------------
//  Global APIs - Get Local Variables
//------------------------------------------------------------------------------------------------------------------------------
U08 ScMQE_IsRunning(void)
{
    return lScMqe.isRunning;
}


//------------------------------------------------------------------------------------------------------------------------------
//  Global APIs - Set Local Variables
//------------------------------------------------------------------------------------------------------------------------------
void ScMQE_SetText(const I08* text)
{
    if (text != NULL)
    {
        strncpy(lScMqe.text, text, MQE_TEXT_MAX_LEN - 1);
        lScMqe.text[MQE_TEXT_MAX_LEN - 1] = '\0';

        // Recalculate text width
        lScMqe.textWidth = MQE_CalcTextWidth(lScMqe.text);
        lMqe_needRedraw = TRUE;
    }
}

void ScMQE_SetColors(U32 fgColor, U32 bgColor)
{
    lScMqe.fgColor = fgColor;
    lScMqe.bgColor = bgColor;
    lMqe_needRedraw = TRUE;
}

void ScMQE_SetSpeed(U08 speed)
{
    if (speed > 0 && speed <= MQE_SCROLL_SPEED)
    {
        lMqe_scrollSpeed = speed;
    }
}


//------------------------------------------------------------------------------------------------------------------------------
//  Global APIs - Operation
//------------------------------------------------------------------------------------------------------------------------------
void ScMQE_InitMain(void)
{
    memset(&lScMqe, 0, sizeof(ScMQE_LS));

    // Default settings
    strcpy(lScMqe.text, MQE_TEXT_PROMO);
    lScMqe.textWidth = MQE_CalcTextWidth(lScMqe.text);
    lScMqe.xPos = 0 - (S16)lScMqe.textWidth;  // Start off-screen left
    lScMqe.isRunning = FALSE;
    lScMqe.lastUpdate = HAL_GetTick();
    lScMqe.fgColor = _cSD_GREEN;           // Default: Green text
    lScMqe.bgColor = _cBGD_SCRN;           // Default: Screen background

    lMqe_scrollSpeed = MQE_SCROLL_SPEED;
    lMqe_needRedraw = TRUE;
}

void ScMQE_ProcMain(void)
{
    U08 moved;

    if (!lScMqe.isRunning)
    {
        return;
    }

    // Handle key input
    MQE_HandleKeyInput();
    if (!lScMqe.isRunning)
    {
        return;
    }

    // Update scroll position
    moved = MQE_UpdatePosition();

    // Draw only when needed to avoid visible tearing/flicker from full-screen clear.
    if (moved || lMqe_needRedraw)
    {
        MQE_DrawScreen();
        lMqe_needRedraw = FALSE;
    }
}

void ScMQE_Start(void)
{
    lScMqe.isRunning = TRUE;
    lScMqe.xPos = 0 - (S16)lScMqe.textWidth;  // Start off-screen left
    lScMqe.lastUpdate = HAL_GetTick();
    lMqe_needRedraw = TRUE;
}

void ScMQE_Stop(void)
{
    lScMqe.isRunning = FALSE;
}

void ScMQE_EnterMode(const I08* text)
{
    // Set text if provided
    if (text != NULL && strlen(text) > 0)
    {
        ScMQE_SetText(text);
    }

    // Start marquee animation
    ScMQE_Start();

    // Switch to marquee screen
    SCRN_SetPage(SCRN_P5_MARQUEE);
}


//------------------------------------------------------------------------------------------------------------------------------
//  EOF
//------------------------------------------------------------------------------------------------------------------------------
