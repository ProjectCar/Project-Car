/**
\file     motor_management/servo_lib.c
\ingroup  servo_lib
\brief    Implementation of the Servo control.
\author   Benjamin Marty
*/

#define F_CPU 8000000UL                                             //Set Clock for delay to 8MHz

#include <avr/io.h>                                                 //Include AVR Library to get the nice Bit definitions
#include "util/delay.h"                                             //Delay Function

void init_servo_1( void )
{
    DDRB |= (1<<DDB1);                                              //Set PB1 (OC1A) to output
    DDRC |= ( (1<<DDC0) | (1<<DDC1) | (1<<DDC2) );                  //SET PC0-PC2 (Debug LED's) to output
    
    TCCR1A |= ( (1<<COM1A1) |  (1<<WGM11) );                        //Set Compare to Fast PWM with match to clear and bottom to set    
    TCCR1B |= ( (1<<WGM12) | (1<<WGM13) | (1<<CS11) );              //Set Clock Dividor to 1024    

    OCR1A = 0x08FF;

    ICR1 = 0x5000;													//Top of Counter

    PORTC |= ( (1<<DDC0) | (1<<DDC1) | (1<<DDC2) );                 //Signalize successfull bootup with all LED's
    _delay_ms(50);
    PORTC &= ~( (1<<DDC0) | (1<<DDC1) | (1<<DDC2) );
}

void control_servo_1( char grad )
{
    int CAL = 0;

    PORTC |= (1<<DDC1);                                             //Set orange LED to show that there is something happening

    CAL = 0x08FF - 0x01FF;                                          //Calibration Value. If you have Issues with this Parameter please consult Bmarty for a solution

    OCR1A = ((CAL/180)*grad) + 0x01FF;                              //Calculate the Value for the Timer based on the given °                                  

    PORTC &= ~(1<<DDC1);                                            //Turn off LED
}

void init_motor_1( void )
{
    DDRB |= (1<<DDB2);                                              //Set PB2 (OC1B) to output
    DDRC |= ( (1<<DDC0) | (1<<DDC1) | (1<<DDC2) );                  //SET PC0-PC2 (Debug LED's) to output
    
    TCCR1A |= ( (1<<COM1B1) |  (1<<WGM11) );                        //Set Compare to Fast PWM with match to clear and bottom to set    
    TCCR1B |= ( (1<<WGM12) | (1<<WGM13) | (1<<CS11) );              //Set Clock Dividor to 1024    

    OCR1B = 0x08FF;

    ICR1 = 0x5000;													//Top of Counter

    PORTC |= ( (1<<DDC0) | (1<<DDC1) | (1<<DDC2) );                 //Signalize successfull bootup with all LED's
    _delay_ms(300);
    PORTC &= ~( (1<<DDC0) | (1<<DDC1) | (1<<DDC2) );
    _delay_ms(300);
}

void control_motor_1( int power )
{
    int CAL = 0;
	double temp = 0;

    PORTC |= (1<<DDC1);                                             //Set orange LED to show that there is something happening

    CAL = 0x08FF + 100 - 0x01FF;                                          //Calibration Value. If you have Issues with this Parameter please consult Bmarty for a solution

    //OCR1B = ((CAL/180)*grad) + 0x01FF;                            //Calculate the Value for the Timer based on the given °                                  
    //temp = power + 100;
	//temp = temp * 1024/2;
	//temp = temp/100;
	//temp = temp + 1024;
	
	OCR1B = ((CAL/190)*power) + 0x01FF - 350;
	//OCR1B = temp;
	
	PORTC &= ~(1<<DDC1);                                            //Turn off LED
}