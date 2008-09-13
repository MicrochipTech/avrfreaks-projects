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
/**************************************
S1D13705 Driver

Functie's:
- Lezen en schrijven via Latch naar S1D13705
- Lezen register
- Schrijven register
- Lezen geheugen
- Schrijven geheugen                    

Versie:
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
**************************************/

/*************************************
        Include header file's                   
*************************************/
#include "LCD_Test_Defs.h"
#include "S1D13705.h" 
#include "S1D13705_LUT.h"       
//*************************************

/**************************************
Functie: Lees data van S1D13705
**************************************/
unsigned char S1D13705_Read_Data(unsigned long int adress)
{ 
        unsigned char S1Ddata;
        unsigned int Addr_High;
        unsigned int Addr_Low;

        //Zet Data / Adres port als output
        PORT_AD_ctrl = PORT_OUPUT;                                      //Maak port ad output
        
        //Bepaal of A16 1 of 0 is
        if((adress & 0x10000) == 0x10000)                               //Als waar A16 is 1 anders 0
                PORT_Addr_A16 = 1;
        else
                PORT_Addr_A16 = 0;
                
        //Maak hoge en lage adress waardes
        Addr_High = (adress >> 8) & 0xFF;
        Addr_Low = adress & 0xFF;         
        
        //Latch de lage waarde naar uitgang van de Latch
        PORT_Addr_Low = Addr_Low;
        
        ALE_ON;                                                         //Latch de data, data word nu naar uitgang geschoven
        ALE_OFF;                                                        //Ale uit
        
        //Zet hoge waarde op Port_addr_high
        PORT_Addr_High = Addr_High;
        
        //Zet AD port als input
        PORT_AD_ctrl = PORT_INPUT;
        
        //Enable S1D13705 en RD on
        S1D13705_CS = CS_ON;
        S1D13705_RD = RD_ON;
        
        //Lees de data
        while(S1D13705_WAIT);                                           //Wacht totdat WAIT 0 wordt
        while(S1D13705_WAIT == 0);                                      //Wacht totdat WAIT 1 wordt lees dan data in
        S1Ddata = PORT_Data_IN;                                         //Lees de data van de S1D13705
        
        //Disable S1D13705 en RD off
        S1D13705_RD = RD_OFF;
        S1D13705_CS = CS_OFF;
        
        return(S1Ddata);
}   
//*************************************

/**************************************
Functie: Schrijf data naar S1D13705
**************************************/
void S1D13705_Write_Data(unsigned long int adress, unsigned char data)
{
        unsigned int Addr_High;
        unsigned int Addr_Low;
        
        //Zet Data / Adres port als output
        PORT_AD_ctrl = PORT_OUPUT;                                      //Maak port ad output

        //Bepaal of A16 1 of 0 is
        if((adress & 0x10000) == 0x10000)                               //Als waar A16 is 1 anders 0
                PORT_Addr_A16 = 1;
        else
                PORT_Addr_A16 = 0;
                
        //Maak hoge en lage adress waardes
        Addr_High = (adress >> 8) & 0xFF;
        Addr_Low = adress & 0xFF;         
        
        //Latch de lage waarde naar uitgang van de Latch
        PORT_Addr_Low = Addr_Low;
        
        ALE_ON;                                                         //Latch de data, data word nu naar uitgang geschoven
        ALE_OFF;                                                        //Ale uit
        
        //Zet hoge waarde op Port_addr_high
        PORT_Addr_High = Addr_High;
         
        //Zet data op port
        PORT_Data_OUT = data;                                           //Schrijf de data naar de S1D13705     

        //Enable S1D13705               
        S1D13705_CS = CS_ON;                                            //Enable de S1D13705
                
        //WR enable S1D13705 en wacht op wait
        S1D13705_WR = WR_ON;                                            //Maak WR laag en zet schrijven actief 
        while(S1D13705_WAIT);                                           //Wacht totdat WAIT 0 wordt
        while(S1D13705_WAIT == 0);                                      //Wacht totdat WAIT 1 wordt
        
        //Disable S1D13705 en WR
        S1D13705_WR = WR_OFF;
        S1D13705_CS = CS_OFF;
        
        return;
}
//*************************************

/**************************************
Functie: Lees register S1D13705
**************************************/
unsigned char S1D13705_Read_Register(unsigned char reg)
{
        unsigned char data;
        
        data = S1D13705_Read_Data(S1D13705_Reg_Base_Addr + reg);
        
        return(data);
}
//*************************************

/**************************************
Functie: Schrijf register S1D13705
**************************************/
void S1D13705_Write_Register(unsigned char reg, unsigned char data)
{
        S1D13705_Write_Data(S1D13705_Reg_Base_Addr + reg, data);
        
        return;
}
//*************************************

/**************************************
Functie: Init S1D13705
**************************************/
void S1D13705_Init(void)
{
        unsigned int i;
        
        //Stel registers van S1D13705 in
        for(i=1;i<28;i++)  
                S1D13705_Write_Register(aS1DRegs[i].Index,aS1DRegs[i].Value);
        
        //Stel LUT tabel in
        #ifdef S1D13705_16_colors
        S1D13705_LUT_16();       
        #endif
        
        #ifdef S1D13705_256_colors
        S1D13705_LUT_256();
        #endif
        
        return;
}
//*************************************

/**************************************
Functie: Zet tekst op LCD op x,y voor 4BPP en 8BPP!!! 
         
         4BPP: 2 pixel per byte geheugen 37,5KB
         8BPP: 1 pixel per byte geheugen 75KB
**************************************/
void S1D13705_Text_LCD(unsigned long int X, unsigned long int Y, unsigned char *Tekst, unsigned char color)
{
        unsigned long int S1D_mem;
        unsigned int Ytekst;
        unsigned char Font_Waarde;
        unsigned char data;
        unsigned char ch;
        unsigned char BPP;
        unsigned char bits;
        
        //Check kleurinstelling
        BPP = S1D13705_Read_Register(0x02);                             //Vanuit gaan dat kleuren scherm is alleen reg 2 lezen
        BPP &= 0xC0;                                                    //Alleen 2 hoogste bit gelden 
        
        //X is pixels, Y in regels 240/8 = 30 regels
        Y = Y * 8;                                                      //Doe Y maal acht vanwege 8 pixel font hoogte
        color &= 0x0F;                                                  //Van kleur alleen de low nibbble
        
        switch(BPP)
        {       
                case 0x80:                                              //4BPP
        
                        while(*Tekst != 0)                                              //Alle tekens gehad, verlaat loop
                        {                                                               
                                ch = *Tekst++;                                          //Haal teken uit de string
                
                                for(Ytekst=0;Ytekst<8;Ytekst++)
                                {
                                        Font_Waarde = FontTable[ch - ' '][Ytekst];      //Haal Font waarde uit fonttable

                        
                                        //Bereken geheugen plaats van te schrijven data voor pixel
                                        S1D_mem = X + (Ytekst * 160) + (Y * 160);       
                        
                                        //Check 2 pixels van Tekst(4bpp mode)
                                        //en zet waarde in S1D mem 
                                        data = 0xFF;                                    //Zet data op wit kleur   
                                        if(Font_Waarde & 0x80)
                                                data = (color<<4 | 0x0F);               //Zet high nibble op kleur 
                                        if(Font_Waarde & 0x40)
                                                data &= (0xF0 | color);                 //Zet low nibble op kleur 
                                        S1D13705_Write_Data(S1D_mem++,data);            //Schrijf data naar memory
                
                                        data = 0xFF;                                    //Zet data op wit kleur   
                                        if(Font_Waarde & 0x20)
                                                data = (color<<4 | 0x0F);               //Zet high nibble op kleur 
                                        if(Font_Waarde & 0x10)
                                                data &= (0xF0 | color);                 //Zet low nibble op kleur 
                                        S1D13705_Write_Data(S1D_mem++,data);            //Schrijf data naar memory
                
                                        data = 0xFF;                                    //Zet data op wit kleur   
                                        if(Font_Waarde & 0x08)
                                                data = (color<<4 | 0x0F);               //Zet high nibble op kleur 
                                        if(Font_Waarde & 0x04)
                                                data &= (0xF0 | color);                 //Zet low nibble op kleur 
                                        S1D13705_Write_Data(S1D_mem++,data);            //Schrijf data naar memory
                
                                        data = 0xFF;                                    //Zet data op wit kleur   
                                        if(Font_Waarde & 0x02)
                                                data = (color<<4 | 0x0F);               //Zet high nibble op kleur 
                                        if(Font_Waarde & 0x01)
                                                data &= (0xF0 | color);                 //Zet low nibble op kleur 
                                        S1D13705_Write_Data(S1D_mem++,data);            //Schrijf data naar memory
                                }
                                
                                X += 4;                                                 //Ga 1 karakter naar rechts
                        }
                        break;
                         
                case 0xC0:                                                              //8BPP
                        while(*Tekst != 0)                                              //Alle tekens gehad, verlaat loop
                        {                                                               
                                ch = *Tekst++;                                          //Haal teken uit de string
                
                                for(Ytekst=0;Ytekst<8;Ytekst++)
                                {
                                        Font_Waarde = FontTable[ch - ' '][Ytekst];      //Haal Font waarde uit fonttable

                                        //Bereken geheugen plaats van te schrijven data voor pixel
                                        S1D_mem = X + (Ytekst * 320) + (Y * 320);       
                        
                                        //Check alle 8 bits en schrijf bij 1 de kleur en bij 0 wit
                                        
                                        for(bits=0;bits<8;bits++)
                                        {
                                                //Check of bit 1, zo ja zet pixel kleur
                                                if(Font_Waarde & 0x80)
                                                        data = color;                   //Zet kleur
                                                else
                                                        data = 0xFF;                    //Zet kleur wit
                                                S1D13705_Write_Data(S1D_mem++,data);    //Schrijf data naar memory
                                                
                                                Font_Waarde <<= 1;                      //Font Waarde 1 naar links opschuiven
                                        }
                                }
                                
                                X +=8;                                                  //Ga 1 karakter naar rechts
                        }
                        break;
                }                                        
                         
        return;
}
//*************************************             

