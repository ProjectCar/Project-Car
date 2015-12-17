/*
 * Display.h
 *
 * Created: 18.11.2015 08:04:38
 *  Author: Severin
 */ 


#include <avr/io.h>


#ifndef DISPLAY_H_
#define DISPLAY_H_


void i2c_start(void);
void i2c_stop(void);
void i2c_write(int def, int command);
void i2c_write_wo(int command);
void ssd1306_command(int command);
void ssd1306_data(int command);
void ssd1306_init(void);
void Display_out (char pic[64][16]);		//In die obersten drei Zeilen darf nicht geschreiben werden sonst giebt es einen Fehler
void Ausgabe (unsigned char led, unsigned int Prozent, char pic[64][16]);

//Defines


//Define 0x3C
#define SLA_W 0x78
#define DATA 0x00

//Defines for later use
#define SSD1306_LCDWIDTH                  128

//	Fundamental #defines
#define SSD1306_SETCONTRAST 0x81
#define SSD1306_DISPLAYALLON_RESUME 0xA4
#define SSD1306_DISPLAYALLON 0xA5
#define SSD1306_NORMALDISPLAY 0xA6
#define SSD1306_INVERTDISPLAY 0xA7
#define SSD1306_DISPLAYOFF 0xAE
#define SSD1306_DISPLAYON 0xAF

//	Scrolling #defines
#define SSD1306_RIGHT_HORIZONTAL_SCROLL 0x26
#define SSD1306_LEFT_HORIZONTAL_SCROLL 0x27
#define SSD1306_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL 0x29
#define SSD1306_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL 0x2A
#define SSD1306_ACTIVATE_SCROLL 0x2F
#define SSD1306_DEACTIVATE_SCROLL 0x2E
#define SSD1306_SET_VERTICAL_SCROLL_AREA 0xA3

// Addressing Setting #defines
#define SSD1306_SETLOWCOLUMN 0x00
#define SSD1306_SETHIGHCOLUMN 0x10
#define SSD1306_MEMORYMODE 0x20
#define SSD1306_COLUMNADDR 0x21
#define SSD1306_PAGEADDR   0x22

//	Hardware Configuration #defines
#define SSD1306_SETSTARTLINE 0x40		//(40-7F)
#define SSD1306_SEGREMAP 0xA0			//Spalte auf 0 setzen (mit 0xA1 wird auf 127 gesetzt)
#define SSD1306_SETMULTIPLEX 0xA8		//setzt multiplex
#define SSD1306_COMSCANINC 0xC0
#define SSD1306_COMSCANDEC 0xC8
#define SSD1306_SETDISPLAYOFFSET 0xD3
#define SSD1306_SETCOMPINS 0xDA
//	Timing & Driving Scheme Setting		#defines
#define SSD1306_SETDISPLAYCLOCKDIV 0xD5
#define SSD1306_SETPRECHARGE 0xD9
#define SSD1306_SETVCOMDETECT 0xDB
#define SSD1306_NOP 0xE3

//	Charge Pump	#defines
#define SSD1306_CHARGEPUMP 0x8D


#define SSD1306_EXTERNALVCC 0x1
#define SSD1306_SWITCHCAPVCC 0x2


