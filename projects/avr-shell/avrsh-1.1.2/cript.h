#ifndef _AVRSH_SCRIPT_H_
#define _AVRSH_SCRIPT_H_
////////////////////////////////////////////////////////////////////////////////
/// Name: script.h
/// Author: B. Gian James  <gian@BattleDroids.net>
/// Description: Scripting infrastructure.
///
/// $Id: script.h,v 1.2 2009/08/29 21:04:21 gian Exp $
////////////////////////////////////////////////////////////////////////////////
#include "common.h"
#include "uart.h"

#define REDIRECT		">"
#define MAX_VARIABLES	20

typedef struct
{
	union {
		struct {
			uint8	valh;
			uint8	vall;
		} val8;
		uint16	val16;
	} val;
	char 	name[8];
	uint8	slot;
} NamedVar;

extern NamedVar	Variables[];

class ScriptEngine
{
public:
	
	ScriptEngine();
	
	void	Init();	
	uint8	Interpret(char *, char **);
	uint8	AddVariable(NamedVar & v);
	
	void	parseEcho(char **);
	void	parseVar(char *, char **);
	void	parsePrint(char **);
	void	parseRegister(char *, char **);
	
	uint8 Count() { return _numVars; }
	
private:
	uint8	_numVars;
	struct _tasklist_ 
	{
		PortPin *	pin;
		uint16		ms;
		NamedVar *	variable;
		_tasklist_ * next;
	
	} TaskList;
};



#endif	/* _AVRSH_SCRIPT_H_ */