/* -----------------------------------------------------------------------------
 * Copyright (c) 2013-2017 ARM Ltd.
 *
 * This software is provided 'as-is', without any express or implied warranty.
 * In no event will the authors be held liable for any damages arising from
 * the use of this software. Permission is granted to anyone to use this
 * software for any purpose, including commercial applications, and to alter
 * it and redistribute it freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software in
 *    a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 *
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any source distribution.
 *
 *
 * $Date:        6. June 2019
 * $Revision:    V1.2
 * Project:      USART Driver definitions for ST STM32H7xx
 * -------------------------------------------------------------------------- */

#ifndef __USART_STM32H7XX_H
#define __USART_STM32H7XX_H

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "Driver_USART.h"
#include "stm32h7xx_hal.h"

#include "RTE_Components.h"
#include "MX_Device.h"

#define  VM_ASYNC                      (1UL)
#define  VM_SYNC                       (2UL)
#define  VM_IRDA                       (3UL)
#define  VM_SMARTCARD                  (4UL)
#define  Asynchronous                  VM_ASYNC
#define  IrDA                          VM_IRDA

#define USART_HAL_STATUS(stat)       ((stat == HAL_OK)      ? ARM_DRIVER_OK :            \
                                     ((stat == HAL_BUSY)    ? ARM_DRIVER_ERROR_BUSY :    \
                                     ((stat == HAL_TIMEOUT) ? ARM_DRIVER_ERROR_TIMEOUT : \
                                                              ARM_DRIVER_ERROR)))



#define USARTx_RESOURCE_ALLOC(x)     extern USART_HandleTypeDef husart##x;                          \
                                     static USART_INFO            USART##x##_Info;                  \
                                     static USART_TRANSFER_INFO   USART##x##_TransferInfo;          \
                                     static const USART_RESOURCES USART##x##_Resources =  {         \
                                               &husart##x,                                          \
                                               USART##x,                                            \
                                               &USART##x##_Info,                                    \
                                               &USART##x##_TransferInfo,                            \
                                               USART##x##_DMA_USE,                                  \
                                               0                                                    \
                                             }

#define USARTx_EXPORT_DRIVER(x)                                                                                                                                                           \
static int32_t                USART##x##_Initialize      (ARM_USART_SignalEvent_t cb_event)                  { return USART_Initialize (cb_event, &USART##x##_Resources); }               \
static int32_t                USART##x##_Uninitialize    (void)                                              { return USART_Uninitialize (&USART##x##_Resources); }                       \
static int32_t                USART##x##_PowerControl    (ARM_POWER_STATE state)                             { return USART_PowerControl (state, &USART##x##_Resources); }                \
static int32_t                USART##x##_Send            (const void *data, uint32_t num)                    { return USART_Send (data, num, &USART##x##_Resources); }                    \
static int32_t                USART##x##_Receive         (void *data, uint32_t num)                          { return USART_Receive (data, num, &USART##x##_Resources); }                 \
static int32_t                USART##x##_Transfer        (const void *data_out, void *data_in, uint32_t num) { return USART_Transfer (data_out, data_in, num, &USART##x##_Resources); }   \
static uint32_t               USART##x##_GetGetTxCount   (void)                                              { return USART_GetTxCount (&USART##x##_Resources); }                         \
static uint32_t               USART##x##_GetGetRxCount   (void)                                              { return USART_GetRxCount (&USART##x##_Resources); }                         \
static int32_t                USART##x##_Control         (uint32_t control, uint32_t arg)                    { return USART_Control (control, arg, &USART##x##_Resources); }              \
static ARM_USART_STATUS       USART##x##_GetStatus       (void)                                              { return USART_GetStatus (&USART##x##_Resources); }                          \
                                                                                                                                                                                          \
ARM_DRIVER_USART Driver_USART##x = {    \
  USART_GetVersion,                     \
  USART_GetCapabilities,                \
  USART##x##_Initialize,                \
  USART##x##_Uninitialize,              \
  USART##x##_PowerControl,              \
  USART##x##_Send,                      \
  USART##x##_Receive,                   \
  USART##x##_Transfer,                  \
  USART##x##_GetGetTxCount,             \
  USART##x##_GetGetRxCount,             \
  USART##x##_Control,                   \
  USART##x##_GetStatus,                 \
  USART_SetModemControl,                \
  USART_GetModemStatus                  \
}


// DMA Use
#define USART_DMA_USE_TX           (1U << 0)
#define USART_DMA_USE_RX           (1U << 1)
#define USART_DMA_USE_TX_RX        (USART_DMA_USE_TX | USART_DMA_USE_RX)

// USART1 Configuration
#ifdef MX_USART1
#if   (MX_USART1_VM == VM_SYNC)

// USART1 used in Synchronous mode
#define USART1_MODE_SYNC         1

// USART1 DMA USE
#ifdef MX_USART1_TX_DMA_Instance
  #define USART1_DMA_USE_TX        USART_DMA_USE_TX
#else
  #define USART1_DMA_USE_TX        0
#endif
#ifdef MX_USART1_RX_DMA_Instance
  #define USART1_DMA_USE_RX        USART_DMA_USE_RX
#else
  #define USART1_DMA_USE_RX        0