void i2c_start(void)
{
	TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);
	
	while (!(TWCR & (1<<TWINT)));
}
void i2c_stop(void)
{
	TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWSTO);
}
void i2c_write(int def, int command)
{
	i2c_start();
	
	TWDR = SLA_W;
	TWCR = (1<<TWINT)|(1<<TWEN);
	
	while (!(TWCR & (1<<TWINT)));

	TWDR = def;
	TWCR = (1<<TWINT) |(1<<TWEN);
	
	while (!(TWCR & (1<<TWINT)));

	TWDR = command;
	TWCR = (1<<TWINT) |(1<<TWEN);
	
	while (!(TWCR & (1<<TWINT)));
	
	i2c_stop();
}
void i2c_write_wo(int command)
{
	TWDR = command;
	TWCR = (1<<TWINT)|(1<<TWEN);
	
	while (!(TWCR & (1<<TWINT)));
}
void ssd1306_command(int command)
{
	i2c_write(0x00, command);	//wen D/C 0 ist werden commands gesendet (D/C ist bit6)
}
void ssd1306_data(int command)
{
	i2c_write(0x40, command);	//wen D/C 1 ist werden daten gesendet (D/C ist bit6)
}
void ssd1306_init(void)		// Init sequence for 128x64 OLED module
{
	//	Auschalten
	ssd1306_command(SSD1306_DISPLAYOFF);                    // 0xAE
	
	//	Set MUX Ratio
	ssd1306_command(SSD1306_SETMULTIPLEX);                  // 0xA8
	ssd1306_command(0x3F);
	//	Set Display Offset
	ssd1306_command(SSD1306_SETDISPLAYOFFSET);              // 0xD3
	ssd1306_command(0x0);                                   // no offset
	//	Set Display Start Line
	ssd1306_command(SSD1306_SETSTARTLINE | 0x0);            // line #0
	
	//	Setzt Horizontaler Adressmodus
	ssd1306_command(SSD1306_MEMORYMODE);                    // 0x20
	ssd1306_command(0x00);                                  // 0x0 act like ks0108
	//	Set Segment  re-map
	ssd1306_command(SSD1306_SEGREMAP | 0x1);
	//	Set COM Output Scan Direction
	ssd1306_command(SSD1306_COMSCANDEC);
	//	Set COM Pins hardware configuration
	ssd1306_command(SSD1306_SETCOMPINS);                    // 0xDA
	ssd1306_command(0x12);							//Theoretisch 0x02
	//	Set Contrast Control
	ssd1306_command(SSD1306_SETCONTRAST);                   // 0x81
	ssd1306_command(0xCF);									// kontrast (Theoretisch 0x7F)
	
	
	//	Disable Entire Display On
	ssd1306_command(SSD1306_DISPLAYALLON_RESUME);           // 0xA4
	//	Set Normal Display
	ssd1306_command(SSD1306_NORMALDISPLAY);                 // 0xA6
	//	Set Osc Frequency
	ssd1306_command(SSD1306_SETDISPLAYCLOCKDIV);            // 0xD5
	ssd1306_command(0x80);                                  // das vorgeschlagene Verhältnis 0x80
	
	ssd1306_command(SSD1306_SETPRECHARGE);                  // 0xd9
	ssd1306_command(0xF1);
	ssd1306_command(SSD1306_SETVCOMDETECT);                 // 0xDB
	ssd1306_command(0x40);
	//	Enable charge pump regulator
	ssd1306_command(SSD1306_CHARGEPUMP);                    // 0x8D
	ssd1306_command(0x14);									//Enable Charge Pump
	//	Display On
	ssd1306_command(SSD1306_DISPLAYON);
	
	
	//Setzt Kollonen Adressbereich (von 0-127)
	ssd1306_command(SSD1306_COLUMNADDR);
	ssd1306_command(0);   // Column start address (0 = reset)
	ssd1306_command(SSD1306_LCDWIDTH - 1); // Column end address (127 = reset)
	
	//Setzt seiten Adressbereich	(von 0-7)
	ssd1306_command(SSD1306_PAGEADDR);
	ssd1306_command(0); // Page start address (0 = reset)

	ssd1306_command(7); // Page end address
	
	TWBR = 10;		//??????
}
void Display_out (char pic[64][16])
{
	i2c_start();
	i2c_write_wo(SLA_W);
	i2c_write_wo(0x40);
	
	char temp_value;
	for(int height_pixels = 0; height_pixels < 8; height_pixels++)
	{
		for(int width_pixels = 0; width_pixels < 16; width_pixels++)
		{
			for(int charcounter = 0; charcounter < 8; charcounter++)
			{
				temp_value = 0x00;
				
				if((pic[(height_pixels * 8)][width_pixels] & (0x80 >> charcounter)))
				{
					temp_value |= 0x01;
				}
				if((pic[(height_pixels * 8) + 1][width_pixels] & (0x80 >> charcounter)))
				{
					temp_value |= 0x02;
				}
				if((pic[(height_pixels * 8) + 2][width_pixels] & (0x80 >> charcounter)))
				{
					temp_value |= 0x04;
				}
				if((pic[(height_pixels * 8) + 3][width_pixels] & (0x80 >> charcounter)))
				{
					temp_value |= 0x08;
				}
				if((pic[(height_pixels * 8) + 4][width_pixels] & (0x80 >> charcounter)))
				{
					temp_value |= 0x10;
				}
				if((pic[(height_pixels * 8) + 5][width_pixels] & (0x80 >> charcounter)))
				{
					temp_value |= 0x20;
				}
				if((pic[(height_pixels * 8) + 6][width_pixels] & (0x80 >> charcounter)) )
				{
					temp_value |= 0x40;
				}
				if((pic[(height_pixels * 8) + 7][width_pixels] & (0x80 >> charcounter)))
				{
					temp_value |= 0x80;
				}
				i2c_write_wo(temp_value);
			}
		}
	}

	i2c_stop();
}



