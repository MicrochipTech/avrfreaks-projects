# music recognizing dancing puppet

Uploaded by admin on 2008-04-19 17:47:00 (rating 0 out of 5)

## Summary

// A to D test code  

// NOTE -- You MUST MOUNT the Aref jumper 


#include   

#include   

#include   

#include   

//I like these definitions  

#define begin {  

#define end } 


#define float2fix(a) ((int)((a)*256.0)) // converts float constants to fix - used  

for filter coeefs  

#define fix2float(a) ((float)(a)/256.0) 


#define t0 60 //motor timing compare flag 


//IIR stuff  

 #pragma regalloc-  

 int b1,a2,a3, xn, xn\_1, xn\_2, yn, yn\_1, yn\_2; // internal parameters for the  

butterworth filter  

 #pragma regalloc+


 //motor control variables  

 int stepcount=0;  

 unsigned int time0;  

unsigned char step; 


char motor\_on=0; //flag to turn motor on  

 char motor\_fwd=1; //flag to move motor fwd  

 char motor\_back=0; //flag to move motor backward 


 char special=0; //flag to change from NORMAL FAST DANCE mode to SPECIAL SLOW  

DANCE MODE


 char move\_done=0; //flag to know when the previous motor move is done


void initialize(void); //intialize all parameters and registers  

void filter(void); //converts a low pass and a high pass filter on an array of  

stored ADC values  

void motor(void); //controls two the stepper motors fwd and backwards and at a  

variable speed and amplitude


int Ain ; //raw A to D number  

int Abuffer[128]; //array of 128 stored ADC values  

int Abuffer\_filtered[128]; //array of 128 stored low pass filtered ADC values  

int Abuffer\_filtered\_high[128]; //array of 128 stored high pass filtered ADC values  

char arrayindex=0;  

char set\_ready=0; //flag to know when the array Abuufer is full and ready to be  

filtered  

char dd, ee; 


int t\_motor=0; //timer running  

int t\_led=0;


int time\_motor\_on = 400; //variable yto control how long the motor is on  

int time\_led\_on = 50;


//int upper\_th=150;  

int upper\_th=175; //detect base threshold  

int upper\_th\_hp=40; //detect mid freq therhold 


//timer 0 compare ISR  

interrupt [TIM0\_COMP] void timer0\_compare(void)  

begin 


 if (time0 > 0) --time0; //for motor timing


 t\_motor++; //increment timers to control time motor is on  

 t\_led++; //increment timers to control time LED is on


 //accumulates ADC values in a buffer of size 128  

 Abuffer[arrayindex] = Ain;  

 arrayindex++;


 if(arrayindex==127) //if array full  

 begin  

 arrayindex=0; //resets the array index for folowing buffer  

 set\_ready=1; //signals main that bufffer is full and ready to be filtered 


 end 


 //get the sample  

 Ain = ADCH;  

 //start another conversion  

 ADCSR.6=1; 


end 


 //========================================================  

int IIR2(int xx) //2 pole filter  

// xx is the current input signal sample  

// returns the current filtered output sample  

begin  

 //LOW PASS FILTER PARAMETERS FOR CUT OFF OF 100 Hz  

 b1 = float2fix(0.1410) ;  

 a2 = float2fix(0.6940) ; //note that sign is negated from design input  

 a3 = float2fix(-0.2580) ; //note that sign is negated from design input 


 #asm  

 .macro mult\_acc ;r31:r30:r24 += r23:r22 * r21:r20  

 muls r23, r21 ; (signed)ah * (signed)bh  

 add r31, r0  

 mul r22, r20 ; al * bl  

 add r24, r0  

 adc r30, r1  

 adc r31, r27  

 mulsu r23, r20 ; (signed)ah * bl  

 add r30, r0  

 adc r31, r1  

 mulsu r21, r22 ; (signed)bh * al  

 add r30, r0  

 adc r31, r1  

 .endm 


 push r20 ;save parameter regs  

 push r21


 clr r27 ;permanent zero  

 clr r24 ;clear 24 bit result reg; msb to lsb => r31:r30:r24  

 clr r30  

 clr r31


 lds R22, \_xn\_1 ;load x(n-1) from RAM  

 lds R23, \_xn\_1+1  

 lsl r22 ;mult by two  

 rol r23  

 ld R20, Y ;load input parameter AA from stack  

 ldd R21, Y+1  

 add r20, r22 ;form AA+2x(n-1)  

 adc r21, r23  

 lds R22, \_xn\_2 ;load x(n-2) from RAM  

 lds R23, \_xn\_2+1  

 add r20, r22 ;form AA+2x(n-1)+x(n-2)  

 adc r21, r23  

 lds R22, \_b1 ;load b1 from RAM  

 lds R23, \_b1+1  

 mult\_acc ; b1*(AA+2*x(n-1)+x(n-2)) 


 lds R22, \_a2 ;load -a2 from RAM  

 lds R23, \_a2+1  

 lds R20, \_yn\_1 ;load y(n-1) from RAM  

 lds R21, \_yn\_1+1  

 mult\_acc ; -a2*y(n-1) 


 lds R22, \_a3 ;load -a3 from RAM  

 lds R23, \_a3+1  

 lds R20, \_yn\_2 ;load y(n-2) from RAM  

 lds R21, \_yn\_2+1  

 mult\_acc ; -a3*y(n-2)


 lds R20, \_xn\_1 ;load x(n-1) from RAM  

 lds R21, \_xn\_1+1  

 sts \_xn\_2, r20 ;store x(n-2) to RAM  

 sts \_xn\_2+1, R21  

 ld R20, Y ;load input parameter AA from stack  

 ldd R21, Y+1  

 sts \_xn\_1, r20 ;store x(n-1) to RAM  

 sts \_xn\_1+1, R21  

 lds R20, \_yn\_1 ;load y(n-1) from RAM  

 lds R21, \_yn\_1+1  

 sts \_yn\_2, R20 ;store y(n-2) to RAM  

 sts \_yn\_2+1, R21  

 sts \_yn\_1, r30 ;store new output as y(n-1) to RAM  

 sts \_yn\_1+1, r31 


 pop r21 ;restore parameter regs  

 pop r20


 #endasm 


