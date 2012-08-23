#include "R347.h"
#include "ascold.h"

volatile int Timeout_counter;
unsigned char String_Buffer[64];

void Init_Ports (void)
{
    on(LED_DDR);
    on(RFD_M0_DDR);
    on(RFD_M1_DDR);
    on(RFD_M2_DDR);
    on(RFD_FAC_PROG_DDR);
    on(RFD_LEARN_DDR);
    off(RFD_RESET);
    on(RFD_RESET_DDR);
    on(RFD_PWR_DDR);
    on(RFD_LOGIC_IO_DDR);
} // End proc

void Init_USART (unsigned int ubrr)
{
    // Set baud rate
    UBRR0H = (unsigned char)(ubrr>>8);
    UBRR0L = (unsigned char)ubrr;
    // Enable RX Complete Interrupt, Receiver and Transmitter
	UCSR0B =  ( (1 << RXEN0)|(1 << TXEN0) );
    // Asynchronous USART, Parity Disable, Stop Bit 1, Set frame format: 8data
    UCSR0C = (unsigned char)(3 << UCSZ00);
} // End proc

void Init_Timer1 (void)
{
    OCR1A = 1000;
    OCR1B = 0;
    ICR1 = 0;
    TCNT1 = 0;
    TCCR1A = 0;                                              // CTC mode
    TCCR1B = (unsigned char)((1 << WGM12)|(1 << CS11));      // clk = fclk/8 (T = 1ms with 1,0MHz fclk)
    TCCR1C = 0;
    TIMSK1 |= (unsigned char)(1 << OCIE1A);                    // Output Compare A Match Interrupt Enable
    TIFR1 |= (unsigned char)((1 << ICF1)|(1 << OCF1A)|(1 << OCF1B)|(1 << TOV1));  // Reset all interrupt vectors
	sei();
} // End proc

//#pragma vector = TIMER1_COMPA_vect // Compare Match A
//__interrupt void timer1_compa_isr (void)
ISR(TIMER1_COMPA_vect)
{
    if (Timeout_counter > 0)
    {
        Timeout_counter--;
    }
} // End proc

void RFD_Set_Mode (unsigned char Mode)
{
    on(RFD_RESET);  // Reset RFD
    switch(Mode)
    {
    case 0:
        off(RFD_M0);
        off(RFD_M1);
        off(RFD_M2);
        break;
    case 1:
        on(RFD_M0);
        off(RFD_M1);
        off(RFD_M2);
        break;
    case 2:
        off(RFD_M0);
        on(RFD_M1);
        off(RFD_M2);
        break;
    case 3:
        on(RFD_M0);
        on(RFD_M1);
        off(RFD_M2);
        break;
    case 4:
        off(RFD_M0);
        off(RFD_M1);
        on(RFD_M2);
        break;
    case 5:
        on(RFD_M0);
        off(RFD_M1);
        on(RFD_M2);
        break;
    case 6:
        off(RFD_M0);
        on(RFD_M1);
        on(RFD_M2);
        break;
    case 7:
        on(RFD_M0);
        on(RFD_M1);
        on(RFD_M2);
        break;
    }
    off(RFD_RESET);  // Release Reset RFD
} // End proc

#define F_BYTE_RECEIVE_ERROR 0
#define F_BYTE_RECEIVE_OK    1
unsigned char USART_Receive_Byte (unsigned char* byte)
{
    // Wait for data to be received
    Timeout_counter = 100; // Timeout 100ms
    while ( !(UCSR0A & (1<<RXC0)) )
    {
        if (Timeout_counter == 0)
        {
            return(F_BYTE_RECEIVE_ERROR);
        }
    }
    // Get and return received data from buffer
    *byte = UDR0;
    return(F_BYTE_RECEIVE_OK);
} // End proc

#define F_BYTE_TRANSMIT_ERROR 0
#define F_BYTE_TRANSMIT_OK    1
unsigned char USART_Transmit_Byte (unsigned char* byte)
{
    // Wait for empty transmit buffer
    Timeout_counter = 100; // Timeout 100ms
    while ( !( UCSR0A & (1<<UDRE0)))
    {
        if (Timeout_counter == 0)
        {
            return(F_BYTE_TRANSMIT_ERROR);
        }
    }
    // Put data into buffer, sends the data
    UDR0 = *byte;
    return(F_BYTE_TRANSMIT_OK);
} // End proc

