// ****************************************************************************
/// \file      bulli.c
///
/// \brief     Queue C Source File
///
/// \details   A the bulli c file
///
/// \author    Nico Korn
///
/// \version   1.0.0.0
///
/// \date      19082022
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

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "bulli.h"
#include "button.h"
#include "ws2812b.h"
#include "events.h"
#include "queue.h"

/* Private includes ----------------------------------------------------------*/

/* Private typedef -----------------------------------------------------------*/
typedef struct
{
   uint8_t     row_start;
   uint16_t    col_start;
   uint8_t     row_end;
   uint16_t    col_end;
}Bulli_light_t;

typedef struct
{
   bool  ignition_on;
   bool  blink_left;
   bool  blink_right;
}Bulli_status_t;

/* Private define ------------------------------------------------------------*/
#define REFRESH_PERIOD_MS        ( 100u )
#define EVENT_QUEUE_CAPACITY     ( 10u )

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
static Queue_t eventQueue;
static uint8_t event;
static Bulli_status_t bulli;
static const Bulli_light_t bulli_r_blink =
{
   .row_start  = 1,
   .col_start  = 2,
   .row_end    = 1,
   .col_end    = 3
};
static const Bulli_light_t bulli_l_blink =
{
   .row_start  = 1,
   .col_start  = 4,
   .row_end    = 1,
   .col_end    = 5
};
static const Bulli_light_t bulli_r_light =
{
   .row_start  = 1,
   .col_start  = 0,
   .row_end    = 1,
   .col_end    = 1
};
static const Bulli_light_t bulli_l_light =
{
   .row_start  = 1,
   .col_start  = 6,
   .row_end    = 1,
   .col_end    = 7
};
static const Bulli_light_t bulli_interior_light =
{
   .row_start  = 0,
   .col_start  = 0,
   .row_end    = 0,
   .col_end    = COL
};
static uint32_t   framecounter;
static uint8_t    r;
static uint8_t    g;
static uint8_t    b;

/* Private function prototypes -----------------------------------------------*/
static void       eventCheck        ( void );
static void       refreshLeds       ( void );
static void       cbButtonIgnition  ( void );
static void       cbButtonLeft      ( void );
static void       cbButtonRight     ( void );
static uint16_t   msToTicks         ( uint16_t ms );
static void       colorWheelPlus    ( uint8_t *red, uint8_t *green, uint8_t *blue );
static void       setBlinkerLeft    ( uint8_t param_r, uint8_t param_g, uint8_t param_b );
static void       setBlinkerRight   ( uint8_t param_r, uint8_t param_g, uint8_t param_b );
static void       setLightLeft     ( uint8_t param_r, uint8_t param_g, uint8_t param_b );
static void       setLightRight     ( uint8_t param_r, uint8_t param_g, uint8_t param_b );
static void       setLightInterior  ( uint8_t param_r, uint8_t param_g, uint8_t param_b );

/* --------Functions ---------------------------------------------------------*/
// ----------------------------------------------------------------------------
/// \brief     Bulli state machine
///
/// \param     [in] state
///
/// \return    Queue_StatusTypeDef
Bulli_StatusTypeDef Bulli_init( void )
{
   // init peripherals for using the ws2812b leds
   if( WS2812B_init() != WS2812B_READY )
   {
      return Bulli_ERROR;
   }
   
   // init event queue
   if( Queue_init( &eventQueue, EVENT_QUEUE_CAPACITY ) != QUEUE_OK )
   {
      return Bulli_ERROR;
   }
   
   // init buttons
   if( Button_init( cbButtonIgnition, cbButtonLeft, cbButtonRight ) != Button_OK )
   {
      return Bulli_ERROR;
   }
   
   return Bulli_OK;
}

