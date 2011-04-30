;Chip type : ATmega8-16PI
;Internal Oscillator set at 8.000 Mhz.
;Author:  Robert Dunn - 2011


;PortB PB0 To pin #10 of Dsp 1 = A
;PortB PB1 To pin #9 of Dsp 1 = B
;PortB PB2 To pin #7 of Dsp 1 = C
;PortB PB3 To pin #5 of Dsp 1 = D
;PortB PB4 To pin #4 of Dsp 1 = E
;PortB PB5 To pin #2 of Dsp 1 = F
;PortB PB6 To pin #1 of Dsp 1 = G
;PortB PB7 To pin #6 of Dsp 1 = DP

;PortC PC0 ADC
;PortC PC1 Not in use
;PortC PC2 Not in use
;PortC PC3 Not in use
;PortC PC4 Not in use
;PortC PC5 Not in use
;PortC PC6 Not in use
;PortC PC7 Not in use

;PortD PD0 To pin #10 of Dsp 1 = A
;PortD PD1 To pin #9 of Dsp 1 = B
;PortD PD2 To pin #7 of Dsp 1 = C
;PortD PD3 To pin #5 of Dsp 1 = D
;PortD PD4 To pin #4 of Dsp 1 = E
;PortD PD5 To pin #2 of Dsp 1 = F
;PortD PD6 To pin #1 of Dsp 1 = G
;PortD PD7 To pin #6 of Dsp 1 = DP


.include	"M8def.inc"

.cseg
.org		0x0000                  //Reset Vector

.def        ADC_L = r16             //ADCL Register
.def        ADC_H = r17             //ADCH Register
.def        Wreg = r18              //Working register 
.def        ValAdc = r20            //ADC value
.def        Time = r24              //Delay Register     
.def        Timeout = r25           //Timeout Register


    rjmp	Start                  // jump over interrupt vector	

.org 0x000E                        //ADC Conversion Complete Interrupt vector:
rjmp ADC_ISR                       //jump to the "ADC_ISR"

Start:

// initialize stack pointer to point to end of memory

    ldi		Wreg, low(RAMEND)
	out     SPL, wreg
    ldi		Wreg, high(RAMEND)
    out		SPH, Wreg

// initialization  of ports
    ldi		Wreg,0b11111111      //set Port B to be all outputs
    out		DDRB, Wreg			 //Write it to the port.
	ldi Wreg, 0xFF               //set Port B to all off
	out PORTB, Wreg              //Write it to the port.

    ldi 	Wreg, 0b11111111     //set Port D to be all outputs
    out		DDRD, Wreg			 //Write it to the port.
	ldi Wreg, 0xFF               //set Port B to all off
	out PORTD, Wreg              //Write it to the port.
    

//Setup the ADC
    ldi Wreg, 0x05                //Load a Five into Wreg
    out ADMUX, Wreg               //Select channel Five of the ADC
    ldi Wreg, 0b11101111          //from left to right: ADC Enable, Start Conversion, Free-Running Mode, write
    out ADCSR, Wreg               //zero to ADC Int flag, enable int, prescaler: 111 for XTAL/64 @8 Mhz.
    clr	r16                       //Clear the register
    sei                           //enable interrupts


// main routine loop
Main:
         
		 rcall Compare1         //Find the correct digit to display on LED display #1
		 rcall Compare42        //Find the correct digit to display on LED display #2         

rjmp Main


// subroutines
;----------------------------------------------------------------------------------------
;         Compare for display #1
;----------------------------------------------------------------------------------------
Compare1:
     ldi Wreg, 0x03                     // 0 (00) Select a 0 for display on DSP#1
     cp ValAdc, Wreg
	 brlo  SetBit_0
	 rjmp Compare2
SetBit_0:
     rjmp DSP1_0    
;----------------------------------------------------------------------------------------
;         Compare for display #1
;----------------------------------------------------------------------------------------
Compare2:	 	 
     ldi Wreg, 0x0A                     // 0 (01) Select a 0 for display on DSP#1
	 cp ValAdc, Wreg
	 brlo SetBit_1
	 rjmp Compare3
SetBit_1:
     rjmp DSP1_0	 	 
;----------------------------------------------------------------------------------------
;         Compare for display #1
;----------------------------------------------------------------------------------------
Compare3:
     ldi Wreg, 0x0F                     // 0 (02) Select a 0 for display on DSP#1
	 cp ValAdc, Wreg
	 brlo SetBit_2
	 rjmp Compare4	 
SetBit_2:
     rjmp DSP1_0
;----------------------------------------------------------------------------------------
;         Compare for display #1
;----------------------------------------------------------------------------------------
Compare4:
     ldi Wreg, 0x15                     // 0 (03) Select a 0 for display on DSP#1
     cp ValAdc, Wreg
	 brlo SetBit_3
	 rjmp Compare5
SetBit_3:
     rjmp DSP1_0
;----------------------------------------------------------------------------------------
;         Compare for display #1
;----------------------------------------------------------------------------------------
Compare5:     
	 ldi Wreg, 0x1B                     // 0 (04) Select a 0 for display on DSP#1
     cp ValAdc, Wreg
	 brlo SetBit_4
	 rjmp Compare6
