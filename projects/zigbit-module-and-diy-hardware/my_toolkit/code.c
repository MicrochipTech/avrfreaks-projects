//********* my_code.h ************//
#include "my_header.h"
#include "pal.h"

void my_init()
{
  asm("NOP");
  void pal_button_init(void);
}

void my_function()
{
  asm("NOP");
}

int poll_button(void)
{
  //button_state_t pal_button_read(button_id_t button_no);
 button_state_t bs;
 int shuntState;
 
 bs = pal_button_read(BUTTON_0); //read the button
 if(bs == BUTTON_PRESSED) 
 {
    shuntState = SHUNT_IN;
  }
 else
 {
   shuntState = SHUNT_OUT;
 }
 return shuntState;
}

