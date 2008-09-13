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
 