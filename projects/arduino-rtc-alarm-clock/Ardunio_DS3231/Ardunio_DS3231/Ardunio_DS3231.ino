/*ALARM CLOCK - Arduino & DS3231
 * author: Dejan Rakijasic
 * date: 27.2.2016.
 * 
 * Arduino alarm clock with DS3231 and IC2 16x2 LCD  
 * FEATURES:
 * Display Time 24h format
 * Display Date in dd/mm/yyyy format
 * Display Day of Week - auto generated DoW
 * Display Temperature (DS3231 function)
 * Menu based navigation system
 * DS3231 - Alarm1  
 * Save information of Alarm in EEPROM
 * Debounce buttons 
 * 
 * LCD (I2C module):
 * SCL - A5
 * SDA - A4
 * VCC
 * GND
 *
 * */
#include <LiquidCrystal_I2C.h> // https://github.com/fdebrabander/Arduino-LiquidCrystal-I2C-library
#include <Button.h>        //https://github.com/JChristensen/Button
#include <DS3232RTC.h>        //http://github.com/JChristensen/DS3232RTC
#include <Streaming.h>        //http://arduiniana.org/libraries/streaming/
#include <Time.h>         //http://www.arduino.cc/playground/Code/Time  
#include <Wire.h>         //http://arduino.cc/en/Reference/Wire (included with Arduino IDE)
#include <EEPROM.h>

LiquidCrystal_I2C lcd(0x27,16,2);  // address LCD(16x2) 0x27 (or 0x3F)

#define BUTTON_PIN_UP 6
#define BUTTON_PIN_DOWN 5   
#define BUTTON_PIN_MENU_SELECT 8
#define BUTTON_PIN_btnBACK 4


#define SQW_PIN 2 //interrupt Alarm pin

#define DEBOUNCE_MS 20     //A debounce time of 20 milliseconds usually works well for tactile button switches.
  
#define REPEAT_FIRST 1000   //ms required before repeating on long press
#define REPEAT_INCR 200    //repeat interval for long press
#define PULLUP true        //To keep things simple, we use the Arduino's internal pullup resistor.
#define INVERT true
//tipkalo
int count;                            //The number that is adjusted
int lastCount = -1;                   //Previous value of count (initialized to ensure it's different when the sketch starts)
unsigned long rpt = REPEAT_FIRST;     //A variable time that is used to drive the repeats for long presses


Button buttonUp(BUTTON_PIN_UP, PULLUP, INVERT, DEBOUNCE_MS);    //Declare the buttons
Button buttonDown(BUTTON_PIN_DOWN, PULLUP, INVERT, DEBOUNCE_MS);
Button buttonSelect(BUTTON_PIN_MENU_SELECT, PULLUP, INVERT, DEBOUNCE_MS);
Button buttonbtnBACK(BUTTON_PIN_btnBACK, PULLUP, INVERT, DEBOUNCE_MS);

// input actions
enum {btnMENU, btnBACK, btnUP, btnDOWN};


/*
 * States of FSM
 */
enum STATES
{
  MAIN,
  MENU_SET_ALARM,
  MENU_SET_TIME,
  MENU_SET_DATE,

  SET_HOUR,
  SET_MINUTE,

  SET_DAY,
  SET_MONTH,
  SET_YEAR,

  SET_AL_HOUR,
  SET_AL_MINUTE,
  SET_AL_ON_OFF,
  ALARM_TIME,
                    //   Otherwise, it times out after 5 seconds, discards the changes and returns to displaying the time
};
STATES state;       // Holds the current state of the system


int8_t button;                             
int8_t trigger;                            

int Second;                         
int Minute;
int Hour;     
int Day;    
int Month;   
int Year;    

int8_t DoW;
String day_of_week;
unsigned char address, data;

int AL_Hour;
int AL_Minute;
bool alarm_active = false; 
int alarm_out = 10; 

