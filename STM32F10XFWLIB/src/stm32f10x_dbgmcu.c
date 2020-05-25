/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : stm32f10x_dbgmcu.c
* Author             : MCD Application Team
* Version            : V2.0
* Date               : 05/23/2008
* Description        : This file provides all the DBGMCU firmware functions.
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH SOFTWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
* FOR MORE INFORMATION PLEASE CAREFULLY READ THE LICENSE AGREEMENT FILE LOCATED 
* IN THE ROOT DIRECTORY OF THIS FIRMWARE PACKAGE.
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_dbgmcu.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define IDCODE_DEVID_Mask    ((u32)0x00000FFF)

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
* Function Name  : DBGMCU_GetREVID
* Description    : Returns the device revision identifier.
* Input          : None
* Output         : None
* Return         : Device revision identifier
*******************************************************************************/
u32 DBGMCU_GetREVID(void)
{
   return(DBGMCU->IDCODE >> 16);
}

/*******************************************************************************
* Function Name  : DBGMCU_GetDEVID
* Description    : Returns the device identifier.
* Input          : None
* Output         : None
* Return         : Device identifier
*******************************************************************************/
u32 DBGMCU_GetDEVID(void)
{
   return(DBGMCU->IDCODE & IDCODE_DEVID_Mask);
}

/*******************************************************************************
*函数名称:DBGMCU_Config
*描述:配置指定的外围和低功率模式的行为
*当单片机在调试模式。
*输入:- DBGMCU_Periph:指定周边和低功率模式。
*这个参数可以是任何组合的下列值:
* - DBGMCU_SLEEP:让调试器连接在睡眠模式
* - DBGMCU_STOP:让调试器连接在停止模式
* - DBGMCU_STANDBY:让调试器连接在待机模式
* - DBGMCU_IWDG_STOP:调试IWDG停止当核心是停止
* - DBGMCU_WWDG_STOP:调试WWDG停止当核心是停止
* - DBGMCU_TIM1_STOP:TIM1计数器停止当核心是停止
* - DBGMCU_TIM2_STOP:TIM2计数器停止当核心是停止
* - DBGMCU_TIM3_STOP:TIM3计数器停止当核心是停止
* - DBGMCU_TIM4_STOP:TIM4计数器停止当核心是停止
* - DBGMCU_CAN_STOP:调试可以停止当核心是停止
* - DBGMCU_I2C1_SMBUS_TIMEOUT:I2C1 SMBUS超时状态停止
*当核心是停止
* - DBGMCU_I2C2_SMBUS_TIMEOUT:I2C2 SMBUS超时状态停止
*当核心是停止
* - DBGMCU_TIM5_STOP:TIM5计数器停止当核心是停止
* - DBGMCU_TIM6_STOP:TIM6计数器停止当核心是停止
* - DBGMCU_TIM7_STOP:TIM7计数器停止当核心是停止
* - DBGMCU_TIM8_STOP:TIM8计数器停止当核心是停止
* - NewState:新的国家指定的外围在调试模式。
*这个参数可以:启用或禁用。
*输出:没有
*返回:无
*******************************************************************************/
void DBGMCU_Config(u32 DBGMCU_Periph, FunctionalState NewState)
{
  /* Check the parameters */
  assert_param(IS_DBGMCU_PERIPH(DBGMCU_Periph));
  assert_param(IS_FUNCTIONAL_STATE(NewState));

  if (NewState != DISABLE)
  {
    DBGMCU->CR |= DBGMCU_Periph;
  }
  else
  {
    DBGMCU->CR &= ~DBGMCU_Periph;
  }
}

/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/
