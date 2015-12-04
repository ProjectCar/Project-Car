//////////////////////////////////////////////////////////////////////////
//	Titel	:	Fernsteuerung
//	Autor	:	Severin Landolt
//	Version	:	1.0
//	Datum	:	16.11.2015
//	Funktion:	Joysticks und Schalter einlessen
//				über SPI Funkkomunikation
//				kleines OLED-Display mit TWI(I2C) ansteuern
//////////////////////////////////////////////////////////////////////////


//Includes
#define F_CPU 8000000UL		//Fuse SET: LowValue FF, HighValue D9, ExtValue FF.

#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>
#include <avr/interrupt.h>

#include "PortInit.h"
#include "Display.h"
#include "Funk.h"
#include "ADC.h"
#include "nrf24.h"

#define empfangen 0x77
#define senden 0x00

#include "car_icon.h"

int x1 = 0;
int y1 = 0;
int x2 = 0;
int y2 = 0;

char led = 0;

unsigned char button = 0;
unsigned char joystick = 0;

uint8_t temp;
uint8_t q = 0;
uint8_t data_array[6];
uint8_t tx_address[5] = {0xE7,0xE7,0xE7,0xE7,0xE7};
uint8_t rx_address[5] = {0xD7,0xD7,0xD7,0xD7,0xD7};
	
int rf_receive(void);
void transmiter(int mode);

int main(void)
{
	
	// Timer/Counter 0 initialization
	// Clock source: System Clock
	// Clock value: 15,625 kHz
	// Mode: Normal top=0xFF
	// OC0A output: Disconnected
	// OC0B output: Disconnected
	// Timer Period: 10,176 ms
	
	TCCR0A=0x00;            //(0<<COM0A1) | (0<<COM0A0) | (0<<COM0B1) | (0<<COM0B0) | (0<<WGM01) | (0<<WGM00);
	TCCR0B=0x05;            //(0<<WGM02) | (1<<CS02) | (0<<CS01) | (1<<CS00);		//CPU-TAKT/1024
	TCNT0=0x00;

	// Timer/Counter 0 Interrupt(s) initialization
	//TIMSK0=(0<<OCIE0B) | (0<<OCIE0A) | (1<<TOIE0);							// Loest Timing Error aus

	// Global enable interrupts
	sei();																		

	PORT_init();
	nrf24_init();																// Funkmodul Initialisieren
	nrf24_config(2,6);															// Den Chanel vom Funkmodul wählen und Anzahl der Byte zum übertragen angeben
	nrf24_tx_address(tx_address);												// Sendeadresse festlegen ( Muss mit empfängeradresse(rx) des anden geräts übereinstimmen)
	nrf24_rx_address(rx_address);												// Empfangsadresse festlegen ( Muss mit sendeadresse(tx) des anden geräts übereinstimmen)
	
	InitADC();
	
	ssd1306_init();	//Init display
	
	//Ausgabe(3,0,car_icon);			// Funktioniert nicht hängt das programm auf
	
	
	
	while(1)
    {
		
		 transmiter(senden);
		 
    }
}


//////////////////////////////////////////
// Funktion: Daten aus funkmodul laden und Funktion zuweisen
// Name:	 Eric Suter
// Datum:	 8.11.2015
// Version:	 1.1
//////////////////////////////////////////

int rf_receive(void){
	
	transmiter(senden);
	
	nrf24_powerUpRx();
	
	if(nrf24_dataReady()){
		
		nrf24_getData(data_array);
		
		if(data_array[0] == senden){
			
			return 1;
			
		}
		if(data_array[0] == empfangen){
			
			return 2;
			
		}
	}
	
	else{ return 3; }
	
}

//////////////////////////////////////////
// Funktion: Daten an die Fernbedienung senden
// Name:	 Severin Landolt,Eric Suter
// Datum:	 8.11.2015
// Version:	 1.1
//////////////////////////////////////////

void transmiter(int mode){
	

		//ADC2/3  &6/7 (x/y)
		y2=ReadADC(2)>>1;		
		x2=ReadADC(3)>>1;
		y1=ReadADC(6)>>1;
		x1=ReadADC(7)>>1;
		
		button = PIND^0xFF;
		joystick = (PINC & 0x03)^0x03;
		
		if (joystick==0x00)
		{
			Ausgabe(3,x1,car_icon);
		}
		else if (joystick==0x01)
		{
			Ausgabe(3,y1,car_icon);
		}
		else if (joystick== 0x02)
		{
			Ausgabe(3,x2,car_icon);
		} 
		else if (joystick==0x03)
		{
			Ausgabe(3,y2,car_icon);
		}
		 
		 data_array[0] = mode;
		 data_array[1] = x1;
		 data_array[2] = y2;
		 data_array[3] = led;

		 nrf24_send(data_array);							/*Automatically goes to TX mode */
		 
		 while(nrf24_isSending());							/* Wait for transmission to end */

		 temp = nrf24_lastMessageStatus();					/* Make analysis on last tranmission attempt */
		 
		 temp = nrf24_retransmissionCount();				/* Retranmission count indicates the tranmission quality */

		 nrf24_powerDown();									/* Or you might want to power down after TX */

		 _delay_ms(20);						
	
	
}


