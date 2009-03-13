// Project Name				: Volt Meter
// Project Description		: Measures the voltage.
// Authors (Programmers)	: Allenrl, ZaryckiCS
// Date						: Tomorrow never knows
// "Volt Meter" uses the ADC feature of the ATmega8 to detect and display
// a voltage from 0 to 10 volts on the Hantronics HDM16216H-5-s00s


// Using an ATmega8
#include	"M8def.inc"

// place any equ's, def's, or set's here

.cseg
.org		0x0000
.equ		CVoltageLSB = 0x0061	//10 bit voltage LSB
.equ		CVoltageMS2 = 0x0062	//10 bit voltage MSB
.equ		VBit0 = 0x0065			//BCD voltage LSB
.equ		VBit1 = 0x0066			//BCD voltage middle bit
.equ		VBit2 = 0x0067			//BCD voltage MSB
rjmp		Start // jump over interrupt vectors	

// interrupt vectors

Start:

// initialize stack pointer to point to end of memory

ldi		r16, low(RAMEND)
ldi		r16, high(RAMEND)
out		SPH, r16

// initialization
ldi		r16, 0b11111111
out		DDRB, r16			//set port B to output (LCD data)

ldi 	r16, 0b00010111
out		DDRD, r16			//set port pin 3 button others control LCD
ldi		r16, 0b00001000		//pullup resistor for button
out		PORTD, r16

clr		r16
out		DDRC, r16			//set PORTC to input voltage

// initalize the ADC 
ldi		r16, 0b11100011		// devide clock by 8, sample 7.5K (ish)
out 	ADCSRA, r16			//initalize status register, enable adc
ldi		r16, 0b01100000
out		ADMUX, r16			//initalize admux (select port and refrence voltage)
clr		r16
sts		CVoltageLSB, r16		//initalize current voltage with 0
sts		CVoltageMS2, r16


// initalize the display based on requirements in datasheet
InitalizeDisplay:
	ldi		r16, 0b00110000
	rcall	WriteInstruction
	rcall	delay20

	ldi		r16, 0b00001110
	rcall 	WriteInstruction
	rcall	delay20

	ldi		r16, 0b00000001
	rcall	WriteInstruction
	rcall	delay20

	ldi		r16, 0b00000110
	rcall	WriteInstruction
	rcall	delay20

	ldi		r16, 0b00001100
	rcall	WriteInstruction
	rcall	delay20


// main routine loop
Main:
	
// 	rcall	ReadSwitch		//disabled to allow constant readout of voltage
	rcall	delay20
	rcall	delay5			//delay will help the screen remain stable

	rcall	GetVoltage		//read voltage from ADC

	rcall	Scale			//scale the voltage

	rcall	Convert			//convert the voltage to ascii
	rcall	delay20			//delay will help the screen remain stable

	rcall	Display			//display the voltage

rjmp Main

// subroutines

ReadSwitch:		//Reads a switch, debounces, and waits for switch to be released.
	

	sbic 	 PIND, PIND3 	//see if switch is pressed
	rjmp	 ReadSwitch		//switch not pressed, loop until pressed

	rcall	 delay20 		//switch pressed, wait for bouncing

	PollingLoop:
		sbis	 PIND, PIND3	 			//check to see if switch is released
		rjmp	 PollingLoop				//switch not released, loop until released
		

ret


// GetVoltage will store 10 bit voltage in memory
// CVoltageLSB is lsb of 10 bit voltage in memory
// CVoltageMS2 is MSB of 10 bit voltage	in memory
// r20 = Current voltage LSB
// r21 = Current voltage MS2
GetVoltage:

	push	r20
	push	r21

	in		r20, ADCL				//load the adc value into current voltage register
	in		r21, ADCH				//load the adc value into current voltage register

	sts		CVoltageLSB, r20		//save current voltage into memory 
	sts		CVoltageMS2, r21		//save current voltage into memory 

	pop		r21
	pop		r20


ret

//Scale multiplies voltage by 10 to get BCD value
//Then the MSB is saved to memory 
//CVoltageMS2 = r18
//CVoltageLSB = r17
//VBit0 BCD voltage LSB
//VBit1 BCD voltage middle bit
//VBit2 BCD voltage MSB
Scale:

	lds		r18, CVoltageMS2
	lds		r17, CVoltageLSB		//load voltage

	rcall	Multiply10
	sts		VBit2, r22				//put BCD MSB into memory

	mov		r18, r21
	mov		r17, r20				//load p0 and p1 into multiply

	rcall 	Multiply10
	sts		VBit1, r22				//put BCD middle bit into memory

	mov 	r18, r21
	mov		r17, r20				//load p0 and p1 into multiply

	rcall 	Multiply10
	sts		VBit0, r22				//put BCD LSB into memory

