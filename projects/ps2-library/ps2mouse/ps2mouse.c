/*************************************************************************************
 Title:     PS/2 mouse library
 Author:    Jan Pieter de Ruiter <janpieterd@lycos.nl>
 Date:      13-Jun-2002
 Software:  AVR-GCC with AvrStudio
 Target:    Any AVR device.

 DESCRIPTION
       Advanced routines for implementing the ps/2 protocol for a mouse.
       
       See http://panda.cs.ndsu.nodak.edu/~achapwes/PICmicro/ps2/ps2.html
       and http://panda.cs.ndsu.nodak.edu/~achapwes/PICmicro/mouse/mouse.html
       for more info

 USAGE
       See the C include ps2mouse.h file for a detailed description
       
**************************************************************************************/
#include "ps2mouse.h"
#include "ps2.h"
#include <avr/io.h>

int ps2_GetError(void)
{
	return lasterror;
}

int ps2_Reset(void)
{
	Write_ps2data(0xFF);
	lasterror=Read_ps2data();
	if(lasterror!=0xFA)return 0;
	lasterror=Read_ps2data();
	if(lasterror!=0xAA)return 0;
	lasterror=0;
	MouseID=Read_ps2data();
	MouseMode=0;
	return 1;
}

int ps2_Init(void)
{
	lasterror=Read_ps2data();
	if(lasterror!=0xAA)return 0;
	MouseID=Read_ps2data();
	MouseMode=0;
	return 1;
}

int ps2_Resend(void)
{
	Write_ps2data(0xFE);
	return Read_ps2data();
}

int ps2_SetDefault(void)
{
	Write_ps2data(0xF6);
	lasterror=Read_ps2data();
	if(lasterror!=0xFA)return 0;
	lasterror=0;
	return 1;
}

int ps2_DisableReport(void)
{
	Write_ps2data(0xF5);
	lasterror=Read_ps2data();
	if(lasterror!=0xFA)return 0;
	lasterror=0;
	return 1;
}

int ps2_EnableReport(void)
{
	Write_ps2data(0xF4);
	lasterror=Read_ps2data();
	if(lasterror!=0xFA)return 0;
	lasterror=0;
	return 1;
}

int ps2_SetSampleRate(int rate)
{
	Write_ps2data(0xF3);
	lasterror=Read_ps2data();
	if(lasterror!=0xFA)return 0;
	else
	{
		Write_ps2data(rate);
		lasterror=Read_ps2data();
		if(lasterror!=0xFA)return 0;
	}
	lasterror=0;
	return 1;
}

int ps2_GetID(void)
{
	Write_ps2data(0xF2);
	lasterror=Read_ps2data();
	if(lasterror!=0xFA)return 0;
	lasterror=0;
	MouseID=Read_ps2data();
	return MouseID;
}

int ps2_SetRemoteMode(void)
{
	Write_ps2data(0xF0);
	lasterror=Read_ps2data();
	if(lasterror!=0xFA)return 0;
	lasterror=0;
	MouseMode=1;
	return 1;
}

int ps2_SetWrapMode(void)
{
	Write_ps2data(0xEE);
	lasterror=Read_ps2data();
	if(lasterror!=0xFA)return 0;
	lasterror=0;
	oldMouseMode=MouseMode;
	MouseMode=2;
	return 1;
}

int ps2_ResetWrapMode(void)
{
	Write_ps2data(0xEC);
	lasterror=Read_ps2data();
	if(lasterror!=0xFA)return 0;
	lasterror=0;
	MouseMode=oldMouseMode;
	return 1;
}