SetBit_4:
     rjmp DSP1_0
;----------------------------------------------------------------------------------------
;         Compare for display #1
;----------------------------------------------------------------------------------------
Compare6:        		  	 
     ldi Wreg, 0x21                     // 0 (05) Select a 0 for display on DSP#1
	 cp ValAdc, Wreg
	 brlo SetBit_5
	 rjmp Compare7
SetBit_5:
     rjmp DSP1_0
;----------------------------------------------------------------------------------------
;         Compare for display #1
;----------------------------------------------------------------------------------------
Compare7:
     ldi Wreg, 0x24                     // 0 (06) Select a 0 for display on DSP#1
     cp ValAdc, Wreg
	 brlo SetBit_6
	 rjmp Compare8
SetBit_6:
     rjmp DSP1_0
;----------------------------------------------------------------------------------------
;         Compare for display #1
;----------------------------------------------------------------------------------------
Compare8:	 
     ldi Wreg, 0x27                     // 0 (07) Select a 0 for display on DSP#1
	 cp ValAdc, Wreg
	 brlo SetBit_7
	 rjmp Compare9
SetBit_7:
     rjmp DSP1_0
;----------------------------------------------------------------------------------------
;         Compare for display #1
;----------------------------------------------------------------------------------------
Compare9:
     ldi Wreg, 0x33                     // 0 (08) Select a 0 for display on DSP#1
	 cp ValAdc, Wreg
	 brlo SetBit_8
	 rjmp Compare10
SetBit_8:
     rjmp DSP1_0
;----------------------------------------------------------------------------------------
;         Compare for display #1
;----------------------------------------------------------------------------------------
Compare10:
     ldi Wreg, 0x39                     // 0 (09) Select a 0 for display on DSP#1
     cp ValAdc, Wreg
	 brlo SetBit_9
	 rjmp Compare11
SetBit_9:
     rjmp DSP1_0
;----------------------------------------------------------------------------------------
;         Compare for display #1
;----------------------------------------------------------------------------------------
Compare11:    
	 ldi Wreg, 0x41                     // 1 (10) Select a 1 for display on DSP#1
     cp ValAdc, Wreg
	 brlo SetBit_10
	 rjmp Compare12
SetBit_10:
     rjmp DSP1_1
;----------------------------------------------------------------------------------------
;         Compare for display #1
;----------------------------------------------------------------------------------------
Compare12:     
     ldi Wreg, 0x45                     // 1 (11) Select a 1 for display on DSP#1
     cp ValAdc, Wreg
	 brlo SetBit_11
	 rjmp Compare13
SetBit_11:
     rjmp DSP1_1
;----------------------------------------------------------------------------------------
;         Compare for display #1
;----------------------------------------------------------------------------------------
Compare13:	 
     ldi Wreg, 0x4B                     // 1 (12) Select a 1 for display on DSP#1
	 cp ValAdc, Wreg
	 brlo SetBit_12
	 rjmp Compare14
SetBit_12:
     rjmp DSP1_1
;----------------------------------------------------------------------------------------
;         Compare for display #1
;----------------------------------------------------------------------------------------
Compare14:
     ldi Wreg, 0x51                     // 1 (13) Select a 1 for display on DSP#1
	 cp ValAdc, Wreg
	 brlo SetBit_13
	 rjmp Compare15
SetBit_13:
     rjmp DSP1_1  
;----------------------------------------------------------------------------------------
;         Compare for display #1
;----------------------------------------------------------------------------------------
Compare15:
     ldi Wreg, 0x57                     // 1 (14) Select a 1 for display on DSP#1
     cp ValAdc, Wreg
	 brlo SetBit_14
	 rjmp Compare16
SetBit_14:
     rjmp DSP1_1
;----------------------------------------------------------------------------------------
;         Compare for display #1
;----------------------------------------------------------------------------------------
Compare16:    
	 ldi Wreg, 0x5D                     // 1 (15) Select a 1 for display on DSP#1
     cp ValAdc, Wreg
	 brlo SetBit_15
	 rjmp Compare17
SetBit_15:
     rjmp DSP1_1
;----------------------------------------------------------------------------------------
;         Compare for display #1
;----------------------------------------------------------------------------------------
Compare17:
     ldi Wreg, 0x63                     // 1 (16) Select a 1 for display on DSP#1
	 cp ValAdc, Wreg
	 brlo SetBit_16
	 rjmp Compare18
SetBit_16:
     rjmp DSP1_1
 ;----------------------------------------------------------------------------------------
;         Compare for display #1
;----------------------------------------------------------------------------------------
Compare18:        
     ldi Wreg, 0x69                     // 1 (17) Select a 1 for display on DSP#1
     cp ValAdc, Wreg
	 brlo SetBit_17
	 rjmp Compare19
SetBit_17:
     rjmp DSP1_1
;----------------------------------------------------------------------------------------
;         Compare for display #1
;----------------------------------------------------------------------------------------
Compare19:	 
     ldi Wreg, 0x6F                     // 1 (18) Select a 1 for display on DSP#1
	 cp ValAdc, Wreg
	 brlo SetBit_18
	 rjmp Compare20
