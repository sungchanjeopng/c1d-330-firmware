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
// std
#include <string.h>
// core
#include "main.h"
#include "fmc.h"
#include "gImg_menu_fact_rel_80x124_ARGB8888.h"
#include "gImg_menu_fact_sel_80x124_ARGB8888.h"
#include "gImg_menu_opti_rel_80x124_ARGB8888.h"
#include "gImg_menu_opti_sel_80x124_ARGB8888.h"
// bsp
#include "bsp_lcd.h"
// app
#include "app_string.h"
// self
#include "bsp_nand.h"

#include "disp_color.h"
#include "bsp_demux.h"
#include "bsp_timer.h"

#include "disp_figure.h"
#include "disp_string.h"

#include "measure_calc.h"
#include "bsp_ram.h"
#include "app_main.h"
#include "menu_measure.h"

#include "bsp_uart.h"
#include "menu_test.h"
#include "menu_data.h"
#include "screen_echo.h"
//------------------------------------------------------------------------------------------------------------------------------
//  Global variables
//------------------------------------------------------------------------------------------------------------------------------


//------------------------------------------------------------------------------------------------------------------------------
//  Local variables
//------------------------------------------------------------------------------------------------------------------------------
NAND_IDTypeDef NAND_ID;
NAND_AddressTypeDef WriteReadAddr;

U16 nand_cnt = 0;
U16 nand_cnt2 = 0;
U16 nand_cnt3 =0;

U32 nand_cnt4 = 0;
U32 nand_cnt5=0;
U16 nand_cnt6 = 0;
U16 nand_cnt7 = 0;
U16 nand_cnt8 = 0;


U32 nand_cnt9=0;
U16 nand_cnt10 = 0;
U16 nand_cnt11 = 0;
U16 nand_cnt12 = 0;

	
U08 nand_year = 0;
U08 nand_mon = 0;
U08 nand_day = 0;
U08 nand_min = 0;
U08 nand_hour = 0;
U08 nand_sec = 0;

U08 nand_year2 = 0;
U08 nand_mon2 = 0;
U08 nand_day2 = 0;
U08 nand_min2 = 0;
U08 nand_hour2 = 0;
U08 nand_sec2=0;

U16 nand_echo_block[APP_CH_NUM];
U16 nand_echo_page[APP_CH_NUM];
U16 nand_meta_block[APP_CH_NUM];
U16 nand_meta_page[APP_CH_NUM];
U16 nand_meta_buffer[APP_CH_NUM];

U16 nand_meta_calib_block[APP_CH_NUM];
U16 nand_meta_calib_page[APP_CH_NUM];

//U16 nand_meta_buffer_main[APP_CH_NUM];

U16 nand_echo_num[APP_CH_NUM];
U16 nand_echo_cnt[APP_CH_NUM];

U16 nand_auto_calib_num[APP_CH_NUM];
U16 nand_auto_calib_cnt[APP_CH_NUM];
U16 nand_auto_calib_sel[APP_CH_NUM];
U16 nand_auto_calib_sel_cnt[APP_CH_NUM];

U16 nand_auto_calib_block[APP_CH_NUM];
U16 nand_auto_calib_page[APP_CH_NUM];

U08 nand_meta_main[APP_CH_NUM][20000][8];


U08 fnand = 0;
//------------------------------------------------------------------------------------------------------------------------------
//  Local Funtions
//------------------------------------------------------------------------------------------------------------------------------
NND_Echo nnd_echo;


//------------------------------------------------------------------------------------------------------------------------------
//  Global APIs - Access Local Variables
//------------------------------------------------------------------------------------------------------------------------------
// Get
U16 NND_Meta_Getmain(U08 ch, U16 cnt, U08 idx)			{	return nand_meta_main[ch][cnt][idx];		}
U16 NND_Meta_GetEchocnt(U08 ch)	{	return nand_echo_cnt[ch];	}
U16 NND_Meta_GetEchonum(U08 ch)	{	return nand_echo_num[ch];	}

// Set


//------------------------------------------------------------------------------------------------------------------------------
//  Global APIs - Operation
//------------------------------------------------------------------------------------------------------------------------------


void NND_Delete_Main(U08 ch)
{
	nand_echo_block[ch] = 0+(200*ch);
	nand_echo_page[ch] = 0;
	nand_meta_block[ch] = 191+(200*ch);
	nand_meta_page[ch] = 0;
	nand_meta_buffer[ch] = 0;
	nand_echo_num[ch] = 0;
	nand_auto_calib_num[ch] = 0;
	nand_auto_calib_cnt[ch] = 0;
	nand_auto_calib_block[ch] = 400+(24*ch);
	nand_auto_calib_page[ch] = 0;
	nand_meta_calib_block[ch] = 451+(10*ch);
	nand_meta_calib_page[ch] = 0;

	nnd_echo.save_interval[ch] = 0;

	if(ch ==APP_CH_1)
	{
		MRM_WrWord(_mNAND_CH1_ECHO_BLOCK_L,nand_echo_block[APP_CH_1]);
		MRM_WrByte(_mNAND_CH1_ECHO_PAGE,nand_echo_page[APP_CH_1]);
		
		MRM_WrWord(_mNAND_CH1_META_BLOCK_L,nand_meta_block[APP_CH_1]);
		MRM_WrByte(_mNAND_CH1_META_PAGE,nand_meta_page[APP_CH_1]);
		MRM_WrWord(_mNAND_CH1_META_BUFFER_L, nand_meta_buffer[APP_CH_1]);

		MRM_WrWord(_mNAND_CH1_ECHO_NUM_L, nand_echo_num[APP_CH_1]);		

		MRM_WrWord(_mNAND_CH1_auto_calib_NUM_L,nand_auto_calib_num[APP_CH_1]);
		MRM_WrWord(_mNAND_CH1_auto_calib_CNT_L,nand_auto_calib_num[APP_CH_1]);
		MRM_WrWord(_mNAND_CH1_AUTO_BLOCK_L,nand_auto_calib_block[APP_CH_1]);
		MRM_WrByte(_mNAND_CH1_AUTO_PAGE,nand_auto_calib_page[APP_CH_1]);
		MRM_WrWord(_mNAND_CH1_CALIB_META_BLOCK_L,nand_meta_calib_block[APP_CH_1]);
		MRM_WrByte(_mNAND_CH1_CALIB_META_PAGE,nand_meta_calib_page[APP_CH_1]);
	}

	if(ch ==APP_CH_2)
	{
		MRM_WrWord(_mNAND_CH2_ECHO_BLOCK_L,nand_echo_block[APP_CH_2]);
		MRM_WrByte(_mNAND_CH2_ECHO_PAGE,nand_echo_page[APP_CH_2]);

		MRM_WrWord(_mNAND_CH2_META_BLOCK_L,nand_meta_block[APP_CH_2]);
		MRM_WrByte(_mNAND_CH2_META_PAGE,nand_meta_page[APP_CH_2]);
		MRM_WrWord(_mNAND_CH2_META_BUFFER_L, nand_meta_buffer[APP_CH_2]);		

		MRM_WrWord(_mNAND_CH2_ECHO_NUM_L, nand_echo_num[APP_CH_2]);		
		
		MRM_WrWord(_mNAND_CH2_auto_calib_NUM_L,nand_auto_calib_num[APP_CH_2]);
		MRM_WrWord(_mNAND_CH2_auto_calib_CNT_L,nand_auto_calib_num[APP_CH_2]);
		MRM_WrWord(_mNAND_CH2_AUTO_BLOCK_L,nand_auto_calib_block[APP_CH_2]);
		MRM_WrByte(_mNAND_CH2_AUTO_PAGE,nand_auto_calib_page[APP_CH_2]);

		MRM_WrWord(_mNAND_CH2_CALIB_META_BLOCK_L,nand_meta_calib_block[APP_CH_2]);
		MRM_WrByte(_mNAND_CH2_CALIB_META_PAGE,nand_meta_calib_page[APP_CH_2]);	
	}

}
void NND_InitMain(void)
{
	HAL_NAND_Reset(&hnand1);

	nand_echo_block[APP_CH_1] = MRM_RdWord(_mNAND_CH1_ECHO_BLOCK_L);
	nand_echo_block[APP_CH_2] = MRM_RdWord(_mNAND_CH2_ECHO_BLOCK_L);
	nand_echo_page[APP_CH_1] = MRM_RdByte(_mNAND_CH1_ECHO_PAGE);
	nand_echo_page[APP_CH_2] = MRM_RdByte(_mNAND_CH2_ECHO_PAGE);	
	nand_meta_block[APP_CH_1] = MRM_RdWord(_mNAND_CH1_META_BLOCK_L);
	nand_meta_block[APP_CH_2] = MRM_RdWord(_mNAND_CH2_META_BLOCK_L);		
	nand_meta_page[APP_CH_1] = MRM_RdByte(_mNAND_CH1_META_PAGE);
	nand_meta_page[APP_CH_2] = MRM_RdByte(_mNAND_CH2_META_PAGE);	
	nand_meta_buffer[APP_CH_1] = MRM_RdWord(_mNAND_CH1_META_BUFFER_L);
	nand_meta_buffer[APP_CH_2] = MRM_RdWord(_mNAND_CH2_META_BUFFER_L);	

	nand_echo_num[APP_CH_1] = MRM_RdWord(_mNAND_CH1_ECHO_NUM_L);
	nand_echo_num[APP_CH_2] = MRM_RdWord(_mNAND_CH2_ECHO_NUM_L);
	nand_echo_cnt[APP_CH_1] = 0;
	nand_echo_cnt[APP_CH_2] = 0;

	nand_auto_calib_num[APP_CH_1] = MRM_RdWord(_mNAND_CH1_auto_calib_NUM_L);
	nand_auto_calib_num[APP_CH_2] = MRM_RdWord(_mNAND_CH2_auto_calib_NUM_L);
	
	nand_auto_calib_cnt[APP_CH_1] = MRM_RdWord(_mNAND_CH1_auto_calib_CNT_L);
	nand_auto_calib_cnt[APP_CH_2] = MRM_RdWord(_mNAND_CH2_auto_calib_CNT_L);
	
	nand_auto_calib_block[APP_CH_1] = MRM_RdWord(_mNAND_CH1_AUTO_BLOCK_L);
	nand_auto_calib_block[APP_CH_2] = MRM_RdWord(_mNAND_CH2_AUTO_BLOCK_L);

	nand_auto_calib_page[APP_CH_1] = MRM_RdByte(_mNAND_CH1_AUTO_PAGE);
	nand_auto_calib_page[APP_CH_2] = MRM_RdByte(_mNAND_CH2_AUTO_PAGE);

	nand_meta_calib_block[APP_CH_1] = MRM_RdWord(_mNAND_CH1_CALIB_META_BLOCK_L);
	nand_meta_calib_block[APP_CH_2] = MRM_RdWord(_mNAND_CH2_CALIB_META_BLOCK_L);		
	nand_meta_calib_page[APP_CH_1] = MRM_RdByte(_mNAND_CH1_CALIB_META_PAGE);
	nand_meta_calib_page[APP_CH_2] = MRM_RdByte(_mNAND_CH2_CALIB_META_PAGE);	

	if(nand_echo_block[APP_CH_1]>200)
	{
		NND_Delete_Main(APP_CH_1);
		NND_Delete_Main(APP_CH_2);
	}

	if(nand_echo_block[APP_CH_2]>500)
	{
		NND_Delete_Main(APP_CH_1);
		NND_Delete_Main(APP_CH_2);
	}

	if(nand_meta_block[APP_CH_1]>200)
	{
		NND_Delete_Main(APP_CH_1);
		NND_Delete_Main(APP_CH_2);
	}

	if(nand_meta_block[APP_CH_2]>500)
	{
		NND_Delete_Main(APP_CH_1);
		NND_Delete_Main(APP_CH_2);
	}



	nnd_echo.save_interval[APP_CH_1] = 0;
	nnd_echo.save_interval[APP_CH_2] = 0;
	//NND_Delete_Main();
}

HAL_StatusTypeDef BSP_NAND_Read_Page_8b(NAND_HandleTypeDef *hnand, 
                                        NAND_AddressTypeDef *pAddress, 
                                        uint8_t *pBuffer, 
                                        uint32_t NumPageToRead)
{
    __IO uint32_t index = 0;
    uint32_t tickstart = 0;
    uint32_t deviceAddress = NAND_DEVICE;
    uint32_t size = 0;
    uint32_t nandAddress = ARRAY_ADDRESS(pAddress, hnand);
    GPIO_PinState rb_status;

    /* Process Locked */
    __HAL_LOCK(hnand); 

    /* Check the NAND controller state */
    if(hnand->State == HAL_NAND_STATE_BUSY)
    {
        __HAL_UNLOCK(hnand);
        return HAL_BUSY;
    }

    /* Update the NAND controller state */ 
    hnand->State = HAL_NAND_STATE_BUSY;

    /* Page(s) read loop */  
    while((NumPageToRead != 0) && 
          (nandAddress < ((hnand->Config.BlockSize) * (hnand->Config.BlockNbr))))
    {	   
        /* Calculate buffer size */
        size = hnand->Config.PageSize;

        /* Send read page command sequence */
        *(__IO uint8_t *)(deviceAddress + CMD_AREA) = NAND_CMD_AREA_A;
        __DSB();

        /* Address cycles */
        if(hnand->Config.PageSize <= 512)
        {
            if ((hnand->Config.BlockSize * hnand->Config.BlockNbr) <= 65535)
            {
                *(__IO uint8_t *)(deviceAddress + ADDR_AREA) = 0x00;
                __DSB();
                *(__IO uint8_t *)(deviceAddress + ADDR_AREA) = ADDR_1ST_CYCLE(nandAddress);
                __DSB();
                *(__IO uint8_t *)(deviceAddress + ADDR_AREA) = ADDR_2ND_CYCLE(nandAddress);
                __DSB();
            }
            else /* ((hnand->Config.BlockSize)*(hnand->Config.BlockNbr)) > 65535 */
            {
                *(__IO uint8_t *)(deviceAddress + ADDR_AREA) = 0x00;
                __DSB();
                *(__IO uint8_t *)(deviceAddress + ADDR_AREA) = ADDR_1ST_CYCLE(nandAddress);
                __DSB();
                *(__IO uint8_t *)(deviceAddress + ADDR_AREA) = ADDR_2ND_CYCLE(nandAddress);
                __DSB();
                *(__IO uint8_t *)(deviceAddress + ADDR_AREA) = ADDR_3RD_CYCLE(nandAddress);
                __DSB();
            }
        }
        else /* (hnand->Config.PageSize) > 512 */
        {
            if ((hnand->Config.BlockSize * hnand->Config.BlockNbr) <= 65535)
            {
                *(__IO uint8_t *)(deviceAddress + ADDR_AREA) = 0x00;
                __DSB();
                *(__IO uint8_t *)(deviceAddress + ADDR_AREA) = 0x00;
                __DSB();
                *(__IO uint8_t *)(deviceAddress + ADDR_AREA) = ADDR_1ST_CYCLE(nandAddress);
                __DSB();
                *(__IO uint8_t *)(deviceAddress + ADDR_AREA) = ADDR_2ND_CYCLE(nandAddress);
                __DSB();
            }
            else /* ((hnand->Config.BlockSize)*(hnand->Config.BlockNbr)) > 65535 */
            {
                *(__IO uint8_t *)(deviceAddress + ADDR_AREA) = 0x00;
                __DSB();
                *(__IO uint8_t *)(deviceAddress + ADDR_AREA) = 0x00;
                __DSB();
                *(__IO uint8_t *)(deviceAddress + ADDR_AREA) = ADDR_1ST_CYCLE(nandAddress);
                __DSB();
                *(__IO uint8_t *)(deviceAddress + ADDR_AREA) = ADDR_2ND_CYCLE(nandAddress);
                __DSB();
                *(__IO uint8_t *)(deviceAddress + ADDR_AREA) = ADDR_3RD_CYCLE(nandAddress);
                __DSB();
            }
        }

        /* Send read true1 command */
        *(__IO uint8_t *)(deviceAddress + CMD_AREA) = NAND_CMD_AREA_TRUE1;
        __DSB();

        /* Handle ExtraCommand if enabled */
        if(hnand->Config.ExtraCommandEnable == ENABLE)
        {
            /* Get tick */
            tickstart = HAL_GetTick();
            
            /* Read status until NAND is ready */
            while(HAL_NAND_Read_Status(hnand) != NAND_READY)
            {
                if((HAL_GetTick() - tickstart ) > 5000)
                {
                    /* Timeout occurred */
                    hnand->State = HAL_NAND_STATE_READY;
                    __HAL_UNLOCK(hnand);
                    return HAL_TIMEOUT; 
                }
            }
            
            /* Go back to read mode */
            *(__IO uint8_t *)(deviceAddress + CMD_AREA) = 0x00U;
            __DSB();
        }
        else
        {
            /* ExtraCommandEnable이 비활성화된 경우, R/B 핀을 폴링하여 준비 상태를 확인 */
            tickstart = HAL_GetTick();
            while(1)
            {
                /* Read the R/B pin */
                rb_status = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_6);
                
                if (rb_status == GPIO_PIN_SET) // R/B 핀이 High면 Ready 상태
                {
                    break;
                }

                /* Check for timeout */
                if ((HAL_GetTick() - tickstart) > 5000)
                {
                    hnand->State = HAL_NAND_STATE_READY;
                    __HAL_UNLOCK(hnand);
                    return HAL_TIMEOUT; 
                }
            }
        }

        /* Get Data into Buffer */    
        for(index = 0; index < size; index++)
        {
            *(uint8_t *)pBuffer++ = *(uint8_t *)deviceAddress;
            // __DSB(); // 필요한 경우에만 사용
        }

        /* Decrement pages to read */
        NumPageToRead--;

        /* Increment the NAND address */
        nandAddress++;

        HAL_Delay(2); // 필요에 따라 조정
    }

    /* Update the NAND controller state */ 
    hnand->State = HAL_NAND_STATE_READY;

    /* Process unlocked */
    __HAL_UNLOCK(hnand);  

    return HAL_OK;
}


