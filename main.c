/*
 */

#include <avr/io.h>
#include <avr/delay.h>
#include <avr/interrupt.h>
#define Led_Links_AAN  (PORTB &= ~(1<<PB6))
#define Led_Links_UIT  (PORTB |= (1<<PB6))
#define Led_Rechts_AAN (PORTB &= ~(1<<PB5))
#define Led_Rechts_UIT (PORTB |= (1<<PB5))
#define Links   1
#define Rechts  2
#define waarschuwLampjes_AAN    (PORTL &= ~(1<<PL4))
#define waarschuwLampjes_UIT    (PORTL |= (1<<PL4))


static volatile int state = 1;
static volatile int teller = 0;
static volatile int plantTeller = 7;
static volatile int laatsteBocht = Rechts;

void init_Interrupt (void)
{
    sei();
    EIMSK |= (1<<INT0);
    EICRA |= (1<<ISC01) | (0<<ISC00);
    EIMSK |= (1<<INT1);
    EICRA |= (1<<ISC11) | (0<<ISC10);
}

ISR(INT0_vect)
{
    _delay_ms(40);
    teller = 0;
    if (plantTeller < 3)
    {
        state = 2;  //Rechtdoor rijden
    }
    if(plantTeller==3)
    {
        state = 3;  //Bocht linksom
    }
    if (plantTeller > 7 && plantTeller < 15)
    {
        state = 2;  //Rechtdoor rijden
    }
    if (plantTeller == 15)
    {
        state = 99;
    }
}

ISR(INT1_vect)
{
    _delay_ms(40);
    teller = 0;
    if (plantTeller > 3 && plantTeller < 15)
    {
        state = 2;  //Rechtdoor rijden
    }
    if (plantTeller==7)
    {
        state = 4;  //Bocht rechtsom
    }
    if (plantTeller == 15)
    {
        state = 99;
    }
}

void init (void)
{
    init_Interrupt();
    init_MotorLinks();
    init_MotorRechts();
    DDRB |= (1<<PB6);     // LED pin output
    PORTB |= (1<<PB6);      // LED off
    DDRB |= (1<<PB5);     // LED pin output
    PORTB |= (1<<PB5);      // LED off
    DDRL |= (1<<PL4);
    PORTL |= (1<<PL4);
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
    int timer = 0;
    int knopje_1 = 1;
    waarschuwLampjes_AAN;

    _delay_ms(4000);
    /*
        state uitleg:
            State == 0: AGV blijft stilstaat
            State == 1: AGV Rijdt rechtdoor
            State == 2: AGV Staat 2 sec stil en daarna gaat hij naar state 1
            State == 3: Bocht Links maken
            State == 4: Bocht Recht smaken
    */
    while(1)
    {
        //Timer code
        if (TIFR5 & (1<<TOV5))  //elke keer als de timer overflowt
        {
            // Timer overflow, +/- 1 ms voorbij
            TIFR5 = (1<<TOV5);  // Wis flag
            teller++;           //Verhoog teller met 1 iedere overflow (244Hz)
        }
        //rijd rechtdoor (50%vermogen)
        if(state == 0)
        {
            MotorLinks_set_percentage(0);
            MotorRechts_set_percentage(0);
        }
        if(state==1)
        {
            MotorLinks_set_percentage(35);
            MotorRechts_set_percentage(40);
        }
        if(state==2)
        {
            MotorLinks_set_percentage(0);
            MotorRechts_set_percentage(0);
            waarschuwLampjes_AAN;
            if (teller > (2000/4))
            {
                plantTeller++;
                state = 1;
                waarschuwLampjes_UIT;
            }
        }
        if(state==3)    //Bocht Linksom
        {
            MotorLinks_set_percentage(0);
            MotorRechts_set_percentage(0);
            if (teller > (2000/4))            //Hoeveel ms/4 het duurt voordat het naar de volgende staat gaat
            {
                plantTeller++;
                state = 5;
                teller = 0;
            }
        }
        if(state==4)    //Bocht Rechtsom
        {
            MotorLinks_set_percentage(0);
            MotorRechts_set_percentage(0);
            if (teller > (2000/4))            // hoeveel seconden wachten voordat state veranderd
            {
                plantTeller++;
                state = 9;
                teller = 0;
            }
        }
        if(state==5)
        {
            MotorLinks_set_percentage(35);
            MotorRechts_set_percentage(40);
            if (teller > (3000/4))            //Hoeveel ms/4 het duurt voordat het naar de volgende staat gaat
            {
                state = 6;
                teller = 0;
            }
        }
        //draai linksom volle snelheid
        if(state==6)
        {
            MotorLinks_set_percentage(0);
            MotorRechts_set_percentage(50);
            if (teller > (1450/4))            // hoeveel seconden wachten voordat state veranderd
            {
                state = 7;
                teller = 0;
            }
        }
        //rijd rechtdoor (40%vermogen)
        if(state==7)
        {
            Led_Links_AAN;
            Led_Rechts_AAN;
            MotorLinks_set_percentage(35);
            MotorRechts_set_percentage(40);
            if (teller > (1400/4))            // hoeveel seconden wachten voordat state veranderd
            {
                state = 8;
                teller = 0;
            }
        }
        //draai linksom volle snelheid
        if(state==8)
        {
            MotorLinks_set_percentage(0);
            MotorRechts_set_percentage(50);
            if (teller > (1400/4))            // hoeveel seconden wachten voordat state veranderd
            {
                state = 1;
                teller = 0;
                laatsteBocht = Links;
            }
        }
        if(state==9)
        {
            MotorLinks_set_percentage(35);
            MotorRechts_set_percentage(40);
            if (teller > (3000/4))            // hoeveel seconden wachten voordat state veranderd
            {
                state = 10;
                teller = 0;
            }
        }
        if(state==10)
        {
            MotorLinks_set_percentage(50);
            MotorRechts_set_percentage(0);
            if (teller > (1450/4))
            {
                state = 11;
                teller = 0;
            }
        }
        //rijd rechtdoor (40%vermogen)
        if(state==11)
        {
            Led_Links_AAN;
            Led_Rechts_AAN;
            MotorLinks_set_percentage(35);
            MotorRechts_set_percentage(40);
            if (teller > (1400/4))            // hoeveel seconden wachten voordat state veranderd
            {
                state = 12;
                teller = 0;
            }
        }
        //draai rechtsom volle snelheid
        if(state==12)
        {
            MotorLinks_set_percentage(50);
            MotorRechts_set_percentage(0);
            if (teller > (1400/4))            // hoeveel seconden wachten voordat state veranderd
            {
                state = 1;
                teller = 0;
                laatsteBocht = Rechts;
            }
        }
        if(state==99)
        {
            MotorLinks_set_percentage(0);
            MotorRechts_set_percentage(0);
            if (teller > (2000/4))
            {
                state = 100;
                teller = 0;
            }
        }
        if(state==100)
        {
            MotorLinks_set_percentage(35);
            MotorRechts_set_percentage(40);
            if (teller > (2000/4))
            {
                state = 101;
                teller = 0;
            }
        }
        if(state==101)
        {
            MotorLinks_set_percentage(0);
            MotorRechts_set_percentage(0);
            waarschuwLampjes_AAN;
        }
    }
    return 0;
}
