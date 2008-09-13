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
        
        
                                                        
        
