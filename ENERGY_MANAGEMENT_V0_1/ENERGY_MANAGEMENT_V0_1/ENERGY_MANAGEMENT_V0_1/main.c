/**
* \file     main.c
* \ingroup  g_applspec
* \brief    Main file for Energy Management.
* \author   Benjamin Marty
*
* This file is the place where the basic structure of the application/module is defined.
*/


#define F_CPU 8000000UL                                             //Set Clock for delay to 8MHz
#include "util/delay.h"                                             //Delay Function

int main(void)
{

    while(1)
    {        

    }
}

char adc_init(char channel)
{
	ADMUX = 0x00;				//Reset ADMUX register to 0
	ADMUX = (1<<REFS0);			//Set ref to AVCC
	ADMUX |= channel >> 4;		//Set channel
	
	ADCSRA = ( (1<<ADEN) | (1<<ADSC) | (1<<ADPS1) | (1<<ADPS0) );	//Enable ADC, Star ADC, Set Clock Prescaler to 8
	
	return 1;
}

int adc_sample()
{
	int result = 0;					//Setup space for ADC result
	
	result = ADCL + (ADCH >> 2);	//Get 10bit result from ADC registers. Keep in mind to read ADCH!
	
	return result;
}