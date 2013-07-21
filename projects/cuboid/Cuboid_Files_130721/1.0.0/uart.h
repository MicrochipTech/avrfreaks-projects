/*
	Uart module interface
*/

//********************************************************************************************************
// Public defines
//********************************************************************************************************

//********************************************************************************************************
// Public variables
//********************************************************************************************************

//      point this at a void function(char)
//      UART receiving ISR will call it and pass received bytes

        extern void (*uart1_rx_ptr)(char x);

//********************************************************************************************************
// Public prototypes
//********************************************************************************************************

//	initialise uart1
	void uart1_init(void);

//	transmit a char
	void uart1_tx_char(char x);
	
//	transmit a string
	void uart1_tx_string(char* text);
	
//	transmit a string from program memory
	void uart1_tx_string_P(PGM_P text);
