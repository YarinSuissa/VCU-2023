/*
 * platform.c
 *
 *  Created on: Mar 23, 2023
 *      Author: Yarin Suissa
 */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
/* USER CODE END PTD */

/* Private includes ------------------------------------------------------------*/
#include <platform.h>
#include <debug_options.h>
/* Extern variables ----------------------------------------------------------*/



/* Private variables ---------------------------------------------------------*/
CAN_HandleTypeDef* hcan;
CAN_TxHeaderTypeDef TxHeader = {0};
CAN_RxHeaderTypeDef RxHeader = {0};
uint32_t TxMailbox = {0};

_Bool ENABLE_UART_DEBUG = 0;
_Bool GAS_IS_ZERO = 0;
/* Functions -----------------------------------------------------------------*/


uint16_t Motors_ConvertTorqueToRegValue(double Desired_Torque_In_Percent)
{
    // Make sure that the percent desired is not negative and isn't higher then the max percent defined
    Desired_Torque_In_Percent = (Desired_Torque_In_Percent > 100) ? 100 : Desired_Torque_In_Percent;
    Desired_Torque_In_Percent = (Desired_Torque_In_Percent < 0) ? 0 : Desired_Torque_In_Percent;
    return MAX_VALUE_TORQUE * Desired_Torque_In_Percent / 100;
}

/* IsTimeOutReached:
 * * Checks if timeout was reached since last update on any the members of the struct
 * * If a certain variable wasn't updated in max timeout period - raise FSM_Error_Handler
 * * Replaces Check_CAN_Data() in old code files
 *
*/
_Bool TimeOutReached(){
	/* Update all members of the TimeOutPeriod struct with +1, using pointer to
	 * point to TimeOutPeriod's location in the memory */
	uint8_t *TimeOutPointer = (uint8_t *)&TimeOutPeriod;
	uint8_t *MaxDelayPointer = (uint8_t *)&MaxDelay;
	for (uint8_t i = 0; i < SizeOfTimeOutPeriod; i++){
		*TimeOutPointer++ += 1;
		if (TimeOutPointer[i] >= MaxDelayPointer[i]){
			Error_Handler(/*i+100*/);
			//TODO create unified error handler
			return true;
		}
	}
	return false;
}

/* CANBUS FUNCTIONS -------------------------------------------------------------*/

void CAN_RX(CAN_HandleTypeDef *hcan){
	HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &RxHeader, ReceivedData);
	StoreCANData(RxHeader.StdId, ReceivedData);
}

void CAN_TX(CAN_HandleTypeDef *hcan, uint8_t DLC, uint32_t Dest_ID){
	TxHeader.DLC 	= DLC;
	TxHeader.StdId 	= Dest_ID;
	TxHeader.IDE 	= CAN_ID_STD;
	TxHeader.RTR 	= CAN_RTR_DATA;

	// STM CAN transmission function
	HAL_Delay(1);
	HAL_CAN_AddTxMessage(hcan, &TxHeader, TransmittedData, &TxMailbox);
}

void CAN_Filter_Init(CAN_HandleTypeDef *hcan)
{
	CAN_FilterTypeDef canFilter;
	canFilter.FilterActivation = ENABLE;
	canFilter.FilterFIFOAssignment = CAN_RX_FIFO0;
	canFilter.FilterMode = CAN_FILTERMODE_IDMASK;
	canFilter.FilterScale = CAN_FILTERSCALE_32BIT;
	//FILTER 0

	canFilter.FilterBank = 0;
	canFilter.FilterIdHigh = 0xFFF0;
	canFilter.FilterIdLow = 0x0000;
	canFilter.FilterMaskIdHigh = 0x0000;
	canFilter.FilterMaskIdLow = 0x0000;

	if (HAL_CAN_ConfigFilter(hcan, &canFilter) != HAL_OK)
		Error_Handler();
}

/* ReadDataFromMotors:
 *
 * Sends a request for both inverters* to send specific information at a relevant time interval.
 * Time Intervals:
 * 1. 0x00 - Send data only once
 * 2. 0xFF - Stop sending data
 * 3. 0xYY - Send data at 0xYY interval (replace YY with value)
 *
 * * TODO: for now, each message is sent to both inverters, but in the future it may be configurable
 * * to send a message to a specific inverter only.
 * */

void ReadDataFromMotors(uint8_t dest_reg, uint8_t time_interval){
	SetPackage(3, 0x3D, dest_reg, time_interval);
	SendToInverters();
}

void DisableMotors(){
	SetPackage(3, REG_DISABLE, 0x4, 0x0);
	SendToInverters();
}

/* EnableMotors:
 * when going from disabled to enabled, it may take a couple of seconds for the motors to be fully operational.
 * */

void EnableMotors(){
	SetPackage(1, REG_DISABLE);
	SendToInverters();
}

// This function sends torque command to motors of a scale between 0.0-100.0%
// The maximum torque allowed to be delivered is constrainted at the motors.h file.

void SetInverterTorque(double DesiredTorque){
    uint16_t Desired_Torque_Value = Motors_ConvertTorqueToRegValue(DesiredTorque);

    if (TORQUE_IN_DEADZONE(Desired_Torque_Value)) {GAS_IS_ZERO=1;}
    SetPackage(3, REG_TORQUE, (uint8_t)((0xFFUL) & Desired_Torque_Value >> 0U), (uint8_t)((0xFFUL) & Desired_Torque_Value >> 8U));

    uint8_t lastTorque[3] = {REG_TORQUE_SENT, TransmittedData[1], TransmittedData[2]};
    StoreCANData(ID_INV1_RX, lastTorque);
    StoreCANData(ID_INV2_RX, lastTorque);

///////////SEND UART - print last LAST TORQUE ///////////
	if (ENABLE_UART_DEBUG)
	{
		sprintf(buffer, "LAST TORQUE SENT:%d \n\r", ((uint16_t)TransmittedData[2] << 8) + TransmittedData[1]);
		HAL_UART_Transmit(&huart2, (uint8_t*) buffer, (uint16_t) strlen(buffer), HAL_MAX_DELAY);
	}
////////////////////////////////////////////////
	SendToInverters();
}

_Bool Request_Motors_Routine(uint8_t status_request){

	switch (status_request)
	{ //Every time request deferent kind of data
		case 0:																	// unused
			if (0){
			ReadDataFromMotors(REG_TORQUE, 80);
			}
			break;
		case 1:
			//ReadDataFromMotors(REG_ERROR, 80);
			break;
		case 2:
			ReadDataFromMotors(REG_DC_BUS, 10);
			break;
		case 3:
			ReadDataFromMotors(REG_RPM, 10);
			break;
		case 4:
			ReadDataFromMotors(REG_TEMP_MOTOR, 80);
			break;
		case 5:
			ReadDataFromMotors(REG_CURRENT, 10);
			break;
		case 6:
			ReadDataFromMotors(REG_VOUT, 10);
			break;
		case 7:
			ReadDataFromMotors(REG_TEMP_INVERTER, 80);
			break;
		case 8:
			if (DEBUG_INVERTER_CALIBRATION_UNDER_LOAD)
			{
				ReadDataFromMotors(CALIBRATION_N_CMD, 10);
				ReadDataFromMotors(CALIBRATION_I_ACTIVE_CMD, 20);
				ReadDataFromMotors(CALIBRATION_I_ACTUAL, 20);
			}
			break;
		default:
			break;
	}

	if (status_request > 8)
	{
		return true;
	}
	return false;
}

