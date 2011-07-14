/***C*********************************************************************************************
**
** SRC-FILE     :   dle_scan_lcd.c
**                                        
** PROJECT      :   DLE SCAN
**                                                                
** SRC-VERSION  :   0
**              
** DATE         :   2011-05-09
**
** AUTHOR       :   AV
**
** DESCRIPTION  :   LCD related subroutines 
**                  
** COPYRIGHT    :   Andrejs Vasiljevs (c) 2011
**
****C*E******************************************************************************************/
#include <avr/io.h>
#include <avr/pgmspace.h>
#include "dle_typedef.h"
#include "dle_scan_lcd.h"

uint8 guc_lcd_redraw;
uint8 guca_lcd_digits_set0[LCD_SET_DIGITS_NUM];
uint8 guca_lcd_digits_set1[LCD_SET_DIGITS_NUM];
uint8 guca_lcd_signs[LCD_SIGNS_NUM>>3];

uint8 gta_digit_decode[] __attribute__ ((section (".text"))) = {

    ( _BV(0) | _BV(1) | _BV(2) | _BV(3) | _BV(4) | _BV(5)          ),      // 0x0,   0,
    (          _BV(1) | _BV(2)                                     ),      // 0x1,   1,    //      0
    ( _BV(0) | _BV(1) |          _BV(3) | _BV(4) |          _BV(6) ),      // 0x2,   2,    //    ----
    ( _BV(0) | _BV(1) | _BV(2) | _BV(3) |                   _BV(6) ),      // 0x3,   3,    //   |    |1
    (          _BV(1) | _BV(2) |                   _BV(5) | _BV(6) ),      // 0x4,   4,    //  5| 6  |
    ( _BV(0) |          _BV(2) | _BV(3) |          _BV(5) | _BV(6) ),      // 0x6,   6,    //    ----
    ( _BV(0) |          _BV(2) | _BV(3) | _BV(4) | _BV(5) | _BV(6) ),      // 0x5,   5,    //   |    |2
    ( _BV(0) | _BV(1) | _BV(2)                                     ),      // 0x7,   7,    //  4|  3 |
    ( _BV(0) | _BV(1) | _BV(2) | _BV(3) | _BV(4) | _BV(5) | _BV(6) ),      // 0x8,   8,    //    ----
    ( _BV(0) | _BV(1) | _BV(2) | _BV(3) |          _BV(5) | _BV(6) ),      // 0x9,   9,
    ( _BV(0) | _BV(1) | _BV(2) |          _BV(4) | _BV(5) | _BV(6) ),      // 0xA,   A,
    (                   _BV(2) | _BV(3) | _BV(4) | _BV(5) | _BV(6) ),      // 0xB,   B,
    ( _BV(0) |                   _BV(3) | _BV(4) | _BV(5)          ),      // 0xC,   C,
    (          _BV(1) | _BV(2) | _BV(3) | _BV(4)          | _BV(6) ),      // 0xD,   D,
    ( _BV(0) |                   _BV(3) | _BV(4) | _BV(5) | _BV(6) ),      // 0xE,   E,
    ( _BV(0) |                            _BV(4) | _BV(5) | _BV(6) ),      // 0xF,   F,
    ( 0                                                            ),      // 0x10,  blank,
    ( _BV(0) | _BV(1) |                   _BV(4) | _BV(5) | _BV(6) ),      // 0x11,   P,
    ( _BV(0) | _BV(1) | _BV(2) | _BV(3) | _BV(4) | _BV(5)          ),      // 0x12,   O,
    (          _BV(1) | _BV(2) |          _BV(4) | _BV(5) | _BV(6) ),      // 0x13,   H
    (                            _BV(3) | _BV(4) | _BV(5)          ),      // 0x14    L,
    (          _BV(1) | _BV(2)                                     ),      // 0x15    I,    
    (          _BV(1) | _BV(2) | _BV(3) | _BV(4) | _BV(5)          ),      // 0x16,   U,
    (                   _BV(2) |          _BV(4) |          _BV(6) ),      // 0x17,   n
    (                   _BV(2) | _BV(3) | _BV(4) | _BV(5) | _BV(6) ),      // 0x18,   b
    (          _BV(1) | _BV(2) | _BV(3) | _BV(4)          | _BV(6) ),      // 0x19,   d,
    (                                     _BV(4) |          _BV(6) ),      // 0x1A,   r
    (                   _BV(2) | _BV(3) | _BV(4) |          _BV(6) ),      // 0x1B,   o
    (                   _BV(2)                                     ),      // 0x1C,   i
    (                   _BV(2) |          _BV(4) | _BV(5) | _BV(6) ),      // 0x1D,   h
    (                                                       _BV(6) ),      // 0x1E,   -
    (                            _BV(3)                            ),      // 0x1F,   _

    ( 0                                                            ),      // 0x20,   blank
    ( _BV(0)                                                       ),      // 0x21,   seg0
    (          _BV(1)                                              ),      // 0x22,   seg1
    (                   _BV(2)                                     ),      // 0x23,   seg2
    (                            _BV(3)                            ),      // 0x24,   seg3
    (                                     _BV(4)                   ),      // 0x25,   seg4
    (                                              _BV(5)          ),      // 0x26,   seg5
    (                                                       _BV(6) ),      // 0x27,   seg6
    



};


