#line 1 "C:/Users/HASAN/Desktop/my code/remote.c"
#line 49 "C:/Users/HASAN/Desktop/my code/remote.c"
sbit LCD_RS at PORTA2_bit;
sbit LCD_EN at PORTA3_bit;
sbit LCD_D4 at PORTA4_bit;
sbit LCD_D5 at PORTA5_bit;
sbit LCD_D6 at PORTA6_bit;
sbit LCD_D7 at PORTA7_bit;

sbit LCD_RS_Direction at DDA2_bit;
sbit LCD_EN_Direction at DDA3_bit;
sbit LCD_D4_Direction at DDA4_bit;
sbit LCD_D5_Direction at DDA5_bit;
sbit LCD_D6_Direction at DDA6_bit;
sbit LCD_D7_Direction at DDA7_bit;





volatile unsigned char IrData[4];
volatile unsigned int Time = 0;
volatile unsigned int TempTime;
volatile unsigned char State;
volatile unsigned char Edge;
volatile unsigned char BitNo;
volatile unsigned char ByteNo;
volatile unsigned char Data_receive = 0;
char txt[7];



void RemoteInit();
void ResetIR();
unsigned char GetRemoteCmd(char wait);








void Timer0_COMP_Interrupt() iv IVT_ADDR_TIMER0_COMP
{
 Time++;
}
void Ext_Interrupt() iv IVT_ADDR_INT0
{
 GICR&=(~(1<<INT0));
 SREG_I_bit = 1;

 TempTime = Time;
 Time=0;
 TCNT0=0;
 switch(State)
 {
 case  0 :
 {
 if(Edge)
 {

 if((TempTime>800) && (TempTime<990))
 {

 State= 1 ;

 MCUCR&=(~((1<<ISC01)|(1<<ISC00)));
 MCUCR|=(1<<ISC01);
 Edge=0;
 }
 else
 {
#line 125 "C:/Users/HASAN/Desktop/my code/remote.c"
 }
 }
 else
 {

 MCUCR|=((1<<ISC01)|(1<<ISC00));
 Edge=1;
 }
 break;
 }
 case  1 :
 {
 if((TempTime>400) && (TempTime<500))
 {

 State= 3 ;
 BitNo=0;
 ByteNo=0;
 MCUCR|=((1<<ISC01)|(1<<ISC00));
 Edge=1;

 }
 else if((TempTime>200) && (TempTime<245))
 {

 }
 else
 {
#line 158 "C:/Users/HASAN/Desktop/my code/remote.c"
 }
 break;
 }
 case  3 :
 {
 if(Edge)
 {

 if((TempTime>=45) && (TempTime<69))
 {


 MCUCR&=(~((1<<ISC01)|(1<<ISC00)));
 MCUCR|=(1<<ISC01);
 Edge=0;
 }
 else
 {
#line 181 "C:/Users/HASAN/Desktop/my code/remote.c"
 }
 }
 else
 {

 if((TempTime>=44) && (TempTime<=66))
 {

 BitNo++;
 if(BitNo==8)
 {
 BitNo=0;
 ByteNo++;
 if(ByteNo==4)
 {
 State= 4 ;
 }
 }
 MCUCR|=((1<<ISC01)|(1<<ISC00));
 Edge=1;
 }
 else if((TempTime>152) && (TempTime<186))
 {

 IrData[ByteNo]|=(1<<BitNo);
 BitNo++;
 if(BitNo==8)
 {
 BitNo=0;
 ByteNo++;
 if(ByteNo==4)
 {
 State= 4 ;
 }
 }
 MCUCR|=((1<<ISC01)|(1<<ISC00));
 Edge=1;

 }
 else
 {
#line 228 "C:/Users/HASAN/Desktop/my code/remote.c"
 }
 }
 break;
 }
 case  4 :
 {
 if(Edge)
 {

 if(IrData[2]==((unsigned char)~IrData[3]))
 {

 Data_receive = 1;

 MCUCR&=(~((1<<ISC01)|(1<<ISC00)));
 MCUCR|=(1<<ISC01);


 }
 }
 break;
 }

 }
 GICR|=(1<<INT0);
}




void main()
{
 volatile unsigned char lcd_data;
 DDD2_bit = 0;
 Lcd_Init();
 Lcd_Cmd(_LCD_CLEAR);
 Lcd_Cmd(_LCD_CURSOR_OFF);

 Lcd_Out(1, 1, "IR RemoteDecoder");
 Delay_ms(2000);
 Lcd_Cmd(_LCD_CLEAR);

 RemoteInit();

 while(1)
 {
 lcd_data = GetRemoteCmd(1);
 SREG_I_bit = 0;
 Lcd_Cmd(_LCD_CLEAR);
 if(lcd_data == 0x0A)
 {
 Lcd_Out(1, 1, "You pressed 1!!");
 }
 else if(lcd_data == 0x1B)
 {
 Lcd_Out(1, 1, "You pressed 2!!");
 }
 else if(lcd_data == 0x1F)
 {
 Lcd_Out(1, 1, "You pressed 3!!");
 }
 else if(lcd_data == 0x0C)
 {
 Lcd_Out(1, 1, "You pressed 4!!");
 }
 else if(lcd_data == 0x0D)
 {
 Lcd_Out(1, 1, "You pressed 5!!");
 }
 else if(lcd_data == 0x0E)
 {
 Lcd_Out(1, 1, "You pressed 6!!");
 }
 else
 {
 Lcd_Out(1, 1, "You pressed:");
 Lcd_Chr(1, 14, lcd_data);
 }
 SREG_I_bit = 1;
 }
}


void RemoteInit()
{
 char i;
 for(i=0;i<4;i++)
 {
 IrData[i]=0;
 }
 State= 0 ;
 Edge=0;



 TCCR0|=((1<<CS00)|(1<<WGM01));
 TIMSK|=(1<<OCIE0);
 OCR0= 160 ;


 MCUCR|=(1<<ISC01);
 GICR|=(1<<INT0);


 SREG_I_bit = 1;
}

void ResetIR()
{
 char i;
 for(i=0;i<4;i++) IrData[i]=0;
 State= 0 ;

 MCUCR&=(~((1<<ISC01)|(1<<ISC00)));
 MCUCR|=(1<<ISC01);
 Edge=0;
 Time=0;
}

unsigned char GetRemoteCmd(char wait)
{
 unsigned char cmd;

 if(wait)
 while(Data_receive == 0);
 else
 {
 if(Data_receive == 0)
 return ( 255 );
 }
 cmd=IrData[2];
 Data_receive = 0;
 ResetIR();
 return cmd;
}
