/* ---- Public Constants and Types --------------------------------------- */

/* Pin Table */ 
#define PCB_Pin1       D, PD7 //illegal pin defaults to Pin5
#define PCB_Pin2       D, PD7 //illegal pin defaults to Pin5 
#define PCB_Pin3       D, PD7 //illegal pin defaults to Pin5
#define PCB_Pin4       D, PD7 //illegal pin defaults to Pin5
#define PCB_Pin5       D, PD7 
#define PCB_Pin6       D, PD6
#define PCB_Pin7       D, PD5
#define PCB_Pin8       D, PD4
#define PCB_Pin9       D, PD3
#define PCB_Pin10      D, PD2
#define PCB_Pin11      D, PD1
#define PCB_Pin12      D, PD0
#define PCB_Pin13      E, PE7
#define PCB_Pin14      E, PE6
#define PCB_Pin15      E, PE5
#define PCB_Pin16      E, PE4
#define PCB_Pin17      E, PE3
#define PCB_Pin18      E, PE2
#define PCB_Pin19      E, PE1
#define PCB_Pin20      E, PE0
#define PCB_Pin21      B, PB7
#define PCB_Pin22      B, PB6
#define PCB_Pin23      B, PB5
#define PCB_Pin24      B, PB4
#define PCB_Pin25      B, PB3
#define PCB_Pin26      B, PB2
#define PCB_Pin27      B, PB1
#define PCB_Pin28      B, PB0
#define PCB_Pin29      F, PF7
#define PCB_Pin30      F, PF6
#define PCB_Pin31      F, PF5
#define PCB_Pin32      F, PF4
#define PCB_Pin33      F, PF3
#define PCB_Pin34      F, PF2
#define PCB_Pin35      F, PF1
#define PCB_Pin36      F, PF0
#define PCB_Pin37      D, PD7 //illegal pin defaults to Pin5
#define PCB_Pin38      D, PD7 //illegal pin defaults to Pin5
#define PCB_Pin39      D, PD7 //illegal pin defaults to Pin5
#define PCB_Pin40      D, PD7 //illegal pin defaults to Pin5


/* HAL Macros */ 
#define _PORTL3(p, b)    PORT ## p 
#define _PORTL2(x)       _PORTL3(x) 
#define _PORTL1(x)       _PORTL2(PCB_Pin ## x) 

#define _PINL3(p, b)      PIN  ## p 
#define _PINL2(x)        _PINL3(x) 
#define _PINL1(x)        _PINL2(PCB_Pin ## x) 

#define _DDRL3(p, b)     DDR  ## p 
#define _DDRL2(x)        _DDRL3(x) 
#define _DDRL1(x)        _DDRL2(PCB_Pin ## x) 

#define _MASKL3(p, b)    (1 << b) 
#define _MASKL2(x)       _MASKL3(x) 
#define _MASKL1(x)       _MASKL2(PCB_Pin ## x) 

#define _BITNUML3(p, b)  b 
#define _BITNUML2(x)     _BITNUML3(x) 
#define _BITNUML1(x)     _BITNUML2(PCB_Pin ## x) 

#define PORT(x)          _PORTL1(x) 
#define PIN(x)           _PINL1(x) 
#define DDR(x)           _DDRL1(x)) 
#define MASK(x)          _MASKL1(x) 
#define BITNUM(x)        _BITNUML1(x) 

/* Bonus Points - Bit Macros */ 
#define SetAsOutput(x)   DDR(x)   |=  MASK(x) 
#define SetAsInput(x)    DDR(x)   &= ~MASK(x) 
#define SetOutputHigh(x) PORT(x)  |=  MASK(x) 
#define SetOutputLow(x)  PORT(x)  &= ~MASK(x) 
#define ToggleOutput(x)  PORT(x)  ^=  MASK(x) 
#define OutputStatus(x)  (PORT(x) &   MASK(x)) 
#define InputStatus(x)   (PIN(x)  &   MASK(x)) 
