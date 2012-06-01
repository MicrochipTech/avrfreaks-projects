/*
/		MENU_UTILS.C
		May 27, 2012
		
		Author: Visakhan C
*/


#include "menu_utils.h"
#include "ext_keys.h"
#include "lcd.h"

/* runs the menu one time, so call in an endless loop if Main menu */
void DoMenu(PGM_P pTitle, MenuItem* pMenu)
{
	uint8_t c,cur=0;
	//uint16_t fileId;
	
	
	lcdClear();
	
	lcdGotoXY(0,4);
	lcdString_P(pTitle);
	lcdGotoXY(1,1);
	lcdString(pMenu[cur++].name);
	lcdGotoXY(1,6);
	lcdString(pMenu[cur++].name);
	lcdGotoXY(1,12);
	lcdString(pMenu[cur].name);
	lcdGotoXY(1,0);
	lcdCursorBlink();
	
	cur = 0;
	
	do {
	
		c = KbGetChar();
		switch(c) {
		
			case L_ARROW	:	
				
				if(cur == 2) {
				
					lcdGotoXY(1,5); cur--;
				}
				else if(cur == 1) {
					
					lcdGotoXY(1,0); cur--;
				}
				break;
			
			case R_ARROW	:
				
				if(cur == 0) {
					
					lcdGotoXY(1,5); cur++;
				}
				else if(cur == 1) {
				
					lcdGotoXY(1,11); cur++;
				}
				break;
			
			case '\r'	:
			
				pMenu[cur].pFunc(); break;
				
		}		
					
				
				
					
	} while(c != '\r');
							
	
}

/* prompt for choosing between YES or NO using arrow keys and ENTER;
	the question string 'que' is displayed on top */ 
RESPONSE PromptYesNo(PGM_P que)
{

	uint8_t c;
	RESPONSE ans=YES;
	
	lcdClear();
	lcdString_P(que);
	lcdGotoXY(1,3);
	lcdString("YES");
	lcdGotoXY(1,11);
	lcdString("NO");
	lcdGotoXY(1,2);
	
	while(1)  {
	
		c = KbGetChar();
		
		switch(c) {
			
			case 'y'		:
			case L_ARROW	:	if(ans==NO) {
									
									lcdGotoXY(1,2);
									ans = YES; 
								} break;
			
			case 'n'		:
			case R_ARROW	:	if(ans==YES) {
						
									lcdGotoXY(1,10);
									ans = NO;
								} break;
								
			case ESC		:	return CANCEL;
			
			case '\r'		:	return ans;
			
			default			: 	break;
			
		}
		
	}
	
}

/* fetches a number from keyboard and returns it through pointer
	return an error code */
ERRORCODE PromptNumber(PGM_P que,uint16_t* pNum)
{

	uint8_t c,iter=0;
	uint8_t n[5]={};
	uint16_t num=0,  place=1;
	
	lcdClear();
	lcdString_P(que);
	lcdGotoXY(1,0);
	
	while(1) {
	
		c = KbGetChar();
		switch(c) {
		
			case ESC	: return ERR_ABORT;
			
			case '\r'	: while(iter-- > 0) {
			
							num += n[iter]*place;
							place *= 10;
							
						  } 
						  *pNum = num;
						  return SUCCESS; 
			
			default		: if(iter<5) {
				
							if( (c >='0') && (c <='9') ) {
								n[iter] = (c-48);
								lcdChar(c);
							}
							else
								return ERR_INVALID;
						  } break;
		
		}
		
		if(++iter > 5) break; 
			
		
	}
	
	return ERR_OVERFLOW;
}