//************************************************************************
// avrio.h	defines functions and variables
// 
//************************************************************************


// external function prototypes

extern void	usart_init(void);
void	usart_tx0_byte(uint8_t data);
void	usart_tx1_byte(uint8_t data);
extern void usart_tx0_string(char *address);
extern void usart_tx1_string(char *address);
extern uint8_t get_pc_command();

extern void lcd(char *string, uint8_t line_no, uint8_t char_no);
extern uint16_t adc_read();
extern void adc_init(void);

uint8_t quick_wait(uint8_t delay);
void delay_wait(uint8_t delay);
void char_to_lcd(uint8_t data);
void to_lcd(uint8_t data, uint8_t poll);
void init_lcd(int mode);


#define SWA		0x40
#define SWB		0x80

#define	IBUF0_SIZE	128		// input buffer size
#define	OBUF0_SIZE	64		// output buffer size

#define	IBUF1_SIZE	16		// input buffer size
#define	OBUF1_SIZE	16		// output buffer size


uint8_t i_buf0[IBUF0_SIZE];	// USART input circular buffer
volatile uint8_t i_wrptr0;	// USART input write pointer
volatile uint8_t i_rdptr0;	// USART input read pointer

uint8_t o_buf0[OBUF0_SIZE];	// USART output circular buffer
volatile uint8_t o_wrptr0;	// USART output write pointer
volatile uint8_t o_rdptr0;	// USART output read pointer

uint8_t i_buf1[IBUF1_SIZE];	// USART input circular buffer
volatile uint8_t i_wrptr1;	// USART input write pointer
volatile uint8_t i_rdptr1;	// USART input read pointer

uint8_t o_buf1[OBUF1_SIZE];// USART output circular buffer
volatile uint8_t o_wrptr1;	// USART output write pointer
volatile uint8_t o_rdptr1;	// USART output read pointer


uint8_t command_mode;		// For LCD code, 1 => command, 0 => data
