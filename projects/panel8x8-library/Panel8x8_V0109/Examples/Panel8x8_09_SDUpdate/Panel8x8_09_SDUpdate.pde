// Panel8x8
// Example 09: SD Update
//
// This example demonstrates using an SD card serve animations.
// Unzip animations.zip to sd card, attach reader as shown.
//
// NOTES:
// Hardware connection
// Arduino     SD Card
// MISO  12    7 DATA_OUT
// MOSI  11    2 DATA_IN
// SCK   13    5 SLCLK
// CCS   10    1 CS
// VCC         4 VDD
// GND         3 VSS1
// GND         6 VSS2
// GND         G GND
//
// POWER: Power requirements are fairly serious now exceeding serial port availability.
// Suggest using a 5v regulated power supply, make sure it's regulated at 5v.
// Power directly to panels, jumper to arduino 5v/gnd, arduino to sd card reader.
//
// Kits & Schematics available at ModernDevice.Com
//
// By Dataman aka Charley Jones, 8x8Panel@CRJones.Com
// 2009-10-12 Initial Version
//
// Please remember to set PANELS to the physical number of
// panels to support in Panel8x8.h -- defaults to 4.
// In most cases for 1-4 panels this won't make any difference.

// Required include for the Panel8x8 library
#include <Panel8x8Serial.h>

// Declare an object of type Panel8x8
Panel8x8Serial Panel;

// Declare the text to be displayed, in this case, in FLASH 
char buffer[65];

// Headers to allow SD card operations
#include <SdReader.h>
#include <FatReader.h>

SdReader card;
FatVolume vol;
FatReader root;
FatReader f;
dir_t dir;
byte SDLoop =0;

void SetupSDCard() {
  if (!card.init()) {
    Serial << "Card init. failed!"; 
    return;
  }
  if (!vol.init(card)) {
    Serial << "No partition!"; 
    return;
  }
  if (!root.openRoot(vol)) {
    Serial << "Couldn't open dir" << endl; 
    return;
  }
  root.rewind();
  if (!GetNextSDFile()) {
    Serial << "No 8x8 File in root." << endl;
    return; 
  }
  if (!f.open(vol, dir)) {
    Serial << "File open error." << endl;
    return;
  }
  // file open and ready
  if (!LoadSDFile()) {
    Serial.println("Info: Animation file header corrupted?");
    return;
  }
  Panel.PanelMode=3; // Live Animation
  Panel.SetScrolling(0);
}

int LoadSDFile() {
  f.read((uint8_t *)buffer, 8);
  Panel.idxScroll=0;     //Reset buffer pointer
  Panel.bIsScrolling=false;
  uint16_t *ptr16 = (uint16_t *)buffer;
  if (*(ptr16++)!=PANEL8X8SERIALFILE) {Serial.println("Animation.8x8 file is wrong format"); return 0;}  //Send back a 0 as error, wrong version.
  Panel.iPanels = *(ptr16++);
  Panel.iFrames = *(ptr16++);
  Panel.frameDelay = *(ptr16);
  //Serial <<  "Panels: " << _DEC(Panel.iPanels) << endl << "Frames: " << Panel.iFrames << endl << "Delay: " << Panel.frameDelay << endl;
  SDLoop = 0;
  return 1;
}

int GetNextSDFile()
{
  byte idx = 0;
  boolean is8x8 = false;
  while (root.readDir(dir) > 0) {
    //Serial << (char *)dir.name << endl;
    return 1;
  }
  return 0;  
}

// LoadAnimation
// Called to load up an animation frame
void LoadAnimation() {
  
    int l = 0;
    uint8_t *ptr8;
    // All this just to make sure we get the next 8 byte buffer.
    while (l<=0) {
     //Serial << "Read SD" << endl;
     ptr8 = (uint8_t *)buffer;
     l = f.read(ptr8,8*Panel.iPanels);
     if (l<8*Panel.iPanels) {
      l=0;
      SDLoop++;
      if (SDLoop < 2) {
        //Serial << "EOF" << endl;
        f.seekSet(8);
      }
      else {
       //Serial << "Next File" << endl;
       f.close();
       if (!GetNextSDFile()) {
        root.rewind();
        GetNextSDFile(); 
        }
        if (!f.open(vol, dir)) {return;}
        LoadSDFile();
        return;
       }
      }
     }
 
     //DumpPanels(); 
     Panel.ClearOutput();
     memcpy((byte *)Panel.iScroll,(byte *)buffer,Panel.iPanels*8);
     
  // Output the panels
  Panel.Scroll();
  
  if (++Panel.idxScroll>=Panel.iFrames) {
   Panel.idxScroll=0;
  }
  
}

//
// Called once by ardruino to setup the sketch
void setup() {
 // Call Panel.Begin to initialize panel buffers
 // Syntax: buffer address, max buffer size, current text size, 0=Ram/1=Flash
 Panel.Begin(buffer,65,0,0);
 // For debugging
 Serial.begin(9600);
 // Setup the SD Card
 SetupSDCard();
};

// Called repeatedly by arduino
void loop() {
  // Call Panel.Loop to pump the panels
  LoadAnimation();
};

 