/**
  * @brief  Read Page(s) from NAND memory block (16-bits addressing)
  * @param  hnand pointer to a NAND_HandleTypeDef structure that contains
  *                the configuration information for NAND module.
  * @param  pAddress  pointer to NAND address structure
  * @param  pBuffer  pointer to destination read buffer. pBuffer should be 16bits aligned
  * @param  NumPageToRead  number of pages to read from block 
  * @retval HAL status
  */
#if 0
HAL_StatusTypeDef BSP_NAND_Read_Page_16b(NAND_HandleTypeDef *hnand, NAND_AddressTypeDef *pAddress, uint16_t *pBuffer, uint32_t NumPageToRead)
{   
  __IO uint32_t index  = 0;
  uint32_t tickstart = 0;
  uint32_t deviceAddress = 0, size = 0, numPagesRead = 0, nandAddress = 0;
  
  /* Process Locked */
  __HAL_LOCK(hnand); 
  
  /* Check the NAND controller state */
  if(hnand->State == HAL_NAND_STATE_BUSY)
  {
     return HAL_BUSY;
  }
  
  /* Identify the device address */
  deviceAddress = NAND_DEVICE;

  /* Update the NAND controller state */ 
  hnand->State = HAL_NAND_STATE_BUSY;
  
  /* NAND raw address calculation */
  nandAddress = ARRAY_ADDRESS(pAddress, hnand);
  
  /* Page(s) read loop */  
  while((NumPageToRead != 0) && (nandAddress < ((hnand->Config.BlockSize) * (hnand->Config.BlockNbr))))
  {	   
    /* update the buffer size */
    size = (hnand->Config.PageSize) + ((hnand->Config.PageSize) * numPagesRead);
    
    /* Send read page command sequence */
    *(__IO uint8_t *)((uint32_t)(deviceAddress | CMD_AREA)) = NAND_CMD_AREA_A;  
    __DSB();
    
    /* Cards with page size <= 512 bytes */
    if((hnand->Config.PageSize) <= 512)
    {
      if (((hnand->Config.BlockSize)*(hnand->Config.BlockNbr)) <= 65535)
      {
        *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = 0x00;
        __DSB();
        *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = ADDR_1ST_CYCLE(nandAddress);
        __DSB();
        *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = ADDR_2ND_CYCLE(nandAddress);
        __DSB();
      }
      else /* ((hnand->Config.BlockSize)*(hnand->Config.BlockNbr)) > 65535 */
      {
        *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = 0x00;
        __DSB();
        *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = ADDR_1ST_CYCLE(nandAddress);
        __DSB();
        *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = ADDR_2ND_CYCLE(nandAddress);
        __DSB();
        *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = ADDR_3RD_CYCLE(nandAddress);
        __DSB();
      }
    }
    else /* (hnand->Config.PageSize) > 512 */
    {
      if (((hnand->Config.BlockSize)*(hnand->Config.BlockNbr)) <= 65535)
      {
        *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = 0x00;
        __DSB();
        *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = 0x00;
        __DSB();
        *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = ADDR_1ST_CYCLE(nandAddress);
        __DSB();
        *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = ADDR_2ND_CYCLE(nandAddress);
        __DSB();
      }
      else /* ((hnand->Config.BlockSize)*(hnand->Config.BlockNbr)) > 65535 */
      {
        *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = 0x00;
        __DSB();
        *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = 0x00;
        __DSB();
        *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = ADDR_1ST_CYCLE(nandAddress);
        __DSB();
        *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = ADDR_2ND_CYCLE(nandAddress);
        __DSB();
        *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = ADDR_3RD_CYCLE(nandAddress);
        __DSB();
      }
    }
  
    *(__IO uint8_t *)((uint32_t)(deviceAddress | CMD_AREA))  = NAND_CMD_AREA_TRUE1;
    __DSB();
    
    if(hnand->Config.ExtraCommandEnable == ENABLE)
    {
      /* Get tick */
      tickstart = HAL_GetTick();
      
      /* Read status until NAND is ready */
      while(HAL_NAND_Read_Status(hnand) != NAND_READY)
      {
        if((HAL_GetTick() - tickstart ) > NAND_WRITE_TIMEOUT)
        {
          return HAL_TIMEOUT; 
        }
      }
      
      /* Go back to read mode */
      *(__IO uint8_t *)((uint32_t)(deviceAddress | CMD_AREA)) = ((uint8_t)0x00U);
      __DSB();
    }

    /* Calculate PageSize */
    if (hnand->Init.MemoryDataWidth == FMC_NAND_PCC_MEM_BUS_WIDTH_8)
    {
      size = size / 2U;
    }
    else
    {
      /* Do nothing */
      /* Keep the same PageSize for FMC_NAND_MEM_BUS_WIDTH_16*/
    }

    /* Get Data into Buffer */    
    for(; index < size; index++)
    {
      *(uint16_t *)pBuffer++ = *(uint16_t *)deviceAddress;
    }
    
    /* Increment read pages number */
    numPagesRead++;
    
    /* Decrement pages to read */
    NumPageToRead--;
    
    /* Increment the NAND address */
    nandAddress = (uint32_t)(nandAddress + 1);

	HAL_Delay(2);
  }
  
  /* Update the NAND controller state */ 
  hnand->State = HAL_NAND_STATE_READY;
  
  /* Process unlocked */
  __HAL_UNLOCK(hnand);  
    
  return HAL_OK;
}
#else
HAL_StatusTypeDef BSP_NAND_Read_Page_16b(NAND_HandleTypeDef *hnand, 
                                         NAND_AddressTypeDef *pAddress, 
                                         uint16_t *pBuffer, 
                                         uint32_t NumPageToRead)
{   
    __IO uint32_t index = 0;
    uint32_t tickstart = 0;
    uint32_t deviceAddress = NAND_DEVICE;
    uint32_t size = 0;
    uint32_t nandAddress = ARRAY_ADDRESS(pAddress, hnand);
    GPIO_PinState rb_status;

    /* Process Locked */
    __HAL_LOCK(hnand); 

    /* Check the NAND controller state */
    if(hnand->State == HAL_NAND_STATE_BUSY)
    {
        __HAL_UNLOCK(hnand);
        return HAL_BUSY;
    }

    /* Update the NAND controller state */ 
    hnand->State = HAL_NAND_STATE_BUSY;

    /* Page(s) read loop */  
    while((NumPageToRead != 0) && 
          (nandAddress < ((hnand->Config.BlockSize) * (hnand->Config.BlockNbr))))
    {	   
        /* Calculate buffer size */
        size = hnand->Config.PageSize;

        /* Send read page command sequence */
        *(__IO uint8_t *)(deviceAddress + CMD_AREA) = NAND_CMD_AREA_A;
        __DSB();

        /* Address cycles */
        if(hnand->Config.PageSize <= 512)
        {
            if ((hnand->Config.BlockSize * hnand->Config.BlockNbr) <= 65535)
            {
                *(__IO uint8_t *)(deviceAddress + ADDR_AREA) = 0x00;
                __DSB();
                *(__IO uint8_t *)(deviceAddress + ADDR_AREA) = ADDR_1ST_CYCLE(nandAddress);
                __DSB();
                *(__IO uint8_t *)(deviceAddress + ADDR_AREA) = ADDR_2ND_CYCLE(nandAddress);
                __DSB();
            }
            else /* ((hnand->Config.BlockSize)*(hnand->Config.BlockNbr)) > 65535 */
            {
                *(__IO uint8_t *)(deviceAddress + ADDR_AREA) = 0x00;
                __DSB();
                *(__IO uint8_t *)(deviceAddress + ADDR_AREA) = ADDR_1ST_CYCLE(nandAddress);
                __DSB();
                *(__IO uint8_t *)(deviceAddress + ADDR_AREA) = ADDR_2ND_CYCLE(nandAddress);
                __DSB();
                *(__IO uint8_t *)(deviceAddress + ADDR_AREA) = ADDR_3RD_CYCLE(nandAddress);
                __DSB();
            }
        }
        else /* (hnand->Config.PageSize) > 512 */
        {
            if ((hnand->Config.BlockSize * hnand->Config.BlockNbr) <= 65535)
            {
                *(__IO uint8_t *)(deviceAddress + ADDR_AREA) = 0x00;
                __DSB();
                *(__IO uint8_t *)(deviceAddress + ADDR_AREA) = 0x00;
                __DSB();
                *(__IO uint8_t *)(deviceAddress + ADDR_AREA) = ADDR_1ST_CYCLE(nandAddress);
                __DSB();
                *(__IO uint8_t *)(deviceAddress + ADDR_AREA) = ADDR_2ND_CYCLE(nandAddress);
                __DSB();
            }
            else /* ((hnand->Config.BlockSize)*(hnand->Config.BlockNbr)) > 65535 */
            {
                *(__IO uint8_t *)(deviceAddress + ADDR_AREA) = 0x00;
                __DSB();
                *(__IO uint8_t *)(deviceAddress + ADDR_AREA) = 0x00;
                __DSB();
                *(__IO uint8_t *)(deviceAddress + ADDR_AREA) = ADDR_1ST_CYCLE(nandAddress);
                __DSB();
                *(__IO uint8_t *)(deviceAddress + ADDR_AREA) = ADDR_2ND_CYCLE(nandAddress);
                __DSB();
                *(__IO uint8_t *)(deviceAddress + ADDR_AREA) = ADDR_3RD_CYCLE(nandAddress);
                __DSB();
            }
        }

        /* Send read true1 command */
        *(__IO uint8_t *)(deviceAddress + CMD_AREA) = NAND_CMD_AREA_TRUE1;
        __DSB();

        /* Handle ExtraCommand if enabled */
        if(hnand->Config.ExtraCommandEnable == ENABLE)
        {
            /* Get tick */
            tickstart = HAL_GetTick();
            
            /* Read status until NAND is ready */
            while(HAL_NAND_Read_Status(hnand) != NAND_READY)
            {
                if((HAL_GetTick() - tickstart ) > 5000)
                {
                    /* Timeout occurred */
                    hnand->State = HAL_NAND_STATE_READY;
                    __HAL_UNLOCK(hnand);
                    return HAL_TIMEOUT; 
                }
            }
            
            /* Go back to read mode */
            *(__IO uint8_t *)(deviceAddress + CMD_AREA) = 0x00U;
            __DSB();
        }
        else
        {
            /* ExtraCommandEnable이 비활성화된 경우, R/B 핀을 폴링하여 준비 상태를 확인 */
            tickstart = HAL_GetTick();
            while(1)
            {
                /* Read the R/B pin */
                rb_status = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_6);
                
                if (rb_status == GPIO_PIN_SET) // R/B 핀이 High면 Ready 상태
                {
                    break;
                }

                /* Check for timeout */
                if ((HAL_GetTick() - tickstart) > 5000)
                {
                    hnand->State = HAL_NAND_STATE_READY;
                    __HAL_UNLOCK(hnand);
                    return HAL_TIMEOUT; 
                }
            }
        }

        /* Adjust size for 8-bit bus width */
        if (hnand->Init.MemoryDataWidth == FMC_NAND_PCC_MEM_BUS_WIDTH_8)
        {
            size = size / 2U;
        }
        /* No change for 16-bit bus width */

        /* Get Data into Buffer */    
        for(index = 0; index < size; index++)
        {
            *(uint16_t *)pBuffer++ = *(__IO uint16_t *)(deviceAddress);
            // __DSB(); // 필요한 경우에만 사용
        }

        /* Decrement pages to read */
        NumPageToRead--;

        /* Increment the NAND address */
        nandAddress++;

        HAL_Delay(2); // 필요에 따라 조정
    }

    /* Update the NAND controller state */ 
    hnand->State = HAL_NAND_STATE_READY;

    /* Process unlocked */
    __HAL_UNLOCK(hnand);  

    return HAL_OK;
}


#endif


/**
  * @brief  Write Page(s) to NAND memory block (8-bits addressing)
  * @param  hnand pointer to a NAND_HandleTypeDef structure that contains
  *                the configuration information for NAND module.
  * @param  pAddress  pointer to NAND address structure
  * @param  pBuffer  pointer to source buffer to write  
  * @param  NumPageToWrite   number of pages to write to block 
  * @retval HAL status
  */
