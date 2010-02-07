# Comparator.c

Uploaded by fei_cinlong on 2010-02-07 12:58:00 (rating 0 out of 5)

## Summary

/*! \file comparator.c \brief analog comparator function library. */  

/*****************************************************  

//Title : 'comparator.c'  

//Version : 1.0  

//Created : 1/3/2010  

//Author : Prasetyo Gunawan  

//Company : Microcontrolsystem.Corp  

//Comments: this code is to activate Analog Comparator  

// AVR Series Microcontroller  

//Target MCU : Atmel AVR Series  

//Editor Tabs : 4  

//This code is for free distributed  

//which can be found at [http://www.4shared.com/file/2013...](http://www.4shared.com/file/201308576/bfad287a/comparator.html)  

//if you find any bug in this code call me :)  

*****************************************************/  

//compare analog input from AIN0 and AIN1  

//The Analog Comparator compares the input values on the positive pin AIN0  

//and negative pin AIN1. When the voltage on the positive pin AIN0 is higher  

//then the voltage on the negative pin AIN1, the Analog comparator output ACO is set  

//the comparator's output can be set to trigger the timer/counter1 input capture function  

//In addition the comparator can trigger a separate interrupt, exclusive  

//to the analog comparator. The user can select interrupt triggering on comparator output rise,  

//fall, or toggle  

//The output of Analog Comparator is on the Analog Comparator Control and Status Register(ACSR)  

//bit-5  

//for more informaation you can read the datasheet of atmel AVR Series  

//Description :  

//  

// |=============================================================|  

// |Pin AIN1 in analog comparator internal AVR as negative input |  

// |Pin AIN0 in analog comparator internal AVR as positive input |  

// |if AIN0 > AIN1 the output analog comparator is high "1" |  

// |if AIN1 > AIN0 the output analog comparator is low "0" |  

// |=============================================================|


#include "global.h"  

#include   

#include   

uint8\_t hasil = 0;  

uint8\_t output\_comparator = 0;  

void compare(void)  

//ACSR and with 0x20 to capture the value of output  

{hasil = ACSR & 0x20;  

 switch(hasil){  

 case 0x00:  

//if AIN0 < AIN1 then var capture = 0  

//The output value is 0  

//the value of hasil is 0  

 output\_comparator = 0;  

 break;  

 case 0x20:  

//The output value is 1  

//the value of hasil is 0x20  

//if AIN0 > AIN1 then var capture = 1  

 output\_comparator = 1;  

 break;  

 default:  

 break;}}

## Compilers

- AVR Studio 4

## Tags

- General Lib.functions
- AVR Studio 4
