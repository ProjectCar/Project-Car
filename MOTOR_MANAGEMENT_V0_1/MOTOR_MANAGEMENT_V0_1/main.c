/**
* \file     main.c
* \ingroup  g_applspec
* \brief    Main file for Motor Management.
* \author   Benjamin Marty
*
* This file is the place where the basic structure of the application/module is defined.
*/


#define F_CPU 8000000UL                                             //Set Clock for delay to 8MHz
#include "util/delay.h"                                             //Delay Function
#include <avr/io.h>                                                 //Include AVR Library to get the nice Bit definitions

#define BAUD 9600
#define MYUBRR F_CPU/16/BAUD-1

#include "motor_management/uart_lib.h"
#include "motor_management/servo_lib.h"                             //Include Servo Library

char data = 0;

/**
* TWI Init
*
*/
void twi_init(void)
{
	//TWI Init
	TWAR = ( 0x0B << 1 );
	//TWAR |= 0x01;
	TWCR = ( (1<<TWEN) | (1<<TWEA) );
}


int main(void)
{
    init_servo_1();                                                 //Initalize our Servo1
    init_motor_1();                                                 //Initalize our Motor1


	control_motor_1(128);
	control_servo_1(128);
    
	//init_uart(MYUBRR);

    //int motor = 0;                                                  //variabel
    
    //unsigned char value_uart = 0;
    
    //char str[10];

    //transmit_uart_string("Motor Controller V0.1 Ready!!");
    //transmit_uart('\r');
    //transmit_uart('\n');    
	
	int motor_data_test = 128;

	twi_init();
	
	int counter = 0;
	int data_twi[2];
	
	char servo_data;
	char motor_data;
	
	//Configure OC0A
	TCCR0A |= (1<<COM0A1);
	TCCR0A |= (1<<COM0B1);
	TCCR0A |= (1<<WGM00)|(1<<WGM01);
	
	TCCR0B |= (1<<CS01);
	
	DDRD |= (1<<PORTD5)|(1<<PORTD6);
	
	
	

    while(1)
    {        
        
		switch(TWSR)        //TWDR    Unknown identifier    Error
		{
			case 0x60:
			TWCR |= ( (1<<TWINT) | (1<<TWEA) );
			
			while(counter < 5)
			{
				counter++;
				
				if(TWSR == 0x80)
				{
					data_twi[0] = TWDR;
					TWCR |= ( (1<<TWINT) | (1<<TWEA) );
					_delay_ms(1);
					if(TWSR == 0x80)
					{
						data_twi[1] = TWDR;
						TWCR |= ( (1<<TWINT) | (1<<TWEA) );
						break;
					}
					break;
				}
				_delay_ms(1);
			}
			counter = counter;
			counter = 0;
			
			break;
			
			case 0x80:
			data = TWDR;

			
			
			TWCR |= ( (1<<TWINT) | (1<<TWEA) );
			
			PORTC |= ( (1<<DDC0) | (1<<DDC1) | (1<<DDC2) );
			_delay_ms(50);
			
			PORTC &= ~( (1<<DDC0) | (1<<DDC1) | (1<<DDC2) );
			_delay_ms(50);
			break;
			
			case 0xA0:            /* Received Stop or Repeated Start while still addressed */
			TWCR |= ( (1<<TWINT) );                            /* Switch to not Addressed */
			break;
			
			default:
			break;
		}
		
		//If servo data
		if(data_twi[0] == 0x55)	
		{
			if(data_twi[1] > 128)
			{
				control_servo_1(data_twi[1]);
			}
			
			if(data_twi[1] < 127)
			{
				control_servo_1(data_twi[1]);				
			}
			
			
			servo_data = data_twi[1];
			
			if(servo_data == 0xff)
			{		
				PORTC |= ( (1<<DDC0) );
			}
			else
			{
				PORTC &= ~( (1<<DDC0) );
			}
		}	

		//If motor data
		if(data_twi[0] == 0x66)
		{
			//If Forward
			if(data_twi[1] > 128)
			{
				OCR0A = (data_twi[1] - 128) * 2;
				//Stop Backward
				OCR0B = 0x00;
			}

			//If Backward
			if(data_twi[1] < 127)
			{
				//Stop Forward
				OCR0A = 0x00;
				OCR0B = (127 - data_twi[1]) * 2;
			}
			

		}
    }
}
