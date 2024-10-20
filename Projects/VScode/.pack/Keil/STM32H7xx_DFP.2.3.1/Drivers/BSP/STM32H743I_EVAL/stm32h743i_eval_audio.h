/**
  ******************************************************************************
  * @file    stm32h743i_eval_audio.h
  * @author  MCD Application Team
  * @brief   This file contains the common defines and functions prototypes for
  *          the stm32h743i_eval_audio.c driver.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2017 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32H743I_EVAL_AUDIO_H
#define __STM32H743I_EVAL_AUDIO_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
/* Include audio component Driver */
#include "../Components/wm8994/wm8994.h"
#include "stm32h743i_eval.h"
#include <stdlib.h>
/* Include PDM to PCM lib header file */
#include "pdm2pcm_glo.h"

/** @addtogroup BSP
  * @{
  */

/** @defgroup STM32H743I_EVAL STM32H743I EVAL board
  * @{
  */

/** @defgroup STM32H743I_EVAL_AUDIO STM32H743I EVAL board BSP AUDIO
  * @{
  */

/** @defgroup STM32H743I_EVAL_AUDIO_Exported_Types AUDIO Exported Types
  * @{
  */
typedef struct
{
  uint32_t               Frequency;      /* Record Frequency */
  uint32_t               BitResolution;  /* Record bit resolution */
  uint32_t               ChannelNbr;     /* Record Channel Number */
  uint16_t               *pRecBuf;       /* Pointer to record user buffer */
  uint32_t               RecSize;        /* Size to record in mono, double size to record in stereo */
  uint32_t               InputDevice;    /* Audio Input Device */
  uint32_t               Interface;      /* Audio Input Interface */
  uint32_t               MultiBuffMode;  /* Multi buffer mode selection */
}AUDIOIN_ContextTypeDef;
/**
  * @}
  */

/** @defgroup STM32H743I_EVAL_AUDIO_Exported_Constants AUDIO Exported Constants
  * @{
  */

/** @defgroup BSP_Audio_Sample_Rate BSP Audio Sample Rate
  * @{
  */
#define BSP_AUDIO_FREQUENCY_96K         SAI_AUDIO_FREQUENCY_96K
#define BSP_AUDIO_FREQUENCY_48K         SAI_AUDIO_FREQUENCY_48K
#define BSP_AUDIO_FREQUENCY_44K         SAI_AUDIO_FREQUENCY_44K
#define BSP_AUDIO_FREQUENCY_32K         SAI_AUDIO_FREQUENCY_32K
#define BSP_AUDIO_FREQUENCY_22K         SAI_AUDIO_FREQUENCY_22K
#define BSP_AUDIO_FREQUENCY_16K         SAI_AUDIO_FREQUENCY_16K
#define BSP_AUDIO_FREQUENCY_11K         SAI_AUDIO_FREQUENCY_11K
#define BSP_AUDIO_FREQUENCY_8K          SAI_AUDIO_FREQUENCY_8K
/**
  * @}
  */

/*------------------------------------------------------------------------------
                          USER SAI defines parameters
 -----------------------------------------------------------------------------*/
/** @defgroup CODEC_AUDIO_FRAME_SLOT_TDM Code audio frame slot in TDM mode.
  * @{
  */
/** In W8994 codec the Audio frame contains 4 slots : TDM Mode
  * TDM format :
  * +------------------|------------------|--------------------|-------------------+
  * | CODEC_SLOT0 Left | CODEC_SLOT1 Left | CODEC_SLOT0 Right  | CODEC_SLOT1 Right |
  * +------------------------------------------------------------------------------+
  */
/* To have 2 separate audio stream in Both headphone and speaker the 4 slot must be activated */
#define CODEC_AUDIOFRAME_SLOT_0123                   SAI_SLOTACTIVE_0 | SAI_SLOTACTIVE_1 | SAI_SLOTACTIVE_2 | SAI_SLOTACTIVE_3

