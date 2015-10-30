/*
 * GccApplication1.cpp
 *
 * Created: 12.10.2015 19:15:21
 *  Author: Eric, Severin
 */ 


#include <avr/io.h>
#include <util/delay.h>


#define w_register(x) (0b00100000 | x)
#define r_register(x) (0b00000000 | x)
#define nop 0xFF
#define ce XX
#define eric severin

#define DDR_SPI DDRB
#define DD_SCK	DDRB1
#define DD_MOSI DDRB2


 int received_data = 0;

void init_radio(void)	{

 _delay_ms(120);									// POR Delay
 
 DDR_SPI = (1 << DD_MOSI)|(1 << DD_SCK);			// Set MOSI and SCK output, all others input 
 SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0);  				// Enable SPI, Master, set clock rate fck/16
 
SPI_tranceive(w_register(0b00000000));
SPI_tranceive(0b0110010);							// Letztes Bit bestimmt RX 1 / 0 TX Mode
_delay_us(1500);
 
 }
 


int SPI_tranceive(int transmit){					// Sendet Daten per Spi
	
	 /* Start transmission */
	 SPDR = transmit;
	 /* Wait for transmission complete */
	 while(!(SPSR & (1<<SPIF)));
	 
	 return SPDR;									// Empfangene Daten geben
	
}


	
int receive_mode(void){

SPI_tranceive(w_register(0b00000000));
SPI_tranceive(0b0110011);							// Letztes Bit bestimmt RX 1 / 0 TX Mode
ce = 1;

}	


int receive_data(void){

SPI_tranceive(0b01100001);							// In den lesemodus wechseln	Muss vl. mit receive_mode(void); ersetzt werden

SPI_tranceive(0xFF);								// Erstes Byte auslesen

received_data = SPDR;

for(int i = 31; i == 0; --i){						// Dummy Daten empfangen

SPI_tranceive(0xFF);

if ( SPDR != 0xFF ){ return(0); }

}



}


int transmit_mode(char send_data){

	SPI_tranceive(0b10100000);							// FIFO Beschreiben

	SPI_tranceive(send_data);							// Daten zum Senden geben

	for(int i = 31; i == 0; --i){						// Dummy Daten senden

		SPI_tranceive(0xFF);

	}

	SPI_tranceive(w_register(0b00000000));
	SPI_tranceive(0b0110010);							// Letztes Bit bestimmt RX 1 / 0 TX Mode
	ce = 1;

}
	




int main (void){

	while(1){


		if (funktioniert);



	}
}