// ****************************************************************************
/// \file      queue.c
///
/// \brief     Queue C Source File
///
/// \details   A generic queue
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
#include "queue.h"

/* Private includes ----------------------------------------------------------*/

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Private user code ---------------------------------------------------------*/

// ----------------------------------------------------------------------------
/// \brief     Queue initialisation.
///
/// \param     [in/out] queue_t *queue
/// \param     [in]     uint16_t capacity
///
/// \return    Queue_StatusTypeDef
Queue_StatusTypeDef Queue_init( Queue_t *queue, uint16_t capacity )
{
   if( capacity == 0 )
   {
      return QUEUE_ERR_PARAM;
   }
   
   queue->array = (uint8_t*)malloc(capacity);
   
   if( queue == NULL )
   {
      return QUEUE_ERR_NOMEM;
   }
   
   queue->capacity   = capacity;
   queue->size       = 0;
   queue->head       = 0;
   queue->tail       = 0;
   
	return QUEUE_OK;
}

// ----------------------------------------------------------------------------
/// \brief     Queue enqueue.
///
/// \param     [in/out] queue_t *queue
/// \param     [in]     uint8_t item
///
/// \return    Queue_StatusTypeDef
Queue_StatusTypeDef Queue_enqueue( Queue_t *queue, uint8_t item )
{
   if( queue == NULL )
   {
      return QUEUE_ERR_PARAM;
   }
   
   if( queue->size == queue->capacity )
   {
      return QUEUE_ERR_NPSPC;
   }
   
   queue->array[queue->tail%queue->capacity] = item;
   queue->tail++;
   queue->size++;
   
   return QUEUE_OK;
}

// ----------------------------------------------------------------------------
/// \brief     Queue dequeue.
///
/// \param     [in/out] queue_t *queue
/// \param     [in/out] uint8_t *item
///
/// \return    Queue_StatusTypeDef
Queue_StatusTypeDef Queue_dequeue( Queue_t *queue, uint8_t *item )
{
   if( queue->size == 0 )
   {
      return QUEUE_ERR_EMPTY;
   }
   
   if( item == NULL )
   {
      return QUEUE_ERR_PARAM;
   }
   
   *item = queue->array[queue->head%queue->capacity];
   queue->head++;
   queue->size--;
   
   return QUEUE_OK; 
}

// ----------------------------------------------------------------------------
/// \brief     Frees memory.
///
/// \param     [in/out] queue_t *queue
///
/// \return    Queue_StatusTypeDef
Queue_StatusTypeDef Queue_kill( Queue_t *queue )
{
   if( queue == NULL )
   {
      return QUEUE_ERR_PARAM;
   }
   
   free( queue->array );
   
   return QUEUE_OK; 
}

// ----------------------------------------------------------------------------
/// \brief     Queue test.
///
/// \param     none
///
/// \return    Queue_StatusTypeDef
Queue_StatusTypeDef Queue_test( void )
{
   Queue_t queue_test;
   uint16_t queue_capcacity = 10;
   uint8_t queue_items[10] = {0,1,2,3,4,5,6,7,8,9};
   uint8_t queue_item;
   
   if( Queue_init( &queue_test, queue_capcacity ) != QUEUE_OK )
   {
      return QUEUE_ERR_TEST;
   }
   
   // fill queue
   for( uint16_t i=0; i<queue_capcacity; i++ )
   {
      Queue_enqueue( &queue_test, queue_items[i] );
   }
   
   // must be full now
   if( Queue_enqueue( &queue_test, 0xff ) != QUEUE_ERR_NPSPC )
   {
      return QUEUE_ERR_TEST;
   }
   
   // check dequeue
   for( uint16_t i=0; i<queue_capcacity; i++ )
   {
      Queue_dequeue( &queue_test, &queue_item );
      if( queue_item != queue_items[i] )
      {
         return QUEUE_ERR_TEST;
      }
   }
   
   // must be empty now
   if( Queue_dequeue( &queue_test, &queue_item ) != QUEUE_ERR_EMPTY )
   {
      return QUEUE_ERR_TEST;
   }
   
   // kill the queue now
   if( Queue_kill( &queue_test ) != QUEUE_OK )
   {
      return QUEUE_ERR_TEST;
   }
   
   return QUEUE_OK; 
}

/************************ (C) COPYRIGHT Nico Korn ***************END OF FILE****/
