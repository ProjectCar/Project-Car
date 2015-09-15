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

#include "motor_management/servo_lib.h"                             //Include Servo Library

int main(void)
{
    init_servo_1();                                                 //Initalize our Servo1

    while(1)
    {        
        for(int x = 0; x < 180; x++)
        {
            control_servo_1(x);                                     //Set the Servo1 to Position x
			_delay_ms(500);
        }

        _delay_ms(1000);

        for(int x = 180; x > 0; x--)
        {
            control_servo_1(x);                                     //Set the Servo1 to Position x
			_delay_ms(500);        
        }
        
		
        _delay_ms(1000);
    }
}