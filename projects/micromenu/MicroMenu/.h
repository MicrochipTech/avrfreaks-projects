/*
	           MICRO-MENU

	     (C) Dean Camera, 2007
	    www.fourwalledcubicle.com
	dean_camera@fourwalledcubicle.com
	
	    Royalty-free for all uses.
	                                  */

#ifndef MENU_H
#define MENU_H

#include <avr/pgmspace.h>

// Typedefs:
typedef void (*FuncPtr)(void);
typedef void (*WriteFuncPtr)(const char*);

typedef struct {
	void       *Next;
	void       *Previous;
	void       *Parent;
	void       *Sibling;
	FuncPtr     SelectFunc;
	FuncPtr     EnterFunc;
	const char  Text[];
} Menu_Item PROGMEM;

// Externs:
extern WriteFuncPtr*    WriteFunc;
extern Menu_Item        Null_Menu;
extern Menu_Item*       CurrMenuItem;

// Defines and Macros:
#define NULL_ENTRY Null_Menu
#define NULL_FUNC  (void*)0
#define NULL_TEXT  0x00

#define PREVIOUS   *((Menu_Item*)pgm_read_word(&CurrMenuItem->Previous))
#define NEXT       *((Menu_Item*)pgm_read_word(&CurrMenuItem->Next))
#define PARENT     *((Menu_Item*)pgm_read_word(&CurrMenuItem->Parent))
#define SIBLING    *((Menu_Item*)pgm_read_word(&CurrMenuItem->Sibling))
#define ENTERFUNC  *((FuncPtr*)pgm_read_word(&CurrMenuItem->EnterFunc))
#define SELECTFUNC *((FuncPtr*)pgm_read_word(&CurrMenuItem->SelectFunc))

#define MAKE_MENU(Name, Next, Previous, Parent, Sibling, SelectFunc, EnterFunc, Text) \
    extern Menu_Item Next;     \
	extern Menu_Item Previous; \
	extern Menu_Item Parent;   \
	extern Menu_Item Sibling;  \
	Menu_Item Name = {(void*)&Next, (void*)&Previous, (void*)&Parent, (void*)&Sibling, (FuncPtr)SelectFunc, (FuncPtr)EnterFunc, { Text }}

#define SET_MENU_WRITE_FUNC(x) \
	WriteFunc = (WriteFuncPtr*)&x;

#define SET_MENU(x) \
	MenuChange((Menu_Item*)&x);
	
#define GO_MENU_FUNC(x)  \
	MenuFunc((FuncPtr*)&x)
	
#define EXTERN_MENU(Name) \
    extern Menu_Item Name;

// Prototypes:
void MenuChange(Menu_Item* NewMenu);
void MenuFunc(FuncPtr* Function);

#endif
