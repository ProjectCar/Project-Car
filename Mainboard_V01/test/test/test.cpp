//////////////////////////////////
// Programm: Mainboard			//
// Name:	 Eric Suter			//
// Datum:	 8.11.2015			//
// Version:	 1.1				//
//////////////////////////////////

#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>
#include <avr/interrupt.h>
#include "predefines.h"

ISR(INT7_vect){														// Wenn Daten vom Funkmodul empfangen worden sind

	PORTF = PORTF | 0x01;											// Interrupt Status LED einschalten
	decode();														// Daten von Funkmodul lesen und verarbeiten
	
	if ( transmit == 0)	{											// Werden Daten von der Fernbedienung verlangt ?
		
		twi_transmit(MM, 0x01,m_data1);								// Daten für Servo senden
		twi_transmit(MM, 0x02,m_data2);								// Daten für Motor senden

	SPI_tranceive(w_register(0x07));
	_delay_us(50);
	SPI_tranceive(0b11110000);										// Letztes Bit bestimmt RX 1 / 0 TX Mode
	_delay_us(1500);

	}


	
	PORTF = PORTF & 0xFE;											// Interrupt Status LED ausschalten
}

int main (void){

	DDRF = 0xFF;													// INIT	von Ports
	PORTF = 0x02;
	DDRE = 0x7F;
	PORTE = PORTE | 0x80;
	DDRC = 0xFF;
	PORTC = 0x00;
	
	EIMSK = 1<<INT7;												// Interrupt auf INT7 aktivieren
	EICRB = 1<<ISC71 | 1<<ISC70;									// Auslösen auf der Positiven Flanke
	sei();															// Interrupts global aktivieren
	
	spi_init();
	radio_init();													// Funkmodul, TWI und SPI initialisieren
	twi_init();
	
	while(1){
		
		if(transmit == 1){											// Wenn daten von fernbedienung verlangt werden
			PORTF = PORTF | 0x04;									// Sendestatus LED einschalten
			int ldata = twi_receive(EM, 0x00);						// Akkustand lesen
			transmit_mode(0xFF, ((ldata << 8) + PORTC));				// Akkustand und Beleuchtungsstand übertragen
			
			transmit = 0;											// in den Empfangsmodus wechseln
			
			receive_mode();											// wieder in den Empfangsmodus wechseln
			
			PORTF = PORTF & 0xFB;									// Sendestatus LED ausschalten
			
		}
		
	}
}

void spi_init(void){
	
	 DDRB = (1<<DDRB2)|(1<<DDRB1)|(1<<SS);							// MOSI, SCK und SS als Ausgang definieren alle anderen als Eingang
	 SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0);							// SPI aktivieren und Clockrate auf Clock/16 setzen

	 PORTB = PORTB | 0x01;											// Chipselect auf High setzen
	 
	 _delay_ms(10);													// Delay zum initialisieren
	
	
}

//////////////////////////////////////////
// Funktion: Entschlüsseln von erhaltenen Funkdaten
// Name:	 Eric Suter
// Datum:	 11.11.2015
// Version:	 1.1
//////////////////////////////////////////


void decode(void){
	
	motordata = receive_data();										// Daten lesen
	m_data1 = (motordata >> 8) & 0x00FF;							// Entschlüsseln von daten
	m_data2 = motordata >> 24;										// Entschlüsseln von daten
	
}

//////////////////////////////////////////
// Funktion: TWI Initialisieren
// Name:	 Eric Suter
// Datum:	 8.11.2015
// Version:	 1.1
//////////////////////////////////////////

void twi_init(void){
	
	twi_transmit(MM, 0x01,0x0000);									// Init von Motor
	twi_transmit(MM, 0x02,0x0000);									// Init von Motor
	twi_receive(EM, 0x01);											// Akkustand laden
	
}

//////////////////////////////////////////
// Funktion: TWI Daten senden
// Name:	 Eric Suter
// Datum:	 8.11.2015
// Version:	 1.1
//////////////////////////////////////////

void twi_transmit(char adress, char mode, int data){
	
	TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);							// Startcondition senden
	while (!(TWCR &(1<<TWINT)));									// Warten bis gesendet
	
	TWDR = adress;													// Adresse Laden
	TWCR = (1<<TWINT) | (1<<TWEN);									// Senden beginnen
	while (!(TWCR &(1<<TWINT)));									// Warten bis gesendet
	
	TWDR = mode;													// Datenbyte 1 Laden
	TWCR = (1<<TWINT) | (1<<TWEN);									// Senden beginnen
	while (!(TWCR &(1<<TWINT)));									// Warten bis gesendet
	
	TWDR = ((data >> 8 )& 0x00FF);									// Datenbyte 2 Laden
	TWCR = (1<<TWINT) | (1<<TWEN);									// Senden beginnen
	while (!(TWCR &(1<<TWINT)));									// Warten bis gesendet

	TWDR = (data & 0x00FF);											// Datenbyte 3 Laden
	TWCR = (1<<TWINT) | (1<<TWEN);									// Senden beginnen
	while (!(TWCR &(1<<TWINT)));									// Warten bis gesendet

	TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWSTO);							// Stopcondition senden
								
	
}