/**************************************
Functie: Teken plaatje op scherm
         Plaatje die als header is toegevoed
**************************************/
void S1D13705_Draw_Pic(unsigned int X, unsigned int Y, unsigned char flash* Pic)
{
        unsigned long int S1D_mem;                                      //S1D13705 memory adress
        unsigned int W;                                                 //Breedte plaatje
        unsigned int H;                                                 //Hoogte plaatje
        unsigned int Wx;                                                //Counter breedte
        unsigned int Byte;                                              //Byte teller van de array
        unsigned int Yx;                                                //Waarde voor scanline bytes, by 4bpp 160, by 8bpp 320
        unsigned char BPP;                                              //Kleur waarde
        
        //Check kleurinstelling
        BPP = S1D13705_Read_Register(0x02);                             //Vanuit gaan dat kleuren scherm is alleen reg 2 lezen
        BPP &= 0xC0;                                                    //Alleen 2 hoogste bit gelden 
       
        //Bepaal welke kleur instelling en laad juiste LUT tabel
        switch(BPP)
        {
                case 0x80:                                              //16 kleuren
                        S1D13705_LUT_16_BMP();                          //Schrijf LUT tabel 16 kleuren
                        Yx = 160;                                       //160 is aantal scanline bytes
                        break;
                case 0xC0:                                              //256 kleuren
                        S1D13705_LUT_256_BMP();                         //Schrijf LUT voor 256 kleuren
                        Yx = 320;                                       //320 is aantal scaneline bytes
                        break;
       }
        
        
        //Lees de hoogte en de breedte van de Pic
        Byte = 0;
        W = Pic[Byte++];
        H = Pic[Byte++];
        
        //Zet S1D_mem op begin adress/pixel
        S1D_mem = X + (Y * Yx);
        
        while(H--)
        {
                Wx = W;
                while(Wx--)
                {
                        S1D13705_Write_Data(S1D_mem++,Pic[Byte++]);     //Schrijf data naar S1D13705
                }
                
                //Herbereken S1D_mem voor de volgende lijn
                S1D_mem = (S1D_mem - W) + Yx;
        }                                     
       
        return;
}
//*************************************

/**************************************
Functie: Teken een pixel
**************************************/
void S1D13705_Draw_Pixel(unsigned long int X, unsigned long int Y, unsigned char color)
{
        unsigned long int S1D_mem_addr;
        
        //Bereken welk adress S1D13705 moet worden geschreven Pixel 0,0 = adres 0, Pixel 319,0 = 319 etc
        S1D_mem_addr = X + (Y * 320);                                   //Voor Y doe Y aantal maal de breedte van het display en tel X erbij op
        
        //Schrijf Pixel
        S1D13705_Write_Data(S1D_mem_addr,color);
        
        return;
}
//*************************************

/**************************************
Functie: Teken een horizontale lijn
**************************************/
void S1D13705_Draw_Horizontal_Line(unsigned long int X, unsigned long int X1, unsigned long int Y, unsigned char color)
{
        unsigned long int S1D_mem_addr;
        unsigned int Xa;
        
        //Bereken begin adress van de beginde pixel 
        S1D_mem_addr = X + (Y * 320);
        
        //Teken lijn, eerst checken of X1 kleiner is als X, dan word lijn naar links getekend
        if(X1 < X)                                                      //X1 is kleiner dan X, dan moet lijn links toe
        {
                Xa= X- X1;
                do
                {
                        S1D13705_Write_Data(S1D_mem_addr--,color);
                }
                while(Xa--);
               
        }
        else          
        {
                Xa = X1 - X;
                do
                {
                        S1D13705_Write_Data(S1D_mem_addr++,color);   
                }
                while(Xa--);                
        }
        
        return;
}
//*************************************

/**************************************
Functie: Teken verticale lijn
**************************************/
void S1D13705_Draw_Vertical_Line(unsigned long int X, unsigned long int Y, unsigned long int Y1, unsigned char color)
{
        unsigned long int S1D_mem_addr;
        unsigned long int Ya;
        
        //Bereken begin mem adress van de beginde pixel
 //       S1D_mem_addr = X + (Y * 320);
        
        //Teken lijn, eerst checken of Y1 kleiner is dan Y, dan wordt lijn naar boven getekend     
        if(Y1 < Y)                                                      //Y1 is kleiner dus lijn naar boven tekenen
        {
                Ya = Y - Y1;
                do
                {
                        S1D_mem_addr = X + ((Y + Ya) * 320);                  //Herberken mem addres
                        S1D13705_Write_Data(S1D_mem_addr,color);        //Schrijf data
                }
                while(Ya--);
        }
        else
        {
                Ya = Y1 - Y;
                do
                {
                        S1D_mem_addr = X + ((Y + Ya) * 320);                  //Herberken mem addres
                        S1D13705_Write_Data(S1D_mem_addr,color);        //Schrijf data
                } 
                while(Ya--);
        }
        return;
}
//*************************************
                 
/**************************************
Functie: Vul LCD scherm met een kleur
**************************************/
void S1D13705_Fill_LCD(unsigned long int adress, unsigned long int len, unsigned char color)
{
        color |=(color<<4);
        while(len--)
                S1D13705_Write_Data(adress++,color); 
              
        return;
}
//*************************************

/**************************************
Functie: LCD CLS
**************************************/
void S1D13705_Clear_LCD(void)
{
        S1D13705_Fill_LCD(0,0x14000,15);
        
        return;
}                   
//*************************************

/**************************************
Functie: Reset S1D13705
**************************************/
void S1D13705_Reset(void)
{
        S1D13705_RST = RST_ON;
        #asm("nop");
        S1D13705_RST = RST_OFF;

        return;
}
//*************************************       
 
/**************************************
        LUT table values:
- 16 Colors
- 256 Colors
- own bmp Colors
**************************************/

/*************************************
        LUT values 16 colors
*************************************/
flash unsigned char LUT_16[16*3] =
{ 
	0x00, 0x00, 0x00,  0x00, 0x00, 0xA0,  0x00, 0xA0, 0x00,  0x00, 0xA0, 0xA0,
	0xA0, 0x00, 0x00,  0xA0, 0x00, 0xA0,  0xA0, 0xA0, 0x00,  0xA0, 0xA0, 0xA0,
	0x00, 0x00, 0x00,  0x00, 0x00, 0xF0,  0x00, 0xF0, 0x00,  0x00, 0xF0, 0xF0,
	0xF0, 0x00, 0x00,  0xF0, 0x00, 0xF0,  0xF0, 0xF0, 0x00,  0xF0, 0xF0, 0xF0
};
//*************************************

