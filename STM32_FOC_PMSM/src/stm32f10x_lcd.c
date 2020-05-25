/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : stm32f10x_lcd.c
* Author             : MCD Application Team & IMS Systems Lab
* Date First Issued  : 21/11/07
* Description        : This file includes the LCD driver for AM-240320LTNQW00H 
*                      (LCD_HX8312) and AM-240320L8TNQW00H (LCD_ILI9320) 
*                      Liquid Crystal Display Module of STM3210B-EVAL board.
********************************************************************************
* History:
* 21/11/07 v1.0
* 29/05/08 v2.0
********************************************************************************
* THE PRESENT SOFTWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH SOFTWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*
* THIS SOURCE CODE IS PROTECTED BY A LICENSE.
* FOR MORE INFORMATION PLEASE CAREFULLY READ THE LICENSE AGREEMENT FILE LOCATED
* IN THE ROOT DIRECTORY OF THIS FIRMWARE PACKAGE.
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_lcd.h"
#include "stm32f10x_Timebase.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define LCD_ILI9320  0x9320
#define LCD_HX8312   0x8312

#define START_BYTE  0x70
#define SET_INDEX   0x00
#define READ_STATUS 0x01
#define WRITE_REG   0x02
#define READ_REG    0x03
     
     /* ASCII Table: each character is 16 column (16dots large) 
        and 24 raw (24 dots high) */
     uc16 ASCII_Table[] =
       {
         /* Space ' ' */
         0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
         0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         /* '!' */
         0x0000, 0x0180, 0x0180, 0x0180, 0x0180, 0x0180, 0x0180, 0x0180,
         0x0180, 0x0180, 0x0180, 0x0180, 0x0180, 0x0180, 0x0000, 0x0000, 
         0x0180, 0x0180, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         /* '"' */
         0x0000, 0x0000, 0x00CC, 0x00CC, 0x00CC, 0x00CC, 0x00CC, 0x00CC,
         0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
         0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         /* '#' */
         0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0C60, 0x0C60,
         0x0C60, 0x0630, 0x0630, 0x1FFE, 0x1FFE, 0x0630, 0x0738, 0x0318, 
         0x1FFE, 0x1FFE, 0x0318, 0x0318, 0x018C, 0x018C, 0x018C, 0x0000,
         /* '$' */
         0x0000, 0x0080, 0x03E0, 0x0FF8, 0x0E9C, 0x1C8C, 0x188C, 0x008C,
         0x0098, 0x01F8, 0x07E0, 0x0E80, 0x1C80, 0x188C, 0x188C, 0x189C, 
         0x0CB8, 0x0FF0, 0x03E0, 0x0080, 0x0080, 0x0000, 0x0000, 0x0000,
         /* '%' */
         0x0000, 0x0000, 0x0000, 0x180E, 0x0C1B, 0x0C11, 0x0611, 0x0611,
         0x0311, 0x0311, 0x019B, 0x018E, 0x38C0, 0x6CC0, 0x4460, 0x4460, 
         0x4430, 0x4430, 0x4418, 0x6C18, 0x380C, 0x0000, 0x0000, 0x0000,
         /* '&' */
         0x0000, 0x01E0, 0x03F0, 0x0738, 0x0618, 0x0618, 0x0330, 0x01F0,
         0x00F0, 0x00F8, 0x319C, 0x330E, 0x1E06, 0x1C06, 0x1C06, 0x3F06, 
         0x73FC, 0x21F0, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         /* ''' */
         0x0000, 0x0000, 0x000C, 0x000C, 0x000C, 0x000C, 0x000C, 0x000C,
         0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
         0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         /* '(' */
         0x0000, 0x0200, 0x0300, 0x0180, 0x00C0, 0x00C0, 0x0060, 0x0060,
         0x0030, 0x0030, 0x0030, 0x0030, 0x0030, 0x0030, 0x0030, 0x0030, 
         0x0060, 0x0060, 0x00C0, 0x00C0, 0x0180, 0x0300, 0x0200, 0x0000,
         /* ')' */
         0x0000, 0x0020, 0x0060, 0x00C0, 0x0180, 0x0180, 0x0300, 0x0300,
         0x0600, 0x0600, 0x0600, 0x0600, 0x0600, 0x0600, 0x0600, 0x0600, 
         0x0300, 0x0300, 0x0180, 0x0180, 0x00C0, 0x0060, 0x0020, 0x0000,
         /* '*' */
         0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x00C0, 0x00C0,
         0x06D8, 0x07F8, 0x01E0, 0x0330, 0x0738, 0x0000, 0x0000, 0x0000, 
         0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         /* '+' */
         0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0180, 0x0180,
         0x0180, 0x0180, 0x0180, 0x3FFC, 0x3FFC, 0x0180, 0x0180, 0x0180, 
         0x0180, 0x0180, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         /* ',' */
         0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
         0x0000, 0x0180, 0x0180, 0x0100, 0x0100, 0x0080, 0x0000, 0x0000,
         /* '-' */
         0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         0x0000, 0x0000, 0x0000, 0x0000, 0x07E0, 0x07E0, 0x0000, 0x0000, 
         0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         /* '.' */
         0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
         0x0000, 0x00C0, 0x00C0, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         /* '/' */
         0x0000, 0x0C00, 0x0C00, 0x0600, 0x0600, 0x0600, 0x0300, 0x0300,
         0x0300, 0x0380, 0x0180, 0x0180, 0x0180, 0x00C0, 0x00C0, 0x00C0, 
         0x0060, 0x0060, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         /* '0' */
         0x0000, 0x03E0, 0x07F0, 0x0E38, 0x0C18, 0x180C, 0x180C, 0x180C,
         0x180C, 0x180C, 0x180C, 0x180C, 0x180C, 0x180C, 0x0C18, 0x0E38, 
         0x07F0, 0x03E0, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         /* '1' */
         0x0000, 0x0100, 0x0180, 0x01C0, 0x01F0, 0x0198, 0x0188, 0x0180,
         0x0180, 0x0180, 0x0180, 0x0180, 0x0180, 0x0180, 0x0180, 0x0180, 
         0x0180, 0x0180, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         /* '2' */
         0x0000, 0x03E0, 0x0FF8, 0x0C18, 0x180C, 0x180C, 0x1800, 0x1800,
         0x0C00, 0x0600, 0x0300, 0x0180, 0x00C0, 0x0060, 0x0030, 0x0018, 
         0x1FFC, 0x1FFC, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
         /* '3' */
         0x0000, 0x01E0, 0x07F8, 0x0E18, 0x0C0C, 0x0C0C, 0x0C00, 0x0600,
         0x03C0, 0x07C0, 0x0C00, 0x1800, 0x1800, 0x180C, 0x180C, 0x0C18, 
         0x07F8, 0x03E0, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         /* '4' */
         0x0000, 0x0C00, 0x0E00, 0x0F00, 0x0F00, 0x0D80, 0x0CC0, 0x0C60,
         0x0C60, 0x0C30, 0x0C18, 0x0C0C, 0x3FFC, 0x3FFC, 0x0C00, 0x0C00, 
         0x0C00, 0x0C00, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         /* '5' */
         0x0000, 0x0FF8, 0x0FF8, 0x0018, 0x0018, 0x000C, 0x03EC, 0x07FC,
         0x0E1C, 0x1C00, 0x1800, 0x1800, 0x1800, 0x180C, 0x0C1C, 0x0E18, 
         0x07F8, 0x03E0, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
         /* '6' */
         0x0000, 0x07C0, 0x0FF0, 0x1C38, 0x1818, 0x0018, 0x000C, 0x03CC,
         0x0FEC, 0x0E3C, 0x1C1C, 0x180C, 0x180C, 0x180C, 0x1C18, 0x0E38, 
         0x07F0, 0x03E0, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         /* '7' */
         0x0000, 0x1FFC, 0x1FFC, 0x0C00, 0x0600, 0x0600, 0x0300, 0x0380,
         0x0180, 0x01C0, 0x00C0, 0x00E0, 0x0060, 0x0060, 0x0070, 0x0030, 
         0x0030, 0x0030, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         /* '8' */
         0x0000, 0x03E0, 0x07F0, 0x0E38, 0x0C18, 0x0C18, 0x0C18, 0x0638,
         0x07F0, 0x07F0, 0x0C18, 0x180C, 0x180C, 0x180C, 0x180C, 0x0C38, 
         0x0FF8, 0x03E0, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         /* '9' */
         0x0000, 0x03E0, 0x07F0, 0x0E38, 0x0C1C, 0x180C, 0x180C, 0x180C,
         0x1C1C, 0x1E38, 0x1BF8, 0x19E0, 0x1800, 0x0C00, 0x0C00, 0x0E1C, 
         0x07F8, 0x01F0, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         /* ':' */
         0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0180, 0x0180,
         0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
         0x0180, 0x0180, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         /* ';' */
         0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0180, 0x0180,
         0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
         0x0180, 0x0180, 0x0100, 0x0100, 0x0080, 0x0000, 0x0000, 0x0000,
         /* '<' */
         /*MODIFIED TO DRAW AN ARROW POINTING LEFT*/
         0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         0x1000, 0x1C00, 0x0F80, 0x03E0, 0x7FF8, 0x7FF8, 0x7FF8, 0x03E0, 
         0x0F80, 0x1C00, 0x1000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         /* '=' */
         0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         0x1FF8, 0x0000, 0x0000, 0x0000, 0x1FF8, 0x0000, 0x0000, 0x0000, 
         0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
         /* '>' */
         /*MODIFIED TO DRAW AN ARROW POINTING RIGHT*/
         0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         0x0008, 0x0038, 0x01F0, 0x07C0, 0x1FFE, 0x1FFE, 0x1FFE, 0x07C0, 
         0x01F0, 0x0038, 0x0008, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         /* '?' */
         0x0000, 0x03E0, 0x0FF8, 0x0C18, 0x180C, 0x180C, 0x1800, 0x0C00,
         0x0600, 0x0300, 0x0180, 0x00C0, 0x00C0, 0x00C0, 0x0000, 0x0000, 
         0x00C0, 0x00C0, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         /* '@' */
         0x0000, 0x0000, 0x07E0, 0x1818, 0x2004, 0x29C2, 0x4A22, 0x4411,
         0x4409, 0x4409, 0x4409, 0x2209, 0x1311, 0x0CE2, 0x4002, 0x2004, 
         0x1818, 0x07E0, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,                       
         /* 'A' */
         0x0000, 0x0380, 0x0380, 0x06C0, 0x06C0, 0x06C0, 0x0C60, 0x0C60,
         0x1830, 0x1830, 0x1830, 0x3FF8, 0x3FF8, 0x701C, 0x600C, 0x600C, 
         0xC006, 0xC006, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         /* 'B' */
         0x0000, 0x03FC, 0x0FFC, 0x0C0C, 0x180C, 0x180C, 0x180C, 0x0C0C,
         0x07FC, 0x0FFC, 0x180C, 0x300C, 0x300C, 0x300C, 0x300C, 0x180C, 
         0x1FFC, 0x07FC, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         /* 'C' */
         0x0000, 0x07C0, 0x1FF0, 0x3838, 0x301C, 0x700C, 0x6006, 0x0006,
         0x0006, 0x0006, 0x0006, 0x0006, 0x0006, 0x6006, 0x700C, 0x301C, 
         0x1FF0, 0x07E0, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         /* 'D' */
         0x0000, 0x03FE, 0x0FFE, 0x0E06, 0x1806, 0x1806, 0x3006, 0x3006,
         0x3006, 0x3006, 0x3006, 0x3006, 0x3006, 0x1806, 0x1806, 0x0E06, 
         0x0FFE, 0x03FE, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         /* 'E' */
         0x0000, 0x3FFC, 0x3FFC, 0x000C, 0x000C, 0x000C, 0x000C, 0x000C,
         0x1FFC, 0x1FFC, 0x000C, 0x000C, 0x000C, 0x000C, 0x000C, 0x000C, 
         0x3FFC, 0x3FFC, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         /* 'F' */
         0x0000, 0x3FF8, 0x3FF8, 0x0018, 0x0018, 0x0018, 0x0018, 0x0018,
         0x1FF8, 0x1FF8, 0x0018, 0x0018, 0x0018, 0x0018, 0x0018, 0x0018, 
         0x0018, 0x0018, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         /* 'G' */
         0x0000, 0x0FE0, 0x3FF8, 0x783C, 0x600E, 0xE006, 0xC007, 0x0003,
         0x0003, 0xFE03, 0xFE03, 0xC003, 0xC007, 0xC006, 0xC00E, 0xF03C, 
         0x3FF8, 0x0FE0, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         /* 'H' */
         0x0000, 0x300C, 0x300C, 0x300C, 0x300C, 0x300C, 0x300C, 0x300C,
         0x3FFC, 0x3FFC, 0x300C, 0x300C, 0x300C, 0x300C, 0x300C, 0x300C, 
         0x300C, 0x300C, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
         /* 'I' */
         0x0000, 0x0180, 0x0180, 0x0180, 0x0180, 0x0180, 0x0180, 0x0180,
         0x0180, 0x0180, 0x0180, 0x0180, 0x0180, 0x0180, 0x0180, 0x0180, 
         0x0180, 0x0180, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         /* 'J' */
         0x0000, 0x0600, 0x0600, 0x0600, 0x0600, 0x0600, 0x0600, 0x0600,
         0x0600, 0x0600, 0x0600, 0x0600, 0x0600, 0x0618, 0x0618, 0x0738, 
         0x03F0, 0x01E0, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         /* 'K' */
         0x0000, 0x3006, 0x1806, 0x0C06, 0x0606, 0x0306, 0x0186, 0x00C6,
         0x0066, 0x0076, 0x00DE, 0x018E, 0x0306, 0x0606, 0x0C06, 0x1806, 
         0x3006, 0x6006, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
         /* 'L' */
         0x0000, 0x0018, 0x0018, 0x0018, 0x0018, 0x0018, 0x0018, 0x0018,
         0x0018, 0x0018, 0x0018, 0x0018, 0x0018, 0x0018, 0x0018, 0x0018, 
         0x1FF8, 0x1FF8, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
         /* 'M' */
         0x0000, 0xE00E, 0xF01E, 0xF01E, 0xF01E, 0xD836, 0xD836, 0xD836,
         0xD836, 0xCC66, 0xCC66, 0xCC66, 0xC6C6, 0xC6C6, 0xC6C6, 0xC6C6, 
         0xC386, 0xC386, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         /* 'N' */
         0x0000, 0x300C, 0x301C, 0x303C, 0x303C, 0x306C, 0x306C, 0x30CC,
         0x30CC, 0x318C, 0x330C, 0x330C, 0x360C, 0x360C, 0x3C0C, 0x3C0C, 
         0x380C, 0x300C, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         /* 'O' */
         0x0000, 0x07E0, 0x1FF8, 0x381C, 0x700E, 0x6006, 0xC003, 0xC003,
         0xC003, 0xC003, 0xC003, 0xC003, 0xC003, 0x6006, 0x700E, 0x381C, 
         0x1FF8, 0x07E0, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         /* 'P' */
         0x0000, 0x0FFC, 0x1FFC, 0x380C, 0x300C, 0x300C, 0x300C, 0x300C,
         0x180C, 0x1FFC, 0x07FC, 0x000C, 0x000C, 0x000C, 0x000C, 0x000C, 
         0x000C, 0x000C, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         /* 'Q' */
         0x0000, 0x07E0, 0x1FF8, 0x381C, 0x700E, 0x6006, 0xE003, 0xC003,
         0xC003, 0xC003, 0xC003, 0xC003, 0xE007, 0x6306, 0x3F0E, 0x3C1C, 
         0x3FF8, 0xF7E0, 0xC000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         /* 'R' */
         0x0000, 0x0FFE, 0x1FFE, 0x3806, 0x3006, 0x3006, 0x3006, 0x3806,
         0x1FFE, 0x07FE, 0x0306, 0x0606, 0x0C06, 0x1806, 0x1806, 0x3006, 
         0x3006, 0x6006, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         /* 'S' */
         0x0000, 0x03E0, 0x0FF8, 0x0C1C, 0x180C, 0x180C, 0x000C, 0x001C,
         0x03F8, 0x0FE0, 0x1E00, 0x3800, 0x3006, 0x3006, 0x300E, 0x1C1C, 
         0x0FF8, 0x07E0, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         /* 'T' */
         0x0000, 0x7FFE, 0x7FFE, 0x0180, 0x0180, 0x0180, 0x0180, 0x0180,
         0x0180, 0x0180, 0x0180, 0x0180, 0x0180, 0x0180, 0x0180, 0x0180, 
         0x0180, 0x0180, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         /* 'U' */
         0x0000, 0x300C, 0x300C, 0x300C, 0x300C, 0x300C, 0x300C, 0x300C,
         0x300C, 0x300C, 0x300C, 0x300C, 0x300C, 0x300C, 0x300C, 0x1818, 
         0x1FF8, 0x07E0, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         /* 'V' */
         0x0000, 0x6003, 0x3006, 0x3006, 0x3006, 0x180C, 0x180C, 0x180C,
         0x0C18, 0x0C18, 0x0E38, 0x0630, 0x0630, 0x0770, 0x0360, 0x0360, 
         0x01C0, 0x01C0, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         /* 'W' */
         0x0000, 0x6003, 0x61C3, 0x61C3, 0x61C3, 0x3366, 0x3366, 0x3366,
         0x3366, 0x3366, 0x3366, 0x1B6C, 0x1B6C, 0x1B6C, 0x1A2C, 0x1E3C, 
         0x0E38, 0x0E38, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         /* 'X' */
         0x0000, 0xE00F, 0x700C, 0x3018, 0x1830, 0x0C70, 0x0E60, 0x07C0,
         0x0380, 0x0380, 0x03C0, 0x06E0, 0x0C70, 0x1C30, 0x1818, 0x300C, 
         0x600E, 0xE007, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         /* 'Y' */
         0x0000, 0xC003, 0x6006, 0x300C, 0x381C, 0x1838, 0x0C30, 0x0660,
         0x07E0, 0x03C0, 0x0180, 0x0180, 0x0180, 0x0180, 0x0180, 0x0180, 
         0x0180, 0x0180, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         /* 'Z' */
         0x0000, 0x7FFC, 0x7FFC, 0x6000, 0x3000, 0x1800, 0x0C00, 0x0600,
         0x0300, 0x0180, 0x00C0, 0x0060, 0x0030, 0x0018, 0x000C, 0x0006, 
         0x7FFE, 0x7FFE, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         /* '[' */
         0x0000, 0x03E0, 0x03E0, 0x0060, 0x0060, 0x0060, 0x0060, 0x0060,
         0x0060, 0x0060, 0x0060, 0x0060, 0x0060, 0x0060, 0x0060, 0x0060, 
         0x0060, 0x0060, 0x0060, 0x0060, 0x0060, 0x03E0, 0x03E0, 0x0000,
         /* '\' */
         0x0000, 0x0030, 0x0030, 0x0060, 0x0060, 0x0060, 0x00C0, 0x00C0,
         0x00C0, 0x01C0, 0x0180, 0x0180, 0x0180, 0x0300, 0x0300, 0x0300, 
         0x0600, 0x0600, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         /* ']' */
         0x0000, 0x03E0, 0x03E0, 0x0300, 0x0300, 0x0300, 0x0300, 0x0300,
         0x0300, 0x0300, 0x0300, 0x0300, 0x0300, 0x0300, 0x0300, 0x0300, 
         0x0300, 0x0300, 0x0300, 0x0300, 0x0300, 0x03E0, 0x03E0, 0x0000,
         /* '^' */
         /* MODIFIED TO DRAW AN ARROW UP POINTING */
         0x0000, 0x0180, 0x01C0, 0x01C0, 0x03E0, 0x03E0, 0x03E0, 0x07F0,
         0x07F0, 0x0DD8, 0x0DD8, 0x01C0, 0x01C0, 0x01C0, 0x01C0, 0x01C0, 
         0x01C0, 0x01C0, 0x01C0, 0x01C0, 0x01C0, 0x01C0, 0x01C0, 0x0000,
         /* '_' */
         0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
         0x0000, 0xFFFF, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         /* ''' */
         0x0000, 0x000C, 0x000C, 0x000C, 0x000C, 0x000C, 0x000C, 0x0000,
         0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
         0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         /* 'a' */
         0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x03F0, 0x07F8, 
         0x0C1C, 0x0C0C, 0x0F00, 0x0FF0, 0x0CF8, 0x0C0C, 0x0C0C, 0x0F1C, 
         0x0FF8, 0x18F0, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         /* 'b' */
         0x0000, 0x0018, 0x0018, 0x0018, 0x0018, 0x0018, 0x03D8, 0x0FF8, 
         0x0C38, 0x1818, 0x1818, 0x1818, 0x1818, 0x1818, 0x1818, 0x0C38, 
         0x0FF8, 0x03D8, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         /* 'c' */
         0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x03C0, 0x07F0, 
         0x0E30, 0x0C18, 0x0018, 0x0018, 0x0018, 0x0018, 0x0C18, 0x0E30, 
         0x07F0, 0x03C0, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         /* 'd' */
         0x0000, 0x1800, 0x1800, 0x1800, 0x1800, 0x1800, 0x1BC0, 0x1FF0, 
         0x1C30, 0x1818, 0x1818, 0x1818, 0x1818, 0x1818, 0x1818, 0x1C30, 
         0x1FF0, 0x1BC0, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         /* 'e' */
         0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x03C0, 0x0FF0, 
         0x0C30, 0x1818, 0x1FF8, 0x1FF8, 0x0018, 0x0018, 0x1838, 0x1C30, 
         0x0FF0, 0x07C0, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         /* 'f' */
         0x0000, 0x0F80, 0x0FC0, 0x00C0, 0x00C0, 0x00C0, 0x07F0, 0x07F0, 
         0x00C0, 0x00C0, 0x00C0, 0x00C0, 0x00C0, 0x00C0, 0x00C0, 0x00C0, 
         0x00C0, 0x00C0, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         /* 'g' */
         0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0DE0, 0x0FF8, 
         0x0E18, 0x0C0C, 0x0C0C, 0x0C0C, 0x0C0C, 0x0C0C, 0x0C0C, 0x0E18, 
         0x0FF8, 0x0DE0, 0x0C00, 0x0C0C, 0x061C, 0x07F8, 0x01F0, 0x0000,
         /* 'h' */
         0x0000, 0x0018, 0x0018, 0x0018, 0x0018, 0x0018, 0x07D8, 0x0FF8, 
         0x1C38, 0x1818, 0x1818, 0x1818, 0x1818, 0x1818, 0x1818, 0x1818, 
         0x1818, 0x1818, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         /* 'i' */
         0x0000, 0x00C0, 0x00C0, 0x0000, 0x0000, 0x0000, 0x00C0, 0x00C0, 
         0x00C0, 0x00C0, 0x00C0, 0x00C0, 0x00C0, 0x00C0, 0x00C0, 0x00C0, 
         0x00C0, 0x00C0, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         /* 'j' */
         0x0000, 0x00C0, 0x00C0, 0x0000, 0x0000, 0x0000, 0x00C0, 0x00C0, 
         0x00C0, 0x00C0, 0x00C0, 0x00C0, 0x00C0, 0x00C0, 0x00C0, 0x00C0, 
         0x00C0, 0x00C0, 0x00C0, 0x00C0, 0x00C0, 0x00F8, 0x0078, 0x0000,
         /* 'k' */
         0x0000, 0x000C, 0x000C, 0x000C, 0x000C, 0x000C, 0x0C0C, 0x060C, 
         0x030C, 0x018C, 0x00CC, 0x006C, 0x00FC, 0x019C, 0x038C, 0x030C, 
         0x060C, 0x0C0C, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         /* 'l' */
         0x0000, 0x00C0, 0x00C0, 0x00C0, 0x00C0, 0x00C0, 0x00C0, 0x00C0, 
         0x00C0, 0x00C0, 0x00C0, 0x00C0, 0x00C0, 0x00C0, 0x00C0, 0x00C0, 
         0x00C0, 0x00C0, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         /* 'm' */
         0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x3C7C, 0x7EFF, 
         0xE3C7, 0xC183, 0xC183, 0xC183, 0xC183, 0xC183, 0xC183, 0xC183, 
         0xC183, 0xC183, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         /* 'n' */
         0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0798, 0x0FF8, 
         0x1C38, 0x1818, 0x1818, 0x1818, 0x1818, 0x1818, 0x1818, 0x1818, 
         0x1818, 0x1818, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         /* 'o' */
         0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x03C0, 0x0FF0, 
         0x0C30, 0x1818, 0x1818, 0x1818, 0x1818, 0x1818, 0x1818, 0x0C30, 
         0x0FF0, 0x03C0, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         /* 'p' */
         0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x03D8, 0x0FF8, 
         0x0C38, 0x1818, 0x1818, 0x1818, 0x1818, 0x1818, 0x1818, 0x0C38, 
         0x0FF8, 0x03D8, 0x0018, 0x0018, 0x0018, 0x0018, 0x0018, 0x0000,
         /* 'q' */
         0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x1BC0, 0x1FF0, 
         0x1C30, 0x1818, 0x1818, 0x1818, 0x1818, 0x1818, 0x1818, 0x1C30, 
         0x1FF0, 0x1BC0, 0x1800, 0x1800, 0x1800, 0x1800, 0x1800, 0x0000,
         /* 'r' */
         0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x07B0, 0x03F0, 
         0x0070, 0x0030, 0x0030, 0x0030, 0x0030, 0x0030, 0x0030, 0x0030, 
         0x0030, 0x0030, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         /* 's' */
         0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x03E0, 0x03F0, 
         0x0E38, 0x0C18, 0x0038, 0x03F0, 0x07C0, 0x0C00, 0x0C18, 0x0E38, 
         0x07F0, 0x03E0, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         /* 't' */
         0x0000, 0x0000, 0x0080, 0x00C0, 0x00C0, 0x00C0, 0x07F0, 0x07F0, 
         0x00C0, 0x00C0, 0x00C0, 0x00C0, 0x00C0, 0x00C0, 0x00C0, 0x00C0, 
         0x07C0, 0x0780, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         /* 'u' */
         0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x1818, 0x1818, 
         0x1818, 0x1818, 0x1818, 0x1818, 0x1818, 0x1818, 0x1818, 0x1C38, 
         0x1FF0, 0x19E0, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         /* 'v' */
         0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x180C, 0x0C18, 
         0x0C18, 0x0C18, 0x0630, 0x0630, 0x0630, 0x0360, 0x0360, 0x0360, 
         0x01C0, 0x01C0, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         /* 'w' */
         0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x41C1, 0x41C1, 
         0x61C3, 0x6363, 0x6363, 0x6363, 0x3636, 0x3636, 0x3636, 0x1C1C, 
         0x1C1C, 0x1C1C, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         /* 'x' */
         0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x381C, 0x1C38, 
         0x0C30, 0x0660, 0x0360, 0x0360, 0x0360, 0x0360, 0x0660, 0x0C30, 
         0x1C38, 0x381C, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         /* 'y' */
         0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x3018, 0x1830, 
         0x1830, 0x1870, 0x0C60, 0x0C60, 0x0CE0, 0x06C0, 0x06C0, 0x0380, 
         0x0380, 0x0380, 0x0180, 0x0180, 0x01C0, 0x00F0, 0x0070, 0x0000,
         /* 'z' */
         0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x1FFC, 0x1FFC, 
         0x0C00, 0x0600, 0x0300, 0x0180, 0x00C0, 0x0060, 0x0030, 0x0018, 
         0x1FFC, 0x1FFC, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
         /* '{' */
         0x0000, 0x0300, 0x0180, 0x00C0, 0x00C0, 0x00C0, 0x00C0, 0x00C0, 
         0x00C0, 0x0060, 0x0060, 0x0030, 0x0060, 0x0040, 0x00C0, 0x00C0, 
         0x00C0, 0x00C0, 0x00C0, 0x00C0, 0x0180, 0x0300, 0x0000, 0x0000,
         /* '|' */ 
         /* MODIFIED TO DRAW AN ARROW DOWN POINTING */
         0x0000, 0x01C0, 0x01C0, 0x01C0, 0x01C0, 0x01C0, 0x01C0, 0x01C0, 
         0x01C0, 0x01C0, 0x01C0, 0x01C0, 0x01C0, 0x0DD8, 0x0DD8, 0x07F0, 
         0x07F0, 0x03E0, 0x03E0, 0x03E0, 0x01C0, 0x01C0, 0x0180, 0x0000,
         /* '}' */
         0x0000, 0x0060, 0x00C0, 0x01C0, 0x0180, 0x0180, 0x0180, 0x0180, 
         0x0180, 0x0300, 0x0300, 0x0600, 0x0300, 0x0100, 0x0180, 0x0180, 
         0x0180, 0x0180, 0x0180, 0x0180, 0x00C0, 0x0060, 0x0000, 0x0000,
         /* '~' */
         0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
         0x10F0, 0x1FF8, 0x0F08, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
         0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
      };
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
  /* Global variables to set the written text color */
