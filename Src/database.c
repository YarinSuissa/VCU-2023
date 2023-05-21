/*
 * platform_database.c
 *
 *  Created on: Mar 23, 2023
 *      Author: Yarin Suissa
 */

/* Private includes ------------------------------------------------------------*/
#include <database.h>
char buffer[200];

/* External variables ----------------------------------------------------------*/
struct FlagStruct FlagDB					= {0,0,0,0,0,0};
struct DriverStruct DriverDB 				= {0,0,0,0};
struct Inverter Inverter1DB 				= {1,0,0,0,0,0,0,0,0,0};
struct Inverter Inverter2DB 				= {2,0,0,0,0,0,0,0,0,0};
struct DataMaxUpdateTime TimeOutPeriod  	= {0,0,0,0,0,0,0,0};
const struct DataMaxUpdateTime MaxDelay 	= {20,50,20,20,20,20,20,20};
uint8_t ReceivedData[8] 					= {0};
uint8_t TransmittedData[8]					= {0};

void StoreToInverterDB(struct Inverter inverter, uint8_t NewData[8]){
	switch(NewData[0]){
		case REG_TORQUE:
			inverter.Torque_Received 		= ((uint16_t)NewData[2] << 8) + NewData[1];
			// add timeout and max delay for torque? wasn't used in old code.
			return;
		case REG_DISABLE:
			inverter.Motor_En_Status		= ((uint16_t)NewData[2] << 8) + NewData[1];
			return;
			// TODO: add timeout and max delay for "Disable"? wasn't used in old code.
		case REG_DC_BUS:
			inverter.DC_Bus_Voltage			= ((uint16_t)NewData[2] << 8) + NewData[1];
			if (inverter.Name == 1){
				TimeOutPeriod.DC_Bus_Inv1 = 0;
			}else{
				TimeOutPeriod.DC_Bus_Inv2 = 0;
			}
			return;
		case REG_RPM:
			inverter.Motor_RPM				= ((uint16_t)NewData[2] << 8) + NewData[1];
			// TODO: add timeout and max delay for RPM? wasn't used in old code.
			return;
		case REG_TEMP_MOTOR:
			inverter.Motor_Temp				= ((uint16_t)NewData[2] << 8) + NewData[1];
			// TODO: Need to add max delay for motor temperature. wasn't used in old code.
			return;
		case REG_CURRENT:
			inverter.Current				= ((uint16_t)NewData[2] << 8) + NewData[1];
			if (inverter.Name == 1){
				TimeOutPeriod.Current_Inv1 = 0;
			}else{
				TimeOutPeriod.Current_Inv2 = 0;
			}
			return;
		case REG_VOUT:
			inverter.Vout					= ((uint16_t)NewData[2] << 8) + NewData[1];
			if (inverter.Name == 1){
				TimeOutPeriod.Vout_Inv1 = 0;
			}else{
				TimeOutPeriod.Vout_Inv2 = 0;
			}
			return;
		case REG_TEMP_INVERTER:
			inverter.Inverter_Temp			= ((uint16_t)NewData[2] << 8) + NewData[1];
			// TODO: Need to add max delay for inverter temperature. wasn't used in old code.
			return;
		case REG_TORQUE_SENT:
			inverter.Torque_Sent 			= ((uint16_t)NewData[2] << 8) + NewData[1];
			return;
	}
}

void StoreCANData(uint32_t RxID, uint8_t NewData[8]){
	switch(RxID){
		case ID_PEDAL:
			TimeOutPeriod.Pedal 			= 0;
			DriverDB.Pedal_Gas_Value   		= (((uint16_t) NewData[0] << 8) | NewData[1]);
			DriverDB.Pedal_Brake_Value 		= (((uint16_t) NewData[2] << 8) | NewData[3]);
			DriverDB.SteeringWheel_Angle	= (((uint16_t) NewData[4] << 8) | NewData[5]);
			DriverDB.Pedal_Brake_Pressure	= (((uint16_t) NewData[8] << 8) | NewData[7]);
			break;
		case ID_INV1_RX:
			StoreToInverterDB(Inverter1DB, NewData);
			break;
		case ID_INV2_RX:
			StoreToInverterDB(Inverter2DB, NewData);
			break;
		case ID_DASHBOARD:
			TimeOutPeriod.Dashboard			= 0;
			FlagDB.R2D_Value				= ReceivedData[0];
			//DriverDB.R2D_Value				= ReceivedData[1]; // Used for UART DEBUG
			break;
		case ID_IMU:
			// TODO
			break;
		case ID_BMS:
			// TODO
			break;
	}
	sprintf(buffer, "Gas Pedal: %d\t Brake Value: %d\t", DriverDB.Pedal_Gas_Value, DriverDB.Pedal_Brake_Value);
	HAL_UART_Transmit(&huart2, (uint8_t*)buffer, strlen(buffer), HAL_MAX_DELAY);
	return;
}