/**************************************
        LUT values 256 colors
**************************************/
flash unsigned char LUT_256[256*3] =
{
/* Primary and secondary colors */
	0x00, 0x00, 0x00,  0x00, 0x00, 0xA0,  0x00, 0xA0, 0x00,  0x00, 0xA0, 0xA0,  
	0xA0, 0x00, 0x00,  0xA0, 0x00, 0xA0,  0xA0, 0xA0, 0x00,  0xA0, 0xA0, 0xA0,  
	0x50, 0x50, 0x50,  0x00, 0x00, 0xF0,  0x00, 0xF0, 0x00,  0x00, 0xF0, 0xF0,  
	0xF0, 0x00, 0x00,  0xF0, 0x00, 0xF0,  0xF0, 0xF0, 0x00,  0xF0, 0xF0, 0xF0,  

/* Gray shades */
	0x00, 0x00, 0x00,  0x10, 0x10, 0x10,  0x20, 0x20, 0x20,  0x30, 0x30, 0x30,  
	0x40, 0x40, 0x40,  0x50, 0x50, 0x50,  0x60, 0x60, 0x60,  0x70, 0x70, 0x70,  
	0x80, 0x80, 0x80,  0x90, 0x90, 0x90,  0xA0, 0xA0, 0xA0,  0xB0, 0xB0, 0xB0,  
	0xC0, 0xC0, 0xC0,  0xD0, 0xD0, 0xD0,  0xE0, 0xE0, 0xE0,  0xF0, 0xF0, 0xF0,  

/* Black to red */
	0x00, 0x00, 0x00,  0x10, 0x00, 0x00,  0x20, 0x00, 0x00,  0x30, 0x00, 0x00,  
	0x40, 0x00, 0x00,  0x50, 0x00, 0x00,  0x60, 0x00, 0x00,  0x70, 0x00, 0x00,  
	0x80, 0x00, 0x00,  0x90, 0x00, 0x00,  0xA0, 0x00, 0x00,  0xB0, 0x00, 0x00,  
	0xC0, 0x00, 0x00,  0xD0, 0x00, 0x00,  0xE0, 0x00, 0x00,  0xF0, 0x00, 0x00,  

/* Black to green */
	0x00, 0x00, 0x00,  0x00, 0x10, 0x00,  0x00, 0x20, 0x00,  0x00, 0x30, 0x00,  
	0x00, 0x40, 0x00,  0x00, 0x50, 0x00,  0x00, 0x60, 0x00,  0x00, 0x70, 0x00,  
	0x00, 0x80, 0x00,  0x00, 0x90, 0x00,  0x00, 0xA0, 0x00,  0x00, 0xB0, 0x00,  
	0x00, 0xC0, 0x00,  0x00, 0xD0, 0x00,  0x00, 0xE0, 0x00,  0x00, 0xF0, 0x00,  

/* Black to blue */
	0x00, 0x00, 0x00,  0x00, 0x00, 0x10,  0x00, 0x00, 0x20,  0x00, 0x00, 0x30,  
	0x00, 0x00, 0x40,  0x00, 0x00, 0x50,  0x00, 0x00, 0x60,  0x00, 0x00, 0x70,  
	0x00, 0x00, 0x80,  0x00, 0x00, 0x90,  0x00, 0x00, 0xA0,  0x00, 0x00, 0xB0,  
	0x00, 0x00, 0xC0,  0x00, 0x00, 0xD0,  0x00, 0x00, 0xE0,  0x00, 0x00, 0xF0,

/* Blue to cyan (blue and green) */
	0x00, 0x00, 0xF0,  0x00, 0x10, 0xF0,  0x00, 0x20, 0xF0,  0x00, 0x30, 0xF0,  
	0x00, 0x40, 0xF0,  0x00, 0x50, 0xF0,  0x00, 0x60, 0xF0,  0x00, 0x70, 0xF0,  
	0x00, 0x80, 0xF0,  0x00, 0x90, 0xF0,  0x00, 0xA0, 0xF0,  0x00, 0xB0, 0xF0,  
	0x00, 0xC0, 0xF0,  0x00, 0xD0, 0xF0,  0x00, 0xE0, 0xF0,  0x00, 0xF0, 0xF0,

/* Cyan (blue and green) to green */
	0x00, 0xF0, 0xF0,  0x00, 0xF0, 0xE0,  0x00, 0xF0, 0xD0,  0x00, 0xF0, 0xC0,  
	0x00, 0xF0, 0xB0,  0x00, 0xF0, 0xA0,  0x00, 0xF0, 0x90,  0x00, 0xF0, 0x80,  
	0x00, 0xF0, 0x70,  0x00, 0xF0, 0x60,  0x00, 0xF0, 0x50,  0x00, 0xF0, 0x40,  
	0x00, 0xF0, 0x30,  0x00, 0xF0, 0x20,  0x00, 0xF0, 0x10,  0x00, 0xF0, 0x00,  

/* Green to yellow (red and green) */
	0x00, 0xF0, 0x00,  0x10, 0xF0, 0x00,  0x20, 0xF0, 0x00,  0x30, 0xF0, 0x00,  
	0x40, 0xF0, 0x00,  0x50, 0xF0, 0x00,  0x60, 0xF0, 0x00,  0x70, 0xF0, 0x00,  
	0x80, 0xF0, 0x00,  0x90, 0xF0, 0x00,  0xA0, 0xF0, 0x00,  0xB0, 0xF0, 0x00,  
	0xC0, 0xF0, 0x00,  0xD0, 0xF0, 0x00,  0xE0, 0xF0, 0x00,  0xF0, 0xF0, 0x00,  

/* Yellow (red and green) to red */
	0xF0, 0xF0, 0x00,  0xF0, 0xE0, 0x00,  0xF0, 0xD0, 0x00,  0xF0, 0xC0, 0x00,  
	0xF0, 0xB0, 0x00,  0xF0, 0xA0, 0x00,  0xF0, 0x90, 0x00,  0xF0, 0x80, 0x00,  
	0xF0, 0x70, 0x00,  0xF0, 0x60, 0x00,  0xF0, 0x50, 0x00,  0xF0, 0x40, 0x00,  
	0xF0, 0x30, 0x00,  0xF0, 0x20, 0x00,  0xF0, 0x10, 0x00,  0xF0, 0x00, 0x00,  

/* Red to magenta (blue and red) */
	0xF0, 0x00, 0x00,  0xF0, 0x00, 0x10,  0xF0, 0x00, 0x20,  0xF0, 0x00, 0x30,  
	0xF0, 0x00, 0x40,  0xF0, 0x00, 0x50,  0xF0, 0x00, 0x60,  0xF0, 0x00, 0x70,  
	0xF0, 0x00, 0x80,  0xF0, 0x00, 0x90,  0xF0, 0x00, 0xA0,  0xF0, 0x00, 0xB0,  
	0xF0, 0x00, 0xC0,  0xF0, 0x00, 0xD0,  0xF0, 0x00, 0xE0,  0xF0, 0x00, 0xF0,  

/* Magenta (blue and red) to blue */
	0xF0, 0x00, 0xF0,  0xE0, 0x00, 0xF0,  0xD0, 0x00, 0xF0,  0xC0, 0x00, 0xF0,  
	0xB0, 0x00, 0xF0,  0xA0, 0x00, 0xF0,  0x90, 0x00, 0xF0,  0x80, 0x00, 0xF0,  
	0x70, 0x00, 0xF0,  0x60, 0x00, 0xF0,  0x50, 0x00, 0xF0,  0x40, 0x00, 0xF0,  
	0x30, 0x00, 0xF0,  0x20, 0x00, 0xF0,  0x10, 0x00, 0xF0,  0x00, 0x00, 0xF0,

/* Black to magenta (blue and red) */
	0x00, 0x00, 0x00,  0x10, 0x00, 0x10,  0x20, 0x00, 0x20,  0x30, 0x00, 0x30,  
	0x40, 0x00, 0x40,  0x50, 0x00, 0x50,  0x60, 0x00, 0x60,  0x70, 0x00, 0x70,  
	0x80, 0x00, 0x80,  0x90, 0x00, 0x90,  0xA0, 0x00, 0xA0,  0xB0, 0x00, 0xB0,  
	0xC0, 0x00, 0xC0,  0xD0, 0x00, 0xD0,  0xE0, 0x00, 0xE0,  0xF0, 0x00, 0xF0,  

/* Black to cyan (blue and green) */
	0x00, 0x00, 0x00,  0x00, 0x10, 0x10,  0x00, 0x20, 0x20,  0x00, 0x30, 0x30,  
	0x00, 0x40, 0x40,  0x00, 0x50, 0x50,  0x00, 0x60, 0x60,  0x00, 0x70, 0x70,  
	0x00, 0x80, 0x80,  0x00, 0x90, 0x90,  0x00, 0xA0, 0xA0,  0x00, 0xB0, 0xB0,  
	0x00, 0xC0, 0xC0,  0x00, 0xD0, 0xD0,  0x00, 0xE0, 0xE0,  0x00, 0xF0, 0xF0,  

/* Red to white */
	0xF0, 0x00, 0x00,  0xF0, 0x10, 0x10,  0xF0, 0x20, 0x20,  0xF0, 0x30, 0x30,  
	0xF0, 0x40, 0x40,  0xF0, 0x50, 0x50,  0xF0, 0x60, 0x60,  0xF0, 0x70, 0x70,  
	0xF0, 0x80, 0x80,  0xF0, 0x90, 0x90,  0xF0, 0xA0, 0xA0,  0xF0, 0xB0, 0xB0,  
	0xF0, 0xC0, 0xC0,  0xF0, 0xD0, 0xD0,  0xF0, 0xE0, 0xE0,  0xF0, 0xF0, 0xF0,  

/* Green to white */
	0x00, 0xF0, 0x00,  0x10, 0xF0, 0x10,  0x20, 0xF0, 0x20,  0x30, 0xF0, 0x30,  
	0x40, 0xF0, 0x40,  0x50, 0xF0, 0x50,  0x60, 0xF0, 0x60,  0x70, 0xF0, 0x70,  
	0x80, 0xF0, 0x80,  0x90, 0xF0, 0x90,  0xA0, 0xF0, 0xA0,  0xB0, 0xF0, 0xB0,  
	0xC0, 0xF0, 0xC0,  0xD0, 0xF0, 0xD0,  0xE0, 0xF0, 0xE0,  0xF0, 0xF0, 0xF0,  

/* Blue to white */
	0x00, 0x00, 0xF0,  0x10, 0x10, 0xF0,  0x20, 0x20, 0xF0,  0x30, 0x30, 0xF0,  
	0x40, 0x40, 0xF0,  0x50, 0x50, 0xF0,  0x60, 0x60, 0xF0,  0x70, 0x70, 0xF0,  
	0x80, 0x80, 0xF0,  0x90, 0x90, 0xF0,  0xA0, 0xA0, 0xF0,  0xB0, 0xB0, 0xF0,  
	0xC0, 0xC0, 0xF0,  0xD0, 0xD0, 0xF0,  0xE0, 0xE0, 0xF0,  0xF0, 0xF0, 0xF0
};
//*************************************

/**************************************
        LUT TABLE PIC 256 Colors
BMP hs not RGB but BGR value, in function
S1D13705_LUT_256_BMP these values are
automaticly being switch
***************************************/
flash unsigned char LUT_256_BMP[256*3] =
{
	0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x80, 0x00, 0x00, 0x80, 0x80, 
	0x80, 0x00, 0x00, 0x80, 0x00, 0x80, 0x80, 0x80, 0x00, 0xC0, 0xC0, 0xC0, 
	0xC0, 0xDC, 0xC0, 0xF0, 0xCA, 0xA6, 0x00, 0x20, 0x40, 0x00, 0x20, 0x60, 
	0x00, 0x20, 0x80, 0x00, 0x20, 0xA0, 0x00, 0x20, 0xC0, 0x00, 0x20, 0xE0, 
	
	0x00, 0x40, 0x00, 0x00, 0x40, 0x20, 0x00, 0x40, 0x40, 0x00, 0x40, 0x60, 
	0x00, 0x40, 0x80, 0x00, 0x40, 0xA0, 0x00, 0x40, 0xC0, 0x00, 0x40, 0xE0, 
	0x00, 0x60, 0x00, 0x00, 0x60, 0x20, 0x00, 0x60, 0x40, 0x00, 0x60, 0x60, 
	0x00, 0x60, 0x80, 0x00, 0x60, 0xA0, 0x00, 0x60, 0xC0, 0x00, 0x60, 0xE0, 

	0x00, 0x80, 0x00, 0x00, 0x80, 0x20, 0x00, 0x80, 0x40, 0x00, 0x80, 0x60, 
	0x00, 0x80, 0x80, 0x00, 0x80, 0xA0, 0x00, 0x80, 0xC0, 0x00, 0x80, 0xE0, 
	0x00, 0xA0, 0x00, 0x00, 0xA0, 0x20, 0x00, 0xA0, 0x40, 0x00, 0xA0, 0x60, 
	0x00, 0xA0, 0x80, 0x00, 0xA0, 0xA0, 0x00, 0xA0, 0xC0, 0x00, 0xA0, 0xE0, 

	0x00, 0xC0, 0x00, 0x00, 0xC0, 0x20, 0x00, 0xC0, 0x40, 0x00, 0xC0, 0x60, 
	0x00, 0xC0, 0x80, 0x00, 0xC0, 0xA0, 0x00, 0xC0, 0xC0, 0x00, 0xC0, 0xE0, 
	0x00, 0xE0, 0x00, 0x00, 0xE0, 0x20, 0x00, 0xE0, 0x40, 0x00, 0xE0, 0x60, 
	0x00, 0xE0, 0x80, 0x00, 0xE0, 0xA0, 0x00, 0xE0, 0xC0, 0x00, 0xE0, 0xE0, 

	0x40, 0x00, 0x00, 0x40, 0x00, 0x20, 0x40, 0x00, 0x40, 0x40, 0x00, 0x60, 
	0x40, 0x00, 0x80, 0x40, 0x00, 0xA0, 0x40, 0x00, 0xC0, 0x40, 0x00, 0xE0, 
	0x40, 0x20, 0x00, 0x40, 0x20, 0x20, 0x40, 0x20, 0x40, 0x40, 0x20, 0x60, 
	0x40, 0x20, 0x80, 0x40, 0x20, 0xA0, 0x40, 0x20, 0xC0, 0x40, 0x20, 0xE0, 

	0x40, 0x40, 0x00, 0x40, 0x40, 0x20, 0x40, 0x40, 0x40, 0x40, 0x40, 0x60, 
	0x40, 0x40, 0x80, 0x40, 0x40, 0xA0, 0x40, 0x40, 0xC0, 0x40, 0x40, 0xE0, 
	0x40, 0x60, 0x00, 0x40, 0x60, 0x20, 0x40, 0x60, 0x40, 0x40, 0x60, 0x60, 
	0x40, 0x60, 0x80, 0x40, 0x60, 0xA0, 0x40, 0x60, 0xC0, 0x40, 0x60, 0xE0, 
	
	0x40, 0x80, 0x00, 0x40, 0x80, 0x20, 0x40, 0x80, 0x40, 0x40, 0x80, 0x60, 
	0x40, 0x80, 0x80, 0x40, 0x80, 0xA0, 0x40, 0x80, 0xC0, 0x40, 0x80, 0xE0, 
	0x40, 0xA0, 0x00, 0x40, 0xA0, 0x20, 0x40, 0xA0, 0x40, 0x40, 0xA0, 0x60, 
	0x40, 0xA0, 0x80, 0x40, 0xA0, 0xA0, 0x40, 0xA0, 0xC0, 0x40, 0xA0, 0xE0, 
	
	0x40, 0xC0, 0x00, 0x40, 0xC0, 0x20, 0x40, 0xC0, 0x40, 0x40, 0xC0, 0x60, 
	0x40, 0xC0, 0x80, 0x40, 0xC0, 0xA0, 0x40, 0xC0, 0xC0, 0x40, 0xC0, 0xE0, 
	0x40, 0xE0, 0x00, 0x40, 0xE0, 0x20, 0x40, 0xE0, 0x40, 0x40, 0xE0, 0x60, 
	0x40, 0xE0, 0x80, 0x40, 0xE0, 0xA0, 0x40, 0xE0, 0xC0, 0x40, 0xE0, 0xE0, 
	
	0x80, 0x00, 0x00, 0x80, 0x00, 0x20, 0x80, 0x00, 0x40, 0x80, 0x00, 0x60, 
	0x80, 0x00, 0x80, 0x80, 0x00, 0xA0, 0x80, 0x00, 0xC0, 0x80, 0x00, 0xE0, 
	0x80, 0x20, 0x00, 0x80, 0x20, 0x20, 0x80, 0x20, 0x40, 0x80, 0x20, 0x60, 
	0x80, 0x20, 0x80, 0x80, 0x20, 0xA0, 0x80, 0x20, 0xC0, 0x80, 0x20, 0xE0, 
	
	0x80, 0x40, 0x00, 0x80, 0x40, 0x20, 0x80, 0x40, 0x40, 0x80, 0x40, 0x60, 
	0x80, 0x40, 0x80, 0x80, 0x40, 0xA0, 0x80, 0x40, 0xC0, 0x80, 0x40, 0xE0, 
	0x80, 0x60, 0x00, 0x80, 0x60, 0x20, 0x80, 0x60, 0x40, 0x80, 0x60, 0x60, 
	0x80, 0x60, 0x80, 0x80, 0x60, 0xA0, 0x80, 0x60, 0xC0, 0x80, 0x60, 0xE0, 
	
	0x80, 0x80, 0x00, 0x80, 0x80, 0x20, 0x80, 0x80, 0x40, 0x80, 0x80, 0x60, 
	0x80, 0x80, 0x80, 0x80, 0x80, 0xA0, 0x80, 0x80, 0xC0, 0x80, 0x80, 0xE0, 
	0x80, 0xA0, 0x00, 0x80, 0xA0, 0x20, 0x80, 0xA0, 0x40, 0x80, 0xA0, 0x60, 
	0x80, 0xA0, 0x80, 0x80, 0xA0, 0xA0, 0x80, 0xA0, 0xC0, 0x80, 0xA0, 0xE0, 

	0x80, 0xC0, 0x00, 0x80, 0xC0, 0x20, 0x80, 0xC0, 0x40, 0x80, 0xC0, 0x60, 
	0x80, 0xC0, 0x80, 0x80, 0xC0, 0xA0, 0x80, 0xC0, 0xC0, 0x80, 0xC0, 0xE0, 
	0x80, 0xE0, 0x00, 0x80, 0xE0, 0x20, 0x80, 0xE0, 0x40, 0x80, 0xE0, 0x60, 
	0x80, 0xE0, 0x80, 0x80, 0xE0, 0xA0, 0x80, 0xE0, 0xC0, 0x80, 0xE0, 0xE0, 

	0xC0, 0x00, 0x00, 0xC0, 0x00, 0x20, 0xC0, 0x00, 0x40, 0xC0, 0x00, 0x60, 
	0xC0, 0x00, 0x80, 0xC0, 0x00, 0xA0, 0xC0, 0x00, 0xC0, 0xC0, 0x00, 0xE0, 
	0xC0, 0x20, 0x00, 0xC0, 0x20, 0x20, 0xC0, 0x20, 0x40, 0xC0, 0x20, 0x60, 
	0xC0, 0x20, 0x80, 0xC0, 0x20, 0xA0, 0xC0, 0x20, 0xC0, 0xC0, 0x20, 0xE0, 

	0xC0, 0x40, 0x00, 0xC0, 0x40, 0x20, 0xC0, 0x40, 0x40, 0xC0, 0x40, 0x60, 
	0xC0, 0x40, 0x80, 0xC0, 0x40, 0xA0, 0xC0, 0x40, 0xC0, 0xC0, 0x40, 0xE0, 
	0xC0, 0x60, 0x00, 0xC0, 0x60, 0x20, 0xC0, 0x60, 0x40, 0xC0, 0x60, 0x60, 
	0xC0, 0x60, 0x80, 0xC0, 0x60, 0xA0, 0xC0, 0x60, 0xC0, 0xC0, 0x60, 0xE0, 

	0xC0, 0x80, 0x00, 0xC0, 0x80, 0x20, 0xC0, 0x80, 0x40, 0xC0, 0x80, 0x60, 
	0xC0, 0x80, 0x80, 0xC0, 0x80, 0xA0, 0xC0, 0x80, 0xC0, 0xC0, 0x80, 0xE0, 
	0xC0, 0xA0, 0x00, 0xC0, 0xA0, 0x20, 0xC0, 0xA0, 0x40, 0xC0, 0xA0, 0x60, 
	0xC0, 0xA0, 0x80, 0xC0, 0xA0, 0xA0, 0xC0, 0xA0, 0xC0, 0xC0, 0xA0, 0xE0, 

	0xC0, 0xC0, 0x00, 0xC0, 0xC0, 0x20, 0xC0, 0xC0, 0x40, 0xC0, 0xC0, 0x60, 
	0xC0, 0xC0, 0x80, 0xC0, 0xC0, 0xA0, 0xF0, 0xFB, 0xFF, 0xA4, 0xA0, 0xA0, 
	0x80, 0x80, 0x80, 0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 
	0xFF, 0x00, 0x00, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF, 0xFF 
};
//*************************************

