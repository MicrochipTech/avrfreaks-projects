// Panel8x8
// Example 07: Update over ethernet
//
// This example demonstrates updating text over ethernet.
//
// REQUIRES: 
// Ethernet Board  : http://arduino.cc/en/Main/ArduinoEthernetShield
//                   For compatability with Webduino, you need a Wiznet variety enthernet board
//                   like the Ethernet shield (configured WizNet) from AdaFruit:
//                   http://www.adafruit.com/index.php?main_page=product_info&cPath=17_21&products_id=83
//                   Ethernet uses digital pins 10, 11, 12, and 13
//
// Webduino Library: http://code.google.com/p/webduino
//                   This is provided in the support directory
//
// Kits & Schematics available at ModernDevice.Com
//
// By Dataman aka Charley Jones, 8x8Panel@CRJones.Com
// 2009-10-06 Initial Version

// Please remember to set PANELS to the physical number of
// panels to support in Panel8x8.h -- defaults to 4.
// In most cases for 1-4 panels this won't make any difference.

// Required include for the Panel8x8 library
#include <Panel8x8.h>

// Declare an object of type Panel8x8
Panel8x8 Panel;

// Declare the text to be displayed, in this case, in FLASH 
char buffer[512]={"This is a test, this is only a test.   "};

// Ethernet Specific 
#include <Ethernet.h>
#include <WebServer.h>

// CHANGE THIS TO YOUR OWN UNIQUE VALUE
static uint8_t mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

// CHANGE THIS TO MATCH YOUR HOST NETWORK
static uint8_t ip[] = { 192, 168, 1, 64 };

// OTHER VARIABLES
#define PREFIX "/"
WebServer webserver(PREFIX, 80);

 char * rtrim(char *ptr) {
 int i = strlen(ptr);
 while(strlen(ptr)>0 && (ptr[i-1])<33) {
  ptr[i-1]=0;
  i--;
 }
 return ptr;
}

// commands are functions that get called by the webserver framework
// they can read any posted data from client, and they output to server
void defaultCmd(WebServer &server, WebServer::ConnectionType type, char *url_tail, bool tail_complete)
{
    boolean bNewMessage = false;
   
    P(htmlHead) =
    "<html>"
    "<head>"
    "<title>Panel8x8 Web Server</title>"
    "</head>"
    "<body>"
    "Modern Devices Panel8x8 on the Web<BR>"
    "Web interface by Charley Jones, Panel8x8@crjones.com<BR>"
    "<BR>" 
    "<form action='\' method='post'>"
    "Frame Delay: <input type='text' name='FrameDelay' size='3' value='";
  
    P(htmlFormMiddle) = 
    "'><BR>"
    "<TextArea rows='6' cols='40' name='TextBox'>" ;
  
    P(htmlTail) =
    "</TextArea><BR>"
    "<input type='submit' value='Submit'/></form>"
    "</body></html>";
    
  if (type == WebServer::POST)
  {
    
    char sFrameDelay[16];
    server.readPOSTparam("FrameDelay",16,sFrameDelay,16);
    Panel.frameDelay = atoi(sFrameDelay);
    server.readPOSTparam("TextBox", 16, buffer,512);
    rtrim(buffer);
    Panel.iBufferLen = strlen(buffer) + 3;
    if (Panel.iBufferLen>509) {Panel.iBufferLen=509; buffer[509]=0;} 
    bNewMessage = true;
  }

  int i;
  server.httpSuccess();
  server.printP(htmlHead);
  server.print(Panel.frameDelay);
  server.printP(htmlFormMiddle);
  server.print(rtrim(buffer));
  server.printP(htmlTail);   

  // Add back 3 spaces to the end.    
  int t=strlen(buffer);
  int u=t;
  for (u=t;u<t+3;u++) {buffer[u]=32;}
  buffer[u]=0;
  Panel.iBufferLen = u;
  
  if (bNewMessage) {Panel.ClearOutput(); Panel.NewMessage();}

  delay(250);
}

// Called once by ardruino to setup the sketch
void setup() {
 // Call Panel.Begin to initialize panel buffers
 // Syntax: buffer address, max buffer size, current text size, 0=Ram/1=Flash
 Panel.Begin(buffer,512,strlen(buffer),0);
 // Call Eternet Setup
 Ethernet.begin(mac, ip);
 webserver.begin();
 webserver.setDefaultCommand(&defaultCmd);
};

// Called repeatedly by arduino
void loop() {
  // Pump the webserver
  webserver.processConnection();
  // Call Panel.Loop to pump the panels
  Panel.Loop();
};
