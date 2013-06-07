/*
 * CX80_SystemCalls.c
 *
 * Created: 19/04/2013 12:55:53
 *  Author: Calogiuri Enzo Antonio
 */ 

#include "CX80.h"
#include "CX80_Ff.h"
#include "CX80_Keyboard.h"
#include "CX80_OS.h"

#define	Z80STRING_LEN			128

extern FIL Files[3];
extern char FileSlot[3];

/*Search a free file slot, return 0xFF if no free file slot*/
static byte Find_Free_File_Slot(void)
{
	byte i, Ret = 0xFF;
	
	for (i = 0; i < 3; i++)
		if (FileSlot[i] == 0)
		{
			Ret = i;
			
			break;
		}		
			
	return Ret;
}

/*Extract a string from ram*/
static void GetZ80String(word Addr, byte *buf)
{
	byte i, v;
	
	for (i = 0; i < Z80STRING_LEN; i++)
	{
		v = RMem(Addr);
		
		buf[i] = v;
		
		if (v == 0)
			break;
			
		Addr++;
	}
}

/*Wait...*/
static void sys_delay_ms(unsigned int n)
{
	while (n--)
		_delay_ms(1);
}

/*Play a sound*/
static void CX80_PlaySound(float duration, float frequency)
{
	long int i, cycles;
	float half_period, wavelength;
	
	wavelength = (1 / frequency) * 1000;
	cycles = duration / wavelength;
	half_period = wavelength / 2;
	
	for (i = 0; i <cycles; i++)	
	{
		sys_delay_ms((int)(half_period));
		
		SYSTEM_BUS_PORT |= (1 << SYSTEM_SND);
		
		sys_delay_ms((int)(half_period));
		
		SYSTEM_BUS_PORT &= ~(1 << SYSTEM_SND);
	}
}

