/*

IR remote interfacing library for AVR series of microcontrollers.
Target MCU ATmege32 or ATmega16 @ 16MHz.

Sensor: TSOP1738 IR receiver module must be connected to INT0 Pin.
                This is PIN16 in ATmega16 and ATmega32.

                 -----
                |  _  |
                | | | |
                | | | |
                -------
                | |   |
                | |   |
                | |   |
            (GND)(5V)(To PIN16)

         **********************
         *TFM 1380T Front View*
         **********************



Resource Usage:
-Timer0
-INT0 (PD2)


Copyright Fahad Mirza 2011
fahadmirza80@yahoo.com

Please give credit to www.techshopbd.com if you use
it in your projects and find it useful
*/

//States
#define F_CPU 16000000
#define IR_HIGH 0
#define IR_LOW 1
#define IR_RECEIVE_BITS 3
#define IR_WAIT_STOP_BIT 4
#define TIMER_COMP_VAL 160  //for 16MHz. For 12MHz = 120 and 8MHz = 80
#define QMAX 8              //Size of the Remote command buffer
#define RC_NONE 255         //This val is returned by GetRemoteCmd when no key is pressed


// LCD module connections
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
// End LCD module connections



//Global Variables
volatile unsigned char IrData[4];  //The four data Bytes of Ir Packet, 2-Byte Address 2-Byte Data
volatile unsigned int Time = 0;    //Main timer, stores time in 10us, Updated by TIMER0_COMP
volatile unsigned int TempTime;
volatile unsigned char State;             //State of receiver
volatile unsigned char Edge;              //Edge of interrupt [ RISING=1 OR FALLING=0 ]
volatile unsigned char BitNo;             //Pos of next BIT
volatile unsigned char ByteNo;            //Pos of current Byte
volatile unsigned char Data_receive = 0;  //Valid remote button receive flag
char txt[7];


// Function Declaration
void RemoteInit();
void ResetIR();
unsigned char GetRemoteCmd(char wait);


/**********************************************************************************************/
/*                                F U N C T I O N S   S T A R T S                                                                                                  */
/**********************************************************************************************/



