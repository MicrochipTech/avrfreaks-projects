#include<avr/io.h>
#include<compat/deprecated.h>
#define F_CPU 8000000
#include<avr/delay.h>
#define RS PA0
#define EN PA2


#define PSclock     2               // PB2
#define PSdata      3               // PB3
#define PSacknolage 4               // PB4
#define PScommand   5               // PB0
#define PSattention 6               // PB6
 
 
// PSx controller communication function.
// send a byte on the command line and receive one on the data line.
// needs Attention pin to have gone low before called to activate controller.
int gameByte(short int command)
{
    short int i ;
    _delay_us(1);
    short int data = 0x00;                                   // clear data variable to save setting low bits later.
    for(i=0;i<8;i++)
    {
	    if(command & _BV(i)) sbi(PORTB, PScommand);      // bit bang "command" out on PScommand wire.
            else cbi(PORTB, PScommand);
            cbi(PORTB, PSclock);                             // CLOCK LOW
            _delay_us(1);                                     // wait for output to stabilise
            sbi(PORTB, PSclock);                             // CLOCK HIGH
            if((PINB & _BV(PSdata))) {sbi(data, i);}         // read PSdata pin and store
            //else {cbi(data, i);}
    }
    sbi(PORTB, PScommand);

    _delay_us(20);                                            // wait for ACK to pass.
	
    return(data);
}

void LCD_init(void)
{
wrcomm(0x38); //initialize LCD 2 lines, 5x7 matrix  00111000
wrcomm(0x06);//entry mode
wrcomm(0x01);//Clear LCD
wrcomm(0x80); //fist line first column
wrcomm(0x0C); //Display on, Cursor Blinking
//wrcomm(0x08);

}

void wrcomm(unsigned char Command)
{
cbi(PORTA,RS);//
PORTC = Command;
 sbi(PORTA,EN);
_delay_ms(20); 
cbi(PORTA,EN);
_delay_ms(20);
}

void wrdata(unsigned char Data)
{
sbi(PORTA,RS); 

PORTC = Data;
sbi(PORTA,EN);
_delay_ms(20);
cbi(PORTA,EN); 
_delay_ms(20);
}


void LCD_String (unsigned char *string)
{
while (*string)
wrdata(*string++);
}


void analog(int datac, int datad)
{
if(datac==0x00)
	LCD_String("LEFT");
if(datac==0xff)
	LCD_String("RIGHT");
if(datad==0x00)
	LCD_String("UP");
if(datac==0xff)
	LCD_String("DOWN");
}


void traverse(char dataa, char datab)
{
//if(dataa==0xfe)
//	LCD_String("SEL");
if(dataa==0xf7)
	LCD_String("START");
if(dataa==0xef)
	LCD_String("UP");
if(dataa==0x7f)
	LCD_String("LEFT");
if(dataa==0xdf)
	LCD_String("RIGHT");
if(dataa==0x9f)
	LCD_String("DOWN");

if (datab==0xfe)
	LCD_String("L2");
if(datab==0xfd)
	LCD_String("R2");
if(datab==0xfb)
	LCD_String("L1");
if(datab==0xf7)
	LCD_String("R1");
if(datab==0xef)
	LCD_String("A");
if(datab==0xdf)
	LCD_String("O");
if(datab==0xbf)
	LCD_String("X");	
if(datab==0x7f)
	LCD_String("U");
}