end


//========================================================  

int Butter2high(int xx)  

// xx is the current input signal sample  

// returns the current filtered output sample  

begin  

 //HIGH PASS FILTER PARAMETERS FOR CUT OFF OF 400 Hz  

 b1=float2fix(0.3913);  

 a2=float2fix(0.3695);  

 a3=float2fix(-0.1958);


 #asm


 push r20 ;save parameter regs  

 push r21


 clr r27 ;permanent zero  

 clr r24 ;clear 24 bit result reg; msb to lsb => r31:r30:r24  

 clr r30  

 clr r31


 lds R22, \_xn\_1 ;load x(n-1) from RAM  

 lds R23, \_xn\_1+1  

 lsl r22 ;mult by two  

 rol r23  

 ld R20, Y ;load input parameter xx from stack  

 ldd R21, Y+1  

 sub r20, r22 ;form xx-2x(n-1)  

 sbc r21, r23  

 lds R22, \_xn\_2 ;load x(n-2) from RAM  

 lds R23, \_xn\_2+1  

 add r20, r22 ;form xx-2x(n-1)+x(n-2)  

 adc r21, r23  

 lds R22, \_b1 ;load b1 from RAM  

 lds R23, \_b1+1  

 mult\_acc ; b1*(xx-2*x(n-1)+x(n-2)) 


 lds R22, \_a2 ;load -a2 from RAM  

 lds R23, \_a2+1  

 lds R20, \_yn\_1 ;load y(n-1) from RAM  

 lds R21, \_yn\_1+1  

 mult\_acc ; -a2*y(n-1) 


 lds R22, \_a3 ;load -a3 from RAM  

 lds R23, \_a3+1  

 lds R20, \_yn\_2 ;load y(n-2) from RAM  

 lds R21, \_yn\_2+1  

 mult\_acc ; -a3*y(n-2)


 lds R20, \_xn\_1 ;load x(n-1) from RAM  

 lds R21, \_xn\_1+1  

 sts \_xn\_2, r20 ;store x(n-2) to RAM  

 sts \_xn\_2+1, R21  

 ld R20, Y ;load input parameter xx from stack  

 ldd R21, Y+1  

 sts \_xn\_1, r20 ;store x(n-1) to RAM  

 sts \_xn\_1+1, R21  

 lds R20, \_yn\_1 ;load y(n-1) from RAM  

 lds R21, \_yn\_1+1  

 sts \_yn\_2, R20 ;store y(n-2) to RAM  

 sts \_yn\_2+1, R21  

 sts \_yn\_1, r30 ;store new output as y(n-1) to RAM  

 sts \_yn\_1+1, r31 


 pop r21 ;restore parameter regs  

 pop r20


 #endasm 


end


void filter(void)  

begin


//ADC sampling f = 2000Hz , sampling T = 0.5ms 


 //FOR LOW PASS FILTER cut off 100 Hz  

 //w cut off = 2*pi*100 * sampling T = 0.314 rad  

 // b =0.1410 0.2820 0.1410  

 // a = 1.0000 -0.6940 0.2580 


 //FOR HIGH PASS FILTER cut off 400 Hz -- 1khz max in hardware


 //b = 0.3913 -0.7827 0.3913  

 //a = 1.0000 -0.3695 0.1958


