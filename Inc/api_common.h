/*
 * api_common.h
 *
 *  Created on: Apr 15, 2023
 *      Author: Silve
 */

#ifndef INC_API_COMMON_H_
#define INC_API_COMMON_H_

#include "platform.h"
#include "debug_options.h"
#include "api_manualDrive.h"
#include "api_autonomous.h"

/* publicly available functions */
void StartUpChecks();
void SetPackage(uint8_t ArgNum, ...);
void PreparationsToDrive();
void Drive();

/* Macros */
#define ASMS_SWITCH_ON HAL_GPIO_ReadPin(GPIOB, ASMS_STATE_Pin)
#define GetLastTorque() GetVarInverter(1, 9)

/* MACROS */


#endif /* INC_API_COMMON_H_ */