void Ausgabe (unsigned char led, unsigned int Prozent, char pic[64][16])
{
	if (Prozent>33)
	{
		pic[10][14] = 0X5F;	pic[10][15] = 0XA0;
		pic[11][14] = 0X5F;	pic[11][15] = 0XA0;
		
		if (Prozent>66)
		{
			pic[7][14] = 0X5F;	pic[7][15] = 0XA0;
			pic[8][14] = 0X5F;	pic[8][15] = 0XA0;
		}
		else
		{
			pic[7][14] = 0X40;	pic[7][15] = 0X20;
			pic[8][14] = 0X40;	pic[8][15] = 0X20;
		}
	}
	else
	{
		pic[7][14] = 0X40;	pic[7][15] = 0X20;
		pic[8][14] = 0X40;	pic[8][15] = 0X20;
		
		pic[10][14] = 0X40;	pic[10][15] = 0X20;
		pic[11][14] = 0X40;	pic[11][15] = 0X20;
	}
	
	if (led & 0x10)		//forne oben
	{
		pic[25][11] = 0X98;
		pic[26][11] = 0X4C;
		pic[27][11] = 0X26;
		pic[28][11] = 0X97;
	}
	else
	{
		pic[25][11] = 0X18;
		pic[26][11] = 0X0C;
		pic[27][11] = 0X06;
		pic[28][11] = 0X87;
	}
	
	if (led & 0x10)		//forne unten
	{
		pic[51][11] = 0X97;
		pic[52][11] = 0X26;
		pic[53][11] = 0X4C;
		pic[54][11] = 0X98;
	}
	else
	{
		pic[51][11] = 0X87;
		pic[52][11] = 0X06;
		pic[53][11] = 0X0C;
		pic[54][11] = 0X18;
	}
	if (led & 0x02)		//hinten oben
	{
		pic[23][4] = 0XAF;
		pic[24][4] = 0XAF;
		pic[25][4] = 0XAF;
		pic[26][4] = 0XAF;
	}
	else
	{
		pic[23][4] = 0X2F;
		pic[24][4] = 0X2F;
		pic[25][4] = 0X2F;
		pic[26][4] = 0X2F;
	}
	
	if (led & 0x04) //hinten unten
	{
		pic[53][4] = 0XAF;
		pic[54][4] = 0XAF;
		pic[55][4] = 0XAF;
		pic[56][4] = 0XAF;
	}
	else
	{
		pic[53][4] = 0X2F;
		pic[54][4] = 0X2F;
		pic[55][4] = 0X2F;
		pic[56][4] = 0X2F;
	}
	
	if (led & 0x08)	//Bremslicht hinten
	{
		pic[36][6] = 0X57;
		pic[37][6] = 0X57;
		pic[38][6] = 0X57;
		pic[39][6] = 0X57;
		pic[40][6] = 0X57;
		pic[41][6] = 0X57;
		pic[42][6] = 0X57;
		pic[43][6] = 0X57;
	}
	else
	{
		pic[36][6] = 0X47;
		pic[37][6] = 0X47;
		pic[38][6] = 0X47;
		pic[39][6] = 0X47;
		pic[40][6] = 0X47;
		pic[41][6] = 0X47;
		pic[42][6] = 0X47;
		pic[43][6] = 0X47;
	}
		

	unsigned char einer=(Prozent % 10);
	unsigned char zehner=((Prozent-einer)%100)/10;
	unsigned char hunderter=(Prozent-einer-zehner)/100;
	
	 
	switch(einer)
	{
		case 0:
			pic[6][11]=0X1F;		pic[6][12]=0X83;
			pic[7][11]=0X3F;		pic[7][12]=0XC4;
			pic[8][11]=0X71;		pic[8][12]=0XE4;
			pic[9][11]=0X61;		pic[9][12]=0XE3;
			pic[10][11]=0X66;		pic[10][12]=0X60;
			pic[11][11]=0X66;		pic[11][12]=0X60;
			pic[12][11]=0X78;		pic[12][12]=0X60;
			pic[13][11]=0X78;		pic[13][12]=0XE1;
			pic[14][11]=0X3F;		pic[14][12]=0XC2;
			pic[15][11]=0X1F;		pic[15][12]=0X84;

		break;
		
		case 1:
			pic[6][11]=0X1E;		pic[6][12]=0X03;
			pic[7][11]=0X1E;		pic[7][12]=0X04;
			pic[8][11]=0X06;		pic[8][12]=0X04;
			pic[9][11]=0X06;		pic[9][12]=0X03;
			pic[10][11]=0X06;		pic[10][12]=0X00;
			pic[11][11]=0X06;		pic[11][12]=0X00;
			pic[12][11]=0X06;		pic[12][12]=0X00;
			pic[13][11]=0X06;		pic[13][12]=0X01;
			pic[14][11]=0X1F;		pic[14][12]=0X82;
			pic[15][11]=0X1F;		pic[15][12]=0X84;
		break;
		
		case 2:
			pic[6][11]=0X7F;		pic[6][12]=0X83;
			pic[7][11]=0X7F;		pic[7][12]=0XC4;
			pic[8][11]=0X00;		pic[8][12]=0X64;
			pic[9][11]=0X00;		pic[9][12]=0XE3;
			pic[10][11]=0X1F;		pic[10][12]=0XC0;
			pic[11][11]=0X3F;		pic[11][12]=0X80;
			pic[12][11]=0X70;		pic[12][12]=0X00;
			pic[13][11]=0X60;		pic[13][12]=0X01;
			pic[14][11]=0X7F;		pic[14][12]=0XE2;
			pic[15][11]=0X7F;		pic[15][12]=0XE4;	
		break;
		
		case 3:
			pic[6][11]= 0X7F;		pic[6][12]= 0X83;
			pic[7][11]= 0X7F;		pic[7][12]= 0XC4;
			pic[8][11]= 0X00;		pic[8][12]= 0XE4;
			pic[9][11]= 0X00;		pic[9][12]= 0XE3;
			pic[10][11]= 0X07;		pic[10][12]= 0XC0;
			pic[11][11]= 0X07;		pic[11][12]= 0XC0;
			pic[12][11]= 0X00;		pic[12][12]= 0XE0;
			pic[13][11]= 0X00;		pic[13][12]= 0XE1;
			pic[14][11]= 0X7F;		pic[14][12]= 0XC2;
			pic[15][11]= 0X7F;		pic[15][12]= 0X84;
		break;
		
		case 4:
			pic[6][11]= 0X07;		pic[6][12]= 0X83;
			pic[7][11]= 0X0F;		pic[7][12]= 0X84;
			pic[8][11]= 0X1D;		pic[8][12]= 0X84;
			pic[9][11]= 0X39;		pic[9][12]= 0X83;
			pic[10][11]= 0X71;		pic[10][12]= 0X80;
			pic[11][11]= 0X61;		pic[11][12]= 0X80;
			pic[12][11]= 0X7F;		pic[12][12]= 0XE0;
			pic[13][11]= 0X7F;		pic[13][12]= 0XE1;
			pic[14][11]= 0X01;		pic[14][12]= 0X82;
			pic[15][11]= 0X01;		pic[15][12]= 0X84;		
		break;
		
		case 5:
			pic[6][11]= 0X7F;		pic[6][12]= 0XE3;
			pic[7][11]= 0X7F;		pic[7][12]= 0XE4;
			pic[8][11]= 0X60;		pic[8][12]= 0X04;
			pic[9][11]= 0X60;		pic[9][12]= 0X03;
			pic[10][11]= 0X7F;		pic[10][12]= 0X80;
			pic[11][11]= 0X7F;		pic[11][12]= 0XC0;
			pic[12][11]= 0X00;		pic[12][12]= 0X60;
			pic[13][11]= 0X00;		pic[13][12]= 0X61;
			pic[14][11]= 0X7F;		pic[14][12]= 0XC2;
			pic[15][11]= 0X7F;		pic[15][12]= 0X84;	
		break;
		
		case 6:
			pic[6][11]= 0X1F;		pic[6][12]= 0XE3;
			pic[7][11]= 0X3F;		pic[7][12]= 0XE4;
			pic[8][11]= 0X60;		pic[8][12]= 0X04;
			pic[9][11]= 0X60;		pic[9][12]= 0X03;
			pic[10][11]= 0X7F;		pic[10][12]= 0X80;
			pic[11][11]= 0X7F;		pic[11][12]= 0XC0;
			pic[12][11]= 0X60;		pic[12][12]= 0X60;
			pic[13][11]= 0X60;		pic[13][12]= 0X61;
			pic[14][11]= 0X3F;		pic[14][12]= 0XC2;
			pic[15][11]= 0X1F;		pic[15][12]= 0X84;		
		break;
		
		case 7:
			pic[6][11]= 0X7F;		pic[6][12]= 0XE3;
			pic[7][11]= 0X7F;		pic[7][12]= 0XE4;
			pic[8][11]= 0X00;		pic[8][12]= 0X64;
			pic[9][11]= 0X00;		pic[9][12]= 0XE3;
			pic[10][11]= 0X01;		pic[10][12]= 0XC0;
			pic[11][11]= 0X03;		pic[11][12]= 0X80;
			pic[12][11]= 0X07;		pic[12][12]= 0X00;
			pic[13][11]= 0X0E;		pic[13][12]= 0X01;
			pic[14][11]= 0X1C;		pic[14][12]= 0X02;
			pic[15][11]= 0X18;		pic[15][12]= 0X04;
		break;
		
		case 8:
			pic[6][11]= 0X1F;		pic[6][12]= 0X83;
			pic[7][11]= 0X3F;		pic[7][12]= 0XC4;
			pic[8][11]= 0X60;		pic[8][12]= 0X64;
			pic[9][11]= 0X60;		pic[9][12]= 0X63;
			pic[10][11]= 0X1F;		pic[10][12]= 0X80;
			pic[11][11]= 0X1F;		pic[11][12]= 0X80;
			pic[12][11]= 0X60;		pic[12][12]= 0X60;
			pic[13][11]= 0X60;		pic[13][12]= 0X61;
			pic[14][11]= 0X3F;		pic[14][12]= 0XC2;
			pic[15][11]= 0X1F;		pic[15][12]= 0X84;
		break;
		
		case 9:
			pic[6][11]= 0X1F;		pic[6][12]= 0X83;
			pic[7][11]= 0X3F;		pic[7][12]= 0XC4;
			pic[8][11]= 0X60;		pic[8][12]= 0X64;
			pic[9][11]= 0X60;		pic[9][12]= 0X63;
			pic[10][11]= 0X3F;		pic[10][12]= 0XE0;
			pic[11][11]= 0X1F;		pic[11][12]= 0XE0;
			pic[12][11]= 0X00;		pic[12][12]= 0X60;
			pic[13][11]= 0X00;		pic[13][12]= 0X61;
			pic[14][11]= 0X7F;		pic[14][12]= 0XC2;
			pic[15][11]= 0X3F;		pic[15][12]= 0X84;		
		break;
	}
	
		switch(zehner)
	{
		case 0:
			pic[6][9]=0X01;		pic[6][10]=0XF8;
			pic[7][9]=0X03;		pic[7][10]=0XFC;
			pic[8][9]=0X07;		pic[8][10]=0X1E;
			pic[9][9]=0X06;		pic[9][10]=0X1E;
			pic[10][9]=0X06;		pic[10][10]=0X66;
			pic[11][9]=0X06;		pic[11][10]=0X66;
			pic[12][9]=0X07;		pic[12][10]=0X86;
			pic[13][9]=0X07;		pic[13][10]=0X8E;
			pic[14][9]=0X03;		pic[14][10]=0XFC;
			pic[15][9]=0X01;		pic[15][10]=0XF8;
		break;
		
		case 1:
			pic[6][9]=0X01;		pic[6][10]=0XE0;
			pic[7][9]=0X01;		pic[7][10]=0XE0;
			pic[8][9]=0X00;		pic[8][10]=0X60;
			pic[9][9]=0X00;		pic[9][10]=0X60;
			pic[10][9]=0X00;		pic[10][10]=0X60;
			pic[11][9]=0X00;		pic[11][10]=0X60;
			pic[12][9]=0X00;		pic[12][10]=0X60;
			pic[13][9]=0X00;		pic[13][10]=0X60;
			pic[14][9]=0X01;		pic[14][10]=0XF8;
			pic[15][9]=0X01;		pic[15][10]=0XF8;
		break;
		
		case 2:
			pic[6][9]=0X07;		pic[6][10]=0XF8;
			pic[7][9]=0X07;		pic[7][10]=0XFC;
			pic[8][9]=0X00;		pic[8][10]=0X06;
			pic[9][9]=0X00;		pic[9][10]=0X0E;
			pic[10][9]=0X01;		pic[10][10]=0XFC;
			pic[11][9]=0X03;		pic[11][10]=0XF8;
			pic[12][9]=0X07;		pic[12][10]=0X00;
			pic[13][9]=0X06;		pic[13][10]=0X00;
			pic[14][9]=0X07;		pic[14][10]=0XFE;
			pic[15][9]=0X07;		pic[15][10]=0XFE;
		break;
		
		case 3:
			pic[6][9]=0X07;		pic[6][10]=0XF8;
			pic[7][9]=0X07;		pic[7][10]=0XFC;
			pic[8][9]=0X00;		pic[8][10]=0X0E;
			pic[9][9]=0X00;		pic[9][10]=0X0E;
			pic[10][9]=0X00;		pic[10][10]=0X7C;
			pic[11][9]=0X00;		pic[11][10]=0X7C;
			pic[12][9]=0X00;		pic[12][10]=0X0E;
			pic[13][9]=0X00;		pic[13][10]=0X0E;
			pic[14][9]=0X07;		pic[14][10]=0XFC;
			pic[15][9]=0X07;		pic[15][10]=0XF8;
		break;
		
		case 4:
			pic[6][9]=0X00;		pic[6][10]=0X78;
			pic[7][9]=0X00;		pic[7][10]=0XF8;
			pic[8][9]=0X01;		pic[8][10]=0XD8;
			pic[9][9]=0X03;		pic[9][10]=0X98;
			pic[10][9]=0X07;		pic[10][10]=0X18;
			pic[11][9]=0X06;		pic[11][10]=0X18;
			pic[12][9]=0X07;		pic[12][10]=0XFE;
			pic[13][9]=0X07;		pic[13][10]=0XFE;
			pic[14][9]=0X00;		pic[14][10]=0X18;
			pic[15][9]=0X00;		pic[15][10]=0X18;
		break;
		
		case 5:
			pic[6][9]=0X07;		pic[6][10]=0XFE;
			pic[7][9]=0X07;		pic[7][10]=0XFE;
			pic[8][9]=0X06;		pic[8][10]=0X00;
			pic[9][9]=0X06;		pic[9][10]=0X00;
			pic[10][9]=0X07;		pic[10][10]=0XF8;
			pic[11][9]=0X07;		pic[11][10]=0XFC;
			pic[12][9]=0X00;		pic[12][10]=0X06;
			pic[13][9]=0X00;		pic[13][10]=0X06;
			pic[14][9]=0X07;		pic[14][10]=0XFC;
			pic[15][9]=0X07;		pic[15][10]=0XF8;
		break;
		
		case 6:
			pic[6][9]=0X01;		pic[6][10]=0XFE;
			pic[7][9]=0X03;		pic[7][10]=0XFE;
			pic[8][9]=0X06;		pic[8][10]=0X00;
			pic[9][9]=0X06;		pic[9][10]=0X00;
			pic[10][9]=0X07;		pic[10][10]=0XF8;
			pic[11][9]=0X07;		pic[11][10]=0XFC;
			pic[12][9]=0X06;		pic[12][10]=0X06;
			pic[13][9]=0X06;		pic[13][10]=0X06;
			pic[14][9]=0X03;		pic[14][10]=0XFC;
			pic[15][9]=0X01;		pic[15][10]=0XF8;
		break;
		
		case 7:
			pic[6][9]=0X07;		pic[6][10]=0XFE;
			pic[7][9]=0X07;		pic[7][10]=0XFE;
			pic[8][9]=0X00;		pic[8][10]=0X06;
			pic[9][9]=0X00;		pic[9][10]=0X0E;
			pic[10][9]=0X00;		pic[10][10]=0X1C;
			pic[11][9]=0X00;		pic[11][10]=0X38;
			pic[12][9]=0X00;		pic[12][10]=0X70;
			pic[13][9]=0X00;		pic[13][10]=0XE0;
			pic[14][9]=0X01;		pic[14][10]=0XC0;
			pic[15][9]=0X01;		pic[15][10]=0X80;
		break;
		
		case 8:
			pic[6][9]=0X01;		pic[6][10]=0XF8;
			pic[7][9]=0X03;		pic[7][10]=0XFC;
			pic[8][9]=0X06;		pic[8][10]=0X06;
			pic[9][9]=0X06;		pic[9][10]=0X06;
			pic[10][9]=0X01;		pic[10][10]=0XF8;
			pic[11][9]=0X01;		pic[11][10]=0XF8;
			pic[12][9]=0X06;		pic[12][10]=0X06;
			pic[13][9]=0X06;		pic[13][10]=0X06;
			pic[14][9]=0X03;		pic[14][10]=0XFC;
			pic[15][9]=0X01;		pic[15][10]=0XF8;
		break;
		
		case 9:
			pic[6][9]=0X01;		pic[6][10]=0XF8;
			pic[7][9]=0X03;		pic[7][10]=0XFC;
			pic[8][9]=0X06;		pic[8][10]=0X06;
			pic[9][9]=0X06;		pic[9][10]=0X06;
			pic[10][9]=0X03;		pic[10][10]=0XFE;
			pic[11][9]=0X01;		pic[11][10]=0XFE;
			pic[12][9]=0X00;		pic[12][10]=0X06;
			pic[13][9]=0X00;		pic[13][10]=0X06;
			pic[14][9]=0X07;		pic[14][10]=0XFC;
			pic[15][9]=0X03;		pic[15][10]=0XF8;
		break;
	}
	
		switch(hunderter)
	{
		case 0:
			pic[6][8]=0X1F;		pic[6][9]=pic[6][9] | 0X80;
			pic[7][8]=0X3F;		pic[7][9]=pic[7][9] | 0XC0;
			pic[8][8]=0X71;		pic[8][9]=pic[8][9] | 0XE0;
			pic[9][8]=0X61;		pic[9][9]=pic[9][9] | 0XE0;
			pic[10][8]=0X66;		pic[10][9]=pic[10][9] | 0X60;
			pic[11][8]=0X66;		pic[11][9]=pic[11][9] | 0X60;
			pic[12][8]=0X78;		pic[12][9]=pic[12][9] | 0X60;
			pic[13][8]=0X78;		pic[13][9]=pic[13][9] | 0XE0;
			pic[14][8]=0X3F;		pic[14][9]=pic[14][9] | 0XC0;
			pic[15][8]=0X1F;		pic[15][9]=pic[15][9] | 0X80;
		break;
		
		case 1:
			pic[6][8]=0X1E;		pic[6][9]=pic[6][9] | 0X00;
			pic[7][8]=0X1E;		pic[7][9]=pic[7][9] | 0X00;
			pic[8][8]=0X06;		pic[8][9]=pic[8][9] | 0X00;
			pic[9][8]=0X06;		pic[9][9]=pic[9][9] | 0X00;
			pic[10][8]=0X06;		pic[10][9]=pic[10][9] | 0X00;
			pic[11][8]=0X06;		pic[11][9]=pic[11][9] | 0X00;
			pic[12][8]=0X06;		pic[12][9]=pic[12][9] | 0X00;
			pic[13][8]=0X06;		pic[13][9]=pic[13][9] | 0X00;
			pic[14][8]=0X1F;		pic[14][9]=pic[14][9] | 0X80;
			pic[15][8]=0X1F;		pic[15][9]=pic[15][9] | 0X80;
		break;
		
		case 2:
			pic[6][8]=0X7F;		pic[6][9]=pic[6][9] | 0X80;
			pic[7][8]=0X7F;		pic[7][9]=pic[7][9] | 0XC0;
			pic[8][8]=0X00;		pic[8][9]=pic[8][9] | 0X60;
			pic[9][8]=0X00;		pic[9][9]=pic[9][9] | 0XE0;
			pic[10][8]=0X1F;		pic[10][9]=pic[10][9] | 0XC0;
			pic[11][8]=0X3F;		pic[11][9]=pic[11][9] | 0X80;
			pic[12][8]=0X70;		pic[12][9]=pic[12][9] | 0X00;
			pic[13][8]=0X60;		pic[13][9]=pic[13][9] | 0X00;
			pic[14][8]=0X7F;		pic[14][9]=pic[14][9] | 0XE0;
			pic[15][8]=0X7F;		pic[15][9]=pic[15][9] | 0XE0;
		break;
		
		case 3:
			pic[6][8] = 0X7F;		pic[6][9]=pic[6][9] | 0X80;
			pic[7][8] = 0X7F;		pic[7][9]=pic[7][9] | 0XC0;
			pic[8][8] = 0X00;		pic[8][9]=pic[8][9] | 0XE0;
			pic[9][8] = 0X00;		pic[9][9]=pic[9][9] | 0XE0;
			pic[10][8] = 0X07;		pic[10][9]=pic[10][9] | 0XC0;
			pic[11][8] = 0X07;		pic[11][9]=pic[11][9] | 0XC0;
			pic[12][8] = 0X00;		pic[12][9]=pic[12][9] | 0XE0;
			pic[13][8] = 0X00;		pic[13][9]=pic[13][9] | 0XE0;
			pic[14][8] = 0X7F;		pic[14][9]=pic[14][9] | 0XC0;
			pic[15][8] = 0X7F;		pic[15][9]=pic[15][9] | 0X80;
		break;
		
		case 4:
			pic[6][8] = 0X07;		pic[6][9]=pic[6][9] | 0X80;
			pic[7][8] = 0X0F;		pic[7][9]=pic[7][9] | 0X80;
			pic[8][8] = 0X1D;		pic[8][9]=pic[8][9] | 0X80;
			pic[9][8] = 0X39;		pic[9][9]=pic[9][9] | 0X80;
			pic[10][8] = 0X71;		pic[10][9]=pic[10][9] | 0X80;
			pic[11][8] = 0X61;		pic[11][9]=pic[11][9] | 0X80;
			pic[12][8] = 0X7F;		pic[12][9]=pic[12][9] | 0XE0;
			pic[13][8] = 0X7F;		pic[13][9]=pic[13][9] | 0XE0;
			pic[14][8] = 0X01;		pic[14][9]=pic[14][9] | 0X80;
			pic[15][8] = 0X01;		pic[15][9]=pic[15][9] | 0X80;
		break;
		
		case 5:
			pic[6][8] = 0X7F;		pic[6][9]=pic[6][9] | 0XE0;
			pic[7][8] = 0X7F;		pic[7][9]=pic[7][9] | 0XE0;
			pic[8][8] = 0X60;		pic[8][9]=pic[8][9] | 0X00;
			pic[9][8] = 0X60;		pic[9][9]=pic[9][9] | 0X00;
			pic[10][8] = 0X7F;		pic[10][9]=pic[10][9] | 0X80;
			pic[11][8] = 0X7F;		pic[11][9]=pic[11][9] | 0XC0;
			pic[12][8] = 0X00;		pic[12][9]=pic[12][9] | 0X60;
			pic[13][8] = 0X00;		pic[13][9]=pic[13][9] | 0X60;
			pic[14][8] = 0X7F;		pic[14][9]=pic[14][9] | 0XC0;
			pic[15][8] = 0X7F;		pic[15][9]=pic[15][9] | 0X80;
		break;
		
		case 6:
			pic[6][8] = 0X1F;		pic[6][9]=pic[6][9] | 0XE0;
			pic[7][8] = 0X3F;		pic[7][9]=pic[7][9] | 0XE0;
			pic[8][8] = 0X60;		pic[8][9]=pic[8][9] | 0X00;
			pic[9][8] = 0X60;		pic[9][9]=pic[9][9] | 0X00;
			pic[10][8] = 0X7F;		pic[10][9]=pic[10][9] | 0X80;
			pic[11][8] = 0X7F;		pic[11][9]=pic[11][9] | 0XC0;
			pic[12][8] = 0X60;		pic[12][9]=pic[12][9] | 0X60;
			pic[13][8] = 0X60;		pic[13][9]=pic[13][9] | 0X60;
			pic[14][8] = 0X3F;		pic[14][9]=pic[14][9] | 0XC0;
			pic[15][8] = 0X1F;		pic[15][9]=pic[15][9] | 0X80;
		break;
		
		case 7:
			pic[6][8] = 0X7F;		pic[6][9]=pic[6][9] | 0XE0;
			pic[7][8] = 0X7F;		pic[7][9]=pic[7][9] | 0XE0;
			pic[8][8] = 0X00;		pic[8][9]=pic[8][9] | 0X60;
			pic[9][8] = 0X00;		pic[9][9]=pic[9][9] | 0XE0;
			pic[10][8] = 0X01;		pic[10][9]=pic[10][9] | 0XC0;
			pic[11][8] = 0X03;		pic[11][9]=pic[11][9] | 0X80;
			pic[12][8] = 0X07;		pic[12][9]=pic[12][9] | 0X00;
			pic[13][8] = 0X0E;		pic[13][9]=pic[13][9] | 0X00;
			pic[14][8] = 0X1C;		pic[14][9]=pic[14][9] | 0X00;
			pic[15][8] = 0X18;		pic[15][9]=pic[15][9] | 0X00;
		break;
		
		case 8:
			pic[6][8] = 0X1F;		pic[6][9]=pic[6][9] | 0X80;
			pic[7][8] = 0X3F;		pic[7][9]=pic[7][9] | 0XC0;
			pic[8][8] = 0X60;		pic[8][9]=pic[8][9] | 0X60;
			pic[9][8] = 0X60;		pic[9][9]=pic[9][9] | 0X60;
			pic[10][8] = 0X1F;		pic[10][9]=pic[10][9] | 0X80;
			pic[11][8] = 0X1F;		pic[11][9]=pic[11][9] | 0X80;
			pic[12][8] = 0X60;		pic[12][9]=pic[12][9] | 0X60;
			pic[13][8] = 0X60;		pic[13][9]=pic[13][9] | 0X60;
			pic[14][8] = 0X3F;		pic[14][9]=pic[14][9] | 0XC0;
			pic[15][8] = 0X1F;		pic[15][9]=pic[15][9] | 0X80;
		break;
		
		case 9:
			pic[6][8] = 0X1F;		pic[6][9]=pic[6][9] | 0X80;
			pic[7][8] = 0X3F;		pic[7][9]=pic[7][9] | 0XC0;
			pic[8][8] = 0X60;		pic[8][9]=pic[8][9] | 0X60;
			pic[9][8] = 0X60;		pic[9][9]=pic[9][9] | 0X60;
			pic[10][8] = 0X3F;		pic[10][9]=pic[10][9] | 0XE0;
			pic[11][8] = 0X1F;		pic[11][9]=pic[11][9] | 0XE0;
			pic[12][8] = 0X00;		pic[12][9]=pic[12][9] | 0X60;
			pic[13][8] = 0X00;		pic[13][9]=pic[13][9] | 0X60;
			pic[14][8] = 0X7F;		pic[14][9]=pic[14][9] | 0XC0;
			pic[15][8] = 0X3F;		pic[15][9]=pic[15][9] | 0X80;
		break;
	}
	
	Display_out(pic);
	
}


#endif /* DISPLAY_H_ */