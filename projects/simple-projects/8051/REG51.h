/*--------------------------------------------------------------------------
REG51.H

Header file for generic 80C51 and 80C31 microcontroller.
Copyright (c) 1988-2002 Keil Elektronik GmbH and Keil Software, Inc.
All rights reserved.
--------------------------------------------------------------------------*/

#ifndef __REG51_H__
#define __REG51_H__

/*  BYTE Register  */
///////////////////
sfr P0   = 0x80;

sfr P0_0   = 0x80;
sfr P0_1   = 0x81;
sfr P0_2   = 0x82;
sfr P0_3   = 0x83;
sfr P0_4   = 0x84;
sfr P0_5   = 0x85;
sfr P0_6   = 0x86;
sfr P0_7   = 0x87;
/////////////////
sfr P1   = 0x90;

sfr P1_0   = 0x90;
sfr P1_1   = 0x91;
sfr P1_2   = 0x92;
sfr P1_3   = 0x93;
sfr P1_4   = 0x94;
sfr P1_5   = 0x95;
sfr P1_6   = 0x96;
sfr P1_7   = 0x97;
/////////////////
sfr P2   = 0xA0;

sfr P2_0   = 0xA0;
sfr P2_1   = 0xA1;
sfr P2_2   = 0xA2;
sfr P2_3   = 0xA3;
sfr P2_4   = 0xA4;
sfr P2_5   = 0xA5;
sfr P2_6   = 0xA6;
sfr P2_7   = 0xA7;
/////////////////
sfr P3   = 0xB0;

sfr P3_0   = 0xB0;
sfr P3_1   = 0xB1;
sfr P3_2   = 0xB2;
sfr P3_3   = 0xB3;
sfr P3_4   = 0xB4;
sfr P3_5   = 0xB5;
sfr P3_6   = 0xB6;
sfr P3_7   = 0xB7;
/////////////////
sfr PSW  = 0xD0;
sfr ACC  = 0xE0;
sfr B    = 0xF0;
sfr SP   = 0x81;
sfr DPL  = 0x82;
sfr DPH  = 0x83;
sfr PCON = 0x87;
sfr TCON = 0x88;
sfr TMOD = 0x89;
sfr TL0  = 0x8A;
sfr TL1  = 0x8B;
sfr TH0  = 0x8C;
sfr TH1  = 0x8D;
sfr IE   = 0xA8;
sfr IP   = 0xB8;
sfr SCON = 0x98;
sfr SBUF = 0x99;


/*  BIT Register  */
/*  PSW   */
sbit CY   = 0xD7;
sbit AC   = 0xD6;
sbit F0   = 0xD5;
sbit RS1  = 0xD4;
sbit RS0  = 0xD3;
sbit OV   = 0xD2;
sbit P    = 0xD0;

/*  TCON  */
sbit TF1  = 0x8F;
sbit TR1  = 0x8E;
sbit TF0  = 0x8D;
sbit TR0  = 0x8C;
sbit IE1  = 0x8B;
sbit IT1  = 0x8A;
sbit IE0  = 0x89;
sbit IT0  = 0x88;

/*  IE   */
sbit EA   = 0xAF;
sbit ES   = 0xAC;
sbit ET1  = 0xAB;
sbit EX1  = 0xAA;
sbit ET0  = 0xA9;
sbit EX0  = 0xA8;

/*  IP   */ 
sbit PS   = 0xBC;
sbit PT1  = 0xBB;
sbit PX1  = 0xBA;
sbit PT0  = 0xB9;
sbit PX0  = 0xB8;

/*  P3  */
sbit RD   = 0xB7;
sbit WR   = 0xB6;
sbit T1   = 0xB5;
sbit T0   = 0xB4;
sbit INT1 = 0xB3;
sbit INT0 = 0xB2;
sbit TXD  = 0xB1;
sbit RXD  = 0xB0;

/*  SCON  */
sbit SM0  = 0x9F;
sbit SM1  = 0x9E;
sbit SM2  = 0x9D;
sbit REN  = 0x9C;
sbit TB8  = 0x9B;
sbit RB8  = 0x9A;
sbit TI   = 0x99;
sbit RI   = 0x98;

#endif
