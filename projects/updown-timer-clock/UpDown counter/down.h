void updown()
{  
   if(!PIND.3)
   {   
      pause=1;
      if(up==1)
      {    
         countid=end;
         up=0;
         lcd_gotoxy(0,0);
         lcd_putsf("Down");
      }
      else
      {
         countid=0;
         up=1;
         lcd_gotoxy(0,0);
         lcd_putsf(" Up ");
      }
   }    
}