static  vu16 TextColor = 0x0000, BackColor = 0xFFFF;
static vu32 LCDType = LCD_ILI9320;

/* Private function prototypes -----------------------------------------------*/
static void LCD_WriteRegHX8312(u8 LCD_Reg, u8 LCD_RegValue);
static void LCD_WriteRegILI9320(u8 LCD_Reg, u16 LCD_RegValue);

/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
* Function Name  : LCD_Setup
* Description    : Setups the LCD.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_Setup(void)
{ 
/* Configure the LCD Control pins --------------------------------------------*/
  LCD_CtrlLinesConfig();
  
/* Configure the SPI2 interface ----------------------------------------------*/
  LCD_SPIConfig();

  if(LCDType == LCD_ILI9320)
  {
    TB_Wait(100); /* Delay 50 ms */
    /* Start Initial Sequence ------------------------------------------------*/
    LCD_WriteReg(R229, 0x8000); /* Set the internal vcore voltage */
    LCD_WriteReg(R0,  0x0001); /* Start internal OSC. */
    LCD_WriteReg(R1,  0x0100); /* set SS and SM bit */
    LCD_WriteReg(R2,  0x0700); /* set 1 line inversion */
    LCD_WriteReg(R3,  0x1030); /* set GRAM write direction and BGR=1. */
    LCD_WriteReg(R4,  0x0000); /* Resize register */
    LCD_WriteReg(R8,  0x0202); /* set the back porch and front porch */
    LCD_WriteReg(R9,  0x0000); /* set non-display area refresh cycle ISC[3:0] */
    LCD_WriteReg(R10, 0x0000); /* FMARK function */
    LCD_WriteReg(R12, 0x0000); /* RGB interface setting */
    LCD_WriteReg(R13, 0x0000); /* Frame marker Position */
    LCD_WriteReg(R15, 0x0000); /* RGB interface polarity */

    /* Power On sequence -----------------------------------------------------*/
    LCD_WriteReg(R16, 0x0000); /* SAP, BT[3:0], AP, DSTB, SLP, STB */
    LCD_WriteReg(R17, 0x0000); /* DC1[2:0], DC0[2:0], VC[2:0] */
    LCD_WriteReg(R18, 0x0000); /* VREG1OUT voltage */
    LCD_WriteReg(R19, 0x0000); /* VDV[4:0] for VCOM amplitude */
    TB_Wait(400);             /* Dis-charge capacitor power voltage (200ms) */
    LCD_WriteReg(R16, 0x17B0); /* SAP, BT[3:0], AP, DSTB, SLP, STB */
    LCD_WriteReg(R17, 0x0137); /* DC1[2:0], DC0[2:0], VC[2:0] */
    TB_Wait(100);                /* Delay 50 ms */
    LCD_WriteReg(R18, 0x0139); /* VREG1OUT voltage */
    TB_Wait(100);                /* Delay 50 ms */
    LCD_WriteReg(R19, 0x1d00); /* VDV[4:0] for VCOM amplitude */
    LCD_WriteReg(R41, 0x0013); /* VCM[4:0] for VCOMH */
    TB_Wait(100);                /* Delay 50 ms */
    LCD_WriteReg(R32, 0x0000); /* GRAM horizontal Address */
    LCD_WriteReg(R33, 0x0000); /* GRAM Vertical Address */

    /* Adjust the Gamma Curve ------------------------------------------------*/
    LCD_WriteReg(R48, 0x0006);
    LCD_WriteReg(R49, 0x0101);
    LCD_WriteReg(R50, 0x0003);
    LCD_WriteReg(R53, 0x0106);
    LCD_WriteReg(R54, 0x0b02);
    LCD_WriteReg(R55, 0x0302);
    LCD_WriteReg(R56, 0x0707);
    LCD_WriteReg(R57, 0x0007);
    LCD_WriteReg(R60, 0x0600);
    LCD_WriteReg(R61, 0x020b);
  
    /* Set GRAM area ---------------------------------------------------------*/
    LCD_WriteReg(R80, 0x0000); /* Horizontal GRAM Start Address */
    LCD_WriteReg(R81, 0x00EF); /* Horizontal GRAM End Address */
    LCD_WriteReg(R82, 0x0000); /* Vertical GRAM Start Address */
    LCD_WriteReg(R83, 0x013F); /* Vertical GRAM End Address */

    LCD_WriteReg(R96,  0x2700); /* Gate Scan Line */
    LCD_WriteReg(R97,  0x0001); /* NDL,VLE, REV */
    LCD_WriteReg(R106, 0x0000); /* set scrolling line */

    /* Partial Display Control -----------------------------------------------*/
    LCD_WriteReg(R128, 0x0000);
    LCD_WriteReg(R129, 0x0000);
    LCD_WriteReg(R130, 0x0000);
    LCD_WriteReg(R131, 0x0000);
    LCD_WriteReg(R132, 0x0000);
    LCD_WriteReg(R133, 0x0000);

    /* Panel Control ---------------------------------------------------------*/
    LCD_WriteReg(R144, 0x0010);
    LCD_WriteReg(R146, 0x0000);
    LCD_WriteReg(R147, 0x0003);
    LCD_WriteReg(R149, 0x0110);
    LCD_WriteReg(R151, 0x0000);
    LCD_WriteReg(R152, 0x0000);

    /* Set GRAM write direction and BGR = 1 */
    /* I/D=01 (Horizontal : increment, Vertical : decrement) */
    /* AM=1 (address is updated in vertical writing direction) */
    LCD_WriteReg(R3, 0x1018);

    LCD_WriteReg(R7, 0x0173); /* 262K color and display ON */
  }
  else if(LCDType == LCD_HX8312)
  {
    /* Enable the LCD Oscillator ---------------------------------------------*/
    LCD_WriteReg(R1, 0x10);
    LCD_WriteReg(R0, 0xA0);
    LCD_WriteReg(R3, 0x01);
    TB_Wait(20); /* Delay 10 ms */
    LCD_WriteReg(R3, 0x00);
    LCD_WriteReg(R43, 0x04);
  
    LCD_WriteReg(R40, 0x18);
    LCD_WriteReg(R26, 0x05);
    LCD_WriteReg(R37, 0x05);
    LCD_WriteReg(R25, 0x00);
    
/* LCD Power On --------------------------------------------------------------*/
    LCD_WriteReg(R28, 0x73);
    LCD_WriteReg(R36, 0x74);
    LCD_WriteReg(R30, 0x01);
    LCD_WriteReg(R24, 0xC1);
    TB_Wait(20); /* Delay 10 ms */
    LCD_WriteReg(R24, 0xE1);
    LCD_WriteReg(R24, 0xF1);
    TB_Wait(120); /* Delay 60 ms */
    LCD_WriteReg(R24, 0xF5);
    TB_Wait(120); /* Delay 60 ms */
    LCD_WriteReg(R27, 0x09);
    TB_Wait(620); /* Delay 10 ms */
    LCD_WriteReg(R31, 0x11);
    LCD_WriteReg(R32, 0x0E);
    LCD_WriteReg(R30, 0x81);
    TB_Wait(20); /* Delay 10 ms */
    
    /* Chip Set --------------------------------------------------------------*/
    LCD_WriteReg(R157, 0x00);
    LCD_WriteReg(R192, 0x00);
   
    LCD_WriteReg(R14, 0x00);
    LCD_WriteReg(R15, 0x00);
    LCD_WriteReg(R16, 0x00);
    LCD_WriteReg(R17, 0x00);
    LCD_WriteReg(R18, 0x00);
    LCD_WriteReg(R19, 0x00);
    LCD_WriteReg(R20, 0x00);
    LCD_WriteReg(R21, 0x00);
    LCD_WriteReg(R22, 0x00);
    LCD_WriteReg(R23, 0x00);
   
    LCD_WriteReg(R52, 0x01);
    LCD_WriteReg(R53, 0x00);

    LCD_WriteReg(R75, 0x00);
    LCD_WriteReg(R76, 0x00);
    LCD_WriteReg(R78, 0x00);
    LCD_WriteReg(R79, 0x00);
    LCD_WriteReg(R80, 0x00);
  
    LCD_WriteReg(R60, 0x00);
    LCD_WriteReg(R61, 0x00);
    LCD_WriteReg(R62, 0x01);
    LCD_WriteReg(R63, 0x3F);
    LCD_WriteReg(R64, 0x02);
    LCD_WriteReg(R65, 0x02);
    LCD_WriteReg(R66, 0x00);
    LCD_WriteReg(R67, 0x00);
    LCD_WriteReg(R68, 0x00);
    LCD_WriteReg(R69, 0x00);
    LCD_WriteReg(R70, 0xEF);
    LCD_WriteReg(R71, 0x00);
    LCD_WriteReg(R72, 0x00);
    LCD_WriteReg(R73, 0x01);
    LCD_WriteReg(R74, 0x3F);
  
    LCD_WriteReg(R29, 0x08);  /* R29:Gate scan direction setting */
  
    LCD_WriteReg(R134, 0x00);
    LCD_WriteReg(R135, 0x30);
    LCD_WriteReg(R136, 0x02);
    LCD_WriteReg(R137, 0x05);
  
    LCD_WriteReg(R141, 0x01);  /* R141:Register set-up mode for one line clock */
    LCD_WriteReg(R139, 0x20);  /* R139:One line SYSCLK number in one-line */
    LCD_WriteReg(R51, 0x01);  /* R51:N line inversion setting */
    LCD_WriteReg(R55, 0x01);  /* R55:Scanning method setting */
    LCD_WriteReg(R118, 0x00);
   
    /* Gamma Set -------------------------------------------------------------*/
    LCD_WriteReg(R143, 0x10);
    LCD_WriteReg(R144, 0x67);
    LCD_WriteReg(R145, 0x07);
    LCD_WriteReg(R146, 0x65);
    LCD_WriteReg(R147, 0x07);
    LCD_WriteReg(R148, 0x01);
    LCD_WriteReg(R149, 0x76);
    LCD_WriteReg(R150, 0x56);
    LCD_WriteReg(R151, 0x00);
    LCD_WriteReg(R152, 0x06);
    LCD_WriteReg(R153, 0x03);
    LCD_WriteReg(R154, 0x00);
  
    /* Display On ------------------------------------------------------------*/
    LCD_WriteReg(R1, 0x50);
    LCD_WriteReg(R5, 0x04);

    LCD_WriteReg(R0, 0x80);
    LCD_WriteReg(R59, 0x01);
    TB_Wait(80);  /* Delay 40 ms */
    LCD_WriteReg(R0, 0x20);
  }  
}