#define _LA(byte, bit) {(uint8)(uint16)(&LCDDR0+byte), _BV(bit)}      

T_DIGIT_MAP gta_digit_set0_map[LCD_SET_DIGITS_NUM] __attribute__ ((section (".text"))) = 
{
    // D.pos ,    Seg0,      Seg1,      Seg2,      Seg3,      Seg4,      Seg5,       Seg6
    {4,    { _LA( 1,2), _LA( 6,1), _LA(16,1), _LA(16,2), _LA(11,2), _LA( 6,2), _LA(11,1) }},
    {3,    { _LA( 1,0), _LA( 0,7), _LA(10,7), _LA(16,0), _LA(11,0), _LA( 6,0), _LA( 5,7) }},
    {2,    { _LA( 0,6), _LA( 0,5), _LA(10,5), _LA(15,6), _LA(10,6), _LA( 5,6), _LA( 5,5) }},
    {1,    { _LA( 0,4), _LA( 5,3), _LA(15,3), _LA(15,4), _LA(10,4), _LA( 5,4), _LA(10,3) }},
    {0,    { _LA( 0,2), _LA( 5,1), _LA(15,1), _LA(15,2), _LA(10,2), _LA( 5,2), _LA(10,1) }},
};

T_DIGIT_MAP gta_digit_set1_map[LCD_SET_DIGITS_NUM] __attribute__ ((section (".text"))) = 
{  // D.pos ,    Seg0,      Seg1,      Seg2,      Seg3,      Seg4,      Seg5,       Seg6
    {4,    { _LA(16,5), _LA(16,6), _LA( 6,6), _LA( 1,5), _LA( 6,5), _LA(11,5), _LA(11,6) }},
    {3,    { _LA(16,7), _LA(17,0), _LA( 7,0), _LA( 1,7), _LA( 6,7), _LA(11,7), _LA(12,0) }},
    {2,    { _LA(17,1), _LA(17,2), _LA( 7,2), _LA( 2,1), _LA( 7,1), _LA(12,1), _LA(12,2) }},
    {1,    { _LA(17,3), _LA(17,4), _LA( 7,4), _LA( 2,3), _LA( 7,3), _LA(12,3), _LA(12,4) }},
    {0,    { _LA(17,5), _LA(17,6), _LA( 7,6), _LA( 2,5), _LA( 7,5), _LA(12,5), _LA(12,6) }},
};

T_SEG_MAP gta_signs_map[LCD_SIGNS_NUM] __attribute__ ((section (".text"))) = {
//                Description                  Bit Number
   _LA(15,5),  // dec. point 0 set 0      //     0 
   _LA(15,7),  // dec. point 1 set 0      //     1 
   _LA(15,0),  // set 0 in meters         //     2 
   _LA(10,0),  // set 0 in foots          //     3 
                                               
   _LA( 2,4),  // dec. point 0 set 1      //     4 
   _LA( 2,2),  // dec. point 1 set 1      //     5 
   _LA( 2,0),  // dec. point 2 set 1      //     6 
   _LA(12,7),  // set 1 in meters         //     7 
   _LA(13,0),  // set 1 in foots          //     8 
                                             
   _LA( 5,0),  // ref point 0             //     9 
   _LA( 0,0),  // ref point 1             //    10
   _LA( 3,0),  // ref point 2             //    11
   _LA( 8,0),  // ref point 3             //    12
                                               
   _LA( 2,7),  // case                    //    13
   _LA( 7,7),  // Laser enable            //    14
   _LA( 2,6),  // temperature             //    15
   _LA( 0,1),  // Battery                 //    16
   _LA( 0,3),  // continous measure       //    17
                                               
   _LA( 1,1),  // cube front bott         //    18
   _LA( 1,4),  // cube front right        //    19
   _LA( 1,3),  // cube front diag         //    20
   _LA( 6,3),  // cube front top left     //    21
   _LA( 1,6),  // cube bott right         //    22
   _LA( 6,4),  // cube back other         //    23
                                             
   _LA(16,3),  // M                       //    24
   _LA(16,4),  // equal top               //    25
   _LA(11,3),  // equal bott              //    26
   _LA(11,4),  // plus                    //    27
   _LA(17,7),  // "2"                     //    28
   _LA(18,0),  // "3"                     //    29
   _LA(18,0),  // not in use              //    30
   _LA(18,0),  // not in use              //    31
};

