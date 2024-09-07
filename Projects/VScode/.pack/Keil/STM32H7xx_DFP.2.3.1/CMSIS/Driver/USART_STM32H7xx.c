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
 *
 * Driver:       Driver_USART1, Driver_USART2, Driver_USART3, Driver_USART6
 *
 * Configured:   via STM32CubeMx configuration tool
 * Project:      USART Driver for ST STM32H7xx
 * --------------------------------------------------------------------------
 * Use the following configuration settings in the middleware component
 * to connect to this driver.
 *
 *   Configuration Setting                   Value   USART Interface
 *   ---------------------                   -----   --------------
 *   Connect to hardware via Driver_USART# = 1       use USART1
 *   Connect to hardware via Driver_USART# = 2       use USART2
 *   Connect to hardware via Driver_USART# = 3       use USART3
 *   Connect to hardware via Driver_USART# = 6       use USART6
 * --------------------------------------------------------------------------
 * Note:
 *  For USART DMA transfers, with DCache enabled, data buffers must be 32 Byte
 *  aligned and data size must be n*32 Bytes.
 *
 *  Get Count (USART_GetTxCount, USART_GetRxCount) functionality is supported
 *  only in interrupt mode. When DMA is used, USART_GetTxCount and
 *  USART_GetRxCount will return 0.
 * -------------------------------------------------------------------------- */

/* History:
 *  Version 1.2
 *    Updated Get Count functionality
 *  Version 1.1
 *    Corrected UARTx/USARTx related typing mistakes
 *  Version 1.0
 *    Initial release
 */

/*! \page stm32h7_usart CMSIS-Driver USART Setup

The CMSIS-Driver USART requires:
  - Setup of USART/UART mode (Asynchronous, Synchronous, Single wire, IrDA or SmartCard)
 
Valid settings for various evaluation boards are listed in the table below:

Peripheral Resource | STM32H743I-EVAL    |
:-------------------|:-------------------|
USART Peripheral    | USART1             |
USART Mode          | <b>Synchronous</b> |
TX Pin              | PB14               |
RX Pin              | PB15               |
CK Pin              | PA8                |

For different boards, refer to the hardware schematics to reflect correct setup values.

The STM32CubeMX configuration for STM32H743I-EVAL with steps for Pinout, Clock, and System Configuration are 
listed below. Enter the values that are marked \b bold.
   
Pinout tab
----------
  1. Configure USART1 mode
     - Peripherals \b USART1: Mode=<b>Synchronous</b>
          
Clock Configuration tab
-----------------------
  1. Configure USART1 Clock.
  
Configuration tab
-----------------
  1. Under Connectivity open \b USART1 Configuration:
     - \e optional <b>DMA Settings</b>: setup DMA transfers for Rx and Tx (DMA is optional)\n
       \b Add - Select \b USART1_RX: Stream=DMA1 Channel 3, DMA Request Settings: not used\n
       \b Add - Select \b USART1_TX: Stream=DMA1 Channel 4, DMA Request Settings: not used

     - <b>GPIO Settings</b>: review settings, no changes required
          Pin Name | Signal on Pin | GPIO mode | GPIO Pull-up/Pull..| Maximum out | User Label
          :--------|:--------------|:----------|:-------------------|:------------|:----------
          PB14     | USART1_TX     | Alternate | Pull-up            | Very High   |.  
          PB15     | USART1_RX     | Alternate | Pull-up            | Very High   |.
          PA8      | USART1_CK     | Alternate | No pull-up and no..| Very High   |.
     - <b>NVIC Settings</b>: enable interrupts
          Interrupt Table                      | Enable | Preemption Priority | Sub Priority
          :------------------------------------|:-------|:--------------------|:--------------
          USART1 global interrupt              |\b ON   | 0                   | 0
          DMA1 stream3 global interrupt        |   ON   | 0                   | 0
          DMA1 stream4 global interrupt        |   ON   | 0                   | 0
     - Parameter Settings: not used
     - User Constants: not used
     - Click \b OK to close the USART1 Configuration dialog

  2. Open <b>Project - Settings - Advanced Settings</b> from the menu and enable "Not Generate Function call" for 
     MX_USART1_Init 
*/

#include "USART_STM32H7xx.h"
#ifdef USARTx_MODE_SYNC

#define ARM_USART_DRV_VERSION ARM_DRIVER_VERSION_MAJOR_MINOR(1,2)

// Driver Version
static const ARM_DRIVER_VERSION usart_driver_version = { ARM_USART_API_VERSION, ARM_USART_DRV_VERSION };

