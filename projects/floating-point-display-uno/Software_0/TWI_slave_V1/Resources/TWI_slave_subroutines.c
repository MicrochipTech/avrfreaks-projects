

//Addresses UNO IS 3 Master sends 7 for data from UNO  (master read) 
//or 6 for data to UNO (master write)

//Address of slave Attiny861 is 4.  Receive 8 to receive data (Master write)
//or 9 to send data (master read).

#define F_CPU 8000000UL
#include <util/delay.h>

unsigned char TWI_slaveAddress;
static volatile unsigned char USI_TWI_Overflow_State;


unsigned char Tx_data[20];
unsigned char Rx_data[4];
volatile int Tx_data_ptr;
volatile int Rx_data_ptr;


volatile char USI_busy;														//My line  used for data flow


 #define DDR_USI             DDRA
 #define PORT_USI            PORTA
 #define PIN_USI             PINA
 #define PORT_USI_SDA        PORTA0
 #define PORT_USI_SCL        PORTA2
 #define PIN_USI_SDA         PINA0
 #define PIN_USI_SCL         PINA2


#define USI_SLAVE_CHECK_ADDRESS                (0x00)
#define USI_SLAVE_SEND_DATA                    (0x01)
#define USI_SLAVE_REQUEST_REPLY_FROM_SEND_DATA (0x02)
#define USI_SLAVE_CHECK_REPLY_FROM_SEND_DATA   (0x03)
#define USI_SLAVE_REQUEST_DATA                 (0x04)
#define USI_SLAVE_GET_DATA_AND_SEND_ACK        (0x05)
#define USI_START_COND_INT  USISIF


#define SET_USI_TO_SEND_ACK()																					\
	{USIDR    =  0;                                             /* Prepare ACK								*/	\
	DDR_USI |=  (1<<PORT_USI_SDA);                              /* Set SDA as output						*/	\
	USISR    =  (0<<USI_START_COND_INT)|(1<<USIOIF)|															\
	(1<<USIPF)|(1<<USIDC)|										/* Clear all flags, except Start Condition  */	\
	(0x0E<<USICNT0);}											/* set USI counter to shift 1 bit. */ 

#define SET_USI_TO_TWI_START_CONDITION_MODE()																						\
{  	USICR    =  (1<<USISIE)|(0<<USIOIE)|                        /* Enable Start Condition Interrupt. Disable Overflow Interrupt.*/  \
	(1<<USIWM1)|(0<<USIWM0)|									/* Set USI in Two-wire mode. No USI Counter overflow hold.      */  \
	(1<<USICS1)|(0<<USICS0)|(0<<USICLK)|						/* Shift Register Clock Source = External, positive edge        */  \
	(0<<USITC);																														\
																																	\
		USISR    =  (0<<USI_START_COND_INT)|\
	(1<<USIOIF)|(1<<USIPF)|(1<<USIDC)|							 /* Clear all flags, except Start Condition           */ \
	(0x0<<USICNT0);\
	USI_busy = 0;}												//Transaction complete: Exit busy state
	

#define SET_USI_TO_SEND_DATA()																					\
{	DDR_USI |=  (1<<PORT_USI_SDA);                              /* Set SDA as output                  */		\
	USISR    =  (0<<USI_START_COND_INT)|																		\
	(1<<USIOIF)|(1<<USIPF)|(1<<USIDC)|							/* Clear all flags, except Start Condition */	\
	(0x0<<USICNT0);	}											/* set USI to shift out 8 bits        */		\


#define SET_USI_TO_READ_DATA()																					\
{ 	DDR_USI &= ~(1<<PORT_USI_SDA);                              /* Set SDA as input                   */		\
	USISR    =  (0<<USI_START_COND_INT)|\
	(1<<USIOIF)|(1<<USIPF)|(1<<USIDC)|							/* Clear all flags, except Start Condition */	\
	(0x0<<USICNT0);												/* set USI to shift out 8 bits        */		\
}


#define SET_USI_TO_READ_ACK()																						\
{ 	DDR_USI &=  ~(1<<PORT_USI_SDA);                             /* Set SDA as input */								\
	USIDR    =  0;                                              /* Prepare ACK        */							\
	USISR    =  (0<<USI_START_COND_INT)|																			\
	(1<<USIOIF)|(1<<USIPF)|(1<<USIDC)|							 /* Clear all flags, except Start Condition  */		\
	(0x0E<<USICNT0);}											/* set USI counter to shift 1 bit. */				\




/***************************************************************************************************************************************/
void USI_TWI_Slave_Initialise(unsigned char TWI_ownAddress )
{
	TWI_slaveAddress = TWI_ownAddress;

	PORT_USI |=  (1<<PORT_USI_SCL);                                 // Set SCL high (WPU)
	PORT_USI |=  (1<<PORT_USI_SDA);                                 // Set SDA high (WPU)
	DDR_USI  |=  (1<<PORT_USI_SCL);                                 // Set SCL as output (HIGH)
	DDR_USI  &= ~(1<<PORT_USI_SDA);                                 // Set SDA as input (HIGH)
	USICR    =  (1<<USISIE)|(0<<USIOIE)|                            // Enable Start Condition Interrupt. Disable Overflow Interrupt.
				(1<<USIWM1)|(0<<USIWM0)|							// Set USI in Two-wire mode. No USI Counter overflow prior
																	// to first Start Condition (potential failure)
				(1<<USICS1)|(0<<USICS0)|(0<<USICLK)|				// Shift Register Clock Source = External, positive edge
				(0<<USITC);
				USISR    = 0xF0;									// Clear all flags and reset overflow counter
				USI_busy = 0;										//My line  used for data flow
}