SetBit_18:
     rjmp DSP1_1
;----------------------------------------------------------------------------------------
;         Compare for display #1
;----------------------------------------------------------------------------------------
Compare20:
     ldi Wreg, 0x75                     // 1 (19) Select a 1 for display on DSP#1
	 cp ValAdc, Wreg
	 brlo SetBit_19
	 rjmp Compare21
SetBit_19:
     rjmp DSP1_1	 
;----------------------------------------------------------------------------------------
;         Compare for display #1
;----------------------------------------------------------------------------------------
Compare21:
     ldi Wreg, 0x81                     // 2 (20) Select a 2 for display on DSP#1
     cp ValAdc, Wreg
	 brlo SetBit_20
	 rjmp Compare22
SetBit_20:
     rjmp DSP1_2
;----------------------------------------------------------------------------------------
;         Compare for display #1
;----------------------------------------------------------------------------------------
Compare22:	 
     ldi Wreg, 0x7B                     // 2 (21) Select a 2 for display on DSP#1
	 cp ValAdc, Wreg
	 brlo SetBit_21
	 rjmp Compare23
SetBit_21:
     rjmp DSP1_2
;----------------------------------------------------------------------------------------
;         Compare for display #1
;----------------------------------------------------------------------------------------
Compare23:	 
     ldi Wreg, 0x87                     // 2 (22) Select a 2 for display on DSP#1
	 cp ValAdc, Wreg
	 brlo SetBit_24
	 rjmp Compare24
SetBit_22:
     rjmp DSP1_2
;----------------------------------------------------------------------------------------
;         Compare for display #1
;----------------------------------------------------------------------------------------
Compare24:	 
     ldi Wreg, 0x8D                     // 2 (23) Select a 2 for display on DSP#1
     cp ValAdc, Wreg
	 brlo SetBit_23
	 rjmp Compare25
SetBit_23:
     rjmp DSP1_2
;----------------------------------------------------------------------------------------
;         Compare for display #1
;----------------------------------------------------------------------------------------
Compare25:	     
	 ldi Wreg, 0x93                     // 2 (24) Select a 2 for display on DSP#1
     cp ValAdc, Wreg
	 brlo SetBit_24
	 rjmp Compare26
SetBit_24:
     rjmp DSP1_2
;----------------------------------------------------------------------------------------
;         Compare for display #1
;----------------------------------------------------------------------------------------
Compare26:	 
     ldi Wreg, 0x99                     // 2 (25) Select a 2 for display on DSP#1
	 cp ValAdc, Wreg
	 brlo SetBit_25
	 rjmp Compare27
SetBit_25:
     rjmp DSP1_2
;----------------------------------------------------------------------------------------
;         Compare for display #1
;----------------------------------------------------------------------------------------
Compare27:
     ldi Wreg, 0x9F                     // 2 (26) Select a 2 for display on DSP#1
     cp ValAdc, Wreg
	 brlo SetBit_26
	 rjmp Compare28
SetBit_26:
     rjmp DSP1_2
;----------------------------------------------------------------------------------------
;         Compare for display #1
;----------------------------------------------------------------------------------------
Compare28:	 	 
     ldi Wreg, 0xA5                     // 2 (27) Select a 2 for display on DSP#1
	 cp ValAdc, Wreg
	 brlo SetBit_27
	 rjmp Compare29
SetBit_27:
     rjmp DSP1_2
;----------------------------------------------------------------------------------------
;         Compare for display #1
;----------------------------------------------------------------------------------------
Compare29:	 
     ldi Wreg, 0xAD                     // 2 (28) Select a 2 for display on DSP#1
	 cp ValAdc, Wreg
	 brlo SetBit_28
	 rjmp Compare30
SetBit_28:
     rjmp DSP1_2
;----------------------------------------------------------------------------------------
;         Compare for display #1
;----------------------------------------------------------------------------------------
Compare30:	 
     ldi Wreg, 0xB1                     // 2 (29) Select a 2 for display on DSP#1
     cp ValAdc, Wreg
	 brlo SetBit_29
	 rjmp Compare31
SetBit_29:
     rjmp DSP1_2
;----------------------------------------------------------------------------------------
;         Compare for display #1
;----------------------------------------------------------------------------------------
Compare31:	     
	 ldi Wreg, 0xB7                     // 3 (30) Select a 3 for display on DSP#1
     cp ValAdc, Wreg
	 brlo SetBit_30
	 rjmp Compare32
SetBit_30:
     rjmp DSP1_3
;----------------------------------------------------------------------------------------
;         Compare for display #1
;----------------------------------------------------------------------------------------
Compare32:	     
     ldi Wreg, 0xBD                     // 3 (31) Select a 3 for display on DSP#1
     cp ValAdc, Wreg
	 brlo SetBit_31
	 rjmp Compare33
SetBit_31:
     rjmp DSP1_3
;----------------------------------------------------------------------------------------
;         Compare for display #1
;----------------------------------------------------------------------------------------
Compare33:	 
     ldi Wreg, 0xC3                     // 3 (32) Select a 3 for display on DSP#1
	 cp ValAdc, Wreg
	 brlo SetBit_32
	 rjmp Compare34
