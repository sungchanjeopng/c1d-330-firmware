//--------------------------------------------------------------------------------------------------
//  Copyright (C) 2022, WESS-Global Inc., All Rights Reserved.
//
//  The information contained herein is confidential property of WESS-Global Inc. The use,
//  copying, transfer or disclosure of such information is prohibited except by express written
//  agreement with WESS-Global Co.,LTD
//
//  $Date: 2022-05-26 12:00:00 +0900 $
//--------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
//  Includes
//--------------------------------------------------------------------------------------------------
// Standard
#include <string.h>
// Core
#include "usart.h"
// self
#include "bsp_uart.h"
#include "bsp_gpio.h"
#include "bsp_demux.h"
// menu
#include "menu_data.h"
// data
#include "data_commu.h"

#include "stm32f7xx_hal_uart.h"

#include "main.h"
//--------------------------------------------------------------------------------------------------
//  Global variables
//--------------------------------------------------------------------------------------------------



//--------------------------------------------------------------------------------------------------
//  Local variables
//--------------------------------------------------------------------------------------------------
URT_LS lUrt;

//--------------------------------------------------------------------------------------------------
//  Local Funtions
//--------------------------------------------------------------------------------------------------
void UrtPrc_Wr485(U08* args, U16 arg_size)
{   
    HAL_GPIO_WritePin(USART3_TX_EN_GPIO_Port, USART3_TX_EN_Pin, GPIO_PIN_SET);
	HAL_UART_Transmit(&huart3,(U08*) args, arg_size, 1000); 
	HAL_GPIO_WritePin(USART3_TX_EN_GPIO_Port, USART3_TX_EN_Pin, GPIO_PIN_RESET);
}

void UrtPrc_Wr232(U08* args, U16 arg_size)
{
    HAL_UART_Transmit(&huart2,(U08*) args, arg_size, 1000);  
}


void UrtPrc_WrRF(U08* args, U16 arg_size)
{
	HAL_GPIO_WritePin(GPIOG, GPIO_PIN_14, GPIO_PIN_SET);
	HAL_UART_Transmit(&huart6,(U08*) args, arg_size, 1000); 
	HAL_GPIO_WritePin(GPIOG, GPIO_PIN_14, GPIO_PIN_RESET);
}

void UrtPrc_WrBT(U08* args, U16 arg_size)
{
    HAL_UART_Transmit(&huart1,(U08*) args, arg_size, 1000);
}

//--------------------------------------------------------------------------------------------------
//  Global APIs
//--------------------------------------------------------------------------------------------------
#if 0
U08 URT_GetBuf_cnt(URT_IDX idx)					{	return lUrt.rx_param[idx].buf_cnt; }
U08 URT_GetBuf(URT_IDX idx, U08 buf_idx)		{	return lUrt.rx_param[idx].buf[buf_idx]; }
U08* URT_GetAddr(URT_IDX idx)					{    return lUrt.rx_param[idx].buf;				}
#endif


void URT_InitMain(U08 idx, U32 br)
{
	UART_HandleTypeDef *huart;

	HAL_GPIO_WritePin(USART3_TX_EN_GPIO_Port, USART3_TX_EN_Pin, GPIO_PIN_RESET);

	switch(idx)
	{
		case URT_IDX_1:		huart = &huart2;	huart->Init.BaudRate = 115200;	break;
		case URT_IDX_2:		huart = &huart3;	huart->Init.BaudRate = br;		break;
		case URT_IDX_RF:	huart = &huart6;	huart->Init.BaudRate = br;		DMX_SetIo(DMX_OUT_RF_RESETIN, GIO_LV_HI);	break;
		case URT_IDX_BT:	huart = &huart1;	huart->Init.BaudRate = br;	break;
		default:
			break;
	}

    if(HAL_UART_Init(huart) != HAL_OK)
        Error_Handler();

	__HAL_UART_ENABLE_IT(&huart1, UART_IT_RXNE);
	__HAL_UART_ENABLE_IT(&huart2, UART_IT_RXNE);
	__HAL_UART_ENABLE_IT(&huart3, UART_IT_RXNE);

}

void URT_TxPkt(U08 idx, U08* args, U16 size)
{
	switch(idx)
	{
		case URT_IDX_1:		
			__HAL_UART_DISABLE_IT(&huart2, UART_IT_RXNE);
			UrtPrc_Wr232(args, size);	
			__HAL_UART_ENABLE_IT(&huart2, UART_IT_RXNE);
			break;
		case URT_IDX_2:		
			__HAL_UART_DISABLE_IT(&huart3, UART_IT_RXNE);
			UrtPrc_Wr485(args, size);
			__HAL_UART_ENABLE_IT(&huart3, UART_IT_RXNE);
			break;
		case URT_IDX_RF:	UrtPrc_WrRF(args,size);		break;
		case URT_IDX_BT:
			__HAL_UART_DISABLE_IT(&huart1, UART_IT_RXNE);
			UrtPrc_WrBT(args, size);
			__HAL_UART_ENABLE_IT(&huart1, UART_IT_RXNE);
			break;
		
		default:										break;
					
	}


}

#if 0
void UrtCfg_SetRxBuff(URT_IDX idx, U08 dr)
{
    if(lUrt.rx_param[idx].buf_cnt >= MDB_RX_BUF_SIZE)
        lUrt.rx_param[idx].buf_cnt = 0;
	
    lUrt.rx_param[idx].buf[lUrt.rx_param[idx].buf_cnt] = dr;
    lUrt.rx_param[idx].buf_cnt++;
}
#endif 

void _ISR_URT_Callback(URT_IDX idx, U08 rdr)
{
	U08 buff[2];
	
	switch(idx)
	{
		case URT_IDX_1:		
		case URT_IDX_2:		
			DaMdb_SetRxBuff(idx, rdr);
			break;
		case URT_IDX_BT:
			DaBT_SetRxBuff(idx, rdr);
			break;
		default:
			break;		
	}
}