#define F_ESN_READ_ERROR 0
#define F_ESN_READ_OK    1
unsigned char RFD_Read_ESN (unsigned char* str)
{
    unsigned char chr;
    on(RFD_RESET);
    on(RFD_LEARN);
    off(RFD_RESET);
	_delay_ms(250);
   // __delay_cycles(250*(MCLK/1000));   // Wait 250ms
    off(RFD_LEARN);
    chr = 'R'; USART_Transmit_Byte(&chr);
    chr = 'E'; USART_Transmit_Byte(&chr);
    chr = 'A'; USART_Transmit_Byte(&chr);
    chr = 'D'; USART_Transmit_Byte(&chr);
    chr = ' '; USART_Transmit_Byte(&chr);
    chr = 'E'; USART_Transmit_Byte(&chr);
    chr = 'S'; USART_Transmit_Byte(&chr);
    chr = 'N'; USART_Transmit_Byte(&chr);
    while (*str != '$')
    {
        if (USART_Receive_Byte(str++) == F_BYTE_RECEIVE_ERROR)
        {
            return(F_ESN_READ_ERROR);
        }
    }
    return(F_ESN_READ_OK);
} // End proc

void WDT_off (void)
{
    //__disable_interrupt();
	cli();
    //__watchdog_reset();
	wdt_reset();
    /* Clear WDRF in MCUSR */
    MCUSR &= ~(1<<WDRF);
    /* Write logical one to WDCE and WDE */
    /* Keep old prescaler setting to prevent unintentional time-out */
    WDTCSR |= (1<<WDCE) | (1<<WDE);
    /* Turn off WDT */
    WDTCSR = 0x00;
} // End proc

void Power_Down (void)
{
    WDT_off();
    //__disable_interrupt();
	cli();
    DDRA = 0x00;
    DDRB = 0x00;
    DDRC = 0x00;
    DDRD = 0x00;
    PORTA = 0x00;
    PORTB = 0x00;
    PORTC = 0x00;
    PORTD = 0x00;
    ADCSRA = 0x00; // Disable ADC
    ACSR = (unsigned char)(1 << ACD); // Disable Analog comparator
    DIDR1 = (unsigned char)((1 << AIN1D)|(1 << AIN0D)); // AIN1:AIN0 Digital Input Disable
    DIDR0 = 0xFF; // ADC7:0 Digital Input Disable
    UCSR0B &= ~((1 << RXCIE0)|(1 << RXEN0)|(1 << TXEN0));
    SMCR = (unsigned char)((1 << SM1)|(1 << SE)); // Power Down Mode
    sleep_enable();
} // End proc

void main (void)
{
    unsigned char byte, i, chr;
	unsigned char testbyte = 'Z';
    Init_Ports();
    Init_USART(MYUBRR);
    Init_Timer1();
    on(RFD_PWR);  // Power ON RFD module
    RFD_Set_Mode(2);
   //__enable_interrupt();
	sei();
    for(;;)
    {
        // LED and Button testing
        if (signal(SW))
        {			
			Timeout_counter = 5000;
			while(Timeout_counter > 0)
			{
				on(LED);
			}
			off(LED);
			USART_Transmit_Byte(&testbyte);
        }
        else
        {
            off(LED);
        }
        // Radio testing
					
        if (USART_Receive_Byte(&byte) == F_BYTE_RECEIVE_OK)
        {
            if ( (byte == 'e') || (byte == 'E') )
            { // Send ESN mode
                if (RFD_Read_ESN(String_Buffer) == F_ESN_READ_OK)
                { // Send ESN
                    for (i = 0; ( (String_Buffer[i] != '$') && (i < sizeof(String_Buffer)) ); i++)
                    {
                         USART_Transmit_Byte(String_Buffer + i);
                    }
                }
                else
                {
                    chr = 'E'; USART_Transmit_Byte(&chr);
                    chr = 'S'; USART_Transmit_Byte(&chr);
                    chr = 'N'; USART_Transmit_Byte(&chr);
                    chr = ' '; USART_Transmit_Byte(&chr);
                    chr = 'E'; USART_Transmit_Byte(&chr);
                    chr = 'R'; USART_Transmit_Byte(&chr);
                    chr = 'R'; USART_Transmit_Byte(&chr);
                }
            }
            else if ( (byte == 'r') || (byte == 'R') )
            { // Radio off mode
                off(RFD_M0);
                off(RFD_M1);
                off(RFD_M2);
                off(RFD_FAC_PROG);
                off(RFD_LEARN);
                on(RFD_RESET);
                off(RFD_PWR);  // Power OFF RFD module
            }
            else if ( (byte == 's') || (byte == 'S') )
            { // Sleep mode
                Power_Down();
            }
            else
            { // Echo mode
				if(byte == 'Z')
				{
					Timeout_counter = 5000;
					while(Timeout_counter > 0)
					{
						on(LED);
					}
					off(LED);
					USART_Transmit_Byte(&byte);
				}					
                
            }
        }
    }
} // End main proc


