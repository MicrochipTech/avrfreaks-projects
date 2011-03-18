/*****************************************************
Project : 
Version : 
Date    : 08-11-2004
Author  : Jan Thogersen
Company : www.future-design.dk
Comments: 


Chip type           : ATtiny2313
Clock frequency     : 8,000000 MHz (Internal or external)
Memory model        : Tiny
External SRAM size  : 0
Data Stack size     : 32
*****************************************************/

#include <tiny2313.h>

#define LED     PORTD.5

// Define the states (DS = Device State)
#define DS_Resetting         0 // Reset procedure
#define DS_WaitForSignalOk   1 // Wait until the signal quality has been aproved.
#define DS_WaitUserMotorStop 2 // Wait for the user to move the transmitter stick to motor off position.
#define DS_MotorControl      3 // In this mode the motor is control'ed by the RC signal.
#define DS_PowerDown         4 // Power down due to lack of power on the batt.
#define DS_CheckPowerStatus  5 // Check if the power is back up.

unsigned char InitPhase;

unsigned char CaptureInput;
unsigned char GoodCounter;
unsigned char ucPowerGood;
unsigned char Ready;

// Timer 0 overflow interrupt service routine
interrupt [TIM0_OVF] void timer0_ovf_isr(void)
{
// Place your code here
    static unsigned int PreScaler;

    if (PreScaler > 0) PreScaler--;
    else {
        PreScaler = 2000; 
        // The prescaler will make the LED flash in different speeds.
        switch (InitPhase) {
        case DS_WaitUserMotorStop: 
            PreScaler = 200;
        case DS_WaitForSignalOk:
            if (LED) OCR0A = 10;
            else OCR0A = 0;
            break;

        case DS_MotorControl:
            PreScaler = 50;
            break;

        case DS_PowerDown:
            if (LED) PreScaler = 50;
            else PreScaler = 400;
            break;

        case DS_CheckPowerStatus:
            if (LED) PreScaler = 50;
            else PreScaler = 4000;
            break;
        }


        if (ACSR & 0x20) {
            if (ucPowerGood > 0) ucPowerGood--;
        } else {
            if (ucPowerGood < 30) ucPowerGood++;
        }

        PORTD.5 ^= 1;
    }
}

// Timer 1 overflow interrupt service routine
interrupt [TIM1_OVF] void timer1_ovf_isr(void)
{
    static unsigned char PreScaler;
// Place your code here
    if (PreScaler) PreScaler--;
    else {
        PreScaler = 4;
        if (GoodCounter > 1) GoodCounter-=1;
        Ready = 1;
    }
}

// Timer 1 input capture interrupt service routine
#define LOW_END	4600
#define HIGH_END 7660
#define FACTOR 12                    

interrupt [TIM1_CAPT] void timer1_capt_isr(void)
{
// Place your code here
    static unsigned char InputState;
    unsigned int Tmp;
    unsigned char Good = 0;

    if (InputState == 0) {
        // Find the positive edge.
        TCNT1 = 0; // Reset timer and measure the complete period.
        InputState = 1;
        TCCR1B &= ~0x40;
    } else {
        // Find the negative edge.
        Tmp = ICR1; // Save the length of the signal from the RC transmitter

        if (Tmp > 4000 && Tmp < 12000) Good = 1;// > 1ms and < 2 ms

        if (Tmp < LOW_END) Tmp = LOW_END;
        if (Tmp > HIGH_END) Tmp = HIGH_END;

        if (Good == 1) {
            if (GoodCounter < 40) GoodCounter += 2;
            CaptureInput = (Tmp - LOW_END) / FACTOR;
            Ready = 1;
        } else {
            if (GoodCounter > 0) GoodCounter--;
        }
        InputState = 0;
        TCCR1B |= 0x40;
    }
}

// Declare your global variables here
unsigned char InitCounter;
unsigned char Tmp;
unsigned char MaxPower;

