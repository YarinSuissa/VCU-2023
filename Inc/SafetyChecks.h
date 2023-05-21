/*
 * SafetyChecks.h
 *
 *  Created on: May 20, 2023
 *      Author: Silve
 */

#ifndef INC_SAFETYCHECKS_H_
#define INC_SAFETYCHECKS_H_

#include "platform.h"

/* Functions */
void Safety_Buzzer();
_Bool Driver_R2D_Pressed();
_Bool Driver_Minimum_Brake_Pressure();
_Bool Driver_Gas_Pedal_In_Range();
_Bool Driver_Gas_Pedal_Calibrated();
_Bool Inverter_Minimum_Voltage();
_Bool ShortCircuitIdentifier(uint16_t Gas_Value, uint16_t Brake_Pressure);



/* Known values and constants */
#define REQUIREMENT_MIN_Vout		   350
#define REQUIREMENT_MIN_BRAKE_PRESSURE 300

/* Macros */
#define BUZZER(X)		HAL_GPIO_WritePin(BUZZER_EN_GPIO_Port, BUZZER_EN_Pin, X)

#endif /* INC_SAFETYCHECKS_H_ */
