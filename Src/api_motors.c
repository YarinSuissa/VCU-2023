/*
 * api_motors.c
 *
 *  Created on: 11 May 2023
 *      Author: Yarin Suissa
 */

#include <api_motors.h>

uint16_t Motors_ConvertTorqueToRegValue(double Desired_Torque_In_Percent)
{
    // Make sure that the percent desired is not negative and isn't higher then the max percent defined
    Desired_Torque_In_Percent = (Desired_Torque_In_Percent > 100) ? 100 : Desired_Torque_In_Percent;
    Desired_Torque_In_Percent = (Desired_Torque_In_Percent < 0) ? 0 : Desired_Torque_In_Percent;
    return MAX_VALUE_TORQUE * Desired_Torque_In_Percent / 100;
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
	SetPackage(3, REG_REQUEST_INFO, dest_reg, time_interval);
	SendToInverters(3);
}

void DisableMotors(){
	SetPackage(3, REG_DISABLE, 0x4, 0x0);
	SendToInverters(3);
}

/* EnableMotors:
 * when going from disabled to enabled, it may take a couple of seconds for the motors to be fully operational.
 * */

void EnableMotors(){
	SetPackage(1, REG_DISABLE);
	SendToInverters(1);
}

/* TimeOutMotors:
 *
 * Makes the inverters shut down if they don't get KeepAliveMsgMotors every 250ms
 * */
void TimeOutMotors(){
	SetPackage(3, REG_TIMEOUT, 0xF4, 0x01);
}
/* KeepAliveMsgMotors:
 *
 * Used to tell the inverters that VCU CAN BUS is still operational.
 * if inverters don't receive this message every ~200ms they shut down
 * */
void KeepAliveMsgMotors(){
	SetPackage(2, REG_REQUEST_INFO, 0x4);
	SendToInverters(2);
}

// This function sends torque command to motors of a scale between 0.0-100.0%
// The maximum torque allowed to be delivered is constraint at the motors.h file.

void SetInverterTorque(double DesiredTorque){
    uint16_t Desired_Torque_Value = Motors_ConvertTorqueToRegValue(DesiredTorque);

    if (TORQUE_IN_DEADZONE(Desired_Torque_Value))
    {
    	SetFlag(1, Zero_Torque);
    	Desired_Torque_Value = 0;
    }
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
	SendToInverters(3);
}



_Bool Request_Motors_Routine(uint8_t status_request){

	switch (status_request)
	{ //Every time request different kind of data
		case 0:
			ReadDataFromMotors(REG_TORQUE, 80);
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




