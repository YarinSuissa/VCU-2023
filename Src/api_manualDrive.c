/*
 * api_manualDrive.c
 *
 *  Created on: May 4, 2023
 *      Author: Yarin Suissa
 */

#include <api_manualDrive.h>

_Bool Preparations_ManualDrive(){
	// Make sure motors are disabled
	DisableMotors();
	SetInverterTorque(0);

	//TODO: R2D, BrakePress, minimum voltage from inverters, and CAN is fine

	if(!TimeOutReached() &&
		GetVarDriver(1) &&			//TODO: check R2D VALUE
		GetVarDriver(6) > REQUIREMENT_MIN_BRAKE_PRESSURE &&
		GetVarInverter(1, 7) > REQUIREMENT_MIN_Vout &&
		GetVarInverter(1, 7) > REQUIREMENT_MIN_Vout)	{return true;}

	//TODO: UART DEBUG

//	if (ENABLE_UART_DEBUG)
//	{
//		sprintf(buffer, "\n\rState_Check_Val:%d\n\r" , State_Check_Val);
//		HAL_UART_Transmit(&huart2, (uint8_t*)buffer, strlen(buffer), HAL_MAX_DELAY);
//		sprintf(buffer, "Can_Check_Val %d\r\n", Can_Check_Val);
//		HAL_UART_Transmit(&huart2, (uint8_t*) buffer, (uint16_t) strlen(buffer), HAL_MAX_DELAY);
//	}

	return false;
}



