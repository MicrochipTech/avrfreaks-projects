/****************************************************************************
 Title  :   C  file for the HEX KEYBOARD FUNCTIONS library (hkb.c)
 Author:    Chris efstathiou hendrix@otenet.gr
 Date:      15/2/2003 7:27:46 μμ
 Software:  AVR-GCC with AVR-AS
 Target:    any AVR device
 Comments:  This software is FREE.

*****************************************************************************/

/* Although i could had written it in the new style, i prefer the old one for compatibility sake. */




/********************************************************************************************************/
/*                                   PREPROCESSOR DIRECTIVES                                            */
/********************************************************************************************************/

#ifndef _IO_REG_MACRO_MODE_
#define _IO_REG_MACRO_MODE_  1     /* In case you have the new assignment mode io headers */
#endif

#ifndef  _SFR_ASM_COMPAT
#define  _SFR_ASM_COMPAT     1     /* This is for GCC 3.2 */
#endif


#ifndef AVRGCC_VERSION
#define AVRGCC_VERSION       330        /* AVRGCC version for including the correct files  */
#endif

/*------------------------------------------------------------------------------------------------------*/
#if AVRGCC_VERSION == 330
/*------------------------------------------------------------------------------------------------------*/

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <stdlib.h>
#include "hkb.h"
#if HEX_KEYB_WILL_USE_INTERRUPT == 1
#include <avr/signal.h>
#endif

/*------------------------------------------------------------------------------------------------------*/
#else
/*------------------------------------------------------------------------------------------------------*/

#include <io.h>
#include <pgmspace.h>
#include <stdlib.h>
#include "hkb.h"
#if HEX_KEYB_WILL_USE_INTERRUPT == 1
#include <sig-avr.h>
#endif

/*------------------------------------------------------------------------------------------------------*/
#endif
/*------------------------------------------------------------------------------------------------------*/

#if HEX_KEYB_WILL_USE_I2C == 1
#include "i2c.h"
#endif

#ifndef _VECTOR
#define _VECTOR(N) __vector_ ## N  
#endif

#if !defined(outp)
#define outp(value,port)  outb(port,value)
#endif

#if !defined(inp)
#define inp(port)  inb(port)
#endif

#if defined(__AVR_ATmega103__)
#define DDRC   0
#define DDRF   0
#endif

#ifndef CONCAT1
#define CONCAT1(a, b) CONCAT2(a, b)
#endif

#ifndef CONCAT2
#define CONCAT2(a, b) a ## b
#endif

#if HEX_KEYB_WILL_USE_I2C == 0

#define HEX_KEYB_WILL_USE_ONE_PORT  1

#define R1_DDR_REG  CONCAT1(DDR, ROW1_PORT)
#define R1_PIN_REG  CONCAT1(PIN, ROW1_PORT)
#define R1_OUT_REG  CONCAT1(PORT,ROW1_PORT)

#define R2_DDR_REG  CONCAT1(DDR, ROW2_PORT)
#define R2_PIN_REG  CONCAT1(PIN, ROW2_PORT)
#define R2_OUT_REG  CONCAT1(PORT,ROW2_PORT)

#if R1_DDR_REG != R2_DDR_REG  &&  HEX_KEYB_WILL_USE_ONE_PORT == 1
#undef  HEX_KEYB_WILL_USE_ONE_PORT
#define HEX_KEYB_WILL_USE_ONE_PORT  0
#endif

#define R3_DDR_REG  CONCAT1(DDR, ROW3_PORT)
#define R3_PIN_REG  CONCAT1(PIN, ROW3_PORT)
#define R3_OUT_REG  CONCAT1(PORT,ROW3_PORT)

#if R2_DDR_REG != R3_DDR_REG  &&  HEX_KEYB_WILL_USE_ONE_PORT == 1  
#undef  HEX_KEYB_WILL_USE_ONE_PORT
#define HEX_KEYB_WILL_USE_ONE_PORT  0
#endif

#define R4_DDR_REG  CONCAT1(DDR, ROW4_PORT)
#define R4_PIN_REG  CONCAT1(PIN, ROW4_PORT)
#define R4_OUT_REG  CONCAT1(PORT,ROW4_PORT)

#if R3_DDR_REG != R4_DDR_REG  &&  HEX_KEYB_WILL_USE_ONE_PORT == 1  
#undef  HEX_KEYB_WILL_USE_ONE_PORT
#define HEX_KEYB_WILL_USE_ONE_PORT  0
#endif

#define C1_DDR_REG  CONCAT1(DDR, COL1_PORT)
#define C1_PIN_REG  CONCAT1(PIN, COL1_PORT)
#define C1_OUT_REG  CONCAT1(PORT,COL1_PORT)

#if R4_DDR_REG != C1_DDR_REG  &&  HEX_KEYB_WILL_USE_ONE_PORT == 1  
#undef  HEX_KEYB_WILL_USE_ONE_PORT
#define HEX_KEYB_WILL_USE_ONE_PORT  0
#endif

#define C2_DDR_REG  CONCAT1(DDR, COL2_PORT)
#define C2_PIN_REG  CONCAT1(PIN, COL2_PORT)
#define C2_OUT_REG  CONCAT1(PORT,COL2_PORT)

#if C1_DDR_REG != C2_DDR_REG  &&  HEX_KEYB_WILL_USE_ONE_PORT == 1  
#undef  HEX_KEYB_WILL_USE_ONE_PORT
#define HEX_KEYB_WILL_USE_ONE_PORT  0
#endif

#define C3_DDR_REG  CONCAT1(DDR, COL3_PORT)
#define C3_PIN_REG  CONCAT1(PIN, COL3_PORT)
#define C3_OUT_REG  CONCAT1(PORT,COL3_PORT)

#if C2_DDR_REG != C3_DDR_REG  &&  HEX_KEYB_WILL_USE_ONE_PORT == 1  
#undef  HEX_KEYB_WILL_USE_ONE_PORT
#define HEX_KEYB_WILL_USE_ONE_PORT  0
#endif

#define C4_DDR_REG  CONCAT1(DDR, COL4_PORT)
#define C4_PIN_REG  CONCAT1(PIN, COL4_PORT)
#define C4_OUT_REG  CONCAT1(PORT,COL4_PORT)

#if C3_DDR_REG != C4_DDR_REG  &&  HEX_KEYB_WILL_USE_ONE_PORT == 1  
#undef  HEX_KEYB_WILL_USE_ONE_PORT
#define HEX_KEYB_WILL_USE_ONE_PORT  0
#endif


