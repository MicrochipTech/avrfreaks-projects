# LCD header file for atmega16

Uploaded by sirius5 on 2012-08-29 15:02:00 (rating 0 out of 5)

## Summary

#define F\_CPU 12000000  

#include  

#include


#define dataport PORTC // LCD data port  

#define commport PORTD  

#define en PD7 // enable signal  

#define wr PD6 // read/write signal  

#define rs PD5 // register select signal  

#define LINE1 0x80  

#define LINE2 0xC0


int lcd\_init(void)  

{  

 DDRC=0xff;  

 DDRD=0xE0;  

 wrcomm(0x38);  

 wrcomm(0x01); //turn on display and cursor  

 wrcomm(0x0E);//cursor line 1 postn 1  

 wrcomm(0x80);  

 return 1;  

}


int wrcomm(unsigned int data)  

{  

 dataport=data;  

 \_delay\_ms(10);  

 commport &= ~(1 << rs);  

 commport &= ~(1 << wr);  

 commport |= (1 << en);  

 commport &= ~(1 << en);  

 \_delay\_ms(10);  

 return 1;  

}


int wrdata(unsigned int data)  

{  

 dataport=data;  

 \_delay\_ms(10);  

 commport |= (1 << rs);  

 commport &= ~(1 << wr);  

 commport |= (1 << en);  

 commport &= ~(1 << en);  

 \_delay\_ms(10);


 return 1;


}


int lcd\_putchar(unsigned char x)  

{  

 wrdata(x);  

 return 1;  

}


void lcd\_gotoxy1(unsigned char pos)  

{  

 wrcomm(LINE1+pos);  

}


void lcd\_gotoxy2(unsigned char pos)  

{  

 wrcomm(LINE1+pos);  

}


void lcd\_char(unsigned char single)  

{  

 int i;  

 if(single =='\t')  

 {  

 for(i =0 ; i<2 ; i++)  

 {  

 lcd\_putchar(single);  

 }  

 }  

 else if(single=='\n')  

 {  

 lcd\_gotoxy2(0);  

 }  

 else  

 {  

 lcd\_putchar(single);  

 }


}


void lcd\_showvalue(unsigned int num) //prints the decimal 3digit value of num  

{  

 unsigned char H=0,T=0,O=0;  

 H=num/100;  

 T=(num - (H*100))/10;  

 O=(num - (H*100) - (T*10));


 lcd\_char(H+48);  

 lcd\_char(T+48);  

 lcd\_char(O+48);  

}

## Compilers

- WinAVR (GNU GCC)

## Tags

- General Lib.functions
- WinAVR (GNU GCC)