uint32_t blink_interval  = 500;               
uint32_t blink_previousMillis = 0;              
boolean blink_state  = false;           
boolean RTC_error = true;                    
boolean long_press_button = false;

 byte bell_symbol[8] = {
        B00100,
        B01110,
        B01110,
        B01110,
        B01110,
        B11111,
        B00000,
        B00100};
        
byte thermometer_symbol[8] = {
        B00100,
        B01010,
        B01010,
        B01110,
        B01110,
        B11111,
        B11111,
        B01110};
        
byte char_S[8] = {
  0b11111,
  0b11111,
  0b11000,
  0b11111,
  0b11111,
  0b00011,
  0b11111,
  0b11111};

byte char_D[8] = {
0b11110,
  0b11111,
  0b11011,
  0b11011,
  0b11011,
  0b11011,
  0b11111,
  0b11110};

byte char_J[8] = {
 0b11111,
 0b11111,
  0b00011,
  0b00011,
  0b00011,
  0b10011,
  0b11111,
  0b11111};

  byte menu_RIGHT[8] = {
   B10000,
   B11000,
   B11100,
   B11110,
   B11100,
   B11000,
   B10000,
   B00000};

    byte menu_LEFT[8] = {
      B00001,
        B00011,
        B00111,
        B01111,
        B00111,
        B00011,
        B00001,
        B00000};

/*
 * SETUP
 */
void setup()
{
    lcd.init();                  
 lcd.backlight();
    Wire.begin();                     // start I2C
pinMode(11,INPUT_PULLUP);

 pinMode(alarm_out, OUTPUT);

   lcd.createChar(0, thermometer_symbol);
    lcd.createChar(1, bell_symbol);
   lcd.createChar(2, char_S);
    lcd.createChar(3, char_D);
    lcd.createChar(4, char_J);
        lcd.createChar(5, menu_RIGHT);
    lcd.createChar(6, menu_LEFT);

    Wire.begin();
 

    setSyncProvider(RTC.get);               // set RTC as the Syncprovider
    setSyncInterval(5);                     // time in sec of resync with RTC

    //Disable the default square wave of the SQW pin.
    RTC.squareWave(SQWAVE_NONE);

//Attach an interrupt on the falling of the SQW pin.
    //digitalWrite(SQW_PIN, HIGH);    //redundant with the following line
    pinMode(SQW_PIN, INPUT_PULLUP);
    attachInterrupt(INT0, alarmIsr, FALLING);

 lcd.clear();
lcd.setCursor(5, 0); 
lcd.write(2);
lcd.setCursor(6, 0); 
lcd.write(2); 
lcd.setCursor(7, 0); 
lcd.write(3); 
lcd.setCursor(8, 0); 
lcd.write(4); 
lcd.setCursor(0, 1); 
lcd.print("Arduino & DS3231"); 
delay(2000); 
  lcd.clear();
    state = MAIN;                      // Initial state of the FSM
}//End SETUP

volatile boolean alarmIsrWasCalled = false;

void alarmIsr()
{
    alarmIsrWasCalled = true;
}


/*
 * LOOP 
 */
void loop()
{
  
    change_states();                         // change states of FSM
    check_inputs();                       // check inputs (buttons)
    check_RTC();                       // check connection RTC

       
    if (alarmIsrWasCalled){
     
        if (RTC.alarm(ALARM_1)) {
             lcd.clear();
             lcd.setCursor(1,0);
             lcd.print("Wake up!!!");
               lcd.setCursor(1,1);
               lcd.print("Wake up!!!");
          
    for(int i=0;i<25; i++){
        buttonbtnBACK.read();
      if (buttonbtnBACK.wasPressed())
      {
           alarmIsrWasCalled = false;  
        break;
      }
              beep(); 
           }

        }  
           lcd.clear();     
    }

        alarmIsrWasCalled = false;
}// End of LOOP


