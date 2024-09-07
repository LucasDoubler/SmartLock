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
 * $Date:        17. August 2017
 * $Revision:    V1.0
 *
 * Project:      OTG Common Driver for ST STM32H7xx
 * -------------------------------------------------------------------------- */

/* History:
 *  Version 1.0
 *    Initial release
 */

#include "OTG_STM32H7xx.h"

#include "Driver_USBH.h"
#include "Driver_USBD.h"

#include "stm32h7xx_hal.h"
#include "stm32h7xx_hal_pcd.h"

#include "USBH_HS1_STM32H7xx.h"
#include "USBH_HS2_STM32H7xx.h"


#if   (OTG_FS_DEVICE_USED == 1) || (OTG_HS_DEVICE_USED == 1)
extern void HAL_PCD_IRQHandler(PCD_HandleTypeDef *hpcd);
#endif

#if   (OTG_FS_DEVICE_USED == 1)
extern PCD_HandleTypeDef hpcd_USB_OTG_FS;
#endif
#if   (OTG_HS_DEVICE_USED == 1)
extern PCD_HandleTypeDef hpcd_USB_OTG_HS;
#endif

#if   (OTG_FS_USED == 1)
extern void OTG_FS_IRQHandler (void);
#endif

#if   (OTG_HS_USED == 1)
extern void OTG_HS_IRQHandler (void);
#endif


// Global variables
uint8_t otg_fs_role = ARM_USB_ROLE_NONE;
uint8_t otg_hs_role = ARM_USB_ROLE_NONE;

// OTG IRQ routine (common for Device and Host) ********************************

#if (OTG_FS_USED == 1)
/**
  \fn          void OTG_FS_IRQHandler (void)
  \brief       USB Interrupt Routine (IRQ).
*/
void OTG_FS_IRQHandler (void) {
#if (OTG_FS_DEVICE_USED == 1) && (OTG_FS_HOST_USED == 1)
  switch (otg_fs_role) {
    case ARM_USB_ROLE_HOST:
      USBH_HS2_IRQ (USB_OTG_FS->GINTSTS & USB_OTG_FS->GINTMSK);
      break;
    case ARM_USB_ROLE_DEVICE:
      HAL_PCD_IRQHandler(&hpcd_USB_OTG_FS);
      break;
    default:
      break;
  }
#elif (OTG_FS_HOST_USED == 1)
  USBH_HS2_IRQ (USB_OTG_FS->GINTSTS & USB_OTG_FS->GINTMSK);
#else
  HAL_PCD_IRQHandler(&hpcd_USB_OTG_FS);
#endif
}
#endif

#if (OTG_HS_USED == 1)
/**
  \fn          void OTG_HS_IRQHandler (void)
  \brief       USB Interrupt Routine (IRQ).
*/
void OTG_HS_IRQHandler (void) {
#if (OTG_HS_DEVICE_USED == 1) && (OTG_HS_HOST_USED == 1)
  switch (otg_hs_role) {
    case ARM_USB_ROLE_HOST:
      USBH_HS1_IRQ (USB_OTG_HS->GINTSTS & USB_OTG_HS->GINTMSK);
      break;
    case ARM_USB_ROLE_DEVICE:
      HAL_PCD_IRQHandler(&hpcd_USB_OTG_HS);
      break;
    default:
      break;
  }
#elif (OTG_HS_HOST_USED == 1)
  USBH_HS1_IRQ (USB_OTG_HS->GINTSTS & USB_OTG_HS->GINTMSK);
#else
  HAL_PCD_IRQHandler(&hpcd_USB_OTG_HS);
#endif
}
#endif