#if HEX_KEYB_WILL_USE_ONE_PORT == 1
#define HEX_KEYB_PORT_DDR_REG       CONCAT1(DDR, ROW1_PORT)
#define HEX_KEYB_PORT_PIN_REG       CONCAT1(PIN, ROW1_PORT)
#define HEX_KEYB_PORT_OUT_REG       CONCAT1(PORT, ROW1_PORT)
#endif

#if  HEX_KEYB_WILL_USE_ONE_PORT == 1
#define CURSOR_ADVANCE_VALUE   (CURSOR_ADVANCE_TIME-1)
#warning " HEX_KEYBOARD USES 1 PORT "
#elif  HEX_KEYB_WILL_USE_ONE_PORT == 0
#define CURSOR_ADVANCE_VALUE   (CURSOR_ADVANCE_TIME-1)
#warning " HEX_KEYBOARD PINS ON VARIUS PORTS "
#endif

#elif HEX_KEYB_WILL_USE_I2C == 1

#if NUMBER_OF_HEX_KEYBOARDS > 8
#warning "TOO MANY HEX KEYBOARDS"
#endif

#define CURSOR_ADVANCE_VALUE   (CURSOR_ADVANCE_TIME-1)
#warning "HEX_KEYBOARD WILL USE I2C BUS"

#endif   /* #if HEX_KEYB_WILL_USE_I2C == 0 */

/* CONVERSION OF TRUE TIME TO A VALUE SUITABLE FOR THE kgets() FUNCTION */ 


/* CONVERSION OF TRUE TIME TO A VALUE SUITABLE FOR THE DELAY FUNCTION */
#define TIME_100_MS        ((100*(F_CPU/6000))-3)
#define TIME_99_MS         ((99*(F_CPU/6000))-3)
#define TIME_30_MS         ((30*(F_CPU/6000))-3)
#define TIME_10_MS         ((10*(F_CPU/6000))-3)
#define TIME_1_MS          ((1*(F_CPU/6000))-3)
#define TIME_100_US        ((F_CPU/60000)-3)

/*******************************************************************************************************/

#if  HEX_KEYB_WILL_USE_INTERRUPT == 1
#warning " HEX_KEYBOARD USES AN EXTERNAL INTERRUPT "
/* DEVICE SPECIFIC EXTERNAL INTERRUPT RELATED REGISTER NAMES  */
#if defined(__AVR_ATmega103__) 

#define HEX_KEYB_INT_SENSE_REG      EICR
#define HEX_KEYB_INT_FLAG_REG       EIFR
#define HEX_KEYB_INT_REG            EIMSK

#elif  defined(__AVR_ATmega128__) 

#if   HEX_KEYB_INTERRUPT <= 3
#define HEX_KEYB_INT_SENSE_REG      EICRA
#elif HEX_KEYB_INTERRUPT >= 4 
#define HEX_KEYB_INT_SENSE_REG      EICRB
#endif
#define HEX_KEYB_INT_FLAG_REG       EIFR
#define HEX_KEYB_INT_REG            EIMSK

#else

#define HEX_KEYB_INT_SENSE_REG      MCUCR
#define HEX_KEYB_INT_FLAG_REG       GIFR
#define HEX_KEYB_INT_REG            GIMSK

#endif

/* SELECT THE APPROPRIATE SIGNAL NAME, ACTIVATION BIT AND SENSE BIT OF THE SPECIFIED EXTERNAL INTERRUPT */
#if     HEX_KEYB_INTERRUPT == 0

#define HEX_KEYB_INT_NAME            SIG_INTERRUPT0
#define HEX_KEYB_INT_BIT             INT0 
#define HEX_KEYB_INT_SENSE_BIT_0     ISC00
#define HEX_KEYB_INT_SENSE_BIT_1     ISC01
#define HEX_KEYB_INT_FLAG            INTF0 

#elif   HEX_KEYB_INTERRUPT == 1

#define HEX_KEYB_INT_NAME            SIG_INTERRUPT1
#define HEX_KEYB_INT_BIT             INT1 
#define HEX_KEYB_INT_SENSE_BIT_0     ISC10
#define HEX_KEYB_INT_SENSE_BIT_1     ISC11
#define HEX_KEYB_INT_FLAG            INTF1 

#elif   HEX_KEYB_INTERRUPT == 2

#define HEX_KEYB_INT_NAME            SIG_INTERRUPT2
#define HEX_KEYB_INT_BIT             INT2 
#define HEX_KEYB_INT_SENSE_BIT_0     ISC20
#define HEX_KEYB_INT_SENSE_BIT_1     ISC21
#define HEX_KEYB_INT_FLAG            INTF2
 
#elif   HEX_KEYB_INTERRUPT == 3

#define HEX_KEYB_INT_NAME            SIG_INTERRUPT3
#define HEX_KEYB_INT_BIT             INT3 
#define HEX_KEYB_INT_SENSE_BIT_0     ISC30
#define HEX_KEYB_INT_SENSE_BIT_1     ISC31
#define HEX_KEYB_INT_FLAG            INTF3

#elif   HEX_KEYB_INTERRUPT == 4

#define HEX_KEYB_INT_NAME            SIG_INTERRUPT4
#define HEX_KEYB_INT_BIT             INT4 
#define HEX_KEYB_INT_SENSE_BIT_0     ISC40
#define HEX_KEYB_INT_SENSE_BIT_1     ISC41
#define HEX_KEYB_INT_FLAG            INTF4

#elif   HEX_KEYB_INTERRUPT == 5

#define HEX_KEYB_INT_NAME            SIG_INTERRUPT5
#define HEX_KEYB_INT_BIT             INT5 
#define HEX_KEYB_INT_SENSE_BIT_0     ISC50
#define HEX_KEYB_INT_SENSE_BIT_1     ISC51
#define HEX_KEYB_INT_FLAG            
#elif   HEX_KEYB_INTERRUPT == 6

#define HEX_KEYB_INT_NAME            SIG_INTERRUPT6
#define HEX_KEYB_INT_BIT             INT6 
#define HEX_KEYB_INT_SENSE_BIT_0     ISC60
#define HEX_KEYB_INT_SENSE_BIT_1     ISC61
#define HEX_KEYB_INT_FLAG            INTF6

#elif   HEX_KEYB_INTERRUPT == 7

#define HEX_KEYB_INT_NAME            SIG_INTERRUPT7
#define HEX_KEYB_INT_BIT             INT7 
#define HEX_KEYB_INT_SENSE_BIT_0     ISC70
#define HEX_KEYB_INT_SENSE_BIT_1     ISC71
#define HEX_KEYB_INT_FLAG            INTF7 

#endif  

#endif /* #if HEX_KEYB_WILL_USE_INTERRUPT == 1 */