static const ARM_USART_CAPABILITIES Capabilities = {
    0,      ///< supports UART (Asynchronous) mode 
    1,      ///< supports Synchronous Master mode
    0,      ///< supports Synchronous Slave mode
    0,      ///< supports UART Single-wire mode
    0,      ///< supports UART IrDA mode
    0,      ///< supports UART Smart Card mode
    0,      ///< Smart Card Clock generator available
    0,      ///< RTS Flow Control available
    0,      ///< CTS Flow Control available
    1,      ///< Transmit completed event: \ref ARM_USART_EVENT_TX_COMPLETE
    0,      ///< Signal receive character timeout event: \ref ARM_USART_EVENT_RX_TIMEOUT
    0,      ///< RTS Line: 0=not available, 1=available
    0,      ///< CTS Line: 0=not available, 1=available
    0,      ///< DTR Line: 0=not available, 1=available
    0,      ///< DSR Line: 0=not available, 1=available
    0,      ///< DCD Line: 0=not available, 1=available
    0,      ///< RI Line: 0=not available, 1=available
    0,      ///< Signal CTS change event: \ref ARM_USART_EVENT_CTS
    0,      ///< Signal DSR change event: \ref ARM_USART_EVENT_DSR
    0,      ///< Signal DCD change event: \ref ARM_USART_EVENT_DCD
    0       ///< Signal RI change event: \ref ARM_USART_EVENT_RI
#if (defined(ARM_USART_API_VERSION) && (ARM_USART_API_VERSION >= 0x203U))
  , 0U  // Reserved bits
#endif
  };

// USART1
#ifdef USART1_MODE_SYNC
USARTx_RESOURCE_ALLOC(1);
#endif

// USART2
#ifdef USART2_MODE_SYNC
USARTx_RESOURCE_ALLOC(2);
#endif

// USART3
#ifdef USART3_MODE_SYNC
USARTx_RESOURCE_ALLOC(3);
#endif

// USART6
#ifdef USART6_MODE_SYNC
USARTx_RESOURCE_ALLOC(6);
#endif

/**
  \fn          const USART_RESOURCES USART_Resources (USART_HandleTypeDef *husart)
  \brief       Get USART_RESOURCES strusture from USART_HandleTypeDef
*/
static const USART_RESOURCES * USART_Resources (USART_HandleTypeDef *husart) {
  const USART_RESOURCES *usart = NULL;

#ifdef USART1_MODE_SYNC
  if (husart->Instance == USART1) { usart = &USART1_Resources; }
#endif
#ifdef USART2_MODE_SYNC
  if (husart->Instance == USART2) { usart = &USART2_Resources; }
#endif
#ifdef USART3_MODE_SYNC
  if (husart->Instance == USART3) { usart = &USART3_Resources; }
#endif
#ifdef USART6_MODE_SYNC
  if (husart->Instance == USART6) { usart = &USART6_Resources; }
#endif

  return usart;
}

/**
  \fn          void USART_PeripheralReset (USART_TypeDef *usart)
  \brief       USART Reset
*/
static void USART_PeripheralReset (USART_TypeDef *usart) {

#ifdef USART1_MODE_SYNC
  if (usart == USART1) { __HAL_RCC_USART1_FORCE_RESET(); }
#endif
#ifdef USART2_MODE_SYNC
  if (usart == USART2) { __HAL_RCC_USART2_FORCE_RESET(); }
#endif
#ifdef USART3_MODE_SYNC
  if (usart == USART3) { __HAL_RCC_USART3_FORCE_RESET(); }
#endif
#ifdef USART6_MODE_SYNC
  if (usart == USART6) { __HAL_RCC_USART6_FORCE_RESET(); }
#endif

      __NOP(); __NOP(); __NOP(); __NOP(); 
#ifdef USART1_MODE_SYNC
  if (usart == USART1) { __HAL_RCC_USART1_RELEASE_RESET(); }
#endif
#ifdef USART2_MODE_SYNC
  if (usart == USART2) { __HAL_RCC_USART2_RELEASE_RESET(); }
#endif
#ifdef USART3_MODE_SYNC
  if (usart == USART3) { __HAL_RCC_USART3_RELEASE_RESET(); }
#endif
#ifdef USART6_MODE_SYNC
  if (usart == USART6) { __HAL_RCC_USART6_RELEASE_RESET(); }
#endif
}

// USART Driver functions

/**
  \fn          ARM_DRIVER_VERSION USARTx_GetVersion (void)
  \brief       Get driver version.
  \return      \ref ARM_DRIVER_VERSION
*/
static ARM_DRIVER_VERSION USART_GetVersion (void) {
  return usart_driver_version;
}

