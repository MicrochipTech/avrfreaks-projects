#include <avr/io.h>
#include <avr/signal.h>
#include <avr/iom16.h>
#include "lcd.c"
#define ADC_VREF_TYPE 0xC0 //internal 2.56

volatile void wait(unsigned long pt) {
  volatile unsigned char tp1;
  for(;pt>0;pt--) 
    for(tp1=255;tp1!=0;tp1--);
}

unsigned int read_adc(unsigned char adc_input){
  ADMUX=adc_input|ADC_VREF_TYPE; //Set channel for input
  ADCSRA=( (1<<ADEN) | (1<<ADSC) | (1<<ADIE) | (1<<ADPS1) | (1<<ADPS2) );//start conversion
  while(!(ADCSRA & 0x10)); //Wait for conversion to complete
  ADCSRA|=0x10;
  return ADCW;
}

unsigned char *temperature(unsigned char channel,unsigned int *maxt){
  unsigned int temp,temp_0,temp_1;
  double mytemp;
  char buf[10],tmp[10];
  mytemp=read_adc(channel);
  mytemp*=2.56/1.024;
  temp=mytemp;
  if(temp>*maxt) *maxt=temp;
  temp_0=temp/10;
  sprintf(buf,"%d",temp_0);
  temp_1=(temp%100)%10;
  sprintf(tmp,"%d",temp_1);
  strcat(buf,"."); strcat(buf,tmp);  
  return buf;
}

unsigned char *voltage(unsigned char channel,unsigned int *maxvol){
  unsigned int vol,vol_0,vol_1;
  double result;
  char buf[10],tmp[10];
  result=read_adc(channel);
  result*=2.56/10.24;
  result*=12/1.97;
  vol=result;
  if(vol>*maxvol) *maxvol=vol;
  vol_0=vol/100;
  sprintf(buf,"%d",vol_0);
  strcat(buf,".");
  vol_1=vol%100;
  if(vol_1<10) strcat(buf,"0");
  sprintf(tmp,"%d",vol_1);
  strcat(buf,tmp);
  return buf;
}

int main(void){
  char buf[10],tmp[10];
  unsigned int i,maxt1=0,maxt2=0,maxvol=0,max_0,max_1;
  double t;
  lcd_init(LCD_DISP_ON);
  lcd_clrscr();
  lcd_puts("Miernikoczujnik\nStart systemu...");
  DDRD=0xFF;
  for(i=0;i<3;i++){
    sbi(PORTD,7);
    wait(150);
    cbi(PORTD,7);
    wait(80);
  }
  lcd_clrscr();
  ADMUX = ADC_VREF_TYPE;
  ADCSRA = ( (1<<ADEN) | (1<<ADPS1) | (1<<ADPS2) );
  while(1){
    if(bit_is_clear(PINC,PC0)) maxt1=maxt2=maxvol=0;
    lcd_gotoxy(0,0);
    lcd_puts(temperature(PA0,&maxt1));
        max_0=maxt1/10;
	sprintf(buf,"%d",max_0);
	strcat(buf,".");
	max_1=maxt1%100; max_1%=10;
	sprintf(tmp,"%d",max_1);
	strcat(buf,tmp);
    lcd_gotoxy(0,1); 
    lcd_puts(buf);
    lcd_gotoxy(5,0);
    lcd_puts(temperature(PA1,&maxt2));
        max_0=maxt2/10;
	sprintf(buf,"%d",max_0);
	strcat(buf,".");
	max_1=maxt2%100; max_1%=10;
	sprintf(tmp,"%d",max_1);
	strcat(buf,tmp);
    lcd_gotoxy(5,1); 
    lcd_puts(buf);
    lcd_gotoxy(10,0);
    lcd_puts(voltage(PA2,&maxvol));
        max_0=maxvol/100;
	sprintf(buf,"%d",max_0);
	strcat(buf,".");
	max_1=maxvol%100;
	if(max_1<10) strcat(buf,"0");
	sprintf(tmp,"%d",max_1);
	strcat(buf,tmp);
    lcd_gotoxy(10,1);
    lcd_puts(buf);
    sbi(PORTD,7);
    wait(20);
    cbi(PORTD,7);
    wait(20);
  }
  return(0);
}//end_main