/********************************************************************************************************/
/*                                   TYPE DEFINITIONS                                                   */
/********************************************************************************************************/

typedef unsigned char u08;
typedef char          s08;
typedef unsigned int  u16;
typedef int           s16;

/********************************************************************************************************/
/*                                   GLOBAL VARIABLES                                                   */
/********************************************************************************************************/

#if HEX_KEYB_WILL_USE_INTERRUPT == 1
volatile unsigned char disable_keyb_int=0, key_pressed=0; 
void (*ptr)(unsigned char code);
#endif

#if HEX_KEYB_WILL_USE_I2C == 1 && NUMBER_OF_HEX_KEYBOARDS >= 2
unsigned char hkb_i2c_address_array[NUMBER_OF_HEX_KEYBOARDS];
unsigned char hkb_i2c_address=HEX_KEYB_0_I2C_ADDRESS;
unsigned char hkb_id=HKB_0;
unsigned char temp1=HKB_0;
unsigned char temp2=HKB_0;
unsigned char hkb_init_lock=0;
#endif

/********************************************************************************************************/
/*                                   LOCAL FUNCTION PROTOTYPES                                          */
/********************************************************************************************************/

static void wait(unsigned long number_of_loops);
#if HEX_KEYB_WILL_USE_ONE_PORT == 0 && HEX_KEYB_WILL_USE_I2C == 0
static void row_pins_as_inputs(void);
static void column_pins_as_inputs(void);
#elif HEX_KEYB_WILL_USE_I2C == 1 && NUMBER_OF_HEX_KEYBOARDS >= 2
static void hkb_configure_io(void);
#endif
/********************************************************************************************************/
/*                                   LOCAL FUNCTION DEFINITIONS                                         */
/********************************************************************************************************/

static void wait(unsigned long number_of_loops) 
{  
/* 
   6 cpu cycles per loop + 17 cycles overhead(18 are pre-subtracted in the definition of DELAY).
   (4 to load the us variable, 4 to call the delay function, 5 to test for us == 0 and 4 to return. 
*/
   while(number_of_loops) { number_of_loops--; }
   
}
/*######################################################################################################*/
#if HEX_KEYB_WILL_USE_ONE_PORT == 0 && HEX_KEYB_WILL_USE_I2C == 0
static void row_pins_as_inputs(void)
{

cbi(R1_DDR_REG, ROW1_PIN);
cbi(R2_DDR_REG, ROW2_PIN);
cbi(R3_DDR_REG, ROW3_PIN);
cbi(R4_DDR_REG, ROW4_PIN);
sbi(C1_DDR_REG, COL1_PIN);
sbi(C2_DDR_REG, COL2_PIN);
sbi(C3_DDR_REG, COL3_PIN);
sbi(C4_DDR_REG, COL4_PIN);

/* Turn on the pull up resistors */
sbi(R1_OUT_REG, ROW1_PIN);
sbi(R2_OUT_REG, ROW2_PIN);
sbi(R3_OUT_REG, ROW3_PIN);
sbi(R4_OUT_REG, ROW4_PIN);
cbi(C1_OUT_REG, COL1_PIN);
cbi(C2_OUT_REG, COL2_PIN);
cbi(C3_OUT_REG, COL3_PIN);
cbi(C4_OUT_REG, COL4_PIN); 

return;
}
/*######################################################################################################*/

static void column_pins_as_inputs(void)
{

cbi(C1_DDR_REG, COL1_PIN);
cbi(C2_DDR_REG, COL2_PIN);
cbi(C3_DDR_REG, COL3_PIN);
cbi(C4_DDR_REG, COL4_PIN);
sbi(R1_DDR_REG, ROW1_PIN);
sbi(R2_DDR_REG, ROW2_PIN);
sbi(R3_DDR_REG, ROW3_PIN);
sbi(R4_DDR_REG, ROW4_PIN);

/* Turn on the pull up resistors */
sbi(C1_OUT_REG, COL1_PIN);
sbi(C2_OUT_REG, COL2_PIN);
sbi(C3_OUT_REG, COL3_PIN);
sbi(C4_OUT_REG, COL4_PIN); 
cbi(R1_OUT_REG, ROW1_PIN);
cbi(R2_OUT_REG, ROW2_PIN);
cbi(R3_OUT_REG, ROW3_PIN);
cbi(R4_OUT_REG, ROW4_PIN);


return;
}

#endif /* #if KEYB_WILL_USE_ONE_PORT == 0 */

#if   HEX_KEYB_WILL_USE_I2C == 1 && NUMBER_OF_HEX_KEYBOARDS >= 2
static void hkb_configure_io(void)
{

        hkb_i2c_address_array[0]=HEX_KEYB_0_I2C_ADDRESS;
        hkb_i2c_address_array[1]=HEX_KEYB_1_I2C_ADDRESS;

#if NUMBER_OF_HEX_KEYBOARDS >=  3
        hkb_i2c_address_array[2]=HEX_KEYB_2_I2C_ADDRESS;
#endif
#if NUMBER_OF_HEX_KEYBOARDS >=  4
        hkb_i2c_address_array[3]=HEX_KEYB_3_I2C_ADDRESS;
#endif
#if NUMBER_OF_HEX_KEYBOARDS >=  5
        hkb_i2c_address_array[4]=HEX_KEYB_4_I2C_ADDRESS;
#endif
#if NUMBER_OF_HEX_KEYBOARDS >=  6
        hkb_i2c_address_array[5]=HEX_KEYB_5_I2C_ADDRESS;
#endif
#if NUMBER_OF_HEX_KEYBOARDS >=  7
        hkb_i2c_address_array[6]=HEX_KEYB_6_I2C_ADDRESS;
#endif
#if NUMBER_OF_HEX_KEYBOARDS >=  8
        hkb_i2c_address_array[7]=HEX_KEYB_7_I2C_ADDRESS;
#endif



return;
}
#endif   /* HEX_KEYB_WILL_USE_I2C == 1 && NUMBER_OF_HEX_KEYBOARDS >= 2 */



/********************************************************************************************************/
/*                                   PUBLIC FUNCTIONS                                                   */
/********************************************************************************************************/

#if HEX_KEYB_WILL_USE_INTERRUPT == 1

