/*
 * debug_options.h
 *
 *  Created on: May 4, 2023
 *      Author: Silve
 */

#ifndef INC_DEBUG_OPTIONS_H_
#define INC_DEBUG_OPTIONS_H_

/* Debug Toggles for calibration */
#define DEBUG_INVERTER_CALIBRATION_UNDER_LOAD 0 	// used to request multiple data items from inverters for calibration purppses
#define DEBUG_UART_COM						  0		// Verbose mode - used to enable all print commands for uart communication.

/* Known Addresses and constants */
#define CALIBRATION_N_CMD					  0x32  // Speed setpoint after ramp and limit
#define CALIBRATION_I_ACTIVE_CMD			  0x26  // Active current (Iq) Setpoint (internal)
#define CALIBRATION_I_ACTUAL				  0x20  // Actual current value

#endif /* INC_DEBUG_OPTIONS_H_ */