#endif
#define USART1_DMA_USE            (USART1_DMA_USE_TX | USART1_DMA_USE_RX)
#endif
#endif

// USART2 Configuration
#ifdef MX_USART2
#if   (MX_USART2_VM == VM_SYNC)

// USART2 used in Synchronous mode
#define USART2_MODE_SYNC         1

// USART2 DMA USE
#ifdef MX_USART2_TX_DMA_Instance
  #define USART2_DMA_USE_TX        USART_DMA_USE_TX
#else
  #define USART2_DMA_USE_TX        0
#endif
#ifdef MX_USART2_RX_DMA_Instance
  #define USART2_DMA_USE_RX        USART_DMA_USE_RX
#else
  #define USART2_DMA_USE_RX        0
#endif
#define USART2_DMA_USE            (USART2_DMA_USE_TX | USART2_DMA_USE_RX)
#endif
#endif

// USART3 Configuration
#ifdef MX_USART3
#if   (MX_USART3_VM == VM_SYNC)

// USART3 used in Synchronous mode
#define USART3_MODE_SYNC         1

// USART3 DMA USE
#ifdef MX_USART3_TX_DMA_Instance
  #define USART3_DMA_USE_TX        USART_DMA_USE_TX
#else
  #define USART3_DMA_USE_TX        0
#endif
#ifdef MX_USART3_RX_DMA_Instance
  #define USART3_DMA_USE_RX        USART_DMA_USE_RX
#else
  #define USART3_DMA_USE_RX        0
#endif
#define USART3_DMA_USE            (USART3_DMA_USE_TX | USART3_DMA_USE_RX)
#endif
#endif

// USART6 Configuration
#ifdef MX_USART6
#if   (MX_USART6_VM == VM_SYNC)

// USART6 used in Synchronous mode
#define USART6_MODE_SYNC         1

// USART6 DMA USE
#ifdef MX_USART6_TX_DMA_Instance
  #define USART6_DMA_USE_TX        USART_DMA_USE_TX
#else
  #define USART6_DMA_USE_TX        0
#endif
#ifdef MX_USART6_RX_DMA_Instance
  #define USART6_DMA_USE_RX        USART_DMA_USE_RX
#else
  #define USART6_DMA_USE_RX        0
#endif
#define USART6_DMA_USE            (USART6_DMA_USE_TX | USART6_DMA_USE_RX)
#endif
#endif

#if defined(USART1_MODE_SYNC) ||     \
    defined(USART2_MODE_SYNC) ||     \
    defined(USART3_MODE_SYNC) ||     \
    defined(USART6_MODE_SYNC)

#define USARTx_MODE_SYNC             1


// USART flags
#define USART_FLAG_INITIALIZED      ((uint8_t)(1U))
#define USART_FLAG_POWERED          ((uint8_t)(1U << 1))
#define USART_FLAG_CONFIGURED       ((uint8_t)(1U << 2))
#define USART_FLAG_TX_ENABLED       ((uint8_t)(1U << 3))
#define USART_FLAG_RX_ENABLED       ((uint8_t)(1U << 4))

// USART Transfer Information (Run-Time)
typedef struct _USART_TRANSFER_INFO {
  uint8_t              *rx_data;        // Pointer to receive data buffer
  uint16_t              def_val;        // Default transfer value
  uint16_t              dma_flag;       // DMA used for transfer
} USART_TRANSFER_INFO;

typedef struct _USART_STATUS {
  uint8_t tx_busy;                      // Transmitter busy flag
  uint8_t rx_busy;                      // Receiver busy flag
  uint8_t tx_underflow;                 // Transmit data underflow detected (cleared on start of next send operation)
  uint8_t rx_overflow;                  // Receive data overflow detected (cleared on start of next receive operation)
  uint8_t rx_break;                     // Break detected on receive (cleared on start of next receive operation)
  uint8_t rx_framing_error;             // Framing error detected on receive (cleared on start of next receive operation)
  uint8_t rx_parity_error;              // Parity error detected on receive (cleared on start of next receive operation)
  uint8_t reserved;
} USART_STATUS;

// USART Information (Run-time)
typedef struct _USART_INFO {
  ARM_USART_SignalEvent_t cb_event;     // Event Callback
  USART_STATUS            status;       // Status flags
  uint8_t                 flags;        // Current USART flags
  uint8_t                 reserved[3];
} USART_INFO;

// USART Resources definition
typedef const struct {
  USART_HandleTypeDef    *h;
  USART_TypeDef          *reg;          // USART peripheral pointer
  USART_INFO             *info;         // Run-Time Information
  USART_TRANSFER_INFO    *xfer;         // USART transfer information
  uint8_t                 dma_use;
  uint8_t                 reserved[3];
} USART_RESOURCES;

// Global functions and variables exported by driver .c module
#ifdef USART1_MODE_SYNC
extern ARM_DRIVER_USART Driver_USART1;
#endif

#ifdef USART2_MODE_SYNC
extern ARM_DRIVER_USART Driver_USART2;
#endif

#ifdef USART3_MODE_SYNC
extern ARM_DRIVER_USART Driver_USART3;
#endif

#ifdef USART6_MODE_SYNC
extern ARM_DRIVER_USART Driver_USART6;
#endif


#endif
#endif /* __USART_STM32H7XX_H */