/**************************************
        LUT TABEL PIC 16 Colors  
BMP has not RGB value but BGR in function:
S1D13705_LUT_16_BMP, the red and blue
are switch automaticly!
**************************************/
flash unsigned char LUT_16_BMP[16*3] =
{    
	0x00, 0x00, 0x00,  0x00, 0x00, 0x80,   0x00, 0x80, 0x00,  0x00, 0x80, 0x80, 
	0x80, 0x00, 0x00,  0x80, 0x00, 0x80,   0x80, 0x80, 0x00,  0x80, 0x80, 0x80, 
	0xC0, 0xC0, 0xC0,  0x00, 0x00, 0xF0,   0x00, 0xF0, 0x00,  0x00, 0xF0, 0xF0, 
	0xF0, 0x00, 0x00,  0xF0, 0x00, 0xF0,   0xF0, 0xF0, 0x00,  0xF0, 0xF0, 0xF0
};
//*************************************
                                        
/**************************************
Functie: Activeer LUT 16 colors
**************************************/
void S1D13705_LUT_16(void)
{
        flash unsigned char *pLUT;
        unsigned char i;
        
        //Stel LUT tabel informatie in
        S1D13705_Write_Register(0x15,0);                                //Stel eerst LUT adres register in
        
        pLUT = LUT_16;                                                  //pLUT wijst naar de LUT array 
        for(i=0;i<16;i++)
        {
                S1D13705_Write_Register(0x17,*pLUT);                    //Zet rood
                pLUT++;        
                S1D13705_Write_Register(0x17,*pLUT);                    //Zet groen
                pLUT++;        
                S1D13705_Write_Register(0x17,*pLUT);                    //Zet blauw
                pLUT++;                                                   
        }
        
        return;
}
//*************************************

/**************************************
Functie: Activeer LUT 256 colors
**************************************/
void S1D13705_LUT_256(void)
{
        flash unsigned char *pLUT;
        unsigned int i;
        
        //Stel LUT tabel informatie in
        S1D13705_Write_Register(0x15,0);                                //Stel eerst LUT adres register in
        
        pLUT = LUT_256;                                                 //pLUT wijst naar de LUT array 
        for(i=0;i<256;i++)
        {
                S1D13705_Write_Register(0x17,*pLUT);                    //Zet rood
                pLUT++;        
                S1D13705_Write_Register(0x17,*pLUT);                    //Zet groen
                pLUT++;        
                S1D13705_Write_Register(0x17,*pLUT);                    //Zet blauw
                pLUT++;                                                   
        }
        
        return;
}
//*************************************

/**************************************
Functie: Write LUT table 16 colors for
         BMP. LUT table BMP begins with
         the blue color and not red!
         LUT BMP is BGR, S1D13705 LUT is RGB!
**************************************/
void S1D13705_LUT_16_BMP(void)
{
        flash unsigned char *pLUT;
        unsigned char red;
        unsigned char green;
        unsigned char blue;
        unsigned char i;
        
        //Set first the LUT adress at 0
        S1D13705_Write_Register(0x15,0);                                
        
        pLUT = LUT_16_BMP;                                              //pLUT points to LUT_16_BMP
        
        //Write LUT values to LUT register        
        for(i=0;i<16;i++)
        {
                //Read color values from LUT table
                blue = *pLUT++;                                         //Read the blue value from LUT table
                green = *pLUT++;                                        //Read the green value from LUT table
                red = *pLUT++;                                          //Read the red value from LUT table
                
                //Write color values to LUT register S1D13705
                S1D13705_Write_Register(0x17,red);                      //Write red value
                S1D13705_Write_Register(0x17,green);                    //Write green value
                S1D13705_Write_Register(0x17,blue);                     //Write blue value
        }
        
        return;
}
//*************************************

/**************************************
Functie: Write LUT tabel 256 colors for
         BMP, LUT table BMP begins with
         the blue color and not red!
         LUT BMP is BGR, S1D13705 LUT is RGB!
**************************************/
void S1D13705_LUT_256_BMP(void)
{
        flash unsigned char *pLUT;
        unsigned char red;
        unsigned char green;
        unsigned char blue;
        unsigned int i;
        
        //Set first the LUT adress at 0
        S1D13705_Write_Register(0x15,0);                                
        
        pLUT = LUT_256_BMP;                                             //pLUT points to LUT_16_BMP
        
        //Schrijf LUT waardes, let erop
        for(i=0;i<256;i++)
        {
                //Read color values from LUT table
                blue = *pLUT++;                                         //Read the blue value from LUT table                                    
                green = *pLUT++;                                        //Read the green value from LUT table
                red = *pLUT++;                                          //Read the red value from LUT table
                
                //Write color values to LUT register S1D13705
                S1D13705_Write_Register(0x17,red);                      //Write red value  
                S1D13705_Write_Register(0x17,green);                    //Write green value
                S1D13705_Write_Register(0x17,blue);                     //Write blue value  
        }
        
        return;
}
//************************************* 
        
/*********************************************************
                COMPACT FLASH DRIVER

Versie informatie:
----------------------------------------------------------
Versie 0.1:
- Beta versie.

----------------------------------------------------------

*********************************************************/

//Include DEFS voor CF


//**************************************
      

//CF DEFS
//Eerst de poorten, dit hoef dan alleen maar verander te worden
#define CF_WriteData    PORTA
#define CF_ReadData     PINA

#define CF_DataPort_Out DDRA = 0xFF
#define CF_DataPort_In  DDRA = 0x00  

#define CF_CE1          PORTB.0                         
#define CF_A2           PORTC.0
#define CF_A1           PORTC.1
#define CF_A0           PORTC.2

#define CF_WR           PORTD.6                 
#define CF_RD           PORTD.7                                

//Besturing CF
#define CF_ON           CF_CE1 = 0  
#define CF_OFF          CF_CE1 = 1
#define CF_Read_ON      CF_RD = 0
#define CF_Read_OFF     CF_RD = 1
#define CF_Write_ON     CF_WR = 0
#define CF_Write_OFF    CF_WR = 1

