#include <vars.h>
#include <stdio.h>
#include <styper.h>
#include <check.h>
#include <sc.h>

void scanner()
{  
   int i; 
    
   // checking for first row (0,x)
   PORTA=0xFE;
   delay_ms(1);
   
   // buttons arranged from upper left side of pad
   
   if(PINA==0x7E)//button (0,0)
   {          
      styper(0);
      if(mode==1)
      {
         mode=2;
         cur=0;
         lcd_gotoxy(0,1); 
         lcd_putsf("_               ");
         lcd_gotoxy(0,0);
         lcd_putsf("New Password:   ");         
         delay_ms(50);
      }
   }   

   if(PINA==0xBE)//button (0,1)
   { 
      styper(1);
      if(mode==1)
      {
         mode=0;
         PORTB.0=0;
         *pw=*rem_pw;
         cur=0;
         lcd_clear();
         delay_ms(50);
         lcd_gotoxy(0,1);
         lcd_putchar('_');
      }
   }
        
   if(PINA==0xDE)//button (0,2)
   { 
      styper(2);
      if(mode==1)
         mode=3;     
   }
   
   if(PINA==0xEE)//button (0,3)
   { 
      styper(3);
   }
   
   // checking for second row (1,x)
   PORTA=0xFD;
   
   // buttons arranged from upper left side of pad
         
   delay_ms(1);
   if(PINA==0x7D)//button (1,0)
   {          
      styper(4);
   }   

   if(PINA==0xBD)//button (1,1)
   { 
      styper(5);
   }
        
   if(PINA==0xDD)//button (1,2)
   { 
      styper(6);
   }
   
   if(PINA==0xED)//button (1,3)
   { 
      styper(7);
   }
   
      
  
   // checking for third row (2,x)
   PORTA=0xFB;
   
   // buttons arranged from upper left side of pad
         
   delay_ms(1);
   if(PINA==0x7B) //button (2,0)
   { 
      styper(8);         
   }   

   if(PINA==0xBB)//button (2,1)
   { 
      styper(9);
   }
        
   if(PINA==0xDB && cur>0 &&((mode==0)||(mode==2)))//button (2,2)
   {   
      lcd_gotoxy(cur,row);
      lcd_putsf(" ");
      lcd_gotoxy(--cur,row);
      lcd_putchar('_');
      delay_ms(120);
      lcd_gotoxy(cur,row);
   }
   
   if(PINA==0xEB)//button (2,3)
   {     
      if(mode==0)
         check();
      
      else if(mode==2)
      {
         for(i=0;i<cur;i++)
            strcpy(opw[i],pw[i]);
         
         lcd_gotoxy(0,0);
         lcd_putsf("Password changed");
         lcd_gotoxy(0,1);
         lcd_putsf("  successfully  "); 
         PORTB.0=0;
         length=cur;
         for(i=cur;i<16;i++)
            strcpy(opw[i],space[0]); 
            
         for(i=0;i<16;i++)      
            nvol_pw[i] = opw[i][0]; 

         cur=0;
         mode=0;
         /*for(i=0;i<16;i++)
            pw[i]=i;*/
         delay_ms(1000);
         lcd_gotoxy(0,1);
         lcd_putsf("_                ");
      }
      
      else if(mode==3)
      mode=1;
   }
   
   // checking for fourth row (3,x)
   PORTA=0xF7;
   delay_ms(1);
   
   // buttons arranged from upper left side of pad
   
   if(PINA==0x77)//button (3,0)
   {          
      sc();
   }   

   if(PINA==0xB7)//button (3,1)
   { 
      styper(10);
   }
        
   if(PINA==0xD7)//button (3,2)
   { 
      if(mode==1 && page>1)
      {
         page--;
         delay_ms(100);
      }
      else if(mode==3 && j>=1)
      {
         j--;
         delay_ms(120);
      }
      
   }
   
   if(PINA==0xE7)//button (3,3)
   {    
      if(mode==1)
      {
         page++;
         delay_ms(100);
      } 
      else if(mode==3 && j<=5)
      {
         j++;
         delay_ms(120);
      }
   }
}