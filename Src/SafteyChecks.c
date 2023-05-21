/*
 * SafteyChecks.c
 *
 *  Created on: May 20, 2023
 *      Author: Yarin Suissa
 */

#include <SafetyChecks.h>


/* Safety_Buzzer:
 *
 * * save previous state of LEDs
 * * Buzz and blink for 5 seconds
 * * apply previous state of LEDs
 * */
void Safety_Buzzer(){
	uint8_t LED_Array_State[3] = {Get_LED(State_One),Get_LED(State_Two), Get_LED(State_Three)};
	BUZZER(SET);
	for (int i = 0; i < 5; i++) {
		ToggleLED(All_LEDS);
		HAL_Delay(700);
	}
	BUZZER(RESET);
	Set_LED(LED_Array_State[0], LED_Array_State[1], LED_Array_State[2], off);
}


_Bool Driver_R2D_Pressed(){
	return GetVarDriver(R2D_Value);
}

/* Inverter_Minimum_Voltage:
 *
 * Used to determine whether high voltage got actually turned on or not.
 * */
_Bool Inverter_Minimum_Voltage(){
	if(GetVarInverter(1, Inverter_Vout) > REQUIREMENT_MIN_Vout &&
	   GetVarInverter(1, Inverter_Vout) > REQUIREMENT_MIN_Vout){
				return true;
			}
	return false;
}

_Bool Driver_Minimum_Brake_Pressure(){
	if (GetVarDriver(Pedal_Brake_Pressure) > REQUIREMENT_MIN_BRAKE_PRESSURE){
		return true;
	}
	return false;
}

_Bool Driver_Gas_Pedal_Calibrated(){
	return GetFlag(Pedal_Gas_Calibrated);
}

_Bool Driver_Gas_Pedal_In_Range(){
	uint16_t Pedal_Value = GetVarDriver(Pedal_Gas_Value);
	return Pedal_Value >= 0 && Pedal_Value <= 1000;
}


/* ShortCircuitIdentifier
 *
 * Used to determine whether the gas/brake pedal are grounded to Vcc/ground
 * */
_Bool ShortCircuitIdentifier(uint16_t Gas_Value, uint16_t Brake_Pressure){
	int GasValueError = ABS_DIFF(Gas_Value,0xFF10);
	int BrakeValueError = ABS_DIFF(Brake_Pressure, 0xFF10);
	if (GasValueError <= 1 || BrakeValueError <=1){
		SetFlag(BAD, SDC_State);
		return true;
	}
	return false;
}