void main(void)
{
// Declare your local variables here

// Crystal Oscillator division factor: 1
    CLKPR=0x80;
    CLKPR=0x00;

// Input/Output Ports initialization
// Port A initialization
// Func2=In Func1=In Func0=In 
// State2=T State1=T State0=T 
    PORTA=0x00;
    DDRA=0x00;

// Port B initialization
// Func7=In Func6=In Func5=In Func4=In Func3=Out Func2=Out Func1=In Func0=In 
// State7=T State6=T State5=T State4=T State3=0 State2=0 State1=T State0=T 
    PORTB=0x00;
    DDRB=0x04;

// Port D initialization
// Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
// State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
    PORTD=0x00;
    DDRD=0x20;

/*
// Timer/Counter 0 initialization
// Clock source: System Clock
// Clock value: 1000,000 kHz
// Mode: Fast PWM top=FFh
// OC0A output: Disconnected
// OC0B output: Non-Inverted PWM
    TCCR0A=0x23;
    TCCR0B=0x02;
    TCNT0=0x00;
    OCR0A=0x00;
    OCR0B=0x00;
*/    
// Timer/Counter 0 initialization
// Clock source: System Clock
// Clock value: 1000,000 kHz
// Mode: Fast PWM top=FFh
// OC0A output: Non-Inverted PWM
// OC0B output: Disconnected
    TCCR0A=0x83;
    TCCR0B=0x02;
    TCNT0=0x00;
    OCR0A=0x00;
    OCR0B=0x00;

// Timer/Counter 1 initialization
// Clock source: System Clock
// Clock value: 4000,000 kHz
// Mode: Normal top=FFFFh
// OC1A output: Discon.
// OC1B output: Discon.
// Noise Canceler: On
// Input Capture on Rising Edge
    TCCR1A=0x00;
    TCCR1B=0xC1;
    TCNT1H=0x00;
    TCNT1L=0x00;
    ICR1H=0x00;
    ICR1L=0x00;
    OCR1AH=0x00;
    OCR1AL=0x00;
    OCR1BH=0x00;
    OCR1BL=0x00;

// External Interrupt(s) initialization
// INT0: Off
// INT1: Off
// Interrupt on any change on pins PCINT0-7: Off
    GIMSK=0x00;
    MCUCR=0x00;

// Timer(s)/Counter(s) Interrupt(s) initialization
    TIMSK=0x8A;

// Analog Comparator initialization
// Analog Comparator: On
// Digital input buffers on AIN0: Off, AIN1: Off
    DIDR=0x03;
// The Analog Comparator's positive input is
// connected to the Bandgap Voltage Reference
// Analog Comparator Input Capture by Timer/Counter 1: Off
// Analog Comparator Output: On
    ACSR=0x60;

// Global enable interrupts
#asm("sei")

    InitPhase = DS_Resetting;  
    InitCounter = 0;
    OCR0A = 0;
    GoodCounter = 0;

    while (1) {

        // Place your code here
        while (!Ready) {
        }; Ready = 0;

        if (GoodCounter == 0) InitPhase = 0;

        switch (InitPhase) {
        case DS_Resetting : 
        // Reset process
            ucPowerGood = 50;   
            MaxPower = 255;
            InitCounter = 0;
            InitPhase++;
            GoodCounter = 1;

        case DS_WaitForSignalOk :
        // Wait for signal
            if (GoodCounter > 30) InitPhase++;
            break;

        case DS_WaitUserMotorStop : 
        // Wait for low pwm command
            if (InitCounter > 0) InitCounter--;
            if (CaptureInput < 10) {
                if (InitCounter < 10) InitCounter += 2;
                else InitPhase++;
            }
            break;
        case DS_MotorControl : 
        // Control engine
            if (ucPowerGood == 0) {
                ucPowerGood = 30;
                InitPhase++; 
                MaxPower = 255;
            }

            OCR0A = CaptureInput;
            break;
        case DS_PowerDown : 
        // Power Dowm
            if (ucPowerGood == 0) {
                MaxPower -= 5;
                ucPowerGood = 2;
                if (MaxPower < 200) {
                    MaxPower = 0; InitPhase++;
                }
            }

            Tmp = CaptureInput;
            if (Tmp > MaxPower) Tmp = MaxPower;
            OCR0A = Tmp;
            break;
        case DS_CheckPowerStatus : 
        // Check power status
            if (ucPowerGood == 25 && CaptureInput < 5) InitPhase = 3;
            OCR0A = 0;
        }
    }
}