void hkb_init(void (*execute_this_function)(unsigned char key_code), unsigned char on_off)
{
#if HEX_KEYB_WILL_USE_ONE_PORT == 0 && HEX_KEYB_WILL_USE_I2C == 0
/* Make the COLUMNS outputs and ROWS inputs */
sbi(C1_DDR_REG, COL1_PIN);
sbi(C2_DDR_REG, COL2_PIN);
sbi(C3_DDR_REG, COL3_PIN);
sbi(C4_DDR_REG, COL4_PIN);
cbi(R1_DDR_REG, ROW1_PIN);
cbi(R2_DDR_REG, ROW2_PIN);
cbi(R3_DDR_REG, ROW3_PIN);
cbi(R4_DDR_REG, ROW4_PIN);

/* Turn on the pull up resistors */
sbi(R1_OUT_REG, ROW1_PIN);
sbi(R2_OUT_REG, ROW2_PIN);
sbi(R3_OUT_REG, ROW3_PIN);
sbi(R4_OUT_REG, ROW4_PIN);
cbi(C1_OUT_REG, COL1_PIN);
cbi(C2_OUT_REG, COL2_PIN);
cbi(C3_OUT_REG, COL3_PIN);
cbi(C4_OUT_REG, COL4_PIN); 

#elif HEX_KEYB_WILL_USE_ONE_PORT == 1 && HEX_KEYB_WILL_USE_I2C == 0

/* Make the COLUMN pins outputs and ROW pins inputs */
outp(((1<<COL4_PIN)|(1<<COL3_PIN)|(1<<COL2_PIN)|(1<<COL1_PIN)), HEX_KEYB_PORT_DDR_REG);
/* Turn on the pull up resistors */
outp(((1<<ROW4_PIN)|(1<<ROW3_PIN)|(1<<ROW2_PIN)|(1<<ROW1_PIN)), HEX_KEYB_PORT_OUT_REG); 
wait(TIME_30_MS);

#elif   HEX_KEYB_WILL_USE_I2C == 1 && NUMBER_OF_HEX_KEYBOARDS >= 2

for(temp1=0; temp1<NUMBER_OF_HEX_KEYBOARDS; temp1++)
  {
     hkb_select(temp1);
     hkb_scan(); 
  }
hkb_select(HKB_0);

#elif   HEX_KEYB_WILL_USE_I2C == 1 && NUMBER_OF_HEX_KEYBOARDS == 1

hkb_scan();

#endif

/* Enable global interrupts and activate the selected external interrupt */ 
if(on_off)
 { 
    sbi(HEX_KEYB_INT_REG, HEX_KEYB_INT_BIT);
    asm("sei");
 }
else{
       cbi(HEX_KEYB_INT_REG, HEX_KEYB_INT_BIT);
    } 

ptr=execute_this_function;


return;
}


#endif /* #if HEX_KEYB_WILL_USE_INTERRUPT == 1 */


/********************************************************************************************************/
#if HEX_KEYB_WILL_USE_ONE_PORT == 1 && HEX_KEYB_WILL_USE_I2C == 0
unsigned char hkb_scan(void)
{ 
unsigned char     rows=0, columns=0, key=0, keyb_input=0;


rows=((1<<ROW4_PIN)|(1<<ROW3_PIN)|(1<<ROW2_PIN)|(1<<ROW1_PIN));
columns=((1<<COL4_PIN)|(1<<COL3_PIN)|(1<<COL2_PIN)|(1<<COL1_PIN));

      /* Check rows only to see if a key is down */ 
      outp(columns,HEX_KEYB_PORT_DDR_REG);                     
      outp(rows,HEX_KEYB_PORT_OUT_REG);
      /* Give time for the pins to settle, due to keyboard and wiring capacitance */
      wait(TIME_100_US);    
      keyb_input = inp(HEX_KEYB_PORT_PIN_REG);     
      /* If there is indeed a key down perform a full keyboard scan */
      if(keyb_input != rows) 
       {                                                
           /* Debounce */
           wait(TIME_30_MS);                             
           /* Read rows */
           outp(columns,HEX_KEYB_PORT_DDR_REG);                     
           outp(rows,HEX_KEYB_PORT_OUT_REG);
           /* Give time for the pins to settle, due to keyboard and wiring capacitance */    
           wait(TIME_100_US);   
           keyb_input = inp(HEX_KEYB_PORT_PIN_REG);
           /* Read columns */
           outp(rows,HEX_KEYB_PORT_DDR_REG);                     /* CHECK COLUMNS  */ 
           outp(columns,HEX_KEYB_PORT_OUT_REG);
           /* Give time for the pins to settle, due to keyboard and wiring capacitance */
           wait(TIME_100_US);   
           keyb_input |= inp(HEX_KEYB_PORT_PIN_REG);
           keyb_input = (~keyb_input);           
           /* Scan code to Ascii conversion look up table and rollover filter */
           switch(keyb_input)                           
                {
                   case((1<<COL1_PIN)|(1<<ROW1_PIN)): key=KEY1;  break;
                   case((1<<COL2_PIN)|(1<<ROW1_PIN)): key=KEY2;  break;
                   case((1<<COL3_PIN)|(1<<ROW1_PIN)): key=KEY3;  break;
                   case((1<<COL4_PIN)|(1<<ROW1_PIN)): key=KEY4;  break;
                   case((1<<COL1_PIN)|(1<<ROW2_PIN)): key=KEY5;  break;
                   case((1<<COL2_PIN)|(1<<ROW2_PIN)): key=KEY6;  break;
                   case((1<<COL3_PIN)|(1<<ROW2_PIN)): key=KEY7;  break;
                   case((1<<COL4_PIN)|(1<<ROW2_PIN)): key=KEY8;  break;
                   case((1<<COL1_PIN)|(1<<ROW3_PIN)): key=KEY9;  break;
                   case((1<<COL2_PIN)|(1<<ROW3_PIN)): key=KEY10; break;
                   case((1<<COL3_PIN)|(1<<ROW3_PIN)): key=KEY11; break;
                   case((1<<COL4_PIN)|(1<<ROW3_PIN)): key=KEY12; break;
                   case((1<<COL1_PIN)|(1<<ROW4_PIN)): key=KEY13; break;
                   case((1<<COL2_PIN)|(1<<ROW4_PIN)): key=KEY14; break;
                   case((1<<COL3_PIN)|(1<<ROW4_PIN)): key=KEY15; break;
                   case((1<<COL4_PIN)|(1<<ROW4_PIN)): key=KEY16; break;
                   default: key=0;
                }

#if HEX_KEYB_WILL_USE_INTERRUPT == 1
/*
           set ROW port pins as inputs with pullups on and COLUMNS port pins as outputs
           in case you are using an interrupt for sensing if any key is pressed.
*/
           /* Make the COLUMN pins outputs and ROW pins inputs */
           outp(columns, HEX_KEYB_PORT_DDR_REG);
           /* Turn on the pull up resistors */
           outp(rows, HEX_KEYB_PORT_OUT_REG); 
           /* Give time for the pins to settle, due to keyboard and wiring capacitance */ 
           wait(TIME_1_MS);
#endif

       }  /* if(keyb_input) */   


return(key); 
}

