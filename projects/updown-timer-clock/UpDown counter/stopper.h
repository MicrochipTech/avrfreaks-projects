void stopper()
{  
   if (!PIND.2)
   {
      pause=1; 
      lcd_gotoxy(6,1);
      lcd_putsf("  0");
      lcd_gotoxy(15,0);
      lcd_putsf("S");      
      if(up==1)
         countid=0;
      else
         countid=end;
   }
}