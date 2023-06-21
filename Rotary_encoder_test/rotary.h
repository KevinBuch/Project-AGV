#ifndef ROTARY_H
#define ROTARY_H
#include <avr/io.h>
#include <avr/interrupt.h>
//define port where encoder is connected
#define ROTPORT PORTA
#define ROTDDR DDRA
#define ROTPIN PINA
//define rotary encoder pins
#define ROTPA PA0
#define ROTPB PA2
//define macros to check status
#define ROTA !((1<<ROTPA)&ROTPIN)
#define ROTB !((1<<ROTPB)&ROTPIN)
//prototypes
void RotaryInit(void);
void RotaryCheckStatus(void);
uint8_t RotaryGetStatus(void);
void RotaryResetStatus(void);
void Timer1_Start(void);
#endif
