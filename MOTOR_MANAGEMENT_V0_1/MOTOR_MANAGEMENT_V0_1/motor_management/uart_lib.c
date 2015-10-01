/**
\file     motor_management/uart_lib.c
\ingroup  uart_lib
\brief    Implementation of the Uart Module.
\author   Benjamin Marty
*/

#define F_CPU 8000000UL                                             //Set Clock for delay to 8MHz

#include <avr/io.h>                                                 //Include AVR Library to get the nice Bit definitions
#include "util/delay.h"                                             //Delay Function

void init_uart( unsigned int ubrr )
{
    /*Set baud rate */
    UBRR0H = (unsigned char)(ubrr>>8);
    UBRR0L = (unsigned char)ubrr;
    /* Enable receiver and transmitter */
    UCSR0B = (1<<RXEN0)|(1<<TXEN0);
    /* Set frame format: 8data, 2stop bit */
    UCSR0C = (1<<USBS0)|(3<<UCSZ00);
}

void transmit_uart( unsigned char data )
{
    /* Wait for empty transmit buffer */
    while ( !( UCSR0A & (1<<UDRE0)) )
    ;
    /* Put data into buffer, sends the data */
    UDR0 = data;
}

void transmit_uart_string(char s[])
{
    int i = 0;

    while (s[i] != 0x00)
    {
        transmit_uart(s[i]);
        i++;
    }
}

unsigned char receive_uart( void )
{
    /* Wait for data to be received */
    while ( !(UCSR0A & (1<<RXC0)) );
    /* Get and return received data from buffer */
    return UDR0;
}