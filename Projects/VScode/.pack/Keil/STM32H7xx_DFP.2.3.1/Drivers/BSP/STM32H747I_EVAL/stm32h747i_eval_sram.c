/**
  ******************************************************************************
  * @file    stm32h747i_eval_sram.c
  * @author  MCD Application Team
  * @brief   This file includes the SRAM driver for the IS61WV102416BLL-10M memory
  *          device mounted on STM32H747I-EVAL evaluation boards.
  @verbatim
  How To use this driver:
  -----------------------
   - This driver is used to drive the IS61WV102416BLL-10M SRAM external memory mounted
     on STM32H747I-EVAL evaluation board.
   - This driver does not need a specific component driver for the SRAM device
     to be included with.

  Driver description:
  ------------------
  + Initialization steps:
     o Initialize the SRAM external memory using the BSP_SRAM_Init() function. This
       function includes the MSP layer hardware resources initialization and the
       FMC controller configuration to interface with the external SRAM memory.

  + SRAM read/write operations
     o SRAM external memory can be accessed with read/write operations once it is
       initialized.
       Read/write operation can be performed with AHB access using the functions
       BSP_SRAM_ReadData()/BSP_SRAM_WriteData(), or by DMA transfer using the functions
       BSP_SRAM_ReadData_DMA()/BSP_SRAM_WriteData_DMA().
     o The AHB access is performed with 16-bit width transaction, the DMA transfer
       configuration is fixed at single (no burst) halfword transfer
       (see the SRAM_MspInit() static function).
     o User can implement his own functions for read/write access with his desired
       configurations.
     o If interrupt mode is used for DMA transfer, the function BSP_SRAM_MDMA_IRQHandler()
       is called in IRQ handler file, to serve the generated interrupt once the DMA
       transfer is complete.
  @endverbatim
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32h747i_eval_sram.h"

/** @addtogroup BSP
  * @{
  */

/** @addtogroup STM32H747I_EVAL
  * @{
  */

/** @addtogroup STM32H747I_EVAL_SRAM
  * @{
  */

/** @defgroup STM32H747I_EVAL_SRAM_Private_Types_Definitions Private Types Definitions
  * @{
  */
/**
  * @}
  */

/** @defgroup STM32H747I_EVAL_SRAM_Private_Defines Private Defines
  * @{
  */
/**
  * @}
  */

/** @defgroup STM32H747I_EVAL_SRAM_Private_Macros Private Macros
  * @{
  */
/**
  * @}
  */
  
/** @defgroup STM32H747I_EVAL_SRAM_Exported_Variables Exported Variables
  * @{
  */
SRAM_HandleTypeDef sramHandle;
/**
  * @}
  */
  
/** @defgroup STM32H747I_EVAL_SRAM_Private_Variables Private Variables
  * @{
  */
static FMC_NORSRAM_TimingTypeDef Timing;
/**
  * @}
  */

/** @defgroup STM32H747I_EVAL_SRAM_Private_Function_Prototypes Private Function Prototypes
  * @{
  */
/**
  * @}
  */

/** @addtogroup STM32H747I_EVAL_SRAM_Exported_Functions
  * @{
  */

/**
  * @brief  Initializes the SRAM device.
  * @retval SRAM status
  */
uint8_t BSP_SRAM_Init(void)
{
  static uint8_t sram_status = SRAM_ERROR;
  /* SRAM device configuration */
  sramHandle.Instance = FMC_NORSRAM_DEVICE;
  sramHandle.Extended = FMC_NORSRAM_EXTENDED_DEVICE;

  /* SRAM device configuration */
  /* Timing configuration derived from system clock (up to 400Mhz)
     for 100Mhz as SRAM clock frequency */
  Timing.AddressSetupTime      = 2;
  Timing.AddressHoldTime       = 1;
  Timing.DataSetupTime         = 2;
  Timing.BusTurnAroundDuration = 1;
  Timing.CLKDivision           = 2;
  Timing.DataLatency           = 2;
  Timing.AccessMode            = FMC_ACCESS_MODE_A;

  sramHandle.Init.NSBank             = FMC_NORSRAM_BANK3;
  sramHandle.Init.DataAddressMux     = FMC_DATA_ADDRESS_MUX_DISABLE;
  sramHandle.Init.MemoryType         = FMC_MEMORY_TYPE_SRAM;
  sramHandle.Init.MemoryDataWidth    = SRAM_MEMORY_WIDTH;
  sramHandle.Init.BurstAccessMode    = SRAM_BURSTACCESS;
  sramHandle.Init.WaitSignalPolarity = FMC_WAIT_SIGNAL_POLARITY_LOW;
  sramHandle.Init.WaitSignalActive   = FMC_WAIT_TIMING_BEFORE_WS;
  sramHandle.Init.WriteOperation     = FMC_WRITE_OPERATION_ENABLE;
  sramHandle.Init.WaitSignal         = FMC_WAIT_SIGNAL_DISABLE;
  sramHandle.Init.ExtendedMode       = FMC_EXTENDED_MODE_DISABLE;
  sramHandle.Init.AsynchronousWait   = FMC_ASYNCHRONOUS_WAIT_DISABLE;
  sramHandle.Init.WriteBurst         = SRAM_WRITEBURST;
  sramHandle.Init.ContinuousClock    = CONTINUOUSCLOCK_FEATURE;
  sramHandle.Init.WriteFifo          = FMC_WRITE_FIFO_DISABLE;
  sramHandle.Init.PageSize           = FMC_PAGE_SIZE_NONE;

  /* SRAM controller initialization */
  BSP_SRAM_MspInit(&sramHandle, NULL); /* __weak function can be rewritten by the application */
  if(HAL_SRAM_Init(&sramHandle, &Timing, &Timing) != HAL_OK)
  {
    sram_status = SRAM_ERROR;
  }
  else
  {
    sram_status = SRAM_OK;
  }
  return sram_status;
}

