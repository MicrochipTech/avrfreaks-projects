
#pragma used+
sfrb TWBR=0;
sfrb TWSR=1;
sfrb TWAR=2;
sfrb TWDR=3;
sfrb ADCL=4;
sfrb ADCH=5;
sfrw ADCW=4;      
sfrb ADCSRA=6;
sfrb ADMUX=7;
sfrb ACSR=8;
sfrb UBRRL=9;
sfrb UCSRB=0xa;
sfrb UCSRA=0xb;
sfrb UDR=0xc;
sfrb SPCR=0xd;
sfrb SPSR=0xe;
sfrb SPDR=0xf;
sfrb PIND=0x10;
sfrb DDRD=0x11;
sfrb PORTD=0x12;
sfrb PINC=0x13;
sfrb DDRC=0x14;
sfrb PORTC=0x15;
sfrb PINB=0x16;
sfrb DDRB=0x17;
sfrb PORTB=0x18;
sfrb PINA=0x19;
sfrb DDRA=0x1a;
sfrb PORTA=0x1b;
sfrb EECR=0x1c;
sfrb EEDR=0x1d;
sfrb EEARL=0x1e;
sfrb EEARH=0x1f;
sfrw EEAR=0x1e;   
sfrb UBRRH=0x20;
sfrb UCSRC=0X20;
sfrb WDTCR=0x21;
sfrb ASSR=0x22;
sfrb OCR2=0x23;
sfrb TCNT2=0x24;
sfrb TCCR2=0x25;
sfrb ICR1L=0x26;
sfrb ICR1H=0x27;
sfrb OCR1BL=0x28;
sfrb OCR1BH=0x29;
sfrw OCR1B=0x28;  
sfrb OCR1AL=0x2a;
sfrb OCR1AH=0x2b;
sfrw OCR1A=0x2a;  
sfrb TCNT1L=0x2c;
sfrb TCNT1H=0x2d;
sfrw TCNT1=0x2c;  
sfrb TCCR1B=0x2e;
sfrb TCCR1A=0x2f;
sfrb SFIOR=0x30;
sfrb OSCCAL=0x31;
sfrb OCDR=0x31;
sfrb TCNT0=0x32;
sfrb TCCR0=0x33;
sfrb MCUCSR=0x34;
sfrb MCUCR=0x35;
sfrb TWCR=0x36;
sfrb SPMCR=0x37;
sfrb TIFR=0x38;
sfrb TIMSK=0x39;
sfrb GIFR=0x3a;
sfrb GICR=0x3b;
sfrb OCR0=0X3c;
sfrb SPL=0x3d;
sfrb SPH=0x3e;
sfrb SREG=0x3f;
#pragma used-

#asm
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
#endasm

typedef char *va_list;

#pragma used+

char getchar(void);
void putchar(char c);
void puts(char *str);
void putsf(char flash *str);

char *gets(char *str,unsigned int len);

void printf(char flash *fmtstr,...);
void sprintf(char *str, char flash *fmtstr,...);
void snprintf(char *str, unsigned int size, char flash *fmtstr,...);
void vprintf (char flash * fmtstr, va_list argptr);
void vsprintf (char *str, char flash * fmtstr, va_list argptr);
void vsnprintf (char *str, unsigned int size, char flash * fmtstr, va_list argptr);
signed char scanf(char flash *fmtstr,...);
signed char sscanf(char *str, char flash *fmtstr,...);

#pragma used-

#pragma library stdio.lib

#pragma used+

void delay_us(unsigned int n);
void delay_ms(unsigned int n);

#pragma used-

#asm
   .equ __lcd_port=0x15 ;PORTC
#endasm

#pragma used+

void _lcd_ready(void);
void _lcd_write_data(unsigned char data);

void lcd_write_byte(unsigned char addr, unsigned char data);

unsigned char lcd_read_byte(unsigned char addr);

void lcd_gotoxy(unsigned char x, unsigned char y);

void lcd_clear(void);
void lcd_putchar(char c);

void lcd_puts(char *str);

void lcd_putsf(char flash *str);

unsigned char lcd_init(unsigned char lcd_columns);

#pragma used-
#pragma library lcd.lib

sfrb PORTA=0x1b;
sfrb PINA=0x19;

unsigned char TimeOut,AckBit,buf[33];  
unsigned int DataTempSHT,DataRHSHT,DataRead;
float RH,Temp; 

