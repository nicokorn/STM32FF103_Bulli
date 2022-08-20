// ****************************************************************************
/// \file      queue.h
///
/// \brief     Queue Header File
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __QUEUE_H
#define __QUEUE_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/

/* Private defines -----------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/
typedef struct
{
	uint8_t*	array;
   uint16_t	capacity;
	uint16_t	size;
	uint16_t	head;
	uint16_t	tail;
}Queue_t;

typedef enum
{
   QUEUE_OK             = 0x00U,
   QUEUE_ERR_NOMEM      = 0x01U,
   QUEUE_ERR_PARAM      = 0x02U,
   QUEUE_ERR_NPSPC      = 0x03U,
   QUEUE_ERR_EMPTY      = 0x04U,
   QUEUE_ERR_TEST       = 0x05U
}Queue_StatusTypeDef;

/* Exported constants --------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/

/* Exported functions prototypes ---------------------------------------------*/
Queue_StatusTypeDef Queue_init      ( Queue_t *queue, uint16_t capacity );
Queue_StatusTypeDef Queue_enqueue   ( Queue_t *queue, uint8_t item );
Queue_StatusTypeDef Queue_dequeue   ( Queue_t *queue, uint8_t *item );
Queue_StatusTypeDef Queue_kill      ( Queue_t *queue );
Queue_StatusTypeDef Queue_test      ( void );

#ifdef __cplusplus
}
#endif

#endif /* __QUEUE_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
