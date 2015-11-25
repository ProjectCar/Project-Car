//////////////////////////////////
// Programm: Mainboard			//
// Name:	 Eric Suter			//
// Datum:	 8.11.2015			//
// Version:	 1.1				//
//////////////////////////////////
#include <avr/io.h>
#include <stdint.h>
#include "nrf24.h"
#define F_CPU 16000000UL
#include <util/delay.h>
#include <avr/interrupt.h>

/* ------------------------------------------------------------------------- */


#define EM 0xFF
#define MM 0x0F
#define akku 0x12
#define motor 0x55
#define servo 0x66
#define empfangen 0x00
#define senden 0x77
#define sback 0x00
#define sfwd 0x01


volatile uint32_t motordata = 0;
volatile int m_data1 = 0;
volatile int m_data2 = 0;
volatile int transmit = 0;

void twi_init(void);
void twi_transmit(char adress, char mode, int data);
int twi_receive(char r_adress, char r_mode);
void decode(void);
void rf_transimit(void);
int rf_receive(void);
void forwardsecure(void);
void backwardsecure(void);

uint8_t temp;
uint8_t q = 0;
uint8_t data_array[4];
uint8_t tx_address[5] = {0xD7,0xD7,0xD7,0xD7,0xD7};
uint8_t rx_address[5] = {0xE7,0xE7,0xE7,0xE7,0xE7};
	
/* ------------------------------------------------------------------------- */

ISR(INT2_vect){
	
	forwardsecure();
	
}

ISR(INT3_vect){
	
	backwardsecure();
	
	
}

/* ------------------------------------------------------------------------- */

int main(){
	
	DDRF = 0xFF;
	PORTF = 0x01;
	DDRC = 0xFF;
	PORTC = 0x00;
	DDRA = 0x00;
	PORTA = 0x00;
	
	EIMSK = (1<<INT2) | (1<<INT3);												// Interrupt auf INT2 und 3 aktivieren
	EICRA = (1<<ISC21) | (0<<ISC20) | (1<<ISC31) | (0<<ISC30);	
	sei();
	
	nrf24_init();																// Funkmodul Initialisieren
	nrf24_config(2,4);															// Den Chanel vom Funkmodul wählen und Anzahl der Byte zum übertragen angeben
	nrf24_tx_address(tx_address);												// Sendeadresse festlegen ( Muss mit empfängeradresse(rx) des anden geräts übereinstimmen)
	nrf24_rx_address(rx_address);												// Empfangsadresse festlegen ( Muss mit sendeadresse(tx) des anden geräts übereinstimmen)

	while(1) {

		switch(rf_receive()) {
			
			case 1: rf_transimit(); break;
			case 2:
				//	twi_transmit(MM, motor, data_array[2]);
				//	twi_transmit(MM, servo, data_array[3]);
				
				if ( data_array[1] == 0xAA)  {
							
				PORTF |= 0x02;
				_delay_ms(250);
				PORTF &= ~(0x02);
							
				}
				
				
				
					break;
			
			case 3: break;
			
		}
	
		
	}
}
/* -------------------------------------------------------*/


//////////////////////////////////////////
// Funktion: Entschlüsseln von erhaltenen Funkdaten
// Name:	 Eric Suter
// Datum:	 11.11.2015
// Version:	 1.1
//////////////////////////////////////////

void decode(void){
	
	
	
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
// Funktion: Daten an die Fernbedienung senden
// Name:	 Eric Suter
// Datum:	 8.11.2015
// Version:	 1.1
//////////////////////////////////////////

void rf_transimit(){
	
	data_array[0] = 0x77;
	data_array[1] = twi_receive(EM, akku);
	data_array[2] = PORTC;
	data_array[3] = 0x00;
	
	
	nrf24_send(data_array);											// Datenarray versenden
	while(nrf24_isSending());										// Warten bis senden beendet ist
	temp = nrf24_lastMessageStatus();								// Schaut ob alle Daten angekommen sind

	if(temp == NRF24_TRANSMISSON_OK){
		
		PORTF |= 0x02;
		_delay_ms(25);
		PORTF |= ~(0x02);
		
	}
	else if(temp == NRF24_MESSAGE_LOST){
		
		PORTF |= 0x08;
		_delay_ms(25);
		PORTF |= ~(0x08);
		
	}
	nrf24_powerUpRx();
	_delay_ms(10);
	
}

//////////////////////////////////////////
// Funktion: Daten aus funkmodul laden und Funktion zuweisen
// Name:	 Eric Suter
// Datum:	 8.11.2015
// Version:	 1.1
//////////////////////////////////////////

int rf_receive(void){
	
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
// Funktion: Daten aus funkmodul laden und Funktion zuweisen
// Name:	 Eric Suter
// Datum:	 8.11.2015
// Version:	 1.1
//////////////////////////////////////////

void backwardsecure(){

	while ((PINA & 0x01) == 0){
	
		//twi_transmit(MM, motor, sback);
		PORTF |= 0x08;
	
	}
	_delay_ms(500);
	PORTF &= ~(0x08);
}

//////////////////////////////////////////
// Funktion: Daten aus funkmodul laden und Funktion zuweisen
// Name:	 Eric Suter
// Datum:	 8.11.2015
// Version:	 1.1
//////////////////////////////////////////


void forwardsecure(){
	
		while ((PINA & 0x02) == 0){
			
			//twi_transmit(MM, motor, sfwd);
			PORTF |= 0x04;
			
		}
		_delay_ms(500);
		PORTF &= ~(0x04);
}