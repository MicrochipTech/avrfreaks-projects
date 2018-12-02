//first, setting up for th LCD display

sbit LCD_RS at PORTC2_bit;
sbit LCD_EN at PORTC3_bit;
sbit LCD_D4 at PORTC4_bit;
sbit LCD_D5 at PORTC5_bit;
sbit LCD_D6 at PORTC6_bit;
sbit LCD_D7 at PORTC7_bit;

sbit LCD_RS_Direction at DDC2_bit;
sbit LCD_EN_Direction at DDC3_bit;
sbit LCD_D4_Direction at DDC4_bit;
sbit LCD_D5_Direction at DDC5_bit;
sbit LCD_D6_Direction at DDC6_bit;
sbit LCD_D7_Direction at DDC7_bit;



//the texts that will be displayed
 
char txt1[] = " is PWM value";
char txt2[] = " is angle ";
char txt3[] = " is setpoint";

char nums[11]= "0123456789";
int count; //encoder counter
char i;  


// PID parameters

char PWM_value;
int setpoint;
int Error;
float Pid;
float timeDiff;
float diff;
double errorSum;
int lastError;
//int kp, ki, kd;

  void interrupt_ISR () org IVT_ADDR_INT0 {           // Interrupt function
   if (PIND1_bit){ count--;}                                        //checking if clockwise or not
   else          { count++;}
  

  INTF0_bit=0;                                                    // Clear interrupt flag
}


#define encoder PIND1
#define encoder_B DDRD
#define CW PORTJ0
#define CW_J DDRJ
#define CCW PORTJ1
#define CCW_J DDRJ


void main(){
encoder_B= 0x00;
CW_J=   0xff;
  DDRD = 0x00;                                      // Set PortD as INPUT
  PortD = 0x00;                                      // value for PortD
  DDA5_bit = 0;                   // Set pin 5 as input
  DDA6_bit = 0;                   // Set pin 6 as input



   //setting up interuupt settings
   
   
  ISC01_bit = 1;                                     // Set interrupts to be detected on rising edge
  ISC00_bit = 1;
  count  = 30;                                         // Set counter

  INT0_bit = 1;
  SREG_I_bit = 1;                                    // Enable Interrupts
    
    
  //setting up pwm
    

 PWM_value = 150;             // initial value for PWM_value


  DDG5_bit = 1;                   // Set PORTG pin 5 as output pin for the PWM (according to datasheet) IO12

     TCCR0B |= (1<<CS00)
           | (1<<CS01);               //set timerO clock prescaler to 64
    TCCR0A |= (1<<WGM00);                   //fast pwm with top as 0xFF

    DDRD |= (1<<5);
    DDRD |= (1<<6);                         //set the OCR0 pins as outputs
      OCR0B = PWM_value;
    TCCR0A |= (1<<COM0B1);

     //PID parameters
     
     setpoint= 50;
     LastError=0;
     timeDiff=1;   //approximate time for each cycle as there is a delay of 1 second

  while (1){
  
  //LCD
  
  Lcd_Init();                        // Initialize LCD
  Lcd_Cmd(_LCD_CLEAR);               // Clear display
  Lcd_Cmd(_LCD_CURSOR_OFF);          // Cursor off


  Lcd_Cmd(_LCD_CLEAR);               // Clear display

  Lcd_Chr(1,1,nums[PWM_value/1000%10]);                 // Write text in first row

  Lcd_Chr(1,2,nums[PWM_value/100%10]);                 // Write text in second row
  Lcd_Chr(1,3,nums[PWM_value/10%10]);                 // Write text in first row
  Lcd_Chr(1,4,nums[PWM_value%10]);
   Lcd_Out(1,5,txt1);
   
  Lcd_Chr(2,1,nums[count/1000%10]);                 // Write text in first row
  Lcd_Chr(2,2,nums[count/100%10]);                 // Write text in second row
  Lcd_Chr(2,3,nums[count/10%10]);                 // Write text in first row
  Lcd_Chr(2,4,nums[count%10]);
  Lcd_Out(2,5,txt2);
  
  Lcd_Chr(3,1,nums[setpoint/1000%10]);                 // Write text in first row
  Lcd_Chr(3,2,nums[setpoint/100%10]);                 // Write text in second row
  Lcd_Chr(3,3,nums[setpoint/10%10]);                 // Write text in first row
  Lcd_Chr(3,4,nums[setpoint%10]);
  Lcd_Out(3,5,txt3);

    OCR0B = PWM_value;
    delay_ms(1000);
       
       
           //PID calculation
           
       if (count != setpoint) {

          Error= setpoint-count;
          errorSum= errorSum + Error;
           diff= (Error-LastError)/timeDiff;
           pid = abs(3*Error + 0.01*errorSum + 0.002*diff) ;       //Kp is 3 and  Ki is 0.01 and kd is 0.002
          PWM_value= pid;
          LastError= Error;

        }

        
     if (count < setpoint) {
                               // Direction of motion clockwise

          PORTJ=0b0000001;
             }
          
          if (count > setpoint) {
                                // Direction of motion anti clockwise
          PORTJ=0b0000010;
          
          }
          
          
          

       if (PINA5_bit) {                        // Detect if PORTA pin 5 is pressed
          Delay_ms(40);                         // Small delay to avoid deboucing effect
          setpoint++;                      // Increment setpoint

        }
        else
          if (PINA6_bit) {                      // Detect if PORTA pin 6 is pressed
            Delay_ms(40);                       // Small delay to avoid deboucing effect
            setpoint--;                    // Decrement setpoint

          }


 
 
 }
 
}