/**
  \fn          ARM_USART_CAPABILITIES USART_GetCapabilities (void)
  \brief       Get driver capabilities
  \param[in]   usart     Pointer to USART resources
  \return      \ref ARM_USART_CAPABILITIES
*/
static ARM_USART_CAPABILITIES USART_GetCapabilities (void) {
  return Capabilities;
}

/**
  \fn          int32_t USART_Initialize (      ARM_USART_SignalEvent_t  cb_event
                                         const USART_RESOURCES         *usart)
  \brief       Initialize USART Interface.
  \param[in]   cb_event  Pointer to \ref ARM_USART_SignalEvent
  \param[in]   usart     Pointer to USART resources
  \return      \ref execution_status
*/
static int32_t USART_Initialize (      ARM_USART_SignalEvent_t  cb_event,
                                 const USART_RESOURCES         *usart) {

  if (usart->info->flags & USART_FLAG_INITIALIZED) {
    // Driver is already initialized
    return ARM_DRIVER_OK;
  }

  usart->h->Init.Mode = 0;

  // Initialize USART Run-time Resources
  usart->info->cb_event = cb_event;

  // Clear transfer information
  memset(usart->xfer, 0, sizeof(USART_TRANSFER_INFO));
  
  usart->h->Instance = usart->reg;

  usart->info->flags = USART_FLAG_INITIALIZED;

  return ARM_DRIVER_OK;
}

/**
  \fn          int32_t USART_Uninitialize (const USART_RESOURCES *usart)
  \brief       De-initialize USART Interface.
  \param[in]   usart     Pointer to USART resources
  \return      \ref execution_status
*/
static int32_t USART_Uninitialize (const USART_RESOURCES *usart) {

  // Reset USART status flags
  usart->info->flags = 0U;

  return ARM_DRIVER_OK;
}

/**
  \fn          int32_t USART_PowerControl (ARM_POWER_STATE state)
  \brief       Control USART Interface Power.
  \param[in]   state  Power state
  \param[in]   usart  Pointer to USART resources
  \return      \ref execution_status
*/
static int32_t USART_PowerControl (      ARM_POWER_STATE  state,
                                   const USART_RESOURCES *usart) {

  switch (state) {
    case ARM_POWER_OFF:

      // USART peripheral reset
      USART_PeripheralReset (usart->reg);

      if (usart->h->Instance != NULL) {
        HAL_USART_MspDeInit (usart->h);
      }

      usart->info->flags &= ~USART_FLAG_POWERED;
      break;
    case ARM_POWER_LOW:
      return ARM_DRIVER_ERROR_UNSUPPORTED;

    case ARM_POWER_FULL:
      if ((usart->info->flags & USART_FLAG_INITIALIZED) == 0U) {
        return ARM_DRIVER_ERROR;
      }
      if ((usart->info->flags & USART_FLAG_POWERED)     != 0U) {
        return ARM_DRIVER_OK;
      }

      usart->xfer->def_val                 = 0U;

      usart->info->flags = USART_FLAG_POWERED | USART_FLAG_INITIALIZED;

      HAL_USART_MspInit (usart->h);

      // USART peripheral reset
      USART_PeripheralReset (usart->reg);

    break;
  }
  return ARM_DRIVER_OK;
}

