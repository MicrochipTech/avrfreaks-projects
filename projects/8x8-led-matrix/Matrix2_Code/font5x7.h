//* \file font5x7.h \brief Graphic LED Matrix Font (Ascii Characters). */
//*****************************************************************************
//
// File Name	: 'font5x7.h'
// Title		: Graphic LCD Font (Ascii Charaters)
// Author		: Joe Clark
// Date			: 05/20/2007
// Revised		: 05/20/2007
// Version		: 0.1
// Target MCU	: Atmel AVR
// Editor Tabs	: 4
//
//*****************************************************************************

#ifndef FONT5X7_H
	#define FONT5X7_H
#endif

	#include <avr/pgmspace.h>

// standard ascii 5x7 font
// defines ascii characters 0x20-0x7F (32-127)
static uint8_t font5x7[] PROGMEM = {
0x00, 0x00, 0x00, 0x00, 0x00,	//space
0x00, 0x00, 0xF2, 0x00, 0x00,	//!
0x00, 0xE0, 0x00, 0xE0, 0x00,	//"
0x28, 0xFE, 0x28, 0xFE, 0x28,	//#
0x24, 0x54, 0xFE, 0x54, 0x48,	//0
0xC4, 0xC8, 0x10, 0x26, 0x46,	//%
0x6C, 0x92, 0xAA, 0x44, 0x04,	//&
0x00, 0xA0, 0xC0, 0x00, 0x00,	//apos
0x00, 0x38, 0x44, 0x82, 0x00,
0x00, 0x82, 0x44, 0x38, 0x00,	//)
0x28, 0x10, 0x7C, 0x10, 0x28,	// *
0x10, 0x10, 0x7C, 0x10, 0x10,	//+
0x0A, 0x0C, 0x00, 0x00, 0x00,	//,
0x10, 0x10, 0x10, 0x10, 0x10,	//-
0x00, 0x06, 0x06, 0x00, 0x00,	// degree sign
0x04, 0x08, 0x10, 0x20, 0x40,	// /
0x7C, 0x8A, 0x92, 0xA2, 0x7C,	//0
0x00, 0x42, 0xFE, 0x02, 0x00,	//1
0x42, 0x86, 0x8A, 0x92, 0x62,	//2
0x84, 0x82, 0xA2, 0xD2, 0x8C,	//3
0x18, 0x28, 0x48, 0xFE, 0x08,	//4
0xE4, 0xA2, 0xA2, 0xA2, 0x9C,	//5
0x3C, 0x52, 0x92, 0x92, 0x0C,	//6
0x80, 0x8E, 0x90, 0xA0, 0xC0,	//7
0x6C, 0x92, 0x92, 0x92, 0x6C,	//8
0x60, 0x92, 0x92, 0x94, 0x78,	//9
0x00, 0x6C, 0x6C, 0x00, 0x00,	//:
0x00, 0x6A, 0x6C, 0x00, 0x00,	//;
0x10, 0x28, 0x44, 0x82, 0x00,	//<
0x28, 0x28, 0x28, 0x28, 0x28,	//=
0x00, 0x82, 0x44, 0x28, 0x10,	//>
0x40, 0x80, 0x8A, 0x90, 0x60,	//?
0x7C, 0x82, 0xBA, 0xAA, 0x78,	//@
0x7E, 0x88, 0x88, 0x88, 0x7E,	//A
0xFE, 0x92, 0x92, 0x92, 0x6C,	//B
0x7C, 0x82, 0x82, 0x82, 0x44,	//C
0xFE, 0x82, 0x82, 0x44, 0x38,	//D
0xFE, 0x92, 0x92, 0x92, 0x82,	//E
0xFE, 0x90, 0x90, 0x90, 0x80,	//F
0x7C, 0x82, 0x92, 0x92, 0x5E,	//G
0xFE, 0x10, 0x10, 0x10, 0xFE,	//H
0x00, 0x82, 0xFE, 0x82, 0x00,	//I
0x04, 0x02, 0x82, 0xFC, 0x80,	//J
0xFE, 0x10, 0x28, 0x44, 0x82,	//K
0xFE, 0x02, 0x02, 0x02, 0x02,	//L
0xFE, 0x40, 0x30, 0x40, 0xFE,	//M
0xFE, 0x20, 0x10, 0x08, 0xFE,	//N
0x7C, 0x82, 0x82, 0x82, 0x7C,	//O
0xFE, 0x90, 0x90, 0x90, 0x60,	//P
0x7C, 0x82, 0x8A, 0x84, 0x7A,	//Q
0xFE, 0x90, 0x98, 0x94, 0x62,	//R
0x64, 0x92, 0x92, 0x92, 0x4C,	//S
0x80, 0x80, 0xFE, 0x80, 0x80,	//T
0xFC, 0x02, 0x02, 0x02, 0xFC,	//U
0xF8, 0x04, 0x02, 0x04, 0xF8,	//V
0xFC, 0x02, 0x1C, 0x02, 0xFC,	//W
0xC6, 0x28, 0x10, 0x28, 0xC6,	//X
0xE0, 0x10, 0x0E, 0x10, 0xE0,	//Y
0x86, 0x8A, 0x92, 0xA2, 0xC2,	//Z
0x00, 0xFE, 0x82, 0x82, 0x00,	//[
0x40, 0x20, 0x10, 0x08, 0x04,	//Backslash
0x00, 0x82, 0x82, 0xFE, 0x00,	//]
0x20, 0x40, 0x80, 0x40, 0x20,	//^
0x02, 0x02, 0x02, 0x02, 0x02,	//_
0x00, 0x00, 0xC0, 0xA0, 0x00,	//`
0x04, 0x2A, 0x2A, 0x2A, 0x1E,	//a
0xFE, 0x22, 0x22, 0x22, 0x1C,	//b
0x1C, 0x22, 0x22, 0x22, 0x22,	//c
0x1C, 0x22, 0x22, 0x22, 0xFE,	//d
0x1C, 0x2A, 0x2A, 0x2A, 0x18,	//e
0x20, 0x20, 0x7E, 0xA0, 0xA0,	//f
0x10, 0x2A, 0x2A, 0x2A, 0x3C,	//g
0xFE, 0x10, 0x20, 0x20, 0x1E,	//h
0x00, 0x22, 0xBE, 0x02, 0x00,	//i
0x04, 0x02, 0x22, 0xBC, 0x00,	//j
0xFE, 0x08, 0x14, 0x22, 0x00,	//k
0x00, 0x82, 0xFE, 0x02, 0x00,	//l
0x3E, 0x20, 0x3E, 0x20, 0x1E,	//m
0x3E, 0x10, 0x20, 0x20, 0x1E,	//n
0x1C, 0x22, 0x22, 0x22, 0x1C,	//o
0x3E, 0x28, 0x28, 0x28, 0x10,	//p
0x10, 0x28, 0x28, 0x28, 0x3E,	//q
0x3E, 0x10, 0x20, 0x20, 0x00,	//r
0x12, 0x2A, 0x2A, 0x2A, 0x24,	//s
0x20, 0x20, 0xFC, 0x22, 0x22,	//t
0x3C, 0x02, 0x02, 0x04, 0x3E,	//u
0x38, 0x04, 0x02, 0x04, 0x38,	//v
0x3C, 0x02, 0x0C, 0x02, 0x3C,	//w
0x22, 0x14, 0x08, 0x14, 0x22,	//x
0x30, 0x0A, 0x0A, 0x0A, 0x3C,	//y
0x22, 0x26, 0x2A, 0x32, 0x22,	//z
0x10, 0x6C, 0x82, 0x82, 0x00,	//
0x00, 0x00, 0xEE, 0x00, 0x00,	//|
0x00, 0x82, 0x82, 0x6C, 0x10,	//
0x10, 0x10, 0x54, 0x38, 0x10,	//<
0x10, 0x38, 0x54, 0x10, 0x10};	//-
