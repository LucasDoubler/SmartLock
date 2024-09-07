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
 * $Date:        30. October 2018
 * $Revision:    V1.2
 *
 * Driver:       Driver_SPI1, Driver_SPI2, Driver_SPI3
 *               Driver_SPI4, Driver_SPI5, Driver_SPI6
 *
 * Configured:   via STM32CubeMx configuration tool
 * Project:      SPI Driver for ST STM32H7xx
 * --------------------------------------------------------------------------
 * Use the following configuration settings in the middleware component
 * to connect to this driver.
 *
 *   Configuration Setting                 Value   SPI Interface
 *   ---------------------                 -----   -------------
 *   Connect to hardware via Driver_SPI# = 1       use SPI1
 *   Connect to hardware via Driver_SPI# = 2       use SPI2
 *   Connect to hardware via Driver_SPI# = 3       use SPI3
 *   Connect to hardware via Driver_SPI# = 4       use SPI4
 *   Connect to hardware via Driver_SPI# = 5       use SPI5
 *   Connect to hardware via Driver_SPI# = 6       use SPI6
 * --------------------------------------------------------------------------
 * Note:
 *  For SPI DMA transfers, with DCache enabled, data buffers must be 32 Byte
 *  aligned and data size must be n*32 Bytes.
 * -------------------------------------------------------------------------- */

/* History:
 *  Version 1.2
 *    Corrected DCache Hanldling
 *    Removed __HAL_SPI_ENABLE from SPI_Control function
 *  Version 1.1.1
 *    Corrected typo in preprocessor condtion
 *  Version 1.1
 *    Corrected Slave select handling
 *  Version 1.0
 *    Initial release
 */
 
 /*! \page stm32h7_spi CMSIS-Driver SPI Setup 

The CMSIS-Driver SPI requires:
  - Setup of SPIx input clock
  - Setup of SPIx in Full-Duplex Master/Slave mode with optional DMA for Rx and Tx transfers

Valid settings for various evaluation boards are listed in the table below:

Peripheral Resource | STM32H743I-EVAL          |
:-------------------|:--------------------------|
SPI Peripheral      | SPI  1                    |
SPI Mode            | <b>Full-Duplex Master</b> |
MOSI Pin            | PA7                       |
MISO Pin            | PA6                       |
SCK Pin             | PA5                       |
NSS Pin             | PA4                       |

For different boards, refer to the hardware schematics to reflect correct setup values.

The STM32CubeMX configuration for STM32H743I-EVAL with steps for Pinout, Clock, and System Configuration are 
listed below. Enter the values that are marked \b bold.
   
Pinout tab
----------
  1. Configure SPI1 mode
     - Peripherals \b SPI1: Mode=<b>Full-Duplex Master</b>, Hardware NSS Signal=<b>ON</b>
          
Clock Configuration tab
-----------------------
  1. Configure SPI1 Clock.
  
Configuration tab
-----------------
  1. Under Connectivity open \b SPI1 Configuration:
     - \e optional <b>DMA Settings</b>: setup DMA transfers for Rx and Tx\n
       \b Add - Select \b SPI1_RX: Stream=DMA1 Channel 0, Direction=Peripheral to Memory, Priority=Low,
          DMA Request Settings: not used\n
       \b Add - Select \b SPI1_TX: Stream=DMA1 Channel 1, Direction=Memory to Peripheral, Priority=Low,
          DMA Request Settings: not used

     - <b>GPIO Settings</b>: review settings, no changes required
          Pin Name | Signal on Pin | GPIO mode | GPIO Pull-up/Pull..| Maximum out | User Label
          :--------|:--------------|:----------|:-------------------|:------------|:----------
          PA5      | SPI1_SCK      | Alternate | No pull-up and no..| High        |.
          PA4      | SPI1_NSS      | Alternate | No pull-up and no..| High        |.
          PA6      | SPI1_MISO     | Alternate | No pull-up and no..| High        |.
          PA7      | SPI1_MOSI     | Alternate | No pull-up and no..| High        |.

     - <b>NVIC Settings</b>: enable interrupts
          Interrupt Table                      | Enable | Preemption Priority | Sub Priority
          :------------------------------------|:-------|:--------------------|:--------------
          DMA1 channel 0 global interrupt      |   ON   | 0                   | 0
          DMA1 channel 1 global interrupt      |   ON   | 0                   | 0
          SPI1 global interrupt                |\b ON   | 0                   | 0

     - Parameter Settings: not used
     - User Constants: not used
   
     Click \b OK to close the SPI1 Configuration dialog

  2. Open <b>Project - Settings - Advanced Settings</b> from the menu and enable "Not Generate Function call" for 
     MX_SPI1_Init 
*/

#include "SPI_STM32H7xx.h"

#define ARM_SPI_DRV_VERSION ARM_DRIVER_VERSION_MAJOR_MINOR(1,2)

// Driver Version
static const ARM_DRIVER_VERSION DriverVersion = { ARM_SPI_API_VERSION, ARM_SPI_DRV_VERSION };

// Driver Capabilities
static const ARM_SPI_CAPABILITIES DriverCapabilities = {
  0,  /* Simplex Mode (Master and Slave) */
  1,  /* TI Synchronous Serial Interface */
  0,  /* Microwire Interface */
  1   /* Signal Mode Fault event: \ref ARM_SPI_EVENT_MODE_FAULT */
#if (defined(ARM_SPI_API_VERSION) && (ARM_SPI_API_VERSION >= 0x202U))
, 0U  /* Reserved bits */
#endif
};

// SPI1
#ifdef MX_SPI1
#ifdef  MX_SPI1_NSS_Pin
  SPIx_PIN_NSS_STRUCT_ALLOC(1);
#endif
// Allocate SPI Resources
SPIx_RESOURCE_ALLOC(1);
#endif

// SPI2
#ifdef MX_SPI2
#ifdef  MX_SPI2_NSS_Pin
  SPIx_PIN_NSS_STRUCT_ALLOC(2);
#endif
// Allocate SPI Resources
SPIx_RESOURCE_ALLOC(2);
#endif

// SPI3
#ifdef MX_SPI3
#ifdef  MX_SPI3_NSS_Pin
  SPIx_PIN_NSS_STRUCT_ALLOC(3);
#endif                   
// Allocate SPI Resources
SPIx_RESOURCE_ALLOC(3);
#endif

