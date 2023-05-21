/*
 * api_motors.h
 *
 *  Created on: 11 May 2023
 *      Author: Silve
 */

#ifndef INC_API_MOTORS_H_
#define INC_API_MOTORS_H_

#include "platform.h"


/* Functions */
void  KeepAliveMsgMotors();
void  SetInverterTorque(double DesiredTorque);
void  TimeOutMotors();
void  EnableMotors();
void  DisableMotors();
void  GetDataFromMotors(uint8_t dest_reg, uint8_t time_interval);
_Bool Request_Motors_Routine(uint8_t status_request);

#endif /* INC_API_MOTORS_H_ */
