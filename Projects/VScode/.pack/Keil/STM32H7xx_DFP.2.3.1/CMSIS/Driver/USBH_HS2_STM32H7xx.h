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
 * $Date:        11. August 2017
 * $Revision:    V1.0
 *
 * Project:      USB Full/Low-Speed Host 1 Driver Header File for ST 
 *               STM32H7xx
 * -------------------------------------------------------------------------- */

#ifndef __USBH1_STM32H7XX_H
#define __USBH1_STM32H7XX_H

#include <stdint.h>

#include "Driver_USBH.h"

// Global functions and variables exported by driver .c module
extern void USBH_HS2_IRQ (uint32_t gintsts);
extern ARM_DRIVER_USBH Driver_USBH1;

#endif /* __USBH1_STM32H7XX_H */
