// ****************************************************************************
/// \file      button.c
///
/// \brief     Button C Source File
///
/// \details   Driver Module for the buttons.
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

// Include ********************************************************************
#include "button.h"
#include "stdbool.h"

// Private define *************************************************************
#define DEBOUNCE_VAL ( 30u )

// Private types     **********************************************************

// Private variables **********************************************************
static uint32_t               timerTick;
static Button_StatusTypeDef   buttonState = Button_RESET;
static uint16_t               button0DebounceCnt;
static uint16_t               button1DebounceCnt;
static uint16_t               button2DebounceCnt;
static uint32_t               button0TickCnt;
static uint32_t               button1TickCnt;
static uint32_t               button2TickCnt;
static bool                   button0DebouncePhase;
static bool                   button1DebouncePhase;
static bool                   button2DebouncePhase;
static TIM_HandleTypeDef      TIM1_Handle;

// Private function prototypes ************************************************
static Button_StatusTypeDef    init_timer              ( void );
static Button_StatusTypeDef    init_gpio               ( void );

// Global variables ***********************************************************

// Function Prototypes ********************************************************
static void (*cbButton0)(void); // = &fun;
static void (*cbButton1)(void);
static void (*cbButton2)(void);

// Functions ******************************************************************
// ----------------------------------------------------------------------------
/// \brief     Initialisation of the periphherals for using the ws2812b leds.
///
/// \param     none
///
/// \return    none
Button_StatusTypeDef Button_init( void (*cbButton0Param)(void), void (*cbButton1Param)(void), void (*cbButton2Param)(void) )
{   
   // bounce counters
   button0DebounceCnt = 0;
   button1DebounceCnt = 0;
   button2DebounceCnt = 0;
   
   // reset debounce phases
   button0DebouncePhase = false;
   button1DebouncePhase = false;
   button2DebouncePhase = false;
   
   // register callbacks
   cbButton0 = cbButton0Param;
   cbButton1 = cbButton1Param;
   cbButton2 = cbButton2Param;
   
   // init peripherals
   if( init_gpio() != Button_OK )
   {
      buttonState = Button_ERROR;
      return buttonState;
   }

   if( init_timer() != Button_OK )
   {
      buttonState = Button_ERROR;
      return buttonState;
   }
   
   // set the ws2812b state flag to ready for operation
   buttonState = Button_OK;
   
   return buttonState;
}

// ----------------------------------------------------------------------------
/// \brief     Initialisation of the Timer.
///
/// \param     none
///
/// \return    none
static Button_StatusTypeDef init_timer( void )
{
   uint16_t                     PrescalerValue;
   
   // TIM1 Periph clock enable
   __HAL_RCC_TIM1_CLK_ENABLE();
   
   // set prescaler to get a 10 kHz clock signal
   PrescalerValue = (uint16_t) (SystemCoreClock / 10000) - 1;
   
   // Time base configuration
   TIM1_Handle.Instance                = TIM1;
   TIM1_Handle.Init.Period             = 9; // each 1 ms
   TIM1_Handle.Init.Prescaler          = PrescalerValue;
   TIM1_Handle.Init.ClockDivision      = 0;
   TIM1_Handle.Init.CounterMode        = TIM_COUNTERMODE_UP;
   TIM1_Handle.Init.RepetitionCounter  = 0;
   TIM1_Handle.Init.AutoReloadPreload  = TIM_AUTORELOAD_PRELOAD_DISABLE;
   if( HAL_TIM_Base_Init(&TIM1_Handle) != HAL_OK )
   {
     return Button_ERROR;
   }

   // configure TIM1 interrupt
   HAL_NVIC_SetPriority(TIM1_UP_IRQn, 5, 5);
   HAL_NVIC_EnableIRQ(TIM1_UP_IRQn);
   
   // start the timer
   if( HAL_TIM_Base_Start_IT(&TIM1_Handle) != HAL_OK )
   {
      return Button_ERROR;
   }
   
   return Button_OK;
}

// ----------------------------------------------------------------------------
/// \brief     Initialisation of the GPIOS.
///
/// \param     none
///
/// \return    none
static Button_StatusTypeDef init_gpio( void )
{
   BUTTON_GPIO_CLK
   GPIO_InitTypeDef GPIO_InitStruct;               
   GPIO_InitStruct.Pin  = BUTTON_0_PIN | BUTTON_1_PIN | BUTTON_2_PIN;
   GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
   GPIO_InitStruct.Pull = GPIO_PULLUP;                         
   HAL_GPIO_Init(BUTTON_GPIO, &GPIO_InitStruct);   
   
   // configure exti interrupt
   HAL_NVIC_SetPriority(BUTTON_0_IRQ, 0, 3);
   HAL_NVIC_EnableIRQ(BUTTON_0_IRQ);
   HAL_NVIC_SetPriority(BUTTON_1_IRQ, 0, 3);
   HAL_NVIC_EnableIRQ(BUTTON_1_IRQ);
   HAL_NVIC_SetPriority(BUTTON_2_IRQ, 0, 3);
   HAL_NVIC_EnableIRQ(BUTTON_2_IRQ);
        
   return Button_OK;
}