SetBit_32:
     rcall DSP1_3
;----------------------------------------------------------------------------------------
;         Compare for display #1
;----------------------------------------------------------------------------------------
Compare34:
     ldi Wreg, 0xC9                     // 3 (33) Select a 3 for display on DSP#1
	 cp ValAdc, Wreg
	 brlo SetBit_33
	 rjmp Compare35
SetBit_33:
     rjmp DSP1_3
;----------------------------------------------------------------------------------------
;         Compare for display #1
;----------------------------------------------------------------------------------------
Compare35:
     ldi Wreg, 0xCF                     // 3 (34) Select a 3 for display on DSP#1
     cp ValAdc, Wreg
	 brlo SetBit_34
	 rjmp Compare36
SetBit_34:
     rjmp DSP1_3
;----------------------------------------------------------------------------------------
;         Compare for display #1
;----------------------------------------------------------------------------------------
Compare36:	     
	 ldi Wreg, 0xD5                     // 3 (35) Select a 3 for display on DSP#1
     cp ValAdc, Wreg
	 brlo SetBit_35
	 rjmp Compare37
SetBit_35:
     rjmp DSP1_3
;----------------------------------------------------------------------------------------
;         Compare for display #1
;----------------------------------------------------------------------------------------
Compare37:	 
     ldi Wreg, 0xDB                     // 3 (36) Select a 3 for display on DSP#1
	 cp ValAdc, Wreg
	 brlo SetBit_36
	 rjmp Compare38
SetBit_36:
     rjmp DSP1_3
;----------------------------------------------------------------------------------------
;         Compare for display #1
;----------------------------------------------------------------------------------------
Compare38:
     ldi Wreg, 0xE1                     // 3 (37) Select a 3 for display on DSP#1
     cp ValAdc, Wreg
	 brlo SetBit_37
	 rjmp Compare39
SetBit_37:
     rjmp DSP1_3
;----------------------------------------------------------------------------------------
;         Compare for display #1
;----------------------------------------------------------------------------------------
Compare39:	 
     ldi Wreg, 0xE7                     // 3 (38) Select a 3 for display on DSP#1
	 cp ValAdc, Wreg
	 brlo SetBit_38
	 rjmp Compare40
SetBit_38:
     rjmp DSP1_3
;----------------------------------------------------------------------------------------
;         Compare for display #1
;----------------------------------------------------------------------------------------
Compare40:	 
     ldi Wreg, 0xEC                     // 3 (39) Select a 3 for display on DSP#1
	 cp ValAdc, Wreg
	 brlo SetBit_39
	 rjmp Compare41
SetBit_39:
     rjmp DSP1_3
;----------------------------------------------------------------------------------------
;         Compare for display #1
;----------------------------------------------------------------------------------------
Compare41:
     ldi Wreg, 0xED                     // 4 (40) Select a 4 for display on DSP#1
     cp ValAdc, Wreg
	 brsh SetBit_40
	 ret
SetBit_40:
     rjmp DSP1_4
;----------------------------------------------------------------------------------------
;         Compare for display #2
;----------------------------------------------------------------------------------------
Compare42:
     ldi Wreg, 0x03                     // 0 (00) Select a 0 for display on DSP#2
     cp ValAdc, Wreg
	 brlo SetBit_41
	 rjmp Compare43
SetBit_41:
     rjmp DSP2_0
;----------------------------------------------------------------------------------------
;         Compare for display #2
;----------------------------------------------------------------------------------------
Compare43:	 		 
     ldi Wreg, 0x0A                     // 1 (01) Select a 1 for display on DSP#2
	 cp ValAdc, Wreg
	 brlo SetBit_42
	 rjmp Compare44
SetBit_42:
     rjmp DSP2_1
;----------------------------------------------------------------------------------------
;         Compare for display #2
;----------------------------------------------------------------------------------------
Compare44:	 
     ldi Wreg, 0x0F                     // 2 (02) Select a 2 for display on DSP#2
	 cp ValAdc, Wreg
	 brlo SetBit_43
	 rjmp Compare45
SetBit_43:
     rjmp DSP2_2
;----------------------------------------------------------------------------------------
;         Compare for display #2
;----------------------------------------------------------------------------------------
Compare45:	 
     ldi Wreg, 0x15                     // 3 (03) Select a 3 for display on DSP#2
     cp ValAdc, Wreg
	 brlo SetBit_44
	 rjmp Compare46
SetBit_44:
     rjmp DSP2_3
;----------------------------------------------------------------------------------------
;         Compare for display #2
;----------------------------------------------------------------------------------------
Compare46:	      
	 ldi Wreg, 0x1B                     // 4 (04) Select a 4 for display on DSP#2
     cp ValAdc, Wreg
	 brlo SetBit_45
	 rjmp Compare47
SetBit_45:
     rjmp DSP2_4
;----------------------------------------------------------------------------------------
;         Compare for display #2
;----------------------------------------------------------------------------------------
Compare47:	 
     ldi Wreg, 0x21                     // 5 (05) Select a 5 for display on DSP#2
	 cp ValAdc, Wreg
	 brlo SetBit_46
	 rjmp Compare48