HAL_StatusTypeDef BSP_NAND_Write_Page_8b(NAND_HandleTypeDef *hnand, 
                                         NAND_AddressTypeDef *pAddress, 
                                         uint8_t *pBuffer, 
                                         uint32_t NumPageToWrite)
{
    __IO uint32_t index;
    uint32_t tickstart;
    uint32_t deviceAddress = NAND_DEVICE;
    uint32_t size;
    uint32_t nandAddress = ARRAY_ADDRESS(pAddress, hnand);
    GPIO_PinState rb_status;

    /* Process Locked */
    __HAL_LOCK(hnand);  

    /* Check the NAND controller state */
    if(hnand->State == HAL_NAND_STATE_BUSY)
    {
        __HAL_UNLOCK(hnand);
        return HAL_BUSY;
    }

    /* Update the NAND controller state */ 
    hnand->State = HAL_NAND_STATE_BUSY;

    /* Page(s) write loop */
    while((NumPageToWrite != 0) && 
          (nandAddress < ((hnand->Config.BlockNbr) * (hnand->Config.BlockSize))))
    {
        /* Reset index for each page write */
        index = 0;

        /* Calculate buffer size */
        size = hnand->Config.PageSize;

        /* Send write page command sequence */
        *(__IO uint8_t *)(deviceAddress + CMD_AREA) = NAND_CMD_AREA_A;
        __DSB();
        *(__IO uint8_t *)(deviceAddress + CMD_AREA) = NAND_CMD_WRITE0;
        __DSB();

        /* Address cycles */
        if(hnand->Config.PageSize <= 512)
        {
            if ((hnand->Config.BlockSize * hnand->Config.BlockNbr) <= 65535)
            {
                *(__IO uint8_t *)(deviceAddress + ADDR_AREA) = 0x00;
                __DSB();
                *(__IO uint8_t *)(deviceAddress + ADDR_AREA) = ADDR_1ST_CYCLE(nandAddress);
                __DSB();
                *(__IO uint8_t *)(deviceAddress + ADDR_AREA) = ADDR_2ND_CYCLE(nandAddress);
                __DSB();
            }
            else
            {
                *(__IO uint8_t *)(deviceAddress + ADDR_AREA) = 0x00;
                __DSB();
                *(__IO uint8_t *)(deviceAddress + ADDR_AREA) = ADDR_1ST_CYCLE(nandAddress);
                __DSB();
                *(__IO uint8_t *)(deviceAddress + ADDR_AREA) = ADDR_2ND_CYCLE(nandAddress);
                __DSB();
                *(__IO uint8_t *)(deviceAddress + ADDR_AREA) = ADDR_3RD_CYCLE(nandAddress);
                __DSB();
            }
        }
        else
        {
            if ((hnand->Config.BlockSize * hnand->Config.BlockNbr) <= 65535)
            {
                *(__IO uint8_t *)(deviceAddress + ADDR_AREA) = 0x00;
                __DSB();
                *(__IO uint8_t *)(deviceAddress + ADDR_AREA) = 0x00;
                __DSB();
                *(__IO uint8_t *)(deviceAddress + ADDR_AREA) = ADDR_1ST_CYCLE(nandAddress);
                __DSB();
                *(__IO uint8_t *)(deviceAddress + ADDR_AREA) = ADDR_2ND_CYCLE(nandAddress);
                __DSB();
            }
            else
            {
                *(__IO uint8_t *)(deviceAddress + ADDR_AREA) = 0x00;
                __DSB();
                *(__IO uint8_t *)(deviceAddress + ADDR_AREA) = 0x00;
                __DSB();
                *(__IO uint8_t *)(deviceAddress + ADDR_AREA) = ADDR_1ST_CYCLE(nandAddress);
                __DSB();
                *(__IO uint8_t *)(deviceAddress + ADDR_AREA) = ADDR_2ND_CYCLE(nandAddress);
                __DSB();
                *(__IO uint8_t *)(deviceAddress + ADDR_AREA) = ADDR_3RD_CYCLE(nandAddress);
                __DSB();
            }
        }

        /* Write data to memory */
        for(index = 0; index < size; index++)
        {
            *(__IO uint8_t *)(deviceAddress) = *(uint8_t *)pBuffer++;
            __DSB();
        }

        /* Send Write True1 command */
        *(__IO uint8_t *)(deviceAddress + CMD_AREA) = NAND_CMD_WRITE_TRUE1;
        __DSB();

        /* Get tick */
        tickstart = HAL_GetTick();

        /* Wait for the write operation to complete */
        while(1)
        {
            /* Read the R/B pin */
            rb_status = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_6);

            if (rb_status == GPIO_PIN_SET) // R/B 핀이 High면 Ready 상태
            {
                break;
            }

            /* Check for timeout */
            if ((HAL_GetTick() - tickstart) > 5000)
            {
                /* Timeout occurred */
                hnand->State = HAL_NAND_STATE_READY;
                __HAL_UNLOCK(hnand);
                return HAL_TIMEOUT; 
            }
        }

        /* Decrement pages to write */
        NumPageToWrite--;

        /* Increment the NAND address */
        nandAddress++;
    }

    /* Update the NAND controller state */ 
    hnand->State = HAL_NAND_STATE_READY;

    /* Process unlocked */
    __HAL_UNLOCK(hnand);      

    return HAL_OK;
}


/**
  * @brief  Write Page(s) to NAND memory block (16-bits addressing)
  * @param  hnand pointer to a NAND_HandleTypeDef structure that contains
  *                the configuration information for NAND module.
  * @param  pAddress  pointer to NAND address structure
  * @param  pBuffer  pointer to source buffer to write. pBuffer should be 16bits aligned
  * @param  NumPageToWrite   number of pages to write to block 
  * @retval HAL status
  */
#if 0
HAL_StatusTypeDef BSP_NAND_Write_Page_16b(NAND_HandleTypeDef *hnand, NAND_AddressTypeDef *pAddress, uint16_t *pBuffer, uint32_t NumPageToWrite)
{
  __IO uint32_t index = 0;
  uint32_t tickstart = 0;
  uint32_t deviceAddress = 0, size = 0, numPagesWritten = 0, nandAddress = 0;
  
  /* Process Locked */
  __HAL_LOCK(hnand);  

  /* Check the NAND controller state */
  if(hnand->State == HAL_NAND_STATE_BUSY)
  {
     return HAL_BUSY;
  }
  
  /* Identify the device address */
  deviceAddress = NAND_DEVICE;
  
  /* Update the NAND controller state */ 
  hnand->State = HAL_NAND_STATE_BUSY;
  
  /* NAND raw address calculation */
  nandAddress = ARRAY_ADDRESS(pAddress, hnand);
  
  /* Page(s) write loop */
  while((NumPageToWrite != 0) && (nandAddress < ((hnand->Config.BlockSize) * (hnand->Config.BlockNbr))))
  {
    /* update the buffer size */
    size = (hnand->Config.PageSize) + ((hnand->Config.PageSize) * numPagesWritten);
 
    /* Send write page command sequence */
    *(__IO uint8_t *)((uint32_t)(deviceAddress | CMD_AREA)) = NAND_CMD_AREA_A;
    __DSB();
    *(__IO uint8_t *)((uint32_t)(deviceAddress | CMD_AREA)) = NAND_CMD_WRITE0;
    __DSB();

    /* Cards with page size <= 512 bytes */
    if((hnand->Config.PageSize) <= 512)
    {
      if (((hnand->Config.BlockSize)*(hnand->Config.BlockNbr)) <= 65535)
      {
        *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = 0x00;
        __DSB();
        *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = ADDR_1ST_CYCLE(nandAddress);
        __DSB();
        *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = ADDR_2ND_CYCLE(nandAddress);
        __DSB();
      }
      else /* ((hnand->Config.BlockSize)*(hnand->Config.BlockNbr)) > 65535 */
      {
        *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = 0x00;
        __DSB();
        *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = ADDR_1ST_CYCLE(nandAddress);
        __DSB();
        *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = ADDR_2ND_CYCLE(nandAddress);
        __DSB();
        *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = ADDR_3RD_CYCLE(nandAddress);
        __DSB();
      }
    }
    else /* (hnand->Config.PageSize) > 512 */
    {
      if (((hnand->Config.BlockSize)*(hnand->Config.BlockNbr)) <= 65535)
      {
        *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = 0x00;
        __DSB();
        *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = 0x00;
        __DSB();
        *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = ADDR_1ST_CYCLE(nandAddress);
        __DSB();
        *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = ADDR_2ND_CYCLE(nandAddress);
        __DSB();
      }
      else /* ((hnand->Config.BlockSize)*(hnand->Config.BlockNbr)) > 65535 */
      {
        *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = 0x00;
        __DSB();
        *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = 0x00;
        __DSB();
        *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = ADDR_1ST_CYCLE(nandAddress);
        __DSB();
        *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = ADDR_2ND_CYCLE(nandAddress);
        __DSB();
        *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = ADDR_3RD_CYCLE(nandAddress);
        __DSB();
      }
    }

    /* Calculate PageSize */
    if (hnand->Init.MemoryDataWidth == FMC_NAND_PCC_MEM_BUS_WIDTH_8)
    {
      size = size / 2U;
    }
    else
    {
      /* Do nothing */
      /* Keep the same PageSize for FMC_NAND_MEM_BUS_WIDTH_16*/
    }

    /* Write data to memory */
    for(; index < size; index++)
    {
      *(__IO uint16_t *)deviceAddress = *(uint16_t *)pBuffer++;
      __DSB();
    }
   
    *(__IO uint8_t *)((uint32_t)(deviceAddress | CMD_AREA)) = NAND_CMD_WRITE_TRUE1;
    __DSB();
    
	/* Get tick */
    tickstart = HAL_GetTick();
	  
    /* Read status until NAND is ready */
    while(HAL_NAND_Read_Status(hnand) != NAND_READY)
    {  
      if((HAL_GetTick() - tickstart ) > NAND_WRITE_TIMEOUT)
      {
        return HAL_TIMEOUT; 
      } 
    }   
 
    /* Increment written pages number */
    numPagesWritten++;
    
    /* Decrement pages to write */
    NumPageToWrite--;
    
    /* Increment the NAND address */
    nandAddress = (uint32_t)(nandAddress + 1);
  }
  
  /* Update the NAND controller state */ 
  hnand->State = HAL_NAND_STATE_READY;
  
  /* Process unlocked */
  __HAL_UNLOCK(hnand);      
  
  return HAL_OK;
}
#else
HAL_StatusTypeDef BSP_NAND_Write_Page_16b(NAND_HandleTypeDef *hnand, 
                                         NAND_AddressTypeDef *pAddress, 
                                         uint16_t *pBuffer, 
                                         uint32_t NumPageToWrite)
{
    __IO uint32_t index;
    uint32_t tickstart;
    uint32_t deviceAddress = NAND_DEVICE;
    uint32_t size;
    uint32_t nandAddress = ARRAY_ADDRESS(pAddress, hnand);
    GPIO_PinState rb_status;

    /* Process Locked */
    __HAL_LOCK(hnand);  

    /* Check the NAND controller state */
    if(hnand->State == HAL_NAND_STATE_BUSY)
    {
        __HAL_UNLOCK(hnand);
        return HAL_BUSY;
    }

    /* Update the NAND controller state */ 
    hnand->State = HAL_NAND_STATE_BUSY;

    /* Page(s) write loop */
    while((NumPageToWrite != 0) && 
          (nandAddress < ((hnand->Config.BlockNbr) * (hnand->Config.BlockSize))))
    {
        /* Reset index for each page write */
        index = 0;

#if 0
		_SPRINTF(sDbg, "%ld %ld %ld ", nandAddress,hnand->Config.BlockNbr,hnand->Config.BlockSize);
		DpFIG_DrwRect(150, 120, 400, 25, _cBGD_SCRN, DpFIG_FILL);
		DpSTR_GuiLeft(150, 120, _cECO_LVL_TITL, _cBGD_SCRN, _fE17HsB, sDbg); 

		_SPRINTF(sDbg, "%d %d %d %d %d", ARRAY_ADDRESS_PAGE(pAddress, hnand),ARRAY_ADDRESS_BLOCK(pAddress, hnand)
			,ARRAY_ADDRESS_PLANE(pAddress, hnand),ARRAY_ADDRESS_PLANESIZE(pAddress, hnand),ARRAY_ADDRESS_BLOCKSIZE(pAddress, hnand));
		DpFIG_DrwRect(150, 150, 400, 25, _cBGD_SCRN, DpFIG_FILL);
		DpSTR_GuiLeft(150, 150, _cECO_LVL_TITL, _cBGD_SCRN, _fE17HsB, sDbg); 
#endif
        /* Calculate buffer size */
        size = hnand->Config.PageSize;

        /* Send write page command sequence */
        *(__IO uint8_t *)(deviceAddress + CMD_AREA) = NAND_CMD_AREA_A;
        __DSB();
        *(__IO uint8_t *)(deviceAddress + CMD_AREA) = NAND_CMD_WRITE0;
        __DSB();

        /* Address cycles */
        if(hnand->Config.PageSize <= 512)
        {
            if ((hnand->Config.BlockSize * hnand->Config.BlockNbr) <= 65535)
            {
                *(__IO uint8_t *)(deviceAddress + ADDR_AREA) = 0x00;
                __DSB();
                *(__IO uint8_t *)(deviceAddress + ADDR_AREA) = ADDR_1ST_CYCLE(nandAddress);
                __DSB();
                *(__IO uint8_t *)(deviceAddress + ADDR_AREA) = ADDR_2ND_CYCLE(nandAddress);
                __DSB();
            }
            else
            {
                *(__IO uint8_t *)(deviceAddress + ADDR_AREA) = 0x00;
                __DSB();
                *(__IO uint8_t *)(deviceAddress + ADDR_AREA) = ADDR_1ST_CYCLE(nandAddress);
                __DSB();
                *(__IO uint8_t *)(deviceAddress + ADDR_AREA) = ADDR_2ND_CYCLE(nandAddress);
                __DSB();
                *(__IO uint8_t *)(deviceAddress + ADDR_AREA) = ADDR_3RD_CYCLE(nandAddress);
                __DSB();
            }
        }
        else
        {
            if ((hnand->Config.BlockSize * hnand->Config.BlockNbr) <= 65535)
            {
                *(__IO uint8_t *)(deviceAddress + ADDR_AREA) = 0x00;
                __DSB();
                *(__IO uint8_t *)(deviceAddress + ADDR_AREA) = 0x00;
                __DSB();
                *(__IO uint8_t *)(deviceAddress + ADDR_AREA) = ADDR_1ST_CYCLE(nandAddress);
                __DSB();
                *(__IO uint8_t *)(deviceAddress + ADDR_AREA) = ADDR_2ND_CYCLE(nandAddress);
                __DSB();
            }
            else
            {
                *(__IO uint8_t *)(deviceAddress + ADDR_AREA) = 0x00;
                __DSB();
                *(__IO uint8_t *)(deviceAddress + ADDR_AREA) = 0x00;
                __DSB();
                *(__IO uint8_t *)(deviceAddress + ADDR_AREA) = ADDR_1ST_CYCLE(nandAddress);
                __DSB();
                *(__IO uint8_t *)(deviceAddress + ADDR_AREA) = ADDR_2ND_CYCLE(nandAddress);
                __DSB();
                *(__IO uint8_t *)(deviceAddress + ADDR_AREA) = ADDR_3RD_CYCLE(nandAddress);
                __DSB();
            }
        }

        /* Adjust size for 16-bit bus width */
        if (hnand->Init.MemoryDataWidth == FMC_NAND_PCC_MEM_BUS_WIDTH_8)
        {
            size = size / 2U;
        }
        /* No change for 16-bit bus width */

        /* Write data to memory */
        for(index = 0; index < size; index++)
        {
            *(__IO uint16_t *)(deviceAddress) = *(uint16_t *)pBuffer++;
            __DSB();
        }

#if 0
		_SPRINTF(sDbg, "%d %d",*(uint16_t *)pBuffer,*(__IO uint16_t *)(deviceAddress));
		DpFIG_DrwRect(150, 180, 400, 25, _cBGD_SCRN, DpFIG_FILL);
		DpSTR_GuiLeft(150, 180, _cECO_LVL_TITL, _cBGD_SCRN, _fE17HsB, sDbg); 
#endif
        /* Send Write True1 command */
        *(__IO uint8_t *)(deviceAddress + CMD_AREA) = NAND_CMD_WRITE_TRUE1;
        __DSB();

        /* Get tick */
        tickstart = HAL_GetTick();

        /* Wait for the write operation to complete */
        while(1)
        {
            /* Read the R/B pin */
            rb_status = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_6);

            if (rb_status == GPIO_PIN_SET) // R/B 핀이 High면 Ready 상태
            {
                break;
            }

            /* Check for timeout */
            if ((HAL_GetTick() - tickstart) > 5000)
            {
                /* Timeout occurred */
                hnand->State = HAL_NAND_STATE_READY;
                __HAL_UNLOCK(hnand);
                return HAL_TIMEOUT; 
            }
        }

        /* Decrement pages to write */
        NumPageToWrite--;

        /* Increment the NAND address */
        nandAddress++;
    }

    /* Update the NAND controller state */ 
    hnand->State = HAL_NAND_STATE_READY;

    /* Process unlocked */
    __HAL_UNLOCK(hnand);      

    return HAL_OK;
}




