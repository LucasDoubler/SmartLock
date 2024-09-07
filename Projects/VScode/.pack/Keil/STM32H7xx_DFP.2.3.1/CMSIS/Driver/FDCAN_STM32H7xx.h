/* -----------------------------------------------------------------------------
 * Copyright (c) 2017 ARM Ltd.
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
 * $Date:        22. September 2017
 * $Revision:    V1.0
 *
 * Project:      FDCAN 1/2 Driver Header File for ST STM32H7xx
 * --------------------------------------------------------------------------*/

#ifndef __FDCAN_STM32H7XX_H
#define __FDCAN_STM32H7XX_H

#include <stdint.h>

#include "Driver_CAN.h"

#include "MX_Device.h"

// Global functions and variables exported by driver .c module

#if (defined(MX_FDCAN1) && (MX_FDCAN1 == 1))
extern void FDCAN1_IT0_IRQHandler (void);
extern ARM_DRIVER_CAN Driver_CAN1;
#endif

#if (defined(MX_FDCAN2) && (MX_FDCAN2 == 1))
extern void FDCAN2_IT0_IRQHandler (void);
extern ARM_DRIVER_CAN Driver_CAN2;
#endif

#endif /* __FDCAN_STM32H7XX_H */
