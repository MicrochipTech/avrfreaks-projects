/*****************  THERMOSTAT  *****************
 *
 * Temperature regulation based on:
 * - Arduino nano
 * - DALLAS DS18B20 temperature sensor
 * - matrix 16x2 LCD 
 * - relay module (250V/10A)
 * - KY-040 rotary encoder (menu and temperature regulation)
 *
 * FEATURES:
 * - Easy rotary encoder based menu
 * - Set regulation temperature 5-55 °C
 * - Save regulation temperature in EEPROM
 *
 * dejan.rakijasic@gmail.com
 */
#include <LiquidCrystal.h>
#include <DS18B20.h>        //https://github.com/nettigo/DS18B20
#include <Button.h>        //https://github.com/JChristensen/Button
#include <OneWire.h>
#include <EEPROM.h>

LiquidCrystal lcd(9, 8, 7, 6, 5, 4);

#define encoder_clk 2 //Arduino UNO (nano) pin 10
#define encoder_dt 3 //Arduino UNO (nano) pin 11
#define relay1 0 //Arduino UNO (nano) pin A0
#define temperatureSetupMode 12 //Rotary encoder SWITCH (SW) - button (menu entry)

volatile byte A = 0; 
volatile byte B = 0; 
volatile float encoder = 0; 
volatile byte encoder_read = 0;
volatile uint8_t PORTB_default = 0xFF;    

#define DEBOUNCE_MS 20      //debounce  20 ms 
#define REPEAT_FIRST 1000   //ms required before repeating on long press
#define REPEAT_INCR 200     //repeat interval for long press
#define PULLUP true         //To keep things simple, we use the Arduino's internal pullup resistor.
#define INVERT true
#define LONG_PRESS 2000    //long press to menu entry (2 seconds)

Button btn_setup(temperatureSetupMode, PULLUP, INVERT, DEBOUNCE_MS);

const byte ONEWIRE_PIN = 3;
byte sensorAddress[8] = {0x28, 0x9A, 0x47, 0xE1, 0x06, 0x00, 0x00, 0xCF};
OneWire onewire(ONEWIRE_PIN);

DS18B20 sensors(&onewire);

float Temp_regulation;
float Temp;
int relay_state = 0;

enum STATES
{
  MEASURE_REGULATION, //measuring and temperature regulation 
  SETUP,     // setup
};
STATES state;

byte thermo_icon[8] = {
  0b00100,
  0b01010,
  0b01010,
  0b01110,
  0b01110,
  0b11111,
  0b11111,
  0b01110
};
byte degree_icon[8] = {
  0b00111,
  0b00101,
  0b00111,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000
};

void setup()
{
  lcd.begin(16, 2);
  lcd.createChar(1, thermo_icon);
  lcd.createChar(2, degree_icon);

  DDRC |= (1 << relay1); //relay1 PC0 (Arduino UNO/nano pin - A0)
  PORTC |= (1 << relay1); //relay1 OFF
  DDRB &= ~(1 << encoder_clk);   //encoder_clk INPUT
  DDRB &= ~(1 << encoder_dt);   //encoder_dt INPUT
  PORTB |= (1 << encoder_clk); //encoder_clk PULLUP
  PORTB |= (1 << encoder_dt); //encoder_dt PULLUP

  lcd.clear();

  // DS18B20 sensors setup
  // Argument: quality - measurement resolution in bits (from 9 to 12)
  sensors.begin(12);
  // The first requests sensor for measurement
  sensors.request(sensorAddress);

   state = MEASURE_REGULATION;
   read_EEPROM(Temp_regulation); 
   encoder = Temp_regulation*10; 
   
   //PIN CHANGE INTERRUPT on encoder_clk and encoder_dt pins 
   PCICR |=(1<<PCIE0);
   PCMSK0 |= (1<<PCINT2) | (1<<PCINT3);
   sei(); 
}

/*MAIN LOOP*/
void loop()
{
  state_change();
  input_check();
  temp_measuring();
}

//PIN change (encoder pins) interrupt routine
ISR(PCINT0_vect)
{
    uint8_t PORTB_change;
    PORTB_change = PINB ^ PORTB_default;
    PORTB_default = PINB;

      if(PORTB_change & (1 << PINB2))//change on PCINT2
    {
         encoder_read = PINB & 0xC;
         if(encoder_read == B00001100 && A) { 
         encoder--; 
         B = 0; 
         A = 0; 
     }
  else if (encoder_read == B00000100) B = 1;
    }

      if(PORTB_change & (1 << PINB3))//change on PCINT3
    {
        encoder_read = PINB & 0xC; 
  if (encoder_read == B00001100 && B) { 
    encoder++; 
    B = 0; 
    A = 0; 
  }
  else if (encoder_read == B00001000) A = 1;
    }
}