// SPI4
#ifdef MX_SPI4
#ifdef  MX_SPI4_NSS_Pin
  SPIx_PIN_NSS_STRUCT_ALLOC(4);
#endif                   
// Allocate SPI Resources
SPIx_RESOURCE_ALLOC(4);
#endif

// SPI5
#ifdef MX_SPI5
#ifdef  MX_SPI5_NSS_Pin
  SPIx_PIN_NSS_STRUCT_ALLOC(5);
#endif                   
// Allocate SPI Resources
SPIx_RESOURCE_ALLOC(5);
#endif

// SPI6
#ifdef MX_SPI6
#ifdef  MX_SPI6_NSS_Pin
  SPIx_PIN_NSS_STRUCT_ALLOC(6);
#endif                   
// Allocate SPI Resources
SPIx_RESOURCE_ALLOC(6);
#endif

/**
  \fn          const SPI_RESOURCES SPI_Resources (SPI_HandleTypeDef *hspi)
  \brief       Get SPI_RESOURCES strusture from SPI_HandleTypeDef
*/
static const SPI_RESOURCES * SPI_Resources (SPI_HandleTypeDef *hspi) {
  const SPI_RESOURCES *spi = NULL;

#ifdef MX_SPI1
  if (hspi->Instance == SPI1) { spi = &SPI1_Resources; }
#endif
#ifdef MX_SPI2
  if (hspi->Instance == SPI2) { spi = &SPI2_Resources; }
#endif
#ifdef MX_SPI3
  if (hspi->Instance == SPI3) { spi = &SPI3_Resources; }
#endif
#ifdef MX_SPI4
  if (hspi->Instance == SPI4) { spi = &SPI4_Resources; }
#endif
#ifdef MX_SPI5
  if (hspi->Instance == SPI5) { spi = &SPI5_Resources; }
#endif
#ifdef MX_SPI6
  if (hspi->Instance == SPI6) { spi = &SPI6_Resources; }
#endif

  return spi;
}

/**
  \fn          uint32_t SPI_GetClk (const SPI_RESOURCES *spi)
  \brief       Get SPI peripheral clock
*/
static uint32_t SPI_GetClk (const SPI_RESOURCES *spi) {
#if defined(MX_SPI4) || defined(MX_SPI5) || defined(MX_SPI6)
  uint32_t src;
  PLL2_ClocksTypeDef pll2_clocks;
  PLL3_ClocksTypeDef pll3_clocks;
#endif

#ifdef MX_SPI1
  if (spi->reg == SPI1) { return HAL_RCCEx_GetPeriphCLKFreq(RCC_PERIPHCLK_SPI1); }
#endif
#ifdef MX_SPI2
  if (spi->reg  == SPI2) { return HAL_RCCEx_GetPeriphCLKFreq(RCC_PERIPHCLK_SPI2); }
#endif
#ifdef MX_SPI3
  if (spi->reg  == SPI3) { return HAL_RCCEx_GetPeriphCLKFreq(RCC_PERIPHCLK_SPI3); }
#endif
#ifdef MX_SPI4
  if (spi->reg  == SPI4) {
    src = __HAL_RCC_GET_SPI4_SOURCE();
    switch (src) {
      case RCC_SPI4CLKSOURCE_D2PCLK1:
        return HAL_RCC_GetPCLK1Freq();

      case RCC_SPI4CLKSOURCE_PLL2:
        HAL_RCCEx_GetPLL2ClockFreq(&pll2_clocks);
        return (pll2_clocks.PLL2_Q_Frequency);

      case RCC_SPI4CLKSOURCE_PLL3:
        HAL_RCCEx_GetPLL3ClockFreq(&pll3_clocks);
        return (pll3_clocks.PLL3_Q_Frequency);

      case RCC_SPI4CLKSOURCE_HSI:
        if (__HAL_RCC_GET_FLAG(RCC_FLAG_HSIDIV) != 0U) {
          return (HSI_VALUE >> (__HAL_RCC_GET_HSI_DIVIDER()>> 3));
        }
        else {
          return (HSI_VALUE);
        }

      case RCC_SPI4CLKSOURCE_CSI:
        return (CSI_VALUE);

      case RCC_SPI4CLKSOURCE_HSE:
        return (HSE_VALUE);
    }
  }
#endif
#ifdef MX_SPI5
  if (spi->reg  == SPI5) {
    src = __HAL_RCC_GET_SPI5_SOURCE();
    switch (src) {
      case RCC_SPI5CLKSOURCE_D2PCLK1:
        return HAL_RCC_GetPCLK1Freq();

      case RCC_SPI5CLKSOURCE_PLL2:
        HAL_RCCEx_GetPLL2ClockFreq(&pll2_clocks);
        return (pll2_clocks.PLL2_Q_Frequency);

      case RCC_SPI5CLKSOURCE_PLL3:
        HAL_RCCEx_GetPLL3ClockFreq(&pll3_clocks);
        return (pll3_clocks.PLL3_Q_Frequency);

      case RCC_SPI5CLKSOURCE_HSI:
        if (__HAL_RCC_GET_FLAG(RCC_FLAG_HSIDIV) != 0U) {
          return (HSI_VALUE >> (__HAL_RCC_GET_HSI_DIVIDER()>> 3));
        }
        else {
          return (HSI_VALUE);
        }

      case RCC_SPI5CLKSOURCE_CSI:
        return (CSI_VALUE);

      case RCC_SPI5CLKSOURCE_HSE:
        return (HSE_VALUE);
    }
  }
#endif
#ifdef MX_SPI6
  if (spi->reg  == SPI6) {
    src = __HAL_RCC_GET_SPI6_SOURCE();
    switch (src) {
      case RCC_SPI6CLKSOURCE_D3PCLK1:
        return HAL_RCCEx_GetD3PCLK1Freq();

      case RCC_SPI6CLKSOURCE_PLL2:
        HAL_RCCEx_GetPLL2ClockFreq(&pll2_clocks);
        return (pll2_clocks.PLL2_Q_Frequency);

      case RCC_SPI6CLKSOURCE_PLL3:
        HAL_RCCEx_GetPLL3ClockFreq(&pll3_clocks);
        return (pll3_clocks.PLL3_Q_Frequency);

      case RCC_SPI6CLKSOURCE_HSI:
        if (__HAL_RCC_GET_FLAG(RCC_FLAG_HSIDIV) != 0U) {
          return (HSI_VALUE >> (__HAL_RCC_GET_HSI_DIVIDER()>> 3));
        }
        else {
          return (HSI_VALUE);
        }

      case RCC_SPI6CLKSOURCE_CSI:
        return (CSI_VALUE);

      case RCC_SPI6CLKSOURCE_HSE:
        return (HSE_VALUE);
    }
  }
#endif

  return 0;
}

