#ifndef _AVRSH_SHELL_H_
#define _AVRSH_SHELL_H_
////////////////////////////////////////////////////////////////////////////
// Name: shell.h
// Author: B. Gian James <gian@BattleDroids.net>
// Description: The shell file for the AVR Shell. Written to be used with
//              the AVR Telnet Gateway.  Contains the shell functionality.
//
// $Id: shell.h,v 1.5 2009/09/02 12:55:17 gian Exp $
////////////////////////////////////////////////////////////////////////////
#include "common.h"
#include "uart.h"
#include "script.h"

#define EE_ROOT 0x01
#define MAX_PROMPTSZ	30
#define DEFAULT_PROMPT	"root@ATmega328p> "

class AVRShell
{
public:
	AVRShell(UART * uart) { out = uart; timeSec = 0; timect=0; bIsAuthorized = 0; StringCopy(_prompt,DEFAULT_PROMPT); bIsFSOn = 0; }
	
	uint8	ExecCmd(char *, char **);
	uint8	Process(const char * b);
	inline void AddSec() volatile { ++timeSec; }
	inline void SetScriptEngine(ScriptEngine * se) { script = se; }
	inline char * Prompt(char * p = NULL) { if (p) StringCopy(_prompt,p); return _prompt; }
	
	// Commands
	void	cmdHelp();
	void	cmdFCPU();
	void	cmdSet(char **);
	void	cmdAuth(char **);
	void	cmdClock(char **);
	void	cmdTimer(char **);
	void	cmdRead(char **);
	void	cmdPasswd(char **);
	void	cmdWrite(char **);
	void	cmdPowerUp(char **);
	void	cmdPowerDown(char **);
	void	cmdEna(char **);
	void	cmdDir(char **);
	
	
	volatile uint8 timect;
	
private:
	ScriptEngine *	script;
	UART *			out;
	char 			_prompt[MAX_PROMPTSZ];
	volatile uint8	timeSec;
	uint8			bIsAuthorized;
	uint8			bIsFSOn;

};
#endif	/* _AVRSH_SHELL_H_ */