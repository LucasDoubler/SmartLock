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
 * Project:      OTG Driver Header for ST STM32H7xx
 * -------------------------------------------------------------------------- */

#ifndef __OTG_STM32H7XX_H
#define __OTG_STM32H7XX_H

#include <stdint.h>

#include "MX_Device.h"

// OTG_FS (OTG_HS2)
#if  defined(MX_USB_OTG_FS)
  #define OTG_FS_USED             1
#else
  #define OTG_FS_USED             0
#endif

#if (defined(MX_USB_OTG_FS_DEVICE) || (defined(MX_USB_OTG_FS) && !defined(MX_USB_OTG_FS_HOST)))
  #define OTG_FS_DEVICE_USED      1
#else
  #define OTG_FS_DEVICE_USED      0
#endif

#if (defined(MX_USB_OTG_FS_HOST)   || (defined(MX_USB_OTG_FS) && !defined(MX_USB_OTG_FS_DEVICE)))
  #define OTG_FS_HOST_USED        1
#else
  #define OTG_FS_HOST_USED        0
#endif

// OTG_HS (OTG_HS1)
#if  defined(MX_USB_OTG_HS)
  #define OTG_HS_USED             1
#else
  #define OTG_HS_USED             0
#endif
#if  defined(MX_USB_OTG_HS_ULPI_D7_Pin)
  #define OTG_HS_ULPI_USED        1
#else
  #define OTG_HS_ULPI_USED        0
#endif

#if (defined(MX_USB_OTG_HS_DEVICE) || (defined(MX_USB_OTG_HS) && !defined(MX_USB_OTG_HS_HOST)))
  #define OTG_HS_DEVICE_USED      1
#else
  #define OTG_HS_DEVICE_USED      0
#endif

#if (defined(MX_USB_OTG_HS_HOST)   || (defined(MX_USB_OTG_HS) && !defined(MX_USB_OTG_HS_DEVICE)))
  #define OTG_HS_HOST_USED        1
#else
  #define OTG_HS_HOST_USED        0
#endif

// Externally defined variables and functions
extern uint8_t otg_fs_role;
extern uint8_t otg_hs_role;

#endif /* __OTG_STM32H7XX_H */
