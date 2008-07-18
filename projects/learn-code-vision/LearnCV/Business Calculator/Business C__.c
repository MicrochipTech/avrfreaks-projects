#include <mega8535.h>
	#ifndef __SLEEP_DEFINED__
	#define __SLEEP_DEFINED__
	.EQU __se_bit=0x40
	.EQU __sm_mask=0xB0
	.EQU __sm_powerdown=0x20
	.EQU __sm_powersave=0x30
	.EQU __sm_standby=0xA0
	.EQU __sm_ext_standby=0xB0
	.EQU __sm_adc_noise_red=0x10
	.SET power_ctrl_reg=mcucr
	#endif


#include <stdio.h>

//flash char fca[]="Scorpion Controls";

//flash char *fptr;
eeprom unsigned char *eepptr;

unsigned char a,b,c;
//pragma w-

//pragma w+

void add(void)
{
        printf("Enter the 1st digit to ADD");
        scanf("%x",&a);
        *++eepptr = a;
        printf("Enter the 2nd digit to ADD");
        scanf("%x",&b);
        *++eepptr = b;
        c=*--eepptr + *++eepptr;
        *++eepptr = c;
        printf("Added Result is:%d",c);
        a=b=c=0;
}
void sub(void)
{
        printf("Enter two digit to SUB");
        scanf("%x%x",&a,&b);
        c=a-b;
        printf("Subtracted Result is:%d",c);

}
void mul(void)
{
        printf("Enter two digit to MUL");
        scanf("%x%x",&a,&b);
        c=a*b;
        printf("Multiplied Result is:%d",c);
}
void div(void)
{
        printf("Enter two digit to DIV");
        scanf("%x%x",&a,&b);
        c=a/b;
        printf("Divided Result is:%d",c);
}
void wreep(unsigned char);


void (*funp[5])(void)= {add,sub,mul,div};


void main(void)
{
        void (*fp) (void);
        unsigned char FunSel;

        UBRRL=0x0C;
        UBRRH=0x00;
        UCSRA=0x02;
        UCSRB=0x18;
        UCSRC=0x86;

        while(1)
        {
                printf("Select any one operation representing it's serial no:");
                printf("(1)ADD(2)SUB(3)MUL(4)DIV");
                scanf("%x",&FunSel);
                wreep(FunSel);

                if((FunSel>0x00) && (FunSel<0x05))
                {
                        FunSel=FunSel-0x01;
                        fp=funp[FunSel];
                        (*fp)();
                }
                printf("End");
        }

}
void wreep(unsigned char data)
{
   *eepptr = data;
}


