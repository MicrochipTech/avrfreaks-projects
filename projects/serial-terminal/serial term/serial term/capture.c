#include "capture.h"
void cap_init (void)
{
j=0;
i=0;
}
void do_states (char s)
{

           
                //if(up_BN==0)
                //////printf("it worked");            
              
                  
                state=bla;
                if(s==0x0a )
                state=crlf;
                if(s==0x08)
                state=bkspce;
                if(s==0x0d || s==0)
                state=ignor;
                if(s==0xff)
                state=scrollup;
                if(s==0xfe)
                state=scrolldown;
                if(s==0x7e)
                state=backlighttoggle;
                switch(state)
                { 
                        case backlighttoggle:
                        backlight=!backlight;
                        break;
                        
                        case crlf:
                        if(wasscroll==1)
                        {
                                //printf("wasscroll just got cleared by crlf");
                                j=jstore;
                                i=istore;
                                lcd_clear();
                                        
                                        if((j-3)>-1)
                                        {
                                                 lcd_puts(line[j-3]);      
                                        }
                                        else
                                        {
                                                 lcd_puts(line[buffersize+j-3]);                                               
                                        }
                                        lcd_gotoxy(0,1);               
                                        if((j-2)>-1)
                                        {
                                                 lcd_puts(line[j-2]);
                                        }
                                        else
                                        {
                                                 lcd_puts(line[buffersize+j-2]);                               
                                        }
                                        lcd_gotoxy(0,2);
                                        if((j-1)>-1)
                                        {
                                                 lcd_puts(line[j-1]);
                                        }
                                        else
                                        {
                                                 lcd_puts(line[buffersize+j-1]);                               
                                        }
                                                                 
                                        lcd_gotoxy(0,3);
                                        lcd_puts(line[j]);
                                        wasscroll=0;
                        }
                        
                        ////printf("CRLF\n\r");
                        for(x=20-i;x!=0;x--)                 //this just fills the remainng of line with spaces
                        {                                                                                                                                                                                                                                                                                                
                                line[j][i]=0x20;
                                i++;
                                ////printf("%i",i);   
                        }
                        i=0;
                        if (lineflag==3)
                        {
                                
                                
                                        lcd_clear();
                                        
                                        if((j-2)>-1)
                                        {
                                                 lcd_puts(line[j-2]);      
                                        }
                                        else
                                        {
                                                 lcd_puts(line[buffersize+j-2]);                                               
                                        }
                                        lcd_gotoxy(0,1);               
                                        if((j-1)>-1)
                                        {
                                                 lcd_puts(line[j-1]);
                                        }
                                        else
                                        {
                                                 lcd_puts(line[buffersize+j-1]);                               
                                        }
                                        lcd_gotoxy(0,2);
                                        lcd_puts(line[j]);                         
                                        lcd_gotoxy(0,3);
                                ////printf("crlf line 1\n\r'%s'\n\r'                '\n\r",line[j]);
                                ////printf("line number is %i\ni is %i\n\r",j,i);
                                if((j+1)<buffersize)
                                j++;
                                else
                                j=0;
                                lcd_gotoxy(0,3);
                                break;
                                
                        }
                        else 
                        
                        {
                                //lcd_clear();
                                ////printf("crlf line 0\n\r'%s'\n\r'                '\n\r",line[j]);
                                ////printf("line number is %i\ni is %i\n\r",j,i);
                                if((j+1)<buffersize)
                                j++;
                                else
                                j=0;
                                lineflag++;
                                lcd_gotoxy(i,lineflag);
                        } 
                              
                        break; 
                        
                        case bla:
                        if(wasscroll==1)
                        {
                                //printf("wasscroll just got cleared by crlf");
                                j=jstore;
                                i=istore;
                                lcd_clear();
                                        
                                        if((j-3)>-1)
                                        {
                                                 lcd_puts(line[j-3]);      
                                        }
                                        else
                                        {
                                                 lcd_puts(line[buffersize+j-3]);                                               
                                        }
                                        lcd_gotoxy(0,1);               
                                        if((j-2)>-1)
                                        {
                                                 lcd_puts(line[j-2]);
                                        }
                                        else
                                        {
                                                 lcd_puts(line[buffersize+j-2]);                               
                                        }
                                        lcd_gotoxy(0,2);
                                        if((j-1)>-1)
                                        {
                                                 lcd_puts(line[j-1]);
                                        }
                                        else
                                        {
                                                 lcd_puts(line[buffersize+j-1]);                               
                                        }
                                                                 
                                        lcd_gotoxy(0,3);
                                        lcd_puts(line[j]);
                                        wasscroll=0;
                        }
                        
                        if(s<0x20)
                        break;
                        if(i<20)
                        {       
                                line[j][i]=s;
                                ////printf("buffer was updated j=%i i=%i s=%c\n\r",j,i,s); 
                                lcd_gotoxy(i,lineflag);
                                lcd_putchar(s);
                                if(i==19)
                                {
                                if(lineflag!=3)
                                lcd_gotoxy(0,lineflag+1);
                                else
                                {
                                _lcd_ready();
                                _lcd_write_data(0x0c);
                                }
                                }
                        
                                
                                ////printf("buffer contains \n\r'%s'\n\r",line[j]);
                                i++;  
                                ////printf("%i",i);
                        }  
                        else
                        {       
                                if(lineflag!=3)
                                {
                                        if((j+1)<buffersize)
                                            j++;
                                        else
                                            j=0;
                                        ////printf("line down j is %i\n\r",j);
                                        
                                        lineflag++;
                                        //printf("%i",lineflag);
                                        lcd_gotoxy(0,lineflag);
                                        lcd_putchar(s);
                                        i=1; 
                                        line[j][0]=s;                                        
                                        break;
                                }
                                else
                                {      


                                
                                        lcd_clear();
                                        
                                        if((j-2)>-1)
                                        {
                                                 lcd_puts(line[j-2]);      
                                        }
                                        else
                                        {
                                                 lcd_puts(line[buffersize+j-2]);                                               
                                        }
                                        lcd_gotoxy(0,1);               
                                        if((j-1)>-1)
                                        {
                                                 lcd_puts(line[j-1]);
                                        }
                                        else
                                        {
                                                 lcd_puts(line[buffersize+j-1]);                               
                                        }
                                        lcd_gotoxy(0,2);
                                        lcd_puts(line[j]);                         
                                        lcd_gotoxy(0,3);
                                        
                                        
                                        ////printf("j is currently %i\n\r",j);
                                        ////printf("buffern contains \n\r%s\n\r",line[j]);
                                        
                                        if((j+1)<buffersize)
                                        j++;
                                        else
                                        j=0;
                                        lcd_putchar(s);
                                        line[j][0]=s;
                                        i=1;
                                        
                                        ////printf("line over flow \n%s\n\r",line[j]);
                                        ////printf("line number is %i\ni is %i\n\r",j,i);       
                                }
                        }
                        
                        break;
                        case bkspce:
                        if(wasscroll==1)
                        {
                                //printf("wasscroll just got cleared by crlf");
                                j=jstore;
                                i=istore;
                                lcd_clear();
                                        
                                        if((j-3)>-1)
                                        {
                                                 lcd_puts(line[j-3]);      
                                        }
                                        else
                                        {
                                                 lcd_puts(line[buffersize+j-3]);                                               
                                        }
                                        lcd_gotoxy(0,1);               
                                        if((j-2)>-1)
                                        {
                                                 lcd_puts(line[j-2]);
                                        }
                                        else
                                        {
                                                 lcd_puts(line[buffersize+j-2]);                               
                                        }
                                        lcd_gotoxy(0,2);
                                        if((j-1)>-1)
                                        {
                                                 lcd_puts(line[j-1]);
                                        }
                                        else
                                        {
                                                 lcd_puts(line[buffersize+j-1]);                               
                                        }
                                                                 
                                        lcd_gotoxy(0,3);
                                        lcd_puts(line[j]);
                                        wasscroll=0;
                        }
                        if(i!=0) //if you are not at the start of a line
                        {      
                                
                                i--;
                                
                                lcd_gotoxy(i,lineflag);
                                lcd_putchar(0x20);
                                lcd_gotoxy(i,lineflag);
                                line[j][i]=(0x20);
                        }
                        else
                        {     printf("%i",lineflag);          
                                        if(lineflag!=0)
                                        lineflag--;
                                        else
                                        {
                                                if(j>0)
                                                j--;
                                                else
                                                j=buffersize-1;
                                                lcd_puts(line[j]);
                                        }
                                        i=19;
                                        lcd_gotoxy(i,lineflag);
                                        lcd_putchar(0x20);
                                        line[j][i]=0x20;
                                        lcd_gotoxy(i,lineflag);      
                                        if(j>0)
                                        j--;
                                        else
                                        j=buffersize-1;
                                        
                                        
                                        
                                
                                
                        }
                        
                                break;

                        
                        case ignor:
                        break;
 
                        case scrolldown :
                        
                        
                                if (wasscroll==0)
                                { 
                                //printf("wasscroll just got set");
                                      wasscroll=1;      
                                
                                        istore=i;
                                        jstore=j; 
                                        for(x=20-i;x!=0;x--)                 //this just fills the remainng of line with spaces
                        {                                                                                                                                                                                                                                                                                                
                                line[j][i]=0x20;
                                i++;
                                ////printf("%i",i);   
                        }

                                }
                                        
                                
                            if((j+1)<buffersize)
                                        j++;
                            else
                                        j=0;
                             lcd_clear();
                             
                                //printf("\n\r%s \n\r",line[j-1]);
                                       lcd_clear();
                                        
                                        if((j-3)>-1)
                                        {
                                                 lcd_puts(line[j-3]);      
                                        }
                                        else
                                        {
                                                 lcd_puts(line[buffersize+j-3]);                                               
                                        }
                                        lcd_gotoxy(0,1);               
                                        if((j-2)>-1)
                                        {
                                                 lcd_puts(line[j-2]);
                                        }
                                        else
                                        {
                                                 lcd_puts(line[buffersize+j-2]);                               
                                        }
                                        lcd_gotoxy(0,2);
                                        if((j-1)>-1)
                                        {
                                                 lcd_puts(line[j-1]);
                                        }
                                        else
                                        {
                                                 lcd_puts(line[buffersize+j-1]);                               
                                        }
                                                                 
                                        lcd_gotoxy(0,3);
                                        lcd_puts(line[j]);
                             //printf("%s \n\r",line[j]);
                             //printf("j is %i\n\r",j);
                             break;
                             
                             
                             case scrollup:
                        
                        
                        
                                if (wasscroll==0)
                                { 
                                //printf("wasscroll just got set");
                                      wasscroll=1;      
                                
                                        istore=i;
                                        jstore=j;
                                        for(x=20-i;x!=0;x--)                 //this just fills the remainng of line with spaces
                        {                                                                                                                                                                                                                                                                                                
                                line[j][i]=0x20;
                                i++;
                                ////printf("%i",i);   
                        }
                                }
                                        
                                
                             if(j>0)   
                                    j--;
                             else
                                   j=buffersize-1;
                                    
                             lcd_clear();
                                        
                                        if((j-3)>-1)
                                        {
                                                 lcd_puts(line[j-3]);      
                                        }
                                        else
                                        {
                                                 lcd_puts(line[buffersize+j-3]);                                               
                                        }
                                        lcd_gotoxy(0,1);               
                                        if((j-2)>-1)
                                        {
                                                 lcd_puts(line[j-2]);
                                        }
                                        else
                                        {
                                                 lcd_puts(line[buffersize+j-2]);                               
                                        }
                                        lcd_gotoxy(0,2);
                                        if((j-1)>-1)
                                        {
                                                 lcd_puts(line[j-1]);
                                        }
                                        else
                                        {
                                                 lcd_puts(line[buffersize+j-1]);                               
                                        }
                                                                 
                                        lcd_gotoxy(0,3);
                                        lcd_puts(line[j]);
                             //printf("%s \n\r",line[j]);
                             //printf("j is %i\n\r",j);
                             break;        
                        default:
                        break;
                }
                        
             
      
      

      
}