/**
  \fn          int32_t USART_Send (const void            *data,
                                         uint32_t         num,
                                   const USART_RESOURCES *usart)
  \brief       Start sending data to USART transmitter.
  \param[in]   data  Pointer to buffer with data to send to USART transmitter
  \param[in]   num   Number of data items to send
  \param[in]   usart Pointer to USART resources
  \return      \ref execution_status
*/
static int32_t USART_Send (const void            *data,
                                 uint32_t         num,
                           const USART_RESOURCES *usart) {

  HAL_StatusTypeDef stat;

  if ((data == NULL) || (num == 0U)) {
    // Invalid parameters
    return ARM_DRIVER_ERROR_PARAMETER;
  }

  if ((usart->info->flags & USART_FLAG_CONFIGURED) == 0U) {
    // USART is not configured (mode not selected)
    return ARM_DRIVER_ERROR;
  }

  switch (HAL_USART_GetState (usart->h)) {
    case HAL_USART_STATE_RESET:
    case HAL_USART_STATE_ERROR:
      return ARM_DRIVER_ERROR;

    case HAL_USART_STATE_TIMEOUT:
      return ARM_DRIVER_ERROR_TIMEOUT;

    case HAL_USART_STATE_BUSY:
    case HAL_USART_STATE_BUSY_TX:
    case HAL_USART_STATE_BUSY_RX:
    case HAL_USART_STATE_BUSY_TX_RX:
      return ARM_DRIVER_ERROR_BUSY;

    case HAL_USART_STATE_READY:
      break;
  }

  usart->xfer->dma_flag = 0U;
  if (usart->dma_use != 0U) {
#if (__DCACHE_PRESENT == 1U)
    // Is DChache enabled
    if ((SCB->CCR & SCB_CCR_DC_Msk) != 0U) {
      if ((((uint32_t)data & 0x1FU) == 0) && ((num & 0x1FU) == 0)) {
        // Data is 32Byte aligned and size is n *32 -> DMA can be used
        usart->xfer->dma_flag = 1U;
        SCB_CleanDCache_by_Addr ((uint32_t *)((uint32_t)data), (int32_t)num);
      }
    }
#else
    dma_flag = 1U;
#endif
  }

  if (usart->xfer->dma_flag != 0U) {
    // DMA mode
    stat = HAL_USART_Transmit_DMA (usart->h, (uint8_t *)(uint32_t)data, (uint16_t)num);
  } else {
    // Interrupt mode
    stat = HAL_USART_Transmit_IT (usart->h, (uint8_t *)(uint32_t)data, (uint16_t)num);
  }

  switch (stat) {
    case HAL_ERROR:
    case HAL_TIMEOUT:
      return ARM_DRIVER_ERROR;

    case HAL_BUSY:
      return ARM_DRIVER_ERROR_BUSY;

    case HAL_OK:
      break;
  }

  return ARM_DRIVER_OK;
}

/**
  \fn          int32_t USART_Receive (      void            *data,
                                            uint32_t         num,
                                      const USART_RESOURCES *usart)
  \brief       Start receiving data from USART receiver.
  \param[out]  data  Pointer to buffer for data to receive from USART receiver
  \param[in]   num   Number of data items to receive
  \param[in]   usart Pointer to USART resources
  \return      \ref execution_status
*/
static int32_t USART_Receive (      void            *data,
                                    uint32_t         num,
                              const USART_RESOURCES *usart) {

  HAL_StatusTypeDef stat;

  if ((data == NULL) || (num == 0U)) {
    // Invalid parameters
    return ARM_DRIVER_ERROR_PARAMETER;
  }

  if ((usart->info->flags & USART_FLAG_CONFIGURED) == 0U) {
    // USART is not configured (mode not selected)
    return ARM_DRIVER_ERROR;
  }

  switch (HAL_USART_GetState (usart->h)) {
    case HAL_USART_STATE_RESET:
    case HAL_USART_STATE_ERROR:
      return ARM_DRIVER_ERROR;

    case HAL_USART_STATE_TIMEOUT:
      return ARM_DRIVER_ERROR_TIMEOUT;

    case HAL_USART_STATE_BUSY:
    case HAL_USART_STATE_BUSY_TX:
    case HAL_USART_STATE_BUSY_RX:
    case HAL_USART_STATE_BUSY_TX_RX:
      return ARM_DRIVER_ERROR_BUSY;

    case HAL_USART_STATE_READY:
      break;
  }

  // Save buffer info
  usart->xfer->rx_data = data;

  usart->xfer->dma_flag = 0U;
  if (usart->dma_use != 0U) {
#if (__DCACHE_PRESENT == 1U)
    // Is DChache enabled
    if ((SCB->CCR & SCB_CCR_DC_Msk) != 0U) {
      if ((((uint32_t)data & 0x1FU) == 0) && ((num & 0x1FU) == 0)) {
        // Data is 32Byte aligned and size is n *32 -> DMA can be used
        usart->xfer->dma_flag = 1U;
      }
    }
#else
    usart->xfer->dma_flag = 1U;
#endif
  }

  if (usart->xfer->dma_flag != 0U) {
    // DMA mode
    stat = HAL_USART_Receive_DMA (usart->h, (uint8_t *)(uint32_t)data, (uint16_t)num);
  } else {
    // Interrupt mode
    stat = HAL_USART_Receive_IT (usart->h, (uint8_t *)(uint32_t)data, (uint16_t)num);
  }

  switch (stat) {
    case HAL_ERROR:
    case HAL_TIMEOUT:
      return ARM_DRIVER_ERROR;

    case HAL_BUSY:
      return ARM_DRIVER_ERROR_BUSY;

    case HAL_OK:
      break;
  }

  return ARM_DRIVER_OK;
}

