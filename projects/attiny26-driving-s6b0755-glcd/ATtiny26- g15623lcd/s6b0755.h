/*  Written by LeRoy Olson  Jan 18 2011
 * Header Name: s6b0755.h
 * Description: Values for operating S6B0755 GLCD Driver in G15623 GLCD
 * Arguments: None
 * Returns:  None
 * NOTES: Header with all the glcd comand data
 *     S6B0755 defines
 */

#ifndef S6B0755_H_
#define S6B0755_H_

#define SOFT_RESET              0b11100010   //Initialize internal functions
#define SET_PAGE_ADDR           0b10110000   //0000xxxx page address

#define SET_COL_ADDR_HI         0b00010000   //00010xxx
#define SET_COL_ADDR_LO         0b00000000   //0000xxxx

#define SHL_SELECT              0b11000000   //COM bidrectionaol 1100 000X 0 normal, 1 rev scan direction.
#define ADC_SELECT              0b10100000   //Bi directional 0000 000X seg 0 norm, 1 rev.

#define SET_DUTY_1              0b01001000
#define SET_DUTY_2              0b01000000

#define SET_BIAS                0b01010001   //01010xxx = bias, Should be 001 for 1/5 bias
#define SET_OSC_ON              0b10101011   // start internal oscilator
#define DC_STEP_UP              0b01100100   //011001xx select 3-5 boosting
#define REG_RESISTOR            0b00100000   //00100000 (center)
#define REG_RESISTOR_VAL        0b00000001   // 0010 0xxx (center) range 0x00 to 0x03 B0000 0001 is best

#define SET_EVR_1               0b10000001   // s byte instruction
#define SET_EVR_2               0b00011100   //00xxxxxx=valus for center

#define POWER_CONTROL           0b00101111   // All circuits on
#define SET_POWER_SAVE          0b10101000   //1010100x power save level 0=standby, 1=sleep
#define CLEAR_POWER_SAVE        0b11100001

#define DISPLAY_ON              0b10101111    //1010 111x 1=display on 0=display off
#define DISPLAY_OFF             0b10101110

#define DISPLAY_INVERT          0b10100110   //1010011x, 0= normal,1= invert video
#define DISPLAY_INVERT_VAL      0b00000001   //range 0000 0000 to 0000 0001

#define ALL_BLACK               0b10100101   //1010010x 1 forces entire display on 0=normal
#define VOLUME_CONTROL_VAL      0b00011100   //range 00XX XXXX to 0011 1111

#define dt1                   Delay_us(60)// works with 60 us delay
#define dt2                   Delay_us(1) //works with 1 ss delay
#define dt3                   Delay_ms(600) //Set delay for switches on stk500 board

#define LCD_DDR_SET           DDRA=0B00101111 //Pins 0 to 3 and 5 as output
#define LCD_PORT_SET          PORTA=0B11010000//Set pins 0 to 3 and 5 low

#define LCD_RS                PORTA1_bit   //PORTA1_bit
#define LCD_SCK               PORTA0_bit   //PORTA0_bit
#define LCD_RESETB            PORTA3_bit   //PORTA3_bit
#define LCD_SID               PORTA2_bit   //PORTA2_bit
#define LCD_CS1B              PORTA5_bit   //PORTA5_bit

#endif /* S6B0755_H_ */