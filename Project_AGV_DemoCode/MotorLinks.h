/*
 * h_bridge.h - XvR 2020
 */

#ifndef _H_BRIDGE_H_
#define _H_BRIDGE_H_

// These pins are available on the shield via the header:
//
//		Mega	Uno
// digital 5	PE3	PD5
// digital 6	PH3	PD6
// analog 5	PF5	PC5

// The settings below are for the Mega, modify
// in case you want to use other pins
#define PORT_RPWM	PORTL
#define PIN_RPWM	PL0
#define DDR_RPWM	DDRL

#define PORT_LPWM	PORTL
#define PIN_LPWM	PL1
#define DDR_LPWM	DDRL

void init_MotorLinks(void);
void MotorLinks_set_percentage(signed char percentage);

#endif /* _H_BRIDGE_H_ */
