/*----------------------------------------------------------------------------
 * Name:    main.c
 * Purpose: DualCore LED Flasher (CM7 part)
 *----------------------------------------------------------------------------*/

#include "stm32h7xx.h"
#include "stm32h7xx_hal.h"
#include "stm32h747i_eval.h"
#include "common.h"


static void CPU_CACHE_Enable(void);
static void SystemClock_Config(void);
static void Error_Handler(void);


/*----------------------------------------------------------------------------
  main loop
 *----------------------------------------------------------------------------*/
int main (void)
{
  uint32_t hsemId = LED2;                                  /* HW semaphore to use */

  CPU_CACHE_Enable();                                      /* Enable the CPU Cache */

  HAL_RCCEx_EnableBootCore(RCC_BOOT_C2);                   /* Enable CPU2 (Cortex-M4) boot regardless of option byte values */

  SystemClock_Config();                                    /* Configure the system clock to 400 MHz (CM7), 200 HMz (CM4)*/

  HAL_Init();                                              /* HAL library initialization */

  __HAL_RCC_HSEM_CLK_ENABLE();                             /* HW semaphore clock enable */
  HAL_HSEM_FastTake(HSEM_ID_SYNC);                         /* Take HSEM */
  HAL_HSEM_Release(HSEM_ID_SYNC, 0);                       /* Release HSEM in order to notify the CPU2 (CM4) */


  BSP_LED_Init(LED1);
  BSP_LED_On(LED1);
  BSP_PB_Init(BUTTON_TAMPER, BUTTON_MODE_GPIO);

  while (1)
  {
    HAL_Delay(100);
    BSP_LED_Toggle(LED1);

    if (BSP_PB_GetState(BUTTON_TAMPER) == RESET)
    {
      HAL_Delay(10);
      while (BSP_PB_GetState(BUTTON_TAMPER) == RESET);

      if (hsemId > LED4)
      {
        hsemId = LED2;
      }

      HAL_HSEM_FastTake(hsemId);                          /* Take HSEM */
      HAL_HSEM_Release(hsemId, 0);                        /* Release HSEM in order to notify the CPU2 (CM4)*/
      hsemId += 1;
    }
  }

}


/*----------------------------------------------------------------------------
   System Clock Configuration (based on ST example)
   The system Clock is configured as follow :
      System Clock source            = PLL (HSE)
      SYSCLK(Hz)                     = 400000000 (Cortex-M7 CPU Clock)
      HCLK(Hz)                       = 200000000 (Cortex-M4 CPU, Bus matrix Clocks)
      AHB Prescaler                  = 2
      D1 APB3 Prescaler              = 2 (APB3 Clock  100MHz)
      D2 APB1 Prescaler              = 2 (APB1 Clock  100MHz)
      D2 APB2 Prescaler              = 2 (APB2 Clock  100MHz)
      D3 APB4 Prescaler              = 2 (APB4 Clock  100MHz)
      HSE Frequency(Hz)              = 25000000
      PLL_M                          = 5
      PLL_N                          = 160
      PLL_P                          = 2
      PLL_Q                          = 4
      PLL_R                          = 2
      VDD(V)                         = 3.3
      Flash Latency(WS)              = 4
 *----------------------------------------------------------------------------*/
static void SystemClock_Config (void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;
  HAL_StatusTypeDef ret = HAL_OK;

  /* Supply configuration update enable */
  HAL_PWREx_ConfigSupply(PWR_DIRECT_SMPS_SUPPLY);

  /* The voltage scaling allows optimizing the power consumption when the device is
     clocked below the maximum system frequency, to update the voltage scaling value
     regarding system frequency refer to product datasheet.  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

  /* Enable HSE Oscillator and activate PLL with HSE as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_OFF;
  RCC_OscInitStruct.CSIState = RCC_CSI_OFF;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;

  RCC_OscInitStruct.PLL.PLLM = 5;
  RCC_OscInitStruct.PLL.PLLN = 160;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLQ = 4;

  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_2;
  ret = HAL_RCC_OscConfig(&RCC_OscInitStruct);
  if (ret != HAL_OK)
  {
    Error_Handler();
  }

  /* Select PLL as system clock source and configure  bus clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK  | RCC_CLOCKTYPE_HCLK    |
                                 RCC_CLOCKTYPE_D1PCLK1 | RCC_CLOCKTYPE_D3PCLK1 |
                                 RCC_CLOCKTYPE_PCLK1   | RCC_CLOCKTYPE_PCLK2    );

  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;
  ret = HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4);
  if (ret != HAL_OK)
  {
    Error_Handler();
  }

  /*
  Note : The activation of the I/O Compensation Cell is recommended with communication  interfaces
          (GPIO, SPI, FMC, QSPI ...)  when  operating at  high frequencies(please refer to product datasheet)
          The I/O Compensation Cell activation  procedure requires :
        - The activation of the CSI clock
        - The activation of the SYSCFG clock
        - Enabling the I/O Compensation Cell : setting bit[0] of register SYSCFG_CCCSR

          To do this please uncomment the following code
  */

  /*
  __HAL_RCC_CSI_ENABLE() ;

  __HAL_RCC_SYSCFG_CLK_ENABLE() ;

  HAL_EnableCompensationCell();
  */
}

/*----------------------------------------------------------------------------
  CPU cache enable function
 *----------------------------------------------------------------------------*/
static void CPU_CACHE_Enable (void)
{
  SCB_EnableICache();
  SCB_EnableDCache();
}

/*----------------------------------------------------------------------------
  Error Handler function
 *----------------------------------------------------------------------------*/
static void Error_Handler (void)
{
  while(1)
  {
    __NOP();                                               /* stay here */
  }
}


/*----------------------------------------------------------------------------
  SysTick IRQ Handler (used from HAL)
 *----------------------------------------------------------------------------*/
void SysTick_Handler (void)
{
  HAL_IncTick();
}

