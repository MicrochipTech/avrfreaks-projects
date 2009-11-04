/*

	All LCD function

*/

#include "global.h"
#include "LCD_ST7565.h"
#include "LCD_Font.h"
#include "dogm-core.h"
#include "main.h"


//uint8_t disp_ram[1024];

uint8_t xy_adrColumn;   // globaler Spiecher für XY_Goto
uint8_t xy_adrPage;

void disp_init(void)
{
	dogm_reset(); //(100ms)


	//Display start line
	dogm_core_cmd(0x40); // Display start line 0

	//Bottom view
	dogm_core_cmd(0xA1); // ADC reverse
	dogm_core_cmd(0xC0); // Normal COM0~COM63

	//Normal / Inverted
	dogm_core_cmd(0xA6); // Display normal

	//Hardware options
	dogm_core_cmd(0xA2); // Set bias 1/9 (Duty 1/65)
	dogm_core_cmd(0x2F); // Booster, Regulator and Follower on
	dogm_core_cmd(0xF8); // Set internal Booster to 4x
	dogm_core_cmd(0x00);

	//Contrast options
	dogm_core_cmd(0x27); // Contrast set
	dogm_core_cmd(0x81);
	dogm_core_cmd(0x16);

	//Indicator options
	dogm_core_cmd(0xAC); // No indicator
	dogm_core_cmd(0x00);

	//(Init done)
	dogm_core_cmd(0xAF); // Display on
	
	LCD_CLR(); // DOGM  Clear
}



void dogm_reset(void)
{
	//Reset display (20ms)
	RES_PORT &= ~(1<<RES_BIT); // RES\ = LOW (Reset)
	Delay(200);	
	RES_PORT |= (1<<RES_BIT); // RES\ = HIGH (kein Reset)
}

void LCD_DisplayOnOff(uint8_t On)
{
	On &= 0x01;
	dogm_core_cmd(0xAE | On);
}

void LCD_PowerControl(uint8_t Power)
{
	Power &= 0x07;
	dogm_core_cmd(0x28 | Power);
}

void LCD_SetStartLine(uint8_t LineStart)
{
	LineStart &= 0x3F;
	dogm_core_cmd(0x40 | LineStart);
}

void LCD_SetPageAddress(uint8_t Page)
{
	Page &= 0x0F;
	dogm_core_cmd(0xB0 | Page);
}

void LCD_SetColumnAddress(uint8_t Column)
{
	dogm_core_cmd(0x10 | (Column>>4));
	dogm_core_cmd(Column & 0x0F);
}

//-----------------------------------------------------------------------------
// LCD:Drav Pixel on screen
//-----------------------------------------------------------------------------
void LCD_Draw_Pixel(uint8_t Xpos,uint8_t Ypos,uint8_t Color)
{
	   	uint8_t adrPage, adrLaw;
		LCD_SetPageAddress(0);
		LCD_SetColumnAddress(0);
		dogm_core_data(1);


}

//-----------------------------------------------------------------------------
// LCD:Clear picture.
//-----------------------------------------------------------------------------
void LCD_CLR_PICTURE(uint8_t Data)
{
   	uint8_t adrPage, adrLaw;
	
   	for(adrPage = 0; adrPage < MAX_PAGE; adrPage++)
	{  
		LCD_SetPageAddress(adrPage);
		LCD_SetColumnAddress(0);
      	for(adrLaw = 0; adrLaw < MAX_COLUMN; adrLaw++)
      	{
      		dogm_core_data(Data);
      	}
   	}
} 

