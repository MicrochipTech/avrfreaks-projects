/**
 * PS2 Mouse driver for the AVR32-Mistral debug port
 * To use this driver you must first call init_mouse();
 * Then you must register mouse-handler function.
 * If you unplug the mouse you must call init_mouse again.
 *
 * TODO:
 * Further work on this driver could include:
 * - scroll-wheel support 
 * - mouse hotplugging.
 *
 * Author: Sverre Sundsdal (summerstudent 2004)
 */

#include "_mouse.h"
#include "sys/unistd.h"
#include "sys/interrupts.h"
#include <sys/cpu.h>

#include "usart.h"
extern	struct usart_options_t opt;
extern volatile struct avr32_usart_t *usart = &AVR32_USART1;

volatile struct avr32_psif_t  *psif  = &AVR32_PSIF  ;

int i=0;
/*
 * The mouse listening function
 */
static void (*mouse_listener)(struct mouseevent*) = NULL;
volatile static int mouse_acking; //waiting for ack
volatile static int mouse_sending; //in sending mode
volatile static int mouse_data[10]; //send/recv buffer
volatile static int mouse_data_recv; //has data been recieved?
static int mouse_byte_nr; //byte nr (of maximal 3)
static int mouse_x; //relative x movement
static int mouse_y; //relative y movement
static int mouse_mid_btn; //middle button pressed
static int mouse_right_btn; //right button pressed
static int mouse_left_btn; //left button pressed
static int mouse_reading = 0; //are we reading mouse data?
/*
 * Mouse interrupt handler
 * Generates a mouseevent and sends to mouse_listener
 */
static void ps2_mouse_interrupt(int int_grp){


  if(mouse_sending){
    //in sending mode
    mouse_sending = 0; //now ready to recieve
    goto out;
  }

   	if(psif->sr1&&0x00000010){
   		if(i<8) i++;
   		mouse_data[i] = psif->rhr1;
   	}
   	else{
  		goto out;
   	}

  mouse_data_recv = 1;//data recieved
  if(!mouse_reading){ //reading regular mouse data ?
/*
	print(usart, "\nmouse_data int ");
	print_ulong(usart,i);
	print(usart, ": ");
	print_ulong(usart,mouse_data[i]);
*/
	udelay(1000);
	
    if(mouse_acking){ //waiting for ack
      if(mouse_data[i] != MOUSE_ACK){
    	  print(usart , "\nMouse: ERROR - not supposed to ACK now\n");
      }
      mouse_acking = 0; //ack recieved
      mouse_byte_nr=0;
      goto out;
    }
    if(mouse_data[i] == MOUSE_ERROR){
      //No error handling
      goto out;
    }
  }
  else {
    switch(mouse_byte_nr){
    case 0:
      if(!(mouse_data[i]&1<<3)){ //third bit in byte 0 always 1
    	  print(usart , "\nMouse: Protocol mistake?\n");
    	  mouse_byte_nr = 0;
    	  goto out;
      }
      mouse_x = mouse_data[i] & (1<<MOUSE_XS)?1:0;  //if x signed, then mouse_x = 1
      mouse_y = mouse_data[i] & (1<<MOUSE_YS)?1:0;   //same as x
      //button info
      mouse_right_btn = mouse_data[i] & (1<<MOUSE_RIGHT)?1:0; 
      mouse_left_btn  = mouse_data[i] & (1<<MOUSE_LEFT) ?1:0;
      mouse_mid_btn   = mouse_data[i] & (1<<MOUSE_MID)  ?1:0;
      mouse_byte_nr++;
      break;
    case 1:
      mouse_x = mouse_x ? -(255-mouse_data[i]): mouse_data[i];  //invert data if signed, else regiser
      mouse_byte_nr++;
      break;
    case 2: 
      mouse_y = mouse_y ? -(255-mouse_data[i]): mouse_data[i];
      mouse_byte_nr = 0;
      if(mouse_listener){
    	  //report data
    	  struct mouseevent event; //the mouseevent to send to user-program
    	  event.x = mouse_x;
    	  event.y = mouse_y;
    	  event.right = mouse_right_btn;
    	  event.left = mouse_left_btn;
    	  event.middle = mouse_mid_btn;
    	  (*mouse_listener)(&event);
      }
      break;
    }
  }

  out:

  ps2_writel(ps2_readl(PS2M_INT_MASK) | (1<<INT_PS2M_FLAG), PS2M_INT_EN);
  //interrupt handeled.

}
/**
 * This function enables the debugport and interrupt lines.
 * It resets the mouse and enables data-reporting.
 */
int init_mouse(){
	int delay = 10000000;
	int counter = delay;
  print(usart , "\nMouse initializing...\n");

  //Prescaler
  psif->psr1 = 0x00FF;

  psif->cr1 = PS2_EN; 
  psif->ier1 = psif->imr1 | (1<<INT_PS2M_FLAG);
  /*set_interrupts_base( (void *) AVR32_INTC_ADDRESS );
  init_interrupts();*/
  register_interrupt((__int_handler)ps2_mouse_interrupt, AVR32_PSIF_IRQ/32,AVR32_PSIF_IRQ%32, INT1);

  mouse_acking = 1;
  mouse_sending = 1;
  psif->thr1 = MOUSE_RESET;
  while(mouse_acking && --counter){
  }
  if(!counter) goto out;


  mouse_data_recv = 0;
  counter = delay;
  while(!mouse_data_recv && --counter){ //wait for self test passed;
  }
  if(!counter) goto out;
  if(mouse_data[2]!=MOUSE_BAT_OK) goto out;


  mouse_data_recv = 0;
  counter = delay;
  //while(!mouse_data_recv && --counter){
    //wait for device id;
  //}
  while(i<3 && --counter){
  }
  if(!counter) goto out;
  if(mouse_data[3]!=0x00) goto out;


  mouse_acking = 1;
  //mouse_sending = 1;
  counter = delay;
  psif->thr1 = MOUSE_DEFAULT;	//246
  while(mouse_acking && --counter){
  }
  if(!counter) goto out;


  psif->thr1 = MOUSE_ENABLE;	//244
  mouse_acking = 1;
  //mouse_sending= 1;
  counter = delay;
  while(mouse_acking && --counter){
  }
  if(!counter) goto out;


  mouse_reading = 1;
  print(usart , "\nMouse initialized\n");
  return 0;
 out:
  ps2_writel(0, PS2M_CTRL);
  ps2_writel(ps2_readl(PS2M_INT_MASK) & ~(1<<INT_PS2M_FLAG), PS2M_INT_EN);
  print(usart , "\nmouse-port: not a mouse connected...\n");
  return -1;

}
/**
 * Registers a mouse handling function
 */
void register_mouse_listener(void (*fp)(struct mouseevent*)){
  mouse_listener=fp;
  mouse_reading =1;
}
/**
 * Removes the mouse handler
 */
void unregister_mouse_listener(){
  mouse_listener=NULL;
  mouse_reading = 0;
}

