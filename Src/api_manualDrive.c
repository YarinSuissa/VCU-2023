/*
 * api_manualDrive.c
 *
 *  Created on: May 4, 2023
 *      Author: Yarin Suissa
 */

#include <api_manualDrive.h>

_Bool Preparations_ManualDrive(){

	if(!TimeOutReached() &&
		Driver_R2D_Pressed() &&
		Driver_Minimum_Brake_Pressure() &&
		Inverter_Minimum_Voltage())
	{
		return true;
	}

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

void Manual_Driving_Routine(){
	//TODO: UART DEBUG STATE 3
	while(1){
		//TODO: Safety Checks & CHECK SD Circuit

		if(Driver_Gas_Pedal_In_Range()){
			SetInverterTorque((double)GetVarDriver(Pedal_Gas_Value)/10); //ediff how & when?
		}
		else{
			SetInverterTorque(0);
		}
	}
}


