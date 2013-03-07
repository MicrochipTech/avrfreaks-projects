#include "lcd.h"

__flash unsigned char english[][16]={

{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},/*" ",0*/
{0x00,0x00,0x00,0x00,0x18,0x3C,0x3C,0x3C,0x18,0x18,0x00,0x18,0x18,0x00,0x00,0x00},/*"!",1*/
{0x00,0x00,0x00,0x00,0x66,0x66,0x66,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},/*""",2*/
{0x00,0x00,0x00,0x00,0x36,0x36,0x7F,0x36,0x36,0x36,0x7F,0x36,0x36,0x00,0x00,0x00},/*"#",3*/
{0x00,0x00,0x18,0x18,0x3C,0x66,0x60,0x30,0x18,0x0C,0x06,0x66,0x3C,0x18,0x18,0x00},/*"$",4*/
{0x00,0x00,0x00,0x70,0xD8,0xDA,0x76,0x0C,0x18,0x30,0x6E,0x5B,0x1B,0x0E,0x00,0x00},/*"%",5*/
{0x00,0x00,0x00,0x00,0x38,0x6C,0x6C,0x38,0x60,0x6F,0x66,0x66,0x3B,0x00,0x00,0x00},/*"&",6*/
{0x00,0x00,0x00,0x00,0x18,0x18,0x18,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},/*"'",7*/
{0x00,0x00,0x00,0x00,0x0C,0x18,0x18,0x30,0x30,0x30,0x30,0x30,0x18,0x18,0x0C,0x00},/*"(",8*/
{0x00,0x00,0x00,0x00,0x30,0x18,0x18,0x0C,0x0C,0x0C,0x0C,0x0C,0x18,0x18,0x30,0x00},/*")",9*/
{0x00,0x00,0x00,0x00,0x00,0x00,0x36,0x1C,0x7F,0x1C,0x36,0x00,0x00,0x00,0x00,0x00},/*"*",10*/
{0x00,0x00,0x00,0x00,0x00,0x00,0x18,0x18,0x7E,0x18,0x18,0x00,0x00,0x00,0x00,0x00},/*"+",11*/
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1C,0x1C,0x0C,0x18,0x00,0x00},/*",",12*/
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x7E,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},/*"-",13*/
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1C,0x1C,0x00,0x00,0x00,0x00},/*".",14*/
{0x00,0x00,0x00,0x06,0x06,0x0C,0x0C,0x18,0x18,0x30,0x30,0x60,0x60,0x00,0x00,0x00},/*"/",15*/
{0x00,0x00,0x00,0x1E,0x33,0x37,0x37,0x33,0x3B,0x3B,0x33,0x1E,0x00,0x00,0x00,0x00},/*"0",16*/
{0x00,0x00,0x00,0x0C,0x1C,0x7C,0x0C,0x0C,0x0C,0x0C,0x0C,0x0C,0x00,0x00,0x00,0x00},/*"1",17*/
{0x00,0x00,0x00,0x3C,0x66,0x66,0x06,0x0C,0x18,0x30,0x60,0x7E,0x00,0x00,0x00,0x00},/*"2",18*/
{0x00,0x00,0x00,0x3C,0x66,0x66,0x06,0x1C,0x06,0x66,0x66,0x3C,0x00,0x00,0x00,0x00},/*"3",19*/
{0x00,0x00,0x00,0x30,0x30,0x36,0x36,0x36,0x66,0x7F,0x06,0x06,0x00,0x00,0x00,0x00},/*"4",20*/
{0x00,0x00,0x00,0x7E,0x60,0x60,0x60,0x7C,0x06,0x06,0x0C,0x78,0x00,0x00,0x00,0x00},/*"5",21*/
{0x00,0x00,0x00,0x1C,0x18,0x30,0x7C,0x66,0x66,0x66,0x66,0x3C,0x00,0x00,0x00,0x00},/*"6",22*/
{0x00,0x00,0x00,0x7E,0x06,0x0C,0x0C,0x18,0x18,0x30,0x30,0x30,0x00,0x00,0x00,0x00},/*"7",23*/
{0x00,0x00,0x00,0x3C,0x66,0x66,0x76,0x3C,0x6E,0x66,0x66,0x3C,0x00,0x00,0x00,0x00},/*"8",24*/
{0x00,0x00,0x00,0x3C,0x66,0x66,0x66,0x66,0x3E,0x0C,0x18,0x38,0x00,0x00,0x00,0x00},/*"9",25*/
{0x00,0x00,0x00,0x00,0x00,0x1C,0x1C,0x00,0x00,0x00,0x1C,0x1C,0x00,0x00,0x00,0x00},/*":",26*/
{0x00,0x00,0x00,0x00,0x00,0x1C,0x1C,0x00,0x00,0x00,0x1C,0x1C,0x0C,0x18,0x00,0x00},/*";",27*/
{0x00,0x00,0x00,0x06,0x0C,0x18,0x30,0x60,0x30,0x18,0x0C,0x06,0x00,0x00,0x00,0x00},/*"<",28*/
{0x00,0x00,0x00,0x00,0x00,0x00,0x7E,0x00,0x7E,0x00,0x00,0x00,0x00,0x00,0x00,0x00},/*"=",29*/
{0x00,0x00,0x00,0x60,0x30,0x18,0x0C,0x06,0x0C,0x18,0x30,0x60,0x00,0x00,0x00,0x00},/*">",30*/
{0x00,0x00,0x00,0x3C,0x66,0x66,0x0C,0x18,0x18,0x00,0x18,0x18,0x00,0x00,0x00,0x00},/*"?",31*/
{0x00,0x00,0x00,0x7E,0xC3,0xC3,0xCF,0xDB,0xDB,0xCF,0xC0,0x7F,0x00,0x00,0x00,0x00},/*"@",32*/
{0x00,0x00,0x00,0x18,0x3C,0x66,0x66,0x66,0x7E,0x66,0x66,0x66,0x00,0x00,0x00,0x00},/*"A",33*/
{0x00,0x00,0x00,0x7C,0x66,0x66,0x66,0x7C,0x66,0x66,0x66,0x7C,0x00,0x00,0x00,0x00},/*"B",34*/
{0x00,0x00,0x00,0x3C,0x66,0x66,0x60,0x60,0x60,0x66,0x66,0x3C,0x00,0x00,0x00,0x00},/*"C",35*/
{0x00,0x00,0x00,0x78,0x6C,0x66,0x66,0x66,0x66,0x66,0x6C,0x78,0x00,0x00,0x00,0x00},/*"D",36*/
{0x00,0x00,0x00,0x7E,0x60,0x60,0x60,0x7C,0x60,0x60,0x60,0x7E,0x00,0x00,0x00,0x00},/*"E",37*/
{0x00,0x00,0x00,0x7E,0x60,0x60,0x60,0x7C,0x60,0x60,0x60,0x60,0x00,0x00,0x00,0x00},/*"F",38*/
{0x00,0x00,0x00,0x3C,0x66,0x66,0x60,0x60,0x6E,0x66,0x66,0x3E,0x00,0x00,0x00,0x00},/*"G",39*/
{0x00,0x00,0x00,0x66,0x66,0x66,0x66,0x7E,0x66,0x66,0x66,0x66,0x00,0x00,0x00,0x00},/*"H",40*/
{0x00,0x00,0x00,0x3C,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x3C,0x00,0x00,0x00,0x00},/*"I",41*/
{0x00,0x00,0x1E,0x0C,0x0C,0x0C,0x0C,0x0C,0xCC,0xCC,0xCC,0x78,0x00,0x00,0x00,0x00},/*"J",42*/
{0x00,0x00,0x00,0x66,0x66,0x6C,0x6C,0x78,0x6C,0x6C,0x66,0x66,0x00,0x00,0x00,0x00},/*"K",43*/
{0x00,0x00,0x00,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x7E,0x00,0x00,0x00,0x00},/*"L",44*/
{0x00,0x00,0x00,0x63,0x63,0x77,0x6B,0x6B,0x6B,0x63,0x63,0x63,0x00,0x00,0x00,0x00},/*"M",45*/
{0x00,0x00,0x00,0x63,0x63,0x73,0x7B,0x6F,0x67,0x63,0x63,0x63,0x00,0x00,0x00,0x00},/*"N",46*/
{0x00,0x00,0x00,0x3C,0x66,0x66,0x66,0x66,0x66,0x66,0x66,0x3C,0x00,0x00,0x00,0x00},/*"O",47*/
{0x00,0x00,0x00,0x7C,0x66,0x66,0x66,0x7C,0x60,0x60,0x60,0x60,0x00,0x00,0x00,0x00},/*"P",48*/
{0x00,0x00,0x00,0x3C,0x66,0x66,0x66,0x66,0x66,0x66,0x66,0x3C,0x0C,0x06,0x00,0x00},/*"Q",49*/
{0x00,0x00,0x00,0x7C,0x66,0x66,0x66,0x7C,0x6C,0x66,0x66,0x66,0x00,0x00,0x00,0x00},/*"R",50*/
{0x00,0x00,0x00,0x3C,0x66,0x60,0x30,0x18,0x0C,0x06,0x66,0x3C,0x00,0x00,0x00,0x00},/*"S",51*/
{0x00,0x00,0x00,0x7E,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x00,0x00,0x00,0x00},/*"T",52*/
{0x00,0x00,0x00,0x66,0x66,0x66,0x66,0x66,0x66,0x66,0x66,0x3C,0x00,0x00,0x00,0x00},/*"U",53*/
{0x00,0x00,0x00,0x66,0x66,0x66,0x66,0x66,0x66,0x66,0x3C,0x18,0x00,0x00,0x00,0x00},/*"V",54*/
{0x00,0x00,0x00,0x63,0x63,0x63,0x6B,0x6B,0x6B,0x36,0x36,0x36,0x00,0x00,0x00,0x00},/*"W",55*/
{0x00,0x00,0x00,0x66,0x66,0x34,0x18,0x18,0x2C,0x66,0x66,0x66,0x00,0x00,0x00,0x00},/*"X",56*/
{0x00,0x00,0x00,0x66,0x66,0x66,0x66,0x3C,0x18,0x18,0x18,0x18,0x00,0x00,0x00,0x00},/*"Y",57*/
{0x00,0x00,0x00,0x7E,0x06,0x06,0x0C,0x18,0x30,0x60,0x60,0x7E,0x00,0x00,0x00,0x00},/*"Z",58*/
{0x00,0x00,0x00,0x3C,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x3C,0x00},/*"[",59*/
{0x00,0x00,0x00,0x60,0x60,0x30,0x30,0x18,0x18,0x0C,0x0C,0x06,0x06,0x00,0x00,0x00},/*"\",60*/
{0x00,0x00,0x00,0x3C,0x0C,0x0C,0x0C,0x0C,0x0C,0x0C,0x0C,0x0C,0x0C,0x0C,0x3C,0x00},/*"]",61*/
{0x00,0x18,0x3C,0x66,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},/*"^",62*/
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0x00},/*"_",63*/
{0x00,0x38,0x18,0x0C,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},/*"`",64*/
{0x00,0x00,0x00,0x00,0x00,0x3C,0x06,0x06,0x3E,0x66,0x66,0x3E,0x00,0x00,0x00,0x00},/*"a",65*/
{0x00,0x00,0x00,0x60,0x60,0x7C,0x66,0x66,0x66,0x66,0x66,0x7C,0x00,0x00,0x00,0x00},/*"b",66*/
{0x00,0x00,0x00,0x00,0x00,0x3C,0x66,0x60,0x60,0x60,0x66,0x3C,0x00,0x00,0x00,0x00},/*"c",67*/
{0x00,0x00,0x00,0x06,0x06,0x3E,0x66,0x66,0x66,0x66,0x66,0x3E,0x00,0x00,0x00,0x00},/*"d",68*/
{0x00,0x00,0x00,0x00,0x00,0x3C,0x66,0x66,0x7E,0x60,0x60,0x3C,0x00,0x00,0x00,0x00},/*"e",69*/
{0x00,0x00,0x00,0x1E,0x30,0x30,0x30,0x7E,0x30,0x30,0x30,0x30,0x00,0x00,0x00,0x00},/*"f",70*/
{0x00,0x00,0x00,0x00,0x00,0x3E,0x66,0x66,0x66,0x66,0x66,0x3E,0x06,0x06,0x7C,0x00},/*"g",71*/
{0x00,0x00,0x00,0x60,0x60,0x7C,0x66,0x66,0x66,0x66,0x66,0x66,0x00,0x00,0x00,0x00},/*"h",72*/
{0x00,0x00,0x18,0x18,0x00,0x78,0x18,0x18,0x18,0x18,0x18,0x7E,0x00,0x00,0x00,0x00},/*"i",73*/
{0x00,0x00,0x0C,0x0C,0x00,0x3C,0x0C,0x0C,0x0C,0x0C,0x0C,0x0C,0x0C,0x0C,0x78,0x00},/*"j",74*/
{0x00,0x00,0x00,0x60,0x60,0x66,0x66,0x6C,0x78,0x6C,0x66,0x66,0x00,0x00,0x00,0x00},/*"k",75*/
{0x00,0x00,0x00,0x78,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x7E,0x00,0x00,0x00,0x00},/*"l",76*/
{0x00,0x00,0x00,0x00,0x00,0x7E,0x6B,0x6B,0x6B,0x6B,0x6B,0x63,0x00,0x00,0x00,0x00},/*"m",77*/
{0x00,0x00,0x00,0x00,0x00,0x7C,0x66,0x66,0x66,0x66,0x66,0x66,0x00,0x00,0x00,0x00},/*"n",78*/
{0x00,0x00,0x00,0x00,0x00,0x3C,0x66,0x66,0x66,0x66,0x66,0x3C,0x00,0x00,0x00,0x00},/*"o",79*/
{0x00,0x00,0x00,0x00,0x00,0x7C,0x66,0x66,0x66,0x66,0x66,0x7C,0x60,0x60,0x60,0x00},/*"p",80*/
{0x00,0x00,0x00,0x00,0x00,0x3E,0x66,0x66,0x66,0x66,0x66,0x3E,0x06,0x06,0x06,0x00},/*"q",81*/
{0x00,0x00,0x00,0x00,0x00,0x66,0x6E,0x70,0x60,0x60,0x60,0x60,0x00,0x00,0x00,0x00},/*"r",82*/
{0x00,0x00,0x00,0x00,0x00,0x3E,0x60,0x60,0x3C,0x06,0x06,0x7C,0x00,0x00,0x00,0x00},/*"s",83*/
{0x00,0x00,0x00,0x30,0x30,0x7E,0x30,0x30,0x30,0x30,0x30,0x1E,0x00,0x00,0x00,0x00},/*"t",84*/
{0x00,0x00,0x00,0x00,0x00,0x66,0x66,0x66,0x66,0x66,0x66,0x3E,0x00,0x00,0x00,0x00},/*"u",85*/
{0x00,0x00,0x00,0x00,0x00,0x66,0x66,0x66,0x66,0x66,0x3C,0x18,0x00,0x00,0x00,0x00},/*"v",86*/
{0x00,0x00,0x00,0x00,0x00,0x63,0x6B,0x6B,0x6B,0x6B,0x36,0x36,0x00,0x00,0x00,0x00},/*"w",87*/
{0x00,0x00,0x00,0x00,0x00,0x66,0x66,0x3C,0x18,0x3C,0x66,0x66,0x00,0x00,0x00,0x00},/*"x",88*/
{0x00,0x00,0x00,0x00,0x00,0x66,0x66,0x66,0x66,0x66,0x66,0x3C,0x0C,0x18,0xF0,0x00},/*"y",89*/
{0x00,0x00,0x00,0x00,0x00,0x7E,0x06,0x0C,0x18,0x30,0x60,0x7E,0x00,0x00,0x00,0x00},/*"z",90*/
{0x00,0x00,0x00,0x0C,0x18,0x18,0x18,0x30,0x60,0x30,0x18,0x18,0x18,0x0C,0x00,0x00},/*"{",91*/
{0x00,0x00,0x00,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x00},/*"|",92*/
{0x00,0x00,0x00,0x30,0x18,0x18,0x18,0x0C,0x06,0x0C,0x18,0x18,0x18,0x30,0x00,0x00},/*"}",93*/
{0x00,0x00,0x00,0x71,0xDB,0x8E,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00} /*"~",94*/
};