SetBit_46:
     rjmp DSP2_5
;----------------------------------------------------------------------------------------
;         Compare for display #2
;----------------------------------------------------------------------------------------
Compare48:	 
     ldi Wreg, 0x24                     // 6 (06) Select a 6 for display on DSP#2
     cp ValAdc, Wreg
	 brlo SetBit_47
	 rjmp Compare49
SetBit_47:
     rjmp DSP2_6
;----------------------------------------------------------------------------------------
;         Compare for display #2
;----------------------------------------------------------------------------------------
Compare49:	 
     ldi Wreg, 0x27                     // 7 (07) Select a 7 for display on DSP#2
	 cp ValAdc, Wreg
	 brlo SetBit_48
	 rjmp Compare50
SetBit_48:
     rjmp DSP2_7
;----------------------------------------------------------------------------------------
;         Compare for display #2
;----------------------------------------------------------------------------------------
Compare50:	 
     ldi Wreg, 0x33                     // 8 (08) Select a 8 for display on DSP#2
	 cp ValAdc, Wreg
	 brlo SetBit_49
	 rjmp Compare51
SetBit_49:
     rjmp DSP2_8
;----------------------------------------------------------------------------------------
;         Compare for display #2
;----------------------------------------------------------------------------------------
Compare51:	 
     ldi Wreg, 0x39                     // 9 (09) Select a 9 for display on DSP#2
     cp ValAdc, Wreg
	 brlo SetBit_50
	 rjmp Compare52
SetBit_50:
     rjmp DSP2_9
;----------------------------------------------------------------------------------------
;         Compare for display #2
;----------------------------------------------------------------------------------------
Compare52:	     
	 ldi Wreg, 0x41                     // 0 (10) Select a 0 for display on DSP#2
     cp ValAdc, Wreg
	 brlo SetBit_51
	 rjmp Compare53
SetBit_51:
     rjmp DSP2_0
;----------------------------------------------------------------------------------------
;         Compare for display #2
;----------------------------------------------------------------------------------------
Compare53:
     ldi Wreg, 0x45                     // 1 (11) Select a 1 for display on DSP#2
     cp ValAdc, Wreg
	 brlo SetBit_52
	 rjmp Compare54
SetBit_52:
     rjmp DSP2_1
;----------------------------------------------------------------------------------------
;         Compare for display #2
;----------------------------------------------------------------------------------------
Compare54:	 	 
     ldi Wreg, 0x4B                     // 2 (12) Select a 2 for display on DSP#2
	 cp ValAdc, Wreg
	 brlo SetBit_53
	 rjmp Compare55
SetBit_53:
     rjmp DSP2_2
;----------------------------------------------------------------------------------------
;         Compare for display #2
;----------------------------------------------------------------------------------------
Compare55:	 
     ldi Wreg, 0x51                     // 3 (13) Select a 3 for display on DSP#2
	 cp ValAdc, Wreg
	 brlo SetBit_54
	 rjmp Compare56
SetBit_54:
     rjmp DSP2_3
;----------------------------------------------------------------------------------------
;         Compare for display #2
;----------------------------------------------------------------------------------------
Compare56:	 
     ldi Wreg, 0x57                     // 4 (14) Select a 4 for display on DSP#2
     cp ValAdc, Wreg
	 brlo SetBit_55
	 rjmp Compare57
SetBit_55:
     rjmp DSP2_4
;----------------------------------------------------------------------------------------
;         Compare for display #2
;----------------------------------------------------------------------------------------
Compare57:	      
	 ldi Wreg, 0x5D                     // 5 (15) Select a 5 for display on DSP#2
     cp ValAdc, Wreg
	 brlo SetBit_56
	 rjmp Compare58
SetBit_56:
     rjmp DSP2_5
;----------------------------------------------------------------------------------------
;         Compare for display #2
;----------------------------------------------------------------------------------------
Compare58:	 
     ldi Wreg, 0x63                     // 6 (16) Select a 6 for display on DSP#2
	 cp ValAdc, Wreg
	 brlo SetBit_57
	 rjmp Compare59
SetBit_57:
     rjmp DSP2_6
;----------------------------------------------------------------------------------------
;         Compare for display #2
;----------------------------------------------------------------------------------------
Compare59:
     ldi Wreg, 0x69                     // 7 (17) Select a 7 for display on DSP#2
     cp ValAdc, Wreg
	 brlo SetBit_58
	 rjmp Compare60
SetBit_58:
     rjmp DSP2_7
;----------------------------------------------------------------------------------------
;         Compare for display #2
;----------------------------------------------------------------------------------------
Compare60:	 	 
     ldi Wreg, 0x6F                     // 8 (18) Select a 8 for display on DSP#2
	 cp ValAdc, Wreg
	 brlo SetBit_59
	 rjmp Compare61
SetBit_59:
     rjmp DSP2_8
;----------------------------------------------------------------------------------------
;         Compare for display #2
;----------------------------------------------------------------------------------------
Compare61:	 
     ldi Wreg, 0x75                     // 9 (19) Select a 9 for display on DSP#2
	 cp ValAdc, Wreg
	 brlo SetBit_60
	 rjmp Compare62
