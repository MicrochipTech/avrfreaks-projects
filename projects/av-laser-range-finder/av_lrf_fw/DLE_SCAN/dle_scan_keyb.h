/***C*********************************************************************************************
**
** SRC-FILE     :   dle_scan_keyb.h
**                                        
** PROJECT      :   DLE SCAN
**                                                                
** SRC-VERSION  :   0
**              
** DATE         :   01/20/2011
**
** AUTHOR       :   AV
**
** DESCRIPTION  :   
**                  
** COPYRIGHT    :   Andrejs Vasiljevs (c) 2011
**
****C*E******************************************************************************************/
#ifndef __DLE_SCAN_KEYB_H__
#define __DLE_SCAN_KEYB_H__

#define KEYB_SCAN_PERIOD (60000/30)   // 30usec ticks
void keyb_scan();
void init_keyb();


#endif // __DLE_SCAN_KEYB_H__
