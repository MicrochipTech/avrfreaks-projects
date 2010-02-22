// Panel8x8
// Example 08: Update over Wifi
//
// This example demonstrates updating text over wireless.
// This should blow the doors off flexibility!
//
// Notes: 
// Must be running WiShield library in WiServer mode,
// Which means edit apps-conf.h
// Comment out //#define APP_WEBSERVER
// Uncomment     #define APP_WISERVER
// Input is limited to 80 bytes due to library limitations
//
// The only way I know to hook up the hardware right now is with a duemilanove (2k9 for short)
// Plug AsyncLabs Wifi Shield version 1.0 into 2k9. 
// Pins 4,5,6,7 connect to panel 4,5,6,7 
// Power, Ground connect to panel Power, Ground
// To simplify testing, wifi sets itself up as an unprotected adhoc server at 169.254.1.2, 255.255.0.0
// Just connect to ASYNCLABS, you should automatically apipa as no dhcp, or hardcode yourself to 169.254.1.3, 255.255.0.0
// Then browse to 169.254.1.2
// Short messages for now only please, will hang if you get up near 100 chars.
// Just reset shield to continue.
// NEXT: Easy enough to secure by setting a password,
// setting to infrastrustructure mode.
//
// Kits & Schematics available at ModernDevice.Com
//
// By Dataman aka Charley Jones, 8x8Panel@CRJones.Com
// 2009-10-07 Initial Version

// Please remember to set PANELS to the physical number of
// panels to support in Panel8x8.h -- defaults to 4.
// In most cases for 1-4 panels this won't make any difference.

// Required include for the Panel8x8 library
#include <Panel8x8.h>

// Declare an object of type Panel8x8
Panel8x8 Panel;

// Declare the text to be displayed, in this case, in FLASH 
char buffer[81]={"This is a test, this is only a test."};

#include <WiServer.h>

#define WIRELESS_MODE_INFRA	1
#define WIRELESS_MODE_ADHOC	2

// Wireless configuration parameters ----------------------------------------
unsigned char local_ip[] = {169,254,1,2};	// IP address of WiShield
unsigned char gateway_ip[] = {0,0,0,0};	// router or gateway IP address
unsigned char subnet_mask[] = {255,255,0,0};	// subnet mask for the local network
const prog_char ssid[] PROGMEM = {"ASYNCLABS"};		// max 32 bytes
const prog_char webpage[] PROGMEM = {"<html><head><body><center><h1>Panel 8x8 WifiServer</h1><form method='get' action='0'>Scroll Text<br /><input type='text' name='1' size='80' maxlength='80' /><br /><input type='submit' name='3' value='submit'></form></center><body><html>"};

char message[81]={"This is a test. "};

unsigned char security_type = 0;	// 0 - open; 1 - WEP; 2 - WPA; 3 - WPA2

// WPA/WPA2 passphrase
const prog_char security_passphrase[] PROGMEM = {"12345678"};	// max 64 characters

// WEP 128-bit keys
// sample HEX keys
prog_uchar wep_keys[] PROGMEM = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d,	// Key 0
				  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// Key 1
				  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// Key 2
				  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00	// Key 3
				};

// setup the wireless mode
// infrastructure - connect to AP
// adhoc - connect to another WiFi device
unsigned char wireless_mode = WIRELESS_MODE_ADHOC;

unsigned char ssid_len;
unsigned char security_passphrase_len;
// End of wireless configuration parameters ----------------------------------------


// This is our page serving function that generates web pages
boolean sendMyPage(char* URL) {
  
    // Check if the requested URL matches "/"
    if (strcmp(URL, "/") == 0) {
        // Use WiServer's print and println functions to write out the page content
    }
    else {
     int i = 5;
     int midx = 0;
     char c=0;
     byte urldata=0;
     byte urlflag=0;
     Panel.ClearOutput();
     while (URL[i]!='&' && URL[i]!=0) {
      c = URL[i++];
      if (c=='%') {
       urlflag=1;
       urldata=0;
       c=0;
      }
      else if (urlflag==1) {
       urldata = (c - '0') * 16;
       urlflag +=1;
       c=0;
      }
      else if (urlflag==2) {
       c -= '0';
       c += urldata;
       urlflag=0;
      } 
      else if (c=='+') {
        c=32;
      }
      if (c>=32) {buffer[midx++]=c;}      
     }
    buffer[midx]=0;
    //Serial << message << endl;
    Panel.iBufferLen=midx;
    Panel.NewMessage();
    }   

    WiServer.print_P(webpage);
    return true;
}


void setup() {
  // Initialize WiServer and have it use the sendMyPage function to serve pages
  WiServer.init(sendMyPage);
  
  // Enable Serial output and ask WiServer to generate log messages (optional)
  //Serial.begin(9600);
  //WiServer.enableVerboseMode(true);

  Panel.Begin(buffer,81,strlen(buffer),0);
}

void loop(){

  // Run WiServer
  WiServer.server_task();
 
  //delay(10);
  Panel.Loop();
}