/* To have an audio stream in headphone only SAI Slot 0 and Slot 2 must be activated */
#define CODEC_AUDIOFRAME_SLOT_02                     SAI_SLOTACTIVE_0 | SAI_SLOTACTIVE_2
/* To have an audio stream in speaker only SAI Slot 1 and Slot 3 must be activated */
#define CODEC_AUDIOFRAME_SLOT_13                     SAI_SLOTACTIVE_1 | SAI_SLOTACTIVE_3
/* To have an audio stream in SAI PDM input Slot 0 must be activated */
#define CODEC_AUDIOFRAME_SLOT_0                      SAI_SLOTACTIVE_0
/**
  * @}
  */
/*------------------------------------------------------------------------------
                        AUDIO OUT CONFIGURATION
------------------------------------------------------------------------------*/
/* SAI peripheral configuration defines */
#define AUDIO_OUT_SAIx                           SAI1_Block_A
#define AUDIO_OUT_SAIx_CLK_ENABLE()              __HAL_RCC_SAI1_CLK_ENABLE()
#define AUDIO_OUT_SAIx_CLK_DISABLE()             __HAL_RCC_SAI1_CLK_DISABLE()
#define AUDIO_OUT_SAIx_AF                        GPIO_AF6_SAI1

#define AUDIO_OUT_SAIx_MCLK_ENABLE()             __HAL_RCC_GPIOE_CLK_ENABLE()
#define AUDIO_OUT_SAIx_MCLK_GPIO_PORT            GPIOE
#define AUDIO_OUT_SAIx_MCLK_PIN                  GPIO_PIN_2
#define AUDIO_OUT_SAIx_SD_FS_CLK_ENABLE()        __HAL_RCC_GPIOE_CLK_ENABLE()
#define AUDIO_OUT_SAIx_SD_FS_SCK_GPIO_PORT       GPIOE
#define AUDIO_OUT_SAIx_FS_PIN                    GPIO_PIN_4
#define AUDIO_OUT_SAIx_SCK_PIN                   GPIO_PIN_5
#define AUDIO_OUT_SAIx_SD_PIN                    GPIO_PIN_6

/* SAI DMA Stream definitions */
#define AUDIO_OUT_SAIx_DMAx_CLK_ENABLE()         __HAL_RCC_DMA2_CLK_ENABLE()
#define AUDIO_OUT_SAIx_DMAx_STREAM               DMA2_Stream1
#define AUDIO_OUT_SAIx_DMAx_REQUEST              DMA_REQUEST_SAI1_A
#define AUDIO_OUT_SAIx_DMAx_IRQ                  DMA2_Stream1_IRQn
#define AUDIO_OUT_SAIx_DMAx_PERIPH_DATA_SIZE     DMA_PDATAALIGN_HALFWORD
#define AUDIO_OUT_SAIx_DMAx_MEM_DATA_SIZE        DMA_MDATAALIGN_HALFWORD
#define AUDIO_OUT_SAIx_DMAx_IRQHandler           DMA2_Stream1_IRQHandler

/* Select the interrupt preemption priority and subpriority for the DMA interrupt */
#define AUDIO_OUT_IRQ_PREPRIO                    ((uint32_t)0x0E)

/*------------------------------------------------------------------------------
                        AUDIO IN CONFIGURATION
------------------------------------------------------------------------------*/
/* SAI peripheral configuration defines */
#define AUDIO_IN_SAIx                           SAI1_Block_B
#define AUDIO_IN_SAIx_CLK_ENABLE()              __HAL_RCC_SAI1_CLK_ENABLE()
#define AUDIO_IN_SAIx_CLK_DISABLE()             __HAL_RCC_SAI1_CLK_DISABLE()
#define AUDIO_IN_SAIx_AF                        GPIO_AF6_SAI1
#define AUDIO_IN_SAIx_SD_ENABLE()               __HAL_RCC_GPIOE_CLK_ENABLE()
#define AUDIO_IN_SAIx_SD_GPIO_PORT              GPIOE
#define AUDIO_IN_SAIx_SD_PIN                    GPIO_PIN_3