void Timer0_COMP_Interrupt() iv IVT_ADDR_TIMER0_COMP
{
        Time++;
}
void Ext_Interrupt() iv IVT_ADDR_INT0
{
        GICR&=(~(1<<INT0));        //Disable INT0
        SREG_I_bit = 1;

        TempTime = Time;
        Time=0;
        TCNT0=0;
        switch(State)
        {
        case IR_HIGH:
        {
             if(Edge)
             {
                //Rising
                if((TempTime>800) && (TempTime<990))
                {
                      //Lead High Correct
                      State=IR_LOW;
                      //INT ON FALLING EDGE
                      MCUCR&=(~((1<<ISC01)|(1<<ISC00)));
                      MCUCR|=(1<<ISC01);
                      Edge=0;
                }
                else
                {
                    /*Lcd_Cmd(_LCD_CLEAR);
                    Lcd_Out(1,1,"LeadHigh");
                    IntToStr(TempTime, txt);
                    Lcd_Out(1,10,txt);
                    ResetIR();*/
                }
             }
             else
             {
                  //Falling
                  MCUCR|=((1<<ISC01)|(1<<ISC00));        //Set INT on Rising Edge
                  Edge=1;
             }
             break;
        }
        case IR_LOW:
        {
             if((TempTime>400) && (TempTime<500))
             {
                //Got a valid leader
                State=IR_RECEIVE_BITS;
                BitNo=0;
                ByteNo=0;
                MCUCR|=((1<<ISC01)|(1<<ISC00));        //Set INT on Rising Edge
                Edge=1;

             }
             else if((TempTime>200) && (TempTime<245))
             {
                // this is for repeat funtion, add ur code
             }
             else
             {
                /*Lcd_Cmd(_LCD_CLEAR);
                Lcd_Out(1,1,"LeadLow");
                IntToStr(TempTime, txt);
                Lcd_Out(1,9,txt);
                ResetIR(); */
             }
             break;
        }
        case IR_RECEIVE_BITS:
        {
             if(Edge)
             {
                //Rising
                if((TempTime>=45) && (TempTime<69))
                {
                   //Correct Begin of BIT found
                   //INT ON FALLING EDGE
                   MCUCR&=(~((1<<ISC01)|(1<<ISC00)));
                   MCUCR|=(1<<ISC01);
                   Edge=0;
                }
                else
                {
                  /* Lcd_Cmd(_LCD_CLEAR);
                   Lcd_Out(1,1,"Bit Beg");
                   IntToStr(TempTime, txt);
                   Lcd_Out(1,9,txt);
                   ResetIR();       */
                }
             }
             else
             {
                //Falling
                if((TempTime>=44) && (TempTime<=66))
                {
                   //We got a '0' here
                   BitNo++;
                   if(BitNo==8)
                   {
                      BitNo=0;
                      ByteNo++;
                      if(ByteNo==4)
                      {
                          State=IR_WAIT_STOP_BIT;
                      }
                   }
                   MCUCR|=((1<<ISC01)|(1<<ISC00));        //Set INT on Rising Edge
                   Edge=1;
                }
                else if((TempTime>152) && (TempTime<186))
                {
                   //We Have got a '1' here
                   IrData[ByteNo]|=(1<<BitNo);
                   BitNo++;
                   if(BitNo==8)
                   {
                      BitNo=0;
                      ByteNo++;
                      if(ByteNo==4)
                      {
                          State=IR_WAIT_STOP_BIT;
                      }
                   }
                   MCUCR|=((1<<ISC01)|(1<<ISC00));        //Set INT on Rising Edge
                   Edge=1;

                }
                else
                {
                   //Invalid Bit
                   /*Lcd_Cmd(_LCD_CLEAR);
                   Lcd_Out(1,1,"Bit Low/Sp");
                   IntToStr(TempTime, txt);
                   Lcd_Out(1,12,txt);
                   ResetIR();*/
                }
             }
             break;
        }
        case IR_WAIT_STOP_BIT:
        {
             if(Edge)
             {
                //Check for integrity
                if(IrData[2]==((unsigned char)~IrData[3]))
                {
                   //Now We Have Got a packet
                   Data_receive = 1;

                   MCUCR&=(~((1<<ISC01)|(1<<ISC00)));
                   MCUCR|=(1<<ISC01);


                }
             }
             break;
        }

        }
        GICR|=(1<<INT0);        //Enable INT1
}




void main()
{
   volatile unsigned char lcd_data;
   DDD2_bit = 0;
   Lcd_Init();                        // Initialize LCD
   Lcd_Cmd(_LCD_CLEAR);               // Clear display
   Lcd_Cmd(_LCD_CURSOR_OFF);          // Cursor off
   
   Lcd_Out(1, 1, "IR RemoteDecoder");
   Delay_ms(2000);
   Lcd_Cmd(_LCD_CLEAR);               // Clear display
   
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
        State=IR_HIGH;
        Edge=0;

        //Setup Timer0
        //------------
        TCCR0|=((1<<CS00)|(1<<WGM01));//Prescaler : Fcpu   Mode : CTC
        TIMSK|=(1<<OCIE0);            //Enable Output Compare Interrupt
        OCR0=TIMER_COMP_VAL;          //Set Compare Value

        //Set Up INT0
        MCUCR|=(1<<ISC01);           //INT ON Falling Edge
        GICR|=(1<<INT0);             //Enable INT0

        //Enable Interrupts
        SREG_I_bit = 1;
}

void ResetIR()
{
        char i;
        for(i=0;i<4;i++) IrData[i]=0;
        State=IR_HIGH;
        //INT ON FALLING EDGE
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
              return (RC_NONE);
        }
        cmd=IrData[2];
        Data_receive = 0;
        ResetIR();
        return cmd;
}