/*
Finite State Machine
*/
void change_states()
{
        //states
    switch (state)
    {
         case MAIN:
    display_time();
    display_date();
    get_alarm(); //read stored alarm time
    display_temperature();
    break;
    case MENU_SET_ALARM:
    menu_set_alarm();
    break;
    case MENU_SET_TIME:
     menu_set_time();
    break;
    case MENU_SET_DATE:
       menu_set_date();
    break;
        case SET_HOUR:
          display_set_time();
            break;
        case SET_MINUTE:
            display_set_time();
            break;
        case SET_DAY:
            display_set_date();
            break;
        case SET_MONTH:
            display_set_date();
            break;
        case SET_YEAR:
            display_set_date();
            break;
    case SET_AL_HOUR:
      display_set_AL_time();
    break;
    case SET_AL_MINUTE:
    display_set_AL_time();
    break;
    case SET_AL_ON_OFF:
  display_set_AL_time();
    break; 
             case ALARM_TIME:
  displayALARM();
  delay(2000);
  lcd.clear();
  state=MAIN;
    break; 
        break;
    }

}


/*
 * Check INPUT
 */
void check_inputs()
{   
    buttonUp.read();                      // check state of buttons
    buttonDown.read();                    
    buttonSelect.read();                  
     buttonbtnBACK.read();

     
    switch (buttonSelect.wasPressed())            // check MENU/SELECT button
    {
        case 1:
            button = btnMENU;      
            transition(button);
        break;

    }
    switch (buttonUp.wasPressed())                // check  UP button
    {
        case 1:
            button = btnUP;
            transition(button);
        break;
    }

    switch (buttonUp.wasReleased())                // check long press UP button
    {
        case 1:
        long_press_button = false;
        rpt = REPEAT_FIRST;
        break;
    }

        switch (buttonUp.pressedFor(rpt))                
    {
        case 1:
           rpt += REPEAT_INCR;               //increment the long press interval
         long_press_button = true;
            button = btnUP;
            transition(button);
        break;
    }
    
    switch (buttonDown.wasPressed())              // check  DOWN button
    {
        case 1:
            button = btnDOWN;
            transition(button);
        break;
    }

 switch (buttonDown.wasReleased())                // check long press DOWN button
    {
        case 1:
        long_press_button = false;
        rpt = REPEAT_FIRST;
        break;
    }

        switch (buttonDown.pressedFor(rpt))                
    {
        case 1:
           rpt += REPEAT_INCR;               //increment the long press interval
         long_press_button = true;
            button = btnDOWN;
            transition(button);
        break;
    }
    
    switch (buttonbtnBACK.wasPressed())              // check btnBACK button
    {
        case 1:
            button = btnBACK;
            transition(button);
        break;
    }
}

