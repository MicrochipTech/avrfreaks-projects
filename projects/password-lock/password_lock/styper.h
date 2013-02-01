#include <string.h>
#include <password.h>

void styper(int x)
{     
      int i;
      int arr[2];
      int port;
      
      if(mode==0 || mode==2)
      {  
      port=PINA;        
      lcd_gotoxy(cur,row);
      lcd_puts(array[small][x][index]);
      lcd_putchar('_');
      delay_ms(100);                  
      for(i=1;i<=900;i++)
      {      
         arr[0]=PINA;
         delay_ms(1);
         arr[1]=PINA;
         if((arr[0]!=arr[1])&&(PINA==port))
         {
            i=1; 
            index++;         
            if(index>4)
               index=0;
            lcd_gotoxy(cur,row);            
            lcd_puts(array[small][x][index]);           
         }           
      }
      pw[cur]=array[small][x][index];
      lcd_gotoxy(cur,row);//To display '*'s            
      lcd_putchar('*');                  
      cur++;
      index=0;
      }
}