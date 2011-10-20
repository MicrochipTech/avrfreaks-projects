#include "PCD8544.h"

float R1 = 5060.0;    // !! resistance of R1 !! battery
float R2 = 400.0;     // !! resistance of R2 !! ground
static const byte oleo = 0;
int mapsen = 1; // Set MAP sensor input on Analog port 2
float boostcal = 0; //int for boostcal
int x = 0; //Dummy Var for boostcal calibration loop
int boostkPa = 0; // Set boost value to 0
int boostPSI = 0; // Set boost value to 0
float mapval = 0; // Set raw map value to 0
int peakboost = 0; //Peak Boost var
//
static const byte bateria = 2;
static const byte discos = 3;
static const byte diesel = 4;
static const byte clock = 13;
static const byte light = 2;
static const byte agua = 5;
static const int fagua = 8;
static const byte LCD_WIDTH = 102;
static const byte LCD_HEIGHT = 72;
static const byte CHART_HEIGHT = 5;
static const byte DEGREES_CHAR = 1;
static const byte degrees_glyph[] = { 0x00, 0x07, 0x05, 0x07, 0x00 };
static PCD8544 lcd;

void setup() {
  lcd.begin(LCD_WIDTH, LCD_HEIGHT);
  
  lcd.createChar(DEGREES_CHAR, degrees_glyph);
  
  pinMode(clock, OUTPUT);
  pinMode(light, OUTPUT);
  digitalWrite(light, HIGH);


}


void loop() {
  reading:
  static byte xChart = LCD_WIDTH;

  digitalWrite(clock, HIGH);
  digitalWrite(light, HIGH);

  float oleoA = (analogRead(oleo) * 5 * 100.0) / 1024.0;
//
while(x < 1) //Run once and setup the cal value for the map sensor.
     {
     boostcal = analogRead(mapsen); // Calabration value for boost sensor.
     x++;

     }

mapval = map(analogRead(mapsen), 0, 1023, 0, 1023); //Reads the MAP sensor raw value on analog port
boostkPa = (mapval-boostcal);
boostPSI = boostkPa * .14503773773020923; //Converts boostkPa to PSI
//
  float bateriaA = (5 * analogRead(bateria)) / 1024.0 / (R2/(R1+R2));
  float discosA = (5 * analogRead(discos) * 100.0) / 1024.0;
  float dieselA = (5 * analogRead(diesel) * 100.0) / 1024.0;
  float aguaA = (5 * analogRead(agua) * 100.0) / 1024.0;


  lcd.setCursor(0, 0);
  lcd.print("Informacao geral");
  lcd.setCursor(0, 2);
  lcd.print("Agua");
  lcd.setCursor(55, 2);
  lcd.print(aguaA, 0);
  lcd.setCursor(85, 2);
  lcd.print("\001C ");
  lcd.setCursor(0, 3);
  lcd.print("Oleo");
  lcd.setCursor(55, 3);
  lcd.print(oleoA, 0);
  lcd.setCursor(85, 3);
  lcd.print("\001C ");
  lcd.setCursor(0, 4);
  lcd.print("Turbo");
  lcd.setCursor(55, 4); 
  lcd.print("   ");
  lcd.setCursor(55, 4);
  lcd.print(boostPSI);
  lcd.setCursor(80, 4);
  lcd.print("PSI");
  lcd.setCursor(0, 5);
  lcd.print("Bateria");
  lcd.setCursor(55, 5);
  lcd.print(bateriaA, 2);
  lcd.setCursor(85, 5);
  lcd.print("+V");
  lcd.setCursor(0, 6);
  lcd.print("Discos");
  lcd.setCursor(55, 6);
  lcd.print(discosA, 0);
  lcd.setCursor(85, 6);
  lcd.print("\001C ");
  lcd.setCursor(0, 7);
  lcd.print("Diesel");
  lcd.setCursor(55, 7);
  lcd.print(dieselA, 2);
  lcd.setCursor(85, 7);
  lcd.print("\001C ");
  digitalWrite(clock, LOW);
  if (digitalRead(fagua) == LOW){
  lcd.setCursor (0, 1);
  lcd.print("Falta de agua");}
  else{
  lcd.setCursor (0, 1);
  lcd.print("             ");}
  delay(500);
}
