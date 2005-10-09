//Update Sep 30, 2005 23:50

#include <lcdlib.h>

void Toggle_Enable(void)
{
      Plcd |= EN;
      _delay_ms(1);
      Plcd &= ~EN;
}

void Set_RW(void)
{
      Plcd |= RW;
}

void Clear_RW(void)
{      
      Plcd &= ~RW;
}

void Set_RS(void)
{      
      Plcd |= RS;
}

void Clear_RS(void)
{      
      Plcd &= ~RS;
}

void LCD_init (void)
{
      unsigned char i; 
      unsigned char init_const[12] ={     0x03,0x03,0x03,
                                          0x02,
                                          0x02,0x0C,
                                          0x00,0x0C,
                                          0x00,0x01,
                                          0x00,0x06};
      Clear_RS();
      Clear_RW();
      for(i=0;i<12;i++)
      {
            Plcd=init_const[i];
            Toggle_Enable();
            _delay_ms(40);      
      }
      LCD_Send(0x0F,0);      
      LCD_Send(0x01,0);
}

// _DC=1 for Data
// _DC=0 for Command
void LCD_Send(char c, unsigned char _DC)
{
      unsigned char bufferH, bufferL;

      bufferH = c >> 4;
      bufferH = bufferH & 0x0F;      
      bufferL = c & 0x0F;
      if (_DC == 1)
      {      
            Plcd = bufferH;
            Set_RS();
            Clear_RW();
            Toggle_Enable();
            _delay_ms(1);      
         
            Plcd = bufferL;
            Set_RS();
            Clear_RW();   
            Toggle_Enable();
             _delay_ms(1);
      }
      else
      {
            Plcd = bufferH;
            Clear_RS();
            Clear_RW();
            Toggle_Enable();
            _delay_ms(1);      
         
            Plcd = bufferL;
            Clear_RS();
            Clear_RW();   
            Toggle_Enable();
             _delay_ms(1);      
      }
}

void LCD_Send_String(unsigned char *s)
{     unsigned char i;
	for( i=0; i < strlen(s);i++)
      {
            LCD_Send( s[i], 1);
      }
}
