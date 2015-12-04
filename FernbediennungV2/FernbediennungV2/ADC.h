/*
 * ADC.h
 *
 * Created: 18.11.2015 08:03:58
 *  Author: Severin
 */ 

#include <avr\io.h>


#ifndef ADC_H_
#define ADC_H_


void InitADC(void);
unsigned int ReadADC(unsigned char ADCchannel);	//ADC2/3  &6/7 (x/y)


void InitADC()
{
	ADMUX |= (1<<REFS0);  // Select Vref=AVcc
	ADCSRA |= (1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0)|(1<<ADEN); //set prescaller to 128 and enable ADC
}

unsigned int ReadADC(unsigned char ADCchannel)	
{
	ADMUX = (ADMUX & 0xF0) | (ADCchannel & 0x0F);  //select ADC channel with safety mask
	ADCSRA |= (1<<ADSC);  //single conversion mode
	while( ADCSRA & (1<<ADSC) );  return ADC; // wait until ADC conversion is complete
}



#endif /* ADC_H_ */