/**
  \fn          void SPI_PeripheralReset (const SPI_TypeDef *spi)
  \brief       SPI Reset
*/
static void SPI_PeripheralReset (const SPI_TypeDef *spi) {

#ifdef SPI1
  if (spi == SPI1) { __HAL_RCC_SPI1_FORCE_RESET(); }
#endif
#ifdef SPI2
  if (spi == SPI2) { __HAL_RCC_SPI2_FORCE_RESET(); }
#endif
#ifdef SPI3
  if (spi == SPI3) { __HAL_RCC_SPI3_FORCE_RESET(); }
#endif
#ifdef SPI4
  if (spi == SPI4) { __HAL_RCC_SPI4_FORCE_RESET(); }
#endif
#ifdef SPI5
  if (spi == SPI5) { __HAL_RCC_SPI5_FORCE_RESET(); }
#endif
#ifdef SPI6
  if (spi == SPI6) { __HAL_RCC_SPI6_FORCE_RESET(); }
#endif

  __NOP(); __NOP(); __NOP(); __NOP(); 

#ifdef SPI1
  if (spi == SPI1) { __HAL_RCC_SPI1_RELEASE_RESET(); }
#endif
#ifdef SPI2
  if (spi == SPI2) { __HAL_RCC_SPI2_RELEASE_RESET(); }
#endif
#ifdef SPI3
  if (spi == SPI3) { __HAL_RCC_SPI3_RELEASE_RESET(); }
#endif
#ifdef SPI4
  if (spi == SPI4) { __HAL_RCC_SPI4_RELEASE_RESET(); }
#endif
#ifdef SPI5
  if (spi == SPI5) { __HAL_RCC_SPI5_RELEASE_RESET(); }
#endif
#ifdef SPI6
  if (spi == SPI6) { __HAL_RCC_SPI6_RELEASE_RESET(); }
#endif
}

/**
  \fn          ARM_DRIVER_VERSION SPIX_GetVersion (void)
  \brief       Get SPI driver version.
  \return      \ref ARM_DRV_VERSION
*/
static ARM_DRIVER_VERSION SPIX_GetVersion (void) {
  return DriverVersion;
}


/**
  \fn          ARM_SPI_CAPABILITIES SPI_GetCapabilities (void)
  \brief       Get driver capabilities.
  \return      \ref ARM_SPI_CAPABILITIES
*/
static ARM_SPI_CAPABILITIES SPIX_GetCapabilities (void) {
  return DriverCapabilities;
}


/**
  \fn          int32_t SPI_Initialize (ARM_SPI_SignalEvent_t cb_event, const SPI_RESOURCES *spi)
  \brief       Initialize SPI Interface.
  \param[in]   cb_event  Pointer to \ref ARM_SPI_SignalEvent
  \param[in]   spi       Pointer to SPI resources
  \return      \ref execution_status
*/
static int32_t SPI_Initialize (ARM_SPI_SignalEvent_t cb_event, const SPI_RESOURCES *spi) {

  if (spi->info->state & SPI_INITIALIZED) { return ARM_DRIVER_OK; }

  // Initialize SPI Run-Time Resources
  spi->info->cb_event = cb_event;

  // Clear transfer information
  memset(spi->xfer, 0, sizeof(SPI_TRANSFER_INFO));

  spi->h->Instance = spi->reg;

  spi->info->state = SPI_INITIALIZED;

  return ARM_DRIVER_OK;
}

/**
  \fn          int32_t SPI_Uninitialize (const SPI_RESOURCES *spi)
  \brief       De-initialize SPI Interface.
  \param[in]   spi  Pointer to SPI resources
  \return      \ref execution_status
*/
static int32_t SPI_Uninitialize (const SPI_RESOURCES *spi) {
  spi->h->Instance = NULL;

  // Clear SPI state
  spi->info->state = 0U;

  return ARM_DRIVER_OK;
}

/**
  \fn          int32_t SPI_PowerControl (ARM_POWER_STATE state, const SPI_RESOURCES *spi)
  \brief       Control SPI Interface Power.
  \param[in]   state  Power state
  \param[in]   spi    Pointer to SPI resources
  \return      \ref execution_status
*/
static int32_t SPI_PowerControl (ARM_POWER_STATE state, const SPI_RESOURCES *spi) {

  switch (state) {
    case ARM_POWER_OFF:
      // SPI peripheral reset
      SPI_PeripheralReset (spi->reg);
      if (spi->h->Instance != NULL) {
        HAL_SPI_MspDeInit (spi->h);
      }

      // Clear powered flag
      spi->info->state &= ~SPI_POWERED;
      break;

    case ARM_POWER_FULL:
      if ((spi->info->state & SPI_INITIALIZED) == 0U) {
        return ARM_DRIVER_ERROR;
      }
      if ((spi->info->state & SPI_POWERED)     != 0U) {
        return ARM_DRIVER_OK;
      }

      spi->xfer->def_val = 0U;

      // Ready for operation - set powered flag
      spi->info->state |= SPI_POWERED;

      HAL_SPI_MspInit (spi->h);

      // SPI peripheral reset
      SPI_PeripheralReset (spi->reg);
      break;

    case ARM_POWER_LOW:
      return ARM_DRIVER_ERROR_UNSUPPORTED;
  }
  return ARM_DRIVER_OK;
}