/* SAI DMA Stream definitions */
#define AUDIO_IN_SAIx_DMAx_CLK_ENABLE()         __HAL_RCC_DMA2_CLK_ENABLE()
#define AUDIO_IN_SAIx_DMAx_STREAM               DMA2_Stream4
#define AUDIO_IN_SAIx_DMAx_REQUEST              DMA_REQUEST_SAI1_B
#define AUDIO_IN_SAIx_DMAx_IRQ                  DMA2_Stream4_IRQn
#define AUDIO_IN_SAIx_DMAx_PERIPH_DATA_SIZE     DMA_PDATAALIGN_HALFWORD
#define AUDIO_IN_SAIx_DMAx_MEM_DATA_SIZE        DMA_MDATAALIGN_HALFWORD

#define AUDIO_IN_SAIx_DMAx_IRQHandler           DMA2_Stream4_IRQHandler

#define AUDIO_IN_INT_GPIO_ENABLE()               __HAL_RCC_GPIOJ_CLK_ENABLE()
#define AUDIO_IN_INT_GPIO_PORT                   GPIOJ
#define AUDIO_IN_INT_GPIO_PIN                    GPIO_PIN_12
#define AUDIO_IN_INT_IRQ                         EXTI15_10_IRQn

/* DFSDM Configuration defines */
#define AUDIO_DFSDMx_MIC1_CHANNEL                       DFSDM1_Channel1    /* MP34DT01TR microphone on PCB top side */
#define AUDIO_DFSDMx_MIC2_CHANNEL                       DFSDM1_Channel0    /* MP34DT01TR microphone on PCB top side */
#define AUDIO_DFSDMx_MIC1_CHANNEL_FOR_FILTER            DFSDM_CHANNEL_1
#define AUDIO_DFSDMx_MIC2_CHANNEL_FOR_FILTER            DFSDM_CHANNEL_0
#define AUDIO_DFSDMx_MIC1_FILTER                        DFSDM1_Filter0     /* Common filter for MP34DT01TR microphone input */
#define AUDIO_DFSDMx_MIC2_FILTER                        DFSDM1_Filter1     /* Common filter for MP34DT01TR microphone input */
#define AUDIO_DFSDMx_CLK_ENABLE()                       __HAL_RCC_DFSDM1_CLK_ENABLE()
#define AUDIO_DFSDMx_CKOUT_PIN                          GPIO_PIN_2
#define AUDIO_DFSDMx_CKOUT_DMIC_GPIO_PORT               GPIOC
#define AUDIO_DFSDMx_CKOUT_DMIC_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOC_CLK_ENABLE()
#define AUDIO_DFSDMx_CKOUT_DMIC_AF                      GPIO_AF6_DFSDM1
#define AUDIO_DFSDMx_DMIC_PIN                           GPIO_PIN_3
#define AUDIO_DFSDMx_DMIC_GPIO_PORT                     GPIOC
#define AUDIO_DFSDMx_DMIC_GPIO_CLK_ENABLE()             __HAL_RCC_GPIOC_CLK_ENABLE()
#define AUDIO_DFSDMx_DMIC_AF                            GPIO_AF3_DFSDM1

/* DFSDM DMA MIC1 and MIC2 channels definitions */
#define AUDIO_DFSDMx_DMAx_CLK_ENABLE()                  __HAL_RCC_DMA2_CLK_ENABLE()
#define AUDIO_DFSDMx_DMAx_MIC1_STREAM                   DMA2_Stream0
#define AUDIO_DFSDMx_DMAx_MIC1_REQUEST                  DMA_REQUEST_DFSDM1_FLT0
#define AUDIO_DFSDMx_DMAx_MIC2_STREAM                   DMA2_Stream5
#define AUDIO_DFSDMx_DMAx_MIC2_REQUEST                  DMA_REQUEST_DFSDM1_FLT1
#define AUDIO_DFSDMx_DMAx_MIC1_IRQ                      DMA2_Stream0_IRQn
#define AUDIO_DFSDMx_DMAx_MIC2_IRQ                      DMA2_Stream5_IRQn
#define AUDIO_DFSDMx_DMAx_PERIPH_DATA_SIZE              DMA_PDATAALIGN_WORD
#define AUDIO_DFSDMx_DMAx_MEM_DATA_SIZE                 DMA_MDATAALIGN_WORD