#elif HEX_KEYB_WILL_USE_ONE_PORT == 0 && HEX_KEYB_WILL_USE_I2C == 0


unsigned char hkb_scan(void)
{ 
unsigned char keyb_input=0, key=0;


row_pins_as_inputs();
wait(TIME_1_MS); 
if(bit_is_clear(R1_PIN_REG,ROW1_PIN)) { keyb_input+=(1<<0); }
if(bit_is_clear(R2_PIN_REG,ROW2_PIN)) { keyb_input+=(1<<1); }
if(bit_is_clear(R3_PIN_REG,ROW3_PIN)) { keyb_input+=(1<<2); }
if(bit_is_clear(R4_PIN_REG,ROW4_PIN)) { keyb_input+=(1<<3); }

if(keyb_input)
 {
         keyb_input=0;
         /* key debounce delay */
         wait(TIME_30_MS);                             
         /* Give time for the pins to settle, due to keyboard and wiring capacitance */    
         if(bit_is_clear(R1_PIN_REG,ROW1_PIN)) { keyb_input+=(1<<0); }
         if(bit_is_clear(R2_PIN_REG,ROW2_PIN)) { keyb_input+=(1<<1); }
         if(bit_is_clear(R3_PIN_REG,ROW3_PIN)) { keyb_input+=(1<<2); }
         if(bit_is_clear(R4_PIN_REG,ROW4_PIN)) { keyb_input+=(1<<3); }

         column_pins_as_inputs(); 
         /* Give time for the pins to settle, due to keyboard and wiring capacitance */
         wait(TIME_1_MS);   
         if(bit_is_clear(C1_PIN_REG,COL1_PIN)) { keyb_input+=(1<<4); }
         if(bit_is_clear(C2_PIN_REG,COL2_PIN)) { keyb_input+=(1<<5); }
         if(bit_is_clear(C3_PIN_REG,COL3_PIN)) { keyb_input+=(1<<6); }
         if(bit_is_clear(C4_PIN_REG,COL4_PIN)) { keyb_input+=(1<<7); }

         /* Scan code to Ascii conversion look up table and rollover filter */
         switch((unsigned char)keyb_input)  
              {
                case((1<<4)|(1<<0)): key=KEY1;  break;
                case((1<<5)|(1<<0)): key=KEY2;  break;
                case((1<<6)|(1<<0)): key=KEY3;  break;
                case((1<<7)|(1<<0)): key=KEY4;  break;
                case((1<<4)|(1<<1)): key=KEY5;  break;
                case((1<<5)|(1<<1)): key=KEY6;  break;
                case((1<<6)|(1<<1)): key=KEY7;  break;
                case((1<<7)|(1<<1)): key=KEY8;  break;
                case((1<<4)|(1<<2)): key=KEY9;  break;
                case((1<<5)|(1<<2)): key=KEY10; break;
                case((1<<6)|(1<<2)): key=KEY11; break;
                case((1<<7)|(1<<2)): key=KEY12; break;
                case((1<<4)|(1<<3)): key=KEY13; break;
                case((1<<5)|(1<<3)): key=KEY14; break;
                case((1<<6)|(1<<3)): key=KEY15; break;
                case((1<<7)|(1<<3)): key=KEY16; break;
                default: key=0;
              }

#if HEX_KEYB_WILL_USE_INTERRUPT == 1
         /* Make the COLUMN pins outputs and ROW pins inputs */
         row_pins_as_inputs();
         wait(TIME_1_MS);
#endif


 } /* if(keyb_input) */


return(key);
}

#endif /* #if HEX_KEYB_WILL_USE_ONE_PORT == 1 -> #elif HEX_KEYB_WILL_USE_ONE_PORT == 0 */
/********************************************************************************************************/
#if HEX_KEYB_WILL_USE_I2C == 1

#if NUMBER_OF_HEX_KEYBOARDS ==  1  
unsigned char hkb_scan(void)
{ 
unsigned char     rows=0, columns=0, key=0, keyb_input=0;


rows=((1<<ROW4_PIN)|(1<<ROW3_PIN)|(1<<ROW2_PIN)|(1<<ROW1_PIN));
columns=((1<<COL4_PIN)|(1<<COL3_PIN)|(1<<COL2_PIN)|(1<<COL1_PIN));

      /* Check rows only to see if a key is down */ 
      I2C_START_TX(HEX_KEYB_0_I2C_ADDRESS);
      i2c_put_byte(rows);
      I2C_START_RX(HEX_KEYB_0_I2C_ADDRESS);
      keyb_input = i2c_get_byte(I2C_QUIT);     
      /* If there is indeed a key down perform a full keyboard scan */
      if(keyb_input!=rows) 
       {                                                
           /* Debounce */
           wait(TIME_30_MS);                             
           /* Read rows */
           I2C_START_TX(HEX_KEYB_0_I2C_ADDRESS);
           i2c_put_byte(rows);
           I2C_START_RX(HEX_KEYB_0_I2C_ADDRESS);
           keyb_input = i2c_get_byte(I2C_QUIT);
           /* Read columns */
           I2C_START_TX(HEX_KEYB_0_I2C_ADDRESS);
           i2c_put_byte(columns);
           I2C_START_RX(HEX_KEYB_0_I2C_ADDRESS);
           keyb_input |= i2c_get_byte(I2C_QUIT);
           keyb_input = (~keyb_input);
           /* Scan code to Ascii conversion look up table and rollover filter */
           switch(keyb_input)                           
                {
                   case((1<<COL1_PIN)|(1<<ROW1_PIN)): key=KEY1;  break;
                   case((1<<COL2_PIN)|(1<<ROW1_PIN)): key=KEY2;  break;
                   case((1<<COL3_PIN)|(1<<ROW1_PIN)): key=KEY3;  break;
                   case((1<<COL4_PIN)|(1<<ROW1_PIN)): key=KEY4;  break;
                   case((1<<COL1_PIN)|(1<<ROW2_PIN)): key=KEY5;  break;
                   case((1<<COL2_PIN)|(1<<ROW2_PIN)): key=KEY6;  break;
                   case((1<<COL3_PIN)|(1<<ROW2_PIN)): key=KEY7;  break;
                   case((1<<COL4_PIN)|(1<<ROW2_PIN)): key=KEY8;  break;
                   case((1<<COL1_PIN)|(1<<ROW3_PIN)): key=KEY9;  break;
                   case((1<<COL2_PIN)|(1<<ROW3_PIN)): key=KEY10; break;
                   case((1<<COL3_PIN)|(1<<ROW3_PIN)): key=KEY11; break;
                   case((1<<COL4_PIN)|(1<<ROW3_PIN)): key=KEY12; break;
                   case((1<<COL1_PIN)|(1<<ROW4_PIN)): key=KEY13; break;
                   case((1<<COL2_PIN)|(1<<ROW4_PIN)): key=KEY14; break;
                   case((1<<COL3_PIN)|(1<<ROW4_PIN)): key=KEY15; break;
                   case((1<<COL4_PIN)|(1<<ROW4_PIN)): key=KEY16; break;
                   default: key=0;
                }

       }

i2c_stop();      

return(key);
}

