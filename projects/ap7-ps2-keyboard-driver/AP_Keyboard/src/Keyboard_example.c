
#include "usart.h"
#include "_keyboard.h"
#include <sys/cpu.h>

#define SUCCESS 0;
#define FAILURE -1;

typedef unsigned char avr32_piomap_t[][2];

/* enable output on pins */
int pio_enable_module(avr32_piomap_t piomap, unsigned int size);

void delay( void );

struct keyevent	  keydata , keydata_last;

void m_handel(struct keyevent* keydata1){
	keydata = *keydata1;
}

int main( void )
{
	set_cpu_hz(20000000);
	/*
	volatile struct pll_opt_t pll_opt;
	pll_opt.oscillator 	= PM_OSC0;
	pll_opt.pll			= PM_PLL0;
	pll_opt.multiplier	= 2;
	pll_opt.divider		= 1;
	pm_set_pll(&pll_opt);
	pm_set_mclk_source(PM_PLL0);
	*/
	
	struct usart_options_t opt;
	volatile struct avr32_usart_t *usart = &AVR32_USART1;
	avr32_piomap_t usart_piomap = {				   \
		{AVR32_USART1_RXD_0_PIN, AVR32_USART1_RXD_0_FUNCTION}, \
		{AVR32_USART1_TXD_0_PIN, AVR32_USART1_TXD_0_FUNCTION}   \
	};
	// Set options for the USART
	opt.baudrate = 115200;
  	opt.charlength = 8;
  	opt.paritytype = USART_NO_PARITY;
  	opt.stopbits = USART_1_STOPBIT;
  	opt.channelmode = USART_NORMAL_CHMODE;
  	// Initialize it in RS232 mode
  	usart_init_rs232(usart, &opt, CPU_HZ);
  	// Setup pio for USART
  	pio_enable_module(usart_piomap, 2);
  	print(usart, "Init USART ... OK\n");
  	
  	print(usart, "CPU Frequency is ");
  	print_ulong(usart,CPU_HZ/1000000);
  	print(usart, " MHz\n");

 	avr32_piomap_t psif_piomap = {				   \
		{AVR32_PSIF_CLOCK_0_PIN, AVR32_PSIF_CLOCK_0_FUNCTION}, \
		{AVR32_PSIF_DATA_0_PIN , AVR32_PSIF_DATA_0_FUNCTION }   \
	};
	pio_enable_module(psif_piomap, 2);


	//print(usart, "Init Keyboard : ");
	init_kbd();
	//print(usart, " OK\n");
	register_kbd_listener(m_handel);

    //init_interrupts();
	udelay(1000);
	ps2_kbd_setled(1,1,1);
	udelay(500000);
	ps2_kbd_setled(0,0,0);
  while(1){

	  //udelay(1000000);
	  if((keydata.key != keydata_last.key)||(keydata.release != keydata_last.release)||
	     (keydata.shift_down != keydata_last.shift_down)||(keydata.ctrl_down != keydata_last.ctrl_down)||
	     (keydata.alt_down != keydata_last.alt_down)){
						  print(usart, "\nKey	  : "); 	print_long(usart,keydata.key);
						  print(usart, "\nRelease   : "); 	print_long(usart,keydata.release);
						  print(usart, "\nSHIFT_down : "); 	print_long(usart, keydata.shift_down);
						  print(usart, "\nCTRL_down  : "); 	print_long(usart, keydata.ctrl_down);
						  print(usart, "\nALT_down   : "); 	print_long(usart, keydata.alt_down);
						  print(usart, "\n\n");
						  keydata_last = keydata;
	  }
  }
  
  return SUCCESS;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
int pio_enable_module(avr32_piomap_t piomap, unsigned int size)
{
  int i;
  volatile struct avr32_pio_t *pio;

  /* get the base address for the port */
  switch (**piomap/32) {

  case 0:
    pio = &AVR32_PIOA;
    break;
  case 1:
    pio = &AVR32_PIOB;
    break;
  case 2:
    pio = &AVR32_PIOC;
    break;
  case 3:
    pio = &AVR32_PIOD;
    break;
  case 4:
    pio = &AVR32_PIOE;
    break;
  default :
    return FAILURE;

  }

  for(i=0; i<size; i++){

    pio->pdr |= ( 1<<( (**piomap) % 32) );
    pio->pudr |= ( 1<<( (**piomap) % 32) );

    switch( *(*piomap+1) ){    
    case 0:
      pio->asr |= ( 1<<( (**piomap) % 32) );
      break;
    case 1:
      pio->bsr |= ( 1<<( (**piomap) % 32) );
      break;
    default:
      return FAILURE;
    }

    ++piomap;

  }
  
  return SUCCESS;
}
