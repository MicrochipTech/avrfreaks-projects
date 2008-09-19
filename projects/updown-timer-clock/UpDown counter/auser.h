#include <vars.h>
void pauser()
{
   if(!PIND.1)
   {
      pause=1;
      lcd_gotoxy(15,0);
      lcd_putsf("P");
   }
   else if(!PIND.0)
   {   
      pause=0;
      lcd_gotoxy(15,0);
      lcd_putsf(" ");
      lcd_gotoxy(4,1);
      lcd_putsf("        ");
   }
}