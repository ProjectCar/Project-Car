
#define w_register(x) (0b00100000 | x)
#define r_register(x) (0b00000000 | x)
#define nop 0xFF

#define DDR_SPI DDRB
#define DD_SCK	DDRB1
#define DD_MOSI DDRB2

#define EM 0xFF
#define MM 0x0F

volatile char r_data1 = 0;
volatile char r_data2 = 0;
volatile char r_data3 = 0;
volatile char r_data4 = 0;
volatile char r_data5 = 0;
volatile uint32_t motordata = 0;								// Init von diversen Variabeln
volatile int m_data1 = 0;
volatile int m_data2 = 0;
volatile int transmit = 0;

void init_radio(void);
void receive_mode(void);
uint32_t receive_data(void);
int SPI_tranceive(int transmit);
void transmit_mode(char mode, int data);						// Init aller Funktionen
void twi_init(void);
void twi_transmit(char adress, char mode, int data);
int twi_receive(char r_adress, char r_mode);
void decode(void);