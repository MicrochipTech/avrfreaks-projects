#include <pauser.h> 
#include <stopper.h>
#include <updown.h> 
#include <adjust.h>
#include <delay.h>
#include <stdio.h>

void count()
{ 
   char strc[3];     
   pauser();
   stopper();    
   updown(); 
   adjust();  
      
   delay_ms(100);         
      if(pause==0)
      {
            i=countid;
            lcd_gotoxy(6,1);
            lcd_putsf("   ");
            lcd_gotoxy(6,1); 
            if(up==1)
            {            
               sprintf(strc,"%3d",i++);
               lcd_puts(strc);                
               if(i>end)
               {
                  i=0;
                  pause=1;   
                  lcd_gotoxy(4,1);
                  lcd_putsf("Finished");
               }           
            }
            else if(up==0) 
            { 
               sprintf(strc,"%3d",i--); 
               lcd_puts(strc); 
               if(i<0)
               {
                  i=end;
                  pause=1;   
                  lcd_gotoxy(4,1);
                  lcd_putsf("Finished");
               }                                                                 
            }  
            countid=i;   
            delay_ms(900);
      }  
}