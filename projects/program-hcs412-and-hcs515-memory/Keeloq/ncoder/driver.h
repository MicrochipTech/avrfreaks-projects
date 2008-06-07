
//define times for programming the HCS412

#define 	tps  3  	//program mode setup time 3mS  (min. 2mS max. 5mS)
#define 	tph1 5	//hold time 1 5mS (min. 4mS)
#define 	tph2 60	//hold time 2 60us (min. 50us)
#define 	tpbw 5	//bulk write time 5mS (min. 4mS)
#define 	tclk 60  //clock high time 50uS (min. 50uS)
#define 	twc  50  //program cycle time 55mS (min. 50mS)
#define 	tack 800 //acknowledge time 800uS (min.800uS)

//define pins for programming
#define PORT_OUT		PORTB
#define PORT_DIR		DDRB
#define PORT_IN		PINB
#define DATA	   	PB1
#define CLK		   	PB0
#define POWER			PB2


//protypes
uint8_t port_init(void);
void send_bit(uint8_t);
void send_word(uint16_t);
uint16_t get_word(void);
