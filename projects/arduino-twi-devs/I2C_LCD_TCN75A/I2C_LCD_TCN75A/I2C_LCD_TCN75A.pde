///Arduino Sample Code
///www.DFRobot.com
///Last modified on 17th September 2010
//1. +V, +5v (or 9-12v depending on the model)
//2. SCL clock – connect to pin A5 on the Uno
//3. SDA data – connect to pin A4 on the Uno
//4. Gnd
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
 
LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display
 
int mittaus = 0;

void setup()
{
  lcd.init();                      // initialize the lcd
  Wire.begin();
  Serial.begin(115200);
  // Print a message to the LCD.
  lcd.backlight();
  //lcd.setCursor(0, 1);
  //lcd.print("Lampotila:");  
}
 
void loop()
{
  // when characters arrive over the serial port...
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
    Wire.beginTransmission(0x48); /* anturin osoite 0x48 : 1001000 */
                            /* koska anturin A2, A1 ja A0 liittimet ovat irti */
  Wire.requestFrom(0x48, 2);

if(2 <= Wire.available())
  {
    mittaus = Wire.receive();
    Serial.print("Lampotila : ");
    Serial.println(mittaus); 
    lcd.print("Lampotila: ");  
    lcd.print(mittaus);
    lcd.print(" C");
    mittaus = 0; /* mittaustulos nollaan */
    
  }
  Wire.endTransmission(); 
  delay(3000);
  lcd.clear();
}