// ----------------------------------------------------------------------------
/// \brief     Infinite while loop for the bulli.
///
/// \param     [in] state
///
/// \return    Queue_StatusTypeDef
void Bulli_run( void )
{
   // set initial bulli states
   bulli.ignition_on = false;
   bulli.blink_left = false;
   bulli.blink_right = false;
   
   // set initial values for frame counting and coloring
   framecounter = 0;
   r = 0xff;
   g = 0x00;   
   b = 0x00;   
   
   // should never left this loop
   while(1)
   {
      eventCheck();
      refreshLeds();
   }
}

// ----------------------------------------------------------------------------
/// \brief     Bulli checks for new events.
///
/// \param     [in] state
///
/// \return    Queue_StatusTypeDef
static void eventCheck( void )
{
   // read event queue
   Queue_dequeue( &eventQueue, &event );
   
   // process event
   switch( event )
   {
      case EVENT_IDLE:
      break;
      case EVENT_BUTTON_IGNITION:
         if( bulli.ignition_on != false )
         {
            bulli.ignition_on = false;
            bulli.blink_left = false;
            bulli.blink_right = false;
         }
         else
         {
            bulli.ignition_on = true;
            bulli.blink_left = false;
            bulli.blink_right = false;
         }
      break;
      case EVENT_BUTTON_LEFT:
         if( bulli.ignition_on != false )
         {
            if( bulli.blink_left != false )
            {
               bulli.blink_left = false;
            }
            else
            {
               bulli.blink_left = true;
            }
            
            if( bulli.blink_right != false )
            {
               bulli.blink_right = false;
            }
         }
      break;
      case EVENT_BUTTON_RIGHT:
         if( bulli.ignition_on != false )
         {
            if( bulli.blink_right != false )
            {
               bulli.blink_right = false;
            }
            else
            {
               bulli.blink_right = true;
            }
            
            if( bulli.blink_left != false )
            {
               bulli.blink_left = false;
            }
         }
      break;
      default:;
   }
}

// ----------------------------------------------------------------------------
/// \brief     Bulli sets leds to the events.
///
/// \param     none
///
/// \return    Queue_StatusTypeDef
static void refreshLeds( void )
{
   // clear framebuffer
   WS2812B_clearBuffer();
   
   // bullis ignition
   if( bulli.ignition_on != false )
   {
      uint8_t ignitionFlicker;
      
      // ignition animation
      if( HAL_GPIO_ReadPin(BUTTON_GPIO, BUTTON_0_PIN) )
      {
         ignitionFlicker = 0xff;
      }
      else
      {
         ignitionFlicker = rand()%0xff;
      }
      setLightLeft( ignitionFlicker, ignitionFlicker, ignitionFlicker );
      setLightRight( ignitionFlicker, ignitionFlicker, ignitionFlicker );
      
      // bullis interior lights
      colorWheelPlus( &r, &g, &b );
      setLightInterior( r, g, b );
   }
   else
   {
      setLightLeft(0x00, 0x00, 0x00);
      setLightRight(0x00, 0x00, 0x00);
   }
   
   // bullis left blinker animation
   if( bulli.blink_left != false )
   {
      if( framecounter%msToTicks(2000) < msToTicks(1000) )
      {
         setBlinkerLeft(0xff, 0x80, 0x00);
      }
      else
      {
         setBlinkerLeft(0x00, 0x00, 0x00);
      }
   }
   else
   {
      setBlinkerLeft(0x00, 0x00, 0x00);
   }
   
   // bullis right blinker animation
   if( bulli.blink_right != false )
   {
      if( framecounter%msToTicks(2000) < msToTicks(1000) )
      {
         setBlinkerRight(0xff, 0x80, 0x00);
      }
      else
      {
         setBlinkerRight(0x00, 0x00, 0x00);
      }
   }
   else
   {
      setBlinkerRight(0x00, 0x00, 0x00);
   }
   
   WS2812B_sendBuffer();
   HAL_Delay(REFRESH_PERIOD_MS);
   framecounter++;
}

static void setBlinkerLeft( uint8_t param_r, uint8_t param_g, uint8_t param_b )
{
   for( uint16_t x=bulli_l_blink.col_start; x<=bulli_l_blink.col_end; x++  )
   {
      for( uint8_t y=bulli_l_blink.row_start; y<=bulli_l_blink.row_end; y++  )
      {
         WS2812B_setPixel( y, x, param_r, param_g, param_b );
      }
   }
}