/**
  \fn          int32_t USART_Transfer (const void             *data_out,
                                             void             *data_in,
                                             uint32_t          num,
                                       const USART_RESOURCES  *usart)
  \brief       Start sending/receiving data to/from USART transmitter/receiver.
  \param[in]   data_out  Pointer to buffer with data to send to USART transmitter
  \param[out]  data_in   Pointer to buffer for data to receive from USART receiver
  \param[in]   num       Number of data items to transfer
  \param[in]   usart     Pointer to USART resources
  \return      \ref execution_status
*/
static int32_t USART_Transfer (const void             *data_out,
                                     void             *data_in,
                                     uint32_t          num,
                               const USART_RESOURCES  *usart) {

  HAL_StatusTypeDef stat;

  if ((data_out == NULL) || (data_in == NULL) || (num == 0U)) {
    // Invalid parameters
    return ARM_DRIVER_ERROR_PARAMETER;
  }

  if ((usart->info->flags & USART_FLAG_CONFIGURED) == 0U) {
    // USART is not configured (mode not selected)
    return ARM_DRIVER_ERROR;
  }

  switch (HAL_USART_GetState (usart->h)) {
    case HAL_USART_STATE_RESET:
    case HAL_USART_STATE_ERROR:
      return ARM_DRIVER_ERROR;

    case HAL_USART_STATE_TIMEOUT:
      return ARM_DRIVER_ERROR_TIMEOUT;

    case HAL_USART_STATE_BUSY:
    case HAL_USART_STATE_BUSY_TX:
    case HAL_USART_STATE_BUSY_RX:
    case HAL_USART_STATE_BUSY_TX_RX:
      return ARM_DRIVER_ERROR_BUSY;

    case HAL_USART_STATE_READY:
      break;
  }

  usart->xfer->dma_flag = 0U;
  if (usart->dma_use != 0U) {
#if (__DCACHE_PRESENT == 1U)
    // Is DChache enabled
    if ((SCB->CCR & SCB_CCR_DC_Msk) != 0U) {
      if ((((uint32_t)data_out & 0x1FU) == 0) &&
          (((uint32_t)data_in  & 0x1FU) == 0) &&
            ((num & 0x1FU) == 0)) {
        // Data is 32Byte aligned and size is n *32 -> DMA can be used
        usart->xfer->dma_flag = 1U;
        SCB_CleanDCache_by_Addr ((uint32_t *)((uint32_t)data_out), (int32_t)num);
      }
    }
#else
    usart->xfer->dma_flag = 1U;
#endif
  }

  if (usart->xfer->dma_flag != 0U) {
    // DMA mode
    stat = HAL_USART_TransmitReceive_DMA (usart->h, (uint8_t *)(uint32_t)data_out, (uint8_t *)(uint32_t)data_in, (uint16_t)num);
  } else {
    // Interrupt mode
    stat = HAL_USART_TransmitReceive_IT (usart->h, (uint8_t *)(uint32_t)data_out, (uint8_t *)(uint32_t)data_in, (uint16_t)num);
  }

  switch (stat) {
    case HAL_ERROR:
    case HAL_TIMEOUT:
      return ARM_DRIVER_ERROR;

    case HAL_BUSY:
      return ARM_DRIVER_ERROR_BUSY;

    case HAL_OK:
      break;
  }

  return ARM_DRIVER_OK;
}

/**
  \fn          uint32_t USART_GetTxCount (const USART_RESOURCES *usart)
  \brief       Get transmitted data count.
  \param[in]   usart     Pointer to USART resources
  \return      number of data items transmitted
*/
static uint32_t USART_GetTxCount (const USART_RESOURCES *usart) {
  uint32_t cnt;

  if (usart->xfer->dma_flag == 0U) {
    cnt = usart->h->TxXferSize - usart->h->TxXferCount;
  } else {
    cnt = 0U;
  }
  return cnt;
}

/**
  \fn          uint32_t USART_GetRxCount (const USART_RESOURCES *usart)
  \brief       Get received data count.
  \param[in]   usart     Pointer to USART resources
  \return      number of data items received
*/
static uint32_t USART_GetRxCount (const USART_RESOURCES *usart) {
  uint32_t cnt;

  if (usart->xfer->dma_flag == 0U) {
    cnt = usart->h->RxXferSize - usart->h->RxXferCount;
  } else {
    cnt = 0U;
  }
  return cnt;
}

