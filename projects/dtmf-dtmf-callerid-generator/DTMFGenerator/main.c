//******************************************************
// behzad khazama 2007/06/04 SABZEVAR-IRAN
// www.khazama.com
//
// to use this project as DTMF caller id generator
// connect tel lines as bellow
//
//  use 2 series capacitor as bellow      
//  TEL(+) --||--*--||-- PORTB.1 (OC1A)
//          +  -   -  +
//       (1uf 50v) (1uf 50v)
//
//  TEL(-) -- connect to GND    
//
//******************************************************


//******************************************************
// DTMF caller id signals generate before first ring
// it starts with DTMF signal D (A or B may use too)
// and terminate with DTMF signal C
// between D and C 'caller number' will generate 
//
// D(number...)C ------- first ring
//******************************************************

#include <mega8.h>
#include <string.h>
#include "dtmf.h"

void main(void){    
   char num[15];     
        dtmf_init();
        strcpyf(num,"D09155714862C");
        Send_DTMF(num);  
        while(1);
}