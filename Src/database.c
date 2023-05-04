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
struct DriverStruct DriverDB 				= {0,0,0,0,0,0,0};
struct Inverter Inverter1DB 				= {1,0,0,0,0,0,0,0,0,0};
struct Inverter Inverter2DB 				= {2,0,0,0,0,0,0,0,0,0};
struct DataMaxUpdateTime TimeOutPeriod  	= {0,0,0,0,0,0,0,0};
const struct DataMaxUpdateTime MaxDelay 	= {20,50,20,20,20,20,20,20};
uint8_t State								=  0 ;
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
			if (inverter.Name == 1)			{TimeOutPeriod.DC_Bus_Inv1 = 0;}
			else 							{TimeOutPeriod.DC_Bus_Inv2 = 0;}
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
			if (inverter.Name == 1)			{TimeOutPeriod.Current_Inv1 = 0;}
			else 							{TimeOutPeriod.Current_Inv2 = 0;}
			return;
		case REG_VOUT:
			inverter.Vout					= ((uint16_t)NewData[2] << 8) + NewData[1];
			if (inverter.Name == 1)			{TimeOutPeriod.Vout_Inv1 = 0;}
			else 							{TimeOutPeriod.Vout_Inv2 = 0;}
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
			DriverDB.Pedal_Brake_Pressure	= (((uint16_t) NewData[4] << 8) | NewData[5]);
			break;
		case ID_INV1_RX:
			StoreToInverterDB(Inverter1DB, NewData);
			break;
		case ID_INV2_RX:
			StoreToInverterDB(Inverter2DB, NewData);
			break;
		case ID_DASHBOARD:
			TimeOutPeriod.Dashboard			= 0;
			DriverDB.R2D_Value				= ReceivedData[0];
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

void SetFlag(uint8_t FlagValue, uint8_t NameOfFlag){
	switch(NameOfFlag){
		case Flag_Pedal_Gas:
			DriverDB.Pedal_Gas_Calibrated = FlagValue;
			break;
		case Flag_R2D:
			DriverDB.R2D_Value 			  = FlagValue;
			break;
		case Flag_HIT:
			DriverDB.HIT_Value			  = FlagValue;
			break;
		case Flag_State:
			State = TWO;
			break;
	}
}

uint16_t GetVarDriver(uint8_t NumOfVar){
	switch (NumOfVar){
		case 1:
			return (uint16_t)DriverDB.R2D_Value;
		case 2:
			return (uint16_t)DriverDB.HIT_Value;
		case 3:
			return DriverDB.Pedal_Gas_Calibrated;
		case 4:
			return DriverDB.Pedal_Gas_Value;
		case 5:
			return DriverDB.Pedal_Brake_Value;
		case 6:
			return DriverDB.Pedal_Brake_Pressure;
		case 7:
			return DriverDB.SteeringWheel_Angle;
		case 8:
			return State;
	}
	return 0xFFFF;
}



uint16_t PullVarFromInverterDB(struct Inverter inverter, uint8_t NumOfVar){
	switch (NumOfVar){
		case 1:
			return inverter.Torque_Received;
		case 2:
			return inverter.DC_Bus_Voltage;
		case 3:
			return inverter.Motor_RPM;
		case 4:
			return inverter.Motor_En_Status;
		case 5:
			return inverter.Motor_Temp;
		case 6:
			return inverter.Current;
		case 7:
			return inverter.Vout;
		case 8:
			return inverter.Inverter_Temp;
		case 9:
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

