/*  ATmega16MIDI.h   

Atmel AVR Design Contest 2006 Registration Number AT3337 

*/
#define UART_BAUD_REG              UBRRL
#define UART_BAUD_REG_H            UBRRH
#define UART_CONTROL_REG           UCSRB
#define UART_STATUS_REG            UCSRA   
#define UART_RCV_INT_VECTOR        SIG_UART_RECV
#define UART_REG_EMPTY_INT_VECTOR  SIG_UART_DATA
#define UART_DATA_REG              UDR
// Keyboard communication routines
#define ISC00 0
#define ISC01 1
// Keyboard connections
#define PIN_KB  PIND
#define PORT_KB PORTD
#define CLOCK   2
#define DATAPIN 3

#define UDRIE 5

/* Maps GCC-AVR  variables to standard data types  */
typedef unsigned char  BOOLEAN;
typedef unsigned char  CHARU;   /* Unsigned  8 bit quantity  */
typedef signed char    CHARS;   /* Signed    8 bit quantity  */
typedef unsigned int   INT16U;  /* Unsigned 16 bit quantity  */
typedef int            INT16S;  /* Signed   16 bit quantity   */
typedef unsigned long  INT32U;  /* Unsigned 32 bit quantity  */
typedef long           INT32S;  /* Signed   32 bit quantity  */
typedef float          FP32;    /* Single precision floating point  */
typedef double         FP64;    /* Double precision floating point  */
typedef enum 
    {
        BAUD115K = 3, BAUD76K = 6, BAUD57K = 8,
    	BAUD38K = 12, BAUD31250 = 15, BAUD19K = 25, BAUD14K = 34,
    	BAUD9600 = 51, BAUD4800 = 103, BAUD2400 = 207, 
    	BAUD1200 = 414, BAUD600 = 818, BAUD300 = 1636
    }   BaudRate;

void initialize(void);
void InitDisplay(void);
void DisplayChar(char);
void DisplayChar_P(const char *addr);
void LCDInstruction(char data);
void PWM1change(void);
void PWM2change(void);
void PlayKeyboard(void);
void MIDIStartFast(void);
void AllNotesOff(void);
void pwmInitialize(void);
void parseInput(char *);
void pwmInit(void);
void setOCR0(unsigned char count);
void setOCR1A(int count);
void setICR1(int count);
void setICR1_P(int code);
void setbaud(BaudRate);
void init_kb(void);
void decode(unsigned char sc);
void put_kbbuff(unsigned char c);
int  getchar(void);
void init_uart(void);
int  putchar(int c);
void print_hexbyte(unsigned char i);
void SinglePulse(void);
void MultiFreq(void);