ret

//Multiply10 will multiply MSB and LSB by 10
//MSB = r18
//LSB = r17
//P0 = r20 (Product lsb)
//P1 = r21 (Product Middle bit)
//P2 = r22 (Product MSB)
Multiply10:
	push 	r16

	ldi		r19, 0x0A				//Multiplicand

	mul		r19, r17				//multiply multiplicand and lsb
	movw 	r20, r0					//move r0:r1 of product into p0:p1
	mul		r19, r18				//multiply multiplicand and msb
	mov		r22, r1					//move msb of product into p2
	add		r21, r0					//add partial products
	clr		r16
	adc		r22, r16				//add carry to p3


	pop		r16

ret

//Convert multiplies BCD value by 0x30 to produce ascii value
//then stores in memory:
//VBit0 BCD voltage LSB
//VBit1 BCD voltage middle bit
//VBit2 BCD voltage MSB
Convert:
	ldi		r17, 0x30
	lds		r16, VBit2
	add		r16, r17
	sts		VBit2, r16
	

	lds		r16, VBit1
	add		r16, r17
	sts		VBit1, r16


	lds		r16, VBit0
	add		r16, r17
	sts		VBit0, r16
ret

//Display writes voltage to screen: x.xxV ie 3.14V
Display:
	lds		r16, VBit2		//write first digit
	rcall	WriteData
	rcall	delay5


	ldi		r16, 0b00101110		//write decimal point
	rcall 	WriteData
	rcall	delay5


	lds		r16, VBit1			//write second digit
	rcall	WriteData
	rcall	delay5


	lds		r16, VBit0			//write third digit
	rcall	WriteData
	rcall	delay5


	ldi		r16, 0b01010110		//write "V"
	rcall	WriteData
	rcall	delay5


 	rcall 	ReturnHome			//move cursor back to "00"
	rcall 	delay5


ret

delay20:		//delay 20ms
	ldi		r19, 100
	ldi		r18, 66
	mov 	r11, r19 ; initialize slow (outer) counter 
	
	DelayLoop120:
		 mov 	 r10, r18 ; initialize fast (inner) counter 
	
	DelayLoop220: 
		 dec 	 r10 ; fast count 
		 brne 	 DelayLoop220 ; until r10 = 0 
		 dec	 r11 ; slow count 
		 brne	 DelayLoop120 ; until r11 = 0 

	ret ; done

delay5:		//delay 5ms
	ldi		r19, 100
	ldi		r18, 16
	mov 	r11, r19 ; initialize slow (outer) counter 
	
	DelayLoop15:
		 mov 	 r10, r18 ; initialize fast (inner) counter 
	
	DelayLoop25: 
		 dec 	 r10 ; fast count 
		 brne 	 DelayLoop25 ; until r10 = 0 
		 dec	 r11 ; slow count 
		 brne	 DelayLoop15 ; until r11 = 0 

	ret ; done

delay100us:  //delay 100us

	ldi		r19, 15
	ldi		r18, 2
	mov 	r11, r19 ; initialize slow (outer) counter 
	
	DelayLoop1100:
		 mov 	 r10, r18 ; initialize fast (inner) counter 
	
	DelayLoop2100: 
		 dec 	 r10 ; fast count 
		 brne 	 DelayLoop2100 ; until r10 = 0 
		 dec	 r11 ; slow count 
		 brne	 DelayLoop1100 ; until r11 = 0 

	ret ; done

ClearDisplay:
	ldi		r16, 0b00000001
	rcall 	WriteInstruction
ret

ReturnHome:

	ldi		r16, 0b00000010
	rcall	WriteInstruction
ret

//WriteData writes r16 to the display in data mode (RS High)
WriteData: //RS high
	
	sbi		PORTD, 0x01				//set rs high
	cbi		PORTD, 0x00				//set RW low
	cbi 	PORTD, 0x02				//enable low
	out		PORTB, r16		 		//data out to display
	sbi		PORTD, 0x02				//enable high
	cbi		PORTD, 0x02				//enable low

ret

//WriteInstruction writes r16 to the display in instruction mode (RS low)
WriteInstruction: //RS low
	cbi		PORTD, 0x01		 		//set rs low
	cbi		PORTD, 0x00				//set RW low
	cbi 	PORTD, 0x02				//enable low
	out		PORTB, r16		 		//data out to display
	sbi		PORTD, 0x02		 		//enable high
	cbi		PORTD, 0x02				//enable low
ret
	
// SRAM data storage
.dseg
.org		0x060