#define AUDIO_DFSDM_DMAx_MIC1_IRQHandler                DMA2_Stream0_IRQHandler
#define AUDIO_DFSDM_DMAx_MIC2_IRQHandler                DMA2_Stream5_IRQHandler

/* Select the interrupt preemption priority and subpriority for the DMA interrupt */
#define AUDIO_IN_IRQ_PREPRIO                ((uint32_t)0x0F)

/* SAI PDM input definitions */
#define AUDIO_IN_SAI_PDMx                       SAI4_Block_A
#define AUDIO_IN_SAI_PDMx_CLK_ENABLE()          __HAL_RCC_SAI4_CLK_ENABLE()
#define AUDIO_IN_SAI_PDMx_CLK_DISABLE()         __HAL_RCC_SAI4_CLK_DISABLE()
#define AUDIO_IN_SAI_PDMx_FS_SCK_AF             GPIO_AF8_SAI4
#define AUDIO_IN_SAI_PDMx_FS_SCK_ENABLE()       __HAL_RCC_GPIOE_CLK_ENABLE()
#define AUDIO_IN_SAI_PDMx_FS_SCK_GPIO_PORT      GPIOE
#define AUDIO_IN_SAI_PDMx_FS_PIN                GPIO_PIN_4
#define AUDIO_IN_SAI_PDMx_SCK_PIN               GPIO_PIN_5

#define AUDIO_IN_SAI_PDMx_CLK_IN_ENABLE()       __HAL_RCC_GPIOE_CLK_ENABLE()
#define AUDIO_IN_SAI_PDMx_CLK_IN_PIN            GPIO_PIN_2
#define AUDIO_IN_SAI_PDMx_CLK_IN_PORT           GPIOE
#define AUDIO_IN_SAI_PDMx_DATA_IN_ENABLE()      __HAL_RCC_GPIOC_CLK_ENABLE()
#define AUDIO_IN_SAI_PDMx_DATA_IN_PIN           GPIO_PIN_1
#define AUDIO_IN_SAI_PDMx_DATA_IN_PORT          GPIOC
#define AUDIO_IN_SAI_PDMx_DATA_CLK_AF           GPIO_AF10_SAI4
#define AUDIO_IN_SAI_PDMx_IRQHandler            SAI4_IRQHandler
#define AUDIO_IN_SAI_PDMx_IRQ                   SAI4_IRQn

/* SAI PDM DMA Stream definitions */
#define AUDIO_IN_SAI_PDMx_DMAx_CLK_ENABLE()         __HAL_RCC_BDMA_CLK_ENABLE()
#define AUDIO_IN_SAI_PDMx_DMAx_STREAM               BDMA_Channel1
#define AUDIO_IN_SAI_PDMx_DMAx_REQUEST              BDMA_REQUEST_SAI4_A
#define AUDIO_IN_SAI_PDMx_DMAx_IRQ                  BDMA_Channel1_IRQn
#define AUDIO_IN_SAI_PDMx_DMAx_PERIPH_DATA_SIZE     DMA_PDATAALIGN_HALFWORD
#define AUDIO_IN_SAI_PDMx_DMAx_MEM_DATA_SIZE        DMA_MDATAALIGN_HALFWORD
#define AUDIO_IN_SAI_PDMx_DMAx_IRQHandler           BDMA_Channel1_IRQHandler
/*------------------------------------------------------------------------------
             CONFIGURATION: Audio Driver Configuration parameters
------------------------------------------------------------------------------*/

#define AUDIODATA_SIZE                      ((uint32_t)2)   /* 16-bits audio data size */

/* Audio status definition */
#define AUDIO_OK                            ((uint8_t)0)
#define AUDIO_ERROR                         ((uint8_t)1)
#define AUDIO_TIMEOUT                       ((uint8_t)2)