/*******************************************************************************
* Function Name  : LCD_Init
* Description    : Initializes the LCD.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_Init(void)
{
  /* Setups the LCD */
  LCD_Setup();

  /* Try to read new LCD controller ID 0x9320 */
  if (LCD_ReadReg(R0) == LCD_ILI9320)
  {
    LCDType = LCD_ILI9320;
  }
  else
  {
    LCDType = LCD_HX8312;
    /* Setups the LCD */
    LCD_Setup();
  } 
}

/*******************************************************************************
* Function Name  : LCD_SetTextColor
* Description    : Sets the Text color.
* Input          : - Color: specifies the Text color code RGB(5-6-5).
* Output         : - TextColor: Text color global variable used by LCD_DrawChar
*                  and LCD_DrawPicture functions.
* Return         : None
*******************************************************************************/
void LCD_SetTextColor(vu16 Color)
{
  TextColor = Color;
}

/*******************************************************************************
* Function Name  : LCD_SetBackColor
* Description    : Sets the Background color.
* Input          : - Color: specifies the Background color code RGB(5-6-5).
* Output         : - BackColor: Background color global variable used by 
*                  LCD_DrawChar and LCD_DrawPicture functions.
* Return         : None
*******************************************************************************/
void LCD_SetBackColor(vu16 Color)
{
  BackColor = Color;
}