//Offset 
#define CF_OffSet_ReadData              (CF_A2 = 0) | (CF_A1 = 0) | (CF_A0 = 0)
#define CF_OffSet_WriteData             (CF_A2 = 0) | (CF_A1 = 0) | (CF_A0 = 0)
#define CF_OffSet_Error                 (CF_A2 = 0) | (CF_A1 = 0) | (CF_A0 = 1)
#define CF_OffSet_Features              (CF_A2 = 0) | (CF_A1 = 0) | (CF_A0 = 1)
#define CF_OffSet_Sector_CNT            (CF_A2 = 0) | (CF_A1 = 1) | (CF_A0 = 0)
#define CF_OffSet_Secotor_NO            (CF_A2 = 0) | (CF_A1 = 1) | (CF_A0 = 1)
#define CF_OffSet_Cylinder_Low          (CF_A2 = 1) | (CF_A1 = 0) | (CF_A0 = 0)     
#define CF_OffSet_Cylinder_High         (CF_A2 = 1) | (CF_A1 = 0) | (CF_A0 = 1)
#define CF_OffSet_Head                  (CF_A2 = 1) | (CF_A1 = 1) | (CF_A0 = 0)       
#define CF_OffSet_Status                (CF_A2 = 1) | (CF_A1 = 1) | (CF_A0 = 1)
#define CF_OffSet_Command               (CF_A2 = 1) | (CF_A1 = 1) | (CF_A0 = 1)
         
//CF-ATA commando's         
#define CF_CMD_Erase_Sector     0xC0
#define CF_CMD_ID               0xEC
#define CF_CMD_Read_Sector      0x20
#define CF_CMD_Write_Sector     0x30
//**************************************

/**************************************
CF INIT
I/O poorten instellen van de CF
**************************************/
void CF_Init(void)
{
        //Zet CF, lezen en schrijven uit
        CF_OFF;                                         
        CF_Read_OFF;
        CF_Write_OFF;                   
        
        //Zet CF offset op 0
        CF_OffSet_ReadData;
        
        return;
}
//*************************************

/**************************************
CF Wait Ready
lees of CF is ready voor actie
**************************************/
void CF_Wait_RDY(void)
{
        unsigned char CF_Register;
        
        //Lees de register van de CF en check of
        //de ready bit is set
        while(1)
        {
               // Check_Data_Ready();
                
                CF_Register = CF_Read_Register(7);      //Offset 7
                CF_Register = CF_Register & 0x40;       //Check alleen RDY Bit
                
                if(CF_Register == 0x40)                 //Als RDY bit is 1 dan verlaat loop
                        break;
        }  

        return;
}
//************************************* 

/**************************************
CF Wait Data Request
wacht totdat de DRQ bit is set
**************************************/
void CF_Wait_DRQ(void)
{
        unsigned char CF_Register;
        
        //Lees de register van de CF en
        //check of de DRQ bit is set
        while(1)
        {
         //       Check_Data_Ready();
               
                CF_Register = CF_Read_Register(7);      //Offset 7
                CF_Register = CF_Register & 0x08;       //Check alleen DRQ Bit
                
                if(CF_Register == 0x08)                 //Als DRQ bit is 1, verlaat loop
                        break;
        }
        
        return;
}
//*************************************

/**************************************
CF Error register
lees de error register van de CF
**************************************/
unsigned char CF_Error_Reg(void)
{
        unsigned char CF_Register;
        
        //Lees de error register van de CF
        CF_Register = CF_Read_Register(1);              //Offset 1
        
        return (CF_Register);
}
//*************************************

/**************************************
CF Write registers
schrijf naar de register van de CF
**************************************/
void CF_Write_Register(unsigned char Reg_Adres, unsigned char Reg_Value)
{
        CF_DataPort_Out;                                //PORTx instellen als output
        CF_Write_OFF;                                   //Zet CF schrijven uit
        
        
        //Bepaal de offset instelling
        switch(Reg_Adres)
        {
                case 0:
                        CF_OffSet_WriteData;            //Offset voor schrijven van data
                        break;
                case 1:
                        CF_OffSet_Features;             //Offset voor schrijven van features
                        break;
                case 2:
                        CF_OffSet_Sector_CNT;           //Offset voor schrijven van de sector teller 
                        break;
                case 3:
                        CF_OffSet_Secotor_NO;           //Offset voor schrijven van de sector nummer
                        break;
                case 4:
                        CF_OffSet_Cylinder_Low;         //Offset voor schrijven van de cylinder laag
                        break;
                case 5:
                        CF_OffSet_Cylinder_High;        //Offset voor schrijven van de cylinder hoog
                        break;
                case 6:
                        CF_OffSet_Head;                 //Offset voor schrijven van de head of select card
                        break;
                case 7:
                        CF_OffSet_Command;              //Offset voor schrijven van de commando
                        break;                                                                 
        }
                
        CF_WriteData = Reg_Value;                       //Zet register waarde op de data port 
        CF_Write_ON;                                    //Zet CF schrijven aan
        
        #asm("nop");                                    //Wacht 4 klokcycles
        #asm("nop");
        #asm("nop");
        #asm("nop");                                                        
        
        CF_Write_OFF;                                   //Zet CF schrijven uit
        

        return;
}
//*************************************

/**************************************
CF Read registers
lees de waarde van een register van de CF
**************************************/
unsigned char CF_Read_Register(unsigned char Reg_Adres)
{
        unsigned char CF_Data;
        
        CF_DataPort_In;                                 //PORTx instellen als input
        CF_Write_OFF;                                   //CF schrijven uit
        CF_Read_OFF;                                    //CF lezen uit
        
        //Bepaal de offset instelling
        switch(Reg_Adres)
        {
                case 0:
                        CF_OffSet_ReadData;             //Offset voor lezen van data
                        break;
                case 1:
                        CF_OffSet_Error;                //Offset voor lezen van errors
                        break;
                case 2:
                        CF_OffSet_Sector_CNT;           //Offset voor lezen van de sector teller 
                        break;
                case 3:
                        CF_OffSet_Secotor_NO;           //Offset voor lezen van de sector nummer
                        break;
                case 4:
                        CF_OffSet_Cylinder_Low;         //Offset voor lezen van de cylinder laag
                        break;
                case 5:
                        CF_OffSet_Cylinder_High;        //Offset voor lezen van de cylinder hoog
                        break;
                case 6:
                        CF_OffSet_Head;                 //Offset voor lezen van de head of select card
                        break;
                case 7:
                        CF_OffSet_Status;               //Offset voor lezen van de status
                        break;                                                                 
        }

        #asm("nop");                                    //Wacht 1 klokcycle
        
        CF_Read_ON;                                     //CF lezen aan
        
        //delay_us(2);                                    //Wacht even voordat data wordt gelezen
        #asm("nop");                                    //Wacht 1 klokcycle
        #asm("nop");                                    //Wacht 1 klokcycle
        #asm("nop");                                    //Wacht 1 klokcycle
        
        CF_Data =  PINA; //CF_ReadData;                         //Lees de data van de CF
        CF_Read_OFF;                                    //CF lezen uit

        return(CF_Data);
}
//*************************************  


/**************************************
CF Read Sector
lees een sector van de CF
**************************************/
//void CF_Read_Sector(unsigned char Sector_Count, unsigned char Sector_No, unsigned char Cylinder_Low, unsigned char Cylinder_High, unsigned char Head)
void CF_Read_Sector(unsigned long int Sector)
{
        unsigned int Bytes;
        unsigned char Sector_Addr[3];
        
        CF_ON;                                          //Zet CF aan
        
        CF_Wait_RDY();                                  //Wacht op CF tot RDY is set
        
        //Bereken de sector die gelezen moet worden
        Sector_Addr[0] = Sector & 0x000000FF;
        Sector_Addr[1] = ((Sector & 0x0000FF00) >> 8);
        Sector_Addr[2] = ((Sector & 0x00FF0000) >> 16); 
        
        //Zend Sector Count, offset = 2
        CF_Write_Register(2,0x01);
        
        //Zend Sector No, offset = 3
        //CF_Write_Register(3,Sector_No);
        CF_Write_Register(3,Sector_Addr[0]);

        //Zend Cylinder Low, offset = 4
        //CF_Write_Register(4,Cylinder_Low);
        CF_Write_Register(4,Sector_Addr[1]);
        
        //Zend Cylinder High, offset = 5 
        //CF_Write_Register(5,Cylinder_High);
        CF_Write_Register(5,Sector_Addr[2]);
        
        //Zend Select Card / Head, offset = 6. Normaal 0xE0
        CF_Write_Register(6,0xE0);
        
        //Zend Commando Read Sector, offset = 7
        CF_Write_Register(7,CF_CMD_Read_Sector);
        
        CF_Wait_DRQ();                                  //Wacht op DRQ bit is set
        
        CF_OffSet_ReadData;                             //Offset op 0, lezen data
        CF_DataPort_In;                                 //PORTx als input
       
        //Inlezen van de sector, dat is 512bytes
        for(Bytes=0; Bytes<512; Bytes++)
        {
                CF_Read_OFF;                            //CF lezen uit
             
                #asm("nop");                            //2x NOP
             //   #asm("nop");                            
                
                CF_Read_ON;                             //CF lezen aan  
                
                #asm("nop");                            //3x NOP
            //    #asm("nop");                            
            //    #asm("nop");                            
                
                CF_Sector_Data[Bytes] = CF_ReadData;    //Lees data van CF en sla op in buffer
                CF_Read_OFF;                            //CF lezen uit
        }
        
        //Lezen voltooid, zet CF uit
        CF_OFF;
        
        return;
}
//*************************************

/**************************************
CF Write Sector
schrijven van een sector naar de CF
**************************************/
//void CF_Write_Sector(unsigned char Sector_Count, unsigned char Sector_No, unsigned char Cylinder_Low, unsigned char Cylinder_High, unsigned char Head)
void CF_Write_Sector(unsigned long int Sector)
{
        unsigned int Bytes;
        unsigned char Sector_Addr[3];

        CF_ON;                                          //Zet CF aan
        
        CF_Wait_RDY();                                  //Wacht op CF tot RDY is set
        
         //Bereken de sector die gelezen moet worden
        Sector_Addr[0] = Sector & 0x000000FF;
        Sector_Addr[1] = ((Sector & 0x0000FF00) >> 8);
        Sector_Addr[2] = ((Sector & 0x00FF0000) >> 16); 
        
        //Zend Sector Count, offset = 2
        CF_Write_Register(2,0x01);
        
        //Zend Sector No, offset = 3
        //CF_Write_Register(3,Sector_No);
        CF_Write_Register(3,Sector_Addr[0]);

        //Zend Cylinder Low, offset = 4
        //CF_Write_Register(4,Cylinder_Low);
        CF_Write_Register(4,Sector_Addr[1]);
        
        //Zend Cylinder High, offset = 5 
        //CF_Write_Register(5,Cylinder_High);
        CF_Write_Register(5,Sector_Addr[2]);
        
        //Zend Select Card / Head, offset = 6. Normaal 0xE0
        CF_Write_Register(6,0xE0);
        
        //Zend Commando Write Sector, offset = 7
        CF_Write_Register(7,CF_CMD_Write_Sector);
        
        CF_Wait_DRQ();                                  //Wacht op DRQ bit is set
        
        CF_OffSet_WriteData;                            //Offset op 0, schrijven van data
        CF_DataPort_Out;                                //PORTx als output
        
        //Schrijf sector, 512bytes naar de CF 
        for(Bytes=0; Bytes<512; Bytes++)
        {
                CF_Write_OFF;                           //CF schrijven uit
                
                #asm("nop");                            //2x nop
                #asm("nop");            
                
                CF_WriteData = CF_Sector_Data[Bytes];   //Zet de te schrijven data klaar voor de CF
                CF_Write_ON;                            //CF schrijven aan
                
                #asm("nop");                            //3x nop
                #asm("nop"); 
                #asm("nop"); 
                
                CF_Write_OFF;                           //CF schrijven uit
                
        }
        
        //Schrijven voltooid, zet CF uit
        CF_OFF;                                 
        
        return;
}
//*************************************

