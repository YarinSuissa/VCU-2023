/*
 * api_common.c
 *
 *  Created on: Apr 15, 2023
 *      Author: Yarin Suissa
 */

#include <api_common.h>
#include <debug_options.h>

/* Set Package:
 * Organize given arguments into TransmittedData
 *
 * If more than 8 arguments are given, only the
 * first 8 will make it in, the remaining arguments will be lost
 */
void SetPackage(uint8_t ArgNum, ...){
	if (ArgNum == 0) {return;}
	int i;
	va_list arglist;
	va_start(arglist, ArgNum);
	for (i = 0 ; i < MIN(ArgNum, 8); i++){
		TransmittedData[i] = va_arg(arglist,int);
	}
	for (i = i; i < 8; i++){
		TransmittedData[i] = 0;
	}
	va_end(arglist);
	return; // needed?
}

_Bool ShortCircuitIdentifier(uint16_t Gas_Value, uint16_t Brake_Pressure){
	return (ABS_DIFF(Gas_Value,0xFF10) <= 1 || ABS_DIFF(Brake_Pressure, 0xFF10)) // simplifir
}

// would remove comments and split function to smaller chunks
void StartUpChecks(){
	CAN_Filter_Init(&hcan1); //1
	HAL_CAN_ActivateNotification(&hcan1,CAN_IT_TX_MAILBOX_EMPTY | CAN_IT_RX_FIFO0_MSG_PENDING | CAN_IT_BUSOFF); //2
	HAL_CAN_Start(&hcan1); //3
	HAL_TIM_Base_Start_IT(&htim6); //4
	HAL_Delay(1000);
	CAN_TX(MAIN_CAN, 3, VCU_BOOT_MSG);
	OPEN_SD_RELAY;
	//StopDataFromMotors(REG_DISABLE);

	// Init shutdown(in order to avoid SD relay closing problem)
	HAL_Delay(250);

	// At the new logic, the SD is closed from the ECU all the time
	CLOSE_SD_RELAY;

	DisableMotors();
	SetInverterTorque(0);

	// Turn on LED state 1
	State_One_LEDS;

//	TODO: DEBUG UART
//	if (ENABLE_UART_DEBUG)
//	{
//		sprintf(buffer, "Motors disabled, torque=%d, LEDS adjusted\r\n", GetLastTorque());
//		HAL_UART_Transmit(&huart2, (uint8_t*) buffer, strlen(buffer),HAL_MAX_DELAY);
//	}

	HAL_Delay(250);
	uint8_t i = 0;
	while(1){
		uint8_t done = Request_Motors_Routine(i++);
		if(done==1){ // linting?
			break;
		}
		GoToSleep(); //GO TO SLEEP
	}

	// Check if gas pedal is calibrated at initial state
	while (GetVarDriver(8) != TWO){
		GoToSleep(); //GO TO SLEEP
		if (GetVarDriver(4) == 0){ // linting?
			SetFlag(1, Flag_Pedal_Gas);
		}
		else{
			SetFlag(0, Flag_Pedal_Gas);
			//TODO: Fsm_Error_Handler(GAS_NOT_CALIBRATED_ERR);
		}

		if (GetVarDriver(3) && !ShortCircuitIdentifier(GetVarDriver(4), GetVarDriver(6)) && !TimeOutReached()) {SetFlag(TWO, Flag_State);}
	}

//	TODO: DEBUG UART
//	if (ENABLE_UART_DEBUG)
//	{
//		sprintf(buffer, "\n\rState_Check_Val:%d\n\r" , State_Check_Val);
//		HAL_UART_Transmit(&huart2, (uint8_t*)buffer, strlen(buffer), HAL_MAX_DELAY);
//		sprintf(buffer, "Can_Check_Val %d\r\n", Can_Check_Val);
//		HAL_UART_Transmit(&huart2, (uint8_t*) buffer, (uint16_t) strlen(buffer), HAL_MAX_DELAY);
//	}
//	return;
}

void PreparationsToDrive(){

	//TODO: DEBUG UART - Welcome To Stage 2

	HAL_TIM_Base_Start_IT(&htim7);

	while(1){
		State_Two_LEDS_BLINK;

		if (ASMS_SWITCH_ON){
			if(Preparations_Autonomous()){// linting?
				break;
			}
		}
		else{
			if(Preparations_ManualDrive()){ // linting?
				break;
			}
		}
	}

	HAL_TIM_Base_Stop_IT(&htim7);

	State_Two_LEDS;

	//TODO: DEBUG UART - Preparations Comepleted, ASMS is: ON/OFF
}


