/*
state legenda:

0: blijf helemaal stilstaan
1: blijf rechtdoor rijden
2: stilstaan bij plantenbak daarna naar state 1
3: stilstaan bij plantenbak daarna bocht linksom
4: stilstaana bij plantenbak daarna bocht rechtsom
99: stilstaan bij de LAATSTE PLANTENBAK van het LAATSTE PAD
 */
jemeodern heeeft een aalzak
#include <avr/io.h>
#include <avr/delay.h>
#include <avr/interrupt.h>

#include "rotary.h"

#define Led_Links_AAN  (PORTB &= ~(1<<PB6))
#define Led_Links_UIT  (PORTB |= (1<<PB6))
#define Led_Rechts_AAN (PORTB &= ~(1<<PB5))
#define Led_Rechts_UIT (PORTB |= (1<<PB5))
#define Links   1
#define Rechts  2
#define waarschuwLampjes_UIT    (PORTB &= ~(1<<PB3))
#define waarschuwLampjes_AAN    (PORTB |= (1<<PB3))
#define startKnopIngedrukt  ((PINA & (1<<PA6)) !=0)
#define startKnopNietGedrukt    ((PINA & (1<<PA6)) ==0)


static volatile int state = 0;
static volatile int teller = 0;
static volatile int plantTeller = 0;
static volatile int laatsteBocht = Rechts;

void init_Interrupt (void)
{
    sei();
    EIMSK |= (1<<INT0);
    EICRA |= (1<<ISC01) | (0<<ISC00);
    EIMSK |= (1<<INT1);
    EICRA |= (1<<ISC11) | (0<<ISC10);
}

ISR(INT1_vect)
{
    _delay_ms(40);
    teller = 0;
    if (plantTeller < 7)
    {
        state = 2;  //Rechtdoor rijden
    }
    if(plantTeller==7)
    {
        state = 3;  //Bocht linksom
    }
    if (plantTeller > 15 && plantTeller < 31)
    {
        state = 2;  //Rechtdoor rijden
    }
    if (plantTeller == 31)
    {
        state = 99;
    }
}

ISR(INT0_vect)
{
    _delay_ms(40);
    teller = 0;
    if (plantTeller > 3 && plantTeller < 31)
    {
        state = 2;  //Rechtdoor rijden
    }
    if (plantTeller==15)
    {
        state = 4;  //Bocht rechtsom
    }
    if (plantTeller == 31)
    {
        state = 99;
    }
}

void init (void)
{
    initTimer();
    RotaryInit();
    init_Interrupt();
    init_MotorLinks();
    init_MotorRechts();

    Timer1_Start();

    DDRB |= (1<<PB6);       // LED pin output
    DDRB |= (1<<PB5);       // LED pin output
    DDRB |= (1<<PB3);       // LED pin output

    PORTB |= (1<<PB6);      // LED off
    PORTB |= (1<<PB5);      // LED off
    waarschuwLampjes_UIT;
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
    int timer = 0;
    int knopje_1 = 1;

    int rotary_1 = 0;
    int waarde_rot_1 = 50;
    int waarde_rot_2 = 50;
    int knopIngedrukt = 0;

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
        if (startKnopNietGedrukt)
        {
            if (knopIngedrukt == 0)
            {
                _delay_ms(20);
                knopIngedrukt = 1;
            }
        }
        if (startKnopIngedrukt)
        {
            if (knopIngedrukt == 1)
            {
                _delay_ms(20);
                waarschuwLampjes_AAN;
                _delay_ms(2000);
                state = 1;
                waarschuwLampjes_UIT;
                knopIngedrukt = 0;
            }
        }
        //Rotary encoder code
        rotary_1 = RotaryGetStatus();       //rotary_1 krijgt de waarde 0 1 of 2 van de rotary encoder uit rotary.c
        if(rotary_1 == 1)
        {
            rotary_1 = 0;
            waarde_rot_1--;
            RotaryResetStatus();
        }
        if(rotary_1 == 2)
        {
            rotary_1 = 0;
            waarde_rot_1++;
            RotaryResetStatus();
        }

        //Timer code
        if (TIFR5 & (1<<TOV5))  //elke keer als de timer overflowt
        {
            // Timer overflow, +/- 1 ms voorbij
            TIFR5 = (1<<TOV5);  // Wis flag
            teller++;           //Verhoog teller met 1 iedere overflow (244Hz)
        }

        // in deze state stopt AGV hier staat deze permanent uit
        if(state == 0)
        {
            MotorLinks_set_percentage(0);
            MotorRechts_set_percentage(0);
        }
        //Blijf rechtdoor rijden tot external interrupt state veranderd naar state 2
        if(state==1)
        {
            MotorLinks_set_percentage(57+(waarde_rot_2-waarde_rot_1));
            MotorRechts_set_percentage(70-(waarde_rot_2-waarde_rot_1));
        }
        //wacht 2 seconden en rijdt door door te veranderen naar state 1
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
            waarschuwLampjes_AAN;
            if (teller > (2000/4))            //Hoeveel ms/4 het duurt voordat het naar de volgende staat gaat
            {
                plantTeller++;
                state = 5;
                teller = 0;
                waarschuwLampjes_UIT;
            }
        }
        if(state==4)    //Bocht Rechtsom
        {
            MotorLinks_set_percentage(0);
            MotorRechts_set_percentage(0);
            waarschuwLampjes_AAN;
            if (teller > (2000/4))            // hoeveel seconden wachten voordat state veranderd
            {
                plantTeller++;
                state = 9;
                teller = 0;
                waarschuwLampjes_UIT;
            }
        }
        if(state==5)
        {
            MotorLinks_set_percentage(60);
            MotorRechts_set_percentage(70);
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
            MotorRechts_set_percentage(80);
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
            MotorLinks_set_percentage(60);
            MotorRechts_set_percentage(70);
            if (teller > (1100/4))            // hoeveel seconden wachten voordat state veranderd
            {
                state = 8;
                teller = 0;
            }
        }
        //draai linksom volle snelheid
        if(state==8)
        {
            MotorLinks_set_percentage(0);
            MotorRechts_set_percentage(80);
            if (teller > (1400/4))            // hoeveel seconden wachten voordat state veranderd
            {
                state = 1;
                teller = 0;
                laatsteBocht = Links;
            }
        }
        if(state==9)
        {
            MotorLinks_set_percentage(60);
            MotorRechts_set_percentage(70);
            if (teller > (3000/4))            // hoeveel seconden wachten voordat state veranderd
            {
                state = 10;
                teller = 0;
            }
        }
        if(state==10)
        {
            MotorLinks_set_percentage(60);
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
            MotorLinks_set_percentage(60);
            MotorRechts_set_percentage(70);
            if (teller > (1400/4))            // hoeveel seconden wachten voordat state veranderd
            {
                state = 12;
                teller = 0;
            }
        }
        //draai rechtsom volle snelheid
        if(state==12)
        {
            MotorLinks_set_percentage(80);
            MotorRechts_set_percentage(0);
            if (teller > (1100/4))            // hoeveel seconden wachten voordat state veranderd
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
            plantTeller = 100;
            if (teller > (2000/4))
            {
                state = 100;
                teller = 0;
            }
        }
        if(state==100)
        {
            MotorLinks_set_percentage(60);
            MotorRechts_set_percentage(70);
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