void LCD_WR_DATA16(unsigned int data)
{
 LCD_RD_H();
 LCD_RS_H();
 LCD_CS_L();  
 LCD_WR_L();	 
 DATA_LCD_PORT=data>>8;
 delay_us(1);
 LCD_WR_H();
 LCD_WR_L();
 delay_us(1);
 DATA_LCD_PORT=data&0XFF; 
 delay_us(1);
 LCD_WR_H();
 LCD_CS_H();  
}

void LCD_WR_REG16(unsigned int index)
{
 LCD_RD_H();
 LCD_RS_L();
 LCD_CS_L();  
 LCD_WR_L();
 delay_us(1);
 DATA_LCD_PORT=index>>8; 
 delay_us(1);
 LCD_WR_H();
 LCD_WR_L();
 delay_us(1);
 DATA_LCD_PORT=index&0XFF; 
 delay_us(1);
 LCD_WR_H();
 LCD_CS_H();
}

void LCD_WR_REG(unsigned int index,unsigned int data)
{
 LCD_WR_REG16(index);
 LCD_WR_DATA16(data);
}

void LCD_Init(void)
{
 LCD_PORT_INI();
 LCD_RST_L();
 delay_ms(100);
 LCD_RST_H();
 delay_ms(100); 
 //initializing funciton 1 
 LCD_WR_REG(0x00E3,0x3008);        
 LCD_WR_REG(0x00E7,0x0012);
 LCD_WR_REG(0x00Ef,0x1231);// Set the internal timing;    
 LCD_WR_REG(0x00,0x0001);
 LCD_WR_REG(0x01, 0x0000); // set SS and SM bit
 LCD_WR_REG(0x02, 0x0700); // set 1 line inversion
 // LCD_WR_REG(0x03, 0x10B0); // set GRAM write direction and BGR=1.
#if   ID_AM==000       
	     LCD_WR_REG(0x0003,0x1000);// TFM=0,TRI=0,SWAP=1,16 bits system interface  swap RGB to BRG 
#elif ID_AM==001        
	     LCD_WR_REG(0x0003,0x1008);      
#elif ID_AM==010  
	     LCD_WR_REG(0x0003,0x1010);        
#elif ID_AM==011
	     LCD_WR_REG(0x0003,0x1018);
#elif ID_AM==100  
	     LCD_WR_REG(0x0003,0x1020);      
#elif ID_AM==101  
	     LCD_WR_REG(0x0003,0x1028);      
#elif ID_AM==110  
	     LCD_WR_REG(0x0003,0x1030);      
#elif ID_AM==111  
	     LCD_WR_REG(0x0003,0x1038);
#endif    
 LCD_WR_REG(0x04, 0x0000); // Resize register
 LCD_WR_REG(0x08, 0x0404); // set the back porch and front porch
 LCD_WR_REG(0x09, 0x0000); // set non-display area refresh cycle ISC[3:0]
 LCD_WR_REG(0x0A, 0x0000); // FMARK function
 //LCD_WR_REG(0x0C, 0x0000); // RGB interface setting
 //LCD_WR_REG(0x0D, 0x0000); // Frame marker Position
 //LCD_WR_REG(0x0F, 0x0000); // RGB interface polarity
 //Power On sequence //
 LCD_WR_REG(0x10, 0x0080); // SAP, BT[3:0], AP, DSTB, SLP, STB
 LCD_WR_REG(0x11, 0x0007); // DC1[2:0], DC0[2:0], VC[2:0]
 LCD_WR_REG(0x12, 0x0000); // VREG1OUT voltage
 LCD_WR_REG(0x13, 0x0000); // VDV[4:0] for VCOM amplitude
 LCD_WR_REG(0x07, 0x0001); // 
		 
 //delay_ms(2000); // Dis-charge capacitor power voltage
 delay_ms(100);
         
 LCD_WR_REG(0x10, 0x1590); // SAP, BT[3:0], AP, DSTB, SLP, STB
 LCD_WR_REG(0x11, 0x0227); // DC1[2:0], DC0[2:0], VC[2:0]
 //delay_ms(1000); // Delay 50ms
 delay_ms(50);
        
 LCD_WR_REG(0x12, 0x001e); // Internal reference voltage= Vci;
 //delay_ms(1000); // Delay 50ms
 delay_ms(50);
 LCD_WR_REG(0x13, 0x1500); // Set VDV[4:0] for VCOM amplitude
 LCD_WR_REG(0x29, 0x0028); // Set VCM[5:0] for VCOMH
 LCD_WR_REG(0x2B, 0x000f); // Set Frame Rate
 //delay_ms(500); // Delay 50ms
 delay_ms(50);
         
#if   ID_AM==000         
 LCD_WR_REG(0x0020,0x00ef);
 LCD_WR_REG(0x0021,0x013f);      
#elif ID_AM==001
 LCD_WR_REG(0x0020,0x00ef);
 LCD_WR_REG(0x0021,0x013f);      
#elif ID_AM==010
 LCD_WR_REG(0x0020,0x0000);
 LCD_WR_REG(0x0021,0x013f);      
#elif ID_AM==011
 LCD_WR_REG(0x0020,0x0000);
 LCD_WR_REG(0x0021,0x013f);       
#elif ID_AM==100
 LCD_WR_REG(0x0020,0x00ef);
 LCD_WR_REG(0x0021,0x0000);      
#elif ID_AM==101  
 LCD_WR_REG(0x0020,0x00ef);
 LCD_WR_REG(0x0021,0x0000);      
#elif ID_AM==110
 LCD_WR_REG(0x0020,0x0000);
 LCD_WR_REG(0x0021,0x0000);      
#elif ID_AM==111
 LCD_WR_REG(0x0020,0x0000);
 LCD_WR_REG(0x0021,0x0000);         
#endif  
// ----------- Adjust the Gamma Curve ----------//
 LCD_WR_REG(0x0030,0x0007);
 LCD_WR_REG(0x0031,0x0707);
 LCD_WR_REG(0x0032,0x0107);
 LCD_WR_REG(0x0035,0x0206);
 LCD_WR_REG(0x0036,0x0406);
 LCD_WR_REG(0x0037,0x0101);
 LCD_WR_REG(0x0038,0x0101);
 LCD_WR_REG(0x0039,0x0207);
 LCD_WR_REG(0x003C,0x0504);
 LCD_WR_REG(0x003D,0x0806);
		 
//------------------ Set GRAM area ---------------//
 LCD_WR_REG(0x0050, 0x0000); // Horizontal GRAM Start Address
 LCD_WR_REG(0x0051, 0x00EF); // Horizontal GRAM End Address
 LCD_WR_REG(0x0052, 0x0000); // Vertical GRAM Start Address
 LCD_WR_REG(0x0053, 0x013F); // Vertical GRAM Start Address
 LCD_WR_REG(0x0060, 0x2700); // Gate Scan Line
 LCD_WR_REG(0x0061, 0x0001); // NDL,VLE, REV
 LCD_WR_REG(0x006A, 0x0000); // set scrolling line
//-------------- Partial Display Control ---------//
 LCD_WR_REG(0x80, 0x0000);
 LCD_WR_REG(0x81, 0x0000);
 LCD_WR_REG(0x82, 0x0000);
 LCD_WR_REG(0x83, 0x0000);
 LCD_WR_REG(0x84, 0x0000);
 LCD_WR_REG(0x85, 0x0000);
//-------------- Panel Control -------------------//
 LCD_WR_REG(0x90, 0x0010);
 LCD_WR_REG(0x92, 0x0600);
 LCD_WR_REG(0x93, 0x0003);
 LCD_WR_REG(0x95, 0x0110);
 LCD_WR_REG(0x97, 0x0000);
 LCD_WR_REG(0x98, 0x0000);		 
 LCD_WR_REG(0x07, 0x0173); // 262K color and display ON   0133         
 LCD_Clear(0);
 delay_ms(2000); // Delay 5ms		 
 LCD_BL_H();  
}

