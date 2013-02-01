void clr_line(int l,int s,int e)
{         
   int i;
   for(i=s;i<=e;i++)
   {
     lcd_gotoxy(i,l);
     lcd_putsf(" ");
   }
}