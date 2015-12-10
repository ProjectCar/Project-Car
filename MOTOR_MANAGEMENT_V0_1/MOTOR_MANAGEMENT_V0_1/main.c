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

    //init_uart(MYUBRR);

    //int motor = 0;                                                  //variabel
    
    //unsigned char value_uart = 0;
    
    //char str[10];

    //transmit_uart_string("Motor Controller V0.1 Ready!!");
    //transmit_uart('\r');
    //transmit_uart('\n');    

	twi_init();

    while(1)
    {        
        
		switch(TWSR)        //TWDR    Unknown identifier    Error
		{
			case 0x60:
			TWCR |= ( (1<<TWINT) | (1<<TWEA) );
			break;
			
			case 0x80:
			data = TWDR;
			control_motor_1(data);
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
		        
        /*
		control_motor_1(motor);                                     //Set the Motor1 to Position x
        
        value_uart = receive_uart();
        
        switch(value_uart)
        {
            case 'w':
                motor++;
                break;
            
            case 's':
                motor--;
                break;
        }
        
        sprintf(str,"%i",motor);
        
        transmit_uart_string(str);
        transmit_uart('\r');
        transmit_uart('\n');
		*/
		
        /*
        motor = 90;
        
        control_motor_1(180);                                     //Set the Motor1 to Position x
        
        test = 100;
        
        control_motor_1(10);                                     //Set the Motor1 to Position x
        
        test1 = 90;        
        */
        
        /*

        for(int x = 0; x < 90; x++)
        {
            control_servo_1(x);                                     //Set the Servo1 to Position x
            //control_motor_1(x);                                     //Set the Motor1 to Position x
            _delay_ms(10);
        }

        _delay_ms(1000);

        for(int x = 90; x < 180; x++)
        {
            control_servo_1(x);                                     //Set the Servo1 to Position x
            //control_motor_1(x);                                     //Set the Motor1 to Position x
            _delay_ms(10);
        }

        _delay_ms(1000);

        for(int x = 180; x > 0; x--)
        {
            control_servo_1(x);                                     //Set the Servo1 to Position x
            //control_motor_1(x);                                     //Set the Motor1 to Position x
            _delay_ms(10);        
        }

        _delay_ms(1000);
        
        */
    }
}