#endif
/**
  * @brief  Read Spare area(s) from NAND memory (8-bits addressing)
  * @param  hnand pointer to a NAND_HandleTypeDef structure that contains
  *                the configuration information for NAND module.
  * @param  pAddress  pointer to NAND address structure
  * @param  pBuffer pointer to source buffer to write  
  * @param  NumSpareAreaToRead Number of spare area to read  
  * @retval HAL status
*/
HAL_StatusTypeDef BSP_NAND_Read_SpareArea_8b(NAND_HandleTypeDef *hnand, NAND_AddressTypeDef *pAddress, uint8_t *pBuffer, uint32_t NumSpareAreaToRead)
{
  __IO uint32_t index = 0;
  uint32_t tickstart = 0U;
  uint32_t deviceAddress = 0, size = 0, numSpareAreaRead = 0, nandAddress = 0, columnAddress = 0;
  
  /* Process Locked */
  __HAL_LOCK(hnand);  
  
  /* Check the NAND controller state */
  if(hnand->State == HAL_NAND_STATE_BUSY)
  {
     return HAL_BUSY;
  }
  
  /* Identify the device address */
  deviceAddress = NAND_DEVICE;
  
  /* Update the NAND controller state */
  hnand->State = HAL_NAND_STATE_BUSY;
  
  /* NAND raw address calculation */
  nandAddress = ARRAY_ADDRESS(pAddress, hnand);
  
  /* Column in page address */
  columnAddress = COLUMN_ADDRESS(hnand);
  
  /* Spare area(s) read loop */ 
  while((NumSpareAreaToRead != 0) && (nandAddress < ((hnand->Config.BlockSize) * (hnand->Config.BlockNbr))))
  {
    /* update the buffer size */
    size = (hnand->Config.SpareAreaSize) + ((hnand->Config.SpareAreaSize) * numSpareAreaRead);   
    
    /* Cards with page size <= 512 bytes */
    if((hnand->Config.PageSize) <= 512)
    {
      /* Send read spare area command sequence */     
      *(__IO uint8_t *)((uint32_t)(deviceAddress | CMD_AREA)) = NAND_CMD_AREA_C;
      __DSB();
      
      if (((hnand->Config.BlockSize)*(hnand->Config.BlockNbr)) <= 65535)
      {
        *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = 0x00;
        __DSB();
        *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = ADDR_1ST_CYCLE(nandAddress);
        __DSB();
        *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = ADDR_2ND_CYCLE(nandAddress);
        __DSB();
      }
      else /* ((hnand->Config.BlockSize)*(hnand->Config.BlockNbr)) > 65535 */
      {
        *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = 0x00;
        __DSB();
        *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = ADDR_1ST_CYCLE(nandAddress);
        __DSB();
        *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = ADDR_2ND_CYCLE(nandAddress);
        __DSB();
        *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = ADDR_3RD_CYCLE(nandAddress);
        __DSB();
      }
    }
    else /* (hnand->Config.PageSize) > 512 */
    {
      /* Send read spare area command sequence */ 
      *(__IO uint8_t *)((uint32_t)(deviceAddress | CMD_AREA)) = NAND_CMD_AREA_A;
      __DSB();
      
      if (((hnand->Config.BlockSize)*(hnand->Config.BlockNbr)) <= 65535)
      {
        *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = COLUMN_1ST_CYCLE(columnAddress);
        __DSB();
        *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = COLUMN_2ND_CYCLE(columnAddress);
        __DSB();
        *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = ADDR_1ST_CYCLE(nandAddress);
        __DSB();
        *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = ADDR_2ND_CYCLE(nandAddress);
        __DSB();
      }
      else /* ((hnand->Config.BlockSize)*(hnand->Config.BlockNbr)) > 65535 */
      {
        *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = COLUMN_1ST_CYCLE(columnAddress);
        __DSB();
        *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = COLUMN_2ND_CYCLE(columnAddress);
        __DSB();
        *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = ADDR_1ST_CYCLE(nandAddress);
        __DSB();
        *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = ADDR_2ND_CYCLE(nandAddress);
        __DSB();
        *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = ADDR_3RD_CYCLE(nandAddress);
        __DSB();
      }
    }

    *(__IO uint8_t *)((uint32_t)(deviceAddress | CMD_AREA)) = NAND_CMD_AREA_TRUE1;
    __DSB();
    
    if(hnand->Config.ExtraCommandEnable == ENABLE)
    {
      /* Get tick */
      tickstart = HAL_GetTick();
      
      /* Read status until NAND is ready */
      while(HAL_NAND_Read_Status(hnand) != NAND_READY)
      {
        if((HAL_GetTick() - tickstart ) > NAND_WRITE_TIMEOUT)
        {
          return HAL_TIMEOUT; 
        }
      }
      
      /* Go back to read mode */
      *(__IO uint8_t *)((uint32_t)(deviceAddress | CMD_AREA)) = ((uint8_t)0x00U);
      __DSB();
    }
    
    /* Get Data into Buffer */
    for(; index < size; index++)
    {
      *(uint8_t *)pBuffer++ = *(uint8_t *)deviceAddress;
    }
    
    /* Increment read spare areas number */
    numSpareAreaRead++;
    
    /* Decrement spare areas to read */
    NumSpareAreaToRead--;
    
    /* Increment the NAND address */
    nandAddress = (uint32_t)(nandAddress + 1);

	HAL_Delay(2);
  }
  
  /* Update the NAND controller state */
  hnand->State = HAL_NAND_STATE_READY;
  
  /* Process unlocked */
  __HAL_UNLOCK(hnand);     

  return HAL_OK;  
}

/**
  * @brief  Read Spare area(s) from NAND memory (16-bits addressing)
  * @param  hnand pointer to a NAND_HandleTypeDef structure that contains
  *                the configuration information for NAND module.
  * @param  pAddress  pointer to NAND address structure
  * @param  pBuffer pointer to source buffer to write. pBuffer should be 16bits aligned.
  * @param  NumSpareAreaToRead Number of spare area to read  
  * @retval HAL status
*/
HAL_StatusTypeDef BSP_NAND_Read_SpareArea_16b(NAND_HandleTypeDef *hnand, NAND_AddressTypeDef *pAddress, uint16_t *pBuffer, uint32_t NumSpareAreaToRead)
{
  __IO uint32_t index = 0; 
  uint32_t tickstart = 0U;
  uint32_t deviceAddress = 0, size = 0, numSpareAreaRead = 0, nandAddress = 0, columnAddress = 0;
  
  /* Process Locked */
  __HAL_LOCK(hnand);  
  
  /* Check the NAND controller state */
  if(hnand->State == HAL_NAND_STATE_BUSY)
  {
     return HAL_BUSY;
  }
  
  /* Identify the device address */
  deviceAddress = NAND_DEVICE;
  
  /* Update the NAND controller state */
  hnand->State = HAL_NAND_STATE_BUSY;
  
  /* NAND raw address calculation */
  nandAddress = ARRAY_ADDRESS(pAddress, hnand);
  
  /* Column in page address */
  columnAddress = (uint32_t)(COLUMN_ADDRESS(hnand));
  
  /* Spare area(s) read loop */ 
  while((NumSpareAreaToRead != 0) && (nandAddress < ((hnand->Config.BlockSize) * (hnand->Config.BlockNbr))))
  {
    /* update the buffer size */
    size = (hnand->Config.SpareAreaSize) + ((hnand->Config.SpareAreaSize) * numSpareAreaRead);

    /* Cards with page size <= 512 bytes */
    if((hnand->Config.PageSize) <= 512)
    {
      /* Send read spare area command sequence */     
      *(__IO uint8_t *)((uint32_t)(deviceAddress | CMD_AREA)) = NAND_CMD_AREA_C;
      __DSB();
      
      if (((hnand->Config.BlockSize)*(hnand->Config.BlockNbr)) <= 65535)
      {
        *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = 0x00;
        __DSB();
        *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = ADDR_1ST_CYCLE(nandAddress);
        __DSB();
        *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = ADDR_2ND_CYCLE(nandAddress);
        __DSB();
      }
      else /* ((hnand->Config.BlockSize)*(hnand->Config.BlockNbr)) > 65535 */
      {
        *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = 0x00;
        __DSB();
        *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = ADDR_1ST_CYCLE(nandAddress);
        __DSB();
        *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = ADDR_2ND_CYCLE(nandAddress);
        __DSB();
        *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = ADDR_3RD_CYCLE(nandAddress);
        __DSB();
      }
    }
    else /* (hnand->Config.PageSize) > 512 */
    {
      /* Send read spare area command sequence */     
      *(__IO uint8_t *)((uint32_t)(deviceAddress | CMD_AREA)) = NAND_CMD_AREA_A;
      __DSB();
      
      if (((hnand->Config.BlockSize)*(hnand->Config.BlockNbr)) <= 65535)
      {
        *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = COLUMN_1ST_CYCLE(columnAddress);
        __DSB();
        *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = COLUMN_2ND_CYCLE(columnAddress);
        __DSB();
        *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = ADDR_1ST_CYCLE(nandAddress);
        __DSB();
        *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = ADDR_2ND_CYCLE(nandAddress);
        __DSB();
      }
      else /* ((hnand->Config.BlockSize)*(hnand->Config.BlockNbr)) > 65535 */
      {
        *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = COLUMN_1ST_CYCLE(columnAddress);
        __DSB();
        *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = COLUMN_2ND_CYCLE(columnAddress);
        __DSB();
        *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = ADDR_1ST_CYCLE(nandAddress);
        __DSB();
        *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = ADDR_2ND_CYCLE(nandAddress);
        __DSB();
        *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = ADDR_3RD_CYCLE(nandAddress);
        __DSB();
      }
    }

    *(__IO uint8_t *)((uint32_t)(deviceAddress | CMD_AREA)) = NAND_CMD_AREA_TRUE1;
    __DSB();

    if(hnand->Config.ExtraCommandEnable == ENABLE)
    {
      /* Get tick */
      tickstart = HAL_GetTick();
      
      /* Read status until NAND is ready */
      while(HAL_NAND_Read_Status(hnand) != NAND_READY)
      {
        if((HAL_GetTick() - tickstart ) > NAND_WRITE_TIMEOUT)
        {
          return HAL_TIMEOUT; 
        }
      }
      
      /* Go back to read mode */
      *(__IO uint8_t *)((uint32_t)(deviceAddress | CMD_AREA)) = ((uint8_t)0x00U);
      __DSB();
    }
    
    /* Get Data into Buffer */
    for(; index < size; index++)
    {
      *(uint16_t *)pBuffer++ = *(uint16_t *)deviceAddress;
    }
    
    /* Increment read spare areas number */
    numSpareAreaRead++;
    
    /* Decrement spare areas to read */
    NumSpareAreaToRead--;
    
    /* Increment the NAND address */
    nandAddress = (uint32_t)(nandAddress + 1);

	HAL_Delay(2);
  }
  
  /* Update the NAND controller state */
  hnand->State = HAL_NAND_STATE_READY;
  
  /* Process unlocked */
  __HAL_UNLOCK(hnand);     

  return HAL_OK;  
}

/**
  * @brief  Write Spare area(s) to NAND memory (8-bits addressing)
  * @param  hnand pointer to a NAND_HandleTypeDef structure that contains
  *                the configuration information for NAND module.
  * @param  pAddress  pointer to NAND address structure
  * @param  pBuffer  pointer to source buffer to write  
  * @param  NumSpareAreaTowrite   number of spare areas to write to block
  * @retval HAL status
  */