void LCD_CLR(void)
{
   	uint8_t adrPage, adrLaw, Data;
	Data = 0;
   	
	for(adrPage = 0; adrPage < MAX_PAGE; adrPage++)
	{  
		LCD_SetPageAddress(adrPage);
		LCD_SetColumnAddress(0);
      	for(adrLaw = 0; adrLaw < MAX_COLUMN; adrLaw++)
      	{
      		dogm_core_data(Data);
      	}
   	}
} 
//-----------------------------------------------------------------------------
// LCD : Display Chinese.
// Size: 16x16
//-----------------------------------------------------------------------------
 uint8_t LCD_DisplayHZ(char* XH_HZ, uint8_t adrPage, uint8_t adrColumn)
{
   	uint8_t *dptr;               // dptr_adderss for HZ code.
   	uint8_t i, j;
	char charH, charL;

	charH = *XH_HZ++;
	charL = *XH_HZ;
	
    for(j=0; j<sizeof(HZ_Table)/sizeof(HZ_t); j++)
    {
    	dptr = &HZ_Table[j].Hz_Data[0];
        if(charH == pgm_read_byte(dptr) && charL == pgm_read_byte(dptr+1))
            break;
    }
	if(j<sizeof(HZ_Table)/sizeof(HZ_t))
   		dptr = HZ_Table[j].Table;
	else 
		return FALSE;
	
	LCD_SetPageAddress(adrPage); 
    LCD_SetColumnAddress(adrColumn);
   	for(i=0; i< 16; i++)    
  	{
     	dogm_core_data(pgm_read_byte(dptr));
     	dptr ++;
  	}
	
	LCD_SetPageAddress(adrPage+1); 
    LCD_SetColumnAddress(adrColumn);
    for(i=0; i< 16; i++)    
  	{
     	dogm_core_data(pgm_read_byte(dptr));
     	dptr ++;
  	}

    return TRUE;
}

//-----------------------------------------------------------------------------
// LCD  : lcd Display ASCII.
// Size : 8X16
//-----------------------------------------------------------------------------
void LCD_DisplayASCII5_7(uint8_t XH_Dig, uint8_t adrPage, uint8_t adrColumn)
{
   	uint8_t *dptr;               // dptr_adderss for Dig code.
   	uint8_t i;

   	if(XH_Dig < 0x20) XH_Dig = 0x20;
	
   	dptr = &ASCII5_7[(XH_Dig-0x20)*5];
	
	LCD_SetPageAddress(adrPage);
	LCD_SetColumnAddress(adrColumn);
	for(i=0; i< 5; i++)
	{
    	dogm_core_data(pgm_read_byte(dptr));
    	dptr = dptr + 1;
	}
	

} 
void LCD_DisplayASCII8_16(uint8_t XH_Dig, uint8_t adrPage, uint8_t adrColumn)
{
   	uint8_t *dptr;               // dptr_adderss for Dig code.
   	uint8_t i;

   	if(XH_Dig < 0x20) XH_Dig = 0x20;
	
   	dptr = &ASCII8_16[(XH_Dig-0x20)*16];
	
	LCD_SetPageAddress(adrPage);
	LCD_SetColumnAddress(adrColumn);
	for(i=0; i< 8; i++)
	{
    	dogm_core_data(pgm_read_byte(dptr));
    	dptr = dptr + 1;
	}
	
	LCD_SetPageAddress(adrPage+1);
	LCD_SetColumnAddress(adrColumn);
	for(i=0; i< 8; i++)
	{
    	dogm_core_data(pgm_read_byte(dptr));
    	dptr = dptr + 1;
	}
} 




//-----------------------------------------------------------------------------
// LCD  : lcd Display String.
// ASCII(8*16)
//-----------------------------------------------------------------------------

uint8_t LCD_DisplayString5_7(char *String, uint8_t adrPage, uint8_t adrColumn)
{
    char cData[2];
    uint8_t uLen,uPage,uCol;
	
	uLen = strlen(String);
    uPage = adrPage;
    uCol = adrColumn;

    while(uLen)
    {
        cData[0] = *String++;
		if(cData[0] <= 0x80)
		{
			LCD_DisplayASCII5_7(cData[0], uPage, uCol);
			uCol += 6;
			uLen --;
		}
		else
		{
			cData[1] = *String++;
			LCD_DisplayHZ(cData,  uPage, uCol);
			uCol += 16;
			uLen -= 2;
		}
		
		if(uCol >= MAX_COLUMN)
		{
			uCol = 0;
			uPage += 2;
			if(uPage >= MAX_PAGE)
				uPage = 0;
		}
	}
	
    return uLen;
}

