/*********************************************************
                Header file Compact flash Driver
*********************************************************/
void CF_Init(void);                            
void CF_Wait_RDY(void);
void CF_Wait_DRQ(void);
void CF_Write_Register(unsigned char Reg_Adres, unsigned char Reg_Value);
unsigned char CF_Read_Register(unsigned char Reg_Adres);
//void CF_Read_Sector(unsigned char Sector_Count, unsigned char Sector_No, unsigned char Cylinder_Low, unsigned char Cylinder_High, unsigned char Head);
//void CF_Write_Sector(unsigned char Sector_Count, unsigned char Sector_No, unsigned char Cylinder_Low, unsigned char Cylinder_High, unsigned char Head);
void CF_Write_Sector(unsigned long int Sector);
void CF_Read_Sector(unsigned long int Sector);
void CF_Info(void);



