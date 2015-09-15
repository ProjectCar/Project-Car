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
	DDRD |= (1<<DDD5);                                            	//Set PD5 (OC0B) to output
	DDRC |= ( (1<<DDC0) | (1<<DDC1) | (1<<DDC2) );                  //SET PC0-PC2 (Debug LED's) to output

	TCCR0A |= ( (1<<COM0B1) | (1<<WGM00) | (1<<WGM01) );           	//Set Compare to Fast PWM with match to clear and bottom to set
	TCCR0A &= ~(1<<COM0B0);
	
	TCCR0B |= ( (1<<CS02)  );                                      	//Set Clock Dividor to 1024	
	TCCR0B &= ~( (1<<CS01) | (1<<CS00) );
	
	OCR0B = 15;                                                     //Set servo to 0°
	
	PORTC |= ( (1<<DDC0) | (1<<DDC1) | (1<<DDC2) );                 //Signalize successfull bootup with all LED's
	_delay_ms(300);
	PORTC &= ~( (1<<DDC0) | (1<<DDC1) | (1<<DDC2) );
	_delay_ms(300);
}

//Function excepts a Value between 0° and 180°
void control_servo_1( char grad )
{
	PORTC |= (1<<DDC1);                                             //Set orange LED to show that there is something happening
	
	OCR0B = (grad/3) + 15;                                          //Calculate the Value for the Timer based on the given °                                  
	
	PORTC &= ~(1<<DDC1);                                            //Turn off LED
}