/*******************************************************************************
* Function Name  : LCD_ClearLine
* Description    : Clears the selected line.
* Input          : - Line: the Line to be cleared.
*                    This parameter can be one of the following values:
*                       - Linex: where x can be 0..9
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_ClearLine(u8 Line)
{
  LCD_DisplayStringLine(Line, "                    ");
}

/*******************************************************************************
* Function Name  : LCD_Clear
* Description    : Clears the hole LCD.
* Input          : Color: the color of the background.
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_Clear(u16 Color)
{
  u32 index = 0;
  
  LCD_SetCursor(0x00, 0x013F); 

  if(LCDType == LCD_ILI9320)
  {
    LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */
  }

  for(index = 0; index < 76800; index++)
  {
    LCD_WriteRAM(Color);
  }

  if(LCDType == LCD_ILI9320)
  {
    LCD_CtrlLinesWrite(GPIOB, CtrlPin_NCS, Bit_SET); 
  }  
}

/*******************************************************************************
* Function Name  : LCD_SetCursor
* Description    : Sets the cursor position.
* Input          : - Xpos: specifies the X position.
*                  - Ypos: specifies the Y position. 
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_SetCursor(u8 Xpos, u16 Ypos)
{
  if(LCDType == LCD_ILI9320)
  {
    LCD_WriteReg(R32, Xpos);
    LCD_WriteReg(R33, Ypos);
  }
  else if(LCDType == LCD_HX8312)
  {
    LCD_WriteReg(R66, Xpos);
    LCD_WriteReg(R67, ((Ypos & 0x100)>> 8));
    LCD_WriteReg(R68, (Ypos & 0xFF));
  }
}

/*******************************************************************************
* Function Name  : LCD_DrawChar
* Description    : Draws a character on LCD.
* Input          : - Xpos: the Line where to display the character shape.
*                    This parameter can be one of the following values:
*                       - Linex: where x can be 0..9
*                  - Ypos: start column address.
*                  - c: pointer to the character data.
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_DrawChar(u8 Xpos, u16 Ypos, uc16 *c)
{
  u32 index = 0, i = 0;
  u8 Xaddress = 0;
   
  Xaddress = Xpos;
  
  LCD_SetCursor(Xaddress, Ypos);
  
  for(index = 0; index < 24; index++)
  {
    if(LCDType == LCD_ILI9320)
    {
      LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */
    }
    for(i = 0; i < 16; i++)
    {
      if((c[index] & (1 << i)) == 0x00)
      {
        LCD_WriteRAM(BackColor);
      }
      else
      {
        LCD_WriteRAM(TextColor);
      }
    }
    if(LCDType == LCD_ILI9320)
    {
      LCD_CtrlLinesWrite(GPIOB, CtrlPin_NCS, Bit_SET);
    }  
    Xaddress++;
    LCD_SetCursor(Xaddress, Ypos);
  }
}

