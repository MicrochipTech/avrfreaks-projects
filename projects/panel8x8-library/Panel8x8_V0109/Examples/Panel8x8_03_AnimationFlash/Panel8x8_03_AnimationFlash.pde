// Panel8x8_03_AnimationFlash
// Example 03: Displaying an animation from flash memory
//
// This example demonstrates a simple animation display from flash memory.
// Using flash allows storage of longer animations than can be stored in ram.
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

// Set up our animation in flash memory
// And before you go insance trying to calculate bit patterns
// 1 frame per line, 1 panel per frame, 8 lines per panel, top to bottom.
// I've also written a program that generates this data and even uploads it to the panels.
// It will be available shortly.
prog_uint8_t buffer[] PROGMEM = {
  129, 66, 36, 24, 24, 36, 66,129,
    2,132, 72, 56, 28, 18, 33, 64,
    4,  8,144, 92, 58,  9, 16, 32,
    8, 16, 16,158,121,  8,  8, 16,
   16, 16, 16, 31,248,  8,  8,  8,
   32, 16, 17, 30,120,136,  8,  4,
   64, 33, 18, 28, 56, 72,132,  2,
   64, 33, 18, 28, 56, 72,132,  2
};

// Called once by ardruino to setup the sketch
void setup() {
 // Call Panel.Begin to initialize panel buffers
 // Syntax: buffer address, max buffer size, current text size, 0=Ram/1=Flash
 Panel.Begin((char *)buffer,64,64,1);  // Initialize with our buffer 
 Panel.SetScrolling(0);                // Turn off scrolling
 Panel.PanelMode=2;                    // Animation mode
 Panel.iFrames=8;                      // Have to tell it how many frames.
 Panel.iPanels=1;                      // Also have to tell it how many panels wide is the animation.
                                       // As we may be trying to display a 1 panel animation on 2 panels.
};

// Called repeatedly by arduino
void loop() {
  Panel.Loop(); // Pump the display
};