/*Execute the system call*/
void CX80_Do_SystemCalls(void)
{
	register byte SystemCallNumber = RMem(CX80_SYSTEM_CALL_FNC);
	byte tmp, v;
	char *txt;
	word reg1, reg2;
	byte Z80String[Z80STRING_LEN], NewBuf[Z80STRING_LEN];
	FRESULT rc;
	uInt br;
	
	switch(SystemCallNumber)
	{
		case				0x00:	//Print a string on video
			reg1 = RMem16(CX80_SYSTEM_REG1L);	//REG1 point to a 0 terminated string into ram
			
			do 
			{
				tmp = RMem(reg1);
				
				CX80_Usart_SendChar((char)tmp);
				
				reg1++;
			} while (tmp != 0);
			break;
			
		case				0x01:	//Print a single character on screen
			tmp = RMem(CX80_SYSTEM_REG1L);		//REG1L is ASCII value of character to print
			
			CX80_Usart_SendChar((char)tmp);
			break;
			
		case				0x02:	//Move the cursor up
			CX80_Do_CursorUp();
			break;
			
		case				0x03:	//Move the cursor down
			CX80_Do_CursorDown();
			break;
			
		case				0x04:	//Move the cursor Right
			CX80_Do_CursorRight();
			break;
			
		case				0x05:	//Move the cursor left
			CX80_Do_CursorLeft();
			break;
			
		case				0x06:	//Clear the screen
			CX80_Do_ClearScreen();
			break;
			
		case				0x07:	//Move cursor to home
			CX80_Do_CursorHome();
			break;
			
		case				0x08:	//Set Reverse Index on screen
			CX80_Do_ReverseLineFeed();
			break;
			
		case				0x09:	//Clears from the cursor (inclusive) to the end of the screen
			CX80_Do_ClearToEndOfScreen();
			break;
			
		case				0x0A:	//Clears from the cursor (inclusive) to the end of the line
			CX80_Do_ClearToEndOfLine();
			break;
			
		case				0x0B:	//Moves the cursor to the specified row and column.
			CX80_Do_MoveCursor(RMem(CX80_SYSTEM_REG1L), RMem(CX80_SYSTEM_REG1H));	//REG1L = row to move, REG1H = column to move
			break;
			
		case				0x0C:	//Set single line dimension.
			CX80_Do_SetLineDimension(RMem(CX80_SYSTEM_REG1L));	//REG1L = type of line on screen ('0'..'5')
			break;
			
		case				0x0D:	//Clears from the cursor (inclusive) to the start of the screen.
			CX80_Do_ClearToStartOfScreen();
			break;
			
		case				0x0E:	//Show or hidden the cursor
			CX80_Do_CursorShow(RMem(CX80_SYSTEM_REG1L));	//REG1L = 1 show the cursor, 0 hidden the cursor
			break;
			
		case				0x0F:	//The current row/column is stored.
			CX80_Do_SaveCursorPosition();
			break;
			
		case				0x10:	//Restore the saved row/column
			CX80_Do_RestoreCursorPosition();
			break;
			
		case				0x11:	//Clears the current row and moves the cursor to column 0.
			CX80_Do_ClearLine();
			break;
			
		case				0x12:	//Clears from the cursor (inclusive) to column 0 on the current row.
			CX80_Do_ClearToStartOfLine();
			break;
			
		case				0x13:	//Enable or disable line overflow
			CX80_Do_LineOverflow(RMem(CX80_SYSTEM_REG1L));	//REG1L = 1 enable line overflow, 0 disable
			break;
			
		case				0x14:	//Set cursor to block shape
			CX80_Do_BlockCursor();
			break;
			
		case				0x15:	//Set cursor to underline shape
			CX80_Do_UnderlineCursor();
			break;
			
		case				0x16:	//set the invert output (black on white background) or normal output (white on black background)
			CX80_Do_VideoOutput(RMem(CX80_SYSTEM_REG1L));	//REG1L = 1 invert output, 0 normal output
			break;
			
		case				0x17:
			/*Open or create a file for input/output
			//REG1L = FA_READ, FA_WRITE, FA_CREATE_NEW, FA_CREATE_ALWAYS, FA_OPEN_EXISTING
			//REG2 = pointer to 0 terminated string with file name
			//
			Return value on REG1H:	0xEF = error on file
									0xFF = no free file slot	
									0 to 2 = number on memory ID to file (max 3 file in use)
			*/
			 
			tmp = Find_Free_File_Slot();
			
			if (tmp == 0xFF)
			{
				WMem(CX80_SYSTEM_REG1H, 0xFF);
				break;
			}
			
			GetZ80String(RMem16(CX80_SYSTEM_REG2L), Z80String);
			
			rc = f_open(&Files[tmp], (const TCHAR *)Z80String, RMem(CX80_SYSTEM_REG1L));
			
			if (rc)
				WMem(CX80_SYSTEM_REG1H, 0xEF);
			else
			{
				WMem(CX80_SYSTEM_REG1H, tmp);
				
				FileSlot[tmp] = 1;
			}
			
			break;
			
		case				0x18:
			/*	Read one byte from file
				REG1L = File ID
				REG1H = read value
				
				Return value on REG2L : 0xEF = wrong file slot
										0xAA = error on reading
										0x00 = read ok
										0xFF = EOF			
			*/
			tmp = RMem(CX80_SYSTEM_REG1L);
			
			if (tmp > 2)
				tmp = 2;
				
			if (FileSlot[tmp] == 0)
			{
				WMem(CX80_SYSTEM_REG2L, 0xEF);
				
				break;
			}
			
			rc = f_read(&Files[tmp], &v, 1, &br);
			
			if (!br)
			{
				WMem(CX80_SYSTEM_REG2L, 0xFF);
				
				break;
			}
			
			if (rc)
			{
				WMem(CX80_SYSTEM_REG2L, 0xAA);
				
				break;
			}
			
			WMem(CX80_SYSTEM_REG2L, 0x00);
			WMem(CX80_SYSTEM_REG1H, v);
			break;
			
		case				0x19:
			/*  Write one byte to file
				REG1L = File ID
				REG1H = value to write
				
				Return value on REG2L : 0xEF = wrong file slot
										0xAA = error on writing
										0x00 = write ok	
			*/
			tmp = RMem(CX80_SYSTEM_REG1L);
			
			if (tmp > 2)
				tmp = 2;
				
			if (FileSlot[tmp] == 0)
			{
				WMem(CX80_SYSTEM_REG2L, 0xEF);
				
				break;
			}
			
			rc = f_write(&Files[tmp], &v, 1, &br);
			
			if (rc)
			{
				WMem(CX80_SYSTEM_REG2L, 0xAA);
				
				break;
			}
			
			WMem(CX80_SYSTEM_REG2L, 0x00);
			break;
			
		case				0x1A:
			/*
				Close an open file.
				REG1L = File ID
				
				Return value on REG2L : 0xEF = wrong file slot										
										0x00 = close ok	
			*/
			tmp = RMem(CX80_SYSTEM_REG1L);
			
			if (tmp > 2)
				tmp = 2;
				
			if (FileSlot[tmp] == 0)
			{
				WMem(CX80_SYSTEM_REG2L, 0xEF);
				
				break;
			}
			
			f_close(&Files[tmp]);
	
			FileSlot[tmp] = 0;
			break;
			
		case				0x1B:
			/*
				Execute a seek on a file.
				REG1L = File ID
				REG2 = Most Significant Word
				REG3 = Least Significant Word
				
				Return value on REG1H : 0xEF = wrong file slot
										0xAA = error on seek
										0x00 = seek ok
			*/
			tmp = RMem(CX80_SYSTEM_REG1L);
			
			if (tmp > 2)
				tmp = 2;
				
			if (FileSlot[tmp] == 0)
			{
				WMem(CX80_SYSTEM_REG1H, 0xEF);
				
				break;
			}
			
			reg1 = RMem16(CX80_SYSTEM_REG2L);
						
			DWord Ofs = (DWord)((DWord)(reg1 << 16) + RMem16(CX80_SYSTEM_REG3L));
			
			rc = f_lseek(&Files[tmp], Ofs);
			
			if (rc)
			{
				WMem(CX80_SYSTEM_REG1H, 0xAA);
				
				break;
			}
			
			WMem(CX80_SYSTEM_REG1H, 0x00);
			break;
			
		case				0x1C:
			/*
				Read from file into ram buffer
				REG1L = File ID
				REG2 = Pointer to file buffer in ram
				REG3 = Number of bytes to read
				
				Return value REG1H : 0x00 = read ok
									 0xFF = EOF before REG3 = 0
										if REG1H = 0xFF, REG3 = bytes read before EOF
									 0xAA = Error while reading
									 0xEF = Wrong file slot
			*/
			tmp = RMem(CX80_SYSTEM_REG1L);
			
			if (tmp > 2)
				tmp = 2;
				
			if (FileSlot[tmp] == 0)
			{
				WMem(CX80_SYSTEM_REG1H, 0xEF);
				
				break;
			}
			
			reg1 = RMem16(CX80_SYSTEM_REG2L);
			reg2 = RMem16(CX80_SYSTEM_REG3L);
			
			while (reg2)
			{
				rc = f_read(&Files[tmp], &v, 1, &br);
				
				if (rc || !br)
					break;
					
				WMem(reg1, v);
				
				reg1++;
				reg2--;
			}
			
			if (rc)
			{
				WMem(CX80_SYSTEM_REG1H, 0xAA);
				
				break;
			}
			
			if (!br)
			{
				WMem(CX80_SYSTEM_REG1H, 0xFF);
				WMem16(CX80_SYSTEM_REG3L, reg2);
				
				break;
			}
			
			WMem(CX80_SYSTEM_REG1H, 0x00);
			break;
			
		case				0x1D:
			/*
				Write into file from ram buffer
				REG1L = File ID
				REG2 = Pointer to file buffer in ram
				REG3 = Number of bytes to write
				
				Return value REG1H : 0x00 = write ok									 
									 0xAA = Error while writing
									 0xEF = Wrong file slot
			*/
			tmp = RMem(CX80_SYSTEM_REG1L);
			
			if (tmp > 2)
				tmp = 2;
				
			if (FileSlot[tmp] == 0)
			{
				WMem(CX80_SYSTEM_REG1H, 0xEF);
				
				break;
			}
			
			reg1 = RMem16(CX80_SYSTEM_REG2L);
			reg2 = RMem16(CX80_SYSTEM_REG3L);
			
			while (reg2)
			{
				v = RMem(reg1);
				
				rc = f_write(&Files[tmp], &v, 1, &br);
				
				if (rc)
					break;
				
				reg1++;
				reg2--;
			}
			
			if (rc)
			{
				WMem(CX80_SYSTEM_REG1H, 0xAA);
				
				break;
			}
			
			WMem(CX80_SYSTEM_REG1H, 0x00);
			break;
			
		case				0x1E:
			/*
				Change the current directory
				REG2 = Pointer to path buffer in ram
				
				Return value on REG1H:	0xAA = error on chdir
										0x00 = ok
			*/			
			GetZ80String(RMem16(CX80_SYSTEM_REG2L), Z80String);
			
			rc = f_chdir((TCHAR *)Z80String);
			
			if (rc)
			{
				WMem(CX80_SYSTEM_REG1H, 0xAA);
				
				break;
			}
			
			WMem(CX80_SYSTEM_REG1H, 0x00);
			break;
			
		case				0x1F:
			/*
				Create a new directory
				REG2 = Pointer to 0 terminated dir name into ram
				
				Return value on REG1H:	0xAA = error on creation
										0x00 = ok
			*/
			GetZ80String(RMem16(CX80_SYSTEM_REG2L), Z80String);
			
			rc = f_mkdir((TCHAR *)Z80String);
			
			if (rc)
			{
				WMem(CX80_SYSTEM_REG1H, 0xAA);
				
				break;
			}
			
			WMem(CX80_SYSTEM_REG1H, 0x00);
			break;
			
		case				0x20:
			/*
				Delete a file/directory
				REG2 = Pointer to 0 terminated dir/file name into ram
				
				Return value on REG1H:	0xAA = error on delete
										0x00 = ok
			*/
			GetZ80String(RMem16(CX80_SYSTEM_REG2L), Z80String);
			
			rc = f_unlink((TCHAR *)Z80String);
			
			if (rc)
			{
				WMem(CX80_SYSTEM_REG1H, 0xAA);
				
				break;
			}
			
			WMem(CX80_SYSTEM_REG1H, 0x00);
			break;
			
		case				0x21:
			/*
				Rename or move a file/directory
				REG2 = Pointer to to old file/dir name (0 terminated)
				REG3 = Pointer to new file/dir name (0 terminated)
				
				Return value on REG1H:	0xAA = error on operation
										0x00 = ok
			*/
			GetZ80String(RMem16(CX80_SYSTEM_REG2L), Z80String);
			memcpy(NewBuf, Z80String, 128);
			GetZ80String(RMem16(CX80_SYSTEM_REG3L), Z80String);
			
			rc = f_rename((TCHAR *)Z80String, (TCHAR *)NewBuf);
			
			if (rc)
			{
				WMem(CX80_SYSTEM_REG1H, 0xAA);
				
				break;
			}
			
			WMem(CX80_SYSTEM_REG1H, 0x00);
			break;
			
		case				0x22:
			/*
				Get current directory
				REG1L = Length of path string into ram (max 128 chars)
				REG2 = Pointer to path string
			*/
			tmp = RMem(CX80_SYSTEM_REG1L);
			reg1 = RMem16(CX80_SYSTEM_REG2L);
			
			f_getcwd((TCHAR *)Z80String, tmp);
			
			for (v = 0; v < tmp; v++)
			{
				WMem(reg1, Z80String[v]);
				
				reg1++;
			}
			
			break;
			
		case				0x23:
			/*
				Return the pressed key
				REG1L = ASCII code of key pressed
				REG1H = Scan code of key pressed
			*/
			WMem(CX80_SYSTEM_REG1L, CX80_GetKey());
			WMem(CX80_SYSTEM_REG1H, KBScancode);
			
			KBScancode = 0;
			break;
			
		case				0x24:
			/*
				Wait for a keypress event
				REG1L = ASCII code of key pressed
				REG1H = Scan code of key pressed
			*/
			tmp = 0;
			
			while (tmp == 0)
				tmp = CX80_GetKey();
				
			WMem(CX80_SYSTEM_REG1L, tmp);
			WMem(CX80_SYSTEM_REG1H, KBScancode);
			
			KBScancode = 0;
			break;
			
		case				0x25:
			/*
				Milliseconds delay
				REG2 = delay
			*/
			reg1 = RMem16(CX80_SYSTEM_REG2L);
			
			sys_delay_ms(reg1);
			break;
			
		case				0x26:
			/*
				Get input string from user
				REG1L = length of input buffer into ram (max 128 chars)
				REG2 = Pointer to input buffer
			*/
			tmp = RMem(CX80_SYSTEM_REG1L);
			reg1 = RMem16(CX80_SYSTEM_REG2L);
			
			if (tmp > (Z80STRING_LEN - 1))
				tmp = Z80STRING_LEN - 1;
			
			CX80_GetLine(Z80String, tmp);			
			
			for (v = 0; v < tmp; v++)
				WMem(reg1++, Z80String[v]);
				
			break;
			
		case				0x27:
			/*
				Play sound via piezo speaker.
				REG1 = sound duration
				REG2 = sound frequency
			*/
			reg1 = RMem16(CX80_SYSTEM_REG1L);
			reg2 = RMem16(CX80_SYSTEM_REG2L);
			
			
			CX80_PlaySound((float)reg1, (float)reg2);
			break;
			
		case				0x28:
			/*
				Write On Expansion Port
				REG1L = value to write
				REG1H = Port address (0..3)
				REG2 = wait time
			*/
			tmp = RMem(CX80_SYSTEM_REG1L);
			v = RMem(CX80_SYSTEM_REG1H);
			reg1 = RMem16(CX80_SYSTEM_REG2L);
			
			SRam_Deselect();
			
			IOREQ_PORT_DDR = 0xFF;
			IOREQ_PORT &= ~(1 << IOREQ);	//Active Low
			
			Setup_CX80_Data_Bus_Output();
			
			DATA_BUS_PORT = tmp;
			ADDRESS_BUS_PORT = v;
			
			SRam_WE_Low();
			
			if (reg1)
				sys_delay_ms(reg1);
				
			IOREQ_PORT |= (1 << IOREQ);
			
			SRam_WE_High();
			
			DATA_BUS_PORT = 0x00;
			ADDRESS_BUS_PORT = 0x00;
			break;
			
		case				0x29:
			/*
				Read On Expansion Port
				REG1L = Read value
				REG1H = Port address (0..3)
				REG2 = wait time
			*/
			v = RMem(CX80_SYSTEM_REG1H);
			reg1 = RMem16(CX80_SYSTEM_REG2L);
			
			SRam_Deselect();
			
			IOREQ_PORT_DDR = 0xFF;
			IOREQ_PORT &= ~(1 << IOREQ);	//Active Low
			
			Setup_CX80_Data_Bus_Input();
			
			ADDRESS_BUS_PORT = v;
			
			SRam_WE_High();
			
			if (reg1)
				sys_delay_ms(reg1);
				
			tmp = DATA_BUS_PORT_READ;
				
			WMem(CX80_SYSTEM_REG1L, tmp);
			
			IOREQ_PORT |= (1 << IOREQ);
			ADDRESS_BUS_PORT = 0x00;
			break;
			
		case				0x2A:
			/*
				Get string from file
				REG1L = File ID				
				REG2 = Pointer to string buffer in ram
				REG3 = Number of bytes to read
				
				Return value REG1H : 0x00 = read ok									 
									 0xAA = Error while reading
									 0xEF = Wrong file slot
									 0xFF = End of File
			*/
			tmp = RMem(CX80_SYSTEM_REG1L);
			
			if (tmp > 2)
				tmp = 2;
				
			if (FileSlot[tmp] == 0)
			{
				WMem(CX80_SYSTEM_REG1H, 0xEF);
				
				break;
			}
			
			reg1 = RMem16(CX80_SYSTEM_REG2L);
			reg2 = RMem16(CX80_SYSTEM_REG3L);
			
			while (reg2-- > 1)
			{
				rc = f_read(&Files[tmp], &v, 1, &br);
				
				if (rc || !br)
					break;
					
				WMem(reg1, v);
				
				reg1++;
				
				if (v == '\n')
					break;
			}
			
			WMem(reg1, 0);
			
			if (rc)
			{
				WMem(CX80_SYSTEM_REG1H, 0xAA);
				
				break;
			}
			
			if (!br)
			{
				WMem(CX80_SYSTEM_REG1H, 0xFF);				
				
				break;
			}
			
			WMem(CX80_SYSTEM_REG1H, 0x00);
			break;
			
		case				0x2B:
			/*
				Get OS CommandLine (0 terminated string)
				REG2 = Pointer to buffer in ram
			*/
			txt = OsCmdLine;
			reg2 = RMem16(CX80_SYSTEM_REG2L);
			
			while (*txt)
			{
				WMem(reg2, *txt);
				
				*txt++;
				reg2++;
			}
			break;
			
		case				0x2C:
			/*
				Get OS Command line parameters
				REG2 = Pointer to first parameter buffer in ram
				REG3 = Pointer to second parameter buffer in ram
			*/
			reg1 = RMem16(CX80_SYSTEM_REG2L);
			reg2 = RMem16(CX80_SYSTEM_REG3L);
			
			for (v = 0; v < 20; v++)
			{
				WMem(reg1, argv2[v]);
				WMem(reg2, argv3[v]);
				
				reg1++;
				reg2++;
			}
			break;
			
		case				0xFF:
			/*
				Stop user program and return to OS
				REG1L = Return value to OS
			*/
			CPU_Stop_Run = 1;
			ReturnValueToOs = RMem(CX80_SYSTEM_REG1L);
			break;
	}
}