//LOW PAS FILTER:  

 for(dd=0;dd<127;dd++)  

 begin  

 Abuffer\_filtered[dd] = IIR2(Abuffer[dd]) ; //low pass filter Abuffer 


 if(Abuffer\_filtered[dd]>upper\_th) //if one signal in filtered signal is higher  

than a set value  

 begin  

 PORTB=0b11011111; //led5 on 


 motor\_on=1 ; //turn motors on  

 t\_motor=0; //intitlaize timers to zero  

 t\_led=0;  

 special =0; //normal fast dance mode  

 time\_motor\_on=10; //motor timing for fast dance mode  

 end  

 end 


 //HIGH PASS FILTER :  

 for(ee=0;ee<127;ee++)  

 begin  

 Abuffer\_filtered\_high[ee] = Butter2high(Abuffer[ee]) ; //high pass filter  

Abuffer 


 if(Abuffer\_filtered\_high[ee]>upper\_th\_hp) //if one signal in filtered signal  

is higher than a set value  

 begin  

 PORTB=0b11111011; //led2 on


 special = 1; //special slow dance mode  

 motor\_on=1 ; //turn motor on  

 time\_motor\_on=400; //motor timing for slow dance mode  

 end  

 end 


end


void main(void)  

begin 


 initialize();


 // measure and display loop  

 while (1)  

 begin 


 //decide left / right...  

 if(motor\_on) //if strong signal has been detected  

 begin  

 if(motor\_fwd==1) //if previous move was forward  

 begin  

 motor\_fwd=0;  

 motor\_back=1; //next move is backward  

 end  

 else //else if previous move was backward  

 begin  

 motor\_back=0;  

 motor\_fwd=1; //next move is forward  

 end  

 end


 //stop motor after variable amount of time  

 if(t\_motor==time\_motor\_on) //after that time elapsed from motor\_on  

 begin  

 motor\_on=0; //turn motor off  

 motor\_fwd=0;  

 motor\_back=0;  

 move\_done=1; //that motor move is done  

 end 


 //turn LED's off after variable amount of time  

 if(t\_led==time\_led\_on)  

 begin  

 PORTB=0xff; //led off  

 end 


 //decide between normal fast dance mode OR special slow dance mode  

 if(special)  

 begin  

 delay\_ms(80);  

 motor();  

 end  

 else  

 begin  

 motor();  

 end 


 if(set\_ready) //if the Abuffer of ADC values is full  

 begin  

 filter();  

 set\_ready=0; //for next detection of Abuffer full  

 end


 end  

end 


void motor(void)  

begin


if(time0 == 0)


 begin


 time0 = t0;


 if(motor\_on && move\_done) //if new signal has been detected and the  

previous move is done  

 begin


 if (motor\_fwd) //move motors forward


 begin  

 if (step > 0)  

 step--;  

 else step = 3;  

 end


 else if (motor\_back) //move motors backwards


 begin  

 if (step < 3)  

 step++;  

 else step = 0;  

 end


 end  

 if (step == 0) PORTC = 1+128; //converted to approapriate bit  

sequence to control motor  

 if (step == 1) PORTC = 2+64;  

 if (step == 2) PORTC = 4+32;  

 if (step == 3) PORTC = 8+16;  

 end  

end


void initialize(void)  

begin 


 DDRB=0xff; // PORT B is an ouput LED's  

 PORTB=0x0f; 


 DDRC=0xff; // PORT C is an output port for motor control  

 PORTC=0x00;


 //set up timer 0  

 TIMSK=2; //turn on timer 0 cmp match ISR 


 OCR0 = 125; //set the compare re to 125 time ticks  

 //interupt will run every 0.5 ms ---> which will make the ADC sampling  

rate 2KHZ  

 //divide OCRO by 2 will double the ADC sampling rate


 //prescalar to 64 and turn on clear-on-match 1 cycle is 4 us  

 TCCR0=0b00001011; 


 //init the A to D converter  

 //channel zero/ left adj /EXTERNAL Aref  

 //!!!CONNECT Aref jumper!!!!  

 ADMUX = 0b00100000;  

 //enable ADC and set prescaler to 1/128*16MHz=125,000  

 //and clear interupt enable  

 //and start a conversion  

 ADCSR = 0b11000111; 


 //initialize motor variables  

 time0=t0;  

 step=0;  

 stepcount=0; 


 //crank up the ISRs  

 #asm  

 sei  

 #endasm 


end

## Compilers

- AVR Studio 4

## Tags

- Complete code
- AVR Studio 4
