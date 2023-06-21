/*
 */

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "rotary.h"
#include "tm1637.h"
#include "MotorRechts.h"
#include "MotorLinks.h"

void init(void)
{
    RotaryInit();
    Timer1_Start();

    init_MotorLinks();
    init_MotorRechts();

    sei();

    //DDRF |= (1<<PF0);       // LED pin output
    //DDRF |= (1<<PF1);       // LED pin output

    //PORTF |= (1<<PF0);      // LED uit
    //PORTF |= (1<<PF1);     // LED aan

    //TM1637_init(1/*enable*/, 3/*brightness*/);
    //TM1637_display_colon(0);        //Colons display uitzetten
}

int main(void)
{
    init();

    int rotary_1 = 0;
    int waarde_rot_1 = 50;
    int waarde_rot_2 = 50;

    while(1)
    {
        rotary_1 = RotaryGetStatus();
        if(rotary_1 == 1)
        {
            rotary_1 = 0;
            waarde_rot_1 = waarde_rot_1 -1;
            RotaryResetStatus();
        }
        if(rotary_1 == 2)
        {
            rotary_1 = 0;
            waarde_rot_1 = waarde_rot_1 + 1;
            RotaryResetStatus();
        }



        if(waarde_rot_1 == waarde_rot_2)
        {
            MotorLinks_set_percentage(0);
            MotorRechts_set_percentage(0);
            //PORTF |= (1<<PF0);      // LED uit
            //PORTF |= (1<<PF1);      // LED uit
        }
        //linksom draaien
        if(waarde_rot_1 > waarde_rot_2)
        {
            MotorLinks_set_percentage(60+(waarde_rot_2-waarde_rot_1));
            MotorRechts_set_percentage(80-(waarde_rot_2-waarde_rot_1));
            //PORTF |= (1<<PF0);      // LED uit
            //PORTF &= ~(1<<PF1);     // LED aan
        }
        //rechtsom draaien
        if(waarde_rot_1 < waarde_rot_2)
        {
            MotorLinks_set_percentage(70+(waarde_rot_2-waarde_rot_1));
            MotorRechts_set_percentage(70-(waarde_rot_2-waarde_rot_1));
            //PORTF &= ~(1<<PF0);     // LED aan
            //PORTF |= (1<<PF1);      // LED uit
        }
    }
    return 0;
}
