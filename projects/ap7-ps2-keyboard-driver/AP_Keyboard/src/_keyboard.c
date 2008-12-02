/**
 * PS2 Keyboard driver for the AVR32-Mistral debug port
 * To use this driver you must first call init_kbd();
 * Then you must register keyboard-handler function.
 * The scancodes for make/break are the same because break code (0xF0) 
 * is filtered out. The release boolean is set to signify a break.
 *
 * This driver does not support the special keys PAUSE and PRINT SCREEN
 * 
 * The setled function may not work.
 *
 * TODO:
 * Further work may include - changing of scan code set (unnecessary)
 *                          - Setting of typematic rate/delay
 *                          - Support PRINT SCREEN key
 *                          - Support PAUSE key
 *                          - Disabling of keyboard after use
 *
 * Author: Sverre Sundsdal (summerstudent 2004)
 */

#include "_keyboard.h"
#include "sys/interrupts.h"
#include "sys/cpu.h"

#include "usart.h"
extern	struct usart_options_t opt;
extern volatile struct avr32_usart_t *usart = &AVR32_USART1;

volatile struct avr32_psif_t  *psif  = &AVR32_PSIF  ;

volatile static int kbd_sending = 0; //is the ps2-port in sending mode?
volatile static int kbd_acking = 0; //are we awaiting an ack?
/*
 * The registered keyboard listener function.
 */
static void (*kbd_listener)(struct keyevent*)= 0;
/**
 * The keyboard interrupt handler
 * Generates keyevent and send to keyboard listener
 * Handles CTRL, ALT and SHIFT (as an extra convenience, but this can
 *  also be handled in the user-program)
 * 
 */
static void ps2_kbd_interrupt(int int_grp){
  static int kbd_scancodes = 0; //stores character
  static int kbd_release = 0;//is the next code a kbd_release key code
  static char kbd_shift; // has KBD_SHIFT been presed down.
  static char kbd_alt; 
  static char kbd_ctrl;
  volatile static int keyboard_data; //send/recv buffer

  //print(usart, "\nINT");

 	if(psif->sr0&&0x00000010){
 		keyboard_data = psif->rhr0;
 	}
 	else{
 		keyboard_data = 0;
 	}
    //print(usart, "\nINT: ");
    //print_long(usart, keyboard_data);
    udelay(1000);

  
  if(kbd_sending){
    //A byte has been sent. We are now ready to recieve
    //    printf("kbd:data sent\n");
    kbd_sending = 0;
  } else if(kbd_acking){
    //Waiting ack and probably recieved now.
    //This is a 0xFA, but no checking done.
    kbd_acking = 0;
  }else if(keyboard_data==0xF0){
    //The key has been released
    kbd_release = TRUE;
  } else if(keyboard_data== 0xE0){
    kbd_scancodes = (kbd_scancodes<<8)|keyboard_data;
    //this is an exteded keycode
  } else {
    kbd_scancodes = (kbd_scancodes<<8)|keyboard_data;

    if(keyboard_data==0x12 || keyboard_data==0x59){
      //shift has been pressed this is nice to keep track of.
      kbd_shift = !kbd_release;
    } else if(keyboard_data == 0x14){
      //Ctrl
      kbd_ctrl = !kbd_release;
    } else if(keyboard_data == 0x11){
      //alt
      kbd_alt = !kbd_release;
    }
    
    if(kbd_listener){
      //call scancode listener if registered
      struct keyevent event; //event to send to user.      
      event.key = kbd_scancodes;
      event.release = kbd_release;
      event.shift_down = kbd_shift;
      event.ctrl_down = kbd_ctrl;
      event.alt_down = kbd_alt;
      (*kbd_listener)(&event);
    }
    
    //clear variables for next interrupt
    kbd_scancodes = 0;
    kbd_release = FALSE;
  }
  
  //interrupt handeled.
  ps2_writel(ps2_readl(PS2K_INT_MASK) | (1<<INT_PS2K_FLAG), PS2K_INT_EN);
  
}

/**
 * Set the led lights on the keyboard
 */
void ps2_kbd_setled(int caps, int scroll, int num){
  char leds;
  //normalize int values.
  if(caps) 		caps = 1;
  if(scroll) 	scroll = 1;
  if(num) 		num = 1;
  leds = caps<<2 | num<<1 | scroll<<0; //format the leds byte

  
  
  //kbd_sending =1; //wait for a sending complete interrupt
  //ps2_writeb(0xED, PS2K_DATA_TX); //Send a "Set/reset LEDs"
  psif->thr0 = 0xED;
  
  kbd_acking = 1; //then wait for an ack
  while(kbd_acking){
  }
  //print(usart, "\nacking");

  //ack recieved
  //ps2_writeb(leds, PS2K_DATA_TX); //send led data
  psif->thr0 = leds;
  //kbd_sending = 1; //wait for sending complete interrupt
  kbd_acking = 1; //then wait for an ack
  while(kbd_acking){
    
  }
  //led set.
  
}

/**
 *Initialize debug ps2-port and set the interrupt handler
 */
void init_kbd(){
  print(usart,"\nKeyboard initializing...\n");
  
  //Prescaler
  psif->psr0 = 0x00FF;
  //ps2_writel(PS2_EN, PS2K_CTRL); //enable ps2K
  //ps2_writel(ps2_readl(PS2K_INT_MASK) | (1<<INT_PS2K_FLAG), PS2K_INT_EN); //enable interrupt
  psif->cr0 = PS2_EN; 
  psif->ier0 = psif->imr0 | (1<<INT_PS2K_FLAG);

  //register interrupt function using newlib
  set_interrupts_base( (void *) AVR32_INTC_ADDRESS );
  init_interrupts();
  register_interrupt((__int_handler)ps2_kbd_interrupt,AVR32_PSIF_IRQ/32,AVR32_PSIF_IRQ%32, INT1);
  
  print(usart, "Keyboard initialized\n");
}

/**
 * Registers a keyboard handling function
 * 
 * TODO: Maybe we should have init-kbd here ?
 */
void register_kbd_listener(void (*fp)(struct keyevent*)){
  kbd_listener=fp;
}


/**
 * Unregisters the keyboard handling function
 */
void unregister_kbd_listener(){
  kbd_listener=0;
}