/*******************************************************************************
* Function Name  : LCD_DisplayChar
* Description    : Displays one character (16dots width, 24dots height).
* Input          : - Line: the Line where to display the character shape .
*                    This parameter can be one of the following values:
*                       - Linex: where x can be 0..9
*                  - Column: start column address.
*                  - Ascii: character ascii code, must be between 0x20 and 0x7E.
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_DisplayChar(u8 Line, u16 Column, u8 Ascii)
{
  Ascii -= 32;
  LCD_DrawChar(Line, Column, &ASCII_Table[Ascii * 24]);
}

/*******************************************************************************
* Function Name  : LCD_DisplayStringLine
* Description    : Displays a maximum of 20 char on the LCD.
* Input          : - Line: the Line where to display the character shape .
*                    This parameter can be one of the following values:
*                       - Linex: where x can be 0..9
*                  - *ptr: pointer to string to display on LCD.
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_DisplayStringLine(u8 Line, u8 *ptr)
{
  u32 i = 0;
  u16 refcolumn = 319;

  /* Send the string character by character on lCD */
  while ((*ptr != 0) & (i < 20))
  {
    /* Display one character on LCD */
    LCD_DisplayChar(Line, refcolumn, *ptr);
    /* Decrement the column position by 16 */
    refcolumn -= 16;
    /* Point on the next character */
    ptr++;
    /* Increment the character counter */
    i++;
  }
}