#elif NUMBER_OF_HEX_KEYBOARDS >=  2

unsigned char hkb_select(unsigned char hkb_unit_number)
{

if(hkb_init_lock == 0) { hkb_configure_io(); hkb_init_lock=1; }

if(hkb_unit_number<NUMBER_OF_HEX_KEYBOARDS)
 { 
     hkb_i2c_address=hkb_i2c_address_array[hkb_unit_number];
     hkb_id=hkb_unit_number;
 }


return(hkb_id);
}



unsigned char hkb_scan(void)
{ 
unsigned char     rows=0, columns=0, key=0, keyb_input=0;


rows=((1<<ROW4_PIN)|(1<<ROW3_PIN)|(1<<ROW2_PIN)|(1<<ROW1_PIN));
columns=((1<<COL4_PIN)|(1<<COL3_PIN)|(1<<COL2_PIN)|(1<<COL1_PIN));

      /* Check rows only to see if a key is down */ 
      I2C_START_TX(hkb_i2c_address);
      i2c_put_byte(rows);
      I2C_START_RX(hkb_i2c_address);
      keyb_input = i2c_get_byte(I2C_QUIT);     
      /* If there is indeed a key down perform a full keyboard scan */
      if(keyb_input!=rows) 
       {                                                
           /* Debounce */
           wait(TIME_30_MS);                             
           /* Read rows */
           I2C_START_TX(hkb_i2c_address);
           i2c_put_byte(rows);
           I2C_START_RX(hkb_i2c_address);
           keyb_input = i2c_get_byte(I2C_QUIT);
           /* Read columns */
           I2C_START_TX(hkb_i2c_address);
           i2c_put_byte(columns);
           I2C_START_RX(hkb_i2c_address);
           keyb_input |= i2c_get_byte(I2C_QUIT);
           keyb_input = (~keyb_input);
           /* Scan code to Ascii conversion look up table and rollover filter */
           switch(keyb_input)                           
                {
                   case((1<<COL1_PIN)|(1<<ROW1_PIN)): key=KEY1;  break;
                   case((1<<COL2_PIN)|(1<<ROW1_PIN)): key=KEY2;  break;
                   case((1<<COL3_PIN)|(1<<ROW1_PIN)): key=KEY3;  break;
                   case((1<<COL4_PIN)|(1<<ROW1_PIN)): key=KEY4;  break;
                   case((1<<COL1_PIN)|(1<<ROW2_PIN)): key=KEY5;  break;
                   case((1<<COL2_PIN)|(1<<ROW2_PIN)): key=KEY6;  break;
                   case((1<<COL3_PIN)|(1<<ROW2_PIN)): key=KEY7;  break;
                   case((1<<COL4_PIN)|(1<<ROW2_PIN)): key=KEY8;  break;
                   case((1<<COL1_PIN)|(1<<ROW3_PIN)): key=KEY9;  break;
                   case((1<<COL2_PIN)|(1<<ROW3_PIN)): key=KEY10; break;
                   case((1<<COL3_PIN)|(1<<ROW3_PIN)): key=KEY11; break;
                   case((1<<COL4_PIN)|(1<<ROW3_PIN)): key=KEY12; break;
                   case((1<<COL1_PIN)|(1<<ROW4_PIN)): key=KEY13; break;
                   case((1<<COL2_PIN)|(1<<ROW4_PIN)): key=KEY14; break;
                   case((1<<COL3_PIN)|(1<<ROW4_PIN)): key=KEY15; break;
                   case((1<<COL4_PIN)|(1<<ROW4_PIN)): key=KEY16; break;
                   default: key=0;
                }

       }

i2c_stop();      

return(key);
}


#endif   /* #if NUMBER_OF_HEX_KEYBOARDS ==  1 -> #elif NUMBER_OF_HEX_KEYBOARDS >=  2 */  

#endif   /* #if HEX_KEYB_WILL_USE_I2C == 1 */


#if HKEYB_SCAN_FUNCTION_ONLY == 0 

