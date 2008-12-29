////////////////////////////////// CodeVision Avr  ///////////////////////////
/// in the name of god ; 
/// Pasargad Semiconductor Co;
/// date :  1387/9/23;
/// project: 7-seg ;
/// writter : S.hashem Yaghobnejad ;
///////////////////////////////////////////////////////////////////////////////////
#include<mega32.h>
#include<delay.h>
#include<math.h>
char cal(char);
char init_configuration(void);
///////////////////////////////////////////// var
                                                      
int ihez,isad,idah,iyek,i;
float fhez,fsad,fdah,fyek,count;    
char ss_out;
///////////// end var . 
interrupt[12]
void tc0_ovf_isr(void)
{                                    
//if(PIND.2==0)
i++; 
delay_ms(2);


count=i;
ihez=0,isad=0,idah=0,iyek=0;
fhez=0,fsad=0,fdah=0,fyek=0;    
        fhez=(count/1000);
        fhez=(count/1000)-ihez; 
        ihez=fhez;
        fhez-=ihez;
        PORTB=0X08;
        PORTA=cal(ihez);            

        isad=fhez*10;
        fsad=(fhez*10)-isad;
        PORTB=0X04;
        PORTA=cal(isad); 
  
        idah=fsad*10;  
        fdah=(fsad*10)-idah;
        PORTB=0X02;
        PORTA=cal(idah);
  
        iyek=ceil(fdah*10);  
        PORTB=0X01;
        PORTA=cal(iyek); 
  

       
}
interrupt[2]
void Ext_int0_isr(void)
{      
                         
//while(PIND.2==0);
i++;



// GICR=0X00;
// GIFR=0X00;   

       
    

}
void main()
{                     
if(!init_configuration())
{
        while(1)        
        {
         PORTA=0X00;
         PORTB=0X00;
         PORTC=0X00;
         PORTD=0X01;
        }
}
while(1)
{





}    // END OF WHILE

}
                                             





char cal(char in)
{
        switch (in)
        {
        case 0:
        ss_out=0xc0;
        break;
        case 1:
        ss_out=0xf9;
        break;
        case 2:
        ss_out=0xa4;
        break;
        case 3:
        ss_out=0xb0;
        break;
        case 4:
        ss_out=0x99;
        break;
        case 5:
        ss_out=0x92;
        break;
        case 6:
        ss_out=0x82;
        break;
        case 7:
        ss_out=0xf8;
        break;
        case 8:
        ss_out=0x80;
        break;
        case 9:
        ss_out=0x90;
        break;

    }
return ss_out;
}


char init_configuration(void)
{
DDRA=0XFF;
PORTA=0XFF;
DDRB=0XFF;
DDRD=0X00;
PORTD.2=1;
PORTB=0X00;
TCCR0=0X02;
TIMSK=0X01;
GICR=0X00;
GIFR=0X00;
#asm("sei")
return 1;
}
////////////////////////////// end program.