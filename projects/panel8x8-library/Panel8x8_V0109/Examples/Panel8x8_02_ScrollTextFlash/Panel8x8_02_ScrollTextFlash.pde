// Panel8x8
// Example 02: Simple FLASH Text Scroller
//
// This example demonstrates a simple text scroller using FLASH.
// Using FLASH allows much longer messages, but the message cannot be changed by the program.
//
// This is actually a nod to Paul Badger as well,
// This is the original demo text from his 8x8 sample code.
//
// Kits & Schematics available at ModernDevice.Com
//
// By Dataman aka Charley Jones, 8x8Panel@CRJones.Com
// 2009-09-22 Removed PANELS include
// 2009-09-17 Initial Version

// Please remember to set PANELS to the physical number of
// panels to support in Panel8x8.h -- defaults to 4.
// In most cases for 1-4 panels this won't make any difference.

// Required include for the Panel8x8 library
#include <Panel8x8.h>

// Declare an object of type Panel8x8
Panel8x8 Panel;

// Declare the text to be displayed, in this case, in FLASH 
prog_uchar signMessage[]  PROGMEM  = {
  "     1. A ROBOT MAY NOT INJURE A HUMAN BEING OR, THROUGH INACTION, ALLOW A HUMAN BEING TO COME TO HARM.     "
  "     2. A ROBOT MUST OBEY ORDERS GIVEN TO IT BY HUMAN BEINGS EXCEPT WHERE SUCH ORDERS WOULD CONFLICT WITH THE FIRST LAW.     "
  "     3. A ROBOT MUST PROTECT ITS OWN EXISTENCE AS LONG AS SUCH PROTECTION DOES NOT CONFLICT WITH THE FIRST OR SECOND LAW.    " 
}; 

// Called once by ardruino to setup the sketch
void setup() {
 // Call Panel.Begin to initialize panel buffers
 // Syntax: buffer address, max buffer size, current text size, 0=Ram/1=Flash
 Panel.Begin((char *)signMessage,strlen((char *)signMessage),strlen((char *)signMessage),1);
};

// Called repeatedly by arduino
void loop() {
  Panel.Loop();
};
