/*
 * CX80_OS.c
 *
 * Created: 26/04/2013 12:20:13
 *  Author: Calogiuri Enzo Antonio
 */ 

#include "CX80.h"
#include "CX80_Keyboard.h"
#include "CX80_Ff.h"

byte ReturnValueToOs = 0;
char OsCmdLine[38];
char argv1[10], argv2[20], argv3[20];

////////////////////////////////////////////////////////////////////////////////////////////////////////

/*From user string, obtain the arguments typed*/
static void Build_Arguments_Array(const char *txt)
{
	char *dest[] = {argv1, argv2, argv3};
	char CmdCnt = 0;
	
	memset(argv1, 0, 10);
	memset(argv2, 0, 20);
	memset(argv3, 0, 20);	
	
	while (*txt)
	{
		if (*txt == ' ')
		{
			CmdCnt++;			
			*txt++;
			
			continue;
		}
		
		*dest[CmdCnt] = toupper(*txt);
		*dest[CmdCnt]++;
		
		*txt++;
	}
}

/*Execute the "DIR" command*/
static void CX80_Do_Dir_Command(void)
{
	DIR dir;
	FILINFO fno;
	FRESULT rc;
	char Cnt = 0;
	unsigned char ch;
	char buff[2] = {0};
	
	rc = f_opendir(&dir, (TCHAR *)buff);
	
	if (rc)
	{
		CX80_Do_PrintString("Error!\r");
		return;
	}	
		
	for (;;)
	{
		rc = f_readdir(&dir, &fno);
		
		if (rc || !fno.fname[0])
			break;
			
		if (Cnt == 24)
		{
			CX80_Do_PrintString("More (Y/N)?");
			
			ch = 0;
			
			for (;;)
			{
				ch = (char)CX80_GetKey();
				
				if (ch != 0)
					break;
			}
			
			CX80_Do_PrintString("\r");
			
			
			if ((ch != 'y') && (ch != 'Y'))
				break;
				
			Cnt = 0;
		}
		
		if (fno.fattrib & AM_DIR)
			CX80_Do_PrintString("   <dir>  ");
			
		CX80_Do_PrintString(fno.fname);
		CX80_Do_PrintString("\r");
		
		Cnt++;
	}
}

/*Execute the "COPY" command*/
static char CX80_Do_CopyFile_Command(char *OldN, char *NewN)
{
	FIL o, n;
	byte r, ret;
	uInt br;
	char buff[64];
	
	ret = 0;
	
	r = f_open(&o, OldN, FA_READ);
	
	if (r)
		ret = 1;
		
	r = f_open(&n, NewN, FA_WRITE | FA_CREATE_ALWAYS);
	
	if (r)
		ret = 1;
		
	if (ret == 0)
		for (;;)
		{
			r = f_read(&o, &buff, 64, &br);
		
			if (!br)
			{
				ret = 0;
			
				break;
			}		
			
			if (!r)
			{
				r = f_write(&n, &buff, br, &br);
			
				if (r)
				{
					ret = 1;
					
					break;
				}				
			}
			else
			{
				ret = 1;
				
				break;
			}			
		}
	
	f_close(&o);
	f_close(&n);
	
	return ret;
}

/*Execute the "RUN" command*/
static char CX80_Do_Run_Command(char *cmd)
{
	FIL prg;
	byte r, ret = 0;
	byte tmpbuf[64];
	uInt br;
	word StartAddr = CX80_CPU_STARTRAM;
	
	memset(tmpbuf, 0, 64);
	
	r = f_open(&prg, cmd, FA_READ);
	
	if (r)
	{
		strcat(tmpbuf, "/CMD/");
		strcat(tmpbuf, cmd);
		
		r = f_open(&prg, (TCHAR *)tmpbuf, FA_READ);
		
		if (r)
			ret = 1;
	}
	
	if (ret == 0)
		for (;;)
		{
			r = f_read(&prg, &tmpbuf, 64, &br);
			
			if (!br)
			{
				ret = 0;
			
				break;
			}
			
			if (r)
			{
				ret = 1;
					
				break;
			}
			
			for (r = 0; r < br; r++)
			{
				WMem(StartAddr, tmpbuf[r]);
			
				StartAddr++;
			}
		}
		
	f_close(&prg);
	
	return ret;
}