/*******************************************************************************
* Function Name  : LCD_SetDisplayWindow
* Description    : Sets a display window
* Input          : - Xpos: specifies the X buttom left position.
*                  - Ypos: specifies the Y buttom left position.
*                  - Height: display window height.
*                  - Width: display window width.
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_SetDisplayWindow(u8 Xpos, u16 Ypos, u8 Height, u16 Width)
{
  if(LCDType == LCD_ILI9320)
  {
    /* Horizontal GRAM Start Address */
    if(Xpos >= Height)
    {
      LCD_WriteReg(R80, (Xpos - Height + 1));
    }
    else
    {
      LCD_WriteReg(R80, 0);
    }
    /* Horizontal GRAM End Address */
    LCD_WriteReg(R81, Xpos);
    /* Vertical GRAM Start Address */
    if(Ypos >= Width)
    {
      LCD_WriteReg(R82, (Ypos - Width + 1));
    }  
    else
    {
      LCD_WriteReg(R82, 0);
    }
    /* Vertical GRAM End Address */
    LCD_WriteReg(R83, Ypos);
  }
  else if(LCDType == LCD_HX8312)
  {  
    LCD_WriteReg(R1, 0xD0);
    LCD_WriteReg(R5, 0x14);
  
    LCD_WriteReg(R69, (Xpos - Height + 1));
    LCD_WriteReg(R70, Xpos);
 
    LCD_WriteReg(R71, (((Ypos - Width + 1) & 0x100)>> 8));
    LCD_WriteReg(R72, ((Ypos - Width + 1) & 0xFF));

    LCD_WriteReg(R73, ((Ypos & 0x100)>> 8));
    LCD_WriteReg(R74, (Ypos & 0xFF));
  }

  LCD_SetCursor(Xpos, Ypos);
}

/*******************************************************************************
* Function Name  : LCD_WindowModeDisable
* Description    : Disables LCD Window mode.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_WindowModeDisable(void)
{
  if(LCDType == LCD_ILI9320)
  {
    LCD_SetDisplayWindow(239, 0x13F, 240, 320);
    LCD_WriteReg(R3, 0x1018);
  }
  else if(LCDType == LCD_HX8312)
  {
    LCD_WriteReg(R1, 0x50);
    LCD_WriteReg(R5, 0x04); 
  }
    
}
/*******************************************************************************
* Function Name  : LCD_DrawLine
* Description    : Displays a line.
* Input          : - Xpos: specifies the X position.
*                  - Ypos: specifies the Y position.
*                  - Length: line length.
*                  - Direction: line direction.
*                    This parameter can be one of the following values: Vertical 
*                    or Horizontal.
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_DrawLine(u8 Xpos, u16 Ypos, u16 Length, u8 Direction)
{
  u32 i = 0;
  
  LCD_SetCursor(Xpos, Ypos);

  if(Direction == Horizontal)
  { 
    if(LCDType == LCD_ILI9320)
    {
      LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */
    }
    for(i = 0; i < Length; i++)
    {
      LCD_WriteRAM(TextColor);
    }
    if(LCDType == LCD_ILI9320)
    {
      LCD_CtrlLinesWrite(GPIOB, CtrlPin_NCS, Bit_SET);
    }
  }
  else
  {
   for(i = 0; i < Length; i++)
    {
      if(LCDType == LCD_ILI9320)
      {
        LCD_WriteRAMWord(TextColor);
      }
      else  if(LCDType == LCD_HX8312)
      {
        LCD_WriteRAM(TextColor);
      }
      Xpos++;
      LCD_SetCursor(Xpos, Ypos);
    }
  }
}