// ----------------------------------------------------------------------------
/// \brief      Timer 1 interrupt handler.
///
/// \param      none
///
/// \return     none
void TIM1_UP_IRQHandler( void )
{
   // handle the irq
   if (__HAL_TIM_GET_FLAG(&TIM1_Handle, TIM_FLAG_UPDATE) != RESET)
   {
      if (__HAL_TIM_GET_IT_SOURCE(&TIM1_Handle, TIM_IT_UPDATE) != RESET)
      {
         __HAL_TIM_CLEAR_IT(&TIM1_Handle, TIM_IT_UPDATE);
      }
   }
   
   // increment tick
   timerTick++;

   // read input pin level and increment debounce counter accordingly
   if( button0DebouncePhase != false )
   {
      if( !HAL_GPIO_ReadPin(BUTTON_GPIO, BUTTON_0_PIN) )
      {
         button0DebounceCnt++;
      }
   }
   
   // check debounce count if sampling count has reached debounce limit
   if( timerTick >= button0TickCnt && button0DebouncePhase != false )
   {
      button0DebouncePhase = false;
      
      if( button0DebounceCnt == DEBOUNCE_VAL ) // || button0DebounceCnt == 0 )
      {
         // button callback
         cbButton0();
      }
   }
   
   // read input pin level and increment debounce counter accordingly
   if( button1DebouncePhase != false )
   {
      if( !HAL_GPIO_ReadPin(BUTTON_GPIO, BUTTON_1_PIN) )
      {
         button1DebounceCnt++;
      }
   }
   
   // check debounce count if sampling count has reached debounce limit
   if( timerTick >= button1TickCnt && button1DebouncePhase != false )
   {
      button1DebouncePhase = false;
      
      if( button1DebounceCnt == DEBOUNCE_VAL ) // || button1DebounceCnt == 0 )
      {
         // button callback
         cbButton1();
      }
   }
   
   // read input pin level and increment debounce counter accordingly
   if( button2DebouncePhase != false )
   {
      if( !HAL_GPIO_ReadPin(BUTTON_GPIO, BUTTON_2_PIN) )
      {
         button2DebounceCnt++;
      }
   }
   
   // check debounce count if sampling count has reached debounce limit
   if( timerTick >= button2TickCnt && button2DebouncePhase != false )
   {
      button2DebouncePhase = false;
      
      if( button2DebounceCnt == DEBOUNCE_VAL ) // || button2DebounceCnt == 0 )
      {
         // button callback on high level
         cbButton2();
      }
   }
}

// ----------------------------------------------------------------------------
/// \brief      Exti0 nterrupt handler.
///
/// \param      none
///
/// \return     none
void EXTI0_IRQHandler( void )
{
   // handle the irq
   if (__HAL_GPIO_EXTI_GET_IT(BUTTON_0_PIN) != 0x00u)
   {
      __HAL_GPIO_EXTI_CLEAR_IT(BUTTON_0_PIN);
      HAL_GPIO_EXTI_Callback(BUTTON_0_PIN);
   }
   
   // reset debounce counter
   button0DebounceCnt = 0;
   
   // set debounce phase
   button0DebouncePhase = true;
   
   // store tick
   button0TickCnt = timerTick+DEBOUNCE_VAL;
}

// ----------------------------------------------------------------------------
/// \brief      Exti1 nterrupt handler.
///
/// \param      none
///
/// \return     none
void EXTI1_IRQHandler( void )
{
   // handle the irq
   if (__HAL_GPIO_EXTI_GET_IT(BUTTON_1_PIN) != 0x00u)
   {
      __HAL_GPIO_EXTI_CLEAR_IT(BUTTON_1_PIN);
      HAL_GPIO_EXTI_Callback(BUTTON_1_PIN);
   }
   
   // reset debounce counter
   button1DebounceCnt = 0;
   
   // set debounce phase
   button1DebouncePhase = true;
   
   // store tick
   button1TickCnt = timerTick+DEBOUNCE_VAL;
}

// ----------------------------------------------------------------------------
/// \brief      Exti2 nterrupt handler.
///
/// \param      none
///
/// \return     none
void EXTI4_IRQHandler( void )
{
   // handle the irq
   if (__HAL_GPIO_EXTI_GET_IT(BUTTON_2_PIN) != 0x00u)
   {
      __HAL_GPIO_EXTI_CLEAR_IT(BUTTON_2_PIN);
      HAL_GPIO_EXTI_Callback(BUTTON_2_PIN);
   }
   
   // reset debounce counter
   button2DebounceCnt = 0;
   
   // set debounce phase
   button2DebouncePhase = true;
   
   // store tick
   button2TickCnt = timerTick+DEBOUNCE_VAL;
}