#if HKEYB_GETS_NEEDED == 1
void hkb_gets(unsigned char *data, unsigned char max_chars, unsigned char x, unsigned char y)
{
 
unsigned char buffer=0, aux_buffer1=0, aux_buffer2=0;                          /* char buffers          */
unsigned char index=0, index_counter=0,  max=3, capital=0;                     /* char array controls   */
unsigned char backspace_hit_counter=0, keystroke_counter=0;                    /* char editor controls  */
                                          
#if ACTIVATE_AUTO_ADVANCE == 1 
unsigned char auto_advance=0;                                                 /* auto advance semaphore */
unsigned int  auto_advance_counter=0;                                         /* cursor advance timer   */
#endif

/* char array containing the keyboard characters */
static const char  key1[7]__attribute__((progmem))=  {' ','?','!',',','.',':','1' };
static const char  key2[4]__attribute__((progmem))=  {'a','b','c','2' };
static const char  key3[4]__attribute__((progmem))=  {'d','e','f','3' };
static const char  key4[4]__attribute__((progmem))=  {'g','h','i','4' };
static const char  key5[4]__attribute__((progmem))=  {'j','k','l','5' };
static const char  key6[4]__attribute__((progmem))=  {'m','n','o','6' };
static const char  key7[5]__attribute__((progmem))=  {'p','q','r','s','7' };
static const char  key8[4]__attribute__((progmem))=  {'t','u','v','8' };
static const char  key9[5]__attribute__((progmem))=  {'w','x','y','z','9' };
static const char key10[7]__attribute__((progmem))=  {'0','+','-','*','/','%','#' };

/* Because the lcd display advances automatically every time a char is displayed
   a lcd_gotoxy() command must be issued after any lcd_put command 
   in order to reposition the cursor to the desired position!!!                  */

/* Initialization */ 
for(index=0;index<=max_chars;index++) { *(data+index)='\0'; } /* Fill buffer with nulls */
if(max_chars) max_chars=max_chars-1; else return;             /* Adjust the char limit  */ 
index=0;
LCD_CURSOR_ON_BLINK(); 
     
/*------------------------------------------------------------------------------------------------------*/
while(1)  
    {  /*main loop start */
/* position cursor, wait for all keys to be released and wait 0,1 sec to prevent key double click */
      LCD_GOTOXY(x, y);  while((buffer=hkb_scan())); wait(TIME_99_MS);
                                                   
#if ACTIVATE_AUTO_ADVANCE == 1 
      while( !(buffer=hkb_scan()) )                         /* Cursor auto advance routine */
          {
            
            auto_advance_counter++;
            if( auto_advance && (auto_advance_counter>=CURSOR_ADVANCE_VALUE) && index<max_chars )
             {
               if((x==LCD_CHARS_PER_LINE-1) && (y<(LCD_LINES-1)) ) { x=0; y++; }
               else if(x<LCD_CHARS_PER_LINE-1) { x++; }  LCD_GOTOXY(x,y); 
               index++; 
               auto_advance=0;  keystroke_counter=0; backspace_hit_counter=1;
               aux_buffer1='\0'; index_counter=0;
             }
            wait(TIME_99_MS); 
          }
#else
      while( !(buffer=hkb_scan()) );                              /* Just wait untill a key is pressed */
#endif                
/*------------------------------------------------------------------------------------------------------*/
      if( buffer==ENTER )                              /* ENTER */
       { 
         for(index=(max_chars+1);index;index--)        /* Trim trailing spaces  */
           { 
             if( *(data+index)=='\0' || *(data+index)==' ' ) { *(data+index)='\0'; } else { break; }
           }
         break;
         
       }     /* Terminate string and exit */
       
/*------------------------------------------------------------------------------------------------------*/
      /* ESCAPE  Turn off cursor and set string to zero */
      else if( buffer==ESCAPE ) { *data='\0'; break; }
             
/*------------------------------------------------------------------------------------------------------*/
      else if( buffer==CAPITAL ) { capital=~capital;  }  /* a->A */           

/*------------------------------------------------------------------------------------------------------*/
      else if(buffer==BACKSPACE)                         /* B  BACKSPACE                              */
            {                                     
              *(data+index)=' ';                         /* write a NULL to string and a space to lcd */
              LCD_PUTC(' '); 
              backspace_hit_counter++;                   /* BACKSPACE consecutive hit counter         */
              if(backspace_hit_counter==2 && index)      /* backwards movement control                */
               {
                 index--; 
                 if((x==0) && (y!=0)) { x=(LCD_CHARS_PER_LINE-1) ; y--; } else if(x>0) x--;
                 if(*(data+index)==' ') backspace_hit_counter=1; else backspace_hit_counter=0; 
               }
              aux_buffer1='\0'; index_counter=0;  keystroke_counter=0;
#if ACTIVATE_AUTO_ADVANCE == 1
              auto_advance=0; auto_advance_counter=0;
#endif                           
              
            } 
/*-----------------------------------------------------------------------------------------------------*/
#if ACTIVATE_ARROWS == 1
      else if(buffer==MOVE_LEFT)                         /* LEFT ARROW <--  */
            {
              if(index)  
               {
                 index--;
                 if((x==0) && (y!=0)) { x=(LCD_CHARS_PER_LINE-1) ; y--; } else if(x>0) x--;
                 aux_buffer1='\0'; index_counter=0; keystroke_counter=0;
#if ACTIVATE_AUTO_ADVANCE == 1
                 auto_advance=0; auto_advance_counter=0;
#endif                 
               }  
            }  
   
/*-----------------------------------------------------------------------------------------------------*/
      else if(buffer==MOVE_RIGHT)                       /* RIGHT ARROW -->  */
            {
              if( index<max_chars ) 
               {
                 if(*(data+index)=='\0') { *(data+index)=' '; }
                 index++; 
                 if((x==LCD_CHARS_PER_LINE-1) && (y<(LCD_LINES-1)) ) { x=0; y++; }
                 else if(x<LCD_CHARS_PER_LINE-1) { x++; }  LCD_GOTOXY(x,y); 
                 if(*(data+index)=='\0') { *(data+index)=' '; LCD_PUTC(' '); }
                 aux_buffer1='\0'; index_counter=0; 
                 keystroke_counter=0;
#if ACTIVATE_AUTO_ADVANCE == 1
                 auto_advance=0; auto_advance_counter=0;
#endif                                  
               }     
            }  

#endif              
/*------------------------------------------------------------------------------------------------------*/
      else if(index<=max_chars)  /* start of write char to array if block */  
            { /* If the same key is pressed more than once */
              if(buffer==aux_buffer1) { index_counter++; if(index_counter>=max) index_counter=0;  } 
              else index_counter=0; /* else if not the same key print the first array char */ 
              switch(buffer)   /* copy the appropriate char from the array to aux_buffer2  */
                   {
                     case(HKB_CHAR_KEY_1):  max=sizeof(key1);
                                            aux_buffer2=PRG_RDB(&key1[index_counter]);
                                            break; 
                     case(HKB_CHAR_KEY_2) : max=sizeof(key2);
                                            aux_buffer2=PRG_RDB(&key2[index_counter]);
                                            break; 
                     case(HKB_CHAR_KEY_3) : max=sizeof(key3);
                                            aux_buffer2=PRG_RDB(&key3[index_counter]);
                                            break;
                     case(HKB_CHAR_KEY_4) : max=sizeof(key4);
                                            aux_buffer2=PRG_RDB(&key4[index_counter]);
                                            break;
                     case(HKB_CHAR_KEY_5) : max=sizeof(key5);
                                            aux_buffer2=PRG_RDB(&key5[index_counter]);
                                            break;
                     case(HKB_CHAR_KEY_6) : max=sizeof(key6);
                                            aux_buffer2=PRG_RDB(&key6[index_counter]);
                                            break;
                     case(HKB_CHAR_KEY_7) : max=sizeof(key7);
                                            aux_buffer2=PRG_RDB(&key7[index_counter]);
                                            break;
                     case(HKB_CHAR_KEY_8) : max=sizeof(key8);
                                            aux_buffer2=PRG_RDB(&key8[index_counter]);
                                            break;
                     case(HKB_CHAR_KEY_9) : max=sizeof(key9);
                                            aux_buffer2=PRG_RDB(&key9[index_counter]);
                                            break;
                     case(HKB_CHAR_KEY_10): max=sizeof(key10);
                                            aux_buffer2=PRG_RDB(&key10[index_counter]);
                                            break;
                     default              : continue; /* if other key start the loop again */
                   }   
               /* if it is not the same key or it is the first keystroke */
               if(buffer!=aux_buffer1) 
                {
                  aux_buffer1=buffer; backspace_hit_counter=0; keystroke_counter++;
                  /* If it is not the first keystroke advance the cursor */
                  if( keystroke_counter>=2 && index<max_chars )
                   {
                     if((x==LCD_CHARS_PER_LINE-1) && (y<(LCD_LINES-1)) ) { x=0; y++; }
                     else if(x<LCD_CHARS_PER_LINE-1) { x++; }  LCD_GOTOXY(x,y); 
                     index++;   backspace_hit_counter=0;
                   }
                }
               /* If capital letters wanted convert small to capital */
               if( capital && (aux_buffer2>='a' && aux_buffer2<='z') ) aux_buffer2-=32;
               
               /* Write char to destination array and LCD screen */
               *(data+index)=aux_buffer2;             
               LCD_PUTC(*(data+index));
#if ACTIVATE_AUTO_ADVANCE == 1                
               auto_advance=1; auto_advance_counter=0;
#endif               
            }           /*  end of write char to array if block */ 
             
    }                   /*main loop end                         */            
LCD_CURSOR_OFF();       /* Turn off cursor and exit             */     


return;
}
#endif   /* #if HKEYB_GETS_NEEDED == 1 */
/*******************************************************************************************************/
#if HKEYB_GETI_NEEDED == 1

