# Pressure and stroke sensor

Uploaded by eliealnahri on 2008-05-14 12:57:00 (rating 0 out of 5)

## Summary

Hallo everyone,


I am doing my first project with ATMega16 and as a compiler AVR Studio4. I have to control a pump using these 2 sensors. once when the max pressure is reached the pressure should stop working and second when the max stroke is reached the pump should also stop working. I should also see my actual pressure and stroke on my display. but i have the following problems:


1st i am not able to show the stroke as a number with comma values i am only getting as results 1 or 2 or 3 althow i am using float and i should get output values with commas!


Second i don't know how to use 2 input pins for my sensors data and than read from each input a value and show it on my display. can someone here also help me. as i said it's my new project and i am new here.  

the code i have done untill now and that works very good with the stroke sensor the only problem is that i am not getting a comma value as i said


Thanks for everyone that will try to help me in advance


#include   

#include   

#include 


PROGRAM\_INIT // Initialize the programm  

DDRA= 0b00000000; // Set the PORTA as input Port  

DDRC=0b11111111;  

PORTA =0b11111111; // Circuit bord works with active low  

int ok;  

int yes;  

int A=0;  

int B=0;  

int C;  

int m=1;  

float S;  

float s=0.0;  

//int p=0;  

ok=1;  

yes=1;  

C=1;


 ACTIVATE\_LCD // To activate the display  

 LCD\_XY (4,1) //The text on the Display will start in the first line position 4  

 LCD\_TEXT ("Guten Tag") // Text input  

 LCD\_XY (1,2)  

 LCD\_TEXT ("ASTech Anwender")  

 WAIT\_SEC(4) // wait 2 seconds before the next command starts  

 ACTIVATE\_LCD // Clears the first text  

 LCD\_XY(2,1)  

 LCD\_TEXT ("Bitte eingeben")  

 WAIT\_SEC(4)


 ACTIVATE\_LCD  

 LCD\_TEXT ("Pmax="); // Asks for the following input values  

 LCD\_XY (1,2)  

 LCD\_TEXT ("Hub max=");


while (C) // infinite loop  

{


 while(ok) // As long as ok=1 this loop will works


 {  

 if (A<2501) // Maximum pressure 2500 bar  

 {


 ON\_BUTTON (3) {  

 A=0; // Following commands to reset all the input data  

 ACTIVATE\_LCD  

 LCD\_TEXT ("Pmax="); // Asks for the following input values  

 LCD\_XY (1,2)  

 LCD\_TEXT ("Hub max=");  

 }


 ON\_BUTTON (8) { //If button 8 is pressed here joystik moved to the left  

 WAIT\_SEC(0.5) //wait 0.5 seconds  

 A=A+1; //increment A by 1  

 }


 ON\_BUTTON (7) { // If button 7 is pressed here joystik moved up  

 WAIT\_SEC(0.5) //wait 0.5 seconds  

 A=A+10; //increment A by 10  

 }


 ON\_BUTTON (6) { //If button 6 is pressed here joystik moved to the right  

 WAIT\_SEC(0.5) //wait 0.5 seconds  

 A=A+100; //increment A by 100  

 }


 LCD\_XY (6,1)  

 LCD\_NUMBER (A)  

 LCD\_XY (10,1)  

 LCD\_TEXT (" bar") //Display the value of Pmax on the first line


 ON\_BUTTON (5) { //If button 5 is pressed here joystik moved down  

 ok = 0; //will set ok to 0 and break the 1rst loop  

 }  

 }  

 else  

 {  

 LCD\_XY (6,1)  

 LCD\_TEXT(" ") //Reset the pressure value  

 A=0;  

 }  

 }


 while (yes) //As long as yes is equal to 1 this loop will works


 {


 if (B<11) // Maximum stoke 12mm  

 {  

 ON\_BUTTON (3) { // Following commands to reset all the input data  

 B=0;


 ACTIVATE\_LCD //Displays the old value of the max. pressure  

 LCD\_TEXT ("Pmax=");  

 LCD\_XY (6,1)  

 LCD\_NUMBER (A)  

 LCD\_XY (10,1)  

 LCD\_TEXT (" bar")  

 LCD\_XY (1,2)  

 LCD\_TEXT ("Hub max=");  

 }


 ON\_BUTTON (8) { //If button 8 is pressed moving the joystik to the left  

 WAIT\_SEC(0.5) //wait 0.5 seconds  

 B=B+1; //increment b by 1  

 }


 ON\_BUTTON (7) { //If button 7 is pressed moving the joystik up  

 WAIT\_SEC(0.5) //wait 0.5 seconds  

 B=B+10; //increment B by 10  

 }


 LCD\_XY (9,2)  

 LCD\_NUMBER (B)  

 LCD\_XY (11,2)  

 LCD\_TEXT (" mm") //Display the value of the Hub max on the second line


 ON\_BUTTON (4) { //If button 4 is pressed  

 yes = 0; //will set yes to 0 and break the 2nd loop  

 C=0;  

 }  

 }  

 else  

 {  

 LCD\_XY (9,2)  

 LCD\_TEXT(" ") // Reset the value of Hub  

 B=0;  

 }  

 }  

 }


 {  

 ACTIVATE\_LCD  

 LCD\_XY (2,1)  

 LCD\_TEXT ("Vorgang wird");  

 LCD\_XY (4,2)  

 LCD\_TEXT ("gestartet")  

 }  

 WAIT\_SEC (4)