void LCD_Clear(unsigned int Color)
{
 unsigned long index = 0;  
 LCD_WR_REG(0x0020,0x0000);
 LCD_WR_REG(0x0021,00000);      
 LCD_WR_REG(0x0050,0x00);
 LCD_WR_REG(0x0051,239);
 LCD_WR_REG(0x0052,0x00);
 LCD_WR_REG(0x0053,319);
 LCD_WR_REG16(0x0022);  
 LCD_RD_H();
 LCD_RS_H();
 LCD_CS_L();   
 for(index = 0; index < 76800; index++)
 {
  // LCD_WR_DATA16(Color); 
  LCD_WR_L();
  DATA_LCD_PORT=Color>>8; 
  LCD_WR_H();
  LCD_WR_L();
  DATA_LCD_PORT=Color&0XFF; 
  LCD_WR_H();   
 } 
 LCD_CS_H();
}

void LCD_write_english(unsigned char data,unsigned int color,unsigned int xcolor)
{
 unsigned char avl,i,n;
 LCD_WR_REG16(0x0022);  
 for (i=0;i<16;i++)
 { 
  avl=pgm_read_byte( &english[data-32][i]);
  for (n=0;n<8;n++)
  {
   if(avl&0x80) LCD_WR_DATA16(color);
   else LCD_WR_DATA16(xcolor);           
   avl<<=1;
  }
 }
}