HAL_StatusTypeDef BSP_NAND_Write_SpareArea_8b(NAND_HandleTypeDef *hnand, NAND_AddressTypeDef *pAddress, uint8_t *pBuffer, uint32_t NumSpareAreaTowrite)
{
  __IO uint32_t index = 0;
  uint32_t tickstart = 0;
  uint32_t deviceAddress = 0, size = 0, numSpareAreaWritten = 0, nandAddress = 0, columnAddress =0;

  /* Process Locked */
  __HAL_LOCK(hnand); 
  
  /* Check the NAND controller state */
  if(hnand->State == HAL_NAND_STATE_BUSY)
  {
     return HAL_BUSY;
  }
  
  /* Identify the device address */
  deviceAddress = NAND_DEVICE;
  
  /* Update the FMC_NAND controller state */
  hnand->State = HAL_NAND_STATE_BUSY;  
  
  /* Page address calculation */
  nandAddress = ARRAY_ADDRESS(pAddress, hnand); 
  
  /* Column in page address */
  columnAddress = COLUMN_ADDRESS(hnand);
  
  /* Spare area(s) write loop */
  while((NumSpareAreaTowrite != 0) && (nandAddress < ((hnand->Config.BlockSize) * (hnand->Config.BlockNbr))))
  {
    /* update the buffer size */
    size = (hnand->Config.SpareAreaSize) + ((hnand->Config.SpareAreaSize) * numSpareAreaWritten);

    /* Cards with page size <= 512 bytes */
    if((hnand->Config.PageSize) <= 512)
    {
      /* Send write Spare area command sequence */
      *(__IO uint8_t *)((uint32_t)(deviceAddress | CMD_AREA)) = NAND_CMD_AREA_C;
      __DSB();
      *(__IO uint8_t *)((uint32_t)(deviceAddress | CMD_AREA)) = NAND_CMD_WRITE0;
      __DSB();
      
      if (((hnand->Config.BlockSize)*(hnand->Config.BlockNbr)) <= 65535)
      {
        *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = 0x00;
        __DSB();
        *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = ADDR_1ST_CYCLE(nandAddress);
        __DSB();
        *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = ADDR_2ND_CYCLE(nandAddress);
        __DSB();
      }
      else /* ((hnand->Config.BlockSize)*(hnand->Config.BlockNbr)) > 65535 */
      {
        *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = 0x00;
        __DSB();
        *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = ADDR_1ST_CYCLE(nandAddress);
        __DSB();
        *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = ADDR_2ND_CYCLE(nandAddress);
        __DSB();
        *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = ADDR_3RD_CYCLE(nandAddress);
        __DSB();
      }
    }
    else /* (hnand->Config.PageSize) > 512 */
    {
      /* Send write Spare area command sequence */
      *(__IO uint8_t *)((uint32_t)(deviceAddress | CMD_AREA)) = NAND_CMD_AREA_A;
      __DSB();
      *(__IO uint8_t *)((uint32_t)(deviceAddress | CMD_AREA)) = NAND_CMD_WRITE0;
      __DSB();
    
      if (((hnand->Config.BlockSize)*(hnand->Config.BlockNbr)) <= 65535)
      {
        *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = COLUMN_1ST_CYCLE(columnAddress);
        __DSB();
        *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = COLUMN_2ND_CYCLE(columnAddress);
        __DSB();
        *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = ADDR_1ST_CYCLE(nandAddress);
        __DSB();
        *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = ADDR_2ND_CYCLE(nandAddress);
        __DSB();
      }
      else /* ((hnand->Config.BlockSize)*(hnand->Config.BlockNbr)) > 65535 */
      {
        *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = COLUMN_1ST_CYCLE(columnAddress);
        __DSB();
        *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = COLUMN_2ND_CYCLE(columnAddress);
        __DSB();
        *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = ADDR_1ST_CYCLE(nandAddress);
        __DSB();
        *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = ADDR_2ND_CYCLE(nandAddress);
        __DSB();
        *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = ADDR_3RD_CYCLE(nandAddress);
        __DSB();
      }
    }
  
    /* Write data to memory */
    for(; index < size; index++)
    {
      *(__IO uint8_t *)deviceAddress = *(uint8_t *)pBuffer++;
      __DSB();
    }
   
    *(__IO uint8_t *)((uint32_t)(deviceAddress | CMD_AREA)) = NAND_CMD_WRITE_TRUE1;
    __DSB();
   
    /* Get tick */
    tickstart = HAL_GetTick();
	
    /* Read status until NAND is ready */
    while(HAL_NAND_Read_Status(hnand) != NAND_READY)
    {  
      if((HAL_GetTick() - tickstart ) > NAND_WRITE_TIMEOUT)
      {
        return HAL_TIMEOUT; 
      }
    }

    /* Increment written spare areas number */
    numSpareAreaWritten++;
    
    /* Decrement spare areas to write */
    NumSpareAreaTowrite--;
    
    /* Increment the NAND address */
    nandAddress = (uint32_t)(nandAddress + 1);
  }

  /* Update the NAND controller state */
  hnand->State = HAL_NAND_STATE_READY;

  /* Process unlocked */
  __HAL_UNLOCK(hnand);

  return HAL_OK;  
}

/**
  * @brief  Write Spare area(s) to NAND memory (16-bits addressing)
  * @param  hnand pointer to a NAND_HandleTypeDef structure that contains
  *                the configuration information for NAND module.
  * @param  pAddress  pointer to NAND address structure
  * @param  pBuffer  pointer to source buffer to write. pBuffer should be 16bits aligned.  
  * @param  NumSpareAreaTowrite   number of spare areas to write to block
  * @retval HAL status
  */
HAL_StatusTypeDef BSP_NAND_Write_SpareArea_16b(NAND_HandleTypeDef *hnand, NAND_AddressTypeDef *pAddress, uint16_t *pBuffer, uint32_t NumSpareAreaTowrite)
{
  __IO uint32_t index = 0;
  uint32_t tickstart = 0;
  uint32_t deviceAddress = 0, size = 0, numSpareAreaWritten = 0, nandAddress = 0, columnAddress = 0;

  /* Process Locked */
  __HAL_LOCK(hnand); 
  
  /* Check the NAND controller state */
  if(hnand->State == HAL_NAND_STATE_BUSY)
  {
     return HAL_BUSY;
  }
  
  /* Identify the device address */
  deviceAddress = NAND_DEVICE;
  
  /* Update the FMC_NAND controller state */
  hnand->State = HAL_NAND_STATE_BUSY;  
  
  /* NAND raw address calculation */
  nandAddress = ARRAY_ADDRESS(pAddress, hnand);
  
  /* Column in page address */
  columnAddress = (uint32_t)(COLUMN_ADDRESS(hnand));
  
  /* Spare area(s) write loop */
  while((NumSpareAreaTowrite != 0) && (nandAddress < ((hnand->Config.BlockSize) * (hnand->Config.BlockNbr))))
  {
    /* update the buffer size */
    size = (hnand->Config.SpareAreaSize) + ((hnand->Config.SpareAreaSize) * numSpareAreaWritten);

    /* Cards with page size <= 512 bytes */
    if((hnand->Config.PageSize) <= 512)
    {
      /* Send write Spare area command sequence */
      *(__IO uint8_t *)((uint32_t)(deviceAddress | CMD_AREA)) = NAND_CMD_AREA_C;
      __DSB();
      *(__IO uint8_t *)((uint32_t)(deviceAddress | CMD_AREA)) = NAND_CMD_WRITE0;
      __DSB();
    
      if (((hnand->Config.BlockSize)*(hnand->Config.BlockNbr)) <= 65535)
      {
        *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = 0x00;
        __DSB();
        *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = ADDR_1ST_CYCLE(nandAddress);
        __DSB();
        *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = ADDR_2ND_CYCLE(nandAddress);
        __DSB();
      }
      else /* ((hnand->Config.BlockSize)*(hnand->Config.BlockNbr)) > 65535 */
      {
        *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = 0x00;
        __DSB();
        *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = ADDR_1ST_CYCLE(nandAddress);
        __DSB();
        *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = ADDR_2ND_CYCLE(nandAddress);
        __DSB();
        *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = ADDR_3RD_CYCLE(nandAddress);
        __DSB();
      }
    }
    else /* (hnand->Config.PageSize) > 512 */
    {
      /* Send write Spare area command sequence */
      *(__IO uint8_t *)((uint32_t)(deviceAddress | CMD_AREA)) = NAND_CMD_AREA_A;
      __DSB();
      *(__IO uint8_t *)((uint32_t)(deviceAddress | CMD_AREA)) = NAND_CMD_WRITE0;
      __DSB();
    
      if (((hnand->Config.BlockSize)*(hnand->Config.BlockNbr)) <= 65535)
      {
        *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = COLUMN_1ST_CYCLE(columnAddress);
        __DSB();
        *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = COLUMN_2ND_CYCLE(columnAddress);
        __DSB();
        *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = ADDR_1ST_CYCLE(nandAddress);
        __DSB();
        *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = ADDR_2ND_CYCLE(nandAddress);
        __DSB();
      }
      else /* ((hnand->Config.BlockSize)*(hnand->Config.BlockNbr)) > 65535 */
      {
        *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = COLUMN_1ST_CYCLE(columnAddress);
        __DSB();
        *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = COLUMN_2ND_CYCLE(columnAddress);
        __DSB();
        *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = ADDR_1ST_CYCLE(nandAddress);
        __DSB();
        *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = ADDR_2ND_CYCLE(nandAddress);
        __DSB();
        *(__IO uint8_t *)((uint32_t)(deviceAddress | ADDR_AREA)) = ADDR_3RD_CYCLE(nandAddress);
        __DSB();
      }
    }
  
    /* Write data to memory */
    for(; index < size; index++)
    {
      *(__IO uint16_t *)deviceAddress = *(uint16_t *)pBuffer++;
      __DSB();
    }
   
    *(__IO uint8_t *)((uint32_t)(deviceAddress | CMD_AREA)) = NAND_CMD_WRITE_TRUE1;
    __DSB();
   
    /* Get tick */
    tickstart = HAL_GetTick();
	  
    /* Read status until NAND is ready */
    while(HAL_NAND_Read_Status(hnand) != NAND_READY)
    { 
      if((HAL_GetTick() - tickstart ) > NAND_WRITE_TIMEOUT)
      {
        return HAL_TIMEOUT; 
      }
    }

    /* Increment written spare areas number */
    numSpareAreaWritten++;
    
    /* Decrement spare areas to write */
    NumSpareAreaTowrite--;
    
    /* Increment the NAND address */
    nandAddress = (uint32_t)(nandAddress + 1);
  }

  /* Update the NAND controller state */
  hnand->State = HAL_NAND_STATE_READY;

  /* Process unlocked */
  __HAL_UNLOCK(hnand);

  return HAL_OK;  
}

void BSP_NND_Read_Page_8b(U16 block, U16 page, U08 *pBuffer, U32 NumPage)
{
	WriteReadAddr.Plane = 0x00;
	WriteReadAddr.Block = block;
	WriteReadAddr.Page = page;
	
	TMR_HalEnable(_F_F, _F_F);

	BSP_NAND_Read_Page_8b(&hnand1, &WriteReadAddr, pBuffer, NumPage);
	
	TMR_HalEnable(_F_T, _F_F);
	
}

void BSP_NND_Write_Page_8b(U16 block, U16 page, U08 *pBuffer, U32 NumPage)
{
	WriteReadAddr.Plane = 0x00;
	WriteReadAddr.Block = block;
	WriteReadAddr.Page = page;
	
	TMR_HalEnable(_F_F, _F_F);
	
	BSP_NAND_Write_Page_8b(&hnand1, &WriteReadAddr, pBuffer, NumPage);
	
	TMR_HalEnable(_F_T, _F_F);
}

void BSP_NND_Read_Page_16b(U16 block, U16 page, U16 *pBuffer, U32 NumPage)
{
	WriteReadAddr.Plane = 0x00;
	WriteReadAddr.Block = block;
	WriteReadAddr.Page = page;
	
	TMR_HalEnable(_F_F, _F_F);

	BSP_NAND_Read_Page_16b(&hnand1, &WriteReadAddr, pBuffer, NumPage);
	
	TMR_HalEnable(_F_T, _F_F);
	
}

void BSP_NND_Write_Page_16b(U16 block, U16 page, U16 *pBuffer, U32 NumPage)
{
	WriteReadAddr.Plane = 0x00;
	WriteReadAddr.Block = block;
	WriteReadAddr.Page = page;
	
	TMR_HalEnable(_F_F, _F_F);
	
	BSP_NAND_Write_Page_16b(&hnand1, &WriteReadAddr, pBuffer, NumPage);
	
	TMR_HalEnable(_F_T, _F_F);
}

void BSP_NND_Read_SpareArea_8b(U32 page_no, U08 *pBuffer, U32 NumSpareArea)
{
	WriteReadAddr.Plane = 0x00;
	WriteReadAddr.Block = (page_no / 64);
	WriteReadAddr.Page = 0x01;
	
	TMR_HalEnable(_F_F, _F_F);

	BSP_NAND_Read_SpareArea_8b(&hnand1, &WriteReadAddr, pBuffer, NumSpareArea);
	
	TMR_HalEnable(_F_T, _F_F);
	
}

void BSP_NND_Write_SpareArea_8b(U32 page_no, U08 *pBuffer, U32 NumSpareArea)
{
	WriteReadAddr.Plane = 0x00;
	WriteReadAddr.Block = (page_no / 64);
	WriteReadAddr.Page = 0x01;
	
	TMR_HalEnable(_F_F, _F_F);
	
	BSP_NAND_Write_SpareArea_8b(&hnand1, &WriteReadAddr, pBuffer, NumSpareArea);
	
	TMR_HalEnable(_F_T, _F_F);
}

void BSP_NND_Read_SpareArea_16b(U16 block, U16 page, U16 *pBuffer, U32 NumPage)
{
	WriteReadAddr.Plane = 0x00;
	WriteReadAddr.Block = block;
	WriteReadAddr.Page = page;

	TMR_HalEnable(_F_F, _F_F);

	BSP_NAND_Read_SpareArea_16b(&hnand1, &WriteReadAddr, pBuffer, NumPage);
	
	TMR_HalEnable(_F_T, _F_F);
	
}

void BSP_NND_Write_SpareArea_16b(U32 page_no, U16 *pBuffer, U32 NumSpareArea)
{
	WriteReadAddr.Plane = 0x00;
	WriteReadAddr.Block = (page_no / 64);
	WriteReadAddr.Page = 0x01;
	
	TMR_HalEnable(_F_F, _F_F);
	
	BSP_NAND_Write_SpareArea_16b(&hnand1, &WriteReadAddr, pBuffer, NumSpareArea);
	
	TMR_HalEnable(_F_T, _F_F);
}

void BSP_NND_Erase_Block(U16 block, U16 page)
{
	WriteReadAddr.Plane = 0x00;
	WriteReadAddr.Block = block;
	WriteReadAddr.Page = page;
	
	TMR_HalEnable(_F_F, _F_F);

	HAL_NAND_Erase_Block(&hnand1, &WriteReadAddr);
	
	TMR_HalEnable(_F_T, _F_F);
}

void NND_Echo_buffer_Read(U16 cnt)
{

	nnd_echo.block = nand_meta_main[nnd_echo.ch][cnt][6];
	nnd_echo.page = nand_meta_main[nnd_echo.ch][cnt][7];

	

	if(nnd_echo.ch==APP_CH_2)
		nnd_echo.block=nnd_echo.block+200;

	HAL_Delay(2);

	BSP_NND_Read_Page_16b(nnd_echo.block, nnd_echo.page, nnd_echo.echo_main, 1);
	HAL_Delay(2);

	nnd_echo.year = nnd_echo.echo_main[0];
	nnd_echo.month = nnd_echo.echo_main[1];
	nnd_echo.day = nnd_echo.echo_main[2];
	nnd_echo.hour = nnd_echo.echo_main[3];
	nnd_echo.min = nnd_echo.echo_main[4];
	nnd_echo.sec = nnd_echo.echo_main[5];

	nnd_echo.empty = nnd_echo.echo_main[10];
	nnd_echo.dead = nnd_echo.echo_main[11];
	nnd_echo.echo_amp = nnd_echo.echo_main[12];

	nnd_echo.rslt_sldg[MsCAL_THR_LIGHT] = nnd_echo.echo_main[17];
	nnd_echo.trac_dist[MsCAL_THR_LIGHT] = nnd_echo.echo_main[17];
	nnd_echo.thr[MsCAL_THR_LIGHT] = nnd_echo.echo_main[21];


	nnd_echo.rslt_sldg[MsCAL_THR_HEAVY] = nnd_echo.echo_main[17];
	nnd_echo.trac_dist[MsCAL_THR_HEAVY] = nnd_echo.echo_main[17];

	nnd_echo.thr[MsCAL_THR_HEAVY] = nnd_echo.echo_main[21];

	nnd_echo.each_slope_val[0][0] = nnd_echo.echo_main[8];
	nnd_echo.each_slope_val[0][1] = nnd_echo.echo_main[9];
	nnd_echo.each_slope_val[1][0] = nnd_echo.echo_main[6];
	nnd_echo.each_slope_val[1][1] = nnd_echo.echo_main[7];

	nnd_echo.each_thr_val[0][0] = nnd_echo.echo_main[15];
	nnd_echo.each_thr_val[0][1] = nnd_echo.echo_main[16];
	nnd_echo.each_thr_val[1][0] = nnd_echo.echo_main[13];
	nnd_echo.each_thr_val[1][1] = nnd_echo.echo_main[14];


	nnd_echo.each_slope = nnd_echo.echo_main[20];
	nnd_echo.result_slope = nnd_echo.echo_main[23];
	nnd_echo.result_echo_amp= nnd_echo.echo_main[26];
	nnd_echo.input_val= nnd_echo.echo_main[18];
	nnd_echo.output_val=nnd_echo.echo_main[25];	
	nnd_echo.result_thr = nnd_echo.echo_main[24];

	ScECH_PrSet_ECHO_SAVE_TIME();
}