uint8_t GetFlag(enumDriver NameOfFlag){
	switch(NameOfFlag){
		case Pedal_Gas_Calibrated:
			return FlagDB.Pedal_Gas_Calibrated;
		case R2D_Value:
			return FlagDB.R2D_Value;
		case HIT_Value:
			return FlagDB.HIT_Value;
		case State:
			return FlagDB.State;
		case Zero_Torque:
			return FlagDB.Zero_Torque;
		default:
			return 0;
	}
}

void SetFlag(uint8_t FlagValue, enumDriver NameOfFlag){
	switch(NameOfFlag){
		case Pedal_Gas_Calibrated:
			FlagDB.Pedal_Gas_Calibrated   = FlagValue;
			break;
		case R2D_Value:
			FlagDB.R2D_Value 			  = FlagValue;
			break;
		case HIT_Value:
			FlagDB.HIT_Value			  = FlagValue;
			break;
		case State:
			FlagDB.State				  = FlagValue;
		case Zero_Torque:
			FlagDB.Zero_Torque			  = FlagValue;
			break;
		case SDC_State:
			FlagDB.SDC_State			  = FlagValue;
			break;
		default:
			break;
	}
}

uint16_t GetVarDriver(enumDriver NumOfVar){
	switch (NumOfVar){
		case R2D_Value:
			return (uint16_t)FlagDB.R2D_Value;
		case HIT_Value:
			return (uint16_t)FlagDB.HIT_Value;
		case Pedal_Gas_Calibrated:
			return FlagDB.Pedal_Gas_Calibrated;
		case Pedal_Gas_Value:
			return DriverDB.Pedal_Gas_Value;
		case Pedal_Brake_Value:
			return DriverDB.Pedal_Brake_Value;
		case Pedal_Brake_Pressure:
			return DriverDB.Pedal_Brake_Pressure;
		case SteeringWheel_Angle:
			return DriverDB.SteeringWheel_Angle;
		case State:
			return FlagDB.State;
		default:
			break;
	}
	return 0xFFFF;
}



uint16_t PullVarFromInverterDB(struct Inverter inverter, enumInverter NumOfVar){
	switch (NumOfVar){
		case Torque_Received:
			return inverter.Torque_Received;
		case DC_Bus_Voltage:
			return inverter.DC_Bus_Voltage;
		case Motor_RPM:
			return inverter.Motor_RPM;
		case Motor_En_Status:
			return inverter.Motor_En_Status;
		case Motor_Temp:
			return inverter.Motor_Temp;
		case Current:
			return inverter.Current;
		case Inverter_Vout:
			return inverter.Vout;
		case Inverter_Temp:
			return inverter.Inverter_Temp;
		case Torque_Sent:
			return inverter.Torque_Sent;
	}
	return 0xFFFF;
}

uint16_t GetVarInverter(uint8_t NumOfInverter, uint8_t NumOfVar){
	switch (NumOfInverter){
		case 1:
			return PullVarFromInverterDB(Inverter1DB, NumOfVar);
		case 2:
			return PullVarFromInverterDB(Inverter2DB, NumOfVar);
	}
	return 0xFFFF;
}


/* ZeroOutStructs:
 *
 * In case of a critical error (such as SD circuit, etc), zero out all stored data to prevent any data
 * from previous drive to make it into the new drive.
 * */
void ZeroOutStructs(){
	memset(&DriverDB, 0, sizeof(struct DriverStruct));
	memset(&Inverter1DB.Torque_Received, 0, sizeof(struct Inverter) - sizeof(Inverter1DB.Name));
	memset(&Inverter2DB.Torque_Received, 0, sizeof(struct Inverter) - sizeof(Inverter2DB.Name));
	memset(&TimeOutPeriod, 0, sizeof(struct DataMaxUpdateTime));
}