/*Execute the "DREG" command*/
static void CX80_Do_DReg_Command(void)
{
	char buf[10];
	byte tmp = 0;
	
	CX80_Do_PrintString("\r");
	
	tmp += (fS) ? 1 : 0;  tmp<<=1;
	tmp += (fZ) ? 1 : 0;  tmp<<=1;
	tmp += (fU5)? 1 : 0;  tmp<<=1;
	tmp += (fH) ? 1 : 0;  tmp<<=1;
	tmp += (fU3)? 1 : 0;  tmp<<=1;
	tmp += (fP) ? 1 : 0;  tmp<<=1;
	tmp += (fN) ? 1 : 0;  tmp<<=1;
	tmp += (fC) ? 1 : 0;
	
	utoa((reg[A] << 8) + tmp, buf, 16);
	
	CX80_Do_PrintString("AF = ");
	CX80_Do_PrintString(buf);
	CX80_Do_PrintString("\r");
	
	utoa((reg[B] << 8) + reg[C], buf, 16);
	
	CX80_Do_PrintString("BC = ");
	CX80_Do_PrintString(buf);
	CX80_Do_PrintString("\r");
	
	utoa((reg[D] << 8) + reg[E], buf, 16);
	
	CX80_Do_PrintString("DE = ");
	CX80_Do_PrintString(buf);
	CX80_Do_PrintString("\r");
	
	utoa((reg[H] << 8) + reg[L], buf, 16);
	
	CX80_Do_PrintString("HL = ");
	CX80_Do_PrintString(buf);
	CX80_Do_PrintString("\r");
	
	utoa((reg[IXH] << 8) + reg[IXL], buf, 16);
	
	CX80_Do_PrintString("IX = ");
	CX80_Do_PrintString(buf);
	CX80_Do_PrintString("\r");
	
	utoa((reg[IYH] << 8) + reg[IYL], buf, 16);
	
	CX80_Do_PrintString("IY = ");
	CX80_Do_PrintString(buf);
	CX80_Do_PrintString("\r");
	
	utoa(regSP, buf, 16);
	
	CX80_Do_PrintString("SP = ");
	CX80_Do_PrintString(buf);
	CX80_Do_PrintString("\r");
}

#define ESCAPE_SEQUENCE			0x1B

static void CX80_Disable_Auto_CR(void)
{
	CX80_Usart_SendChar(ESCAPE_SEQUENCE);
	CX80_Usart_SendChar('y');
	CX80_Usart_SendChar('8');
	
	CX80_Usart_SendChar(ESCAPE_SEQUENCE);
	CX80_Usart_SendChar('y');
	CX80_Usart_SendChar('9');
}

static void CX80_Enable_Auto_CR(void)
{
	CX80_Usart_SendChar(ESCAPE_SEQUENCE);
	CX80_Usart_SendChar('x');
	CX80_Usart_SendChar('8');
	
	CX80_Usart_SendChar(ESCAPE_SEQUENCE);
	CX80_Usart_SendChar('x');
	CX80_Usart_SendChar('9');
}

void CX80_Run_OS(void)
{
	char ch;
	
	CX80_Do_PrintString("CX80 OS - v1.2\r");
	
	for (;;)
	{
		memset(OsCmdLine, 0, 38);
		
		CX80_Usart_SendChar('>');		
		
		CX80_GetLine(OsCmdLine, 38);
		
		Build_Arguments_Array(OsCmdLine);
		
		if (strlen(argv1) > 0)
		{
			if (strcmp("CLS", argv1) == 0)
			{
				CX80_Do_ClearScreen();
				
				continue;
			}
			
			if (strcmp("DIR", argv1) == 0)
			{
				CX80_Do_Dir_Command();
				
				continue;
			}
			
			if (strcmp("CD", argv1) == 0)
			{
				if (f_chdir(argv2))
					CX80_Do_PrintString("Error!\r");
				
				continue;
			}
			
			if (strcmp("MKDIR", argv1) == 0)
			{
				if (f_mkdir(argv2))
					CX80_Do_PrintString("Error!\r");
				
				continue;
			}
			
			if (strcmp("DEL", argv1) == 0)
			{
				CX80_Do_PrintString("Confirm (Y/N)?");
				
				ch = 0;
			
				for (;;)
				{
					ch = (char)CX80_GetKey();
				
					if (ch != 0)
						break;
				}
			
				CX80_Do_PrintString("\r");
				
				if ((ch == 'y') || (ch == 'Y'))
				{
					if (f_unlink(argv2))
						CX80_Do_PrintString("Error!\r");
				}
				
				continue;
			}
			
			if (strcmp("REN", argv1) == 0)
			{
				if (f_rename(argv2, argv3))
					CX80_Do_PrintString("Error!\r");
					
				continue;
			}
			
			if (strcmp("COPY", argv1) == 0)
			{
				if (CX80_Do_CopyFile_Command(argv2, argv3))
					CX80_Do_PrintString("Error!\r");
				
				continue;
			}
			
			if (strcmp("RUN", argv1) == 0)
			{
				if (CX80_Do_Run_Command(argv2))
					CX80_Do_PrintString("Error!\r");
				else
				{
					CPU_Stop_Run = 0;
					
					CX80_InitZ80CPU();
					
					CX80_Disable_Auto_CR();
					
					while (CPU_Stop_Run == 0)
						CX80_Z80Run();
						
					CX80_Enable_Auto_CR();
						
					if (ReturnValueToOs)
					{
						CX80_Do_PrintString("\rExit with code:");
						
						utoa(ReturnValueToOs, OsCmdLine, 10);
						
						CX80_Do_PrintString(OsCmdLine);
						CX80_Do_PrintString("\r");
					}
				}					
					
				continue;
			}
			
			if (strcmp("DREG", argv1) == 0)
			{
				CX80_Do_DReg_Command();
				
				continue;
			}
			
			CX80_Do_PrintString("Unknown command.\r");
		}
	}
}