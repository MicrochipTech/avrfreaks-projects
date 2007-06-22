
void matrix_key_init(void);
void matrix_key_chk(void);
unsigned char chk_column1(void); 
unsigned char chk_column2(void); 
unsigned char chk_column3(void);
void key2lcd(void); 

unsigned char i;

void matrix_key_init(void)
{
	PORTA=0xE0;
	DDRA=0xE0;
}

void matrix_key_chk(void)
{             
	i=chk_column1();
	key2lcd();
	i=chk_column2(); 
	key2lcd();
	i=chk_column3();
	key2lcd();	
}

unsigned char chk_column1(void)
{
	//checking Column 1 keys.....
	PORTA=~(0x40);          
	i=(PINA & 0x1E);	 	//0b00011110
	if (i!=0x1E)
	{
		switch (i)
		{
		case 0x1C:
			return('0');
			break;
		case 0x1A:
			return('7');
			break;
		case 0x16:
			return('4');
			break;
		case 0x0E:
			return('1');
			break;
		default:
			return ('x');
		}
	}
	return ('x');    
} 
	
unsigned char chk_column2(void)
{
	//checking Column 2 keys.....
	PORTA=~(0x20);          
	i=(PINA & 0x1E);	 	//0b00011110
	if (i!=0x1E)
	{
		switch (i)
		{
		case 0x1C:
			lcd_clr();
			//lcd_delay1s();  
			//lcd_delay1s();
			return('x');
			break;
		case 0x1A:
			return('8');
			break;
		case 0x16:
			return('5');
			break;
		case 0x0E:
			return('2');
			break;
		default:
			return ('x');
		}
	}  
	return('x');   
}	

unsigned char chk_column3(void) 
{
	//checking Column 3 keys.....
	PORTA=~(0x80);          
	i=(PINA & 0x1E);	 	//0b00011110
	if (i!=0x1E)
	{
		switch (i)
		{
		case 0x1C:
			return('s');
			break;
		case 0x1A:
			return('9');
			break;
		case 0x16:
			return('6');
			break;
		case 0x0E: 
			return('3');
			break;
		default:
			return ('x');
		}
	} 
	return('x');  
}	
	     
void key2lcd(void)
{
	if (i!='x')
	{
		lcd_char(i);
		printf("%x",i);
		lcd_delay1s();  
		lcd_delay1s();	
	}	
}	