int main(void)
{
DDRA=0xff;
DDRC=0xff;
LCD_init();
_delay_ms(10);
wrcomm(0x80);
//LCD_String("hello");

   // PSx controller I/O pin setup:
   sbi(DDRB, PB2);                 // clock. output. (blue)

   cbi(DDRB, PB3);                 // data. input. (brown)
   sbi(PORTB, PB3);               //    enable pullup resistor

   cbi(DDRB, PB4);                 // acknolage. input. (green)
   sbi(PORTB, PB4);               //    enable pullup resistor

   sbi(DDRB, PB5);                  // command. output. (orange)

   sbi(DDRB, PB6);                 // attention. output. (yellow)



	
   // this loop continues to put PSx controller into analouge mode untill the
   // controller responds with 0x73 in the 2nd byte. 
   // (PS2 controller responds with 0x73 when in analouge mode.)
   // the status LEDs will continue to count upwards untill a controller is found.
   // if everything is working correctly this should happen on the first pass of
   // this loop but occasionally errors occur and a 2nd or 3rd itteration happen.
   unsigned char chk_ana = 0, cnt = 0;
   int d;
   while(chk_ana != 0x73){
       // put controller in config mode
       sbi(PORTB, PScommand);
       sbi(PORTB, PSclock);
       cbi(PORTB, PSattention);

       gameByte(0x01);
       gameByte(0x43);
       gameByte(0x00);
       gameByte(0x01);
       gameByte(0x00);

       sbi(PORTB, PScommand);
       _delay_ms(1);
       sbi(PORTB, PSattention);

       _delay_ms(10);

       // put controller in analouge mode
       sbi(PORTB, PScommand);
       sbi(PORTB, PSclock);
       cbi(PORTB, PSattention);

       gameByte(0x01);
       gameByte(0x44);
       gameByte(0x00);
       gameByte(0x01);
       gameByte(0x03);
       gameByte(0x00);
       gameByte(0x00);
       gameByte(0x00);
       gameByte(0x00);

       sbi(PORTB, PScommand);
       _delay_ms(1);
       sbi(PORTB, PSattention);

       _delay_ms(10);

       // exit config mode
       sbi(PORTB, PScommand);
       sbi(PORTB, PSclock);
       cbi(PORTB, PSattention);

       gameByte(0x01);
       gameByte(0x43);
       gameByte(0x00);
       gameByte(0x00);
       gameByte(0x5A);
       gameByte(0x5A);
       gameByte(0x5A);
       gameByte(0x5A);
       gameByte(0x5A);

       sbi(PORTB, PScommand);
       _delay_ms(1);
       sbi(PORTB, PSattention);

       _delay_ms(10);

       // poll controller and check in analouge mode.
       sbi(PORTB, PScommand);
       sbi(PORTB, PSclock);
       cbi(PORTB, PSattention);

       gameByte(0x01);
       chk_ana = gameByte(0x42);            // the 2nd byte to be returned from the controller should = 0x73 for "red" analouge controller.
       gameByte(0x00);
       gameByte(0x00);
       gameByte(0x00);
       gameByte(0x00);
       gameByte(0x00);
       gameByte(0x00);
       gameByte(0x00);

       sbi(PORTB, PScommand);
       _delay_ms(1);
       sbi(PORTB, PSattention);
       _delay_ms(10);
   }
//wrdata(cnt);
LCD_String("start");
//_delay_ms(100);
   short int temp, data0, data1, data2, data3, data4, data5, i ,debounceSelect;
  // main program loop:
   while (1){

        sbi(PORTB, PScommand);                          // start communication with PSx controller
        sbi(PORTB, PSclock);
        cbi(PORTB, PSattention);

        gameByte(0x01);                                       // bite 0. header.
        temp = gameByte(0x42);                          // bite 1. header. (should possibly put test on this byte to detect unplugging of controller.)
        gameByte(0x00);                                       // bite 2. header.

        data0 = gameByte(0x00);                         // bite 3. first data bite.
        data1 = gameByte(0x00);                         // bite 4.
        data2 = gameByte(0x00);                         // bite 5.
        data3 = gameByte(0x00);                         // bite 6.
        data4 = gameByte(0x00);                         // bite 7.
        data5 = gameByte(0x00);                         // bite 8.

        _delay_us(1);
        sbi(PORTB, PScommand);                      // close communication with PSx controller
        _delay_us(1);
        sbi(PORTB, PSattention);                        // all done.


traverse(data0,data1);
analog(data4,data5);
       
   } // while(1) 
} //main