/***C*F************************************************************************
**
** FUNCTION     :   
** DATE         :   2011-05-09
** AUTHOR       :   Andrejs Vasiljevs (AV)
**
** DESCRIPTION  :   Initialize LCD registers
**
***C*F*E**********************************************************************/
void init_lcd(){

    uint8 uc_i;
//LCDFFR 0x65
//LCDCCR 0x17
//LCDCRB 0x37
//LCDCRA 0x80

    // Disable LCD. Just in case...
    LCDCRA = 0;

    // 
    LCDFRR = 
        (5 << LCDCD0) |         // LCD Clock Divide (N+1)
        (6 << LCDPS0);          // 6 -> clkLCD/2048

    // Contrast = 100%
    LCDCCR = 
        (1 << LCDMDT);

    // 
    LCDCRB = 
        (3 << LCDMUX0) |     // DUTY = 1/4; BIAS = 1/2 or 1/3; COM0:3
        (7 << LCDPM0);       // LCD port mask SEG0:24 25

    // Enable LCD
    LCDCRA = 
        (1 << LCDEN);


    for (uc_i = 0; uc_i < LCD_SET_DIGITS_NUM; uc_i++)
    {
        guca_lcd_digits_set0[uc_i] = 0;    
        guca_lcd_digits_set1[uc_i] = 0;    
    }

    for (uc_i = 0; uc_i < sizeof(guca_lcd_signs); uc_i ++)
    {
        guca_lcd_signs[uc_i] = 0;
    }


    // Hello
    // -----
    guca_lcd_digits_set0[0] = 0x1E;
    guca_lcd_digits_set0[1] = 0x1E;
    guca_lcd_digits_set0[2] = 0x1E;
    guca_lcd_digits_set0[3] = 0x1E;
    guca_lcd_digits_set0[4] = 0x1E;

    guca_lcd_digits_set1[0] = 0x13;
    guca_lcd_digits_set1[1] = 0x0E;
    guca_lcd_digits_set1[2] = 0x14;
    guca_lcd_digits_set1[3] = 0x14;
    guca_lcd_digits_set1[4] = 0x12;
 
    LCD_SET_SIGN(13);
 
    lcd_render();

}