/* Audio In default settings */
#define DEFAULT_AUDIO_IN_FREQ               BSP_AUDIO_FREQUENCY_16K
#define DEFAULT_AUDIO_IN_BIT_RESOLUTION     ((uint8_t)16)
#define DEFAULT_AUDIO_IN_CHANNEL_NBR        ((uint8_t)2)
#define DEFAULT_AUDIO_IN_VOLUME             ((uint16_t)64)

/*------------------------------------------------------------------------------
                            OUTPUT DEVICES definition
------------------------------------------------------------------------------*/
/* Alias on existing output devices to adapt for 2 headphones output */
#define OUTPUT_DEVICE_HEADPHONE1 OUTPUT_DEVICE_HEADPHONE
#define OUTPUT_DEVICE_HEADPHONE2 OUTPUT_DEVICE_SPEAKER /* Headphone2 is connected to Speaker output of the wm8994 */

/*------------------------------------------------------------------------------
                           INPUT DEVICES definition
------------------------------------------------------------------------------*/
/* Analog microphone input from 3.5 audio jack connector */
#define INPUT_DEVICE_ANALOG_MIC        ((uint32_t)0x00000001)
/* MP34DT01TR digital microphone on PCB top side */
#define INPUT_DEVICE_DIGITAL_MIC1      ((uint32_t)0x00000010)
#define INPUT_DEVICE_DIGITAL_MIC2      ((uint32_t)0x00000020)
#define INPUT_DEVICE_DIGITAL_MIC       ((uint32_t)(INPUT_DEVICE_DIGITAL_MIC1 | INPUT_DEVICE_DIGITAL_MIC2))
#define DFSDM_MIC_NUMBER               ((uint32_t)2)

/* Audio In interface for Digital mic */
#define AUDIO_IN_INTERFACE_SAI        ((uint16_t)0)
#define AUDIO_IN_INTERFACE_PDM        ((uint16_t)1)
#define AUDIO_IN_INTERFACE_DFSDM      ((uint16_t)2)

/**
  * @}
  */

/** @defgroup STM32H743I_EVAL_AUDIO_Exported_Macros AUDIO Exported Macros
  * @{
  */
#define DMA_MAX_SIZE         0xFFFF
#define DMA_MAX(x)           (((x) <= DMA_MAX_SIZE)? (x):DMA_MAX_SIZE)
#define POS_VAL(VAL)         (POSITION_VAL(VAL) - 4)
/**
  * @}
  */

/** @defgroup STM32H743I_EVAL_AUDIO_OUT_Exported_Functions AUDIO OUT Exported Functions
  * @{
  */
uint8_t BSP_AUDIO_OUT_Init(uint16_t OutputDevice, uint8_t Volume, uint32_t AudioFreq);
void    BSP_AUDIO_OUT_DeInit(void);
uint8_t BSP_AUDIO_OUT_Play(uint16_t* pBuffer, uint32_t Size);
void    BSP_AUDIO_OUT_ChangeBuffer(uint16_t *pData, uint16_t Size);
uint8_t BSP_AUDIO_OUT_Pause(void);
uint8_t BSP_AUDIO_OUT_Resume(void);
uint8_t BSP_AUDIO_OUT_Stop(uint32_t Option);
uint8_t BSP_AUDIO_OUT_SetVolume(uint8_t Volume);
void    BSP_AUDIO_OUT_SetFrequency(uint32_t AudioFreq);
void    BSP_AUDIO_OUT_SetAudioFrameSlot(uint32_t AudioFrameSlot);
uint8_t BSP_AUDIO_OUT_SetMute(uint32_t Cmd);
uint8_t BSP_AUDIO_OUT_SetOutputMode(uint8_t Output);

/* User Callbacks: user has to implement these functions in his code if they are needed. */
/* This function is called when the requested data has been completely transferred.*/
void    BSP_AUDIO_OUT_TransferComplete_CallBack(void);

/* This function is called when half of the requested buffer has been transferred. */
void    BSP_AUDIO_OUT_HalfTransfer_CallBack(void);

/* This function is called when an Interrupt due to transfer error on or peripheral
   error occurs. */
void    BSP_AUDIO_OUT_Error_CallBack(void);

/* These function can be modified in case the current settings (e.g. DMA stream)
   need to be changed for specific application needs */