/**
  \fn          int32_t USART_Control (      uint32_t          control,
                                            uint32_t          arg,
                                      const USART_RESOURCES  *usart)
  \brief       Control USART Interface.
  \param[in]   control  Operation
  \param[in]   arg      Argument of operation (optional)
  \param[in]   usart    Pointer to USART resources
  \return      common \ref execution_status and driver specific \ref usart_execution_status
*/
static int32_t USART_Control (      uint32_t          control,
                                    uint32_t          arg,
                              const USART_RESOURCES  *usart) {

  HAL_StatusTypeDef status;

  if ((usart->info->flags & USART_FLAG_POWERED) == 0U) {
    // USART not powered
    return ARM_DRIVER_ERROR;
  }

  switch (control & ARM_USART_CONTROL_Msk) {
     // Control break
    case ARM_USART_CONTROL_BREAK:
      return ARM_DRIVER_ERROR_UNSUPPORTED;

    // Abort
    case ARM_USART_ABORT_SEND:
    case ARM_USART_ABORT_RECEIVE: 
    case ARM_USART_ABORT_TRANSFER:
      HAL_USART_Abort(usart->h);
      return ARM_DRIVER_OK;

    // Control TX
    case ARM_USART_CONTROL_TX:
      if (arg) {
        // Transmitter enable
        usart->h->Init.Mode |=  USART_MODE_TX;
      } else {
        // Transmitter disable
        usart->h->Init.Mode &= ~USART_MODE_TX;
      }
      status = HAL_USART_Init(usart->h);
      return USART_HAL_STATUS(status);

    // Control RX
    case ARM_USART_CONTROL_RX:
      if (arg) {
        usart->h->Init.Mode |= USART_MODE_RX;
      } else {
        // Receiver disable
        usart->h->Init.Mode &= ~USART_MODE_RX;
      }
      status = HAL_USART_Init(usart->h);
      return USART_HAL_STATUS(status);
    default: break;
  }

  switch (control & ARM_USART_CONTROL_Msk) {
    case ARM_USART_MODE_ASYNCHRONOUS:
    case ARM_USART_MODE_IRDA:
    case ARM_USART_MODE_SMART_CARD:
    case ARM_USART_MODE_SYNCHRONOUS_SLAVE:
    case ARM_USART_MODE_SINGLE_WIRE:
      return ARM_USART_ERROR_MODE;

    case ARM_USART_MODE_SYNCHRONOUS_MASTER:
      break;

    // Default TX value
    case ARM_USART_SET_DEFAULT_TX_VALUE:
      usart->xfer->def_val = (uint16_t)arg;
      return ARM_DRIVER_OK;

    default: { return ARM_DRIVER_ERROR_UNSUPPORTED; }
  }

  // USART Data bits
  switch (control & ARM_USART_DATA_BITS_Msk) {
    case ARM_USART_DATA_BITS_6:
      if ((control & ARM_USART_PARITY_Msk) != ARM_USART_PARITY_NONE) {
        usart->h->Init.WordLength = USART_WORDLENGTH_7B;
      } else {
        return ARM_USART_ERROR_DATA_BITS;
      }
      break;
    case ARM_USART_DATA_BITS_7:
      if ((control & ARM_USART_PARITY_Msk) == ARM_USART_PARITY_NONE) {
        usart->h->Init.WordLength = USART_WORDLENGTH_7B;
      } else {
        usart->h->Init.WordLength = USART_WORDLENGTH_8B;
      }
      break;
    case ARM_USART_DATA_BITS_8:
      if ((control & ARM_USART_PARITY_Msk) == ARM_USART_PARITY_NONE) {
        usart->h->Init.WordLength = USART_WORDLENGTH_8B;
      } else {
        usart->h->Init.WordLength = USART_WORDLENGTH_9B;
      }
      break;
    case ARM_USART_DATA_BITS_9:
      if ((control & ARM_USART_PARITY_Msk) == ARM_USART_PARITY_NONE) {
        usart->h->Init.WordLength = USART_WORDLENGTH_9B;
      } else {
        return ARM_USART_ERROR_DATA_BITS;
      }
      break;
    default: return ARM_USART_ERROR_DATA_BITS;
  }

  // USART Parity
  switch (control & ARM_USART_PARITY_Msk) {
    case ARM_USART_PARITY_NONE:
      usart->h->Init.Parity = USART_PARITY_NONE;
      break;
    case ARM_USART_PARITY_EVEN:
      usart->h->Init.Parity = USART_PARITY_EVEN;
      break;
    case ARM_USART_PARITY_ODD:
      usart->h->Init.Parity = USART_PARITY_ODD;
      break;
    default: return ARM_USART_ERROR_PARITY;
  }

  // USART Stop bits
  switch (control & ARM_USART_STOP_BITS_Msk) {
    case ARM_USART_STOP_BITS_1:
      usart->h->Init.StopBits = USART_STOPBITS_1;
      break;
    case ARM_USART_STOP_BITS_2:
      usart->h->Init.StopBits = USART_STOPBITS_2;
      break;
    case ARM_USART_STOP_BITS_1_5:
      usart->h->Init.StopBits = USART_STOPBITS_1_5;
      break;
    case ARM_USART_STOP_BITS_0_5:
      usart->h->Init.StopBits = USART_STOPBITS_0_5;
      break;
    default: return ARM_USART_ERROR_STOP_BITS;
  }

  // USART Flow control
  switch (control & ARM_USART_FLOW_CONTROL_Msk) {
    case ARM_USART_FLOW_CONTROL_NONE:
      break;
    case ARM_USART_FLOW_CONTROL_RTS:
    case ARM_USART_FLOW_CONTROL_CTS:
    case ARM_USART_FLOW_CONTROL_RTS_CTS:
    default: return ARM_USART_ERROR_FLOW_CONTROL;
  }

  // Clock setting for synchronous mode
  // Polarity
  switch (control & ARM_USART_CPOL_Msk) {
    case ARM_USART_CPOL0:
      usart->h->Init.CLKPolarity = USART_POLARITY_LOW;
      break;
    case ARM_USART_CPOL1:
      usart->h->Init.CLKPolarity = USART_POLARITY_HIGH;
      break;
    default: return ARM_USART_ERROR_CPOL;
  }

  // Phase
  switch (control & ARM_USART_CPHA_Msk) {
    case ARM_USART_CPHA0:
      usart->h->Init.CLKPhase = USART_PHASE_1EDGE;
      break;
    case ARM_USART_CPHA1:
      usart->h->Init.CLKPhase = USART_PHASE_2EDGE;
      break;
    default: return ARM_USART_ERROR_CPHA;
  }

  // USART Baudrate
  usart->h->Init.BaudRate = arg;

  // Set configured flag
  usart->info->flags |= USART_FLAG_CONFIGURED;

  // Initialize USART
  status = HAL_USART_Init(usart->h);

  // Reconfigure DMA
  if (usart->dma_use != 0) {
    if (usart->h->hdmatx != NULL) {
      if ((control & ARM_USART_DATA_BITS_Msk) > ARM_USART_DATA_BITS_8) {
        usart->h->hdmatx->Init.MemDataAlignment    = DMA_MDATAALIGN_HALFWORD;
        usart->h->hdmatx->Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
      } else {
        usart->h->hdmatx->Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
        usart->h->hdmatx->Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
      }
      HAL_DMA_Init(usart->h->hdmatx);
    }

    if (usart->h->hdmarx != NULL) {
      if ((control & ARM_USART_DATA_BITS_Msk) > ARM_USART_DATA_BITS_8) {
        usart->h->hdmarx->Init.MemDataAlignment    = DMA_MDATAALIGN_HALFWORD;
        usart->h->hdmarx->Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
      } else {
        usart->h->hdmarx->Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
        usart->h->hdmarx->Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
      }
      HAL_DMA_Init(usart->h->hdmarx);
    }
  }
  return USART_HAL_STATUS(status);
}

