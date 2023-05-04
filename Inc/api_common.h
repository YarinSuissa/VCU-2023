/*
 * api_common.h
 *
 *  Created on: Apr 15, 2023
 *      Author: Silve
 */

#ifndef INC_API_COMMON_H_
#define INC_API_COMMON_H_

#include "platform.h"
#include "api_manualDrive.h"

/* publicly available functions */
void StartUpChecks();
void SetPackage(uint8_t ArgNum, ...);

/* Known values and constants */
#define REQUIREMENT_MIN_Vout		   350
#define REQUIREMENT_MIN_BRAKE_PRESSURE 300

/* Macros */
#define ASMS_SWITCH_ON HAL_GPIO_ReadPin(GPIOB, ASMS_STATE_Pin)
#define GetLastTorque() GetVarInverter(1, 9)

/* MACROS */


#endif /* INC_API_COMMON_H_ */
