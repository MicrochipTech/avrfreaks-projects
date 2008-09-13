/*****************************************************
This program was produced by the
CodeWizardAVR V1.24.6 Standard
Automatic Program Generator
© Copyright 1998-2005 Pavel Haiduc, HP InfoTech s.r.l.
http://www.hpinfotech.com
e-mail:office@hpinfotech.com

Project : LCD Test
Version : 
Date    : 22-2-2007
Author  : Patrick Loef                    
Company : -                               
Comments: 
------------------------------------------------------
Version: 
        0,1: First release 
                - Clear and Fill LCD
                - 16 colors
        
        0,2: Second release
                - Font 8x8
                - BMP picture working
                
        0,3: Release
                - 256 colors text possible
                - separate LUT source in file
                - possible to define 16 or 256 colors   
        0,4: Release
                - 256 color bmp picture possible
                - BMP LUT table compatible, no need to manual switch the RED and BLUE value
                - Debug information in ENGLISH
                                                      
        0,5: Release
                - Compact flash driver added
                - CF info present on LCD(When choosed in Terminal menu)
                - Application tool avaible to send BMP to CF
                - Draw picture from CF data to S1D13705
                - Terminal menu present for settings
                - Link menu present(When choosded in Terminal menu)
                - Baudrate increased to 57600, this the max of 16Mhz      
                - Make a define option in S1D13705.h for more info of the S1D13705
        0,6: Release           
                - Mod: Increased the receiving speed a lot! This done with receive 512 bytes and then send the ready sign
                - Mod: Changed Blok to Page  
                - Mod: Changed calculation of the sector of the CF info (152 * Page) + 1, default of Page always 0
                - Mod: Changed Show_BMP '1 to 32' to '0 to 32' and adjusted function for that
                - Bug: Only Page 1 was written, error at the check function in RX_BMP_Data:
                       the check was uncorrect, The /else Page = 1/ removed and set as default value
                       also the i-1 function in CF_Page[i-1] changed to CF_Page[i]  
        1.0: Release
                - Mod:  Function CF_Info disabled for memory programm space
                - Mod:  Added touchscreen driver
                - Mod:  Added touchscreen calibration
                - Mod:  Added vertical and horizontal line drawing S1D13705
                - Mod:  Added LTC1298(ADC) driver for touchscreen control
                - Mod:  When pressed on the touchscreen a cross is drawed
                - Mod:  Added storing calibration coeffecients and touchscreen flag register in eeprom
                
------------------------------------------------------
Chip type           : ATmega162
Program type        : Application
Clock frequency     : 16,000000 MHz
Memory model        : Small
External SRAM size  : 0
Data Stack size     : 256
*****************************************************/

#include <mega162.h>
#include <stdio.h> 
#include <delay.h> 
#include <string.h>
#include <spi.h>

//Include niet standaard bibliotheek file's
#include "LCD_Test_Defs.h"
#include "S1D13705.h"
#include "S1D13705_LUT.h"       
#include "Font8x8.h" 
#include "cf_driver.h"  
#include "cf_functions.h"                     
#include "RX_BMP_Data.h"
#include "Touchscreen.h"
#include "ADC_LTC1298.h"

//Global variabelen      
unsigned char CF_Sector_Data[512]; 
unsigned char string[20];   
unsigned char CF_Page[32];        

//Global variabelen structere
typedef struct
{
        unsigned char Flag_Register;
        unsigned int X_pos;
        unsigned int Y_pos;
}TouchData;
TouchData Touchscreen;

//Eeprom variabelen
eeprom unsigned char ee_Touch_Flag = 0;                 //Eeprom touchscreen flag register
eeprom float ee_float1 = 0.0;           
eeprom float ee_float2 = 0.0;
eeprom float ee_float3 = 0.0;
eeprom float ee_float4 = 0.0;
eeprom float ee_float5 = 0.0;
eeprom float ee_float6 = 0.0;



