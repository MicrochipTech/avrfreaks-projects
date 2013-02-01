void menu()
{  

   if(page>2)
      page=2; 

   delay_ms(100);   
   lcd_gotoxy(0,0);
   if(page==1)
   {  
      lcd_putsf("1.Alter password");
      lcd_gotoxy(0,1);
      lcd_putsf("2.Sign out    1>");
   }
   else if(page==2)
   {
      lcd_putsf("3.About         "); 
      lcd_gotoxy(0,1);
      lcd_putsf("             <2 ");           
   }    
}