/**************************************
CF Info
lees informatie over de aangesloten CF
**************************************/
void CF_Info(void)
{
        unsigned int Bytes;
                   
        CF_ON;                                          //Zet CF aan
        
        CF_Wait_RDY();                                  //Wacht op CF tot RDY is set

        //Zend 0 commande voor Head, offset = 6
        CF_Write_Register(6,0);
        
        //Zend Commando ID device, offset = 7
        CF_Write_Register(7,CF_CMD_ID);
         
        CF_Wait_DRQ();                                  //Wacht op DRQ bit is set
        
        CF_OffSet_ReadData;                             //Offset op 0, lezen data
        CF_DataPort_In;                                 //PORTx als input
       
        //Inlezen van de info, dat is 512bytes
        for(Bytes=0; Bytes<512; Bytes++)
        {
                CF_Read_OFF;                            //CF lezen uit
                
                #asm("nop");                            //2x NOP
                #asm("nop");                            
                
                CF_Read_ON;                             //CF lezen aan  
                
                #asm("nop");                            //3x NOP
                #asm("nop");                            
                #asm("nop");                            
                
                CF_Sector_Data[Bytes] = CF_ReadData;    //Lees data van CF en sla op in buffer
                CF_Read_OFF;                            //CF lezen uit
        }
        
        //Lezen info voltooid, zet CF uit
        CF_OFF;
        
        return;
}
//*************************************
        
        
                                                        
        
/**************************************
             CF functions
**************************************/

/*************************************
        Include header file's                   
*************************************/
#include "LCD_Test_Defs.h"
#include "S1D13705.h"          
//*************************************     

/**************************************
Function: Read info CF and put on LCD
**************************************/
void CF_Read_Info(void)
{
        unsigned int i;
        unsigned int test[5];
        unsigned char d;   

        S1D13705_Clear_LCD();                                                   //Clear the LCD
        
        CF_Info();                                                              //Read info from CF
        
        sprintf(string,"Signature CF: %x%x",CF_Sector_Data[1],CF_Sector_Data[0]);
        S1D13705_Text_LCD(0,2,string,0);                       
        
        test[1] = CF_Sector_Data[3];
        test[2] = (test[1] << 8) + CF_Sector_Data[2];
        
        sprintf(string,"Cylinders: %d",test[2]);
        S1D13705_Text_LCD(0,3,string,0);
        
        sprintf(string,"Heads: %d",CF_Sector_Data[6]);
        S1D13705_Text_LCD(0,4,string,0);
        
        test[1] = CF_Sector_Data[13];
        test[2] = (test[1] << 8) + CF_Sector_Data[12];
        
        sprintf(string,"Number Sectors/Track: %d",test[2]);
        S1D13705_Text_LCD(0,5,string,0);
        
        test[0] = CF_Sector_Data[15];
        test[1] = (test[0] <<8) + CF_Sector_Data[14];
        test[2] = CF_Sector_Data[17];
        test[3] = (test[2] <<8) + CF_Sector_Data[16];
        
        sprintf(string,"Sectors: %x%x HEX",test[1],test[3]);
        S1D13705_Text_LCD(0,6,string,0);
        
        //Serial number
        i = 39;
        d = 40;
        sprintf(string,"SN: ");
        S1D13705_Text_LCD(0,7,string,0);
        
        while(i >19)
        {
                sprintf(string,"%c",CF_Sector_Data[i]);
                S1D13705_Text_LCD(d,7,string,0);
                i--;  
                d += 8;
        }
         
        delay_ms(1000);
        
        return;
}
//*************************************         

/**************************************
Function: Check of Blok/data info 
          aanwezig is in de CF
**************************************/
unsigned char CF_Check_Blok_Data(void)
{
        unsigned char i;            
        unsigned char Page_Present;
        unsigned long int Page;
        
        //Wis CF_Blok info
        for(i=0;i<32;i++)
                CF_Page[i] = 0xFF;
        
        Page = 0;                                                       //Zet Page
        Page_Present = 0;                                               //Aantal Pages 0
        
        for(i=0;i<32;i++)
        {
                CF_Read_Sector((152 * Page) + 1);
                
                if(CF_Sector_Data[0] == 'B' & CF_Sector_Data[1] == 'l'& CF_Sector_Data[2] == 'o' & CF_Sector_Data[3] == 'k')
                {
                        CF_Page[i] = i;                                 //Page aanwezig zet Page aanwezig in Memory
                        Page_Present++;                                 //Als Page aanwezig is met 1 verhogen 
                }       
        
                Page++;                                                 //Verhoog Page met 1 voor volgende sector
        }  
        
        
        return(Page_Present);
}               
//*************************************

/**************************************
Function: Draw data on LCD from CF
**************************************/
void CF_Draw_Pic(unsigned int X, unsigned int Y, unsigned long int Page)
{
        unsigned char b;                                                //blauwe kleur
        unsigned char g;                                                //groene kleur
        unsigned char r;                                                //rode kleur

        unsigned int i;
        unsigned int W;                                                 //Breedte plaatje
        unsigned int H;                                                 //Hoogte plaatje
        unsigned int Wx;                                                //Counter breedte
        unsigned int Byte;                                              //Byte teller van de array
        unsigned int Yx;                                                //Waarde voor scanline bytes, by 4bpp 160, by 8bpp 320

        unsigned long int Sector;                                       //Sector van CF
        unsigned long int S1D_mem;                                      //S1D13705 memory adress
        
        //S1D13705_Clear_LCD();                                   //Clear the LCD

        //Bereken de begin sector van de CF
        Sector = (152 * Page) + 1;
        
        //Lees eerste info sector in
        CF_Read_Sector(Sector++);
        
         //Lees de hoogte en de breedte van de Pic
        W = CF_Sector_Data[6];                                          //Lees lage Width
        i = CF_Sector_Data[7];                                          //Lees hoge Width
        W = W + i;                                                      //Tel ze bij elkaar op
        H = CF_Sector_Data[8];                                          //Hoogte van BMP in pixels
        
        
        //Check kleur en stel S1D13705 in
        if(CF_Sector_Data[5] == 4)                                      //16 kleuren
        {
                printf("\n\rLUT 16 config");
                S1D13705_Write_Register(0x02,0xB0);                     //Stel kleur register 2 in op 16 kleuren
                S1D13705_Write_Register(0x1C,0xA0);                     //Stel line byte counter in op 160 lines
                
                //Zend LUT tabel van BMP plaatje naar S1D13705
                S1D13705_Write_Register(0x15,0);                        //Set first the LUT adress at 0
                
                Byte = 255;                                             //Kleur begin Sector
                for(i=0;i<16;i++)
                { 
                        //Read color values from LUT table
                        b = CF_Sector_Data[Byte++];                     //Read the blue value from LUT table
                        g = CF_Sector_Data[Byte++];                     //Read the green value from LUT table
                        r = CF_Sector_Data[Byte++];                     //Read the red value from LUT table
                
                        //Write color values to LUT register S1D13705
                        S1D13705_Write_Register(0x17,r);                //Write red value
                        S1D13705_Write_Register(0x17,g);                //Write green value
                        S1D13705_Write_Register(0x17,b);                //Write blue value  
                }
                
                Sector++; 
                Yx = 160;                                               //Stel Yx in op 160 scanline bytes
               
                printf("\n\rLUT 16 config done");
        }
        
        if(CF_Sector_Data[5] == 8)                                      //256 kleuren
        {
                printf("\n\rLUT 256 config");
                S1D13705_Write_Register(0x02,0xF0);                     //Stel kleur register 2 in op 256 kleuren
                S1D13705_Write_Register(0x1C,0xFF);                     //Stel line byte counter in op 320 lines
                
                //Zend LUT tabel van BMP plaatje naar S1D13705
                S1D13705_Write_Register(0x15,0);                        //Set first the LUT adress at 0
                
                Byte = 254;                                             //Kleur begin Sector
                
                //Schrijf eerste 258 kleuren daarna CF lezen
                for(i=0;i<86;i++)
                {
                        //Read color values from LUT table
                        b = CF_Sector_Data[Byte++];                     //Read the blue value from LUT table
                        g = CF_Sector_Data[Byte++];                     //Read the green value from LUT table
                        r = CF_Sector_Data[Byte++];                     //Read the red value from LUT table
                
                        //Write color values to LUT register S1D13705
                        S1D13705_Write_Register(0x17,r);                //Write red value
                        S1D13705_Write_Register(0x17,g);                //Write green value
                        S1D13705_Write_Register(0x17,b);                //Write blue value       
                }
                
                //Lees tweede info sector in
                CF_Read_Sector(Sector++);   
                
                Byte = 0;
                //Schrijf de laatste 510 kleuren naar S1D13705
                for(i=0;i<170;i++)
                {
                        //Read color values from LUT table
                        b = CF_Sector_Data[Byte++];                     //Read the blue value from LUT table
                        g = CF_Sector_Data[Byte++];                     //Read the green value from LUT table
                        r = CF_Sector_Data[Byte++];                     //Read the red value from LUT table
                
                        //Write color values to LUT register S1D13705
                        S1D13705_Write_Register(0x17,r);                //Write red value
                        S1D13705_Write_Register(0x17,g);                //Write green value
                        S1D13705_Write_Register(0x17,b);                //Write blue value       
                }
                 
                Yx = 320;                                               //Stel scanline bytes in op 320
                
                printf("\n\rLUT 256 config done");
        }
                
                
        printf("\n\rDraw BMP data on LCD");
        
        //Zet S1D_mem op begin adress/pixel
        S1D_mem = X + (Y * Yx);
        Byte = 0;
        
        //Lees eerste bitmap data in
        CF_Read_Sector(Sector++);   
        
        while(H--)
        {
                Wx = W;
                while(Wx--)
                {
                        S1D13705_Write_Data(S1D_mem++,CF_Sector_Data[Byte++]);     //Schrijf data naar S1D13705
                        if(Byte == 512)                                            //Nieuwe data inlezen van CF
                        {
                                Byte = 0;
                                CF_Read_Sector(Sector++);   
                        }
                }
                
                //Herbereken S1D_mem voor de volgende lijn
                S1D_mem = (S1D_mem - W) + Yx;
        }                                     
       
        return;
}                                                    
//*************************************              

