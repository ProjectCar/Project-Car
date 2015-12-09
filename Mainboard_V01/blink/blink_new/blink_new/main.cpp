/*
 * blink_new.cpp
 *
 * Created: 09.12.2015 12:55:31
 * Author : bmarty
 */ 

#include <avr/io.h>


int main(void)
{
    /* Replace with your application code */
    while (1) 
    {
		int test = 0x00;
		DDRF |= ( (1<<PORTF0) );
		PORTF |= (1<<PORTF0); 
		PORTF &= ~(1<<PORTF0);
	}
}

