/*----------------------------------------------------------------------------
 * Name:    main.c
 * Purpose: DualCore LED Flasher (CM4 part)
 *----------------------------------------------------------------------------*/

#include "stm32h7xx.h"
#include "stm32h7xx_hal.h"
#include "stm32h747i_eval.h"
#include "common.h"


uint32_t hsemMsk   = 0xFFFF;
uint32_t hsemIdCur = (uint32_t)LED2;
uint32_t hsemIdPrv = (uint32_t)LED2;

int main(void)
{

  __HAL_RCC_HSEM_CLK_ENABLE();                             /* HW semaphore Clock enable */
  HAL_HSEM_ActivateNotification(HSEM_MSK_SYNC);            /* Activate HSEM notification for Cortex-M4 */

  /*
    Domain D2 goes to STOP mode (Cortex-M4 in deep-sleep)
    waiting for Cortex-M7 to perform system initialization (system clock config, ... )
  */
  HAL_PWREx_ClearPendingEvent();
  HAL_PWREx_EnterSTOPMode(PWR_MAINREGULATOR_ON, PWR_STOPENTRY_WFE, PWR_D2_DOMAIN);

  /* continue here after Sync event from CM7 */
  __HAL_HSEM_CLEAR_FLAG(HSEM_MSK_SYNC);                    /* Clear HSEM flag */

  HAL_Init();                                              /* HAL library initialization */

  BSP_LED_Init(LED2);
  BSP_LED_Init(LED3);
  BSP_LED_Init(LED4);

  HAL_HSEM_ActivateNotification(HSEM_MSK_LED2 |
                                HSEM_MSK_LED3 |
                                HSEM_MSK_LED4  );          /* Activate HSEM notification */
  HAL_NVIC_ClearPendingIRQ(HSEM2_IRQn);
//HAL_NVIC_SetPriority(HSEM2_IRQn, 10, 0);
  HAL_NVIC_EnableIRQ(HSEM2_IRQn);                          /* Enable HSEM Interrupt */

  while (1)
  {
    if (hsemMsk != 0xFFFF) {
      HAL_Delay(400);
      BSP_LED_Toggle((Led_TypeDef)hsemIdCur);
    }
  }
}

/*----------------------------------------------------------------------------
  HW Semaphore callback (called from HAL HSEM IRQ Handler)
 *----------------------------------------------------------------------------*/
void HAL_HSEM_FreeCallback(uint32_t SemMask)
{
  hsemMsk = SemMask;
  hsemIdCur = 31U - __CLZ(SemMask);                        /* Calculate used HW semaphore ID */
  HAL_HSEM_ActivateNotification(hsemMsk);                  /* Activate HSEM notification again */

  BSP_LED_Off((Led_TypeDef)hsemIdPrv);                     /* Switch off prvious LED */
  hsemIdPrv = hsemIdCur;
}


/*----------------------------------------------------------------------------
  SysTick IRQ Handler (used from HAL)
 *----------------------------------------------------------------------------*/
void SysTick_Handler (void)
{
  HAL_IncTick();
}

/*----------------------------------------------------------------------------
  HW Semaphore CPU2 IRQ Handler
 *----------------------------------------------------------------------------*/
void HSEM2_IRQHandler(void)
{
  HAL_HSEM_IRQHandler();
}
