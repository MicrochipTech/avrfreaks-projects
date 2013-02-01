void disp_mail()
{  
   int i;
   for(i=j;i<=22;i++)
   {
      lcd_gotoxy((i-j),1);
      lcd_putchar(mail[i]);
   }
}