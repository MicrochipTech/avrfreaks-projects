/***********************************************************************
Content: Library for Hitachi 44780 based 1 or 2 line Text LCD displays.
Created: 13.10.2003
Last modified: 17.02.2004
Copyrights: Free to use, free to change, free to delete :-)
Compiler: ImageCraft AVR
Written by: Knut Baardsen @ Baardsen Software, Norway
Updates: http://www.baso.no
------------------------------------------------------------------------
This software is provided "as is"; Without warranties either express or
implied, including any warranty regarding merchantability, fitness for 
a particular purpose or noninfringement. 
In no event shall Baardsen Software or its suppliers be liable for any 
special,indirect,incidential or concequential damages resulting from 
the use or inability to use this software.
***********************************************************************/



/***********************************************************************
Includes
***********************************************************************/
#include "common.h"



/***********************************************************************
Delay routine
NB! Remember to set CPU_CLOCK in MHz before using this delay function.
***********************************************************************/
void waitms(long milliseconds)
{
  long longloop;
  int shortloop;
  int calc_adj = CPU_CLOCK * 100;
  for (longloop=0; longloop<milliseconds; longloop++)
  {
    for (shortloop=0; shortloop<calc_adj; shortloop++) NOP();
  }
}


/***********************************************************************
End of code listing
***********************************************************************/