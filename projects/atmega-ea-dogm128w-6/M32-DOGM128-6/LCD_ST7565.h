#define MAX_LINE	64
#define MAX_COLUMN	128
#define MAX_PAGE	(MAX_LINE/8)
#define DISP_WIDTH   128L
#define DISP_HEIGHT  64L
#define DISP_DEPTH   1   //monochrome

#define LCD_DOGM_DPORT  DDRB					// Portbelegung Atmeg644   EA DOGM128
#define LCD_DOGM_PORT   PORTB
#define LCD_DOGM_CSB    PB1     				// PB1 = 			CS   	(Pin40)
#define LCD_DOGM_A0     PB0						// PB0 = 			A0   	(Pin38)  

#define RES_PORT 		PORTB		
#define RES_DDR  		DDRB	
#define RES_BIT  		PB2						// PB2 = 			Reset	(Pin39)

												// PB7 = 			SCK, 	(Pin37)
									    		// PB5 =			MOSI   	(Pin		

#define LCD_BACKLIGHT   PORT1
#define LCD_BackLightOn()           (PORTC |= BIT(LCD_BACKLIGHT))
#define LCD_BackLightOff()          (PORTC &= ~BIT(LCD_BACKLIGHT))

//-----------------------------------------------------------------------------
// LCD : Display Chinese. wird nicht verwendet
// Size: 16x16
//-----------------------------------------------------------------------------
extern uint8_t LCD_DisplayHZ(char* XH_HZ, uint8_t adrPage, uint8_t adrColumn);

//-----------------------------------------------------------------------------
// LCD  : lcd Display ASCII.
// Size : 5X7
// Size : 8X16
//-----------------------------------------------------------------------------
void LCD_DisplayASCII5_7(uint8_t XH_Dig, uint8_t adrPage, uint8_t adrColumn);
void LCD_DisplayASCII8_16(uint8_t XH_Dig, uint8_t adrPage, uint8_t adrColumn);

//-----------------------------------------------------------------------------
// LCD  : lcd Display String.
// ASCII(8*16)
//-----------------------------------------------------------------------------
extern uint8_t LCD_DisplayString5_7(char *String, uint8_t adrPage, uint8_t adrColumn);
extern uint8_t LCD_DisplayString8_16(char *String, uint8_t adrPage, uint8_t adrColumn);

//-----------------------------------------------------------------------
// LCD_DisplayPicture
// Picture Size: Hight*Weight
//-----------------------------------------------------------------------


uint8_t LCD_DisplayPicture(char *Picture, uint8_t adrPage, uint8_t adrColumn,uint8_t Weight, uint8_t Hight); 

void LCD_Draw_Pixel(uint8_t Xpos,uint8_t Ypos,uint8_t Color);
							
void XY_Goto5_7(uint8_t adrPage, uint8_t adrColumn);  // Coursor XY_Goto 
void XY_Goto8_16(uint8_t adrPage, uint8_t adrColumn);  // Coursor XY_Goto 


void disp_set_pixel(uint8_t x, uint8_t y, uint8_t pixel_status);
void disp_clear_dont_refresh();

void lcd_spi_write(unsigned char hexwrite); //SPI Init starten
void dogm_reset(void);						// Display Reset
void lcd_set_cs(uint8_t enable);			// set Display cs
void lcd_set_a0(uint8_t type);				// set Display a0
void delay_s(uint8_t sec); 					// Delay 

void disp_init(void);						//Display EA DOGM128-6 Init und Clear
void LCD_DisplayChar5_7(uint8_t Dig);
void LCD_DisplayChar8_16(uint8_t Dig);
void LCD_DisplayOnOff(uint8_t On);
void LCD_PowerControl(uint8_t Power);
void LCD_SetStartLine(uint8_t LineStart);
void LCD_SetPageAddress(uint8_t Page);
void LCD_SetColumnAddress(uint8_t Column);
void LCD_CLR_PICTURE(uint8_t Data);
void LCD_CLR(void);
uint8_t xy_adrColumn;
uint8_t xy_adrPage;