/***C*F************************************************************************
**
** FUNCTION     :   
** DATE         :   2011-05-09
** AUTHOR       :   Andrejs Vasiljevs (AV)
**
** DESCRIPTION  :   Decomposite data from global digits sets 
**                  (guca_lcd_digits_set0/1[]) and sign set (guca_lcd_signs[])
**                  to LCD registers.
**                  Digits are decoded according to the table gta_digit_decode[]
**                  Signs  are decoded according to the table gta_signs_map[]
**
** Performance  :   ~2500 cycles
***C*F*E**********************************************************************/
void lcd_render()
{
    uint8 uc_i;
    uint8 uc_digit_mask;
    uint8 uc_digit, uc_set;

    // Clear all segments
    for(uc_i = 0; uc_i < (uint8)(&LCDDR18 - &LCDDR0 + 1); uc_i ++)
    {
        (&LCDDR0)[uc_i] = 0;
    }

    // -----------------------------------------------------------------
    // --- Decomposite Digits set 0/1
    // -----------------------------------------------------------------
    T_DIGIT_MAP *pt_digit_map;
    uint8   *puc_inp;

    for (uc_set = 0; uc_set < 2; uc_set ++)   
    {
        // init pointer to input data and decoding table
        if (uc_set == 0)
        {
            pt_digit_map = &gta_digit_set0_map[0];
            puc_inp = &guca_lcd_digits_set0[0];
        }
        else
        {
            pt_digit_map = &gta_digit_set1_map[0];
            puc_inp = &guca_lcd_digits_set1[0];
        }

        for (uc_digit = 0; uc_digit < LCD_SET_DIGITS_NUM; uc_digit ++)
        {               
            T_SEG_MAP *pt_seg_map;

            // validate puc_inp[uc_digit]
            // ...

            // Convert digit to 7segment bitmask        
            uc_digit_mask = pgm_read_byte_near( &gta_digit_decode[*puc_inp++]);

            pt_seg_map = &(pt_digit_map->ta_seg_map[0]);

            // decode segments from mask to LCD reg bits
            while(uc_digit_mask)
            {
                if (uc_digit_mask & 1)
                {
                    uint16 us_map;
                    us_map  = pgm_read_word_near( &pt_seg_map->uc_addr);
                    *(volatile uint8 *)(us_map & 0xFF) |= (us_map >> 8);           // AV Note: LCD register address is 8bits long
                }          
                uc_digit_mask >>= 1;
                pt_seg_map++;
            }
            pt_digit_map++;

        } // End if digits for
    } // End of sets for

    //
    // -----------------------------------------------------------------
    // --- Decomposite Other signs according to received bit mask
    // -----------------------------------------------------------------
    uint8 uc_sign, uc_mask;
    T_SEG_MAP *pt_sign_map;

    pt_sign_map = &gta_signs_map[0];
    puc_inp = &guca_lcd_signs[0];

    uc_mask = 0; // AV: warning suppression

    for (uc_sign = 0; uc_sign < LCD_SIGNS_NUM; uc_sign++)
    {
        if ((uc_sign & 0x7) == 0)
            uc_mask = *puc_inp++;

        if (uc_mask & 1)
        {
            uint16 us_map;
            us_map  = pgm_read_word_near( &pt_sign_map->uc_addr);
            *(volatile uint8 *)(us_map & 0xFF) |= (us_map >> 8);           // AV Note: LCD register address is 8bits long
        }
        pt_sign_map++;
        uc_mask >>= 1;
    }

} 

/***C*F************************************************************************
**
** FUNCTION     :   
** DATE         :   2011-05-09
** AUTHOR       :   Andrejs Vasiljevs (AV)
**
** DESCRIPTION  :   Straight forward conversion from decimal to BCD format
**                  2345 => 0x10 0x02 0x03 0x04 0x05
**
** Performance  :   ~250 cycles
***C*F*E**********************************************************************/
void dec2bcd(uint8 *puc_dst, uint16 us_distance)
{
    uint8   uc_tmp, uc_leading, uc_short_distance;


    // [0] = 10000
    // [1] = 1000
    // [2] = 100
    // [3] = 10
    // [4] = 1

    // [0] = 10000
    uc_leading = 1;
    uc_tmp = 0;
    while(us_distance >= 10000)
    {
        us_distance -= 10000;
        uc_tmp ++;
    }

    if (uc_tmp == 0)    {        if (uc_leading) uc_tmp = 0x10;    }
    else                {        uc_leading = 0;                   }

    puc_dst[0] = uc_tmp;

    // [1] = 1000
    uc_tmp = 0;
    while(us_distance >= 1000)
    {
        us_distance -= 1000;
        uc_tmp ++;
    }

    if (uc_tmp == 0)    {        if (uc_leading) uc_tmp = 0x10;    }
    else                {        uc_leading = 0;                   }
    puc_dst[1] = uc_tmp;


    // [2] = 100
    uc_tmp = 0;
    while(us_distance >= 100)
    {
        us_distance -= 100;
        uc_tmp ++;
    }

    if (uc_tmp == 0)    {        if (uc_leading) uc_tmp = 0x10;    }
    else                {        uc_leading = 0;                   }
    puc_dst[2] = uc_tmp;

    // [3] = 10
    uc_short_distance = (uint8)us_distance;
    uc_tmp = 0;
    while(uc_short_distance >= 10)
    {
        uc_short_distance -= 10;
        uc_tmp ++;
    }

    if (uc_tmp == 0)    {        if (uc_leading) uc_tmp = 0x10;    }
    else                {        uc_leading = 0;                   }
    puc_dst[3] = uc_tmp;


    puc_dst[4] = uc_short_distance;

}