/**
  * @brief  DeInitializes the SRAM device.
  * @retval SRAM status
  */
uint8_t BSP_SRAM_DeInit(void)
{
  static uint8_t sram_status = SRAM_ERROR;
  /* SRAM device de-initialization */
  sramHandle.Instance = FMC_NORSRAM_DEVICE;
  sramHandle.Extended = FMC_NORSRAM_EXTENDED_DEVICE;

  if(HAL_SRAM_DeInit(&sramHandle) != HAL_OK)
  {
    sram_status = SRAM_ERROR;
  }
  else
  {
    sram_status = SRAM_OK;
  }

  /* SRAM controller de-initialization */
  BSP_SRAM_MspDeInit(&sramHandle, NULL);

  return sram_status;
}

/**
  * @brief  Reads an amount of data from the SRAM device in polling mode.
  * @param  uwStartAddress: Read start address
  * @param  pData: Pointer to data to be read
  * @param  uwDataSize: Size of read data from the memory
  * @retval SRAM status
  */
uint8_t BSP_SRAM_ReadData(uint32_t uwStartAddress, uint16_t *pData, uint32_t uwDataSize)
{
  if(HAL_SRAM_Read_16b(&sramHandle, (uint32_t *)uwStartAddress, pData, uwDataSize) != HAL_OK)
  {
    return SRAM_ERROR;
  }
  else
  {
    return SRAM_OK;
  }
}

/**
  * @brief  Reads an amount of data from the SRAM device in DMA mode.
  * @param  uwStartAddress: Read start address
  * @param  pData: Pointer to data to be read
  * @param  uwDataSize: Size of read data from the memory
  * @retval SRAM status
  */
uint8_t BSP_SRAM_ReadData_DMA(uint32_t uwStartAddress, uint16_t *pData, uint32_t uwDataSize)
{
  if(HAL_SRAM_Read_DMA(&sramHandle, (uint32_t *)uwStartAddress, (uint32_t *)pData, (uint32_t)(uwDataSize/2)) != HAL_OK)
  {
    return SRAM_ERROR;
  }
  else
  {
    return SRAM_OK;
  }
}

/**
  * @brief  Writes an amount of data from the SRAM device in polling mode.
  * @param  uwStartAddress: Write start address
  * @param  pData: Pointer to data to be written
  * @param  uwDataSize: Size of written data from the memory
  * @retval SRAM status
  */
uint8_t BSP_SRAM_WriteData(uint32_t uwStartAddress, uint16_t *pData, uint32_t uwDataSize)
{
  if(HAL_SRAM_Write_16b(&sramHandle, (uint32_t *)uwStartAddress, pData, uwDataSize) != HAL_OK)
  {
    return SRAM_ERROR;
  }
  else
  {
    return SRAM_OK;
  }
}

/**
  * @brief  Writes an amount of data from the SRAM device in DMA mode.
  * @param  uwStartAddress: Write start address
  * @param  pData: Pointer to data to be written
  * @param  uwDataSize: Size of written data from the memory
  * @retval SRAM status
  */
uint8_t BSP_SRAM_WriteData_DMA(uint32_t uwStartAddress, uint16_t *pData, uint32_t uwDataSize)
{
  if(HAL_SRAM_Write_DMA(&sramHandle, (uint32_t *)uwStartAddress, (uint32_t *)pData, (uint32_t)(uwDataSize/2)) != HAL_OK)
  {
    return SRAM_ERROR;
  }
  else
  {
    return SRAM_OK;
  }
}