void  BSP_AUDIO_OUT_ClockConfig(SAI_HandleTypeDef *hsai, uint32_t AudioFreq, void *Params);
void  BSP_AUDIO_OUT_MspInit(SAI_HandleTypeDef *hsai, void *Params);
void  BSP_AUDIO_OUT_MspDeInit(SAI_HandleTypeDef *hsai, void *Params);

/**
  * @}
  */

/** @defgroup STM32H743I_EVAL_AUDIO_IN_Exported_Functions AUDIO IN Exported Functions
  * @{
  */
uint8_t BSP_AUDIO_IN_Init(uint32_t AudioFreq, uint32_t BitRes, uint32_t ChnlNbr);
uint8_t BSP_AUDIO_IN_InitEx(uint16_t InputDevice, uint32_t AudioFreq, uint32_t BitRes, uint32_t ChnlNbr);
uint8_t BSP_AUDIO_IN_ConfigMicDefault(uint32_t InputDevice);
uint8_t BSP_AUDIO_IN_ConfigDigitalMic(uint32_t InputDevice, void *Params);
uint8_t BSP_AUDIO_IN_AllocScratch (int32_t *pScratch, uint32_t size);
uint8_t BSP_AUDIO_IN_GetChannelNumber(void);
uint8_t BSP_AUDIO_IN_Record(uint16_t *pBuf, uint32_t Size);
uint8_t BSP_AUDIO_IN_RecordEx(uint32_t *pBuf, uint32_t Size);
uint8_t BSP_AUDIO_IN_SetFrequency(uint32_t AudioFreq);
uint8_t BSP_AUDIO_IN_Stop(void);
uint8_t BSP_AUDIO_IN_StopEx(uint32_t InputDevice);
uint8_t BSP_AUDIO_IN_Pause(void);
uint8_t BSP_AUDIO_IN_PauseEx(uint32_t InputDevice);
uint8_t BSP_AUDIO_IN_Resume(void);
uint8_t BSP_AUDIO_IN_ResumeEx(uint32_t *pBuf, uint32_t InputDevice);
uint8_t BSP_AUDIO_IN_SetVolume(uint8_t Volume);
void    BSP_AUDIO_IN_DeInit(void);
uint8_t BSP_AUDIO_IN_PDMToPCM(uint16_t *PDMBuf, uint16_t *PCMBuf);
uint8_t BSP_AUDIO_IN_PDMToPCM_Init(uint32_t AudioFreq, uint32_t ChnlNbrIn, uint32_t ChnlNbrOut);
void    BSP_AUDIO_IN_SelectInterface(uint32_t Interface);
uint32_t BSP_AUDIO_IN_GetInterface(void);
uint8_t BSP_AUDIO_IN_OUT_Init(uint32_t InputDevice, uint32_t OutputDevice, uint32_t AudioFreq, uint32_t BitRes, uint32_t ChnlNbr);

/* User Callbacks: user has to implement these functions in his code if they are needed. */
/* This function should be implemented by the user application.
   It is called into this driver when the current buffer is filled to prepare the next
   buffer pointer and its size. */
void    BSP_AUDIO_IN_TransferComplete_CallBack(void);
void    BSP_AUDIO_IN_HalfTransfer_CallBack(void);
void    BSP_AUDIO_IN_TransferComplete_CallBackEx(uint32_t InputDevice);
void    BSP_AUDIO_IN_HalfTransfer_CallBackEx(uint32_t InputDevice);

/* This function is called when an Interrupt due to transfer error on or peripheral
   error occurs. */
void    BSP_AUDIO_IN_Error_CallBack(void);

/* These function can be modified in case the current settings (e.g. DMA stream)
   need to be changed for specific application needs */
void BSP_AUDIO_IN_ClockConfig(uint32_t AudioFreq, void *Params);
void BSP_AUDIO_IN_MspInit(void);
void BSP_AUDIO_IN_MspDeInit(void);

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

#ifdef __cplusplus
}
#endif

#endif /* __STM32H743I_EVAL_AUDIO_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