void LCD_write_english_string(unsigned int x,unsigned int y,/*unsigned*/ char *s,unsigned int color,unsigned int xcolor)
{
 unsigned int k=0;
 while (*s) 
 {
  Lcd_SetBox(y,x+k,15,8);
  LCD_write_english( *s,color,xcolor);
  k=k+8;
  s++;  
 }
}  

void LCD_write_SUM(unsigned int x,unsigned int y,unsigned char SUM,unsigned int color,unsigned int xcolor)
{
 unsigned char avl,i,n;
 Lcd_SetBox(y,x,15,8);
 LCD_WR_REG16(0x0022);  
 for (i=0;i<16;i++)
 { 
  avl=pgm_read_byte( &english[SUM+16][i]);
  for (n=0;n<8;n++)
  {
   if(avl&0x80) LCD_WR_DATA16(color);
   else LCD_WR_DATA16(xcolor);           
   avl<<=1;
  }
 }
}
void Lcd_SetBox(unsigned int xStart,unsigned int yStart,unsigned int xLong,unsigned int yLong)
{
  
#if ID_AM==000    
 LCD_SetCursor(xStart+xLong-1,312-yStart+yLong-1);

#elif ID_AM==001
 LCD_SetCursor(xStart+xLong-1,312-yStart+yLong-1);
     
#elif ID_AM==010
 LCD_SetCursor(xStart,312-yStart+yLong-1);
     
#elif ID_AM==011 
 LCD_SetCursor(xStart,312-yStart+yLong-1);
     
#elif ID_AM==100
 LCD_SetCursor(xStart+xLong-1,312-yStart);     
     
#elif ID_AM==101
 LCD_SetCursor(xStart+xLong-1,312-yStart);     
     
#elif ID_AM==110
 LCD_SetCursor(xStart,312-yStart); 
      
#elif ID_AM==111
 LCD_SetCursor(xStart,312-yStart);  
     
#endif
     
 LCD_WR_REG(0x0050,xStart);
 LCD_WR_REG(0x0051,xStart+xLong-1);
 LCD_WR_REG(0x0052,312-yStart);
 LCD_WR_REG(0x0053,312-yStart+yLong-1);
}

