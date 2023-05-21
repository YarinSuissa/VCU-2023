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

/* Extern variables ----------------------------------------------------------*/



/* Private variables ---------------------------------------------------------*/
CAN_HandleTypeDef* hcan;
CAN_TxHeaderTypeDef TxHeader = {0};
CAN_RxHeaderTypeDef RxHeader = {0};
uint32_t TxMailbox = {0};

_Bool ENABLE_UART_DEBUG = 0;
/* Functions -----------------------------------------------------------------*/
uint8_t Get_LED(enumLEDs LED_Num){
	switch(LED_Num){
	case State_One:
		return HAL_GPIO_ReadPin(LED_STAGE_1_GPIO_Port, LED_STAGE_1_Pin);
	case State_Two:
		return HAL_GPIO_ReadPin(LED_STAGE_2_GPIO_Port, LED_STAGE_2_Pin);
	case State_Three:
		return HAL_GPIO_ReadPin(LED_STAGE_3_GPIO_Port, LED_STAGE_3_Pin);
	default:
		return 0;
	}
}

void Set_LED(enumSwitch LED1_Switch,enumSwitch LED2_Switch,enumSwitch LED3_Switch, enumSwitch LED_ERROR_Switch){
		HAL_GPIO_WritePin(LED_STAGE_1_GPIO_Port, LED_STAGE_1_Pin, LED1_Switch);
		HAL_GPIO_WritePin(LED_STAGE_2_GPIO_Port, LED_STAGE_2_Pin, LED2_Switch);
		HAL_GPIO_WritePin(LED_STAGE_3_GPIO_Port, LED_STAGE_3_Pin, LED3_Switch);
		HAL_GPIO_WritePin(LED_ERROR_GPIO_Port, LED_ERROR_Pin, LED_ERROR_Switch);
}

void ToggleLED(enumLEDs LED_Num){
	uint8_t LED_Array_State[3] = {Get_LED(State_One),Get_LED(State_Two), Get_LED(State_Three)};
	switch(LED_Num){
		case State_One:
			HAL_GPIO_TogglePin(LED_STAGE_1_GPIO_Port, LED_STAGE_1_Pin);
			break;
		case State_Two:
			HAL_GPIO_TogglePin(LED_STAGE_2_GPIO_Port, LED_STAGE_2_Pin);
			break;
		case State_Three:
			HAL_GPIO_TogglePin(LED_STAGE_3_GPIO_Port, LED_STAGE_3_Pin);
			break;
		case All_LEDS:
			if(LED_Array_State[0]){
				Set_LED(on,on,on, off);
			}else{
				Set_LED(off,off,off, off);
			}
			break;
	}
}

void BrakeLights(){
	if (GetVarDriver(Pedal_Brake_Value) > 300){
		BRAKE_LIGHTS(SET);
	}
	else{
		BRAKE_LIGHTS(RESET);
	}
}

void SendToInverters(uint8_t DLC){
	CAN_TX(MAIN_CAN, DLC, ID_INV1_TX);
	CAN_TX(MAIN_CAN, DLC, ID_INV2_TX);
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

/* Used to announce the state VCU is in. used mainly for compatibility */
void Announce_State_TX(int MSG){
	SetPackage(1, 0);
	CAN_TX(MAIN_CAN, 1, MSG);
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

void VCU_Error_Handler(enumError ErrorCode){

	Safety_Buzzer();
	switch(ErrorCode){
		case Communication_Error_Pedal:
			break;
		default:
			break;

	}
}
