//////////////////////////////////
// Programm: Mainboard			//
// Name:	 Eric Suter			//
// Datum:	 8.11.2015			//
// Version:	 1.1				//
//////////////////////////////////

#define F_CPU 1000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>
#include <avr/interrupt.h>
#include "predefines.h"
#include "interrupts.h"

int main (void){
	
	DDRF = 0xFF;													// INIT	von Ports
	PORTF = 0x01;
	DDRD = DDRD | 0x80;
	PORTD = PORTD | 0x80;
	
	init_radio();													// Funkmodul und TWI initialisieren
	twi_init();
	
	EIMSK = 1<<INT7;												// Interrupt auf INT7 aktivieren
	EICRB = 1<<ISC71 | 1<<ISC71;									// Auslösen auf der Positiven Flanke
	sei();															// Interrupts global aktivieren
	
	while(1){
		
		decode();													// Daten von Funkmodul laden und verarbeiten
		
		if(transmit == 1){											// Wenn daten von fernbedienung verlangt werden
			
			int ldata = twi_receive(EM, 0x00);						// Akkustand lesen
			transmit_mode(0xFF, ldata);								// Akkustand übertragen
			transmit_mode(0x0F, PORTC);								// Beleuchtungsstand übertragen
			
			transmit = 0;											// in den Empfangsmodus wechseln
			receive_mode();
			
		}
		
		twi_transmit(MM, 0x01,m_data1);								// Daten für Servo senden
		twi_transmit(MM, 0x02,m_data2);								// Daten für Motor senden

	}
}

void decode(void){
	
	motordata = receive_data();										// daten lesen
	m_data1 = (motordata >> 8) & 0x00FF;							// entschlüsseln von daten
	m_data2 = motordata >> 24;										// entschlüsseln von daten
	
}

//////////////////////////////////////////
// Funktion: TWI Initialisieren
// Name:	 Eric Suter
// Datum:	 8.11.2015
// Version:	 1.1
//////////////////////////////////////////

void twi_init(void){
																
																// Init von TWI EF
	twi_transmit(MM, 0x01,0x0000);								// Init von Motor
	twi_transmit(MM, 0x02,0x0000);								// Init von Motor
	twi_receive(EM, 0x01);										// Akkustand laden
	
}

//////////////////////////////////////////
// Funktion: TWI Daten senden
// Name:	 Eric Suter
// Datum:	 8.11.2015
// Version:	 1.1
//////////////////////////////////////////

void twi_transmit(char adress, char mode, int data){
	
																// EF
	
}

//////////////////////////////////////////
// Funktion: TWI Daten anfordern
// Name:	 Eric Suter
// Datum:	 8.11.2015
// Version:	 1.1
//////////////////////////////////////////

int twi_receive(char r_adress, char r_mode){
	
	int accu = 0;											// define
	
															// daten empfangen EF
	
	return(accu);											// Wert zurückgeben
}

//////////////////////////////////////////
// Funktion: Initialisiert das Funkmodul und SPI Schnittstelle.
// Name:	 Eric Suter, Severin Landolt		
// Datum:	 8.11.2015			
// Version:	 1.1				
//////////////////////////////////////////

void init_radio(void)	{

	_delay_ms(120);										// POR Delay
	
	DDR_SPI = (1 << DD_MOSI)|(1 << DD_SCK);				// SPI Initialisieren
	SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0);  				// SPI Initialisieren
	
	SPI_tranceive(w_register(0b00000000));
	
	SPI_tranceive(0b0110010);							// Letztes Bit bestimmt RX 1 / 0 TX Mode
	
	_delay_us(1500);
	
	receive_mode();
	
}

//////////////////////////////////////////
// Funktion: Übertägt SPI Daten und gibt empfangene Daten zurück.
// Name:	 Eric Suter, Severin Landolt
// Datum:	 8.11.2015
// Version:	 1.1
//////////////////////////////////////////

int SPI_tranceive(int transmit){						// Sendet Daten per Spi
	
	SPDR = transmit;									// Starte übertragung
	
	while(!(SPSR & (1<<SPIF)));							// Warten bis übertragung beendet ist
	
	return SPDR;										// Empfangene Daten geben
	
}

//////////////////////////////////////////
// Funktion: Setzt das Funkmodul in den Empfangs modus.
// Name:	 Eric Suter, Severin Landolt
// Datum:	 8.11.2015
// Version:	 1.1
//////////////////////////////////////////

void receive_mode(void){

	SPI_tranceive(w_register(0b00000000));
	SPI_tranceive(0b0110011);							// Letztes Bit bestimmt RX 1 / 0 TX Mode
	//ce = 1;

}

//////////////////////////////////////////
// Funktion: Liest empfangene daten aus Funkmodul.
// Name:	 Eric Suter, Severin Landolt
// Datum:	 8.11.2015
// Version:	 1.1
//////////////////////////////////////////

uint32_t receive_data(void){
	
	SPI_tranceive(0b01100001);							// In den lesemodus wechseln	Muss vl. mit receive_mode(void); ersetzt werden
	
	SPI_tranceive(0xFF);								// Erstes Byte auslesen
	r_data1 = SPDR;
	
	if (r_data1 == 0x77){ transmit = 1; }				// Daten senden wechsel
	
	SPI_tranceive(0xFF);								// Zweites Byte auslesen
	r_data2 = SPDR;
	
	SPI_tranceive(0xFF);								// Drites Byte auslesen
	r_data3 = SPDR;
	
	SPI_tranceive(0xFF);								// Viertes Byte auslesen
	r_data4 = SPDR;
		
	SPI_tranceive(0xFF);								// Fünftes Byte auslesen
	r_data5 = SPDR;
	
	uint32_t result = ((((((((r_data5<<8)+r_data4)<<8)+r_data3)<<8)+r_data2)<<8)+r_data1);
	
	for(int i = 29; i == 0; --i){						// Dummy Daten empfangen

		SPI_tranceive(0xFF);

	}
	
	return(result);
	
}

//////////////////////////////////////////
// Funktion: Setzt das Funkmodul in den Übertragungsmodus und sendet die daten
// Name:	 Eric Suter, Severin Landolt
// Datum:	 8.11.2015
// Version:	 1.1
//////////////////////////////////////////

void transmit_mode(char mode, int data){

	SPI_tranceive(0b10100000);							// FIFO Beschreiben

	SPI_tranceive(mode);								// Daten zum Senden geben
	SPI_tranceive(data | 0x00FF);
	SPI_tranceive((data | 0xFF00) >> 8);
	for(int i = 29; i == 0; --i){						// Dummy Daten senden

		SPI_tranceive(0xFF);

	}

	SPI_tranceive(w_register(0b00000000));
	SPI_tranceive(0b0110010);							// Letztes Bit bestimmt RX 1 / 0 TX Mode
	//ce = 1;

}
