/*		
/		MENU_UTILS.H
/
/		Author: Visakhan C
*/

#include <avr/pgmspace.h>
#include "ps2_kb.h"
#include "common.h"

						 
typedef struct {

	char name[8];
	void (*pFunc)(void);

} MenuItem;

void DoMenu(PGM_P pTitle, MenuItem* pMenu);
RESPONSE PromptYesNo(PGM_P que);
ERRORCODE PromptNumber(PGM_P que,uint16_t* pNum);