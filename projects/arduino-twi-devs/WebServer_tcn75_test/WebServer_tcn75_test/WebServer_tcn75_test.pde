/*
  Web  Server
 
 A simple web server that shows the value of the analog input pins.
 using an Arduino Wiznet Ethernet shield. 
 
 Circuit:
 * Ethernet shield attached to pins 10, 11, 12, 13
 * Analog inputs attached to pins A0 through A5 (optional)
 
 created 18 Dec 2009
 by David A. Mellis
 modified 4 Sep 2010
 by Tom Igoe
 modified 2 Apr 2011
 by Lauri Tarkiainen
 
 */
#include <Wire.h>
#include <SPI.h>
#include <Ethernet.h>
#include <LiquidCrystal_I2C.h>

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
byte ip[] = { 192,168,1, 177 };
int mittaus = 0;            //oma
LiquidCrystal_I2C lcd(0x27,16,2);
// Initialize the Ethernet server library
// with the IP address and port you want to use 
// (port 80 is default for HTTP):
Server server(80);

void setup()
{
  lcd.init();
   Serial.begin(115200);
  lcd.backlight();
  // start the Ethernet connection and the server:
  Ethernet.begin(mac, ip);
  server.begin();
  Wire.begin();
}

void loop()
{
   if (Serial.available()) {
    // wait a bit for the entire message to arrive
    delay(100);
    // clear the screen
    lcd.clear();
    // read all the available characters
    while (Serial.available() > 0) {
      // display each character to the LCD
    lcd.write(Serial.read());
    }
  }
  Wire.requestFrom(0x48, 2);
  Wire.beginTransmission(0x48); /* anturin osoite 0x48 : 1001000  koska anturin A2, A1 ja A0 liittimet ovat irti */

  if(2 <= Wire.available())
  {
    mittaus = Wire.receive();
    Serial.print("Lampotila : ");
    Serial.println(mittaus); 
    lcd.print("Lampotila: ");  
    lcd.print(mittaus);
    lcd.print(" C");
   // mittaus = 0; /* mittaustulos nollaan */ 
  }
  
  Wire.endTransmission(); 
  delay(3000);
  lcd.clear();

  // listen for incoming clients
  Client client = server.available();
  if (client) {
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println();
          client.println("Lampotila:");
          client.print(mittaus);            //oma
          client.println("<br />");

          break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        } 
        else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client.stop();
  }
}
