/*
 * platform.h
 *
 *  Created on: Mar 23, 2023
 *      Author: Silve
 */

#ifndef INC_PLATFORM_H_
#define INC_PLATFORM_H_

#include "stm32f4xx_hal.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <math.h>
#include "database.h"
#include "main.h"
#include "debug_options.h"
#include "SafetyChecks.h"

#define LED_ERROR_GPIO_Port GPIOC
#define LED_ERROR_Pin GPIO_PIN_0

/* enums */
typedef enum{
	State_One,
	State_Two,
	State_Three,
	All_LEDS,
}enumLEDs;

typedef enum{
	off,
	on
}enumSwitch;

typedef enum{
	Communication_Error_Pedal,
	Communication_Error_Inverter1,
	Communication_Error_Inverter2,
	Communication_Error_Dashboard,
	SD_Circuit_Open_Error,
	Short_Circuit_GND_VCC_Error,
	Implausability_Value_Error,
	Pedal_Gas_Calibration_Error,
}enumError;


extern _Bool ENABLE_UART_DEBUG;

/* List of needed constants*/
#define MAX_VALUE_TORQUE	       32760


/* Legacy STATE definitions, to not break compatibility */
#define ONE				1
#define TWO				2
#define TWO_HALF		3
#define THREE			4

/* ID of received CAN messages */
#define ID_PEDAL 		0x030
#define ID_IMU	 		0x040
#define ID_DASHBOARD 	0x050
#define ID_INV1_RX 		0x181
#define ID_INV2_RX 		0x182
#define ID_INV1_TX 		0x202
#define ID_INV2_TX 		0x201
#define ID_BMS	 		0x240

/* ID for transmitted CAN messages*/
#define VCU_ERROR_MSG		  		0x20
#define VCU_BOOT_MSG		  		0x100
#define VCU_STATE_ONE_MSG		  	0x101
#define VCU_STATE_TWO_MSG		  	0x102
#define VCU_STATE_TWO_HALF_MSG		0x103
#define VCU_STATE_THREE_MSG		  	0x104
#define STM_ERROR_HANDLER_MSG		0x412

/* REG (Registers) locations for inverters */
#define REG_TORQUE 			   0x090
#define REG_DISABLE			   0x051
#define REG_REQUEST_INFO	   0x03D
#define REG_DC_BUS 			   0x0eb
#define REG_RPM 			   0x030
#define REG_TEMP_MOTOR		   0x049
#define REG_TEMP_INVERTER	   0x04a
#define REG_VOUT			   0x08a
#define REG_CURRENT			   0x05f
#define REG_TORQUE_SENT 	   0x001
#define REG_TIMEOUT			   0x0D0

#define ERROR_LED_ON 		HAL_GPIO_WritePin(LED_ERROR_GPIO_Port, LED_ERROR_Pin, SET)
#define ERROR_LED_OFF 		HAL_GPIO_WritePin(LED_ERROR_GPIO_Port, LED_ERROR_Pin, RESET)

/* Functions */
void Announce_State_TX();
_Bool TimeOutReached();
void CAN_RX(CAN_HandleTypeDef *hcan);
void CAN_TX(CAN_HandleTypeDef *hcan, uint8_t DLC, uint32_t Dest_ID);
void CAN_Filter_Init();
uint8_t Get_LED(enumLEDs LED_Num);
void Set_LED(enumSwitch LED1_Switch,enumSwitch LED2_Switch,enumSwitch LED3_Switch, enumSwitch LED_ERROR_Switch);
void ToggleLED(enumLEDs LED_Num);
void SendToInverters(uint8_t DLC);

/* MACROS */

#define ASMS_STATE 			HAL_GPIO_ReadPin(GPIOB, ASMS_STATE_Pin)
#define BRAKE_LIGHTS(X)		HAL_GPIO_WritePin(BRAKE_LIGHT_GPIO_Port, BRAKE_LIGHT_Pin, X)

#define GoToSleep() do { \
		  HAL_SuspendTick(); \
		  HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI); \
		  HAL_ResumeTick(); \
}while(0)

#define COMMONTASKS() do { \
		GoToSleep(); \
		RDDE(); \
		BreakLights(); \
}while(0)

#define TryRaiseError(try, errCode) do { \
		if (try == 0xFFFF){ \
		ErrorHandler(errCode);\
		}; \
}while(0)

/* Conditions */
#define MIN(X, Y)						(X < Y) ? X : Y
#define MAX(X, Y)						(X > Y) ? X : Y
#define TORQUE_IN_DEADZONE(X)			(X>=-1 && X<=33) ? 1 : 0
#define ABS_DIFF(X,Y)					(X > Y) ? (X-Y) : (Y-X)

/* */
#define SDC_RELAY(X)					HAL_GPIO_WritePin(SD_RELAY_GPIO_Port, SD_RELAY_Pin, X)

#define CLOSE_SD_RELAY 					HAL_GPIO_WritePin(SD_RELAY_GPIO_Port, SD_RELAY_Pin, SET)
#define  OPEN_SD_RELAY 					HAL_GPIO_WritePin(SD_RELAY_GPIO_Port, SD_RELAY_Pin, RESET)
#endif /* INC_PLATFORM_H_ */