//////////////////////////////////////////
// Funktion: TWI Daten anfordern
// Name:	 Eric Suter
// Datum:	 8.11.2015
// Version:	 1.1
//////////////////////////////////////////

int twi_receive(char r_adress, char r_mode){
	
	int accu = 0;													// Define von Akku Variabel
	
																	// Daten empfangen EF
	
	return(accu);													// Wert zurückgeben
}

//////////////////////////////////////////
// Funktion: Initialisiert das Funkmodul und SPI Schnittstelle.
// Name:	 Eric Suter, Severin Landolt		
// Datum:	 8.11.2015			
// Version:	 1.1				
//////////////////////////////////////////

void radio_init(void)	{

	_delay_ms(120);													// POR Delay
	SPI_tranceive(w_register(0b00000000));
	_delay_us(50);
	SPI_tranceive(0b00110011);										// Letztes Bit bestimmt RX 1 / 0 TX Mode
	_delay_us(1500);
	SPI_tranceive(w_register(0x07));
	_delay_us(50);
	SPI_tranceive(0b11110000);										// Letztes Bit bestimmt RX 1 / 0 TX Mode
	_delay_us(1500);
	receive_mode();
	
}

//////////////////////////////////////////
// Funktion: Übertägt SPI Daten und gibt empfangene Daten zurück.
// Name:	 Eric Suter, Severin Landolt
// Datum:	 8.11.2015
// Version:	 1.1
//////////////////////////////////////////

int SPI_tranceive(int transmit){									// Sendet Daten per Spi

	SPDR = transmit;												// Starte übertragung
	while(!(SPSR & (1<<SPIF)));										// Warten bis übertragung beendet ist
	return SPDR;													// Empfangene Daten zurückgeben
	
}

//////////////////////////////////////////
// Funktion: Setzt das Funkmodul in den Empfangs modus.
// Name:	 Eric Suter, Severin Landolt
// Datum:	 8.11.2015
// Version:	 1.1
//////////////////////////////////////////

void receive_mode(void){

	SPI_tranceive(w_register(0b00000000));							// Funkmodul auf das Lesen von Registern vorbereiten
	SPI_tranceive(0b00110011);										// Einstellregister beschreiben. Letztes Bit bestimmt RX 1 / 0 TX Mode
	PORTE = PORTE | 0x40;											// CE high schalten

}

//////////////////////////////////////////
// Funktion: Liest empfangene daten aus Funkmodul.
// Name:	 Eric Suter, Severin Landolt
// Datum:	 8.11.2015
// Version:	 1.1
//////////////////////////////////////////

uint32_t receive_data(void){
	
	SPI_tranceive(0b01100001);										// In den lesemodus wechseln Muss vl. mit receive_mode(void); ersetzt werden
	
	SPI_tranceive(0xFF);											// Erstes Byte auslesen
	r_data1 = SPDR;													// Daten zwischenspeichern
	
	if (r_data1 == 0x77){ transmit = 1; }							// Wenn gewisse daten kommen in den Sendemodus wechseln.
	
	SPI_tranceive(0xFF);											// Zweites Byte auslesen
	r_data2 = SPDR;													// Daten zwischenspeichern
	
	SPI_tranceive(0xFF);											// Drites Byte auslesen
	r_data3 = SPDR;													// Daten zwischenspeichern
	
	SPI_tranceive(0xFF);											// Viertes Byte auslesen
	r_data4 = SPDR;													// Daten zwischenspeichern
		
	SPI_tranceive(0xFF);											// Fünftes Byte auslesen
	r_data5 = SPDR;													// Daten zwischenspeichern
	
	uint32_t result = ((((((((r_data5<<8)+r_data4)<<8)+r_data3)<<8)+r_data2)<<8)+r_data1);	// Die ganzen Daten in einen Datentyp zum übergeben wandeln
	
	for(int i = 29; i > 0; --i){									// Dummy Daten empfangen

		SPI_tranceive(0xFF);										// SPI NOP

	}
	
	return(result);													// Resultat zurückgeben
	
}

//////////////////////////////////////////
// Funktion: Setzt das Funkmodul in den Übertragungsmodus und sendet die daten
// Name:	 Eric Suter, Severin Landolt
// Datum:	 8.11.2015
// Version:	 1.1
//////////////////////////////////////////

void transmit_mode(char mode, int data){

	SPI_tranceive(0b10100000);										// FIFO Beschreiben

	SPI_tranceive(mode);											// Fifo mit dem Modus beschreiben
	SPI_tranceive(data | 0x00FF);									// FIFO mit 
	SPI_tranceive((data | 0xFF00) >> 8);						
	for(int i = 29; i != 0; --i){									// Dummy Daten senden

		SPI_tranceive(0xFF);

	}

	SPI_tranceive(w_register(0b00000000));
	SPI_tranceive(0b00110010);										// Letztes Bit bestimmt RX 1 / 0 TX Mode
	PORTE = PORTE | 0x40;											// CE high schalten

}
