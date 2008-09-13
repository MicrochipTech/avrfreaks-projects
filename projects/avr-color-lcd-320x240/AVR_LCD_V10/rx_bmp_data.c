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