/*******************************************************************************
* Function Name  : LCD_DrawRect
* Description    : Displays a rectangle.
* Input          : - Xpos: specifies the X position.
*                  - Ypos: specifies the Y position.
*                  - Height: display rectangle height.
*                  - Width: display rectangle width.
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_DrawRect(u8 Xpos, u16 Ypos, u8 Height, u16 Width)
{
  LCD_DrawLine(Xpos, Ypos, Width, Horizontal);
  LCD_DrawLine((Xpos + Height), Ypos, Width, Horizontal);
  
  LCD_DrawLine(Xpos, Ypos, Height, Vertical);
  LCD_DrawLine(Xpos, (Ypos - Width + 1), Height, Vertical);
}

/*******************************************************************************
* Function Name  : LCD_DrawCircle
* Description    : Displays a circle.
* Input          : - Xpos: specifies the X position.
*                  - Ypos: specifies the Y position.
*                  - Height: display rectangle height.
*                  - Width: display rectangle width.
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_DrawCircle(u8 Xpos, u16 Ypos, u16 Radius)
{
  s32  D;/* Decision Variable */ 
  u32  CurX;/* Current X Value */
  u32  CurY;/* Current Y Value */ 
  
  D = 3 - (Radius << 1);
  CurX = 0;
  CurY = Radius;
  
  while (CurX <= CurY)
  {
    LCD_SetCursor(Xpos + CurX, Ypos + CurY);
    if(LCDType == LCD_ILI9320)
    {
      LCD_WriteRAMWord(TextColor);
    }
    else if(LCDType == LCD_HX8312)
    {
      LCD_WriteRAM(TextColor);
    }
    LCD_SetCursor(Xpos + CurX, Ypos - CurY);
    if(LCDType == LCD_ILI9320)
    {
      LCD_WriteRAMWord(TextColor);
    }
    else if(LCDType == LCD_HX8312)
    {
      LCD_WriteRAM(TextColor);
    }
    LCD_SetCursor(Xpos - CurX, Ypos + CurY);
    if(LCDType == LCD_ILI9320)
    {
      LCD_WriteRAMWord(TextColor);
    }
    else if(LCDType == LCD_HX8312)
    {
      LCD_WriteRAM(TextColor);
    }
    LCD_SetCursor(Xpos - CurX, Ypos - CurY);
    if(LCDType == LCD_ILI9320)
    {
      LCD_WriteRAMWord(TextColor);
    }
    else if(LCDType == LCD_HX8312)
    {
      LCD_WriteRAM(TextColor);
    }
    LCD_SetCursor(Xpos + CurY, Ypos + CurX);
    if(LCDType == LCD_ILI9320)
    {
      LCD_WriteRAMWord(TextColor);
    }
    else if(LCDType == LCD_HX8312)
    {
      LCD_WriteRAM(TextColor);
    }
    LCD_SetCursor(Xpos + CurY, Ypos - CurX);
    if(LCDType == LCD_ILI9320)
    {
      LCD_WriteRAMWord(TextColor);
    }
    else if(LCDType == LCD_HX8312)
    {
      LCD_WriteRAM(TextColor);
    }
    LCD_SetCursor(Xpos - CurY, Ypos + CurX);
    if(LCDType == LCD_ILI9320)
    {
      LCD_WriteRAMWord(TextColor);
    }
    else if(LCDType == LCD_HX8312)
    {
      LCD_WriteRAM(TextColor);
    }
    LCD_SetCursor(Xpos - CurY, Ypos - CurX);
    if(LCDType == LCD_ILI9320)
    {
      LCD_WriteRAMWord(TextColor);
    }
    else if(LCDType == LCD_HX8312)
    {
      LCD_WriteRAM(TextColor);
    }

    if (D < 0)
    { 
      D += (CurX << 2) + 6;
    }
    else
    {
      D += ((CurX - CurY) << 2) + 10;
      CurY--;
    }
    CurX++;
  }
}

/*******************************************************************************
* Function Name  : LCD_DrawMonoPict
* Description    : Displays a monocolor picture.
* Input          : - Pict: pointer to the picture array.
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_DrawMonoPict(uc32 *Pict)
{
  u32 index = 0, i = 0;

  LCD_SetCursor(0, 319); 
  if(LCDType == LCD_ILI9320)
  {
    LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */
  }
  for(index = 0; index < 2400; index++)
  {
    for(i = 0; i < 32; i++)
    {
      if((Pict[index] & (1 << i)) == 0x00)
      {
        LCD_WriteRAM(BackColor);
      }
      else
      {
        LCD_WriteRAM(TextColor);
      }
    }
  }
  if(LCDType == LCD_ILI9320)
  {
    LCD_CtrlLinesWrite(GPIOB, CtrlPin_NCS, Bit_SET);
  }
}


/*******************************************************************************
* Function Name  : LCD_nCS_StartByte
* Description    : Reset LCD control line(/CS) and Send Start-Byte
* Input          : - Start_Byte: the Start-Byte to be sent
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_nCS_StartByte(u8 Start_Byte)
{
  LCD_CtrlLinesWrite(GPIOB, CtrlPin_NCS, Bit_RESET);

  SPI_I2S_SendData(SPI2, Start_Byte);
  while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_BSY) != RESET)
  {
  }
}

/*******************************************************************************
* Function Name  : LCD_WriteRegIndex
* Description    : Writes index to select the LCD register.
* Input          : - LCD_Reg: address of the selected register.
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_WriteRegIndex(u8 LCD_Reg)
{
  /* Reset LCD control line(/CS) and Send Start-Byte */
  LCD_nCS_StartByte(START_BYTE | SET_INDEX);

  /* Write 16-bit Reg Index (High Byte is 0) */
  SPI_I2S_SendData(SPI2, 0x00);
  while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_BSY) != RESET)
  {
  }
  SPI_I2S_SendData(SPI2, LCD_Reg);
  while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_BSY) != RESET)
  {
  }

  LCD_CtrlLinesWrite(GPIOB, CtrlPin_NCS, Bit_SET);
}

/*******************************************************************************
* Function Name  : LCD_WriteRegILI9320
* Description    : Writes to the selected LCD ILI9320 register.
* Input          : - LCD_Reg: address of the selected register.
*                  - LCD_RegValue: value to write to the selected register.
* Output         : None
* Return         : None
*******************************************************************************/
static void LCD_WriteRegILI9320(u8 LCD_Reg, u16 LCD_RegValue)
{
  /* Write 16-bit Index (then Write Reg) */
  LCD_WriteRegIndex(LCD_Reg);

  /* Write 16-bit Reg */
  /* Reset LCD control line(/CS) and Send Start-Byte */
  LCD_nCS_StartByte(START_BYTE | WRITE_REG);

  SPI_I2S_SendData(SPI2, LCD_RegValue>>8);
  while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_BSY) != RESET)
  {
  }
  SPI_I2S_SendData(SPI2, (LCD_RegValue & 0xFF));
  while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_BSY) != RESET)
  {
  }

  LCD_CtrlLinesWrite(GPIOB, CtrlPin_NCS, Bit_SET);
}

/*******************************************************************************
* Function Name  : LCD_ReadReg
* Description    : Reads the selected LCD Register.
* Input          : None
* Output         : None
* Return         : LCD Register Value.
*******************************************************************************/
u16 LCD_ReadReg(u8 LCD_Reg)
{
  u16 tmp = 0;
  u8 i = 0;
  
  /* SPI2 prescaler: 4 */
  SPI2->CR1 &= 0xFFC7;
  SPI2->CR1 |= 0x0008;

  /* Write 16-bit Index (then Read Reg) */
  LCD_WriteRegIndex(LCD_Reg);

  /* Read 16-bit Reg */
  /* Reset LCD control line(/CS) and Send Start-Byte */
  LCD_nCS_StartByte(START_BYTE | READ_REG);
  
  for(i = 0; i < 5; i++)
  {
    SPI_I2S_SendData(SPI2, 0xFF);
    while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_BSY) != RESET)
    {
    }
    /* One byte of invalid dummy data read after the start byte */
    while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET)
    {    
    }
    SPI_I2S_ReceiveData(SPI2); 
  }

  SPI_I2S_SendData(SPI2, 0xFF);
  /* Read upper byte */
  while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_BSY) != RESET)
  {
  }
  /* Read lower byte */
  while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET)
  {
  }
  tmp = SPI_I2S_ReceiveData(SPI2);
  
  
  SPI_I2S_SendData(SPI2, 0xFF);
  while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_BSY) != RESET)
  {
  }
  /* Read lower byte */
  while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET)
  {
  }
  tmp = ((tmp & 0xFF) << 8) | SPI_I2S_ReceiveData(SPI2);

  LCD_CtrlLinesWrite(GPIOB, CtrlPin_NCS, Bit_SET);

  /* SPI2 prescaler: 2 */
  SPI2->CR1 &= 0xFFC7;

  return tmp;
}

/*******************************************************************************
* Function Name  : LCD_WriteRAM_Prepare
* Description    : Prepare to write to the LCD RAM.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_WriteRAM_Prepare(void)
{
  LCD_WriteRegIndex(R34); /* Select GRAM Reg */

  /* Reset LCD control line(/CS) and Send Start-Byte */
  LCD_nCS_StartByte(START_BYTE | WRITE_REG);
}

/*******************************************************************************
* Function Name  : LCD_WriteRAMWord
* Description    : Writes 1 word to the LCD RAM.
* Input          : - RGB_Code: the pixel color in RGB mode (5-6-5).
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_WriteRAMWord(u16 RGB_Code)
{
  LCD_WriteRAM_Prepare();

  LCD_WriteRAM(RGB_Code);

  LCD_CtrlLinesWrite(GPIOB, CtrlPin_NCS, Bit_SET);
}

/*******************************************************************************
* Function Name  : LCD_WriteRegHX8312
* Description    : Writes to the selected LCD HX8312 register.
* Input          : - LCD_Reg: address of the selected register.
*                  - LCD_RegValue: value to write to the selected register.
* Output         : None
* Return         : None
*******************************************************************************/
static void LCD_WriteRegHX8312(u8 LCD_Reg, u8 LCD_RegValue)
{
  u16 tmp = 0;
  
  LCD_CtrlLinesWrite(GPIOD, CtrlPin_NWR, Bit_RESET);
  LCD_CtrlLinesWrite(GPIOD, CtrlPin_RS, Bit_RESET);
  LCD_CtrlLinesWrite(GPIOB, CtrlPin_NCS, Bit_RESET);
  
  tmp = LCD_Reg << 8;
  tmp |= LCD_RegValue;

  SPI_I2S_SendData(SPI2, tmp);
  while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_BSY) != RESET)
  {
  }
  
  LCD_CtrlLinesWrite(GPIOB, CtrlPin_NCS, Bit_SET);
}