/**************************************
Function: Delete data on CF
**************************************/
void CF_Delete_Data(void)
{
        unsigned int i;
        unsigned long int Sector;
        unsigned long int End_Sector = 152 * 32;   
        
        //Wis CF_Sector_data
        for(i=0;i<512;i++)
                CF_Sector_Data[i] = 0x00;
                
        //Wis de blad blok data
        Sector = 1;
        while(Sector < End_Sector)
        {
                CF_Write_Sector(Sector++);
        }
        
        return;
}
//*************************************
        
                
/**************************************
                LINK
This will recieve the data and put it 
in the CF
**************************************/

/**************************************
        Include header file's                   
**************************************/
#include "LCD_Test_Defs.h" 
#include "cf_driver.h"
#include "S1D13705.h"          
//************************************* 

/**************************************
Function: Send ready sign to PC
**************************************/
void TX_Ready_Sign(void)
{
        #asm("nop");
        putchar('!');
        
        return;
}
//*************************************

/**************************************
Function: Receive bmp data and store
**************************************/
void Receive_BMP_Data(void)
{
        unsigned char data;

        unsigned int i;
        unsigned int Aantal_BMP_Sectors;                        

        unsigned long int Page;           
        unsigned long int Sector;   
        
        //Check welke blok vrij is begin daar
        Page = 0;                                               //Stel Page altijd standaard in op 0
        while(Page<32)
        {
                if(CF_Page[Page] == Page)                        //Als Page gelijk is dan Page met 1 verhogen anders loop verlaten
                        Page++;
                else
                        break;                                  //Break the loop                
        }
          
        //Bepaal start Sector
        Sector = (152 * Page)+ 1;                               //Example 152 * 2 = 304 + 1 = 305
        
        //Zet ID in CF_Sector_data
        CF_Sector_Data[0] = 'B';
        CF_Sector_Data[1] = 'l';
        CF_Sector_Data[2] = 'o';
        CF_Sector_Data[3] = 'k';
        CF_Sector_Data[4] = Page;
        
        S1D13705_Clear_LCD();                                   //Clear the LCD
        
        sprintf(string,"Waiting for Link");
        S1D13705_Text_LCD(0,0,string,0);

        data = getchar();
        if(data == 'D')  
        {
                sprintf(string,"Link is maked");
                S1D13705_Text_LCD(0,1,string,0);
                TX_Ready_Sign();
        }
               
        //Ontvang hoeveel sectors nodig voor bitmap data
        Aantal_BMP_Sectors = getchar();
        
        //Ontvang info over bmp, kleurdiepte/width/hoogte
        CF_Sector_Data[5] = getchar();                          //Kleurdiepte
        CF_Sector_Data[6] = getchar();                          //Width_l
        CF_Sector_Data[7] = getchar();                          //Width_h
        CF_Sector_Data[8] = getchar();                          //Hoogte
        
        
        //Ontvang LUT tabel
        if(CF_Sector_Data[5] == 4)                              //16 kleuren, 48 bytes ontvangen
        {
               sprintf(string,"Info recieved!Receive LUT 16");
                S1D13705_Text_LCD(0,2,string,0);
                TX_Ready_Sign();                                //Zend ready Signal
                for(i=255;i<=302;i++)                           //Sla data op in CF_Sector_data
                {
                        TX_Ready_Sign();                        //Zend ready signal;
                        CF_Sector_Data[i] = getchar();
                }
                
                //Sla CF_Sector_Data op
                CF_Write_Sector(Sector);                        //Sla Informatie Sector op in begin Sector
                Sector += 2;                                    //Verhoog Sector met 2, omdat dan de BMP data begint
        }
        if(CF_Sector_Data[5] == 8)                              //256 kleuren 768 bytes ontvangen verdelen over 2 sectoren
        {

                sprintf(string,"Info recieved!Receive LUT 256");
                S1D13705_Text_LCD(0,2,string,0);
                
                //Ontvanger eerst 256 bytes in de eerste sector
                TX_Ready_Sign();                                //Zend ready signal;
                for(i=254;i<=511;i++)
                {
                        TX_Ready_Sign();                        //Zend ready signal;
                        CF_Sector_Data[i] = getchar();
                }        
                
                //Sla eerste info sector op CF
                CF_Write_Sector(Sector);                        //Sla eerste info sector op in start sector
                Sector++;                                       //Verhoog met 1                           
                
                               
                //Ontvang tweede sector LUT
                for(i=0;i<=509;i++)
                {
                        TX_Ready_Sign();                        //Zend ready signal;
                        CF_Sector_Data[i] = getchar();
                }                        
                
                //Sla tweede info sector op CF
                CF_Write_Sector(Sector);
                Sector++;
        }
        
        sprintf(string,"LUT recieved! Get BITMAP data");
        S1D13705_Text_LCD(0,3,string,0);
        sprintf(string,"Sectors to recieve: %d at Page %d",Aantal_BMP_Sectors,Page);
        S1D13705_Text_LCD(0,4,string,0);
        
        //Ontvangen van de bitmap Data
        TX_Ready_Sign();                                        //Zend ready signal;
        
        for(data=1;data<=Aantal_BMP_Sectors;data++)
        {
                //Lees eerste 512 bytes in van de PC
                for(i=0;i<=511;i++) 
                {      
                        //TX_Ready_Sign();                        //Zend ready signal;
                        CF_Sector_Data[i] = getchar();  
                }
                        
                sprintf(string,"Sector %d stored",data);
                S1D13705_Text_LCD(0,5,string,0);
                
                //Sla bitmap data op
                CF_Write_Sector(Sector++);
                TX_Ready_Sign();                        //Zend ready signal
        }
        
        sprintf(string,"Received done");
        S1D13705_Text_LCD(0,6,string,0);
                                      
        delay_ms(1000); 

        return;
}
//*************************************       

/**************************************
      4 Wire Touchscreen driver      
      
Version:  
**************************************/        
#include "lcd_test_defs.h"
//#include "s1d13705.h"
#include "Touchscreen.h"               

static float A,B,C,D,E,F;

/**************************************
Functie: Touchscreen init
**************************************/
void Touchscreen_Init(void)
{
        //Lees calibratie coeffecients in van eeprom
        A = ee_float1;
        B = ee_float2;
        C = ee_float3;
        D = ee_float4;
        E = ee_float5;
        F = ee_float6;
        
        //Lees Flag register in van eeprom
        Touchscreen.Flag_Register = ee_Touch_Flag; 
        printf("\n\rTouch_Flag: %x",Touchscreen.Flag_Register);
        //Zet Touchscreen standby
        Touch_Standby;
        
        //Zet TouchPressed flag op 0
        Touchscreen.Flag_Register &= ~(1<<TouchPressed);
        
        //Zet int0 actief
        GIFR |= (1<<INT0);                                      //Erase bit so that no interupt is generated 
        GICR |= (1<<INT0);                                      //Enable the INT0 request

        return;
}
//*************************************

/**************************************
Functie: Check of touchscreen nog is 
         ingedrukt,debounce
**************************************/
unsigned char Touchscreen_Pressed(void)
{
        //Check of de touchscreen nog is ingedrukt
        delay_ms(10);
        if(TouchPress != 0)             
                return(0);                                      //Touchscreen is niet ingedrukt, ga terug.
        
        return(0x01);                                           //Touchscreen is nog ingedrukt return een 1
}
//*************************************
                                             
/**************************************
Functie: Lees x of y raw value van touchscreen
**************************************/
unsigned int Touchscreen_Value(unsigned char T_as)
{
        unsigned int Touch_Data[6];                             //Gemeten touch data 
        unsigned char Channel;                                  //Channel voor de ADC om te meten   
        unsigned char i;
        
        //Checke of aan X of Y moeten worden gemeten
        if(T_as == 0)                                           //X as       
        {
                Touch_Xas;                                      //Stel I/O in zodat aan de X as wordt gemeten.
                Channel = 0x01;                                 //Stel channel in om te meten aan X as
        }
        else                                                           
        {
                Touch_Yas;                                      //Stel I/O in zodat aan de Y as wordt gemeten.
                Channel = 0x00;                                 //Stel channel in om te meten aan Y as
        }
        
        //Meet 5x touchscreen data
        for(i=0;i<5;i++) 
                Touch_Data[i] = ADC_LTC1298_Data(Channel);
        
        //Neem gemiddelde van meetwaarde
        Touch_Data[5] = (Touch_Data[0] / 5) + (Touch_Data[1] / 5) + (Touch_Data[2] / 5) + (Touch_Data[3] / 5) + (Touch_Data[4] / 5);
        
        return(Touch_Data[5]);
}
//*************************************

/**************************************
Functie: Touchscreen X and Y positie
**************************************/
void Touchscreen_XY_positie(void)
{
        float Xpos, Ypos;
        unsigned int Xraw, Yraw;
        unsigned char X = 0;
        unsigned char Y = 1;    
        unsigned char i;
        
        //Check of de touchscreen nog is ingedrukt dan pas meten
        if((i=Touchscreen_Pressed()) != 1)                              //Touchscreen niet ingedrukt, return
        {
                //Zet TouchPressed flag op 0
                Touchscreen.Flag_Register &= ~(1<<TouchPressed); 
                printf("\n\rReleased");    
                return;
        }
                
        //Meet Xraw en Yraw value
        Xraw = Touchscreen_Value(X);                                    //Meet Xraw data
        Yraw = Touchscreen_Value(Y);                                    //Meet Yraw data
        
        //Bereken Xpos en Ypos
        Xpos = (A * (float)Xraw) + (B * (float)Yraw) + C;
        Ypos= (D * (float)Xraw) + (E * (float)Yraw) + F;
        
        //Sla Xpos en Ypos op in sruct als unsigned int
        Touchscreen.X_pos = (unsigned int)Xpos;
        Touchscreen.Y_pos = (unsigned int)Ypos;        
        
        //Zet TouchPressed flag op 0
        Touchscreen.Flag_Register &= ~(1<<TouchPressed);  
        
        //Zet XY_Value flag op 1 als teken dat er een geldige XY waarde aanwezig is
        Touchscreen.Flag_Register |= (1<<XY_Value);
        
        //Zet touchscreen standby voor nieuwe drukmeting
        Touch_Standby;
        
        return;
}
//*************************************
                       