SetBit_60:
     rjmp DSP2_9
;----------------------------------------------------------------------------------------
;         Compare for display #2
;----------------------------------------------------------------------------------------
Compare62:	 	 
     ldi Wreg, 0x7B                     // 0 (20) Select a 0 for display on DSP#2
     cp ValAdc, Wreg
	 brlo SetBit_61
	 rjmp Compare63
SetBit_61:
     rjmp DSP2_0
;----------------------------------------------------------------------------------------
;         Compare for display #2
;----------------------------------------------------------------------------------------
Compare63:
     ldi Wreg, 0x81                     // 1 (21) Select a 1 for display on DSP#2
	 cp ValAdc, Wreg 
	 brlo SetBit_62
	 rjmp Compare64
SetBit_62:
     rjmp DSP2_1
;----------------------------------------------------------------------------------------
;         Compare for display #2
;----------------------------------------------------------------------------------------
Compare64:	 
     ldi Wreg, 0x87                     // 2 (22) Select a 2 for display on DSP#2
	 cp ValAdc, Wreg
	 brlo SetBit_63
	 rjmp Compare65
SetBit_63:
     rjmp DSP2_2
;----------------------------------------------------------------------------------------
;         Compare for display #2
;----------------------------------------------------------------------------------------
Compare65:
     ldi Wreg, 0x8D                     // 3 (23) Select a 3 for display on DSP#2
     cp ValAdc, Wreg
	 brlo SetBit_64
	 rjmp Compare66
SetBit_64:
     rjmp DSP2_3
;----------------------------------------------------------------------------------------
;         Compare for display #2
;----------------------------------------------------------------------------------------
Compare66:	      
	 ldi Wreg, 0x93                     // 4 (24) Select a 4 for display on DSP#2
     cp ValAdc, Wreg
	 brlo SetBit_67
	 rjmp Compare67
SetBit_65:
     rjmp DSP2_4
;----------------------------------------------------------------------------------------
;         Compare for display #2
;----------------------------------------------------------------------------------------
Compare67:	 
     ldi Wreg, 0x99                     // 5 (25) Select a 5 for display on DSP#2
	 cp ValAdc, Wreg
	 brlo SetBit_66
	 rjmp Compare68
SetBit_66:
     rjmp DSP2_5
;----------------------------------------------------------------------------------------
;         Compare for display #2
;----------------------------------------------------------------------------------------
Compare68:
     ldi Wreg, 0x9F                     // 6 (26) Select a 6 for display on DSP#2
     cp ValAdc, Wreg
	 brlo SetBit_67
	 rjmp Compare69
SetBit_67:
     rjmp DSP2_6
;----------------------------------------------------------------------------------------
;         Compare for display #2
;----------------------------------------------------------------------------------------
Compare69:	 	 
     ldi Wreg, 0xA5                     // 7 (27) Select a 7 for display on DSP#2
	 cp ValAdc, Wreg
	 brlo SetBit_68
	 rjmp Compare70
SetBit_68:
     rjmp DSP2_7
;----------------------------------------------------------------------------------------
;         Compare for display #2
;----------------------------------------------------------------------------------------
Compare70:	 
     ldi Wreg, 0xAD                     // 8 (28) Select a 8 for display on DSP#2
	 cp ValAdc, Wreg
	 brlo SetBit_69
	 rjmp Compare71
SetBit_69:
     rjmp DSP2_8
;----------------------------------------------------------------------------------------
;         Compare for display #2
;----------------------------------------------------------------------------------------
Compare71:	 
     ldi Wreg, 0xB1                     // 9 (29) Select a 9 for display on DSP#2
     cp ValAdc, Wreg
	 brlo SetBit_70
	 rjmp Compare72
SetBit_70:
     rjmp DSP2_9
;----------------------------------------------------------------------------------------
;         Compare for display #2
;----------------------------------------------------------------------------------------
Compare72:	     
	 ldi Wreg, 0xB7                     // 0 (30) Select a 0 for display on DSP#2
     cp ValAdc, Wreg
	 brlo SetBit_71
	 rjmp Compare73
SetBit_71:
     rjmp DSP2_0
;----------------------------------------------------------------------------------------
;         Compare for display #2
;----------------------------------------------------------------------------------------
Compare73:	      
     ldi Wreg, 0xBD                     // 1 (31) Select a 1 for display on DSP#2
     cp ValAdc, Wreg
	 brlo SetBit_72
	 rjmp Compare74
SetBit_72:
     rjmp DSP2_1
;----------------------------------------------------------------------------------------
;         Compare for display #2
;----------------------------------------------------------------------------------------
Compare74:	 	 
     ldi Wreg, 0xC3                     // 2 (32) Select a 2 for display on DSP#2
	 cp ValAdc, Wreg
	 brlo SetBit_73
	 rjmp Compare75
SetBit_73:
     rjmp DSP2_2
;----------------------------------------------------------------------------------------
;         Compare for display #2
;----------------------------------------------------------------------------------------
Compare75:	 
     ldi Wreg, 0xC9                     // 3 (33) Select a 3 for display on DSP#2
	 cp ValAdc, Wreg
	 brlo SetBit_74
	 rjmp Compare76
