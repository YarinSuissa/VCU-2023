/*
 * database.h
 *
 *  Created on: 30 Mar 2023
 *      Author: Silve
 */

#ifndef INC_DATABASE_H_
#define INC_DATABASE_H_

#include "platform.h"

/* enums */

typedef enum{
	R2D_Value,
	HIT_Value,
	Pedal_Gas_Calibrated,
	Pedal_Gas_Value,
	Pedal_Brake_Value,
	Pedal_Brake_Pressure,
	SteeringWheel_Angle,
	State,
	Zero_Torque,
	SDC_State,
}enumDriver;

typedef enum{
	GOOD,
	BAD,
}enumState;

typedef enum {
	Torque_Received,
	DC_Bus_Voltage,
	Motor_RPM,
	Motor_En_Status,
	Motor_Temp,
	Current,
	Inverter_Vout,
	Inverter_Temp,
	Torque_Sent
}enumInverter;

/* Private Structures */

struct FlagStruct {
	uint8_t  R2D_Value;
	uint8_t  HIT_Value;
	uint8_t  Zero_Torque;
	uint8_t  SDC_State;
	uint8_t  State;
	uint8_t  Pedal_Gas_Calibrated;
};

struct DriverStruct {

	uint16_t Pedal_Gas_Value;
	uint16_t Pedal_Brake_Value;
	uint16_t Pedal_Brake_Pressure;
	uint16_t SteeringWheel_Angle;
};

struct Inverter {
	const int Name;
	uint16_t Torque_Received;
	uint16_t DC_Bus_Voltage;
	uint16_t Motor_RPM;
	uint16_t Motor_En_Status;
	uint16_t Motor_Temp;
	uint16_t Current;
	uint16_t Vout;
	uint16_t Inverter_Temp;
	uint16_t Torque_Sent;
};

struct DataMaxUpdateTime {
	uint8_t Pedal;
	uint8_t Dashboard;
	uint8_t DC_Bus_Inv1;
	uint8_t DC_Bus_Inv2;
	uint8_t Vout_Inv1;
	uint8_t Vout_Inv2;
	uint8_t Current_Inv1;
	uint8_t Current_Inv2;
};

struct CanData {
	uint8_t Data[8];
};

/*Functions */

void 	 StoreCANData(uint32_t RxID, uint8_t NewData[8]);
void 	 SetFlag(uint8_t FlagValue, enumDriver NameOfFlag);
uint16_t GetVarDriver(uint8_t NumOfVar);
uint16_t GetVarInverter(uint8_t NumOfInverter, uint8_t NumOfVar);
uint8_t  GetLastTorqueSent();
uint8_t  GetFlag(enumDriver NameOfFlag);


extern struct DataMaxUpdateTime TimeOutPeriod;
extern const struct DataMaxUpdateTime MaxDelay;

extern uint8_t ReceivedData[8];
extern uint8_t TransmittedData[8];

#define SizeOfTimeOutPeriod sizeof(TimeOutPeriod)/sizeof(uint8_t)


#endif /* INC_DATABASE_H_ */
