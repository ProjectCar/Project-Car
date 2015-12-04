/*
 * Funk.h
 *
 * Created: 18.11.2015 08:04:18
 *  Author: Severin
 */ 

//#include <asf.h>
#include <util/delay.h>
#include <avr/io.h>
#include <stdint.h>


#ifndef FUNK_H_
#define FUNK_H_

#define SS	DDB1
#define CE	DDB0
#define CE_ON	PORTB = PORTB | 0x01 
#define CE_OFF	PORTB = PORTB & ~0x01
#define CSN_ON	PORTB = PORTB | 0x02
#define CSN_OFF	PORTB = PORTB & ~0x02   

#define w_register(x) (0b00100000 | x)
#define r_register(x) (0b00000000 | x)

#define r_rx_payload 0b01100001 
#define w_tx_payload 0b10100000 
/*#define nop 0xFF*/


volatile char r_data1 = 0;
volatile char r_data2 = 0;
volatile char r_data3 = 0;
volatile char r_data4 = 0;
volatile char r_data5 = 0;
volatile uint32_t motordata = 0;
volatile int m_data1 = 0;
volatile int m_data2 = 0;
volatile int transmit = 0;

void SPI_radi_sync(unsigned int reg, unsigned int value);

void SPI_Init(void);
void radio_init(void);
void receive_mode(void);
uint32_t receive_data(void);
int SPI_tranceive(int transmit);
void transmit_mode(void);	//char mode, int data);
void decode(void);



void SPI_Init(void)
{
	 //(CE nur wegen Funkmodul)
	DDRB = ((1<<DDB3)|(1<<DDB5)|(1<<SS)|(1<<CE));	//Set MOSI, SCK, SS und CE output, all others input
	 
	SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0);	// Enable SPI, Master, set clock rate fck/16 
	
	PORTB = PORTB | 0x02;				// Chipselect auf High setzen
	
	_delay_ms(10);					// Delay zum initialisieren
}

int SPI_tranceive(int transmit){									// Sendet Daten per Spi
	
	SPDR = transmit;												// Starte übertragung
	while(!(SPSR & (1<<SPIF)));										// Warten bis übertragung beendet ist
	return SPDR;													// Empfangene Daten zurückgeben
	
}

void SPI_radi_sync(unsigned int reg, unsigned int value)
{
	CSN_OFF;
	SPI_tranceive(w_register(0x00));
	SPI_tranceive(0b00110010);										// Letztes Bit bestimmt RX 1 / 0 TX Mode
	CSN_ON;
}
void radio_init(void)	{

	_delay_ms(120);													// POR Delay
	SPI_tranceive(w_register(0x00));
	_delay_us(150);
	SPI_tranceive(0b00110010);										// Letztes Bit bestimmt RX 1 / 0 TX Mode
	_delay_us(150);
	
	SPI_tranceive(w_register(0x02));
	_delay_us(150);
	SPI_tranceive(0b00000001);			//Eneable data pipe 0
	_delay_us(150);
	
	SPI_tranceive(w_register(0x05));
	_delay_us(150);
	SPI_tranceive(0b01110000);			//RF-CH 7
	_delay_us(150);
	
	SPI_tranceive(w_register(0x06));
	_delay_us(150);
	SPI_tranceive(0b00100110);				//RF setup
	_delay_us(150);
	
	SPI_tranceive(w_register(0x07));
	_delay_us(150);
	SPI_tranceive(0b01110000);						//Status
	_delay_us(150);
	
	SPI_tranceive(w_register(0x0B));
	_delay_us(150);
	SPI_tranceive(0x20);			//send 32 bytes
	
	
	_delay_us(1500);
	
}

void receive_mode(void){

	SPI_tranceive(w_register(0b00000000));							// Funkmodul auf das Lesen von Registern vorbereiten
	SPI_tranceive(0b00110011);										// Einstellregister beschreiben. Letztes Bit bestimmt RX 1 / 0 TX Mode
	PORTB = PORTB | 0x01;											// CE high schalten

}

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

void transmit_mode(void){//char mode, int data){
	
	SPI_tranceive(w_tx_payload);
	SPI_tranceive(0xF0);										// FIFO Beschreiben
// 
// 	SPI_tranceive(mode);											// Fifo mit dem Modus beschreiben
// 	SPI_tranceive(data | 0x00FF);									// FIFO mit
// 	SPI_tranceive((data | 0xFF00) >> 8);
	for(int i = 32; i > 0; --i){									// Dummy Daten senden
		SPI_tranceive(w_tx_payload);
		SPI_tranceive(0xF0);

	}

	SPI_tranceive(w_register(0b00000000));
	SPI_tranceive(0b00110010);										// Letztes Bit bestimmt RX 1 / 0 TX Mode
	PORTB = PORTB | 0x01;											// CE high schalten

}

void decode(void){
	
	motordata = receive_data();										// Daten lesen
	m_data1 = (motordata >> 8) & 0x00FF;							// Entschlüsseln von daten
	m_data2 = motordata >> 24;										// Entschlüsseln von daten
	
}

#endif /* FUNK_H_ */