void transition(int trigger)
{
    switch (state)
    {
        case MAIN:
            if (trigger == btnMENU)
            {
               lcd.clear();
                state = MENU_SET_ALARM;
            }
             else if (trigger == btnBACK)
            {
               lcd.clear();
                state = ALARM_TIME;
            }
            break;
            
  case MENU_SET_ALARM:
     if (trigger == btnUP)
    {
      lcd.clear();
      state = MENU_SET_TIME;
    }
    else if (trigger == btnUP)
     {
      lcd.clear();
      state = MENU_SET_DATE;
    }

   if (trigger == btnMENU)
    {
       lcd.clear();
      state = SET_AL_HOUR;
    }
    if (trigger == btnBACK)
    {
       lcd.clear();
      state = MAIN;
    }
    break;


    case MENU_SET_TIME:
     if (trigger == btnUP)
    {
      lcd.clear();
      state = MENU_SET_DATE;
    }
   
    if (trigger == btnDOWN)
    {
      lcd.clear();
      state = MENU_SET_ALARM;
    }
    

   if (trigger == btnMENU)
    {
     lcd.clear();
         
      state = SET_HOUR;
    }
    if (trigger == btnBACK)
    {
       lcd.clear();
      state = MAIN;
    }
    break; 

   case MENU_SET_DATE:
    
    if (trigger == btnDOWN)
    {
      lcd.clear();
      state = MENU_SET_TIME;
    }
    else if (trigger == btnDOWN)
     {
      lcd.clear();
      state = MENU_SET_ALARM;
    }
  
   if (trigger == btnMENU)
    {
       lcd.clear();
      state = SET_DAY;
      
    }
    if (trigger == btnBACK)
    {
       lcd.clear();
      state = MAIN;
    }
    break;


       
        case SET_HOUR:
            if (trigger == btnUP)
            {
                Hour++; 
                if (Hour>23) Hour = 0;
            }
            else if (trigger == btnDOWN)
            {
                Hour--; 
                if (Hour<0) Hour = 23;
            }
            if (trigger == btnMENU)
            {
                state = SET_MINUTE;            
            }
            break;
            
        case SET_MINUTE:
            if (trigger == btnUP)
            {
                Minute++; 
                if (Minute>59) Minute = 0;
            }
            else if (trigger == btnDOWN)
            {
                Minute--; 
                if (Minute<0) Minute = 59;
            }

             if (trigger == btnBACK)
            {
    
               state = SET_HOUR;
                }
              if (trigger == btnMENU)
            {
              
                        set_time();
                 lcd.clear(); 
                 lcd.setCursor(4,0);
                 lcd.print("Time Set!");
                 delay(1000);

                 lcd.clear();
                state = MAIN;              
            }
            break;
            
         
        case SET_DAY:
            if (trigger == btnUP)
            {
                Day++; 
                if (Day>31) Day = 1;
            }
            else if (trigger == btnDOWN)
            {
                Day--; 
                if (Day<1) Day = 31;
            }
            if (trigger == btnMENU)
            {
                state = SET_MONTH;              
            }
            break;
        case SET_MONTH:
            if (trigger == btnUP)
            {
                Month++; 
                if (Month>12) Month = 1;
            }
            else if (trigger == btnDOWN)
            {
                Month--; 
                if (Month<1) Month = 12;
            }
            if (trigger == btnMENU)
            {
                state = SET_YEAR;               
            }
             if (trigger == btnBACK)
            {
    
               state = SET_DAY;
                }
            break;
        case SET_YEAR:
            if (trigger == btnUP)
            {
                Year++; 
                if (Year>99) Year = 0;
            }
            else if (trigger == btnDOWN)
            {
                Year--; 
                if (Year<0) Year = 99;
            }
             if (trigger == btnBACK)
            {
    
               state = SET_MONTH;
                }
            if (trigger == btnMENU)
            {
             set_date();
                lcd.clear();
                 lcd.setCursor(4,0);
                 lcd.print("Date Set!");
                 delay(1000);
                 lcd.clear();
                state = MAIN;              
            }
            break;

             case SET_AL_HOUR:
            if (trigger == btnUP)
            {
                AL_Hour++; 
                if (AL_Hour>23) AL_Hour = 0;
            }
            else if (trigger == btnDOWN)
            {
                AL_Hour--; 
                if (AL_Hour<0) AL_Hour = 23;
            }
            if (trigger == btnMENU)
            {
                state = SET_AL_MINUTE;            
            }
            break;

       case SET_AL_MINUTE:
            if (trigger == btnUP)
            {
                AL_Minute++; 
                if (AL_Minute>59) AL_Minute = 0;
            }
            else if (trigger == btnDOWN)
            {
                AL_Minute--; 
                if (AL_Minute<0) AL_Minute = 59;
            }
              if (trigger == btnMENU)
            {
              state=SET_AL_ON_OFF;  
            }
             if (trigger == btnBACK)
            {
    
               state = SET_AL_HOUR;
                }
            break;

            case SET_AL_ON_OFF:
            if (trigger == btnUP)
            {
                alarm_active = true;
            }
            else if (trigger == btnDOWN)
            {
                 alarm_active =false;
            }

             if (trigger == btnBACK)
            {
    
               state = SET_AL_MINUTE;
                }
            if (trigger == btnMENU)
            {
              set_alarm();
                lcd.clear(); 
                 lcd.setCursor(4,0);
                 lcd.print("Alarm Set!");
                 delay(1000);
                 lcd.clear();
                state = MAIN; 
            }
            
            break;}}