static void setBlinkerRight( uint8_t param_r, uint8_t param_g, uint8_t param_b )
{
   for( uint16_t x=bulli_r_blink.col_start; x<=bulli_r_blink.col_end; x++  )
   {
      for( uint8_t y=bulli_r_blink.row_start; y<=bulli_r_blink.row_end; y++  )
      {
         WS2812B_setPixel( y, x, param_r, param_g, param_b );
      }
   }
}

static void setLightLeft( uint8_t param_r, uint8_t param_g, uint8_t param_b )
{
   for( uint16_t x=bulli_l_light.col_start; x<=bulli_l_light.col_end; x++  )
   {
      for( uint8_t y=bulli_l_light.row_start; y<=bulli_l_light.row_end; y++  )
      {
         WS2812B_setPixel( y, x, param_r, param_g, param_b );
      }
   }
}

static void setLightRight( uint8_t param_r, uint8_t param_g, uint8_t param_b )
{
   for( uint16_t x=bulli_r_light.col_start; x<=bulli_r_light.col_end; x++  )
   {
      for( uint8_t y=bulli_r_light.row_start; y<=bulli_r_light.row_end; y++  )
      {
         WS2812B_setPixel( y, x, param_r, param_g, param_b );
      }
   }
}

static void setLightInterior( uint8_t param_r, uint8_t param_g, uint8_t param_b )
{
   for( uint16_t x=bulli_interior_light.col_start; x<=bulli_interior_light.col_end; x++  )
   {
      for( uint8_t y=bulli_interior_light.row_start; y<=bulli_interior_light.row_end; y++  )
      {
         WS2812B_setPixel( y, x, param_r, param_g, param_b );
      }
   }
}

// ----------------------------------------------------------------------------
/// \brief     Convertion from ms into ticks
///
/// \param     [in]  uint16_t ms
///
/// \return    uint16_t ticks
static uint16_t msToTicks( uint16_t ms )
{
   return (uint16_t)((double)ms/(double)REFRESH_PERIOD_MS);
}

// ----------------------------------------------------------------------------
/// \brief     Led color wheel function
///
/// \param     [in]  uint8_t *red
/// \param     [in]  uint8_t *green
/// \param     [in]  uint8_t *blue
///
/// \return    uint16_t ticks
static void colorWheelPlus( uint8_t *red, uint8_t *green, uint8_t *blue )
{
	//set next colors
	if(*green == 0x00 && *red < 0xff && *blue >= 0x00){
		*red += 0x01;
		*green = 0x00;
		*blue -= 0x01;
	}else if(*green < 0xff && *red >= 0x00 && *blue == 0x00){
		*red -= 0x01;
		*green += 0x01;
		*blue = 0x00;
	}else if(*green >= 0x00 && *red == 0x00 && *blue < 0xff){
		*red = 0x00;
		*green -= 0x01;
		*blue += 0x01;
	}
}


// ----------------------------------------------------------------------------
/// \brief     Ignition button callback.
///
/// \param     none
///
/// \return    none
static void cbButtonIgnition( void )
{
   Queue_enqueue( &eventQueue, EVENT_BUTTON_IGNITION );
}

// ----------------------------------------------------------------------------
/// \brief     Left button callback.
///
/// \param     none
///
/// \return    none
static void cbButtonLeft( void )
{
   Queue_enqueue( &eventQueue, EVENT_BUTTON_LEFT );
}

// ----------------------------------------------------------------------------
/// \brief     Right button callback.
///
/// \param     none
///
/// \return    none
static void cbButtonRight( void )
{
   Queue_enqueue( &eventQueue, EVENT_BUTTON_RIGHT );
}

// ----------------------------------------------------------------------------



/************************ (C) COPYRIGHT Nico Korn ***************END OF FILE****/