#if 0
void NND_auto_calib_buffer_Read(U08 ch,U16 block, U08 page)
{
	BSP_NND_Read_Page_16b(block, page, nnd_echo.echo_main, 1);

}
#endif



void NND_auto_calib_buffer_Read(U16 cnt)
{
	nnd_echo.block = nand_meta_main[nnd_echo.ch][cnt][6];
	nnd_echo.page = nand_meta_main[nnd_echo.ch][cnt][7];

	

	nnd_echo.block=nnd_echo.block+400;

	HAL_Delay(1);

	BSP_NND_Read_Page_16b(nnd_echo.block, nnd_echo.page, nnd_echo.echo_main, 1);
	HAL_Delay(2);

	nnd_echo.year = nnd_echo.echo_main[0];
	nnd_echo.month = nnd_echo.echo_main[1];
	nnd_echo.day = nnd_echo.echo_main[2];
	nnd_echo.hour = nnd_echo.echo_main[3];
	nnd_echo.min = nnd_echo.echo_main[4];
	nnd_echo.sec = nnd_echo.echo_main[5];

	nnd_echo.empty = nnd_echo.echo_main[10];
	nnd_echo.dead = nnd_echo.echo_main[11];
	nnd_echo.echo_amp = nnd_echo.echo_main[12];

	nnd_echo.rslt_sldg[MsCAL_THR_LIGHT] = nnd_echo.echo_main[17];
	nnd_echo.trac_dist[MsCAL_THR_LIGHT] = nnd_echo.echo_main[17];
	nnd_echo.thr[MsCAL_THR_LIGHT] = nnd_echo.echo_main[21];


	nnd_echo.rslt_sldg[MsCAL_THR_HEAVY] = nnd_echo.echo_main[17];
	nnd_echo.trac_dist[MsCAL_THR_HEAVY] = nnd_echo.echo_main[17];

	nnd_echo.thr[MsCAL_THR_HEAVY] = nnd_echo.echo_main[21];

	nnd_echo.each_slope_val[0][0] = nnd_echo.echo_main[8];
	nnd_echo.each_slope_val[0][1] = nnd_echo.echo_main[9];
	nnd_echo.each_slope_val[1][0] = nnd_echo.echo_main[6];
	nnd_echo.each_slope_val[1][1] = nnd_echo.echo_main[7];

	nnd_echo.each_thr_val[0][0] = nnd_echo.echo_main[15];
	nnd_echo.each_thr_val[0][1] = nnd_echo.echo_main[16];
	nnd_echo.each_thr_val[1][0] = nnd_echo.echo_main[13];
	nnd_echo.each_thr_val[1][1] = nnd_echo.echo_main[14];


	nnd_echo.each_slope = nnd_echo.echo_main[20];
	nnd_echo.result_slope = nnd_echo.echo_main[23];
	nnd_echo.result_echo_amp= nnd_echo.echo_main[26];
	nnd_echo.input_val= nnd_echo.echo_main[18];
	nnd_echo.output_val=nnd_echo.echo_main[25];	
	nnd_echo.result_thr = nnd_echo.echo_main[24];

	ScECH_PrSet_ECHO_SAVE_TIME();

#if 0
	I08 sDbg[256];
	_SPRINTF(sDbg, "%d %d %d %d", nnd_echo.echo_amp,cnt,nnd_echo.block,nnd_echo.page);
	DpFIG_DrwRect(100, 30, 400, 25, _cBGD_SCRN, DpFIG_FILL);
	DpSTR_GuiLeft(100, 30, _cECO_LVL_TITL, _cBGD_SCRN, _fE17HsB, sDbg);			
#endif
}


void NND_auto_calib_Read(U08 ch,U08 sel)
{
	U16 block= nand_meta_block[ch];
	U16 page = 0;
	U16 buffer = nand_meta_buffer[ch];
	U08 metabuffer_sub[2048];
	U16 i,j;
	U08 cnt = nand_auto_calib_cnt;
	U16 max;

	if(cnt>=4)	cnt = 0;

	RTC_TM rtc = RTC_GetTime();

	if(nand_auto_calib_num[ch]<1200)
	{
		block = nand_meta_calib_block_value+ch*nand_meta_calib_block_range+sel;
		max = nand_auto_calib_num[ch]/300;
	}
	else
	{
		block = nand_meta_calib_block_value+ch*nand_meta_calib_block_range+sel;
		if(block==nand_meta_calib_block_value+(ch*nand_meta_calib_block_range)+4)
			block = nand_meta_calib_block_value+ch*nand_meta_calib_block_range;
		max = nand_auto_calib_num[ch]/300;
	}

	nand_echo_cnt[ch] = 0;

	
#if 1

	HAL_Delay(1);
	BSP_NND_Read_Page_8b(block, page, metabuffer_sub, 1);
	HAL_Delay(2);

	for(j=0; j<400; j=j+8)
	{
		nand_meta_main[ch][nand_echo_cnt[ch]][0] = metabuffer_sub[j+0];
		nand_meta_main[ch][nand_echo_cnt[ch]][1] = metabuffer_sub[j+1];
		nand_meta_main[ch][nand_echo_cnt[ch]][2] = metabuffer_sub[j+2];
		nand_meta_main[ch][nand_echo_cnt[ch]][3] = metabuffer_sub[j+3];
		nand_meta_main[ch][nand_echo_cnt[ch]][4] = metabuffer_sub[j+4];
		nand_meta_main[ch][nand_echo_cnt[ch]][5] = metabuffer_sub[j+5];
		nand_meta_main[ch][nand_echo_cnt[ch]][6] = metabuffer_sub[j+6];
		nand_meta_main[ch][nand_echo_cnt[ch]][7] = metabuffer_sub[j+7];
		nand_echo_cnt[ch]++;
	}


	
	nnd_echo.num = nand_echo_cnt[ch]-1;
	nnd_echo.ch =ch;
	NND_auto_calib_buffer_Read(nnd_echo.num);

	
#endif
	//URT_InitMain(URT_IDX_1,115200);
	//buff[buff_cnt++] = 0x2C;
	//URT_TxPkt(URT_IDX_1, buff, buff_cnt);

	

}


#if 0
void NND_auto_calib_Read(U08 ch)
{
	U16 i,j;
	U16 block = nand_auto_calib_block[ch];
	U08 page = nand_auto_calib_page[ch];
	U16 cnt = nand_auto_calib_cnt[ch];

	if(block == (500+(24*ch)+24)) block = 500+(24*ch);
	if(cnt==4) cnt =0;
	j=0;
	//nand_auto_calib_block[ch] = 500+(24*ch)+(nand_auto_calib_cnt[ch]*6)+lMsCal.auto_calib_cnt;


	if(nand_auto_calib_num[ch]<1200)
	{
		block = 500+(24*ch);
		page = 0;

		for(i=0; i<nand_auto_calib_num[ch]/300; i++)
		{
			BSP_NND_Read_Page_16b(block, page, nnd_echo.echo_main, 1);
			nand_meta_main[ch][i][0] = nnd_echo.echo_main[0];
			nand_meta_main[ch][i][1] = nnd_echo.echo_main[1];
			nand_meta_main[ch][i][2] = nnd_echo.echo_main[2];
			nand_meta_main[ch][i][3] = nnd_echo.echo_main[3];
			nand_meta_main[ch][i][4] = nnd_echo.echo_main[4];
			block+=6;
		}
	}
	else
	{
		page = 0;
		for(i=0; i<nand_auto_calib_num[ch]/300; i++)
		{
			BSP_NND_Read_Page_16b(block, page, nnd_echo.echo_main, 1);
			nand_meta_main[ch][i][0] = nnd_echo.echo_main[0];
			nand_meta_main[ch][i][1] = nnd_echo.echo_main[1];
			nand_meta_main[ch][i][2] = nnd_echo.echo_main[2];
			nand_meta_main[ch][i][3] = nnd_echo.echo_main[3];
			nand_meta_main[ch][i][4] = nnd_echo.echo_main[4];

			block+=6;
			if(block >= (500+(24*ch)+24))
				block = 500+(24*ch);
		}
	}

	nnd_echo.num = 0;
	nnd_echo.ch =ch;

	BSP_NND_Read_Page_16b(nnd_echo.block, nnd_echo.page, nnd_echo.echo_main, 1);

}
#endif
void NND_Echo_Read(U08 ch)
{
	U16 block= nand_meta_block[ch];
	U16 page = nand_meta_page[ch];
	U16 buffer = nand_meta_buffer[ch];
	U08 metabuffer_sub[2048];
	U16 i,j;
	//U16 echo_max;
	//U08 buff[64] = {0, };
	//U16 buff_cnt=0;
	U16 max;
	//I08 sDbg[32];

	RTC_TM rtc = RTC_GetTime();

	nand_cnt2=rtc.seconds;

	nand_echo_cnt[ch] = 0;

#if 0
	if(fnand==1)
	{
		for(j=0; j<nand_echo_cnt[ch]; j++)
		{
			buff[buff_cnt++] = 0x0D;	// CR
			buff[buff_cnt++] = 0x0A;	// LF
			buff[buff_cnt++] = (j / 10000) % 10 + '0';
			buff[buff_cnt++] = (j / 1000) % 10 + '0';			
			buff[buff_cnt++] = (j / 100) % 10 + '0';
			buff[buff_cnt++] = (j / 10) % 10 + '0';
			buff[buff_cnt++] = j% 10 + '0';

			buff[buff_cnt++] = 0x2C;
			for(i=0; i<8; i++)
			{
				buff[buff_cnt++] = (nand_meta_main[ch][j][i] / 100) % 10 + '0';
				buff[buff_cnt++] = (nand_meta_main[ch][j][i] / 10) % 10 + '0';
				buff[buff_cnt++] = nand_meta_main[ch][j][i]% 10 + '0';
				buff[buff_cnt++] = 0x2C;
			}

			URT_TxPkt(URT_IDX_1, buff, buff_cnt);
			buff_cnt = 0;
		}	
		fnand=2;
		return;
	}


	if(fnand==2)
		return;

	fnand = 1;	
#endif

	nand_cnt2=gTmr.c.c001ms_cnt;

	if(nand_echo_num[ch]<=10000)
	{
		block = 191+(200*ch);
		page = 0;
		max = ((int)(nand_echo_num[ch]/250))*250;
	}
	else if(nand_echo_num[ch]<20000)
	{
		block = nand_meta_block[ch] - 1;
		page = nand_meta_page[ch];
		max = 10000;
	}
	else
	{
		block = (nand_meta_block[ch] == 191+(200*ch)) ? 192+(200*ch) : 191+(200*ch);
		page = nand_meta_page[ch];
		max = 10000;
	}

	while(nand_echo_cnt[ch]<max)
	{
		if(nand_echo_num[ch]<250)
			break;		

		HAL_Delay(2);
		BSP_NND_Read_Page_8b(block, page, metabuffer_sub, 1);
		HAL_Delay(2);

#if 1

		//U08 buff[2048] = {0, };
		//U16 buff_cnt = 0;
		
		//buff_cnt = 0;
		//buff[buff_cnt++] = 0x4d; // '['
		//URT_TxPkt(URT_IDX_1, buff, buff_cnt);
#endif
		
		for(i=0;i<2000; i=i+8)
		{
			nand_meta_main[ch][nand_echo_cnt[ch]][0] = metabuffer_sub[i+0];
			nand_meta_main[ch][nand_echo_cnt[ch]][1] = metabuffer_sub[i+1];
			nand_meta_main[ch][nand_echo_cnt[ch]][2] = metabuffer_sub[i+2];
			nand_meta_main[ch][nand_echo_cnt[ch]][3] = metabuffer_sub[i+3];
			nand_meta_main[ch][nand_echo_cnt[ch]][4] = metabuffer_sub[i+4];
			nand_meta_main[ch][nand_echo_cnt[ch]][5] = metabuffer_sub[i+5];
			nand_meta_main[ch][nand_echo_cnt[ch]][6] = metabuffer_sub[i+6];
			nand_meta_main[ch][nand_echo_cnt[ch]][7] = metabuffer_sub[i+7];
			nand_echo_cnt[ch]++;
		}	

		if(++page>=40)
		{
			page = 0;
			block++;
		}
		if(block==193+(200*ch))
			block = 191+(200*ch);		
	}


	if(nand_meta_buffer[ch]>0)
	{
		HAL_Delay(2);
		BSP_NND_Read_Page_8b(190+(200*ch), 0, metabuffer_sub, 1);
		HAL_Delay(2);
		for(i=0; i<nand_meta_buffer[ch]; i=i+8)
		{
			nand_meta_main[ch][nand_echo_cnt[ch]][0] = metabuffer_sub[i+0];
			nand_meta_main[ch][nand_echo_cnt[ch]][1] = metabuffer_sub[i+1];
			nand_meta_main[ch][nand_echo_cnt[ch]][2] = metabuffer_sub[i+2];
			nand_meta_main[ch][nand_echo_cnt[ch]][3] = metabuffer_sub[i+3];
			nand_meta_main[ch][nand_echo_cnt[ch]][4] = metabuffer_sub[i+4];
			nand_meta_main[ch][nand_echo_cnt[ch]][5] = metabuffer_sub[i+5];
			nand_meta_main[ch][nand_echo_cnt[ch]][6] = metabuffer_sub[i+6];
			nand_meta_main[ch][nand_echo_cnt[ch]][7] = metabuffer_sub[i+7];
			nand_echo_cnt[ch]++;
		}	

	}


	nand_cnt=gTmr.c.c001ms_cnt-nand_cnt2;

	nnd_echo.num = nand_echo_cnt[ch]-1;
	nnd_echo.ch =ch;
	NND_Echo_buffer_Read(nnd_echo.num);




	//URT_InitMain(URT_IDX_1,115200);
	//buff[buff_cnt++] = 0x2C;
	//URT_TxPkt(URT_IDX_1, buff, buff_cnt);

}