/*
 * FUNCTIONS
 */
void beep(){
  
   digitalWrite(alarm_out,HIGH);
   lcd.noBacklight();
   delay(500);
   digitalWrite(alarm_out, LOW);
   lcd.backlight();
   delay(500);}
 
void menu_set_time(){
   lcd.setCursor(6,0);
    lcd.print("MENU");
       lcd.setCursor(4,1);
    lcd.print("Set Time");
   lcd.setCursor(15,1);
    lcd.write(5);
      lcd.setCursor(0,1);
    lcd.write(6);
}

void menu_set_date(){
   lcd.setCursor(6,0);
    lcd.print("MENU");
       lcd.setCursor(4,1);
    lcd.print("Set Date");
lcd.setCursor(0,1);
    lcd.write(6);
}

void menu_set_alarm(){
   lcd.setCursor(6,0);
    lcd.print("MENU");
       lcd.setCursor(4,1);
    lcd.print("Set Alarm");
  lcd.setCursor(15,1);
    lcd.write(5);
}

void display_time()
{
  get_time();
  lcd.setCursor(0,0);
    display_position(Hour);
    lcd.print(":");
    display_position(Minute);
     lcd.print(":");
    display_position(Second);}

void display_date(){
 get_date();
    lcd.setCursor(8,1);
    display_position(Day);
    lcd.print("/");
    display_position(Month);
     lcd.print("/");
    display_position(Year);
DoW= weekday(); 
    switch(DoW){
       case 1: day_of_week = "Sun"; break;
       case 2: day_of_week  = "Mon"; break;
       case 3: day_of_week  = "Tue"; break;
       case 4: day_of_week  = "Wed"; break;
       case 5: day_of_week  = "Thu"; break;
       case 6: day_of_week  = "Fri"; break;
       case 7: day_of_week  = "Sat"; break;}
  lcd.setCursor(0,1);
   lcd.print (day_of_week);}


void display_set_time(){
      lcd.setCursor(3,0);
      lcd.print("Set Time:");
    switch (state)
    {
    case SET_HOUR:
        lcd.setCursor(10,1);
      lcd.print("h");
     if (long_press_button == false){
     if (blink_state == 0){
    lcd.setCursor(4,1);
    display_position(Hour);}
    else{
      lcd.setCursor(4,1);
        lcd.print("  ");}}
        else{
              lcd.setCursor(4,1);
    display_position(Hour);}
    lcd.print(":");
    display_position(Minute);
    break;
    case SET_MINUTE:
     lcd.setCursor(10,1);
      lcd.print("m");
    lcd.setCursor(4,1);
    display_position(Hour);
    lcd.print(":");
    if (long_press_button == false){
     if (blink_state == 0){
          lcd.setCursor(7,1);
    display_position(Minute);
     }
     else{
          lcd.setCursor(7,1);
        lcd.print("  ");
     }}
     else{
          lcd.setCursor(7,1);
    display_position(Minute);
     }
    break;
    }
unsigned long blink_currentMillis = millis();
if(blink_currentMillis - blink_previousMillis > blink_interval){
blink_previousMillis = blink_currentMillis;
if (blink_state == 0){
blink_state = 1;
}else{
blink_state = 0;
}}}

