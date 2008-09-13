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
