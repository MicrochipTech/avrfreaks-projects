#include <disp_mail.h>
void about()
{ 
   
   if(mode==3)
   {
   char *str[11]={"C_","R_","E_","A_","T_","E_","D_"," _","B_","Y_",":"};
   char name[16]={"OSAMA SALAH    "};
   int i;
   lcd_clear();
   
   for(i=0;i<3;i++)
   {
      lcd_gotoxy(0,0);
      lcd_putchar('_');
      delay_ms(300);
      lcd_gotoxy(0,0);
      lcd_putchar(' ');   
      delay_ms(300);
   }
   
   for(i=0;i<11;i++)
   {
      lcd_gotoxy(i,0);
      lcd_puts(str[i]);
      delay_ms(200);
   }
   
   for(i=15;i>=0;i--)
   {
      lcd_gotoxy(i,1);
      lcd_puts(name);
      delay_ms(75);
   } 
    
   delay_ms(1000);
   lcd_gotoxy(0,0);
   lcd_puts(name);
   clr_line(1,0,16); 
   
   delay_ms(500);
   
   for(i=15;i>=0;i--)
   {
      lcd_gotoxy(i,1);
      lcd_puts(mail);
      delay_ms(75);
   }
   
   while(mode==3)
   {
      scanner();
      disp_mail();
      if(mode!=3)
         break;
      delay_ms(10);
   }
   }
}