uint8_t LCD_DisplayString8_16(char *String, uint8_t adrPage, uint8_t adrColumn)
{
    char cData[2];
    uint8_t uLen,uPage,uCol;
	
	uLen = strlen(String);
    uPage = adrPage;
    uCol = adrColumn;

    while(uLen)
    {
        cData[0] = *String++;
		if(cData[0] <= 0x80)
		{
			LCD_DisplayASCII8_16(cData[0], uPage, uCol);
			uCol += 8;
			uLen --;
		}
		else
		{
			cData[1] = *String++;
			LCD_DisplayHZ(cData,  uPage, uCol);
			uCol += 16;
			uLen -= 2;
		}
		
		if(uCol >= MAX_COLUMN)
		{
			uCol = 0;
			uPage += 2;
			if(uPage >= MAX_PAGE)
				uPage = 0;
		}
	}
	
    return uLen;
}

//-----------------------------------------------------------------------
// LCD_DisplayPicture
// Picture Size: Hight*Weight
//-----------------------------------------------------------------------
uint8_t LCD_DisplayPicture(char *Picture, uint8_t adrPage, uint8_t adrColumn,uint8_t Weight, uint8_t Hight)
{
	uint8_t *dptr;               // dptr_adderss for Dig code.
   	uint8_t i, j;

	if((adrPage+Hight)>MAX_PAGE) 		return FALSE;
	if((adrColumn+Weight)>MAX_COLUMN) 	return FALSE;
	
	dptr = (uint8_t *)Picture;
	
	for(i=0; i<Hight; i++)
	{
		LCD_SetPageAddress(adrPage+i);
		LCD_SetColumnAddress(adrColumn);
		for(j=0; j<Weight; j++)
		{
			dogm_core_data(pgm_read_byte(dptr));
		}
	}
	
	return TRUE;
}

void LCD_DisplayChar5_7(uint8_t XH_Dig)
{
    
	uint8_t *dptr;               // dptr_adderss for Dig code.
   	uint8_t i;

   	if(XH_Dig < 0x20) XH_Dig = 0x20;
   	dptr = &ASCII5_7[(XH_Dig-0x20)*5];
	LCD_SetPageAddress(xy_adrPage);
	LCD_SetColumnAddress(xy_adrColumn);
	for(i=0; i< 5; i++)
	{
    	dogm_core_data(pgm_read_byte(dptr));
    	dptr = dptr + 1;
	}
		xy_adrColumn = xy_adrColumn+6;  // Zeichenabstand 6 = ok, 5 fast zu eng, 
}

void LCD_DisplayChar8_16(uint8_t XH_Dig)
{
    
	uint8_t *dptr;               // dptr_adderss for Dig code.
   	uint8_t i;

   	if(XH_Dig < 0x20) XH_Dig = 0x20;
	
   	dptr = &ASCII8_16[(XH_Dig-0x20)*16];
	
	LCD_SetPageAddress(xy_adrPage);
	LCD_SetColumnAddress(xy_adrColumn);
	for(i=0; i< 8; i++)
	{
    	dogm_core_data(pgm_read_byte(dptr));
    	dptr = dptr + 1;
	}

	LCD_SetPageAddress(xy_adrPage+1);
	LCD_SetColumnAddress(xy_adrColumn);
	for(i=0; i< 8; i++)
	{
		dogm_core_data(pgm_read_byte(dptr));
    	dptr = dptr + 1;
	}
	xy_adrColumn = xy_adrColumn+8;
}


void XY_Goto5_7(uint8_t adrPage, uint8_t adrColumn) // nach Page 0-7 und Column 0-24 
{
 	xy_adrColumn = adrColumn;
 	xy_adrColumn = ((xy_adrColumn*5)+1);
	if(xy_adrColumn>= 121)	xy_adrColumn = 121;
	xy_adrPage   = adrPage;
	if(xy_adrPage>= 7)		xy_adrPage = 7;
}

void XY_Goto8_16(uint8_t adrPage, uint8_t adrColumn) // nach Page 0-6 und Column 0-15 
{
 	xy_adrColumn = adrColumn;
 	xy_adrColumn = (xy_adrColumn*8);
	if(xy_adrColumn>= 120)	xy_adrColumn = 120;
	xy_adrPage   = adrPage;
	if(xy_adrPage>= 6)		xy_adrPage = 6;
}