void LCD_SetCursor(unsigned int Xpos, unsigned int Ypos)
{
 // CLR_RS;
 LCD_WR_REG(0x20, Xpos);
 LCD_WR_REG(0x21, Ypos);
}

void  GUI_Point(unsigned int x, unsigned int y,unsigned int color) 
{
 LCD_WR_REG(0x0020,x);
 LCD_WR_REG(0x0021,y);
 //  LCD_WR_REG(0x0050,x);
 //  LCD_WR_REG(0x0051,x);
 //  LCD_WR_REG(0x0052,y);
 //  LCD_WR_REG(0x0053,y);
 LCD_WR_REG16(0x0022); 
 LCD_WR_DATA16(color);    
 }

void Test(void)
{       
 unsigned long n,i;
 LCD_WR_REG(0x0020,0x0000);
 LCD_WR_REG(0x0021,0x0000); 
 for(i=0;i<7;i++)          
 {                   
  LCD_WR_REG(0x0050,0x00);
  LCD_WR_REG(0x0051,239);
  LCD_WR_REG(0x0052,0x00);
  LCD_WR_REG(0x0053,319);
           
  LCD_WR_REG16(0x0022);       
  for(n=0;n<76800;n++)
  {                   
   if(i==0)LCD_WR_DATA16(BLUE);   
   if(i==1)LCD_WR_DATA16(RED); 
   if(i==2)LCD_WR_DATA16(GREEN);
   if(i==3)LCD_WR_DATA16(CYAN); 
   if(i==4)LCD_WR_DATA16(MAGENTA); 
   if(i==5)LCD_WR_DATA16(YELLOW);				  
   if(i==6)LCD_WR_DATA16(BLACK); 
  }    
  delay_ms(100);                   
 }       
}