/*******************************************************************************
* Function Name  : LCD_WriteReg
* Description    : Writes to the selected LCD register.
* Input          : - LCD_Reg: address of the selected register.
*                  - LCD_RegValue: value to write to the selected register.
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_WriteReg(u8 LCD_Reg, u16 LCD_RegValue)
{
  if(LCDType == LCD_ILI9320)
  {
    LCD_WriteRegILI9320(LCD_Reg, LCD_RegValue);
  }
  else if(LCDType == LCD_HX8312)
  {
    LCD_WriteRegHX8312(LCD_Reg, ((u8)LCD_RegValue));
  }
}


/*******************************************************************************
* Function Name  : LCD_WriteRAM
* Description    : Writes to the LCD RAM.
* Input          : - RGB_Code: the pixel color in RGB mode (5-6-5).
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_WriteRAM(u16 RGB_Code)
{
  if(LCDType == LCD_ILI9320)
  {
    SPI_I2S_SendData(SPI2, RGB_Code >> 8);
    while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_BSY) != RESET)
    {
    }
    SPI_I2S_SendData(SPI2, RGB_Code & 0xFF);
    while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_BSY) != RESET)
    {
    }
  }
  if(LCDType == LCD_HX8312)
  {
    LCD_CtrlLinesWrite(GPIOD, CtrlPin_NWR, Bit_RESET);
    LCD_CtrlLinesWrite(GPIOD, CtrlPin_RS, Bit_SET);
    LCD_CtrlLinesWrite(GPIOB, CtrlPin_NCS, Bit_RESET);

    SPI_I2S_SendData(SPI2, RGB_Code);
    while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_BSY) != RESET)
    {
    }
  
    LCD_CtrlLinesWrite(GPIOB, CtrlPin_NCS, Bit_SET);
  }
}

/*******************************************************************************
* Function Name  : LCD_PowerOn
* Description    : Power on the LCD.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_PowerOn(void)
{
  if(LCDType == LCD_ILI9320)
  {
    /* Power On sequence ---------------------------------------------------------*/
    LCD_WriteReg(R16, 0x0000); /* SAP, BT[3:0], AP, DSTB, SLP, STB */
    LCD_WriteReg(R17, 0x0000); /* DC1[2:0], DC0[2:0], VC[2:0] */
    LCD_WriteReg(R18, 0x0000); /* VREG1OUT voltage */
    LCD_WriteReg(R19, 0x0000); /* VDV[4:0] for VCOM amplitude */
    TB_Wait(400);                 /* Dis-charge capacitor power voltage (200ms) */
    LCD_WriteReg(R16, 0x17B0); /* SAP, BT[3:0], AP, DSTB, SLP, STB */
    LCD_WriteReg(R17, 0x0137); /* DC1[2:0], DC0[2:0], VC[2:0] */
    TB_Wait(100);                  /* Delay 50 ms */
    LCD_WriteReg(R18, 0x0139); /* VREG1OUT voltage */
    TB_Wait(100);                  /* delay 50 ms */
    LCD_WriteReg(R19, 0x1d00); /* VDV[4:0] for VCOM amplitude */
    LCD_WriteReg(R41, 0x0013); /* VCM[4:0] for VCOMH */
    TB_Wait(100);                  /* delay 50 ms */
    LCD_WriteReg(R7, 0x0173);  /* 262K color and display ON */
  }
  else if(LCDType == LCD_HX8312)
  {  
    /* Power On Set */
    LCD_WriteReg(R28, 0x73);
    LCD_WriteReg(R36, 0x74);
    LCD_WriteReg(R30, 0x01);
    LCD_WriteReg(R24, 0xC1);
    TB_Wait(20); /* Delay 10 ms */
    LCD_WriteReg(R24, 0xE1);
    LCD_WriteReg(R24, 0xF1);
    TB_Wait(120); /* Delay 60 ms */
    LCD_WriteReg(R24, 0xF5);
    TB_Wait(120); /* Delay 60 ms */
    LCD_WriteReg(R27, 0x09);
    TB_Wait(20); /* Delay 10 ms */
    LCD_WriteReg(R31, 0x11);
    LCD_WriteReg(R32, 0x0E);
    LCD_WriteReg(R30, 0x81);
    TB_Wait(20); /* Delay 10 ms */
  }
}

/*******************************************************************************
* Function Name  : LCD_DisplayOn
* Description    : Enables the Display.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_DisplayOn(void)
{
  if(LCDType == LCD_ILI9320)
  {
    /* Display On */
    LCD_WriteReg(R7, 0x0173); /* 262K color and display ON */
  }
  else if(LCDType == LCD_HX8312)
  {  
    LCD_WriteReg(R1, 0x50);
    LCD_WriteReg(R5, 0x04);

    /* Display On */
    LCD_WriteReg(R0, 0x80);
    LCD_WriteReg(R59, 0x01);
    TB_Wait(80);                 /* Delay 40 ms */
    LCD_WriteReg(R0, 0x20);
  }  
}

/*******************************************************************************
* Function Name  : LCD_DisplayOff
* Description    : Disables the Display.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_DisplayOff(void)
{
  if(LCDType == LCD_ILI9320)
  {
    /* Display Off */
    LCD_WriteReg(R7, 0x0);
  }
  else if(LCDType == LCD_HX8312)
  { 
    /* Display Off */
    LCD_WriteReg(R0, 0xA0);
    TB_Wait(80);                 /* Delay 40 ms */
    LCD_WriteReg(R59, 0x00);
  } 
}

/*******************************************************************************
* Function Name  : LCD_CtrlLinesConfig
* Description    : Configures LCD control lines in Output Push-Pull mode.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_CtrlLinesConfig(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  /* Configure NCS (PB.02) in Output Push-Pull mode */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  
  /* Configure NWR(RNW), RS (PD.15, PD.07) in Output Push-Pull mode */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_15;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
  
  LCD_CtrlLinesWrite(GPIOD, CtrlPin_NWR, Bit_SET);
  LCD_CtrlLinesWrite(GPIOD, CtrlPin_RS, Bit_SET);
}

/*******************************************************************************
* Function Name  : LCD_CtrlLinesWrite
* Description    : Sets or reset LCD control lines.
* Input          : - GPIOx: where x can be B or D to select the GPIO peripheral.
*                  - CtrlPins: the Control line. This parameter can be:
*                       - CtrlPin_NCS: Chip Select pin (PB.02)
*                       - CtrlPin_NWR: Read/Write Selection pin (PD.15)
*                       - CtrlPin_RS: Register/RAM Selection pin (PD.07)
*                  - BitVal: specifies the value to be written to the selected bit.
*                    This parameter can be:
*                       - Bit_RESET: to clear the port pin
*                       - Bit_SET: to set the port pin
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_CtrlLinesWrite(GPIO_TypeDef* GPIOx, u16 CtrlPins, BitAction BitVal)
{
  /* Set or Reset the control line */
  GPIO_WriteBit(GPIOx, CtrlPins, BitVal);
}

/*******************************************************************************
* Function Name  : LCD_SPIConfig
* Description    : Configures the SPI2 interface.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_SPIConfig(void)
{
  SPI_InitTypeDef    SPI_InitStructure;
  GPIO_InitTypeDef   GPIO_InitStructure;

  /* Enable GPIOB clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

  /* Enable SPI2 clock  */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);

  /* Configure SPI2 pins: NSS, SCK, MISO and MOSI */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  SPI_I2S_DeInit(SPI2);
  
  /* SPI2 Config */
  SPI_StructInit(&SPI_InitStructure);
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  if(LCDType == LCD_ILI9320)
  {
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
  }
  else if(LCDType == LCD_HX8312)
  {
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_16b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
  }
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_Init(SPI2, &SPI_InitStructure);

  /* SPI2 enable */
  SPI_Cmd(SPI2, ENABLE);
}

/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/