/**************************************
Functie: Touchscreen calibratie
**************************************/
void Touchscreen_Calibratie(void)
{
        unsigned int Xt1,Xt2,Xt3;                                       //3 sample points X as van de touchscreen
        unsigned int Yt1,Yt2,Yt3;                                       //3 sample points Y as van de touchscreen
        unsigned int Xd1,Xd2,Xd3;                                       //3 referentie punten van de display X 
        unsigned int Yd1,Yd2,Yd3;                                       //3 referentie punten van de display Y
        unsigned char X = 0;
        unsigned char Y = 1;   
        unsigned char i;
        
        //Zet in flag register TouchCalProcess aan
        Touchscreen.Flag_Register |= (1<<TouchCalProcess);      
        
        //Stel referentie punten vast van de display
        //Meet punt 1
        Xd1 = 31;
        Yd1 = 215;
        //Xd1 = 287;
        //Yd1 = 119;
        
        //Meet punt 2
        Xd2 = 159;
        Yd2 = 23;
        
        //Meet punt 3
        Xd3 = 287;
        Yd3 = 119;
        //Xd3 = 31;
        //Yd3 = 23;
           
        //Stel display in
        S1D13705_Clear_LCD();                                           //Wis LCD scherm
        
        //Teken kruis meet Punt1
        //S1D13705_Draw_Horizontal_Line(283,291,119,0);                 //Instellen als X(begin), X1(eind), Y, kleur
        //S1D13705_Draw_Vertical_Line(287,115,123,0);                   //Instellen als X, Y(begin), Y1(eind), kleur
        S1D13705_Draw_Horizontal_Line(27,35,215,0);                     //Instellen als X(begin), X1(eind), Y, kleur
        S1D13705_Draw_Vertical_Line(31,211,219,0);                      //Instellen als X, Y(begin), Y1(eind), kleur     
                 
        do
        {
                //Wacht totdat op het touchscreen is gedrukt
                while((Touchscreen.Flag_Register & (1<<TouchPressed)) == 0);     //Als uitkomst 0 is dan is er nog niet gedrukt 
                
                //Check of de touchscreen nog is ingedrukt dan pas meten
                if((i=Touchscreen_Pressed()) == 1)                      //Als waarde 1 is dan is touchscreen nog ingedrukt ga meten
                {        
                        Xt1 = Touchscreen_Value(X);                     //Meet de X waarde
                        Yt1 = Touchscreen_Value(Y);                     //Meet de Y waarde
                }
                else
                {
                        Xt1 = 0;
                        Yt1 = 0;
                }
                
                
                //Zet touchscreen standby voor nieuwe drukmeting
                Touch_Standby;
        
                //Zet TouchPressed flag op 0
                Touchscreen.Flag_Register &= ~(1<<TouchPressed);
                
                //Wacht totdat touchscreen wordt los gelaten
                delay_ms(10);
                while(TouchPress == 0);

                GIFR |= (1<<INT0);                                      //Erase bit so that no interupt is generated 
                GICR |= (1<<INT0);                                      //Enable the INT0 request

        }
        while(Xt1 == 0 || Yt1 == 0);                                    //Als Xt1 of Yt1 0 is dan is een error bij meten dan nog een keer
                
        //Meten punt 1 klaar
        //Wis kruis meetpunt 1
        //S1D13705_Draw_Horizontal_Line(283,291,119,15);                //Stuur kleur wit om de pixels te wissen       
        //S1D13705_Draw_Vertical_Line(287,115,123,15);                                                    
        S1D13705_Draw_Horizontal_Line(27,35,215,15);                    //Stuur kleur wit om de pixels te wissen         
        S1D13705_Draw_Vertical_Line(31,211,219,15);                                         
        
        //Teken kruis meet punt 2 
        S1D13705_Draw_Horizontal_Line(155,163,23,0);                    //Instellen als X(begin), X1(eind), Y, kleur
        S1D13705_Draw_Vertical_Line(159,19,27,0);                       //Instellen als X, Y(begin), Y1(eind), kleur  
        
        do
        {
                //Wacht totdat op het touchscreen is gedrukt
                while((Touchscreen.Flag_Register & (1<<TouchPressed)) == 0);         //Als uitkomst 0 is dan is er nog niet gedrukt 

                //Check of de touchscreen nog is ingedrukt dan pas meten
                if((i=Touchscreen_Pressed()) == 1)                      //Als waarde 1 is dan is touchscreen nog ingedrukt ga meten
                {        
                        Xt2 = Touchscreen_Value(X);                     //Meet de X waarde
                        Yt2 = Touchscreen_Value(Y);                     //Meet de Y waarde
                }
                else
                {
                        Xt2 = 0;
                        Yt2 = 0;
                }
                
                //Zet touchscreen standby voor nieuwe drukmeting
                Touch_Standby;


                //Zet TouchPressed flag op 0
                Touchscreen.Flag_Register &= ~(1<<TouchPressed);
                
                //Wacht totdat touchscreen wordt los gelaten
                delay_ms(10);
                while(TouchPress == 0);

                GIFR |= (1<<INT0);                                      //Erase bit so that no interupt is generated 
                GICR |= (1<<INT0);                                      //Enable the INT0 request

        }
        while(Xt2 == 0 || Yt2 == 0);                                    //Als Xt1 of Yt1 0 is dan is een error bij meten dan nog een keer
        
        //Meten punt 2 klaar
        //Wis kruis meetpunt 2   
        S1D13705_Draw_Horizontal_Line(155,163,23,15);                   //Stuur kleur wit om de pixels te wissen
        S1D13705_Draw_Vertical_Line(159,19,27,15);   
        
        //Teken kruis meet punt 3
        //S1D13705_Draw_Horizontal_Line(27,35,215,0);                   //Instellen als X(begin), X1(eind), Y, kleur
        //S1D13705_Draw_Vertical_Line(31,211,219,0);                    //Instellen als X, Y(begin), Y1(eind), kleur     
        S1D13705_Draw_Horizontal_Line(283,291,119,0);                   //Instellen als X(begin), X1(eind), Y, kleur
        S1D13705_Draw_Vertical_Line(287,115,123,0);                     //Instellen als X, Y(begin), Y1(eind), kleur
               
        do
        {
                //Wacht totdat op het touchscreen is gedrukt
                while((Touchscreen.Flag_Register & (1<<TouchPressed)) == 0);         //Als uitkomst 0 is dan is er nog niet gedrukt 

                //Check of de touchscreen nog is ingedrukt dan pas meten
                if((i=Touchscreen_Pressed()) == 1)                      //Als waarde 1 is dan is touchscreen nog ingedrukt ga meten
                {        
                        Xt3 = Touchscreen_Value(X);                     //Meet de X waarde
                        Yt3 = Touchscreen_Value(Y);                     //Meet de Y waarde
                }
                else
                {
                        Xt3 = 0;
                        Yt3 = 0;
                } 
                
                //Zet touchscreen standby voor nieuwe drukmeting
                Touch_Standby;

                //Zet TouchPressed flag op 0
                Touchscreen.Flag_Register &= ~(1<<TouchPressed);
                
                //Wacht totdat touchscreen wordt los gelaten
                delay_ms(10);
                while(TouchPress == 0);
        
                GIFR |= (1<<INT0);                                      //Erase bit so that no interupt is generated 
                GICR |= (1<<INT0);                                      //Enable the INT0 request
                
        }
        while(Xt3 == 0 || Yt3 == 0);                                    //Als Xt1 of Yt1 0 is dan is een error bij meten dan nog een keer
         
        
        
        //Bereken de calibratie coeffenties
        A = ((((float)Xd1 - (float)Xd3) * ((float)Yt2 - (float)Yt3)) - (((float)Xd2 - (float)Xd3) * ((float)Yt1 - (float)Yt3))) / ((((float)Xt1 -(float) Xt3) * ((float)Yt2 - (float)Yt3)) - (((float)Xt2 - (float)Xt3) * ((float)Yt1 - (float)Yt3)));
        B = ( (A * ((float)Xt3 - (float)Xt2)) + (float)Xd2 - (float)Xd3) / ((float)Yt2 - (float)Yt3);
        C = (float)Xd3 - (A * (float)Xt3) - (B * (float)Yt3);  
        D = ((((float)Yd1 - (float)Yd3) * ((float)Yt2 - (float)Yt3)) - (((float)Yd2 - (float)Yd3) * ((float)Yt1 - (float)Yt3))) / ((((float)Xt1 - (float)Xt3) * ((float)Yt2 - (float)Yt3)) - (((float)Xt2 - (float)Xt3) * ((float)Yt1 - (float)Yt3)));
        E = ((D * ((float)Xt3 - (float)Xt2)) + (float)Yd2 - (float)Yd3) / ((float)Yt2 - (float)Yt3);
        F = (float)Yd3 - (D * (float)Xt3) - (E * (float)Yt3);
       
        
        //Sla waardes op in de EEPROM
        ee_float1 = A;
        ee_float2 = B;
        ee_float3 = C;
        ee_float4 = D;
        ee_float5 = E;
        ee_float6 = F;

        //Meten punt 3 klaar
        //Wis kruis meetpunt 3
        //S1D13705_Draw_Horizontal_Line(27,35,215,15);                  //Stuur kleur wit om de pixels te wissen         
        //S1D13705_Draw_Vertical_Line(31,211,219,15);                                                       
        S1D13705_Draw_Horizontal_Line(283,291,119,15);                  //Stuur kleur wit om de pixels te wissen       
        S1D13705_Draw_Vertical_Line(287,115,123,15);                                                    

        //Zet Flag in register dat calibratie is gedaan en wis calprocces
        Touchscreen.Flag_Register &= ~(1<<TouchCalProcess);             //Wis de TouchCalProcess bit      
        Touchscreen.Flag_Register &= ~(1<<TouchPressed);                //Wis de TouchPressed bit
        Touchscreen.Flag_Register |= (1<<TouchCalibratie);              
        ee_Touch_Flag = Touchscreen.Flag_Register;                      //Sla Flag register op in eeprom

        return;
}                                         
//*************************************
/**************************************
          ADC LTC 1298 driver
**************************************/
#include "lcd_test_defs.h"             
#include "adc_ltc1298.h"

/**************************************
Function: ADC On 
**************************************/
void ADC_LTC1298_On(void)
{                                                
        //SPI clock 125Khz
        //SPI_CLK_125K;  
        
        //Zet ADC actief
        ADC_LTC1298_Power_On;
        
        return;
}
//*************************************

/**************************************
Function: ADC off
**************************************/
void ADC_LTC1298_Off(void)
{
        //Set ADC off
        ADC_LTC1298_Power_Off;
        
        return;
}
//*************************************

/**************************************
Function: ADC measure data
**************************************/
unsigned int ADC_LTC1298_Data(unsigned char Channel)
{
        unsigned char LowByte;
        unsigned int HighByte;
        unsigned int test;   
        unsigned int test2;
        
        ADC_LTC1298_On();                                       //Set ADC active
        SPI_ON;                                                 //Set SPI on
        
        //Check wich channel must be measure
        if(Channel == 0)                                        //Channel 0
                spi(ADC_LTC1298_Channel_0);                     //Zend starbit en config voor channel 0
        else                                                    //Channel 1
                spi(ADC_LTC1298_Channel_1);                     //Zend startbit en config voor channel 1
               
        //Ontvang 12bit data, MSB eerst
        HighByte = spi(0xFF);        
        LowByte = spi(0xFF);                                    //Send dummy data on the spi to create CLK    
        
        ADC_LTC1298_Off();                                      //Set ADC inactive
        SPI_OFF;                                                //Set SPI off
        
        //Schuif HighByte naar hoge byte daarna lowbyte dan 
        //correctie voor 12bit waarde, 3 naar rechts schuiven
        return(((HighByte << 8) | LowByte) >> 3);               //Return with measure data
}
//*************************************

