#include <stdio.h>
void adjust()
{ 
   char stra[2];
   if(!PIND.4)
   { 
      pause=1;
      if(end<=98)
         end++;
      else
         end=0;
        
      if(up==1)
         countid=0;
      else
        countid=end;
   }   
   
   else if(!PIND.5)
   {      
      pause=1;
      if(end>=3)
         end--;   
      else        
         end=99;
   
      if(up==1)
         countid=0;
      else
         countid=end;
   } 
   
   lcd_gotoxy(10,0);
   sprintf(stra,"%2d",end);
   lcd_puts(stra);
         
}