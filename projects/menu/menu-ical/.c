/*****************************************************
Project : 
Version : 
Date    : 9/15/2012
Author  : Freeware, for evaluation and non-commercial use only
Company : 
Chip type               : ATmega16
Program type            : Application
AVR Core Clock frequency: 12.000000 MHz
Memory model            : Small
External RAM size       : 0
Data Stack size         : 256
*****************************************************/
#include <mega16.h>
#asm
   .equ __lcd_port=0x15 ;PORTC
#endasm
#include <delay.h>
#include <stdio.h>


// Alphanumeric LCD Module functions
#include <alcd.h>
// Declare your global variables here
unsigned char x,y;




/*prosedur utama*/
void main(void)
{
/*definisi port*/
PORTA=0x00; DDRA=0xF0; //pina.0 - pina.3 input, pina.4 - pina.7 ouput
PORTB=0x00; DDRB=0x00;
PORTC=0x00; DDRC=0x08; // 0000-1000 = pinc.3 NC
//PORTD=0x00; DDRD=0x00;
PORTD=0xFF; DDRD=0x07;

// Alphanumeric LCD initialization
// Connections specified in the
// Project|Configure|C Compiler|Libraries|Alphanumeric LCD menu:
// RS - PORTC Bit 0
// RD - PORTC Bit 1
// EN - PORTC Bit 2
// D4 - PORTC Bit 4
// D5 - PORTC Bit 5
// D6 - PORTC Bit 6
// D7 - PORTC Bit 7
// Characters/line: 16

/* initialization */
lcd_init(16);
lcd_gotoxy(0,0); lcd_putsf("ical Instrument");
lcd_gotoxy(0,1); lcd_putsf(" -Program Menu- ");
 delay_ms(3000);
lcd_clear();
lcd_gotoxy(0,0);lcd_putsf("   Loading ");      
lcd_gotoxy(0,1);lcd_putsf("...");delay_ms(1000); 
lcd_gotoxy(0,4);lcd_putsf(".....");delay_ms(1000);
lcd_gotoxy(0,10);lcd_putsf("......");delay_ms(1000);
lcd_clear();

      { 
      menu: //label menu       
       lcd_clear(); 
       lcd_gotoxy(0,0); 
       lcd_putsf("HOME,Tekan *&0=");
       lcd_gotoxy(0,1); 
       lcd_putsf("Menu Led, #=Set");
        while(1)
        {
                PORTA = 0b01111111;  //pina.7 = row 4
                delay_us(1);                                  
                if (PINA.0 ==0 && PINA.1 ==0)// tekan -> * dan 0
                {  
                        lcd_clear();
                        lcd_gotoxy(0,0);
                        lcd_putsf(" - Menu LED -  "); 
                        PORTA = 0b11101111; //pina.4 = row 1 
                        delay_us(1);     
                while(1)
                  {                       
                        //Mengaktifkan LED 1
                        if (PINA.0==0)//kolom1 -> tekan 1    
                        {                                             
                                 if (x==1)// kalau sudah disetting
                                {           
                                lcd_clear();
                                lcd_gotoxy(0,0);
                                lcd_putsf("LED 1 ON");
                                lcd_gotoxy(0,1);
                                lcd_putsf("tekan 3 setting");
                                PORTD.0 = 0;
                                PORTD.2 = 1; 
                                }                         
                                 else if (x==0)// kalau tidak disetting
                                {
                                lcd_clear();
                                lcd_gotoxy(0,0);      
                                lcd_putsf("LED1 belum diset");
                                lcd_gotoxy(0,1);
                                lcd_putsf("tekan 3 setting");
                                PORTD.0 = 1;
                                PORTD.2 = 1;    
                                };
                        }                   

                        //Mengaktifkan LED 2
                        else if (PINA.1==0)//kolom2 -> tekan 2  
                        {
                                 if (y==1)// kalau sudah disetting
                                 {              
                                  lcd_clear();
                                  lcd_gotoxy(0,0);
                                  lcd_putsf("LED 2 ON");  
                                  lcd_gotoxy(0,1);
                                  lcd_putsf("tekan 3 setting");
                                  PORTD.0 = 1;
                                  PORTD.2 = 0;
                                }                         
                                 else if (y==0)// kalau tidak disetting
                                {           
                                lcd_clear();
                                lcd_gotoxy(0,0);      
                                lcd_putsf("belum disetting");  
                                lcd_gotoxy(0,1);
                                lcd_putsf("LED2 belum diset");
                                PORTD.0 = 1;
                                PORTD.2 = 1;   
                                };        
                        }                              
                                                       
                        //optional
                        else if (PINA.2==0)//kolom3 -> tekan 3   
                        {         
                        goto optional;         
                        };                       
                  }                          
                  
                }                   
                // Masuk menu optional -> tekan 0 
                else if (PINA.1==0) 
                {  
                        optional:           
                        lcd_clear();
                        lcd_gotoxy(0,0);
                        lcd_putsf("tekan->4 ALL ON,");
                        lcd_gotoxy(0,1);
                        lcd_putsf("5 ALL OFF,6 Set");
                        
                        PORTA = 0b11011111;  //pina.5 = row 2
                        delay_us(1);                  
                      while(1)
                      {  
                        //mengaktifkan semua led
                        if (PINA.0==0) //-> tekan 4 
                        {    
                         if(x==1 && y==1)
                         {
                         lcd_clear();
                         PORTD.0=0;
                         PORTD.2=0;
                         lcd_gotoxy(0,0);
                         lcd_putsf("LED 1&2 ON");
                         lcd_gotoxy(0,1);
                         lcd_putsf("6ke-menu setting");
                         }
                         else
                         {                                    
                          lcd_clear();
                          lcd_gotoxy(0,0);
                          lcd_putsf("belum disetting");
                          lcd_gotoxy(0,1);
                          lcd_putsf("6ke-menu setting");
                         };
                         }                       
                        
                        //mematikan LED 1&2 dan Set menjadi 0
                        else if (PINA.1==0) //-> tekan 5
                        {                         
                         PORTD.0 = 1;
                         PORTD.2 = 1;
                         x = 0;
                         y = 0;
                         lcd_clear();
                         lcd_gotoxy(0,0);
                         lcd_putsf("-ALL OFF/Set 0-");
                         lcd_gotoxy(0,1);
                         lcd_putsf("6ke-menu setting");
                        }                        
                                           
                        //lonjat ke label atur
                        else if (PINA.2==0) //-> tekan 6
                        {  
                        goto atur;
                        };
                      }
                }
                
                // Masuk Menu Setting tekan #
                
                else if (PINA.2==0)                
                
                {             
                 atur:
                 lcd_clear();    
                 lcd_gotoxy(0,0);
                 lcd_putsf("MenuSetLED,Tekan");
                 lcd_gotoxy(0,1);
                 lcd_putsf("7Set-LED1,8-LED2");
                                  
                        while(1)
                        {
                                PORTA = 0b10111111;
                                delay_us(1);
                             
                                //setting untuk menyalakan LED 1
                                 if (PINA.0==0)// -> tekan 7
                                {
                                x = 1;        
                                lcd_clear();
                                lcd_gotoxy(0,0);
                                lcd_putsf("- LED 1 Set -");  
                                lcd_gotoxy(0,1);
                                lcd_putsf("tekan 9 Ke-Menu");
                                }                
                                
                                //setting untuk menyalakan LED 2
                                else if (PINA.1==0)// -> tekan 8
                                {
                                y = 1;        
                                lcd_clear();
                                lcd_gotoxy(0,0);
                                lcd_putsf("- LED 2 Set -");  
                                lcd_gotoxy(0,1);
                                lcd_putsf("tekan 9 Ke-Menu");
                                }                 

                                //setting untuk kembali ke-menu
                                else if (PINA.2==0)//-> tekan 9
                                {
                                 goto menu;
                                };
                        }
                 
                };                       
         }            
      };
}

