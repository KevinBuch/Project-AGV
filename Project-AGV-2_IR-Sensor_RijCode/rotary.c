#include "rotary.h"

static volatile int rotarystatus;
//In library source file we declare following functions. First of all initialize pins where rotary encoder is connected:
void RotaryInit(void)
{
//set pins as input
ROTDDR &= ~((1<<ROTPA)|(1<<ROTPB));
//enable interrnal pullups;
ROTPORT |= (1<<ROTPA)|(1<<ROTPB);
}
//we set selected port pins as inputs and enable internal pull-up resistors.
//Then follows our rotary check status function which sets our internal variable to some variable.
void RotaryCheckStatus(void)
{
//reading rotary and button
//check if rotation is left
     if(ROTA & (!ROTB))
        {
            loop_until_bit_is_set(ROTPIN, ROTPA);
            if (ROTB)
            rotarystatus=1;
        //check if rotation is right
        }
        else if(ROTB & (!ROTA))
        {
            loop_until_bit_is_set(ROTPIN, ROTPB);
            if (ROTA)
                rotarystatus=2;
        }
        else if (ROTA & ROTB)
        {
            loop_until_bit_is_set(ROTPIN, ROTPA);
            if (ROTB)
                rotarystatus=1;
             else rotarystatus=2;
        }
}
//if know was turned left – rotary status is set to 1, if right then value is 2 and if button was pressed – status is set to 3.
//Following two functions simply returns status and resets it:
//return button status
uint8_t RotaryGetStatus(void)
{
return rotarystatus;
}
//reset status
void RotaryResetStatus(void)
{
rotarystatus=0;
}

//We are using a graphical LCD based on ks0108 controller for displaying messages. How co control it and how to set up library follow this post. We won’t go in to details on this.
//To check rotary encoder status we are using Timer2 overflow interrupts. Microcontroller is running at 16MHz so with prescaller 256 overflow interrupt occur 122 times/s. Speed seems to be suitable for fluent operation.
void Timer1_Start(void)
{
    TCCR1B|=(0<<CS12)|(0<<CS11)|(1<<CS10);    //prescaller 256 ~122 interrupts/s
    TIMSK1|=(1<<TOIE1);             //Enable Timer1 Overflow interrupts
}

//After timer has been started we can put RotarryCheckStatus function inside it and read encoder actions:
ISR(TIMER1_OVF_vect)
{
//reading rotary
RotaryCheckStatus();
}