/**************************************
Functie: Laad geslecteerde plaatje zien
**************************************/
void Show_BMP(void)
{
        int Page_Number;
        
        while(1)
        {
                printf("\n\rGive Page number(0 to 31)");
                scanf("%d",&Page_Number);
        
                if(Page_Number<0 | Page_Number>31)
                        printf("\n\rNot correct between 0 and 31");
                else
                        break;
        }
        
        //Check of blok data aanwezig is
        if(CF_Page[Page_Number] != Page_Number)
        {
                printf("\n\rPage %d is not present, return",Page_Number);
                return;
        }
        
        //Laat plaatje zien
        printf("\n\rShow BMP picture: %d",Page_Number);
        
        CF_Draw_Pic(0,0,Page_Number);
        
        return;
}
//*************************************

/************************************** 
External Interrupt 0 service routine  
**************************************/
interrupt [EXT_INT0] void ext_int0_isr(void)
{
        unsigned long int i;
        unsigned int c;
        
        //Interupt uit
        GICR &= ~(1<<INT0);                             
        
        if((Touchscreen.Flag_Register & (1<<TouchCalProcess)) == (1<<TouchCalProcess))                          //Als calibratie bezig is dan terug Flag zetten en terug gaan
        {
                Touchscreen.Flag_Register |= (1<<TouchPressed);                 //Zet TouchPress flag
                printf("\n\rTouchPressed");
        }
        else
        {
                Touchscreen.Flag_Register |= (1<<TouchPressed);                 //Zet TouchPress flag
                
                Touchscreen_XY_positie();                                       //Meet XY postie
                printf("\n\rTouchPressed");

                GIFR |= (1<<INT0);                                              //Erase bit so that no interupt is generated 
                GICR |= (1<<INT0);                                              //Enable the INT0 request
        }

        if((Touchscreen.Flag_Register & (1<<XY_Value)) == (1<<XY_Value))
        {
               // printf("\n\rX: %d, Y: %d",Touchscreen.X_pos,Touchscreen.Y_pos);
                Touchscreen.Flag_Register &= ~(XY_Value);
                TouchscreenTest(Touchscreen.X_pos,Touchscreen.Y_pos);
        }
                
}
//*************************************
  
void TouchscreenTest(unsigned int X, unsigned int Y)
{
        static unsigned int Xbak;
        static unsigned int Ybak;  
        
        S1D13705_Draw_Horizontal_Line(X-3,X+3,Y,0);                             //Instellen als X(begin), X1(eind), Y, kleur
        S1D13705_Draw_Vertical_Line(X,Y-3,Y+3,0);                               //Instellen als X, Y(begin), Y1(eind), kleur     
        
        S1D13705_Draw_Horizontal_Line(Xbak-3,Xbak+3,Ybak,15);                   //Instellen als X(begin), X1(eind), Y, kleur
        S1D13705_Draw_Vertical_Line(Xbak,Ybak-3,Ybak+3,15);                     //Instellen als X, Y(begin), Y1(eind), kleur      
        
        Xbak = X;
        Ybak = Y;
        return;
}

