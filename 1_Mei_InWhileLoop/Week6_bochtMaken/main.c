/*
 */

#include <avr/io.h>
#include <avr/delay.h>
#define Led_Links_AAN  (PORTB &= ~(1<<PB6))
#define Led_Links_UIT  (PORTB |= (1<<PB6))
#define Led_Rechts_AAN (PORTB &= ~(1<<PB5))
#define Led_Rechts_UIT (PORTB |= (1<<PB5))


void init (void)
{
    init_MotorLinks();
    init_MotorRechts();
    DDRB |= (1<<PB6);     // LED pin output
    PORTB |= (1<<PB6);      // LED off
    DDRB |= (1<<PB5);     // LED pin output
    PORTB |= (1<<PB5);      // LED off
    Led_Links_AAN;
    Led_Rechts_AAN;
}

void initTimer (void)
{
    // Timer 5 is de systeemtimer (overflow)
    TCCR5A = 0;

    // Timer 8 kan niet ver genoeg tellen!
    // 8 bit timer 16000000 Hz / 64 / 256 = 976.56 Hz
    // 16 bit timer 16000000 Hz / 64 / 65.536 = 3.906,25 Hz
    // 16 bit timer 16000000 Hz / 65.536 = 244 Hz
    // start timer 0, prescaler = 64
    TCCR5B = TCCR5B | (0 << CS51) | (1 << CS50); //244,14 Hz
}

int main(void)
{
    init();
    initTimer();
    int state = 0;
    int teller = 0;
    int timer = 0;
    int knopje_1 = 1;

    while(1)
    {

        if (TIFR5 & (1<<TOV5))  //elke keer als de timer overflowt
        {
            // Timer overflow, +/- 1 ms voorbij
            TIFR5 = (1<<TOV5);  // Wis flag
            teller++;           //Verhoog teller met 1 iedere overflow (244Hz)
        }
        //rijd rechtdoor (50%vermogen)
        if(state==0 && knopje_1)        //als state = 0 en de startknop is ingedrukt
        {
            Led_Links_AAN;
            Led_Rechts_AAN;
            MotorLinks_set_percentage(45);
            MotorRechts_set_percentage(50);
            if (teller > (5000/4))            //Hoeveel ms/4 het duurt voordat het naar de volgende staat gaat
            {
                state = 1;
                teller = 0;
            }
        }
        //draai linksom volle snelheid
        if(state==1)
        {
            Led_Links_UIT;
            Led_Rechts_AAN;
            MotorLinks_set_percentage(-50);
            MotorRechts_set_percentage(50);
            if (teller > (2000/4))            // hoeveel seconden wachten voordat state veranderd
            {
                state = 2;
                teller = 0;
            }
        }
        //rijd rechtdoor (50%vermogen)
        if(state==2)
        {
            Led_Links_AAN;
            Led_Rechts_AAN;
            MotorLinks_set_percentage(50);
            MotorRechts_set_percentage(50);
            if (teller > (5000/4))            // hoeveel seconden wachten voordat state veranderd
            {
                state = 3;
                teller = 0;
            }
        }
        //draai rechtsom volle snelheid
        if(state==3)
        {
            Led_Links_AAN;
            Led_Rechts_UIT;
            MotorLinks_set_percentage(-100);
            MotorRechts_set_percentage(100);
            if (teller > (2000/4))            // hoeveel seconden wachten voordat state veranderd
            {
                state = 0;
                teller = 0;
            }
        }
    }

    return 0;
}