void display_set_date(){
    lcd.setCursor(3,0);
      lcd.print("Set Date:");
    switch (state)
    {
    case SET_DAY:
    if (long_press_button == false){
 if (blink_state == 0){
       lcd.setCursor(4,1);
        display_position(Day);
        }
    else{
       lcd.setCursor(4,1);
          lcd.print("  ");    
    }
    }
    else{
        lcd.setCursor(4,1);
        display_position(Day);
    }
      lcd.print("/");
    display_position(Month);
       lcd.print("/");
           display_position(Year);
    break;
    case SET_MONTH:
       lcd.setCursor(4,1);
        display_position(Day);
      lcd.print("/");
         if (long_press_button == false){
       if (blink_state == 0){
           lcd.setCursor(7,1);
          display_position(Month);
       }
       else{
           lcd.setCursor(7,1);
          lcd.print("  ");    
       }
         }else{
           lcd.setCursor(7,1);
          display_position(Month);
         }
        
       lcd.print("/");
           display_position(Year);
    break;
    case SET_YEAR:
     
       lcd.setCursor(4,1);
        display_position(Day);
      lcd.print("/");
    display_position(Month);
       lcd.print("/");
        if (long_press_button == false){
       if (blink_state == 0){
         lcd.setCursor(10,1);
          display_position(Year);
       }
    else{
           lcd.setCursor(10,1);
          lcd.print("  ");    
       }}
       else{
           lcd.setCursor(10,1);
          display_position(Year);
       }
    break;
    }
unsigned long blink_currentMillis = millis();
if(blink_currentMillis - blink_previousMillis > blink_interval){
blink_previousMillis = blink_currentMillis;
if (blink_state == 0){
blink_state = 1;
}else{
blink_state = 0;}}}

void display_set_AL_time(){
 lcd.setCursor(1,0);
      lcd.print("Set Alarm Time:");
      if (alarm_active==true){
        lcd.setCursor(12,1);
        lcd.print("ON");
      }else{
       lcd.setCursor(12,1);
        lcd.print("OFF");}
        
    switch (state)
    {
    case SET_AL_HOUR:
        lcd.setCursor(10,1);
      lcd.print("h");
     if (long_press_button == false){
     if (blink_state == 0){
    lcd.setCursor(4,1);
    display_position(AL_Hour);}
    else{
      lcd.setCursor(4,1);
        lcd.print("  ");}}
        else{
              lcd.setCursor(4,1);
    display_position(AL_Hour);
        }
    lcd.print(":");
    display_position(AL_Minute);
    break;
    case SET_AL_MINUTE:
     lcd.setCursor(10,1);
      lcd.print("m");
    lcd.setCursor(4,1);
    display_position(AL_Hour);
    lcd.print(":");
    if (long_press_button == false){
     if (blink_state == 0){
          lcd.setCursor(7,1);
    display_position(AL_Minute);
     }
     else{
          lcd.setCursor(7,1);
        lcd.print("  ");
     }}
     else{
          lcd.setCursor(7,1);
    display_position(AL_Minute);
     }
    break;
    
    case SET_AL_ON_OFF:
          lcd.setCursor(7,1); 
    display_position(AL_Minute);
    
    if (alarm_active == true){
    if (blink_state == 0){
      lcd.setCursor(14,1);
      lcd.print(" ");
    lcd.setCursor(12,1);
    lcd.print("ON");}
    else{
      lcd.setCursor(12,1);
        lcd.print("  ");}}
        else{
          if (blink_state == 0){
             lcd.setCursor(12,1);
    lcd.print("OFF");
          }
          else{
             lcd.setCursor(12,1);
        lcd.print("   ");
          }
        }
    break;
    }
    unsigned long blink_currentMillis = millis();
if(blink_currentMillis - blink_previousMillis > blink_interval){
blink_previousMillis = blink_currentMillis;
if (blink_state == 0){
blink_state = 1;
}else{
blink_state = 0;}}}