/**************************************
Functie: MAIN
**************************************/
void main(void)
{
// Declare your local variables here

#ifdef Info_S1D13705
unsigned char r;
unsigned char g;
unsigned char b;
#endif

unsigned char Blok_Data;
unsigned char i;  
unsigned int T_waarde;  


// Crystal Oscillator division factor: 1
#pragma optsize-
CLKPR=0x80;
CLKPR=0x00;
#ifdef _OPTIMIZE_SIZE_
#pragma optsize+
#endif

// Input/Output Ports initialization
// Port A initialization
// Func7=Out Func6=Out Func5=Out Func4=Out Func3=Out Func2=Out Func1=Out Func0=Out 
// State7=0 State6=0 State5=0 State4=0 State3=0 State2=0 State1=0 State0=0 
PORTA=0x00;
DDRA=0xFF;

// Port B initialization
// Func7=In Func6=In Func5=In Func4=Out Func3=Out Func2=Out Func1=Out Func0=Out 
// State7=T State6=T State5=T State4=0 State3=0 State2=1 State1=1 State0=1 
PORTB=0x07;
DDRB=0xBF;

// Port C initialization
// Func7=Out Func6=Out Func5=Out Func4=Out Func3=Out Func2=Out Func1=Out Func0=Out 
// State7=0 State6=0 State5=0 State4=0 State3=0 State2=0 State1=0 State0=0 
PORTC=0x00;
DDRC=0xFF;

// Port D initialization
// Func7=Out Func6=Out Func5=Out Func4=Out Func3=Out Func2=In Func1=In Func0=In 
// State7=1 State6=1 State5=1 State4=1 State3=1 State2=T State1=T State0=T 
PORTD=0xF0;
DDRD=0xF8;

// Port E initialization
// Func2=Out Func1=Out Func0=In 
// State2=0 State1=0 State0=T 
PORTE=0x00;
DDRE=0x06;

// Timer/Counter 0 initialization
// Clock source: System Clock
// Clock value: Timer 0 Stopped
// Mode: Normal top=FFh
// OC0 output: Disconnected
TCCR0=0x00;
TCNT0=0x00;
OCR0=0x00;

// Timer/Counter 1 initialization
// Clock source: System Clock
// Clock value: Timer 1 Stopped
// Mode: Normal top=FFFFh
// OC1A output: Discon.
// OC1B output: Discon.
// Noise Canceler: Off
// Input Capture on Falling Edge
// Timer 1 Overflow Interrupt: Off
// Input Capture Interrupt: Off
// Compare A Match Interrupt: Off
// Compare B Match Interrupt: Off
TCCR1A=0x00;
TCCR1B=0x00;
TCNT1H=0x00;
TCNT1L=0x00;
ICR1H=0x00;
ICR1L=0x00;
OCR1AH=0x00;
OCR1AL=0x00;
OCR1BH=0x00;
OCR1BL=0x00;

// Timer/Counter 2 initialization
// Clock source: System Clock
// Clock value: Timer 2 Stopped
// Mode: Normal top=FFh
// OC2 output: Disconnected
ASSR=0x00;
TCCR2=0x00;
TCNT2=0x00;
OCR2=0x00;

// Timer/Counter 3 initialization
// Clock value: Timer 3 Stopped
// Mode: Normal top=FFFFh
// Noise Canceler: Off
// Input Capture on Falling Edge
// OC3A output: Discon.
// OC3B output: Discon.
// Timer 3 Overflow Interrupt: Off
// Input Capture Interrupt: Off
// Compare A Match Interrupt: Off
// Compare B Match Interrupt: Off
TCCR3A=0x00;
TCCR3B=0x00;
TCNT3H=0x00;
TCNT3L=0x00;
ICR3H=0x00;
ICR3L=0x00;
OCR3AH=0x00;
OCR3AL=0x00;
OCR3BH=0x00;
OCR3BL=0x00;

// External Interrupt(s) initialization
// INT0: Off
// INT0 Mode: Falling Edge
// INT1: Off
// INT2: Off
// Interrupt on any change on pins PCINT0-7: Off
// Interrupt on any change on pins PCINT8-15: Off
GICR=0x00;
MCUCR=0x02;
EMCUCR=0x00;

// Timer(s)/Counter(s) Interrupt(s) initialization
TIMSK=0x00;
ETIMSK=0x00;

// USART0 initialization
// Communication Parameters: 8 Data, 1 Stop, No Parity
// USART0 Receiver: On
// USART0 Transmitter: On
// USART0 Mode: Asynchronous
// USART0 Baud rate: 57600 (Double Speed Mode)
UCSR0A=0x02;
UCSR0B=0x18;
UCSR0C=0x86;
UBRR0H=0x00;
UBRR0L=0x22;

// SPI initialization
// SPI Type: Master
// SPI Clock Rate: 125 kHz
// SPI Clock Phase: Cycle Start
// SPI Clock Polarity: High
// SPI Data Order: MSB First
SPCR=0x13;
SPSR=0x00;

// Analog Comparator initialization
// Analog Comparator: Off
// Analog Comparator Input Capture by Timer/Counter 1: Off
ACSR=0x80;
//*************************************

/**************************************
             Main program
**************************************/

        #asm("sei");                                            //Enable the global interupt.
        
        //Init de compact flash
        CF_Init();
        
        //Init Touchscreen
        Touchscreen_Init();
        
        
        printf("\n\rHello World!!\n\r");
        printf("\n\rS1D13705 test programm V:0.6");
        printf("\n\rS1D13705 Reset and Init");
        
        //Reset en init de S1D13705
        S1D13705_Reset();
        S1D13705_Init();
        
        printf("\n\rS1D13705 ID: %x", i = S1D13705_Read_Register(0));
        
        #ifdef Info_S1D13705  
        for(i=1;i<29;i++)
                printf("\n\rRegister: %x, value: %x",i, data = S1D13705_Read_Register(i));
        
        //Lees LUT tabel
        S1D13705_Write_Register(0x15,0x00);
        for(i=0;i<16;i++)
        {
                S1D13705_Write_Register(0x15,i);                //Selecteer LUT adress register

                r = S1D13705_Read_Register(0x17);               //Lees de rode kleur
                g = S1D13705_Read_Register(0x17);               //Lees de groene kleur
                b = S1D13705_Read_Register(0x17);               //Lees de blauwe kleur
                printf("\n\rRed: %x, Green: %x, Blue: %x",r,g,b);                   
        }
        #endif
        
        printf("\n\rS1D13705 init done\n\r");


        //Clear het scherm
        S1D13705_Clear_LCD();
        
        //Zet LCD voeding aan
        LCD_PWR_ON;
        
        //Check CompactFlash en lees blok informatie uit
        sprintf(string,"Hello World!");
        S1D13705_Text_LCD(1,0,string,0);                       //Center tekst = (ScanlineBytes/2) - ((Lengestring*8)/4)   
        
        sprintf(string,"Make use of the terminal for settings");
        S1D13705_Text_LCD(1,2,string,0);    
        
        //Check of touchscreen is gekalibreerdt
        if((Touchscreen.Flag_Register & (1<<TouchCalibratie)) == 0)   
        {
                printf("Touchscreen niet geijkt, ga nu ijken");
                Touchscreen_Calibratie();
        }
        
        //Hoofdmenu op de terminal
        while(1)                
        {
                T_waarde = 0;
                //Check eerst of er blok data aanwezig is
                Blok_Data = CF_Check_Blok_Data();
                
                printf("\n\rMain Menu");
                printf("\n\r1. Link menu");
                printf("\n\r2. Read info CF");
                printf("\n\r3. Delete data on CF");
                printf("\n\r4. Show Picture of selected Blok");
                printf("\n\r5. Touchscreen Calibration\n\r");
                printf("\n\rPage data present: %d",Blok_Data);
                
                i = getchar();
                
                switch(i)
                {
                        case '1':
                                printf("\n\rGo to link menu");
                                Receive_BMP_Data();
                                break;
                        case '2':
                              //  printf("\n\rCheck CF card, info on LCD");
                              //  CF_Read_Info();
                                break;
                        case '3':
                                printf("\n\rDelete data on CF wait..");
                                CF_Delete_Data();
                                printf("\n\rData on CF deleted");
                                break;
                        case '4':
                                if(Blok_Data == 0)
                                        printf("\n\rNo data present on CF");
                                else
                                        Show_BMP();
                                break;
                        case '5':
                                printf("\n\rTouchscreen calibration");
                                Touchscreen_Calibratie();
                                break;                        
                }
        }
        
                     
        
        
        while (1);
}
