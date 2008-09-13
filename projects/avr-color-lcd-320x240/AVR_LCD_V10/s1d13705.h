/**************************************
        S1D13705 header file
**************************************/
unsigned char S1D13705_Read_Data(unsigned long int adress); 
void S1D13705_Write_Data(unsigned long int adress, unsigned char data);
unsigned char S1D13705_Read_Register(unsigned char reg);        
void S1D13705_Write_Register(unsigned char reg, unsigned char data);
void S1D13705_Text_LCD(unsigned long int X, unsigned long int Y, unsigned char *Tekst, unsigned char color);
void S1D13705_Fill_LCD(unsigned long int adress, unsigned long int len, unsigned char color);
void S1D13705_Draw_Pic(unsigned int X, unsigned int Y, unsigned char flash* Pic); 
void S1D13705_Draw_Pixel(unsigned long int X, unsigned long int Y, unsigned char color);
void S1D13705_Draw_Horizontal_Line(unsigned long int X, unsigned long int X1, unsigned long int Y, unsigned char color);
void S1D13705_Draw_Vertical_Line(unsigned long int X, unsigned long int Y, unsigned long int Y1, unsigned char color);
void S1D13705_Init(void);      
void S1D13705_Reset(void);   
void S1D13705_Clear_LCD(void); 
 
//#define Info_S1D13705
//#define S1D13705_16_colors
#define S1D13705_256_colors    


//Control cmd
#define RD_ON                   0
#define RD_OFF                  1
#define WR_ON                   0
#define WR_OFF                  1
#define RST_ON                  0
#define RST_OFF                 1
#define CS_ON                   0
#define CS_OFF                  1

//PORTS
#define PORT_AD_ctrl            DDRA  
#define PORT_Data_IN            PINA
#define PORT_Data_OUT           PORTA
#define PORT_Addr_Low           PORTA
#define PORT_Addr_High          PORTC
#define PORT_Addr_A16           PORTE.2  


//Control
#define S1D13705_RD             PORTD.7
#define S1D13705_WR             PORTD.6
#define S1D13705_RST            PORTD.5
#define S1D13705_CS             PORTD.4
#define S1D13705_WAIT           PINE.0            

//Register 
#define S1D13705_Reg_Base_Addr  0x1FFE0 
#define S1DNUMREGS              30

//Kleuren
#define BLACK                   0
#define BLUE                    1
#define GREEN                   2
#define CYAN                    4
#define RED                     5
#define PURPLE                  6
#define YELLOW                  7
#define WHITE                   8
#define LT_BLUE                 10
#define LT_GREEN                11
#define LT_CYAN                 12
#define LT_RED                  13
#define LT_PURPLE               14
#define LT_YELLOW               15
#define LT_WHITE                16		
                                             
/**************************************
Header gedeelte gemaakt door 13705CFG.EXE (Build 9)

Panel: 320x240x8bpp 53Hz Color 12-Bit TFT/D-TFD (PCLK=12.000MHz)
**************************************/ 

#ifdef S1D13705_16_colors
#define S1D13705_MEM_SIZE               38400           //37,5Kb
#endif

#ifdef S1D13705_256_colors
#define S1D13705_MEM_SIZE               76800           //75Kb
#endif
	       

typedef unsigned char S1D_INDEX;
typedef unsigned char S1D_VALUE;
typedef struct
{
    S1D_INDEX Index;
    S1D_VALUE Value;
} S1D_REGS;

#ifdef S1D13705_16_colors 
static S1D_REGS aS1DRegs[] =
{
    { 0x00,             0x24 },       // Revision Code Register
    { 0x01,             0xA1 },       // Mode Register 0 Register
    { 0x02,             0xB0 },       // Mode Register 1 Register
    { 0x03,             0x03 },       // Mode Register 2 Register
    { 0x04,             0x27 },       // Horizontal Panel Size Register
    { 0x05,             0xEF },       // Vertical Panel Size Register (LSB)
    { 0x06,             0x00 },       // Vertical Panel Size Register (MSB)
    { 0x07,             0x01 },       // FPLINE Start Position Register
    { 0x08,             0x03 },       // Horizontal Non-Display Period Register
    { 0x09,             0x05 },       // FPFRAME Start Position Register
    { 0x0A,             0x0B },       // Vertical Non-Display Period Register
    { 0x0B,             0x00 },       // MOD Rate Register
    { 0x0C,             0x00 },       // Screen 1 Start Address Register (LSB)
    { 0x0D,             0x00 },       // Screen 1 Start Address Register (MSB)
    { 0x0E,             0x00 },       // Screen 2 Start Address Register (LSB)
    { 0x0F,             0x00 },       // Screen 2 Start Address Register (MSB)
    { 0x10,             0x00 },       // Screen Start Address Overflow Register
    { 0x11,             0x00 },       // Memory Address Offset Register
    { 0x12,             0xFF },       // Screen 1 Vertical Size Register (LSB)
    { 0x13,             0x03 },       // Screen 1 Vertical Size Register (MSB)
    { 0x18,             0x00 },       // GPIO Configuration Control Register
    { 0x19,             0x00 },       // GPIO Status/Control Register
    { 0x1A,             0x00 },       // Scratch Pad Register
    { 0x1B,             0x00 },       // SwivelView Mode Register
    { 0x1C,             0xA0 }        // Line Byte Count Register
}; 
#endif

#ifdef S1D13705_256_colors 
static S1D_REGS aS1DRegs[] =
{
    { 0x00,             0x24 },       // Revision Code Register
    { 0x01,             0xA1 },       // Mode Register 0 Register
    { 0x02,             0xF0 },       // Mode Register 1 Register
    { 0x03,             0x03 },       // Mode Register 2 Register
    { 0x04,             0x27 },       // Horizontal Panel Size Register
    { 0x05,             0xEF },       // Vertical Panel Size Register (LSB)
    { 0x06,             0x00 },       // Vertical Panel Size Register (MSB)
    { 0x07,             0x01 },       // FPLINE Start Position Register
    { 0x08,             0x03 },       // Horizontal Non-Display Period Register
    { 0x09,             0x05 },       // FPFRAME Start Position Register
    { 0x0A,             0x0B },       // Vertical Non-Display Period Register
    { 0x0B,             0x00 },       // MOD Rate Register
    { 0x0C,             0x00 },       // Screen 1 Start Address Register (LSB)
    { 0x0D,             0x00 },       // Screen 1 Start Address Register (MSB)
    { 0x0E,             0x00 },       // Screen 2 Start Address Register (LSB)
    { 0x0F,             0x00 },       // Screen 2 Start Address Register (MSB)
    { 0x10,             0x00 },       // Screen Start Address Overflow Register
    { 0x11,             0x00 },       // Memory Address Offset Register
    { 0x12,             0xFF },       // Screen 1 Vertical Size Register (LSB)
    { 0x13,             0x03 },       // Screen 1 Vertical Size Register (MSB)
    { 0x18,             0x00 },       // GPIO Configuration Control Register
    { 0x19,             0x00 },       // GPIO Status/Control Register
    { 0x1A,             0x00 },       // Scratch Pad Register
    { 0x1B,             0x00 },       // SwivelView Mode Register
    { 0x1C,             0xFF }        // Line Byte Count Register
}; 
#endif
//*************************************


        
        