int ps2_ReadData(struct MouseData *data)
{
	int mouseinf;
	int posx;
	int posy;
	int deltax;
	int deltay;
	int extrabyte;
	if(MouseMode==STREAM)
	{
		mouseinf=Read_ps2data();
		posx=Read_ps2data();
		posy=Read_ps2data();
		if(MouseID!=0x00)extrabyte=Read_ps2data();
		data->Xmovement=deltax-(0x10*(mouseinf&0x10));
		data->Ymovement=deltay-(0x08*(mouseinf&0x20));
		data->leftbuttonpressed=mouseinf&0x01;
		data->middlebuttonpressed=(mouseinf&0x04)/0x04;
		data->rightbuttonpressed=(mouseinf&0x02)/0x02;
		if(MouseID==0x03)data->Zmovement=(extrabyte&0x07)-(0x20*(extrabyte&0x08));
		if(MouseID==0x04)
		{
			data->Zmovement=(extrabyte&0x07)-(0x20*(extrabyte&0x08));
			data->fourthbuttonpressed=(extrabyte&0x10)/0x10;
			data->fifthbuttonpressed=(extrabyte&0x20)/0x20;
		}
		lasterror=0;
		return 1;
	}
	if(MouseMode==REMOTE)
	{
		Write_ps2data(0xEB);
		lasterror=Read_ps2data();
		if(lasterror!=0xFA)return 0;
		mouseinf=Read_ps2data();
		posx=Read_ps2data();
		posy=Read_ps2data();
		if(MouseID!=0x00)extrabyte=Read_ps2data();
		data->Xmovement=deltax-(0x10*(mouseinf&0x10));
		data->Ymovement=deltay-(0x08*(mouseinf&0x20));
		data->leftbuttonpressed=mouseinf&0x01;
		data->middlebuttonpressed=(mouseinf&0x04)/0x04;
		data->rightbuttonpressed=(mouseinf&0x02)/0x02;
		if(MouseID==0x03)data->Zmovement=(extrabyte&0x07)-(0x20*(extrabyte&0x08));
		if(MouseID==0x04)
		{
			data->Zmovement=(extrabyte&0x07)-(0x20*(extrabyte&0x08));
			data->fourthbuttonpressed=(extrabyte&0x10)/0x10;
			data->fifthbuttonpressed=(extrabyte&0x20)/0x20;
		}
		lasterror=0;
		return 1;
	}
	return 0;
}

int ps2_SetStreamMode(void)
{
	Write_ps2data(0xEA);
	lasterror=Read_ps2data();
	if(lasterror!=0xFA)return 0;
	lasterror=0;
	MouseMode=STREAM;
	return 1;
}

int ps2_GetStatus(struct MouseStatus *status)
{
	int byte;
	Write_ps2data(0xE9);
	lasterror=Read_ps2data();
	if(lasterror!=0xFA)return 0;
	lasterror=0;
	byte=Read_ps2data();
	status->resolution=Read_ps2data();
	status->samplerate=Read_ps2data();
	status->leftbuttonpressed=(byte&0x04)/0x04;
	status->middlebuttonpressed=(byte&0x02)/0x02;
	status->rightbuttonpressed=byte&0x01;
	status->scaling=(byte&0x10)/0x10;
	status->reportenabled=(byte&0x20)/0x20;
	status->mode=(byte&0x40)/0x40;
	return 1;
}
	
int ps2_SetResolution(int resolution)
{
	Write_ps2data(0xE8);
	lasterror=Read_ps2data();
	if(lasterror!=0xFA)return 0;
	Write_ps2data(resolution);
	lasterror=Read_ps2data();
	if(lasterror!=0xFA)return 0;
	return 1;
}

int ps2_SetScaling(int scaling)
{
	if(scaling)Write_ps2data(0xE7);
	else Write_ps2data(0xE6);
	lasterror=Read_ps2data();
	if(lasterror!=0xFA)return 0;
	return 1;
}	

int ps2_EnterScrollMode(void)
{
	Write_ps2data(0xF3);
	lasterror=Read_ps2data();
	if(lasterror!=0xFA)return 0;
	Write_ps2data(200);
	lasterror=Read_ps2data();
	if(lasterror!=0xFA)return 0;
	Write_ps2data(0xF3);
	lasterror=Read_ps2data();
	if(lasterror!=0xFA)return 0;
	Write_ps2data(100);
	lasterror=Read_ps2data();
	if(lasterror!=0xFA)return 0;
	Write_ps2data(0xF3);
	lasterror=Read_ps2data();
	if(lasterror!=0xFA)return 0;
	Write_ps2data(80);
	lasterror=Read_ps2data();
	if(lasterror!=0xFA)return 0;
	Write_ps2data(0xF2);
	lasterror=Read_ps2data();
	if(lasterror!=0xFA)return 0;
	MouseID=Read_ps2data();
	return 1;
}
int ps2_Enter5ButtonMode(void)
{
	Write_ps2data(0xF3);
	lasterror=Read_ps2data();
	if(lasterror!=0xFA)return 0;
	Write_ps2data(200);
	lasterror=Read_ps2data();
	if(lasterror!=0xFA)return 0;
	Write_ps2data(0xF3);
	lasterror=Read_ps2data();
	if(lasterror!=0xFA)return 0;
	Write_ps2data(200);
	lasterror=Read_ps2data();
	if(lasterror!=0xFA)return 0;
	Write_ps2data(0xF3);
	lasterror=Read_ps2data();
	if(lasterror!=0xFA)return 0;
	Write_ps2data(80);
	lasterror=Read_ps2data();
	if(lasterror!=0xFA)return 0;
	Write_ps2data(0xF2);
	lasterror=Read_ps2data();
	if(lasterror!=0xFA)return 0;
	MouseID=Read_ps2data();
	return 1;
}