SetBit_74:
     rjmp DSP2_3
;----------------------------------------------------------------------------------------
;         Compare for display #2
;----------------------------------------------------------------------------------------
Compare76:
     ldi Wreg, 0xCF                     // 4 (34) Select a 4 for display on DSP#2
     cp ValAdc, Wreg
	 brlo SetBit_75
	 rjmp Compare77
SetBit_75:
     rjmp DSP2_4
;----------------------------------------------------------------------------------------
;         Compare for display #2
;----------------------------------------------------------------------------------------
Compare77:	      
	 ldi Wreg, 0xD5                     // 5 (35) Select a 5 for display on DSP#2
     cp ValAdc, Wreg
	 brlo SetBit_76
	 rjmp Compare78
SetBit_76:
     rjmp DSP2_5
;----------------------------------------------------------------------------------------
;         Compare for display #2
;----------------------------------------------------------------------------------------
Compare78:	 
     ldi Wreg, 0xDB                     // 6 (36) Select a 6 for display on DSP#2
	 cp ValAdc, Wreg
	 brlo SetBit_77
	 rjmp Compare79
SetBit_77:
     rjmp DSP2_6
;----------------------------------------------------------------------------------------
;         Compare for display #2
;----------------------------------------------------------------------------------------
Compare79:
     ldi Wreg, 0xE1                     // 7 (37) Select a 7 for display on DSP#2
     cp ValAdc, Wreg
	 brlo SetBit_78
	 rjmp Compare80
SetBit_78:
     rjmp DSP2_7
;----------------------------------------------------------------------------------------
;         Compare for display #2
;----------------------------------------------------------------------------------------
Compare80:	 	 
     ldi Wreg, 0xE7                     // 8 (38) Select a 8 for display on DSP#2
	 cp ValAdc, Wreg
	 brlo SetBit_79
	 rjmp Compare81
SetBit_79:
     rjmp DSP2_8
;----------------------------------------------------------------------------------------
;         Compare for display #2
;----------------------------------------------------------------------------------------
Compare81:	 
     ldi Wreg, 0xEC                     // 9 (39) Select a 9 for display on DSP#2
	 cp ValAdc, Wreg
	 brlo SetBit_80
	 rjmp Compare82
SetBit_80:
     rjmp DSP2_9
;----------------------------------------------------------------------------------------
;         Compare for display #2
;----------------------------------------------------------------------------------------
Compare82:
     ldi Wreg, 0xED                     // 0 (40) Select a 0 for display on DSP#2
     cp ValAdc, Wreg
	 brsh SetBit_81
	 ret
SetBit_81:
     rjmp DSP2_0	 
;----------------------------------------------------------------------------------------
;         Display a Zero on DSP1
;----------------------------------------------------------------------------------------
DSP1_0:          
          ldi		Wreg,0b11000000      //set Port B bits
          out		PORTB, Wreg			 //Write it to the port
		  ret
;----------------------------------------------------------------------------------------
;         Display a One on DSP1 
;----------------------------------------------------------------------------------------
DSP1_1:
          ldi		Wreg,0b11111001      //set Port B bits
          out		PORTB, Wreg			 //Write it to the port
		  ret   
;----------------------------------------------------------------------------------------
;         Display a Two on DSP1
;----------------------------------------------------------------------------------------
DSP1_2: 
          ldi		Wreg,0b10100100      //set Port B bits
          out		PORTB, Wreg			 //Write it to the port        
		  ret	  
;----------------------------------------------------------------------------------------
;         Display a Three on DSP1
;----------------------------------------------------------------------------------------
DSP1_3: 
          ldi		Wreg,0b10110000      //set Port B bits
          out		PORTB, Wreg			 //Write it to the port 
		  ret	  
;----------------------------------------------------------------------------------------
;         Display a Four on DSP1
;----------------------------------------------------------------------------------------
DSP1_4: 
          ldi		Wreg,0b10011001      //set Port B bits
          out		PORTB, Wreg			 //Write it to the port 
		  ret	  
;----------------------------------------------------------------------------------------
;         Display a Five on DSP1
;----------------------------------------------------------------------------------------
DSP1_5: 
          ldi		Wreg,0b10010010      //set Port B bits
          out		PORTB, Wreg			 //Write it to the port
		  ret
;----------------------------------------------------------------------------------------
;         Display a Six on DSP1
;----------------------------------------------------------------------------------------
DSP1_6: 
          ldi		Wreg,0b10000010      //set Port B bits
          out		PORTB, Wreg			 //Write it to the port
		  ret	  
;----------------------------------------------------------------------------------------
;         Display a Seven on DSP1
;----------------------------------------------------------------------------------------
DSP1_7: 
          ldi		Wreg,0b11111000      //set Port B bits
          out		PORTB, Wreg			 //Write it to the port
		  ret
;----------------------------------------------------------------------------------------
;         Display a Eight on DSP1
;----------------------------------------------------------------------------------------
DSP1_8: 
          ldi		Wreg,0b10000000      //set Port B bits
          out		PORTB, Wreg			 //Write it to the port 
		  ret
