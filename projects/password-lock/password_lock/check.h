#include <clr_line.h>
void check()
{      
   int i; 
   int true=1;    
   
   for(i=cur;i<16;i++)
      pw[i]=space[0];
      
   for(i=0;i<16;i++)
   {
      if(strcmp(pw[i],opw[i])!=0)
         true=0;
   }     
   lcd_gotoxy(0,1);
   if(true==1)          
   {          
      lcd_gotoxy(0,0);     
      lcd_putsf("    *Welcome*    ");
      lcd_gotoxy(0,1);
      lcd_putsf("    *********    "); 
      mode=1;
      delay_ms(900);
      PORTB.0=1;
      lcd_clear();
      lcd_gotoxy(0,0);
   }
   else
   {     
      lcd_putsf("Wrong passwprd");
      delay_ms(1000);          
      clr_line(1,0,15);
      cur=0;
   } 
   delay_ms(100);
}