/**
  * @brief  Initializes SRAM MSP.
  * @param  hsram: SRAM handle
  * @param  Params: Pointer to void
  * @retval None
  */
__weak void BSP_SRAM_MspInit(SRAM_HandleTypeDef  *hsram, void *Params)
{
  static MDMA_HandleTypeDef mdma_handle;
  GPIO_InitTypeDef gpio_init_structure;

  /* Enable FMC clock */
  __HAL_RCC_FMC_CLK_ENABLE();

  /* Enable chosen DMAx clock */
  __SRAM_MDMAx_CLK_ENABLE();

  /* Enable GPIOs clock */
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();

  /* Common GPIO configuration */
  gpio_init_structure.Mode      = GPIO_MODE_AF_PP;
  gpio_init_structure.Pull      = GPIO_PULLUP;
  gpio_init_structure.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
  gpio_init_structure.Alternate = GPIO_AF12_FMC;

  /* GPIOD configuration */
  gpio_init_structure.Pin   = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_3| GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 |\
                              GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 |\
                              GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
  HAL_GPIO_Init(GPIOD, &gpio_init_structure);

  /* GPIOE configuration */
  gpio_init_structure.Pin   = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_3| GPIO_PIN_4 | GPIO_PIN_7     |\
                              GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 |\
                              GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
  HAL_GPIO_Init(GPIOE, &gpio_init_structure);

  /* GPIOF configuration */
  gpio_init_structure.Pin   = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2| GPIO_PIN_3 | GPIO_PIN_4     |\
                              GPIO_PIN_5 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;

  HAL_GPIO_Init(GPIOF, &gpio_init_structure);

  /* GPIOG configuration */
  gpio_init_structure.Pin   = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2| GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 |\
                              GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14;
  HAL_GPIO_Init(GPIOG, &gpio_init_structure);


  /* Configure common MDMA parameters */
  mdma_handle.Init.Request = MDMA_REQUEST_SW;
  mdma_handle.Init.TransferTriggerMode = MDMA_BLOCK_TRANSFER;
  mdma_handle.Init.Priority = MDMA_PRIORITY_HIGH;
  mdma_handle.Init.Endianness = MDMA_LITTLE_ENDIANNESS_PRESERVE;
  mdma_handle.Init.SourceInc = MDMA_SRC_INC_WORD;
  mdma_handle.Init.DestinationInc = MDMA_DEST_INC_WORD;
  mdma_handle.Init.SourceDataSize = MDMA_SRC_DATASIZE_WORD;
  mdma_handle.Init.DestDataSize = MDMA_DEST_DATASIZE_WORD;
  mdma_handle.Init.DataAlignment = MDMA_DATAALIGN_PACKENABLE;
  mdma_handle.Init.SourceBurst = MDMA_SOURCE_BURST_SINGLE;
  mdma_handle.Init.DestBurst = MDMA_DEST_BURST_SINGLE;
  mdma_handle.Init.BufferTransferLength = 128;
  mdma_handle.Init.SourceBlockAddressOffset = 0;
  mdma_handle.Init.DestBlockAddressOffset = 0;

  mdma_handle.Instance = SRAM_MDMAx_CHANNEL;

   /* Associate the DMA handle */
  __HAL_LINKDMA(hsram, hmdma, mdma_handle);

  /* Deinitialize the Stream for new transfer */
  HAL_MDMA_DeInit(&mdma_handle);

  /* Configure the DMA Stream */
  HAL_MDMA_Init(&mdma_handle);

  /* NVIC configuration for DMA transfer complete interrupt */
  HAL_NVIC_SetPriority(SRAM_MDMAx_IRQn, 0x0F, 0);
  HAL_NVIC_EnableIRQ(SRAM_MDMAx_IRQn);
}


/**
  * @brief  DeInitializes SRAM MSP.
  * @param  hsram: SRAM handle
  * @param  Params: Pointer to void  
  * @retval None
  */
__weak void BSP_SRAM_MspDeInit(SRAM_HandleTypeDef  *hsram, void *Params)
{
    static MDMA_HandleTypeDef mdma_handle;

    /* Disable NVIC configuration for DMA interrupt */
    HAL_NVIC_DisableIRQ(SRAM_MDMAx_IRQn);

    /* Deinitialize the stream for new transfer */
    mdma_handle.Instance = SRAM_MDMAx_CHANNEL;
    HAL_MDMA_DeInit(&mdma_handle);

    /* GPIO pins clock, FMC clock and DMA clock can be shut down in the applications
       by surcharging this __weak function */
}

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
