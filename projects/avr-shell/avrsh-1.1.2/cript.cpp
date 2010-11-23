////////////////////////////////////////////////////////////////////////////////
/// Name: script.cpp
/// Author: B. Gian James  <gian@BattleDroids.net>
/// Description: Scripting infrastructure.
///
/// $Id: script.cpp,v 1.6 2009/09/05 04:06:17 gian Exp $
////////////////////////////////////////////////////////////////////////////////
#include <string.h>
#include <stdlib.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include "script.h"
#include "register.h"

// Prototypes
static NamedVar	 GetVariableByName(const char *);
static NamedVar  GetNextAvailVariable(void);

NamedVar 	Variables[MAX_VARIABLES] EEMEM;

NamedVar  GetVariableByName(const char * n)
{
	NamedVar	var; 
	for (uint8 x = 0; x < MAX_VARIABLES; x++)
	{
		eeprom_read_block((void*)&var,(const void*)&Variables[x], sizeof(NamedVar));
		if (!strcmp(var.name,n))
			return var;
	}
	StringCopy(var.name,"!FOUND");
	return var;
}

NamedVar  GetNextAvailVariable()
{
	NamedVar	var;
	uint8 idx = appGetScriptEngine().Count();
	eeprom_read_block((void*)&var,(const void*)&Variables[idx], sizeof(NamedVar));
	
	return var;
}

/////////////////////////////////////////////////////[ class ScriptEngine
void 
ScriptEngine::parseEcho(char ** args)
{ 
	if (*args[0] == '%') 
	{	// register
		uint8 *	reg = Register::GetRegisterByName(++(args[0]));
		if (reg) {
			WriteRAM(args[0],0);
			Register::WriteRegister(reg);
			WriteLine(PSTR("\r\n"));
		}
	} else if (*args[0] == '$')
	{
		NamedVar v = GetVariableByName(++args[0]);
		if (!strcmp(v.name,"!FOUND")) {
			WriteLine(PSTR("Unknown variable.\r\n"));
			return;
		}
		char buff[25];
		sprintf_P(buff,PSTR("$%8s --> %d\r\n"),v.name,v.val.val16);
		WriteRAM(buff);
	}
}

void 
ScriptEngine::parseRegister(char * c, char ** args)
{	// %REGISTER[bit] = value
	char * nreg = ++c;
	while(*c != '[') ++c;
	*c = '\0';
	uint8 bit = atoi((++c));
	uint8 * reg = Register::GetRegisterByName(nreg);
	if (reg == NULL) {
		WriteLine(PSTR("Unknown register.\r\n"));
		return;
	}
	WriteRAM(nreg,0);
	Register::WriteRegister(reg);
	WriteLine(PSTR(" -->  "),0);
	if (atoi(args[1]))
		BSET(*reg, bit);
	else
		BCLR(*reg,bit);
	Register::WriteRegister(reg);
	WriteLine(PSTR("\r\n"));
}
void 
ScriptEngine::parseVar(char * c, char ** args)
{	// $VARIABLE = value
	NamedVar  var;
	if (_numVars == MAX_VARIABLES) {
		WriteLine(PSTR("Maximum variables in use.\r\n"));
		return;
	}
	if ( var = GetVariableByName((++c)), !strcmp(var.name,"!FOUND")) {
		// new variable
		var = GetNextAvailVariable();
		StringCopy(var.name,c);
		var.val.val16 = atol(args[1]);
		if (!AddVariable(var))
			WriteLine(PSTR("Problem writing variable to EEPROM."));
	}	else {	// reassigning an instantiated variable
		var.val.val16 = atoi(args[1]);
		eeprom_write_block((const void*)&var,(void*)&Variables[var.slot], sizeof(NamedVar));	
	}
}

void
ScriptEngine::parsePrint(char ** args)
{ 
	char buff[25];
	if (!strcmp(*args,"registers"))
		Register::PrintRegisters();
	else if (!strcmp(*args,"variables")) {
		WriteLine(PSTR("User-defined variables:\r\n"),0);
		NamedVar var;
		sprintf_P(buff,PSTR("%-5s %-8s  ->  %-5s\r\n"),"Index","Name","Value");
		WriteRAM(buff,0);
		for (uint8 x = 0; x < MAX_VARIABLES; x++) {
			eeprom_read_block((void*)&var,(const void*)&Variables[x],sizeof(var));
			sprintf_P(buff,PSTR("(%02d): %-8s  ->  %5d\r\n"),(x+1),var.name,var.val.val16);
			WriteRAM(buff,0);
		}
		WriteLine(PSTR("Complete.\r\n"));
	}
	else if (!strcmp(*args,"tasks")) {
		WriteLine(PSTR("Current tasks:\r\n"));
		while(TaskList.next != NULL) {
			sprintf_P(buff,PSTR("Polling %s every %s ms with results going to %s"),
				TaskList.pin->name, TaskList.ms, (TaskList.variable ? TaskList.variable->name : "OUT" ));
			TaskList.next = TaskList.next->next;
		}
	}	
}

uint8
ScriptEngine::Interpret(char * cmd, char ** args)
{
	return 0;
}

void
ScriptEngine::Init()
{
	NamedVar	vars[MAX_VARIABLES];
	eeprom_read_block((void*)&vars,(const void*)&Variables,(sizeof(NamedVar)*MAX_VARIABLES));
	
	for(uint8 x = 0; x < MAX_VARIABLES; x++) 
	{
		StringCopy(vars[x].name,"$FREE$");
		vars[x].val.val16 = 0x0000;
		vars[x].slot = x;
	}
	
	eeprom_write_block((const void*)&vars,(void*)&Variables,(sizeof(NamedVar)*MAX_VARIABLES));
}

uint8
ScriptEngine::AddVariable(NamedVar & var)
{
	if (_numVars == MAX_VARIABLES)
		return 0;
	eeprom_write_block((const void*)&var,(void*)&Variables[_numVars], sizeof(NamedVar));
	++_numVars;
	return (_numVars);
}

ScriptEngine::ScriptEngine()
{
	TaskList.next	= NULL;
	TaskList.variable = NULL;
	TaskList.pin	= NULL;
	_numVars = 0;
}