void displayALARM(){
  lcd.setCursor(2,0);
      lcd.print("<< ALARM >>");
        lcd.setCursor(3,1);
    display_position(AL_Hour);
     lcd.print(":");
        lcd.setCursor(6,1);
    display_position(AL_Minute);
     lcd.setCursor(9,1);
     lcd.print("h");
   
      if (alarm_active == true){
    lcd.setCursor(13,1);
      lcd.print(" ");
    lcd.setCursor(11,1);
    lcd.print("ON");}
    else{
      lcd.setCursor(12,1);
    lcd.print("OFF");
    }}

void get_time(){
  Wire.beginTransmission(0x68); 
  Wire.write(0); //set register to zero
  Wire.endTransmission();
  Wire.requestFrom(0x68, 3);// 3 bytes (sec, min, hour)
  Second = bcdToDec(Wire.read() & 0x7f);
  Minute = bcdToDec(Wire.read());
  Hour = bcdToDec(Wire.read() & 0x3f);}
  
void get_date(){
  Wire.beginTransmission(0x68); 
  Wire.write(4);//set register to 3 (day)
  Wire.endTransmission();
  Wire.requestFrom(0x68, 3); // 3 bytes (day, month, year) - DOW get from Time.h library
  Day  = bcdToDec(Wire.read());
  Month = bcdToDec(Wire.read());
  Year  = bcdToDec(Wire.read());}

void set_time(){
  Wire.beginTransmission(0x68);
  Wire.write(0x00); 
  Second=0;
  Wire.write(decToBcd(Second));
  Wire.write(decToBcd(Minute));
  Wire.write(decToBcd(Hour));
  Wire.write(0x00); 
  Wire.endTransmission();}

void set_date(){
  Wire.beginTransmission(0x68);
  Wire.write(4);
  //Wire.write(decToBcd(DoW));
  Wire.write(decToBcd(Day));
  Wire.write(decToBcd(Month));
  Wire.write(decToBcd(Year));
  Wire.endTransmission();}


void set_alarm(){ //save alarm time to EEPROM
   EEPROM.write(0, AL_Hour);
   EEPROM.write(1, AL_Minute);
   EEPROM.write(2, alarm_active);

   RTC.setAlarm(ALM1_MATCH_HOURS, 0, AL_Minute, AL_Hour, 0);    //sec,min,hour,0
    RTC.alarm(ALARM_1);                   //ensure RTC interrupt flag is cleared
    if (alarm_active==true){
        RTC.alarmInterrupt(ALARM_1, true);
    }
   else{
     RTC.alarmInterrupt(ALARM_1, false);
   }}

void get_alarm(){ //read alarm time from EEPROM
 AL_Hour = EEPROM.read(0);
 if (AL_Hour > 23) AL_Hour=0;
 AL_Minute = EEPROM.read(1);
 if (AL_Minute > 59) AL_Minute=0;
 alarm_active = EEPROM.read(2);
 if(alarm_active==true){
  lcd.setCursor(5,1);
 lcd.write(1);
 }
 else{
   lcd.setCursor(5,1);
 lcd.print(" ");
 }}

void display_temperature(){
  int temp_C = RTC.temperature() / 4;
lcd.setCursor(11, 0);
  lcd.write(0);  //thermometer symbol 
lcd.setCursor(12, 0);
if (temp_C<10){
      lcd.print('0');}
lcd.print(temp_C);
lcd.print((char)223);
lcd.print("C"); 
}

// check communication with RTC
void check_RTC()
{
    if (timeStatus() != timeSet) 
         RTC_error = true;                   // RTC could not be read
    else
         RTC_error = false;                  // RTC could be read
}

byte decToBcd(byte val){
return ( (val/10*16) + (val%10) );}

byte bcdToDec(byte val){
  return ( (val/16*10) + (val%16) );}


void display_position(int digits){
    if(digits < 10)
    lcd.print("0");
    lcd.print(digits);}