void NND_Echo_AutoGain_ProfSave(U08 ch, U08 gain)
{
	U16 echobuffer[1024];
	U32 i;
	RTC_TM rtc = RTC_GetTime();
	U16 val  = MnMSR_CalGet_Ch_Value(ch, MnMS1_OPT_SINGLE_AUTO_CAL);
	U16 empty = MnMSR_BaseGet_Ch_Value(ch, MnMS0_OPT_SINGLE_EMPTY);
	U16 dead = MnMSR_BaseGet_Ch_Value(ch, MnMS0_OPT_SINGLE_DEADZONE);
	U08 metabuffer[2048];

	if(++nand_auto_calib_num[ch]>=1200)
		nand_auto_calib_num[ch] = 1200;


	if(nand_auto_calib_page[ch]==0)
	{
		BSP_NND_Erase_Block(nand_auto_calib_block[ch], 0);
		HAL_Delay(1);
		
	}

	for(i=0; i<1020; i++)
	{
		echobuffer[i] = SDRAM_aEco_prof_S3[(gain*ADC_BUFF_MAX*6)+5*ADC_BUFF_MAX+i];
	}


	echobuffer[0] = rtc.year;	
	echobuffer[1] = rtc.month;
	echobuffer[2] = rtc.day;
	echobuffer[3] = rtc.hours;
	echobuffer[4] = rtc.minutes;
	echobuffer[5] = rtc.seconds;
	echobuffer[6] = lMsCal.auto_calib_val_max[nand_auto_calib_page[ch]+1][0];
	echobuffer[7] = lMsCal.auto_calib_val_max[nand_auto_calib_page[ch]+1][1];
	echobuffer[8] = lMsCal.auto_calib_val_min[nand_auto_calib_page[ch]+1][0];
	echobuffer[9] = lMsCal.auto_calib_val_min[nand_auto_calib_page[ch]+1][1];
	echobuffer[10] = empty;	//empty
	echobuffer[11] = dead;
	echobuffer[12] = nand_auto_calib_page[ch]+1;
	echobuffer[13] = lMsCal.auto_calib_thr_max[nand_auto_calib_page[ch]+1][0];
	echobuffer[14] = lMsCal.auto_calib_thr_max[nand_auto_calib_page[ch]+1][1];
	echobuffer[15] = lMsCal.auto_calib_thr_min[nand_auto_calib_page[ch]+1][0];
	echobuffer[16] = lMsCal.auto_calib_thr_min[nand_auto_calib_page[ch]+1][1];
	echobuffer[17] = empty-lMsCal.auto_calib_result_val;
	echobuffer[18] = val;
	echobuffer[19] = nand_auto_calib_cnt[ch];
	echobuffer[20] = lMsCal.auto_calib_slope[nand_auto_calib_page[ch]+1];
	echobuffer[21] = lMsCal.auto_calib_thr[nand_auto_calib_page[ch]+1];
	echobuffer[22] = lMsCal.auto_calib_fresult;
	echobuffer[23] = lMsCal.auto_calib_result_slope;
	echobuffer[24] = lMsCal.auto_calib_result_thr;
	echobuffer[25] = lMsCal.auto_calib_result_val;
	echobuffer[26] = lMsCal.auto_calib_result_echo_amp;
	echobuffer[27] = MnTST_PrGet_Value(MnTST_OPT_AUTO_RANGE);
	echobuffer[28] = MnTST_PrGet_Value(MnTST_OPT_AUTO_VOLT_MIN);
	echobuffer[29] = MnTST_PrGet_Value(MnTST_OPT_AUTO_VOLT_PEAK);
	echobuffer[30] = MnTST_PrGet_Value(MnTST_OPT_AUTO_THR_MIN);
	echobuffer[31] = MnTST_PrGet_Value(MnTST_OPT_AUTO_THR_MAX);
	echobuffer[32] = MnTST_PrGet_Value(MnTST_OPT_AUTO_LEVEL_RANGE);	
	echobuffer[33] = (MnTST_PrGet_Value(MnTST_OPT_AUTO_VOLT_DIFF)*100)/MnTST_PrGet_Value(MnTST_OPT_AUTO_RANGE);


	BSP_NND_Write_Page_16b(nand_auto_calib_block[ch],nand_auto_calib_page[ch], echobuffer, 1);
	
	HAL_Delay(1);	


	BSP_NND_Read_Page_8b(nand_meta_calib_block[ch], 0,metabuffer, 1);
	HAL_Delay(1);
	BSP_NND_Erase_Block(nand_meta_calib_block[ch], 0);
	HAL_Delay(1);		


	metabuffer[(gain-1)*8+0] = rtc.year;
	metabuffer[(gain-1)*8+1] = rtc.month;
	metabuffer[(gain-1)*8+2] = rtc.day;
	metabuffer[(gain-1)*8+3] = rtc.hours;
	metabuffer[(gain-1)*8+4] = rtc.minutes;
	metabuffer[(gain-1)*8+5] = rtc.seconds;

	if(ch==APP_CH_1)
		metabuffer[(gain-1)*8+6] = nand_auto_calib_block[ch]-400;
	else if(ch==APP_CH_2)
		metabuffer[(gain-1)*8+6] = nand_auto_calib_block[ch]-400;

	metabuffer[(gain-1)*8+7] = nand_auto_calib_page[ch];

	BSP_NND_Write_Page_8b(nand_meta_calib_block[ch],0, metabuffer, 1);	
	HAL_Delay(1);
#if 0

	HAL_Delay(1);	


	I08 sDbg[256];
	_SPRINTF(sDbg, "%d", echobuffer[12]);
	DpFIG_DrwRect(700, 30, 400, 25, _cBGD_SCRN, DpFIG_FILL);
	DpSTR_GuiLeft(700, 30, _cECO_LVL_TITL, _cBGD_SCRN, _fE17HsB, sDbg);		
#endif
}


void NND_Echo_AutoGain_RealSave(U08 ch, U08 gain, U08 cnt)
{
	U16 echobuffer[1024];
	U32 i;
	RTC_TM rtc = RTC_GetTime();

	U16 val  = MnMSR_CalGet_Ch_Value(ch, MnMS1_OPT_SINGLE_AUTO_CAL);
	U16 empty = MnMSR_BaseGet_Ch_Value(ch, MnMS0_OPT_SINGLE_EMPTY);
	U16 dead = MnMSR_BaseGet_Ch_Value(ch, MnMS0_OPT_SINGLE_DEADZONE);

	if(++nand_auto_calib_num[ch]>=1200)
		nand_auto_calib_num[ch] = 1200;

	if(nand_auto_calib_page[ch]==0)
	{
		BSP_NND_Erase_Block(nand_auto_calib_block[ch], 0);
		HAL_Delay(5);
	}


	for(i=0; i<1020; i++)
	{
		echobuffer[i] = SDRAM_aEco_prof_S3[(gain*ADC_BUFF_MAX*6)+cnt*ADC_BUFF_MAX+i];
	}



	echobuffer[0] = rtc.year;	
	echobuffer[1] = rtc.month;
	echobuffer[2] = rtc.day;
	echobuffer[3] = rtc.hours;
	echobuffer[4] = rtc.minutes;
	echobuffer[5] = rtc.seconds;
	echobuffer[6] = lMsCal.auto_calib_val_max[nand_auto_calib_page[ch]+1][0];
	echobuffer[7] = lMsCal.auto_calib_val_max[nand_auto_calib_page[ch]+1][1];
	echobuffer[8] = lMsCal.auto_calib_val_min[nand_auto_calib_page[ch]+1][0];
	echobuffer[9] = lMsCal.auto_calib_val_min[nand_auto_calib_page[ch]+1][1];
	echobuffer[10] = empty;	//empty
	echobuffer[11] = dead;
	echobuffer[12] = nand_auto_calib_page[ch]+1;
	echobuffer[13] = lMsCal.auto_calib_thr_max[nand_auto_calib_page[ch]+1][0];
	echobuffer[14] = lMsCal.auto_calib_thr_max[nand_auto_calib_page[ch]+1][1];
	echobuffer[15] = lMsCal.auto_calib_thr_min[nand_auto_calib_page[ch]+1][0];
	echobuffer[16] = lMsCal.auto_calib_thr_min[nand_auto_calib_page[ch]+1][1];
	echobuffer[17] = empty-lMsCal.auto_calib_result_val;
	echobuffer[18] = val;
	echobuffer[19] = nand_auto_calib_cnt[ch];
	echobuffer[20] = lMsCal.auto_calib_slope[nand_auto_calib_page[ch]+1];
	echobuffer[21] = lMsCal.auto_calib_thr[nand_auto_calib_page[ch]+1];
	echobuffer[22] = lMsCal.auto_calib_fresult;
	echobuffer[23] = lMsCal.auto_calib_result_slope;
	echobuffer[24] = lMsCal.auto_calib_result_thr;
	echobuffer[25] = lMsCal.auto_calib_result_val;
	echobuffer[26] = lMsCal.auto_calib_result_echo_amp;
	echobuffer[27] = MnTST_PrGet_Value(MnTST_OPT_AUTO_RANGE);
	echobuffer[28] = MnTST_PrGet_Value(MnTST_OPT_AUTO_VOLT_MIN);
	echobuffer[29] = MnTST_PrGet_Value(MnTST_OPT_AUTO_VOLT_PEAK);
	echobuffer[30] = MnTST_PrGet_Value(MnTST_OPT_AUTO_THR_MIN);
	echobuffer[31] = MnTST_PrGet_Value(MnTST_OPT_AUTO_THR_MAX);
	echobuffer[32] = MnTST_PrGet_Value(MnTST_OPT_AUTO_LEVEL_RANGE);	
	echobuffer[33] = (MnTST_PrGet_Value(MnTST_OPT_AUTO_VOLT_DIFF)*100)/MnTST_PrGet_Value(MnTST_OPT_AUTO_RANGE);

	HAL_Delay(5);	

	BSP_NND_Write_Page_16b(nand_auto_calib_block[ch],nand_auto_calib_page[ch], echobuffer, 1);

	HAL_Delay(5);		




#if 0
	echobuffer[20] = 5;

	BSP_NND_Read_Page_16b(nand_auto_calib_block[ch],nand_auto_calib_page[ch], echobuffer, 1);
	HAL_Delay(1);

	I08 sDbg[32];

	_SPRINTF(sDbg, "%d %d %d ", echobuffer[20],nand_auto_calib_block[ch],nand_auto_calib_page[ch]);
	DpFIG_DrwRect(150, 90, 400, 25, _cBGD_SCRN, DpFIG_FILL);
	DpSTR_GuiLeft(150, 90, _cECO_LVL_TITL, _cBGD_SCRN, _fE17HsB, sDbg);	
#endif
}



void NND_Echo_Save(U08 ch)
{
	//U16 empty_range = empty*1.1;
	U16 echobuffer[1024];
	U08 metabuffer[2048];
	U08 metabuffer_main[2048];
	//U16 easer[1024];
	U32 i;
	I08 sDbg[32];
	RTC_TM rtc = RTC_GetTime();


	if(++nand_echo_num[ch]>=20000)
		nand_echo_num[ch] = 20000;

	if(nand_echo_page[ch]==0)
	{
		BSP_NND_Erase_Block(nand_echo_block[ch],0);
		HAL_Delay(2);
	}
	for(i=0; i<1020; i++)
	{
		echobuffer[i] = MsCAL_GetVl_EchoBuff(ch,i);
	}

	echobuffer[0] = rtc.year;
	echobuffer[1] = rtc.month;
	echobuffer[2] = rtc.day;
	echobuffer[3] = rtc.hours;
	echobuffer[4] = rtc.minutes;
	echobuffer[5] = rtc.seconds;

	echobuffer[6] =lMsCal.auto_gain_current_max[ch][0];
	echobuffer[7] =lMsCal.auto_gain_current_max[ch][1];
	echobuffer[8] =lMsCal.auto_gain_current_min[ch][0];
	echobuffer[9] =lMsCal.auto_gain_current_min[ch][1];

	

	
	echobuffer[10] = MnMSR_BaseGet_Ch_Value(ch, MnMS0_OPT_SINGLE_EMPTY);	//empty
	echobuffer[11] = MnMSR_BaseGet_Ch_Value(ch, MnMS0_OPT_SINGLE_DEADZONE);
	echobuffer[12] = MnMSR_CalGet_Ch_Value(ch, MnMS1_OPT_SINGLE_ECHO_AMP);
	echobuffer[13] = lMsCal.thr_max_index[ch];
	echobuffer[14] = lMsCal.thr_max[ch];
	echobuffer[15] = lMsCal.thr_min_index[ch];
	echobuffer[16] = lMsCal.thr_min[ch];
	echobuffer[17] = MsCAL_GetVl_TracDist(ch,MsCAL_THR_HEAVY);
	echobuffer[18] = MsCAL_GetVl_RsltSldg(ch, MsCAL_THR_HEAVY);
	echobuffer[19] = 1;
	echobuffer[20] = lMsCal.auto_gain_current_slope[ch];
	echobuffer[21] = MnMSR_Get_Threshold_Ch_Value(ch, MnMS1_OPT_SINGLE_THR_HEAVY);
	echobuffer[22] = 1;
	echobuffer[23] = lMsCal.auto_gain_before_slope[ch];
	echobuffer[24] = MnMSR_Get_Threshold_Ch_Value(ch, MnMS1_OPT_SINGLE_THR_HEAVY);
	echobuffer[25] =  MsCAL_GetVl_RsltSldg(ch, MsCAL_THR_HEAVY);
	echobuffer[26] =  MnMSR_CalGet_Ch_Value(ch, MnMS1_OPT_SINGLE_ECHO_AMP);
	echobuffer[27] = MnTST_PrGet_Value(MnTST_OPT_AUTO_RANGE);
	echobuffer[28] = lMsCal.auto_gain_avg[ch];
	echobuffer[29] = 1;
	echobuffer[30] =1;
	echobuffer[31] = 1;
	echobuffer[32] = 1;
	echobuffer[33] = 1;

	for(i=500; i<560; i++)
		echobuffer[i] = lMsCal.auto_gain_buffer[ch][i-500];


	BSP_NND_Write_Page_16b(nand_echo_block[ch],nand_echo_page[ch], echobuffer, 1);

	HAL_Delay(2);
#if 0

	BSP_NND_Read_Page_16b(nand_echo_block[ch],nand_echo_page[ch], echobuffer, 1);


	_SPRINTF(sDbg, "%d %d %d %d %d %d", rtc.seconds,echobuffer[5],nand_echo_block[ch],nand_echo_page[ch],ch,nand_echo_num[ch]);
	DpFIG_DrwRect(150, 30, 400, 25, _cBGD_SCRN, DpFIG_FILL);
	DpSTR_GuiLeft(150, 30, _cECO_LVL_TITL, _cBGD_SCRN, _fE17HsB, sDbg);
#endif

	if(ch==APP_CH_1)
	{
		MRM_WrWord(_mNAND_CH1_ECHO_BLOCK_L,nand_echo_block[ch]);
		MRM_WrByte(_mNAND_CH1_ECHO_PAGE,nand_echo_page[ch]);
	}
	else if(ch == APP_CH_2)
	{
		MRM_WrWord(_mNAND_CH2_ECHO_BLOCK_L,nand_echo_block[ch]);
		MRM_WrByte(_mNAND_CH2_ECHO_PAGE,nand_echo_page[ch]);
	}

	BSP_NND_Read_Page_8b(190+(200*ch), 0,metabuffer, 1);
	HAL_Delay(2);

	BSP_NND_Erase_Block(190+(200*ch),0);

	HAL_Delay(2);

	metabuffer[nand_meta_buffer[ch]+0] = rtc.year;
	metabuffer[nand_meta_buffer[ch]+1] = rtc.month;
	metabuffer[nand_meta_buffer[ch]+2] = rtc.day;
	metabuffer[nand_meta_buffer[ch]+3] = rtc.hours;
	metabuffer[nand_meta_buffer[ch]+4] = rtc.minutes;
	metabuffer[nand_meta_buffer[ch]+5] = rtc.seconds;

	if(ch==APP_CH_1)
		metabuffer[nand_meta_buffer[ch]+6] = nand_echo_block[ch];
	else if(ch==APP_CH_2)
		metabuffer[nand_meta_buffer[ch]+6] = nand_echo_block[ch]-200;
	metabuffer[nand_meta_buffer[ch]+7] = nand_echo_page[ch];

	BSP_NND_Write_Page_8b(190+(200*ch),0, metabuffer, 1);

	HAL_Delay(2);

	nand_meta_buffer[ch]= nand_meta_buffer[ch]+8;
	if(nand_meta_buffer[ch]>=2000)
	{
		if(nand_meta_page[ch] == 0)
			BSP_NND_Erase_Block(nand_meta_block[ch],nand_meta_page[ch]);
		HAL_Delay(2);
		BSP_NND_Write_Page_8b(nand_meta_block[ch],nand_meta_page[ch], metabuffer, 1);
		HAL_Delay(2);

	
	}



	if(ch==APP_CH_1)
	{
		MRM_WrWord(_mNAND_CH1_ECHO_BLOCK_L,nand_echo_block[ch]);
		MRM_WrByte(_mNAND_CH1_ECHO_PAGE,nand_echo_page[ch]);
		
		MRM_WrWord(_mNAND_CH1_META_BLOCK_L,nand_meta_block[ch]);
		MRM_WrByte(_mNAND_CH1_META_PAGE,nand_meta_page[ch]);
		MRM_WrWord(_mNAND_CH1_META_BUFFER_L, nand_meta_buffer[ch]);

		MRM_WrWord(_mNAND_CH1_ECHO_NUM_L, nand_echo_num[ch]);		

	}
	else if(ch == APP_CH_2)
	{
		MRM_WrWord(_mNAND_CH2_ECHO_BLOCK_L,nand_echo_block[ch]);
		MRM_WrByte(_mNAND_CH2_ECHO_PAGE,nand_echo_page[ch]);

		MRM_WrWord(_mNAND_CH2_META_BLOCK_L,nand_meta_block[ch]);
		MRM_WrByte(_mNAND_CH2_META_PAGE,nand_meta_page[ch]);
		MRM_WrWord(_mNAND_CH2_META_BUFFER_L, nand_meta_buffer[ch]);		

		MRM_WrWord(_mNAND_CH2_ECHO_NUM_L, nand_echo_num[ch]);		
	}


	if(++nand_echo_page[ch]>=64)
	{
		nand_echo_page[ch] = 0;
		nand_echo_block[ch]++;
	}

	if(nand_echo_block[ch]>=170+(200*ch))
	{
		nand_echo_block[ch] = 0+(200*ch);
	}

	if(nand_meta_buffer[ch]>=2000)
	{
		nand_meta_buffer[ch] = 0;
		nand_meta_page[ch]++;
		
		if(nand_meta_page[ch]>=40)
		{
			nand_meta_block[ch]++;
			nand_meta_page[ch] = 0;
		}
		
		if(nand_meta_block[ch] == 193+(200*ch))
		{
			nand_meta_block[ch] = 191+(200*ch);
		}
	}

#if 0
	if(ch==APP_CH_2)
		return;
	BSP_NND_Read_Page_16b(199, 0,metabuffer, 1);

#if 1
	if(nand_meta_page[ch]==0)
		BSP_NND_Erase_Block(199,0);
#endif
	//BSP_NND_Write_Page_16b(199,0, easer, 1);


	for(i=0; i<100; i++)
	{
		metabuffer[i] = nand_cnt5;
	}

	BSP_NND_Write_Page_16b(199, 0,metabuffer, 1);


	nand_cnt4 = gTmr.c.c001ms_cnt-nand_cnt5;

	//HAL_NAND_Reset(&hnand1);
#if 0
	BSP_NND_Read_Page_16b(199, 0,metabuffer, 1);
	BSP_NND_Read_Page_16b(0, 0,echobuffer, 1);
	
	_SPRINTF(sDbg, "%d   %d %d", metabuffer[55], echobuffer[100],echobuffer[0]);
	DpFIG_DrwRect(150, 30, 400, 25, _cBGD_SCRN, DpFIG_FILL);
	DpSTR_GuiLeft(150, 30, _cECO_LVL_TITL, _cBGD_SCRN, _fE17HsB, sDbg);	


	_SPRINTF(sDbg, "%d", nand_cnt4);
	DpFIG_DrwRect(150, 60, 400, 25, _cBGD_SCRN, DpFIG_FILL);
	DpSTR_GuiLeft(150, 60, _cECO_LVL_TITL, _cBGD_SCRN, _fE17HsB, sDbg);	
#endif
#endif
}


