# Ran

Uploaded by hirushanan. on 2020-11-13 04:36:00 (rating 0 out of 5)

## Summary

#include "MAX30100\_PulseOximeter.h"


#define REPORTING\_PERIOD\_MS 1000


#define Heart\_dis\_addr 0x01


#define Sop2\_dis\_addr 0x05


#define connect\_sta\_addr 0x08


unsigned char heart\_rate\_send[8]= {0xA5, 0x5A, 0x05, 0x82,\


0x00, Heart\_dis\_addr, 0x00, 0x00};


unsigned char Sop2\_send[8]= {0xA5, 0x5A, 0x05, 0x82, 0x00, \


Sop2\_dis\_addr, 0x00, 0x00};


unsigned char connect\_sta\_send[8]={0xA5, 0x5A, 0x05, 0x82, 0x00, \


connect\_sta\_addr,0x00, 0x00};


// PulseOximeter is the higher level interface to the sensor


// it offers:


// * beat detection reporting


// * heart rate calculation


// * SpO2 (oxidation level) calculation


PulseOximeter pox;


uint32\_t tsLastReport = 0;


// Callback (registered below) fired when a pulse is detected


void onBeatDetected()


{


// Serial.println("Beat!");


}


void setup()


{


Serial.begin(115200);


// Serial.print("Initializing pulse oximeter..");


// Initialize the PulseOximeter instance


// Failures are generally due to an improper I2C wiring, missing power supply


// or wrong target chip


if (!pox.begin())  

{


// Serial.println("FAILED");


// connect\_sta\_send[7]=0x00;


// Serial.write(connect\_sta\_send,8);


for(;;);


}  

else  

{


connect\_sta\_send[7]=0x01;


Serial.write(connect\_sta\_send,8);


// Serial.println("SUCCESS");


}


// The default current for the IR LED is 50mA and it could be changed


// by uncommenting the following line. Check MAX30100\_Registers.h for all the


// available options.


pox.setIRLedCurrent(MAX30100\_LED\_CURR\_7\_6MA);


// Register a callback for the beat detection


pox.setOnBeatDetectedCallback(onBeatDetected);


}


void loop()


{


// Make sure to call update as fast as possible


pox.update();


// Asynchronously dump heart rate and oxidation levels to the serial


// For both, a value of 0 means "invalid"


if (millis() - tsLastReport > REPORTING\_PERIOD\_MS)  

{


// Serial.print("Heart rate:");


// Serial.print(pox.getHeartRate());


// Serial.print("bpm / SpO2:");


// Serial.print(pox.getSpO2());


// Serial.println("%");


heart\_rate\_send[7]=(uint32\_t)pox.getHeartRate();


Serial.write(heart\_rate\_send,8);


Sop2\_send[7]=pox.getSpO2();


Serial.write(Sop2\_send,8);


tsLastReport = millis();


}


}  

// By Kapilathan

## Compilers

- AVR32 GNU Toolchain

## Tags

- AVR32 GNU Toolchain