/**
  \fn          int32_t SPI_Send (const void *data, uint32_t num, const SPI_RESOURCES *spi)
  \brief       Start sending data to SPI transmitter.
  \param[in]   data  Pointer to buffer with data to send to SPI transmitter
  \param[in]   num   Number of data items to send
  \param[in]   spi   Pointer to SPI resources
  \return      \ref execution_status
*/
static int32_t SPI_Send (const void *data, uint32_t num, const SPI_RESOURCES *spi) {
  HAL_StatusTypeDef stat;
#ifdef __SPI_DMA_TX
  uint32_t tx_dma_flag;
#endif

  if ((data == NULL) || (num == 0U)) { return ARM_DRIVER_ERROR_PARAMETER; }

  switch (HAL_SPI_GetState (spi->h)) {
    case HAL_SPI_STATE_ABORT:
    case HAL_SPI_STATE_RESET:
    case HAL_SPI_STATE_ERROR:
      return ARM_DRIVER_ERROR;

    case HAL_SPI_STATE_BUSY:
    case HAL_SPI_STATE_BUSY_TX:
    case HAL_SPI_STATE_BUSY_RX:
    case HAL_SPI_STATE_BUSY_TX_RX:
      return ARM_DRIVER_ERROR_BUSY;

    case HAL_SPI_STATE_READY:
      break;
  }

  // Save transfer info
  spi->xfer->num = num;
  spi->xfer->cnt = 0;

#ifdef __SPI_DMA_TX
  tx_dma_flag = 0U;
  if ((spi->dma_use & SPI_DMA_USE_TX) != 0U) {
#if (__DCACHE_PRESENT == 1U)
    // Is DChache enabled
    if ((SCB->CCR & SCB_CCR_DC_Msk) != 0U) {
      if ((((uint32_t)data & 0x1FU) == 0) && ((num & 0x1FU) == 0)) {
        // Data is 32Byte aligned and size is n *32 -> DMA can be used
        tx_dma_flag = 1U;
        SCB_CleanDCache_by_Addr ((uint32_t *)((uint32_t)data), (int32_t)(num  * spi->xfer->dataSize));
      }
    } else {
      spi->xfer->dma_flag = 1U;
    }
#else
    tx_dma_flag = 1U;
#endif
  }
#endif

#ifdef __SPI_DMA_TX
  if (tx_dma_flag != 0U) {
    // DMA mode
    stat = HAL_SPI_Transmit_DMA (spi->h, (uint8_t *)(uint32_t)data, (uint16_t)num);
  } else
#endif
  {
    // Interrupt mode
    stat = HAL_SPI_Transmit_IT (spi->h, (uint8_t *)(uint32_t)data, (uint16_t)num);
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
  \fn          int32_t SPI_Receive (void *data, uint32_t num, const SPI_RESOURCES *spi)
  \brief       Start receiving data from SPI receiver.
  \param[out]  data  Pointer to buffer for data to receive from SPI receiver
  \param[in]   num   Number of data items to receive
  \param[in]   spi   Pointer to SPI resources
  \return      \ref execution_status
*/
static int32_t SPI_Receive (void *data, uint32_t num, const SPI_RESOURCES *spi) {
  uint32_t          i;
  uint8_t          *data_ptr8;
  uint16_t         *data_ptr16;
  HAL_StatusTypeDef stat;

  if ((data == NULL) || (num == 0U))  { return ARM_DRIVER_ERROR_PARAMETER; }

  switch (HAL_SPI_GetState (spi->h)) {
    case HAL_SPI_STATE_ABORT:
    case HAL_SPI_STATE_RESET:
    case HAL_SPI_STATE_ERROR:
      return ARM_DRIVER_ERROR;

    case HAL_SPI_STATE_BUSY:
    case HAL_SPI_STATE_BUSY_TX:
    case HAL_SPI_STATE_BUSY_RX:
    case HAL_SPI_STATE_BUSY_TX_RX:
      return ARM_DRIVER_ERROR_BUSY;

    case HAL_SPI_STATE_READY:
     break;
  }

  // Save transfer info
  spi->xfer->rx_data = data;
  spi->xfer->num     = num;
  spi->xfer->cnt      = 0;

  // Fill buffer with default transmit value
  if (spi->h->Init.DataSize <= SPI_DATASIZE_8BIT) {
    data_ptr8 = data;
    for (i = 0; i < num; i++) {
      *data_ptr8++ = (uint8_t)spi->xfer->def_val;
    }
  } else {
    data_ptr16 = data;
    for (i = 0; i < num; i++) {
      *data_ptr16++ = spi->xfer->def_val;
    }
  }

  spi->xfer->dma_flag = 0U;
  if ((spi->dma_use & SPI_DMA_USE_RX) != 0) {
#if (__DCACHE_PRESENT == 1U)
    // Is DChache enabled
    if ((SCB->CCR & SCB_CCR_DC_Msk) != 0U) {
      if ((((uint32_t)data & 0x1FU) == 0) && ((num & 0x1FU) == 0)) {
        SCB_CleanDCache_by_Addr ((uint32_t *)((uint32_t)data), (int32_t)(num  * spi->xfer->dataSize));
        // Data is 32Byte aligned and size is n *32 -> DMA can be used
        spi->xfer->dma_flag = 1U;
      }
    } else {
      spi->xfer->dma_flag = 1U;
    }
#else
    spi->xfer->dma_flag = 1U;
#endif
  }

#ifdef __SPI_DMA_RX
  if (spi->xfer->dma_flag != 0) {
    // DMA mode
    stat = HAL_SPI_Receive_DMA (spi->h, (uint8_t *)(uint32_t)data, (uint16_t)num);
  } else
#endif
  {
    // Interrupt mode
    stat = HAL_SPI_Receive_IT (spi->h, (uint8_t *)(uint32_t)data, (uint16_t)num);
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
  \fn          int32_t SPI_Transfer (const void *data_out, void *data_in, uint32_t num, const SPI_RESOURCES *spi)
  \brief       Start sending/receiving data to/from SPI transmitter/receiver.
  \param[in]   data_out  Pointer to buffer with data to send to SPI transmitter
  \param[out]  data_in   Pointer to buffer for data to receive from SPI receiver
  \param[in]   num       Number of data items to transfer
  \param[in]   spi       Pointer to SPI resources
  \return      \ref execution_status
*/
static int32_t SPI_Transfer (const void *data_out, void *data_in, uint32_t num, const SPI_RESOURCES *spi) {
  HAL_StatusTypeDef stat;

  if ((data_out == NULL) || (data_in == NULL) || (num == 0U)) { return ARM_DRIVER_ERROR_PARAMETER; }

  switch (HAL_SPI_GetState (spi->h)) {
    case HAL_SPI_STATE_ABORT:
    case HAL_SPI_STATE_RESET:
    case HAL_SPI_STATE_ERROR:
      return ARM_DRIVER_ERROR;

    case HAL_SPI_STATE_BUSY:
    case HAL_SPI_STATE_BUSY_TX:
    case HAL_SPI_STATE_BUSY_RX:
    case HAL_SPI_STATE_BUSY_TX_RX:
      return ARM_DRIVER_ERROR_BUSY;

    case HAL_SPI_STATE_READY:
      break;
  }

  // Save transfer info
  spi->xfer->rx_data = data_in;
  spi->xfer->num = num;
  spi->xfer->cnt = 0;

  spi->xfer->dma_flag = 0U;
  if (spi->dma_use == SPI_DMA_USE_TX_RX) {
#if (__DCACHE_PRESENT == 1U)
    // Is DChache enabled
    if ((SCB->CCR & SCB_CCR_DC_Msk) != 0U) {
      if ((((uint32_t)data_out & 0x1FU) == 0) &&
          (((uint32_t)data_in  & 0x1FU) == 0) &&
            ((num & 0x1FU) == 0)) {
        // Data is 32Byte aligned and size is n *32 -> DMA can be used
        spi->xfer->dma_flag = 1U;
        SCB_CleanDCache_by_Addr ((uint32_t *)((uint32_t)data_out), (int32_t)(num  * spi->xfer->dataSize));
      }
    } else {
      spi->xfer->dma_flag = 1U;
    }
#else
    spi->xfer->dma_flag = 1U;
#endif
  }

#ifdef __SPI_DMA
  if (spi->xfer->dma_flag != 0U) {
    // DMA mode
    stat = HAL_SPI_TransmitReceive_DMA (spi->h, (uint8_t *)(uint32_t)data_out, (uint8_t *)(uint32_t)data_in, (uint16_t)num);
  } else
#endif
  {
    // Interrupt mode
    stat = HAL_SPI_TransmitReceive_IT (spi->h, (uint8_t *)(uint32_t)data_out, (uint8_t *)(uint32_t)data_in, (uint16_t)num);
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
  \fn          uint32_t SPI_GetDataCount (const SPI_RESOURCES *spi)
  \brief       Get transferred data count.
  \param[in]   spi  Pointer to SPI resources
  \return      number of data items transferred
*/
static uint32_t SPI_GetDataCount (const SPI_RESOURCES *spi) {
  return (spi->xfer->cnt);
}

/**
  \fn          int32_t SPI_Control (uint32_t control, uint32_t arg, const SPI_RESOURCES *spi)
  \brief       Control SPI Interface.
  \param[in]   control  operation
  \param[in]   arg      argument of operation (optional)
  \param[in]   spi      Pointer to SPI resources
  \return      \ref execution_status
*/
static int32_t SPI_Control (uint32_t control, uint32_t arg, const SPI_RESOURCES *spi) {
  GPIO_InitTypeDef GPIO_InitStruct;
  uint32_t         clk, val;
  bool             recofigure_nss_pin = false;

  if ((spi->info->state & SPI_POWERED) == 0U) { return ARM_DRIVER_ERROR; }

  if ((control & ARM_SPI_CONTROL_Msk) == ARM_SPI_ABORT_TRANSFER) {
    HAL_SPI_Abort(spi->h);
    return ARM_DRIVER_OK;
  }

  // Check for busy flag
  switch (HAL_SPI_GetState (spi->h)) {
    case HAL_SPI_STATE_ABORT:
    case HAL_SPI_STATE_BUSY:
    case HAL_SPI_STATE_BUSY_TX:
    case HAL_SPI_STATE_BUSY_RX:
    case HAL_SPI_STATE_BUSY_TX_RX:
      return ARM_DRIVER_ERROR_BUSY;

    case HAL_SPI_STATE_READY:
    case HAL_SPI_STATE_RESET:
    case HAL_SPI_STATE_ERROR:
      break;
  }

  switch (control & ARM_SPI_CONTROL_Msk) {
    case ARM_SPI_MODE_INACTIVE:
      __HAL_SPI_DISABLE (spi->h);
      return ARM_DRIVER_OK;

    case ARM_SPI_MODE_MASTER:
      spi->h->Init.Mode      = SPI_MODE_MASTER;
      spi->h->Init.Direction = SPI_DIRECTION_2LINES;
      break;

    case ARM_SPI_MODE_SLAVE:
      spi->h->Init.Mode      = SPI_MODE_SLAVE;
      spi->h->Init.Direction = SPI_DIRECTION_2LINES;
      break;

    case ARM_SPI_MODE_MASTER_SIMPLEX:
      spi->h->Init.Mode      = SPI_MODE_MASTER;
      spi->h->Init.Direction = SPI_DIRECTION_1LINE;
      break;

    case ARM_SPI_MODE_SLAVE_SIMPLEX:
      spi->h->Init.Mode      = SPI_MODE_SLAVE;
      spi->h->Init.Direction = SPI_DIRECTION_1LINE;
      break;

    case ARM_SPI_SET_BUS_SPEED:
      // Set SPI Bus Speed 
      clk = SPI_GetClk(spi);

      for (val = 0U; val < 8U; val++) {
        if (arg >= (clk >> (val + 1U))) { break; }
      }
      if ((val == 8U) || (arg < (clk >> (val + 1U)))) {
        // Requested Bus Speed can not be configured
        return ARM_DRIVER_ERROR;
      }

      // Save prescaler value
      spi->h->Init.BaudRatePrescaler = (val << 3);

      if (HAL_SPI_Init (spi->h) != HAL_OK) {
        return ARM_DRIVER_ERROR;
      }
      return ARM_DRIVER_OK;

    case ARM_SPI_GET_BUS_SPEED:
      // Return current bus speed
      clk = SPI_GetClk(spi);
      return ((int32_t)(clk >> (((spi->reg->CFG1 & SPI_CFG1_MBR) >> SPI_CFG1_MBR_Pos) + 1U)));

    case ARM_SPI_SET_DEFAULT_TX_VALUE:
      spi->xfer->def_val = (uint16_t)(arg & 0xFFFFU);
      return ARM_DRIVER_OK;

    case ARM_SPI_CONTROL_SS:
      val = (spi->info->mode & ARM_SPI_CONTROL_Msk);
      // Master modes
      if (val == ARM_SPI_MODE_MASTER) {
        val = spi->info->mode & ARM_SPI_SS_MASTER_MODE_Msk;
        // Check if NSS pin is available and
        // software slave select master is selected
        if ((spi->nss != NULL) && (val == ARM_SPI_SS_MASTER_SW)) {
          // Set/Clear NSS pin
          if (arg == ARM_SPI_SS_INACTIVE) {
            // Inactive High
            HAL_GPIO_WritePin (spi->nss->port, (uint16_t)spi->nss->pin, GPIO_PIN_SET);
          } else {
            // Active Low
            HAL_GPIO_WritePin (spi->nss->port, (uint16_t)spi->nss->pin, GPIO_PIN_RESET);
          }
        } else return ARM_DRIVER_ERROR;
        return ARM_DRIVER_OK;
      }
      // Slave modes
      else if (val == ARM_SPI_MODE_SLAVE) {
        val = spi->info->mode & ARM_SPI_SS_SLAVE_MODE_Msk;
        // Check if slave select slave mode is selected
        if (val == ARM_SPI_SS_SLAVE_SW) {
          if (arg == ARM_SPI_SS_ACTIVE) {
            // Active Low
            spi->reg->CR1 &= ~SPI_CR1_SSI;
          }
          else {
            spi->reg->CR1 |= SPI_CR1_SSI;
          }
          return ARM_DRIVER_OK;
        } else { return ARM_DRIVER_ERROR; }
      } else { return ARM_DRIVER_ERROR; }

    default:
      return ARM_DRIVER_ERROR_UNSUPPORTED;
  }

  // Frame format:
  spi->h->Init.TIMode = SPI_TIMODE_DISABLE;
  switch (control & ARM_SPI_FRAME_FORMAT_Msk) {
    case ARM_SPI_CPOL0_CPHA0:
      spi->h->Init.CLKPhase    = SPI_PHASE_1EDGE;
      spi->h->Init.CLKPolarity = SPI_POLARITY_LOW;
      break;
    case ARM_SPI_CPOL0_CPHA1:
      spi->h->Init.CLKPhase    = SPI_PHASE_2EDGE;
      spi->h->Init.CLKPolarity = SPI_POLARITY_LOW;
      break;
    case ARM_SPI_CPOL1_CPHA0:
      spi->h->Init.CLKPhase    = SPI_PHASE_1EDGE;
      spi->h->Init.CLKPolarity = SPI_POLARITY_HIGH;
      break;
    case ARM_SPI_CPOL1_CPHA1:
      spi->h->Init.CLKPhase    = SPI_PHASE_2EDGE;
      spi->h->Init.CLKPolarity = SPI_POLARITY_HIGH;
      break;
    case ARM_SPI_TI_SSI:
      spi->h->Init.TIMode      = SPI_TIMODE_ENABLE;
      break;
    case ARM_SPI_MICROWIRE:
      return ARM_SPI_ERROR_FRAME_FORMAT;
    default: return ARM_SPI_ERROR_FRAME_FORMAT;
  }

  // Data Bits
  switch (control & ARM_SPI_DATA_BITS_Msk) {
    case ARM_SPI_DATA_BITS(4U):   spi->h->Init.DataSize = SPI_DATASIZE_4BIT;  break;
    case ARM_SPI_DATA_BITS(5U):   spi->h->Init.DataSize = SPI_DATASIZE_5BIT;  break;
    case ARM_SPI_DATA_BITS(6U):   spi->h->Init.DataSize = SPI_DATASIZE_6BIT;  break;
    case ARM_SPI_DATA_BITS(7U):   spi->h->Init.DataSize = SPI_DATASIZE_7BIT;  break;
    case ARM_SPI_DATA_BITS(8U):   spi->h->Init.DataSize = SPI_DATASIZE_8BIT;  break;
    case ARM_SPI_DATA_BITS(9U):   spi->h->Init.DataSize = SPI_DATASIZE_9BIT;  break;
    case ARM_SPI_DATA_BITS(10U):  spi->h->Init.DataSize = SPI_DATASIZE_10BIT; break;
    case ARM_SPI_DATA_BITS(11U):  spi->h->Init.DataSize = SPI_DATASIZE_11BIT; break;
    case ARM_SPI_DATA_BITS(12U):  spi->h->Init.DataSize = SPI_DATASIZE_12BIT; break;
    case ARM_SPI_DATA_BITS(13U):  spi->h->Init.DataSize = SPI_DATASIZE_13BIT; break;
    case ARM_SPI_DATA_BITS(14U):  spi->h->Init.DataSize = SPI_DATASIZE_14BIT; break;
    case ARM_SPI_DATA_BITS(15U):  spi->h->Init.DataSize = SPI_DATASIZE_15BIT; break;
    case ARM_SPI_DATA_BITS(16U):  spi->h->Init.DataSize = SPI_DATASIZE_16BIT; break;
    case ARM_SPI_DATA_BITS(17U):  spi->h->Init.DataSize = SPI_DATASIZE_17BIT; break;
    case ARM_SPI_DATA_BITS(18U):  spi->h->Init.DataSize = SPI_DATASIZE_18BIT; break;
    case ARM_SPI_DATA_BITS(19U):  spi->h->Init.DataSize = SPI_DATASIZE_19BIT; break;
    case ARM_SPI_DATA_BITS(20U):  spi->h->Init.DataSize = SPI_DATASIZE_20BIT; break;
    case ARM_SPI_DATA_BITS(21U):  spi->h->Init.DataSize = SPI_DATASIZE_21BIT; break;
    case ARM_SPI_DATA_BITS(22U):  spi->h->Init.DataSize = SPI_DATASIZE_22BIT; break;
    case ARM_SPI_DATA_BITS(23U):  spi->h->Init.DataSize = SPI_DATASIZE_23BIT; break;
    case ARM_SPI_DATA_BITS(24U):  spi->h->Init.DataSize = SPI_DATASIZE_24BIT; break;
    case ARM_SPI_DATA_BITS(25U):  spi->h->Init.DataSize = SPI_DATASIZE_25BIT; break;
    case ARM_SPI_DATA_BITS(26U):  spi->h->Init.DataSize = SPI_DATASIZE_26BIT; break;
    case ARM_SPI_DATA_BITS(27U):  spi->h->Init.DataSize = SPI_DATASIZE_27BIT; break;
    case ARM_SPI_DATA_BITS(28U):  spi->h->Init.DataSize = SPI_DATASIZE_28BIT; break;
    case ARM_SPI_DATA_BITS(29U):  spi->h->Init.DataSize = SPI_DATASIZE_29BIT; break;
    case ARM_SPI_DATA_BITS(30U):  spi->h->Init.DataSize = SPI_DATASIZE_30BIT; break;
    case ARM_SPI_DATA_BITS(31U):  spi->h->Init.DataSize = SPI_DATASIZE_31BIT; break;
    case ARM_SPI_DATA_BITS(32U):  spi->h->Init.DataSize = SPI_DATASIZE_32BIT; break;
    default: return ARM_SPI_ERROR_DATA_BITS;
  }

  // Save Data item size [in Bytes]
  val = (control & ARM_SPI_DATA_BITS_Msk) >> ARM_SPI_DATA_BITS_Pos;
  if      (val > 16) { spi->xfer->dataSize = 4; }
  else if (val > 8)  { spi->xfer->dataSize = 2; }
  else               { spi->xfer->dataSize = 1; }


  // Bit order
  if ((control & ARM_SPI_BIT_ORDER_Msk) == ARM_SPI_LSB_MSB) {
    spi->h->Init.FirstBit = SPI_FIRSTBIT_LSB;
  } else {
    spi->h->Init.FirstBit = SPI_FIRSTBIT_MSB;
  }

  // Slave select master modes
  spi->h->Init.NSSPMode = SPI_NSS_PULSE_DISABLE;
  if (spi->h->Init.Mode == SPI_MODE_MASTER) {
    switch (control & ARM_SPI_SS_MASTER_MODE_Msk) {
      case ARM_SPI_SS_MASTER_UNUSED:
        spi->h->Init.NSS = SPI_NSS_SOFT;
        break;

      case ARM_SPI_SS_MASTER_HW_INPUT:
        spi->h->Init.NSS = SPI_NSS_HARD_INPUT;
        if (spi->nss) {
          // Configure NSS pin
          GPIO_InitStruct.Pin       = spi->nss->pin;
          GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
          GPIO_InitStruct.Pull      = GPIO_NOPULL;
          GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_LOW;
          GPIO_InitStruct.Alternate = spi->nss->af;
          recofigure_nss_pin = true;
        } else {
          // NSS pin is not available
          return ARM_SPI_ERROR_SS_MODE;
        }
        break;

      case ARM_SPI_SS_MASTER_SW:
        spi->h->Init.NSS      = SPI_NSS_SOFT;
        if (spi->nss) {
          // Configure NSS pin as GPIO output
          GPIO_InitStruct.Pin       = spi->nss->pin;
          GPIO_InitStruct.Mode      = GPIO_MODE_OUTPUT_PP;
          GPIO_InitStruct.Pull      = GPIO_NOPULL;
          GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_LOW;
          recofigure_nss_pin = true;
        } else {
          // NSS pin is not available
          return ARM_SPI_ERROR_SS_MODE;
        }
        break;

      case ARM_SPI_SS_MASTER_HW_OUTPUT:
        spi->h->Init.NSS      = SPI_NSS_HARD_OUTPUT;
        if (spi->nss) {
          // Configure NSS pin - SPI NSS alternative function
          GPIO_InitStruct.Pin       = spi->nss->pin;
          GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
          GPIO_InitStruct.Pull      = GPIO_NOPULL;
          GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_LOW;
          GPIO_InitStruct.Alternate = spi->nss->af;
          recofigure_nss_pin = true;
        } else {
          // NSS pin is not available
          return ARM_SPI_ERROR_SS_MODE;
        }
        break;
        default: return ARM_SPI_ERROR_SS_MODE;
    }
  }

  // Slave select slave modes
  if (spi->h->Init.Mode ==  SPI_MODE_SLAVE) {
    switch (control & ARM_SPI_SS_SLAVE_MODE_Msk) {
      case ARM_SPI_SS_SLAVE_HW:
        spi->h->Init.NSS = SPI_NSS_HARD_INPUT;
        if (spi->nss) {
          // Configure NSS pin - SPI NSS alternative function
          GPIO_InitStruct.Pin       = spi->nss->pin;
          GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
          GPIO_InitStruct.Pull      = GPIO_NOPULL;
          GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_LOW;
          GPIO_InitStruct.Alternate = spi->nss->af;
          recofigure_nss_pin = true;
        } else {
          // NSS pin is not available
          return ARM_SPI_ERROR_SS_MODE;
        }
        break;

      case ARM_SPI_SS_SLAVE_SW:
        spi->h->Init.NSS = SPI_NSS_SOFT;
        if (spi->nss) {
          // Unconfigure NSS pin
          HAL_GPIO_DeInit (spi->nss->port, spi->nss->pin);
        }
        break;
      default: return ARM_SPI_ERROR_SS_MODE;
    }
  }

  // Set SPI Bus Speed 
  clk = SPI_GetClk(spi);
  for (val = 0U; val < 8U; val++) {
    if (arg >= (clk >> (val + 1U))) break;
  }
  if ((val == 8U) || (arg < (clk >> (val + 1U)))) {
    // Requested Bus Speed can not be configured
    return ARM_DRIVER_ERROR;
  }
  // Save prescaler value
  switch (val) {
    case 0: spi->h->Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;   break;
    case 1: spi->h->Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;   break;
    case 2: spi->h->Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;   break;
    case 3: spi->h->Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;  break;
    case 4: spi->h->Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32;  break;
    case 5: spi->h->Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_64;  break;
    case 6: spi->h->Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_128; break;
    case 7: spi->h->Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256; break;
  }

  spi->h->Init.MasterKeepIOState = SPI_MASTER_KEEP_IO_STATE_ENABLE;
  if (HAL_SPI_Init (spi->h) != HAL_OK) {
    return ARM_DRIVER_ERROR;
  }

  // Reconfigure nss pin
  if (recofigure_nss_pin == true) {
    HAL_GPIO_Init(spi->nss->port, &GPIO_InitStruct);
  }
  // Reconfgure DMA
#ifdef __SPI_DMA_RX
  if (((spi->dma_use & SPI_DMA_USE_RX) != 0) && (spi->h->hdmarx != NULL)) {
    if ((control & ARM_SPI_DATA_BITS_Msk) > ARM_SPI_DATA_BITS(16U)) {
      spi->h->hdmarx->Init.MemDataAlignment    = DMA_MDATAALIGN_WORD;
      spi->h->hdmarx->Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    } else if ((control & ARM_SPI_DATA_BITS_Msk) > ARM_SPI_DATA_BITS(8U)) {
      spi->h->hdmarx->Init.MemDataAlignment    = DMA_MDATAALIGN_HALFWORD;
      spi->h->hdmarx->Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
    } else {
      spi->h->hdmarx->Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
      spi->h->hdmarx->Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    }
    HAL_DMA_Init(spi->h->hdmarx);
  }
#endif

#ifdef __SPI_DMA_TX
  if (((spi->dma_use & SPI_DMA_USE_TX) != 0) && (spi->h->hdmatx != NULL)) {
    if ((control & ARM_SPI_DATA_BITS_Msk) > ARM_SPI_DATA_BITS(16U)) {
      spi->h->hdmatx->Init.MemDataAlignment    = DMA_MDATAALIGN_WORD;
      spi->h->hdmatx->Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    } else if ((control & ARM_SPI_DATA_BITS_Msk) > ARM_SPI_DATA_BITS(8U)) {
      spi->h->hdmatx->Init.MemDataAlignment    = DMA_MDATAALIGN_HALFWORD;
      spi->h->hdmatx->Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
    } else {
      spi->h->hdmatx->Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
      spi->h->hdmatx->Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    }
    HAL_DMA_Init(spi->h->hdmatx);
  }
#endif

  spi->info->mode   = control;
  spi->info->state |= SPI_CONFIGURED;

  return ARM_DRIVER_OK;
}

/**
  \fn          ARM_SPI_STATUS SPI_GetStatus (const SPI_RESOURCES *spi)
  \brief       Get SPI status.
  \param[in]   spi  Pointer to SPI resources
  \return      SPI status \ref ARM_SPI_STATUS
*/
static ARM_SPI_STATUS SPI_GetStatus (const SPI_RESOURCES *spi) {
  ARM_SPI_STATUS status;
  uint32_t       error;

  error = HAL_SPI_GetError (spi->h);

  switch (HAL_SPI_GetState (spi->h)) {
    case HAL_SPI_STATE_ABORT:
    case HAL_SPI_STATE_BUSY:
    case HAL_SPI_STATE_BUSY_TX:
    case HAL_SPI_STATE_BUSY_RX:
    case HAL_SPI_STATE_BUSY_TX_RX:
      status.busy = 1;
      break;

    case HAL_SPI_STATE_RESET:
    case HAL_SPI_STATE_ERROR:
    case HAL_SPI_STATE_READY:
      status.busy = 0;
      break;
  }

  if (error & HAL_SPI_ERROR_OVR)  { status.data_lost  = 1; }
  else                            { status.data_lost  = 0; }
  if (error & HAL_SPI_ERROR_OVR)  { status.mode_fault = 1; }
  else                            { status.mode_fault = 0; }

  return status;
}

/**
  \fn          void SPI_TransferComplete (SPI_HandleTypeDef *hspi)
  \brief       Transfer Complete Callback
*/
static void SPI_TransferComplete (SPI_HandleTypeDef *hspi) {
  const SPI_RESOURCES * spi;
  spi = SPI_Resources (hspi);

#if (__DCACHE_PRESENT == 1U)
  if (spi->xfer->dma_flag != 0) {
    SCB_InvalidateDCache_by_Addr ((uint32_t *)((uint32_t)spi->xfer->rx_data), (int32_t)(spi->xfer->num * spi->xfer->dataSize));
  }
#endif

  spi->xfer->cnt = spi->xfer->num;

  if (spi->info->cb_event != NULL) {
    spi->info->cb_event(ARM_SPI_EVENT_TRANSFER_COMPLETE);
  }
}

/**
  * @brief Tx Transfer completed callback.
  * @param  hspi: pointer to a SPI_HandleTypeDef structure that contains
  *               the configuration information for SPI module.
  * @retval None
  */
void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi) {
  const SPI_RESOURCES * spi;
  spi = SPI_Resources (hspi);

  HAL_SPIEx_FlushRxFifo(spi->h);
  SPI_TransferComplete (hspi);
}

/**
  * @brief Rx Transfer completed callback.
  * @param  hspi: pointer to a SPI_HandleTypeDef structure that contains
  *               the configuration information for SPI module.
  * @retval None
  */
void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi) {
  SPI_TransferComplete (hspi);
}

/**
  * @brief Tx and Rx Transfer completed callback.
  * @param  hspi: pointer to a SPI_HandleTypeDef structure that contains
  *               the configuration information for SPI module.
  * @retval None
  */
void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi) {
  SPI_TransferComplete (hspi);
}

/**
  * @brief SPI error callback.
  * @param  hspi: pointer to a SPI_HandleTypeDef structure that contains
  *               the configuration information for SPI module.
  * @retval None
  */
void HAL_SPI_ErrorCallback(SPI_HandleTypeDef *hspi) {
  uint32_t  error, event;
  const SPI_RESOURCES * spi;

  spi = SPI_Resources (hspi);
  error = HAL_SPI_GetError (hspi);

  event = 0;
  if (error & HAL_SPI_ERROR_MODF) {
    event |= ARM_SPI_EVENT_MODE_FAULT;
  }
  if (error & HAL_SPI_ERROR_OVR) {
    event |= ARM_SPI_EVENT_DATA_LOST;
  }

  if ((spi->info->cb_event != NULL) && (event != 0)) {
    spi->info->cb_event(event);
  }
}

// SPI1
#ifdef MX_SPI1
SPIx_EXPORT_DRIVER(1);
#endif

// SPI2
#ifdef MX_SPI2
SPIx_EXPORT_DRIVER(2);
#endif

// SPI3
#ifdef MX_SPI3
SPIx_EXPORT_DRIVER(3);
#endif

// SPI4
#ifdef MX_SPI4
SPIx_EXPORT_DRIVER(4);
#endif

// SPI5
#ifdef MX_SPI5
SPIx_EXPORT_DRIVER(5);
#endif

// SPI6
#ifdef MX_SPI6
SPIx_EXPORT_DRIVER(6);
#endif