/*Change STATES*/
void state_change()
{
  switch (state)
  {
    case MEASURE_REGULATION:
      lcd.setCursor(0, 0);
      lcd.write(1); //thermometer icon

      if (Temp<10){
      lcd.setCursor(2, 0);
      lcd.print(" ");
      lcd.setCursor(3, 0);
      lcd.print(Temp, 1);
      }else{
      lcd.setCursor(2, 0);
      lcd.print(Temp, 1);
      }
      lcd.setCursor(6, 0);
      lcd.write(2); //degree icon
      lcd.print("C");
      lcd.setCursor(0, 1);
      lcd.print("T=");
      if (Temp_regulation<10){
      lcd.setCursor(2, 1);
      lcd.print(" ");
      lcd.setCursor(3, 1);
      lcd.print(Temp_regulation, 1);
      }else{
      lcd.setCursor(2, 1);
      lcd.print(Temp_regulation, 1);
      }
      lcd.setCursor(6, 1);
      lcd.write(2); 
      lcd.print("C");
      lcd.setCursor(10, 0);
      lcd.print("RELAY");
      if (relay_state) {
        lcd.setCursor(13, 1);
        lcd.print(" ");
        lcd.setCursor(11, 1);
        lcd.print("ON");
      }
      else
      {
        lcd.setCursor(11, 1);
        lcd.print("OFF");
      }
      break;
    case SETUP:
      lcd.setCursor(0, 0);
      lcd.print("Set Temperature:");
      lcd.setCursor(0, 1);
      lcd.print("T=");
      if (Temp_regulation<10){
      lcd.setCursor(2, 1);
      lcd.print(" ");
      lcd.setCursor(3, 1);
      lcd.print(Temp_regulation, 1);
      }else{
      lcd.setCursor(2, 1);
      lcd.print(Temp_regulation, 1);
      }
      lcd.setCursor(6, 1);
      lcd.write(2); 
      lcd.print("C");
      break;
  }
}

/*Check input - rotary encoder button (SW)*/
void input_check()
{
  btn_setup.read(); //read rotary encoder switch (SW pin)

  switch (state)
  {
    case MEASURE_REGULATION:
     regulation();
      if (btn_setup.pressedFor(LONG_PRESS)) //long press encoder switch to enter setup
      {
        lcd.clear();
        state = SETUP;
      }
      break;
    case SETUP:
      PORTC |= (1 << relay1); //relay1 OFF
       Temp_regulation =  encoder / 10;
      if (Temp_regulation > 55.0)
      {
        Temp_regulation = 5.0;
      }
      if (Temp_regulation < 5.0)
      {
        Temp_regulation = 55.0;
      }
     
      if (btn_setup.wasPressed())
      {

        lcd.clear();
        write_EEPROM(0, Temp_regulation);
        state = MEASURE_REGULATION;
      }
      break;
  }
}


/*Get temperature form sensor*/
void temp_measuring()
{
  if (sensors.available())
  {
    Temp = sensors.readTemperature(sensorAddress);
    sensors.request(sensorAddress);
  }
}

/*save temperature setup in EEPROM*/
void write_EEPROM(int address, float Temp_regulation) 
{
  byte* p = (byte*)(void*)&Temp_regulation;
  for (int i = 0; i < sizeof(Temp_regulation); i++)
    EEPROM.write(address++, *p++);

lcd.setCursor(1,0);
lcd.print("Saved!!!");
  delay(1000);  
  lcd.clear();
}

/*read from EEPROM*/
float read_EEPROM(int address) 
{
  byte* p = (byte*)(void*)& Temp_regulation;
  for (int i = 0; i < sizeof(Temp_regulation); i++)
    *p++ = EEPROM.read(address++);
  return Temp_regulation;
 if (isnan(Temp_regulation))
 {
  Temp_regulation=22.2;
 }
}

/*TEMPERATURE REGULATION*/
void regulation()
{
  if (Temp <= Temp_regulation)
  {
    PORTC &= ~(1 << relay1); //relay1 ON
    relay_state = 1;
  }
  else if (Temp >= Temp_regulation)
  {
    PORTC |= (1 << relay1); //relay1 OFF
    relay_state = 0;
  }
}
