/*
 * api_common.c
 *
 *  Created on: Apr 15, 2023
 *      Author: Yarin Suissa
 */

#include <api_common.h>

/* Set Package:
 * Organize given arguments into TransmittedData
 *
 * If more than 8 arguments are given, only the
 * first 8 will make it in, the remaining arguments will be lost
 */
void SetPackage(uint8_t ArgNum, ...){
	if (ArgNum == 0) {return;}
	uint8_t i;
	va_list arglist;
	va_start(arglist, ArgNum);
	uint8_t loop = MIN(ArgNum, 8);
	for (i = 0 ; i < loop; i++){
		TransmittedData[i] = va_arg(arglist,int);
	}
	for (i = i; i < 8; i++){
		TransmittedData[i] = 0;
	}
	va_end(arglist);
	return;
}

/* Ready To Drive Sequence *******************************************************************************/

/* StartUpChecks :
 * 					* 1. Set up filter for CAN communication messages.
 * 					* 2. Allow interrupts by the buffer of the CAN messages. (Receiver FIFO 0/FIFO 1 & if the buffer is empty)
 * 					* 3. Initialize CAN module
 * 					* 4. Start timer to time receiving messages
 * 					* Check ShutDown circuitry
 * 					* Adjust LEDs
 * 					* Request data from inverters one time, as needed for startup
 * 					* Check gas pedal calibration
 * 					* Check ASMS switch state
 * 					* Disable Motors and make sure torque = 0
 *
 * equivalent to STATE 1 in previous code
 */
void StartUpChecks(){
	CAN_Filter_Init(&hcan1);
	HAL_CAN_ActivateNotification(&hcan1,CAN_IT_TX_MAILBOX_EMPTY | CAN_IT_RX_FIFO0_MSG_PENDING | CAN_IT_BUSOFF);
	HAL_CAN_Start(&hcan1);
	HAL_TIM_Base_Start_IT(&htim6);
	HAL_Delay(1000);
	Announce_State_TX(VCU_BOOT_MSG);
	SDC_RELAY(off);

	SDC_RELAY(on);

	DisableMotors();

	Set_LED(on, off, off, off);

	HAL_Delay(250);
	uint8_t i = 0;
	Announce_State_TX(VCU_BOOT_MSG);

	/* Request inverters to start sending relevant data at given time intervals*/
	while(1){
		uint8_t done = Request_Motors_Routine(i++);
		if(done==1)
		{
			break;
		}
		GoToSleep(); //GO TO SLEEP
	}

	if (ENABLE_UART_DEBUG)
	{
		sprintf(buffer, "Motors data requested\n");
		HAL_UART_Transmit(&huart2, (uint8_t*)buffer, strlen(buffer), HAL_MAX_DELAY);
	}

	while (GetVarDriver(State) != TWO){
		GoToSleep();
		if (GetVarDriver(Pedal_Gas_Value) == 0){
			SetFlag(1, Pedal_Gas_Calibrated);
		}
		else{
			SetFlag(0, Pedal_Gas_Calibrated);
		}

		int CANCommunicationValid = TimeOutReached();
		if (ENABLE_UART_DEBUG)
		{
			sprintf(buffer, "Gas pedal Calibrated: %d, Value: %d\n CAN Valid: %d", GetFlag(Pedal_Gas_Calibrated), GetFlag(Pedal_Gas_Value), CANCommunicationValid);
			HAL_UART_Transmit(&huart2, (uint8_t*)buffer, strlen(buffer), HAL_MAX_DELAY);
		}

		if (GetVarDriver(Pedal_Gas_Calibrated) && !ShortCircuitIdentifier(GetVarDriver(Pedal_Gas_Value), GetVarDriver(Pedal_Brake_Pressure)) && !CANCommunicationValid){
			SetFlag(TWO, State);
		}
	}

}

void PreparationsToDrive(){

	// Make sure motors are disabled
	DisableMotors();
	SetInverterTorque(0);

	Set_LED(off, on, off, off);
	Announce_State_TX(VCU_STATE_TWO_MSG);
	if (ENABLE_UART_DEBUG)
	{
		sprintf(buffer, "Stage 2");
		HAL_UART_Transmit(&huart2, (uint8_t*)buffer, strlen(buffer), HAL_MAX_DELAY);
	}

	while(1){
		ToggleLED(All_LEDS);

		if (ASMS_SWITCH_ON){
			if(Preparations_Autonomous())
			{
				break;
			}
		}else{
			if(Preparations_ManualDrive())
			{
				SetFlag(3, State);
				Safety_Buzzer();
				break;
			}
		}
	}

	if (ENABLE_UART_DEBUG)
	{
		sprintf(buffer, "Stage 2 is completed, ASMS: %d", ASMS_STATE);
		HAL_UART_Transmit(&huart2, (uint8_t*)buffer, strlen(buffer), HAL_MAX_DELAY);
	}
}

/* Common Tasks ****************************************************************************************/

/* RDDE (Receive Data & Detect Errors):
 * 					* Check if maximum waiting period between messages has been reached
 * 					* Calculate and organize received data from CAN bus
 * 					* Check short circuit to Vcc/Ground in gas/break pedals circuitry (<- There_is_Pedal_SCS_Error)
 * 					* Sample and close SD (ShutDown) circuit (as needed for it's default value)
 *
 */

void RDDE(){

	// Check SCS errors - 0xFF10/0xFF11, only gas_pedal_value and brake_pressure value influence wheel torque
	if (ShortCircuitIdentifier(GetVarDriver(Pedal_Gas_Value), GetVarDriver(Pedal_Brake_Pressure))){
	  //TODO: ErrorHandler(ShortCircuit);
	}
	else	// No SCS exists
	{
	  //TODO SCS ERROR OFF
	  if (!TimeOutReached() && Driver_Gas_Pedal_Calibrated()){
		  //TODO Turn off error LED
	}
	// Send CAN messages regarding final shutdown state, except for State three, which handles this differently(250ms delay)

	if (ENABLE_UART_DEBUG)
	{
		//TODO DEBUG
	}
	return;
	}
}

void Drive(){
	Set_LED(off, off, on, off);
	EnableMotors();
	HAL_TIM_Base_Start_IT(&htim7); // KeepAliveMsgMotors timing is handled by Timer 7
	TimeOutMotors();

	if(ASMS_STATE){
		Autonomous_Driving_Routine();
	}else{
		Manual_Driving_Routine();
	}
}
