#ifndef _AVRSH_REGISTER_H_
#define _AVRSH_REGISTER_H_
////////////////////////////////////////////////////////////////////////////
// Name: register.h
// Author: B. Gian James <gian@BattleDroids.net>
// Description: This file declares the scripting components that allow 
//              the user to check registers in real-time.
//
// $Id: register.h,v 1.3 2009/08/30 17:44:28 gian Exp $
////////////////////////////////////////////////////////////////////////////
typedef struct _rv
{
	volatile uint8 *	reg;
	const char *			    name;
	
} RegisterVars;

//extern RegisterVars	Registers[];

class Register
{
public:
	Register();
	static uint8 *	GetRegisterByName(const char *);
	static void		PrintRegisters();
	static void		WriteRegister(uint8 * reg);

private:
	static RegisterVars Registers[];
	
	static char *	getRegNameFromPgmSpace(uint8);

};







#endif	/* _AVRSH_SCRIPT_H_ */