void NND_Echo_ProcSave(void)
{
	for(U08 i=0; i<APP_CH_NUM; i++)
	{	
		switch(MnDAT_EchoSavePrGet_SingleValue(i, MnDS1_OPT_SINGLE_SAVEINTERVAL))
		{
			case MnDS1_SAVEINTERVAL_01S:
				NND_Echo_Save(i);
				break;
			case MnDS1_SAVEINTERVAL_10S:
				if(++nnd_echo.save_interval[i]>=10)
				{
					NND_Echo_Save(i);
					nnd_echo.save_interval[i] = 0;
				}
				break;
			case MnDS1_SAVEINTERVAL_01M:
				if(++nnd_echo.save_interval[i]>=60)
				{
					NND_Echo_Save(i);
					nnd_echo.save_interval[i] = 0;
				}
				break;
			case MnDS1_SAVEINTERVAL_10M:
				if(++nnd_echo.save_interval[i]>=600)
				{
					NND_Echo_Save(i);
					nnd_echo.save_interval[i] = 0;
				}
				break;
			case MnDS1_SAVEINTERVAL_01H:
				if(++nnd_echo.save_interval[i]>=3600)
				{
					NND_Echo_Save(i);
					nnd_echo.save_interval[i] = 0;
				}
				break;
		}
	}

}

#if 0
void NND_ChkID(void)
{	
	NAND_ID.Maker_Id = 0;
	I08 sDbg[32];
	U32 val;

	//val =  HAL_NAND_Read_ID(&hnand1, &NAND_ID);	

#if defined(_NAND_DP_DBG_)	
#if 1	
	U16 page_size = 24;
	U08 num_page = 1;
	U32 i;
	U16 j;
	U16 writebuffer[page_size * num_page];
	U16 readbuffer[page_size];

#if 1
	U16 cnt[8][8000];
#endif


	if(nand_cnt == 0)
	{
		RTC_TM rtc = RTC_GetTime();
		nand_year2 = rtc.year;
		nand_mon2 = rtc.month;
		nand_day2 = rtc.day;
		nand_hour2 = rtc.hours;
		nand_min2 = rtc.minutes;
		nand_sec2 = rtc.seconds;
	}

	//nand_cnt5 = gTmr.c.c001ms_cnt;
#if 0
	for(j=0; j<8; j++)
	{
		for(i=0; i<8000; i++)
		{
			cnt[j][i] = 0xffff;
		}
	}
#endif


	//nand_cnt4 = gTmr.c.c001ms_cnt-nand_cnt5;
#if 0
	_SPRINTF(sDbg, "%d   %d", readbuffer[65520], readbuffer[65515]);
	DpFIG_DrwRect(150, 30, 400, 25, _cBGD_SCRN, DpFIG_FILL);
	DpSTR_GuiLeft(150, 30, _cECO_LVL_TITL, _cBGD_SCRN, _fE17HsB, sDbg);	


	_SPRINTF(sDbg, "%d", nand_cnt4);
		
	DpFIG_DrwRect(150, 60, 400, 25, _cBGD_SCRN, DpFIG_FILL);
	DpSTR_GuiLeft(150, 60, _cECO_LVL_TITL, _cBGD_SCRN, _fE17HsB, sDbg); 
#endif
#if 1

	while(nand_cnt<125)
	{
#if 0
		for(i=0; i<1024; i++)
			writebuffer[i] = nand_cnt2;
#endif


#if 0
		if(nand_cnt3==0)
			BSP_NND_Erase_Block(nand_cnt,0);
#endif
		//BSP_NND_Write_Page_16b(nand_cnt,nand_cnt3, writebuffer, num_page);

		//BSP_NND_Erase_Block(0);

		BSP_NND_Read_SpareArea_16b(nand_cnt, nand_cnt3,readbuffer, num_page);

#if 0
		_SPRINTF(sDbg, "%d %d %d %d %d %d %d",nand_cnt,nand_cnt3,writebuffer[512],readbuffer[0],
			readbuffer[1019],readbuffer[1022],readbuffer[1023]);
		DpFIG_DrwRect(150, 90, 400, 25, _cBGD_SCRN, DpFIG_FILL);
		DpSTR_GuiLeft(150, 90, _cECO_LVL_TITL, _cBGD_SCRN, _fE17HsB, sDbg); 
#endif

#if 0
		if(nand_cnt==511)
		{
			if(nand_cnt3==63)
			{
				_SPRINTF(sDbg, "%d %d %d %d",nand_cnt,nand_cnt3,writebuffer[3],readbuffer[3]);
				DpFIG_DrwRect(150, 60, 400, 25, _cBGD_SCRN, DpFIG_FILL);
				DpSTR_GuiLeft(150, 60, _cECO_LVL_TITL, _cBGD_SCRN, _fE17HsB, sDbg); 
			}
		}
#endif
#if 0
		if(nand_cnt3!=0)
		{
			//BSP_NND_Read_Page_16b(nand_cnt, nand_cnt3-1,readbuffer, num_page);
			_SPRINTF(sDbg, "%d %d %d %d",nand_cnt,nand_cnt3-1,writebuffer[nand_cnt],readbuffer[nand_cnt]);
			DpFIG_DrwRect(150, 120, 400, 25, _cBGD_SCRN, DpFIG_FILL);
			DpSTR_GuiLeft(150, 120, _cECO_LVL_TITL, _cBGD_SCRN, _fE17HsB, sDbg); 
		}
#endif


		if(++nand_cnt3 == 64)
		{
			nand_cnt3=0;
			nand_cnt++;
		}

		if((nand_cnt == 125))
		{
#if 0
			for(j=0; j<8; j++)
			{
				for(i=0; i<8000; i++)
				{
					cnt[j][i] = 0xffff;
				}
			}
#endif

			RTC_TM rtc = RTC_GetTime();
			nand_year = rtc.year;
			nand_mon = rtc.month;
			nand_day = rtc.day;
			nand_hour = rtc.hours;
			nand_min = rtc.minutes;
			nand_sec = rtc.seconds;
			nand_cnt4 = gTmr.c.c001ms_cnt;
		}
		if(nand_cnt2 == 65535)
			nand_cnt2 = 65530;

		nand_cnt2++;
	}


	_SPRINTF(sDbg, "20%02d - %d - %d   %2d:%02d:%02d %d", nand_year, nand_mon, nand_day, nand_hour,nand_min,nand_sec,nand_cnt4);
		
	DpFIG_DrwRect(150, 90, 400, 25, _cBGD_SCRN, DpFIG_FILL);
	DpSTR_GuiLeft(150, 90, _cECO_LVL_TITL, _cBGD_SCRN, _fE17HsB, sDbg); 

	_SPRINTF(sDbg, "20%02d - %d - %d   %2d:%02d:%02d %d", nand_year2, nand_mon2, nand_day2, nand_hour2,nand_min2,nand_sec2,nand_cnt5);
		
	DpFIG_DrwRect(150, 60, 400, 25, _cBGD_SCRN, DpFIG_FILL);
	DpSTR_GuiLeft(150, 60, _cECO_LVL_TITL, _cBGD_SCRN, _fE17HsB, sDbg); 	
#endif

#else
	BSP_NND_Erase_Block(640);
	val = HAL_NAND_Erase_Block(&hnand1, &WriteReadAddr);


	_SPRINTF(sDbg, "%d", val);
	DpFIG_DrwRect(150, 120, 400, 25, _cBGD_SCRN, DpFIG_FILL);
	DpSTR_GuiLeft(150, 120, _cECO_LVL_TITL, _cBGD_SCRN, _fE17HsB, sDbg); 
	//NAND_Read_Page_16b(&hnand1, &WriteReadAddr, pBuffer, NumPage);
	val =  HAL_NAND_Read_ID(&hnand1, &NAND_ID);	
	if ((NAND_ID.Maker_Id == 0x2C) && (NAND_ID.Device_Id == 0xF1) && (NAND_ID.Third_Id == 0x80) 
		  && (NAND_ID.Fourth_Id == 0x95))
	{
		_SPRINTF(sDbg, "MT29F1G08ABAEA");
		DpFIG_DrwRect(150, 30, 400, 25, _cBGD_SCRN, DpFIG_FILL);
		DpSTR_GuiLeft(150, 30, _cECO_LVL_TITL, _cBGD_SCRN, _fE17HsB, sDbg); 
	}
	else
	{
		nand_cnt++;
		_SPRINTF(sDbg, "%d",nand_cnt);
		DpFIG_DrwRect(150, 60, 400, 25, _cBGD_SCRN, DpFIG_FILL);
		DpSTR_GuiLeft(150, 60, _cECO_LVL_TITL, _cBGD_SCRN, _fE17HsB, sDbg); 
		if(val == 0x02U)
			nand_cnt2++;
		else if (val == 0x00U)
			nand_cnt3++;
		
		_SPRINTF(sDbg, "%d %d %d",val,nand_cnt2,nand_cnt3);
		DpFIG_DrwRect(150, 90, 400, 25, _cBGD_SCRN, DpFIG_FILL);
		DpSTR_GuiLeft(150, 90, _cECO_LVL_TITL, _cBGD_SCRN, _fE17HsB, sDbg); 

		if(nand_cnt >= 0xfffffff0)
			nand_cnt = 0xfffffff0;

		if(nand_cnt2 >= 0xfffffff0)
			nand_cnt2 = 0xfffffff0;

		if(nand_cnt3 >= 0xfffffff0)
			nand_cnt3 = 0xfffffff0;

		LCD_DisplayStringAt(0, 24*5, (U08 *)STR_Num2Str(NAND_ID.Maker_Id, 3, 0, 0), DP_ALIGN_LEFT);
		LCD_DisplayStringAt(0, 24*6, (U08 *)STR_Num2Str(NAND_ID.Device_Id, 3, 0, 0), DP_ALIGN_LEFT);
		LCD_DisplayStringAt(0, 24*7, (U08 *)STR_Num2Str(NAND_ID.Third_Id, 3, 0, 0), DP_ALIGN_LEFT);
		LCD_DisplayStringAt(0, 24*8, (U08 *)STR_Num2Str(NAND_ID.Fourth_Id, 3, 0, 0), DP_ALIGN_LEFT);
	}

	MX_FMC_Init();
	NND_InitMain();
#endif

#endif
}
#endif

#if 0
	if(!MnEGN_PrGet_Item(MnEGN_I03_fDP_DBG))
		return;
#endif
#if 0
	_SPRINTF(sDbg, "%d   %d", lMsCal.rslt_dist[0][0], lMsCal.rslt_dist[0][1]);
	DpFIG_DrwRect(150, 30, 400, 25, _cBGD_SCRN, DpFIG_FILL);
	DpSTR_GuiLeft(150, 30, _cECO_LVL_TITL, _cBGD_SCRN, _fE17HsB, sDbg);	
#endif



//------------------------------------------------------------------------------------------------------------------------------
//  EOF
//------------------------------------------------------------------------------------------------------------------------------