unsigned char hkb_geti(int *data, unsigned char digits, unsigned char lcd_x, unsigned char lcd_y)
{
 /* 1 for the munus sign, 5 for the number and one for the terminating char = 7 */ 
 unsigned char number[7]={'\0','\0','\0','\0','\0','\0','\0'}; 
 unsigned char index=0, buffer=0,counter1=0, password_mode=0;


  
  if(digits==0) { password_mode=1; digits=4; }
  LCD_CURSOR_ON_BLINK(); 
  while(1)
      { /* main loop start */     
/* position cursor, wait for all keys to be released and wait 0,1 sec to prevent key double click */
        LCD_GOTOXY(lcd_x, lcd_y);
        while((buffer=hkb_scan()));
        wait(TIME_100_MS);
        while( !(buffer=hkb_scan()) );
/*-----------------------------------------------------------------------------------------------------*/        
        if( buffer==ENTER )     { LCD_CURSOR_OFF(); break; }
/*-----------------------------------------------------------------------------------------------------*/   
        else if(buffer==ESCAPE) { LCD_CURSOR_OFF(); return(0); }
/*-----------------------------------------------------------------------------------------------------*/          
        else if( buffer==BACKSPACE )
              { 
                LCD_CURSOR_ON_BLINK();
                if(lcd_x) { lcd_x--;  } 
                if(index) index--; 
                if(counter1) { counter1--; }
                number[index]='\0'; 
                LCD_GOTOXY(lcd_x, lcd_y); 
                LCD_PUTC(' ');   
              }
/*-----------------------------------------------------------------------------------------------------*/          
        else if(buffer==MINUS && index==0) 
              {
                number[index]='-';
                index++;
                LCD_PUTC('-'); lcd_x++;   
              }
/*-----------------------------------------------------------------------------------------------------*/                   
        else if( buffer>='0' && buffer<='9' )
              { 
                if(counter1<digits)
                 {
                   number[index]=(buffer);  index++; counter1++; 
                   if(password_mode) LCD_PUTC('*'); else LCD_PUTC(buffer);  
                   lcd_x++; 
                   if(counter1==digits) LCD_CURSOR_OFF();
                 }
              }
       
      } /* main loop end */   
/*-----------------------------------------------------------------------------------------------------*/   
*data=atoi(number);


return(counter1);
}
#endif   /* #if HKEYB_GETI_NEEDED == 1 */
/*******************************************************************************************************/
#if HKEYB_GETC_NEEDED == 1
unsigned char hkb_getc(void)
{
register unsigned char buffer=0;

/*  wait for all keys to be released, wait for a key pressed and return the scan code */  
while(hkb_scan());  while( !(buffer=hkb_scan()) ); 


  
return(buffer);
}
#endif   /* #if HKEYB_GETC_NEEDED == 1 */

#endif /* #if HKEYB_SCAN_FUNCTION_ONLY == 0 */

/********************************************************************************************************/
/*                                   INTERRUPT SERVICE ROUTINES                                         */
/********************************************************************************************************/

#if HEX_KEYB_WILL_USE_INTERRUPT == 1

/* KEYB ISR HANDLER. ALL OTHER INTERRUPTS ARE ENABLED TO INTERRUPT THIS ISR! */
SIGNAL(HEX_KEYB_INT_NAME)
{
/* Disable further keyb interrupt triggering */
cbi(HEX_KEYB_INT_REG, HEX_KEYB_INT_BIT);
asm("sei"); 


#if NUMBER_OF_HEX_KEYBOARDS >= 2 && HEX_KEYB_WILL_USE_I2C == 1 
/* 
    Find out which key is down which keyboard generated the interrupt by polling every keyboard
    untill the one that has a key pressed is found. If all keyboards are found not to have
    a key pressed reenable the external interrupt and return from this ISR.
*/

for(temp1=0; temp1<=NUMBER_OF_HEX_KEYBOARDS; temp1++)
  {
     if(temp1 >= NUMBER_OF_HEX_KEYBOARDS) { sbi(HEX_KEYB_INT_REG, HEX_KEYB_INT_BIT); return; }
     hkb_select(temp1);
     key_pressed=hkb_scan(); 
     if(key_pressed) { break; }
  }

/* 
   If key pressed is not 0 (possibly noise) execute the below function using a pointer
   and wait until all keys are released. Do that even in case that the keyboard id
   has changed by the user. 
*/
temp1=hkb_get_id();
(*ptr)(key_pressed);  
temp2=hkb_get_id();
if(temp1==temp2)
 {
    while(hkb_scan());
 }
else{
       hkb_select(temp1);
       while(hkb_scan());
       hkb_select(temp2);
    }     

#elif HEX_KEYB_WILL_USE_I2C == 0 || NUMBER_OF_HEX_KEYBOARDS == 1    

/* 
   If key pressed is not 0 (possibly noise) execute the below function using a pointer
   and wait until all keys are released 
*/
key_pressed=hkb_scan();
if(key_pressed)
 { 
    (*ptr)(key_pressed);
    while(hkb_scan());
 }

#endif

/* Reenable keyb interrupt */
sbi(HEX_KEYB_INT_REG, HEX_KEYB_INT_BIT);


return;
}
#endif

/*######################################################################################################*/
/*                                         T H E   E N D                                                */
/*######################################################################################################*/