/**
  \fn          ARM_USART_STATUS USART_GetStatus (const USART_RESOURCES *usart)
  \brief       Get USART status.
  \param[in]   usart     Pointer to USART resources
  \return      USART status \ref ARM_USART_STATUS
*/
static ARM_USART_STATUS USART_GetStatus (const USART_RESOURCES *usart) {
  ARM_USART_STATUS  status;
  uint32_t          error;

  status.rx_busy = 0;
  status.tx_busy = 0;
  switch (HAL_USART_GetState (usart->h)) {
    case HAL_USART_STATE_BUSY:
    case HAL_USART_STATE_BUSY_TX_RX:
      status.rx_busy = 1;
      status.tx_busy = 1;
      break;
    case HAL_USART_STATE_BUSY_TX:
      status.tx_busy = 1;
      break;
    case HAL_USART_STATE_BUSY_RX:
      status.rx_busy = 1;
      break;

    case HAL_USART_STATE_TIMEOUT:
    case HAL_USART_STATE_READY:
    case HAL_USART_STATE_RESET:
    case HAL_USART_STATE_ERROR:
      break;
  }

  error = HAL_USART_GetError (usart->h);

  if (error & HAL_USART_ERROR_PE) {
    status.rx_parity_error = 1;
  } else {
    status.rx_parity_error = 0;
  }

  if (error & HAL_USART_ERROR_FE) {
    status.rx_framing_error = 1;
  } else {
    status.rx_framing_error = 0;
  }

  if (error & HAL_USART_ERROR_ORE) {
    status.rx_overflow = 1;
  } else {
    status.rx_overflow = 0;
  }

  status.tx_underflow  = 0;

  return status;
}

