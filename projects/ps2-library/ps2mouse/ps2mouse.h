/*************************************************************************************
 Title:     C include file for the PS/2 mouse library (ps2mouse.c)
 Author:    Jan Pieter de Ruiter <janpieterd@lycos.nl>
 Date:      13-Jun-2002
 Software:  AVR-GCC with AvrStudio
 Target:    Any AVR device

 DESCRIPTION
       Advanced routines for implementing the ps/2 protocol for a mouse.
       I think the function names speaks for themselve.
       They return 1 if the function succeeds, 0 if they fail.
       You can get the errorcode (RESEND or ERROR) with ps2_GetError() if a function fails.
       I found that some mice does not work with these functions, so use these functions
       at your own risk

**************************************************************************************/

#ifndef __PS2MOUSE_H_
#define __PS2MOUSE_H_ 1

#define STREAM 0
#define REMOTE 1
#define WRAP   2

#define STANDARDMOUSE    0x00
#define SCROLLMOUSE      0x03
#define FIVEBUTTONMOUSE  0x04

#define ONECOUNT_MM   0
#define TWOCOUNT_MM   1
#define FOURCOUNT_MM  2
#define EIGHTCOUNT_MM 3

#define SCALING1_1 0
#define SCALING2_1 1

#define RESEND 0xFE
#define ERROR  0xFC

int lasterror;
int MouseID;
int MouseMode;
int oldMouseMode;

struct MouseData
{
	int Xmovement;
	int Ymovement;
	int Zmovement;
	int leftbuttonpressed;
	int middlebuttonpressed;
	int rightbuttonpressed;
	int fourthbuttonpressed;
	int fifthbuttonpressed;
};

struct MouseStatus
{
	int mode;
	int reportenabled;
	int scaling;
	int leftbuttonpressed;
	int middlebuttonpressed;
	int rightbuttonpressed;
	int resolution;
	int samplerate;
};

int ps2_GetError(void);
int ps2_Init(void);
int ps2_Reset(void);
int ps2_Resend(void);
int ps2_SetDefault(void);
int ps2_DisableReport(void);
int ps2_EnableReport(void);
int ps2_SetSampleRate(int rate);
int ps2_GetID(void);
int ps2_SetRemoteMode(void);
int ps2_SetWrapMode(void);
int ps2_ResetWrapMode(void);
int ps2_ReadData(struct MouseData *data);
int ps2_SetStreamMode(void);
int ps2_GetStatus(struct MouseStatus *status);
int ps2_SetResolution(int resolution);
int ps2_SetScaling(int scaling);
int ps2_EnterScrollMode(void);
int ps2_Enter5ButtonMode(void);

#endif