/***************************************************************************************************************************************/
ISR (USI_START_vect)
{	
	unsigned char tmpUSISR;											// Temporary variable to store volatile
	tmpUSISR = USISR;												// Not necessary, but prevents warnings
																	// Set default starting conditions for new TWI package
	USI_TWI_Overflow_State = USI_SLAVE_CHECK_ADDRESS;
	DDR_USI  &= ~(1<<PORT_USI_SDA);									// Set SDA as input
	while ( (PIN_USI & (1<<PORT_USI_SCL)) &\
	 !(tmpUSISR & (1<<USIPF)) );									// Wait for SCL to go low to ensure the "Start Condition" has completed.
	
	_delay_us(5);													//Additional delay added for more reliable operation
																	//for slower TWI clocks
		
	USICR   =   (1<<USISIE)|(1<<USIOIE)|							// Enable Overflow and Start Condition Interrupt. (Keep StartCondInt to detect RESTART)
	(1<<USIWM1)|(1<<USIWM0)|										// Set USI in Two-wire mode.
	(1<<USICS1)|(0<<USICS0)|(0<<USICLK)|							// Shift Register Clock Source = External, positive edge
	(0<<USITC);
	
	USISR  =    (1<<USI_START_COND_INT)|\
	(1<<USIOIF)|(1<<USIPF)|(1<<USIDC)|								// Clear flags INCLUDING the start condition interrupt fag
	(0x0<<USICNT0);													// Set USI to sample 8 bits i.e. count 16 external pin toggles.
}


ISR (USI_OVF_vect)
{	
	switch (USI_TWI_Overflow_State)												//Transaction starts here
	{	case USI_SLAVE_CHECK_ADDRESS:
		if ((USIDR == 0) || (( USIDR>>1 ) == TWI_slaveAddress))
		{	USI_busy = 1;														//Own address detected: Enter busy state
			if ( USIDR & 0x01 )
			{USI_TWI_Overflow_State = USI_SLAVE_SEND_DATA;						//Master requires data								
			Tx_data_ptr = 0;}													//Initialise Tx_data pointer
			else
			USI_TWI_Overflow_State = USI_SLAVE_REQUEST_DATA;					//Master has data for slave
			Rx_data_ptr = 0;													//Initialise receive data buffer pointer
			SET_USI_TO_SEND_ACK();}
		else
		{SET_USI_TO_TWI_START_CONDITION_MODE();}break;							//Abort transaction: Not our address
		
		
		/************************************************************************/
		case USI_SLAVE_CHECK_REPLY_FROM_SEND_DATA:
		if ( USIDR )															// If NACK, the master does not want more data.
		{	
			SET_USI_TO_TWI_START_CONDITION_MODE();return;}
		
		
		/************************************************************************/
		case USI_SLAVE_SEND_DATA:
		if (Tx_data[Tx_data_ptr]) 
		{USIDR = Tx_data[Tx_data_ptr];  Tx_data_ptr += 1;}							//Tx data string is terminated in a null
		
		
		/****************************************************************/
		
		else {
		SET_USI_TO_TWI_START_CONDITION_MODE();	return;}							//Exit when all data sent or NACK received
			
		USI_TWI_Overflow_State = USI_SLAVE_REQUEST_REPLY_FROM_SEND_DATA;
		SET_USI_TO_SEND_DATA();
		break;
		
		/************************************************************************/
		case USI_SLAVE_REQUEST_REPLY_FROM_SEND_DATA: 
		USI_TWI_Overflow_State = USI_SLAVE_CHECK_REPLY_FROM_SEND_DATA;
		SET_USI_TO_READ_ACK();
		break;

		
		
		/************************************************************************/
		case USI_SLAVE_REQUEST_DATA:
		USI_TWI_Overflow_State = USI_SLAVE_GET_DATA_AND_SEND_ACK;
		SET_USI_TO_READ_DATA();
		break;

		
		/************************************************************************/
		case USI_SLAVE_GET_DATA_AND_SEND_ACK:
		
		Rx_data[Rx_data_ptr] = USIDR;												//Load receive data buffer
		Rx_data_ptr += 1;															//Increment buffer pointer
		if (Rx_data_ptr == 4)														//Buffer can only hold 4 data items
		{//Rx_data_ptr = 0;
			SET_USI_TO_TWI_START_CONDITION_MODE();return;};							//Terminate transmission and return to ready state
		USI_TWI_Overflow_State = USI_SLAVE_REQUEST_DATA;
		SET_USI_TO_SEND_ACK();
		break;
	}
}