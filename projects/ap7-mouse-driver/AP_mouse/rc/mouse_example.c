
#include "usart.h"
#include "_mouse.h"
#include <sys/cpu.h>

#define SUCCESS 0;
#define FAILURE -1;

typedef unsigned char avr32_piomap_t[][2];

/* enable output on pins */
int pio_enable_module(avr32_piomap_t piomap, unsigned int size);

void delay( void );

struct mouseevent	mouseplace , mouseplace_last;

void m_handel(struct mouseevent* mouseplace1){
	mouseplace = *mouseplace1;
}

int main( void )
{
	set_cpu_hz(20000000);
	
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

    
  	
  	signed int test=0;
 
	avr32_piomap_t psif_piomap = {				   				\
		{AVR32_PSIF_CLOCK_1_PIN, AVR32_PSIF_CLOCK_1_FUNCTION}, 	\
		{AVR32_PSIF_DATA_1_PIN , AVR32_PSIF_DATA_1_FUNCTION }   \
	};
	pio_enable_module(psif_piomap, 2);

	print(usart, "Init Mouse : ");
	test = init_mouse();
	usart_putchar(usart,test);
	print(usart, " OK\n");
	register_mouse_listener(m_handel);

  while(1){

	  if((mouseplace.x != mouseplace_last.x)||(mouseplace.y != mouseplace_last.y)||
	    (mouseplace.middle != mouseplace_last.middle)||(mouseplace.right != mouseplace_last.right)||
	    (mouseplace.left != mouseplace_last.left)){
						  print(usart, "\nMouse X : "); print_long(usart,mouseplace.x);
						  print(usart, "\nMouse Y : "); print_long(usart,mouseplace.y);
						  print(usart, "\nMouse Middle : "); print_long(usart, mouseplace.middle);
						  print(usart, "\nMouse Right  : "); print_long(usart, mouseplace.right);
						  print(usart, "\nMouse left   : "); print_long(usart, mouseplace.left);
						  print(usart, "\n\n");
						  
						  mouseplace_last = mouseplace;
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
