/*
 * PortInit.h
 *
 * Created: 18.11.2015 08:45:41
 *  Author: Severin
 */ 

#include <avr/io.h>

#ifndef PORTINIT_H_
#define PORTINIT_H_

void PORT_init(void);


void PORT_init(void)
{
		DDRD=0x00; //PORTD as INPUT
		PORTD = 0xFF;
		DDRC=0x00;	//PORTC als INPUT definieren

}

#endif /* PORTINIT_H_ */