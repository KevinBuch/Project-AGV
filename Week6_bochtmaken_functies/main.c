/*
 */

#include <avr/io.h>
#include <avr/delay.h>
#define Led_Links_AAN  (PORTB |= (1<<PB7))
#define Led_Links_UIT  (PORTB &= ~(1<<PB7))
//#define Led_Rechts_AAN (PORTB &= ~(1<<PB5))
//#define Led_Rechts_UIT (PORTB |= (1<<PB5))

static volatile int teller = 0;
//int teller = 0;

volatile int state = 0;

void init (void)
{
    init_MotorLinks();
    init_MotorRechts();
    initTimer();
    DDRB |= (1<<PB7);     // LED pin output
    PORTB &= ~(1<<PB7);      // LED off
    //DDRB |= (1<<PB5);     // LED pin output
    //PORTB &= ~(1<<PB5);      // LED off
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

void timer()
{
    if (TIFR5 & (1<<TOV5))  //elke keer als de timer overflowt
    {
        // Timer overflow, +/- 1 ms voorbij
        TIFR5 = (1<<TOV5);  // Wis flag
        teller++;           //Verhoog teller met 1 iedere overflow (244Hz)
    }
}

void rechtdoor(int tijd)
{
    Led_Links_UIT;
    MotorLinks_set_percentage(35);
    MotorRechts_set_percentage(40);
    if(teller > (tijd/4))
    {
        MotorLinks_set_percentage(0);
        MotorRechts_set_percentage(0);
        state++;
        teller = 0;
    }
}

void Bocht_Links(void)
{
    static int i = 0;
    //wielen uit
    if (i==0)
    {
        MotorLinks_set_percentage(0);
        MotorRechts_set_percentage(0);
        if (teller > (1450/4))          // hoeveel seconden wachten voordat state veranderd
        {
            i=1;
            teller = 0;
        }
    }
    //draai linksom
    if (i==1)
    {
        Led_Links_AAN;
        MotorLinks_set_percentage(0);
        MotorRechts_set_percentage(40);
        if (teller > (1450/4))          // hoeveel seconden wachten voordat state veranderd
        {
            i=2;
            teller = 0;
        }
    }

    //rijdt rechtdoor
    if (i==2)
    {
        MotorLinks_set_percentage(35);
        MotorRechts_set_percentage(40);
        if (teller > (1400/4))                  // hoeveel seconden wachten voordat state veranderd
        {
            i=3;
            teller = 0;
        }
    }

    //draai linksom
    if (i==3)
    {
        MotorLinks_set_percentage(0);
        MotorRechts_set_percentage(40);

        if (teller > (1400/4))            // hoeveel seconden wachten voordat state veranderd
        {
            i=4;
            teller = 0;
        }
    }
    if(i==4)
    {
        i=0;
        MotorLinks_set_percentage(0);
        MotorRechts_set_percentage(0);
        state++;
        teller = 0;
    }
}

void Bocht_Rechts(void)
{
    static int i = 0;
    //wielen uit
    if (i==0)
    {
        MotorLinks_set_percentage(0);
        MotorRechts_set_percentage(0);
        if (teller > (1450/4))          // hoeveel seconden wachten voordat state veranderd
        {
            i=1;
            teller = 0;
        }
    }

    //draai rechtsom
    if (i==1)
    {
        MotorLinks_set_percentage(40);
        MotorRechts_set_percentage(0);
        if (teller > (1450/4))          // hoeveel seconden wachten voordat state veranderd
        {
            i=2;
            teller = 0;
        }
    }

    //rijdt rechtdoor
    if (i==2)
    {
        MotorLinks_set_percentage(40);
        MotorRechts_set_percentage(35);
        if (teller > (1400/4))                  // hoeveel seconden wachten voordat state veranderd
        {
            i=3;
            teller = 0;
        }
    }
    //draai rechtsom
    if (i==3)
    {
        MotorLinks_set_percentage(40);
        MotorRechts_set_percentage(0);
        if (teller > (1400/4))            // hoeveel seconden wachten voordat state veranderd
        {
            i=4;
            teller = 0;
        }
    }
    if(i==4)
    {
        i=0;
        MotorLinks_set_percentage(0);
        MotorRechts_set_percentage(0);
        state++;
        teller = 0;
    }
}

int main(void)
{
    init();
    int knopje_1 = 1;

    _delay_ms(500);            //0,5 seconden wachten voor beginnen met rijden
    while(1)
    {
        timer();
        if (state == 0)
        {
            rechtdoor(2000);
        }
        if (state == 1)
        {
            Bocht_Links();
        }
        if (state == 2)
        {
            rechtdoor(5000);
        }
        if (state == 3)
        {
            Bocht_Rechts();
        }

    }
    return 0;
}
