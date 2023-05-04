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
#include "database.h"
#include "main.h"

#define LED_ERROR_GPIO_Port GPIOC
#define LED_ERROR_Pin GPIO_PIN_0


void CAN_RX(CAN_HandleTypeDef *hcan);
void CAN_TX(CAN_HandleTypeDef *hcan, uint8_t DLC, uint32_t Dest_ID);
void CAN_Filter_Init();
void SetInverterTorque(double DesiredTorque);

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
#define REG_DC_BUS 			   0x0eb
#define REG_RPM 			   0x030
#define REG_TEMP_MOTOR		   0x049
#define REG_TEMP_INVERTER	   0x04a
#define REG_VOUT			   0x08a
#define REG_CURRENT			   0x05f
#define REG_TORQUE_SENT 	   0x001

#define ERROR_LED_ON 		HAL_GPIO_WritePin(LED_ERROR_GPIO_Port, LED_ERROR_Pin, SET)
#define ERROR_LED_OFF 		HAL_GPIO_WritePin(LED_ERROR_GPIO_Port, LED_ERROR_Pin, RESET)

/* Functions */

void  DisableMotors();
void  GetDataFromMotors(uint8_t dest_reg, uint8_t time_interval);
_Bool TimeOutReached();
_Bool Request_Motors_Routine(uint8_t status_request);

/* MACROS */

#define GoToSleep() do { \
		HAL_SuspendTick(); \
		__WFI(); \
}while(0)

#define SendToInverters() do { \
		CAN_TX(MAIN_CAN, 3, ID_INV1_TX); \
		CAN_TX(MAIN_CAN, 3, ID_INV2_TX); \
}while(0)

#define State_One_LEDS do { \
		HAL_GPIO_WritePin(LED_STAGE_2_GPIO_Port, LED_STAGE_2_Pin, RESET); \
		HAL_GPIO_WritePin(LED_STAGE_3_GPIO_Port, LED_STAGE_3_Pin, RESET); \
		HAL_GPIO_WritePin(LED_STAGE_1_GPIO_Port, LED_STAGE_1_Pin, SET);	  \
}while(0);

#define State_Two_LEDS do { \
		HAL_GPIO_WritePin(LED_STAGE_2_GPIO_Port, LED_STAGE_2_Pin, SET); \
		HAL_GPIO_WritePin(LED_STAGE_3_GPIO_Port, LED_STAGE_3_Pin, RESET); \
		HAL_GPIO_WritePin(LED_STAGE_1_GPIO_Port, LED_STAGE_1_Pin, RESET);  \
}while(0);

#define State_Two_LEDS_BLINK do { \
		HAL_GPIO_TogglePin(LED_STAGE_2_GPIO_Port, LED_STAGE_2_Pin); \
		HAL_GPIO_WritePin(LED_STAGE_3_GPIO_Port, LED_STAGE_3_Pin, RESET); \
		HAL_GPIO_WritePin(LED_STAGE_1_GPIO_Port, LED_STAGE_1_Pin, RESET);  \
}while(0);

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
#define CLOSE_SD_RELAY 					HAL_GPIO_WritePin(SD_RELAY_GPIO_Port, SD_RELAY_Pin, SET)
#define  OPEN_SD_RELAY 					HAL_GPIO_WritePin(SD_RELAY_GPIO_Port, SD_RELAY_Pin, RESET)
#endif /* INC_PLATFORM_H_ */
