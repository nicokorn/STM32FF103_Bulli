// ****************************************************************************
/// \file      button.h
///
/// \brief     Button C HeaderFile
///
/// \details   Driver Module for button.
///
/// \author    Nico Korn
///
/// \version   1.0.0.0
///
/// \date      20082022
/// 
/// \copyright Copyright (c) 2022 Nico Korn
/// 
/// Permission is hereby granted, free of charge, to any person obtaining a copy
/// of this software and associated documentation files (the "Software"), to deal
/// in the Software without restriction, including without limitation the rights
/// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
/// copies of the Software, and to permit persons to whom the Software is
/// furnished to do so, subject to the following conditions:
/// 
/// The above copyright notice and this permission notice shall be included in all
/// copies or substantial portions of the Software.
/// 
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
/// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
/// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
/// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
/// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
/// SOFTWARE.
///
/// \pre       
///
/// \bug       
///
/// \warning   
///
/// \todo      
///
// ****************************************************************************

// Define to prevent recursive inclusion **************************************
#ifndef _BUTTON_H
#define _BUTTON_H

// Include ********************************************************************
#include "stm32f1xx_hal.h"

// Exported defines ***********************************************************
#define BUTTON_0_PIN       GPIO_PIN_0
#define BUTTON_1_PIN       GPIO_PIN_1
#define BUTTON_2_PIN       GPIO_PIN_3
#define BUTTON_0_IRQ       EXTI0_IRQn
#define BUTTON_1_IRQ       EXTI1_IRQn
#define BUTTON_2_IRQ       EXTI3_IRQn
#define BUTTON_GPIO        GPIOB
#define BUTTON_GPIO_CLK    __HAL_RCC_GPIOB_CLK_ENABLE(); 

// Exported types *************************************************************
typedef enum
{
   Button_OK       = 0x00U,
   Button_ERROR    = 0x01U,
   Button_RESET    = 0x02U
} Button_StatusTypeDef;

// Exported functions *********************************************************
Button_StatusTypeDef Button_init( void (*cbButton0Param)(void), void (*cbButton1Param)(void), void (*cbButton2Param)(void) );
#endif // _BUTTON_H