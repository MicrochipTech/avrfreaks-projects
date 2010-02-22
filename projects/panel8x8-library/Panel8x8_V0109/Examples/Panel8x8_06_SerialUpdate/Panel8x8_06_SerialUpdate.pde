// Panel8x8Serial
// Example 06: Serial Update
//
// This example demonstrates updating the panels over a serial interface.
// This greatly extends the usefullness of the panels, as text can be updated
// on the fly without needing to recompile code.
// The Panel8x8Utility programs work with this example and require this program for operation.
// These programs are DotNet based and currently work under Windows.
// The Panel8x8Utilities demonstrate serial update of text, animations, and live animations.
//
// Kits & Schematics available at ModernDevice.Com
//
// By Dataman aka Charley Jones, 8x8Panel@CRJones.Com
// 2009-09-30 Initial Version
//
// Please remember to set PANELS to the physical number of
// panels to support in Panel8x8.h -- defaults to 4.
// In most cases for 1-4 panels this won't make any difference.

// Required include for the Panel8x8 Serial library
#include <Panel8x8Serial.h>

// Declare an object of type Panel8x8
Panel8x8Serial Panel;

// Declare the text to be displayed, in this case, in FLASH 
char buffer[512]={".       "};

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