;----------------------------------------------------------------------------------------
;         Display a Nine on DSP1
;----------------------------------------------------------------------------------------
DSP1_9: 
          ldi		Wreg,0b10011000      //set Port B bits
          out		PORTB, Wreg			 //Write it to the port
		  ret
;----------------------------------------------------------------------------------------
;         Display a Decimal point on DSP1
;----------------------------------------------------------------------------------------
DSP1_D: 
          ldi		Wreg,0b01111111      //set Port B bits
          out		PORTB, Wreg			 //Write it to the port.                
		  ret
		  
		  	  		  
;----------------------------------------------------------------------------------------
;         Display a Zero on DSP2
;----------------------------------------------------------------------------------------
DSP2_0: 
          ldi		Wreg,0b11000000      //set Port D bits
          out		PORTD, Wreg			 //Write it to the port
		  ret          
;----------------------------------------------------------------------------------------
;         Display a One on DSP2 
;----------------------------------------------------------------------------------------
DSP2_1: 
          ldi		Wreg,0b11111001      //set Port D bits
          out		PORTD, Wreg			 //Write it to the port
		  ret 
;----------------------------------------------------------------------------------------
;         Display a Two on DSP2
;----------------------------------------------------------------------------------------
DSP2_2: 
          ldi		Wreg,0b10100100      //set Port D bits
          out		PORTD, Wreg			 //Write it to the port        
		  ret		  
;----------------------------------------------------------------------------------------
;         Display a Three on DSP2
;----------------------------------------------------------------------------------------
DSP2_3: 
          ldi		Wreg,0b10110000      //set Port D bits
          out		PORTD, Wreg			 //Write it to the port 
		  ret		  
;----------------------------------------------------------------------------------------
;         Display a Four on DSP2
;----------------------------------------------------------------------------------------
DSP2_4: 
          ldi		Wreg,0b10011001      //set Port D bits
          out		PORTD, Wreg			 //Write it to the port 
		  ret		  
;----------------------------------------------------------------------------------------
;         Display a Five on DSP2
;----------------------------------------------------------------------------------------
DSP2_5: 
          ldi		Wreg,0b10010010      //set Port D bits
          out		PORTD, Wreg			 //Write it to the port
		  ret		  
;----------------------------------------------------------------------------------------
;         Display a Six on DSP2
;----------------------------------------------------------------------------------------
DSP2_6: 
          ldi		Wreg,0b10000010      //set Port D bits
          out		PORTD, Wreg			 //Write it to the port
		  ret		  
;----------------------------------------------------------------------------------------
;         Display a Seven on DSP2
;----------------------------------------------------------------------------------------
DSP2_7: 
          ldi		Wreg,0b11111000      //set Port D bits
          out		PORTD, Wreg			 //Write it to the port
		  ret
;----------------------------------------------------------------------------------------
;         Display a Eight on DSP2
;----------------------------------------------------------------------------------------
DSP2_8: 
          ldi		Wreg,0b10000000      //set Port D bits
          out		PORTD, Wreg			 //Write it to the port 
		  ret
;----------------------------------------------------------------------------------------
;         Display a Nine on DSP2
;----------------------------------------------------------------------------------------
DSP2_9: 
          ldi		Wreg,0b10011000      //set Port D bits
          out		PORTD, Wreg			 //Write it to the port
		  ret
;----------------------------------------------------------------------------------------
;         Display a Decimal point on DSP2
;----------------------------------------------------------------------------------------
DSP2_D: 
          ldi		Wreg,0b01111111      //set Port D bits
          out		PORTD, Wreg			 //Write it to the port.                
		  ret		  		  
;----------------------------------------------------------------------------------------
;         ADC Interupt service routine
;----------------------------------------------------------------------------------------
ADC_ISR:
     push ADC_L                   //push ADC_L on stack
     in ADC_L, SREG               //use ADC_L to save SREG
     push ADC_L                   //push both on stack
     push ADC_H                   //Push ADC_H on stack


	 in ADC_L, ADCL              //get the last ADC result, low byte first.
     in ADC_H, ADCH              //Get the high byte
     lsr ADC_H                   //shift ADC result right (2 bits)
     ror ADC_L                   //by first shifting out bit 0 of ADC_L, then shifting it into ADC_H
     lsr ADC_H                   //
     ror ADC_L                   //(twice)

	 ;com ADC_L                   //now invert result,
	 movw ValAdc, ADC_L
     ;out PortD, ADC_L            //and write to port.

	 pop ADC_H
     pop ADC_L
     out SREG, ADC_L
     pop ADC_L
	 sbi	ADCSRA,ADSC	       //start adc conversion 
reti		  		  		  		  		  		          
;=========================================================================
; Delay routines
;=========================================================================
delay:	
		
		ldi 	Time,255                     
			
cagain:		
        dec	Time
		brne 	cagain
		dec 	timeout
		brne 	delay
		ret

dly:
	ser	r17

dly1:
	ser	r18

dly2:
	dec	r18
	brne dly2
	dec r17
	brne dly1
	ret


	
// SRAM data storage
.dseg
.org		0x060
