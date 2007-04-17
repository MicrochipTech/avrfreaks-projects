/************************Information about this file***************************/
/*Author: omella

*/
/************************Include files*****************************************/
#include <iom169.h>
#include <inavr.h>
/******************************************************************************/
/************************VARIABLE DECLARATION**********************************/
/******************************************************************************/
unsigned long P          = 0;
unsigned long A          = 0;
unsigned long Vin        = 0;
unsigned long Vout       = 0;
unsigned long result     = 0;
unsigned long count1     = 0;
unsigned long count2     = 0;
unsigned long pause_10ms = 0;
unsigned int  comp_value = 260;
unsigned int  temp_comp  = 0;
unsigned int  flag       = 0;
unsigned int  ten_sec    = 1;
unsigned int  battery	 = 0;
unsigned int  ADC_input;
const int	  factor	 = 5;
/******************************************************************************/
/************************FUNCTION PROTOTYPES***********************************/
/******************************************************************************/
void init_port(void);
void timer1_ovf_isr(void);
void init_interrupt(void);
void read_Vin(void);
void read_A(void);
void read_Vout(void);
void ADC_conv(void);
void calc_effect(void);
void check_flag(void);
void inc_A(void);
void dec_A(void);
void init_PWM(void);
void effectcontrol(void);
void check_battery(void);
void every_quarter(void);
void maintenance_charging(void);
void change_compvalue(void);
void pause(void);
void pause_10s(void);
void charging(void);
/************************Initiates I/O's***************************************/
 void init_port(void)
 {
 DDRA = 0b11111000;	 			   //Ain,Vin,Vout are inputs 
 DDRB = 0b00100000;				   //FB is output (PWM-out)
 }
/************************Initiates PWM*****************************************/
void init_PWM(void)
 {
 TCCR1A = 0b10000010;			   //FastPWM with ICR1 as topvalue
 TCCR1B = 0B00011001;			   //Compare match resets OCA1
 change_compvalue();			   //Initiates a new compare value
 ICR1H = 1;
 ICR1L = 243;					   //2kHz PWM frequency @ 1MHz cpu-clk
/************************Initiates isr*****************************************/
void init_interrupt(void)
 {
 TIMSK = 0b00000100;	 		   //Enables OF-interrupt
 SEI();	 						   //Enables interrupts globally
 }
/************************Changes compare value for PWM*************************/
void change_compvalue(void)
 {
 if(comp_value > 256)
  {
  OCR1AH = comp_value%256;
  OCR1AL = 256*((comp_value/256)-(comp_value%256));
  }
 else
  {
  OCR1AH = 0;
  OCR1AL = comp_value;
  }
 }
/************************OverFlow interrupt************************************/
#pragma interrupt_handler timer1_ovf_isr:10
void timer1_ovf_isr(void)
 {
 change_compvalue();
 count1++;
 count2++;
 if(count1 == 1800000)	  	       //Flag every 15th minute
  {
  flag   = 1; 		  	  				   
  count1 = 0;
  }
 if(count2 == 20000)	           // Mainroutine every 10th second
  {
  ten_sec = 1;
  count2  = 0;
  }
 }
/************************25 ms pause*******************************************/
void pause(void)
 {
 int i = 0;
 while(i<25000) {i++;}
 }
/************************10 s pause********************************************/
void pause_10s(void)
 {
 int i;
 for(i=0; i<400; i++) {pause();}
 }
/************************ADC***************************************************/
void ADC_conv(void)
 {
 int i;
 int sum_LSB = 0;
 int sum_MSB = 0;
 ADCSR = 0b10000110;
 ADMUX = ADC_input;
 for(i=0; i<16; i++)
  {
  ADCSR = 0b01000000;		       // Starts conversion
  pause();
  sum_LSB += ADCL;				   // Adds up 8 LSB
  sum_MSB += ADCH;				   // Adds up 8 MSB
  }
  result = (sum_MSB*256)+sum_LSB;  // Adds up LSB and MSB
  result = (result/16);			   // Averagin the measurements
/************************Measures Vin******************************************/
void read_Vin(void)
 {
 ADC_input = 0b01000000;		   // PA0 is input (Vin)
 ADC_conv();
 Vin = result;
 }
/************************Measures A (Current)**********************************/
void read_A(void)
 {
 ADC_input = 0b01000001;		   // PA1 is input (A)
 ADC_conv();
 A = result;
 }
/************************Measures Vout*****************************************/
void reas_Vut(void)
 {
 ADC_input = 0b01000010;		   // PA2 is input (Vout)
 ADC_conv();
 Vout = result;
 }
/************************Calculates effect*************************************/
void calc_effect(void)
 {
 read_A();
 read_Vin();
 P = (A*Vin);
 }
/************************Increase current(compare value)***********************/
void inc_A(void)
 {
 comp_value = (comp_value + factor);
 if(comp_value >= 498) {dec_A();}
 }
/************************Decrease current (compare value)**********************/
void dec_A(void)
 {
 comp_value = comp_value - factor; 
 if(comp_value <= 50) {inc_A();}
 }
/************************OPTIMA energy-utilization*****************************/
void effectcontrol(void)
 {
 double alt_one = 0;
 double alt_two = 0;
 temp_comp = comp_value;		   // Backup of compare value
 inc_A();  	 					   // Tries to increase the current
 pause_10s();
 calc_effect();
 alt_en = P;   					   // Stores the result of increasing the current
 comp_value = temp_comp;		   // Load compare_value backup
 dec_A();	  					   // Tries to decrease the current
 pause_10s(); 
 calc_effect();
 alt_two = P;  					   // Stores the result of decr. the current
 if(alt_two > alt_one)
  {
  dec_A();	  		  			   // If it was better to decr. the current
  								   // Decrease it even more..
  pause_10s();
  }
 else
  {
  comp_value = temp_comp;		   // Load backup value
  inc_A();	   					   // If not, try to increase it instead
  pause_10s();
  }
 }
/************************Check 15 minute flag**********************************/
void check_flag(void)
 {
 if(flag==1) {every_quarter();}
 }
/************************Every 15th minute*************************************/
void every_quarter(void)
 {
 flag = 0;
 check_battery();
 }
/************************Checking battery status*******************************/
void check_battery(void)
 {
 temp_comp = comp_value;		   // Backup of compare value
 comp_value = 0;				   // STOP the charging !
 pause_10s();
 read_Vout(); 					   // Measure the battery voltage
 comp_value = temp_comp;		   // Reload the compare value
 if(Vout >= 646) {battery = 1;}	   // If Vbatt. > 12.6V
 		 		 		  		   // Change status of battery-variable
 else {battery = 0;}
 }
/************************Maintenance charging**********************************/
void maintenance_charging(void)
 {
 comp_value = 250;			   	   // When the battery is full, charge with 
 			  					   // a low current
 check_flag();
 }
/************************Normal charging***************************************/
void charging(void)
 {
 effectcontrol();  				   // OPTIMA charging
 check_flag();
 ten_sec = 0;
 }
/******************************************************************************/
/************************MAIN ROUTINE******************************************/
/******************************************************************************/
void main(void)
 {
 init_port();
 init_interrupt();
 init_PWM();
 while(1)
  {
  if(battery==0)&&(ten_sec==1) {charging();}
  else{maintenance_charging();}
  }
 