/**
  \fn          int32_t USART_SetModemControl (ARM_USART_MODEM_CONTROL  control)
  \brief       Set USART Modem Control line state.
  \param[in]   control   \ref ARM_USART_MODEM_CONTROL
  \return      \ref execution_status
*/
static int32_t USART_SetModemControl (ARM_USART_MODEM_CONTROL  control) {

  (void) control;

  // No modem control in synchronous mode
  return ARM_DRIVER_ERROR;
}

/**
  \fn          ARM_USART_MODEM_STATUS USART_GetModemStatus (void)
  \brief       Get USART Modem Status lines state.
  \return      modem status \ref ARM_USART_MODEM_STATUS
*/
static ARM_USART_MODEM_STATUS USART_GetModemStatus (void) {
  ARM_USART_MODEM_STATUS modem_status;

  modem_status.cts = 0U;
  modem_status.dsr = 0U;
  modem_status.ri  = 0U;
  modem_status.dcd = 0U;

  return modem_status;
}

/**
  * @brief Tx Transfer completed callback.
  * @param husart: USART handle.
  * @retval None
  */
void HAL_USART_TxCpltCallback(USART_HandleTypeDef *husart) {
  const USART_RESOURCES * usart;

  usart = USART_Resources (husart);

  if (usart->info->cb_event != NULL) {
    usart->info->cb_event(ARM_USART_EVENT_TX_COMPLETE | ARM_USART_EVENT_SEND_COMPLETE);
  }
}

/**
  * @brief  Rx Transfer completed callback.
  * @param husart: USART handle.
  * @retval None
  */
void HAL_USART_RxCpltCallback(USART_HandleTypeDef *husart) {
  const USART_RESOURCES * usart;

  usart = USART_Resources (husart);

#if (__DCACHE_PRESENT == 1U)
  if (usart->xfer->dma_flag != 0) {
    SCB_InvalidateDCache_by_Addr ((uint32_t *)((uint32_t)usart->xfer->rx_data), (int32_t)usart->h->RxXferSize);
  }
#endif

  if (usart->info->cb_event != NULL) {
    usart->info->cb_event(ARM_USART_EVENT_RECEIVE_COMPLETE);
  }
}

/**
  * @brief Tx/Rx Transfers completed callback for the non-blocking process.
  * @param husart: USART handle.
  * @retval None
  */
void HAL_USART_TxRxCpltCallback(USART_HandleTypeDef *husart) {
  const USART_RESOURCES * usart;

  usart = USART_Resources (husart);

#if (__DCACHE_PRESENT == 1U)
  if (usart->xfer->dma_flag != 0) {
    SCB_InvalidateDCache_by_Addr ((uint32_t *)((uint32_t)usart->xfer->rx_data), (int32_t)usart->h->RxXferSize);
  }
#endif

  if (usart->info->cb_event != NULL) {
    usart->info->cb_event(ARM_USART_EVENT_TRANSFER_COMPLETE);
  }
}

/**
  * @brief USART error callback.
  * @param husart: USART handle.
  * @retval None
  */
void HAL_USART_ErrorCallback(USART_HandleTypeDef *husart) {
  const USART_RESOURCES * usart;
        uint32_t          error;
        uint32_t          event;

  usart = USART_Resources (husart);

  error = HAL_USART_GetError (usart->h);
  event = 0;

  if (error & HAL_USART_ERROR_PE) {
    event |= ARM_USART_EVENT_RX_PARITY_ERROR;
  }
  if (error & HAL_USART_ERROR_FE) {
    event |= ARM_USART_EVENT_RX_FRAMING_ERROR;
  }
  if (error & HAL_USART_ERROR_ORE) {
    event |= ARM_USART_EVENT_RX_OVERFLOW;
  }

  if ((event != 0) && (usart->info->cb_event != NULL)) {
    usart->info->cb_event(event);
  }
}


#ifdef USART1_MODE_SYNC
USARTx_EXPORT_DRIVER(1);
#endif

#ifdef USART2_MODE_SYNC
USARTx_EXPORT_DRIVER(2);
#endif

#ifdef USART3_MODE_SYNC
USARTx_EXPORT_DRIVER(3);
#endif

#ifdef USART6_MODE_SYNC
USARTx_EXPORT_DRIVER(6);
#endif

#endif /*! \endcond */