ACTIVATE\_LCD


//ACTIVATE\_ADC // Activate the analog to digital conversion  

//ADC\_CHANNEL(1) // Choose PORTA PIN0 as input for the AD conversion


ADCSRA= 0b10000100; //activate AD converter  

ADMUX=0; // Input on PIN0  

ADMUX=ADMUX | 0b01000000; //AVCC (5 V) with external capacitor at AREF Pin 


 while (m) // loop as long as m=1  

 {


if (!(PINA & 0b00000010)) // As long as button 2 is pressed


{


ACTIVATE\_LCD  

LCD\_TEXT("Hub=")  

LCD\_XY(9,1)  

LCD\_TEXT("mm")  

LCD\_XY (1,2)  

LCD\_TEXT ("P=")  

LCD\_XY(9,2)  

LCD\_TEXT ("bar")  

 while (s
 {


 PORTC=0b11111110; // output signal for the pump to work  

 ADCSRA = ADCSRA | 0b01000000; //one ADC conversion  

 while (ADCSRA & 0b01000000) // to Start a single conversion and wait untill it´s done  

 {  

 ;  

 }  

 S = ADCL + (ADCH<<8); // Read the ADCW


 // ADCONVERT(S) // Convert the input of PIN0 and save it in S  

 s=(S-54)/91.5; // Calibration for the stroke sensor  

 LCD\_XY(5,1)  

 LCD\_NUMBER (s) // Display the value of the driven stroke


 }


 PORTC=0b11111111; // output signal for the pump to stop  

 ACTIVATE\_LCD  

 LCD\_XY(5,1)  

 LCD\_TEXT ("Ueberhub")  

 LCD\_XY (3,2)  

 LCD\_TEXT("Neu starten?") // When the max stroke is reached this message will be shown on the display  

 WAIT\_SEC (1)  

 ACTIVATE\_LCD  

 WAIT\_SEC (1)


 ON\_BUTTON (3)  

 {  

 s=0; // If the button 3 is pressed for 1 sec the procedure will start again  

 ACTIVATE\_LCD  

 LCD\_TEXT ("Hub=")  

 LCD\_XY(9,1)  

 LCD\_TEXT("mm")  

 LCD\_XY (1,2)  

 LCD\_TEXT ("P=")  

 LCD\_XY(9,2)  

 LCD\_TEXT ("bar")  

 }


 ON\_BUTTON (4) // if button 4 is pressed the procedure will be broken  

 {  

 m=0;  

 ACTIVATE\_LCD  

 LCD\_XY (5,1)  

 LCD\_TEXT ("Vorgang")  

 LCD\_XY (3,2)  

 LCD\_TEXT ("abgebrochen")  

 WAIT\_SEC (4)  

 }


} //if  

else // If button 2 is not pressed than  

{  

PORTC=0b11111111; // output signal for the pump to stop  

ACTIVATE\_LCD  

LCD\_TEXT ("Geringer Gewinde")  

LCD\_XY (4,2)  

LCD\_TEXT ("-uebergang")  

WAIT\_SEC (1)  

ACTIVATE\_LCD  

WAIT\_SEC (1)  

if (!(PINA & 0b00000010))  

{}


}  

 }


ACTIVATE\_LCD  

LCD\_XY (2,1)  

LCD\_TEXT ("Reset zum neu")  

LCD\_XY (5,2)  

LCD\_TEXT ("Neustart")


PROGRAM\_START // Starts the program  

PROGRAM\_END // Ends the program

## Compilers

- AVR Studio 4

## Tags

- Complete code
- AVR Studio 4
