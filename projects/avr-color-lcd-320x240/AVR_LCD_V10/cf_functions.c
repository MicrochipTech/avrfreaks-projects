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
        
                