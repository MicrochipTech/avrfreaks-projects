// Panel8x8
// Example 01: Simple Text Scroller
//
// This example demonstrates a simple text scroller using RAM.
// Using ram would allow the program to make updates to message displayed.
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
char buffer[512]={"This is a test, this is only a test."};

// Called once by ardruino to setup the sketch
void setup() {
 // Call Panel.Begin to initialize panel buffers
 // Syntax: buffer address, max buffer size, current text size, 0=Ram/1=Flash
 Panel.Begin(buffer,512,strlen(buffer),0);
};

// Called repeatedly by arduino
void loop() {
  // Call Panel.Loop to pump the panels
  Panel.Loop();
};