void StartSignal (void)
{        
unsigned char DDRATemp;
DDRATemp = DDRA;    
DDRA |= 0x01;       
PORTA.0 = 1;
PORTA.1 = 0;
PORTA.1 = 1;           
PORTA.0 = 0;
PORTA.1 = 0;
PORTA.1 = 1;           
PORTA.0 = 1;
PORTA.1 = 0;           
DDRA = DDRATemp;        
}

void ResetSHT (void)
{
unsigned char i,DDRATemp;
DDRATemp = DDRA;
DDRA |= 0x01;
PORTA.0 = 1;
PORTA.1 = 0;
for (i=0; i<=8; i++)
{
PORTA.1 = 1;     
PORTA.1 = 0;
}
StartSignal();        
DDRA = DDRATemp;
}

void SHTWait (void)
{
unsigned char i,DDRATemp;
DDRATemp = DDRA;
DDRA |= 0x01;
PORTA.0=1;          
DDRA &= 0xFE;
for (i=0; i<250; i++)
{
TimeOut=PINA.0;        
if (TimeOut==0) goto ExitSHT_Wait;
delay_ms(1);
}       
ExitSHT_Wait:        
DDRA = DDRATemp;  
} 

void SHTWriteByte (unsigned char data)
{
unsigned char i,DDRATemp;
DDRATemp = DDRA;
DDRA |= 0x01;
for (i=0; i<8; i++)
{
if ((data>>7)==1) PORTA.0 = 1;    
else PORTA.0 = 0;
PORTA.1 = 1;                 
PORTA.1 = 0;
data <<= 1;             
}
PORTA.0 = 1;                  
PORTA.1 = 1;
DDRA &= 0xFE;
AckBit = PINA.0;             
PORTA.1 = 0;
DDRA = DDRATemp; 
}

void SHTReadByte (void)
{
unsigned char i,DDRATemp;
DataRead = 0x00;
DDRATemp = DDRA;
DDRA |= 0x01;
PORTA.0 = 1;          
DDRA &= 0xFE;
for (i=0; i<8; i++)
{
DataRead<<=1;
PORTA.1 = 1;
DataRead |= PINA.0;  
PORTA.1 = 0;
}
DDRA |= 0x01;    
if (AckBit==1) PORTA.0 = 1;   
else PORTA.0 = 0;             
PORTA.1 = 1;
PORTA.1 = 0;
PORTA.0 = 1;          
DDRA = DDRATemp;
}

void SHTReadTemp (void)
{        
StartSignal();
SHTWriteByte(0x03);    		
if (AckBit==0)
{
SHTWait();         		
if (TimeOut==0)
{
AckBit=0;                    
SHTReadByte();              
DataTempSHT = DataRead;
DataTempSHT <<= 8;
AckBit=1;                   
SHTReadByte();
DataTempSHT |= DataRead;   

DataRead = DataTempSHT;            
}
}
}

void SHTReadHumidity (void)
{
StartSignal();
SHTWriteByte(0x05);   
if (AckBit==0)
{
SHTWait();
if (TimeOut==0)
{
AckBit=0;
SHTReadByte();
DataRHSHT = DataRead;
DataRHSHT <<= 8;
AckBit=1;
SHTReadByte();
DataRHSHT |= DataRead;

DataRead = DataRHSHT;
}
}
}

void main(void)
{

PORTA=0x00;
DDRA=0x02;

PORTB=0x00;
DDRB=0x08;

PORTC=0x00;
DDRC=0xFF;

PORTD=0x00;
DDRD=0x04;

TCCR0=0x71;
TCNT0=0x00;
OCR0=0x00;

TCCR1A=0x00;
TCCR1B=0x00;
TCNT1H=0x00;
TCNT1L=0x00;
ICR1H=0x00;
ICR1L=0x00;
OCR1AH=0x00;
OCR1AL=0x00;
OCR1BH=0x00;
OCR1BL=0x00;

ASSR=0x00;
TCCR2=0x00;
TCNT2=0x00;
OCR2=0x00;

MCUCR=0x00;
MCUCSR=0x00;

TIMSK=0x00;

ACSR=0x80;
SFIOR=0x00;

lcd_init(16);
delay_ms(1000);
ResetSHT();

while (1)
{

SHTReadHumidity();
RH=((float)(DataRead*0.0405)-(DataRead*DataRead*0.0000028)-4);
ResetSHT();
SHTReadTemp();
Temp=((float)(DataRead-4000))/100;
ResetSHT();
lcd_clear();
lcd_gotoxy (0,0);
lcd_putsf(" RH : Temp");
lcd_gotoxy(0,1); 
sprintf(buf,"%0.2f %0.2f",RH,Temp);
lcd_puts(buf);
delay_ms(1000);

};
}
