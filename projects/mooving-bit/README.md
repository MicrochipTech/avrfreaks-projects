# Mooving bit

Uploaded by sirek86 on 2012-06-19 17:09:00 (rating 0 out of 5)

## Summary

/*  

 * main.c  

 *  

 * Created on: 2012-06-16  

 * Author: Sirek  

 */


#include   

#include 


// definicja dla procesora  

#define LED\_PIN (0xFF) // przypisanie stanÃ³w wysokich na wyj.


uint16\_t licznik=1;  

uint8\_t kierunek=0; //kierunek - 0 = w lewo


int main (void)  

{  

 // **** inicjalizacja ***  

 DDRC |= LED\_PIN; //kierunek pinow PC0,1,2,3,4,5,6,7 - wyjsciowy  

 PORTC |= LED\_PIN-1; // podanie stanu wysokiego na wyjÅ›ciu -gaszenie LED


 while (1)  

 {


 if (kierunek ==0)  

 {  

 licznik <<=1;  

 if (licznik> 128)  

 {  

 kierunek=1;  

 licznik >>=1;  

 }  

 }


 if (kierunek==1) {  

 licznik >>=1;


 if (licznik < 2)  

 {  

 kierunek=0;  

 }  

 }  

 PORTC = ~licznik ;  

 \_delay\_ms(40);  

 }  

}

## Compilers

- AVR Studio 5

## Tags

- Complete code
- AVR Studio 5
