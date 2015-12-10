/**
* file     main.c
* ingroup  g_applspec
* brief    Main file for Energy Management.
* author   Benjamin Marty
*
* This file is the place where the basic structure of the application/module is defined.
*/


#define F_CPU 8000000UL     //Set Clock for delay to 8MHz

#define BAUD 9600
#define MYUBRR F_CPU/16/BAUD-1

#include "util/delay.h"     //Delay Function
#include <avr/io.h>         //Include AVR Library to get the nice Bit definitions

#define ON 1
#define OFF 0


/**
* inits the general powermanagement
*
* @return Voltage
*/
void powermanagement_init()
{
	DDRD |= (1<<PORTD4);                 //Set tristate for ENABLE_MCU
	DDRB |= (1<<PORTB0);                 //Set tristate for CRUISE_CONTROL
}

/**
* inits the adc with the corresponding channel
*
* @param Channel
*/
void adc_init(int channel)
{
	ADMUX = 0x00;                        //Reset ADMUX register to 0
	ADCSRA = 0x00;                       //Reset ADCSRA register to 0
	
	ADMUX |= (1<<REFS0);                 //Set ref to AVCC
	int channel_bit = (channel);
	//ADMUX |= (channel << 4);           //Set channel
	
	ADMUX |= channel_bit;
	
	ADCSRA |= ( (1<<ADEN) | (1<<ADPS1) | (1<<ADPS0) | (1<<ADATE) );   //Enable ADC, Star ADC, Set Clock Prescaler to 8
	ADCSRA |= (1<<ADSC);
	
	//return 1;
}

/**
* make a sample on the adc
*
* @return ADC-Value
*/
int adc_sample()
{
	int result = 0;                      //Setup space for ADC result
	
	result = ADCL + (ADCH << 8);         //Get 10bit result from ADC registers. Keep in mind to read ADCH!
	
	return result;
}

/**
* Returns the actual voltage on the five Volt rail
*
* @return Voltage
*/
float get_five_rail()
{
	adc_init(0);
	_delay_ms(100);
	
	float meassurement = (((float)adc_sample()/1023)*3.3);
	
	meassurement = (meassurement/2.5)*5.2;
	
	return meassurement;
}

/**
* Returns the actual voltage on the three point three Volt rail
*
* @return Voltage
*/
float get_three_rail()
{
	adc_init(1);
	_delay_ms(100);
	
	float meassurement = (((float)adc_sample()/1023)*3.3);
	
	meassurement = (meassurement/2.5)*3.4;
	
	return meassurement;
}

/**
* Returns the actual current consumption (Not yet implemented!!)
*
* @return Current
*/
float get_current()
{
	adc_init(2);
	_delay_ms(100);
	
	float meassurement = (((float)adc_sample()/1023)*3.3);
	
	meassurement = (meassurement/2.5)*0;
	
	return meassurement;
}

/**
* Returns the actual lipo voltage
*
* @return Voltage
*/
float get_lipo()
{
	adc_init(3);
	_delay_ms(100);
	
	float meassurement = (((float)adc_sample()/1023)*3.3);
	
	meassurement = (meassurement/2.5)*12.4;
	
	return meassurement;
}

/**
* Init USART interface
*
*/
char usart_init( unsigned int ubrr)
{
	
	UBRR0H = (unsigned char)(ubrr>>8);   //Set baud rate
	UBRR0L = (unsigned char)ubrr;
	
	UCSR0B = (1<<RXEN0)|(1<<TXEN0);      //Enable receiver and transmitter
	
	UCSR0C = (1<<USBS0)|(3<<UCSZ00);     //Set frame format: 8data, 2stop bit
	
	return 1;
}

/**
* Transmits given data over USART
*
* @param Data
*/
void USART_Transmit( unsigned char data )
{
	
	while ( !( UCSR0A & (1<<UDRE0)) );   //Wait for empty transmit buffer
	
	
	UDR0 = data;                         //Put data into buffer, sends the data
	
	//return 1;
}

/**
* Power control
*
* @param on or off
*/
void power_control(char state)
{
	if(state == ON)
	{
		PORTD &= ~(1<<PORTD4);
		
	}
	else if(state == OFF)
	{
		PORTD |= (1<<PORTD4);
	}
	
	//return 1;
}

/**
* CruiseControl control
*
* @param on or off
*/
void cc_control(char state)
{
	if(state == ON)
	{
		PORTB |= (1<<PORTB0);
		
	}
	else if(state == OFF)
	{
		PORTB &= ~(1<<PORTB0);
	}
	
	//return 1;
}

/**
* TWI Init
*
*/
void twi_init(void)
{
	//TWI Init
	TWAR = ( 0x0A << 1 );
	//TWAR |= 0x01;
	TWCR = ( (1<<TWEN) | (1<<TWEA) );
}

int main(void)
{
	powermanagement_init();
	
	char power_status = 1;
	char cc_status = 1;
	
	float five_rail = 0;
	float three_rail = 0;
	float current = 0;
	float lipo = 0;
	
	int data = 0;
	
	while(1)
	{
		DDRD |= ( (1<<PORTD5) | (1<<PORTD6) | (1<<PORTD7) );
		power_control(power_status);
		cc_control(cc_status);
		
		five_rail = get_five_rail();
		three_rail = get_three_rail();
		current = get_current();
		lipo = get_lipo();

      twi_init();
		
		switch(TWSR)        //TWDR    Unknown identifier    Error
		{
			case 0x60:
				TWCR |= ( (1<<TWINT) | (1<<TWEA) );
				break;
			
			case 0x80:
				data = TWDR;
				TWCR |= ( (1<<TWINT) | (1<<TWEA) );
				PORTD |= ( (1<<PORTD5) | (1<<PORTD6) | (1<<PORTD7) );
				_delay_ms(50);
				PORTD &= ~( (1<<PORTD5) | (1<<PORTD6) | (1<<PORTD7) );
				break;
			
			case 0xA0:            /* Received Stop or Repeated Start while still addressed */
				TWCR |= ( (1<<TWINT) );                            /* Switch to not Addressed */
				break;
			
			default:
				break;
		}
	}
}