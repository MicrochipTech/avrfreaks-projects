/*This file is prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief 3D Engine file.
 *
 * - Compiler:           IAR EWAVR32 and GNU GCC for AVR32
 * - Supported devices:  All AVR32 devices can be used.
 * - AppNote:
 *
 * \author               Atmel Corporation: http://www.atmel.com \n
 *                       Support and FAQ: http://support.atmel.no/
 *
 ******************************************************************************/

/* Copyright (C) 2006-2008, Atmel Corporation All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. The name of ATMEL may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE EXPRESSLY AND
 * SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

//_____  I N C L U D E S ___________________________________________________
#include "3dengine.h"

//_____ M A C R O S ________________________________________________________

//_____ D E F I N I T I O N S ______________________________________________
#define FIXEDBITPRECSCALE (256*256)
#define MAXPOSVAL         (FIXEDBITPRECSCALE - 1)
#define VECPREC           15
#define COSTABLENBELTS    (sizeof(COSTABLE_STEP_0_050000_QUADPOS)/sizeof(int))

//_____ D E C L A R A T I O N S ____________________________________________
const int ROTX_001STEPF_FFIX[9]={0x00000100, 0x00000000, 0x00000000, 0x00000000, 0x000000FC, 0xFFFFFFD8, 0x00000000, 0x00000028, 0x000000FC};
const int ROTY_001STEPF_FFIX[9]={0x000000FC, 0x00000000, 0x00000028, 0x00000000, 0x00000100, 0x00000000, 0xFFFFFFD8, 0x00000000, 0x000000FC};
const int ROTZ_001STEPF_FFIX[9]={0x00000100, 0x00000000, 0x00000000, 0x00000000, 0x00000100, 0x00000000, 0x00000000, 0x00000000, 0x00000100};
const int COSTABLE_STEP_0_050000_QUADPOS[] = {0x0000FFFF, 0x0000FFE9, 0x0000FFA6, 0x0000FF35, 0x0000FE98, 0x0000FDCF, 0x0000FCD9, 0x0000FBB6, 0x0000FA67, 0x0000F8ED, 0x0000F746, 0x0000F575, 0x0000F378, 0x0000F150, 0x0000EEFF, 0x0000EC83, 0x0000E9DE, 0x0000E70F, 0x0000E419, 0x0000E0FA, 0x0000DDB3, 0x0000DA46, 0x0000D6B3, 0x0000D2F9, 0x0000CF1B, 0x0000CB19, 0x0000C6F3, 0x0000C2A9, 0x0000BE3E, 0x0000B9B2, 0x0000B504, 0x0000B038, 0x0000AB4C, 0x0000A642, 0x0000A11B, 0x00009BD7, 0x00009679, 0x00009100, 0x00008B6D, 0x000085C2, 0x00007FFF, 0x00007A27, 0x00007438, 0x00006E35, 0x0000681F, 0x000061F7, 0x00005BBE, 0x00005574, 0x00004F1B, 0x000048B5, 0x00004241, 0x00003BC3, 0x00003539, 0x00002EA6, 0x0000280C, 0x0000216A, 0x00001AC2, 0x00001415, 0x00000D65, 0x000006B3 };
const int SINTABLE_STEP_0_050000_QUADPOS[] = {0x00000000, 0x000006B3, 0x00000D65, 0x00001415, 0x00001AC2, 0x0000216A, 0x0000280C, 0x00002EA6, 0x00003539, 0x00003BC3, 0x00004241, 0x000048B5, 0x00004F1B, 0x00005574, 0x00005BBE, 0x000061F7, 0x0000681F, 0x00006E35, 0x00007438, 0x00007A27, 0x00008000, 0x000085C2, 0x00008B6D, 0x00009100, 0x00009679, 0x00009BD7, 0x0000A11B, 0x0000A642, 0x0000AB4C, 0x0000B038, 0x0000B504, 0x0000B9B2, 0x0000BE3E, 0x0000C2A9, 0x0000C6F3, 0x0000CB19, 0x0000CF1B, 0x0000D2F9, 0x0000D6B3, 0x0000DA46, 0x0000DDB3, 0x0000E0FA, 0x0000E419, 0x0000E70F, 0x0000E9DE, 0x0000EC83, 0x0000EEFF, 0x0000F150, 0x0000F378, 0x0000F575, 0x0000F746, 0x0000F8ED, 0x0000FA67, 0x0000FBB6, 0x0000FCD9, 0x0000FDCF, 0x0000FE98, 0x0000FF35, 0x0000FFA6, 0x0000FFE9 };

int cprodf_FFIX(int *m1,int l1,int c1,int *m2,int l2,int c2,int *m3);
int cprodf_FFIX(int *m1,int l1,int c1,int *m2,int l2,int c2,int *m3)
{
 int i,j,k;
 int r;

 for(i=0;i<l1;i++) {
  for(j=0;j<c2;j++){
   r=0;
   for(k=0;k<c1;k++){   
    r=r+((m1[i*c1+k]*m2[k*c2+j]) >> 8);
   }
   m3[i*c2+j]=r;
  }
 }
 return(1);
}

void transforme_C8_FFIX(OBJ3D_C8_FFIX *ob,OBJ3D_C8_FFIX *ob2,int *tsf) {
 int i,k;
 int res[3],tmp[3];
 ob2->zoom=ob->zoom;
   for(i=0;i<ob->nbpoints;i++) {
  
    for(k=0;k<3;k++){
     tmp[k]=((int)(ob->ob[k*ob->nbpoints+i])<<8);
    }
    cprodf_FFIX(tsf,3,3,tmp,3,1,res);
  
    for(k=0;k<3;k++) {
     ob2->ob[k*ob->nbpoints+i]=(signed char)(res[k]>>8);
    }
  
  }
}

void mrot_C8_FULL_FFIX(int *m,int teta,int a,int b,int c)
{
    int costeta, sinteta;
        
     if((teta<COSTABLENBELTS)) {
      costeta= COSTABLE_STEP_0_050000_QUADPOS[teta];
      sinteta= SINTABLE_STEP_0_050000_QUADPOS[teta];
     } else if(teta==COSTABLENBELTS) {
       costeta=0;
       sinteta=FIXEDBITPRECSCALE-1;
     } else if((teta<2*COSTABLENBELTS)) {
      costeta=-COSTABLE_STEP_0_050000_QUADPOS[COSTABLENBELTS-(teta-COSTABLENBELTS)];
      sinteta= SINTABLE_STEP_0_050000_QUADPOS[COSTABLENBELTS-(teta-COSTABLENBELTS)];
     } else if(teta==2*COSTABLENBELTS) {
       costeta=-(FIXEDBITPRECSCALE-1);
       sinteta=0;
     } else if((teta<3*COSTABLENBELTS)) {
       costeta=-COSTABLE_STEP_0_050000_QUADPOS[teta-2*COSTABLENBELTS];
       sinteta=-SINTABLE_STEP_0_050000_QUADPOS[teta-2*COSTABLENBELTS];
     } else if((teta == 3*COSTABLENBELTS)) {
       costeta=0;
       sinteta=-(FIXEDBITPRECSCALE-1);
     } else {
      costeta= COSTABLE_STEP_0_050000_QUADPOS[COSTABLENBELTS-(teta-3*COSTABLENBELTS)];
      sinteta=-SINTABLE_STEP_0_050000_QUADPOS[COSTABLENBELTS-(teta-3*COSTABLENBELTS)];
     }
     
     int term1;
    
     term1=(((((MAXPOSVAL-costeta)*((a*a)>>VECPREC)))))>>VECPREC;
     m[0]=(int)(((           term1 + costeta)                         ))>>8;
    
     term1=(((((MAXPOSVAL-costeta)*((a*b)>>VECPREC)))));
     m[1]=(int)((            term1 -(c*sinteta)   ))>>VECPREC>>8;
    
     term1=(((((MAXPOSVAL-costeta)*((a*c)>>VECPREC)))));
     m[2]=(int)(((           term1 +(b*sinteta))  ))>>VECPREC>>8;
    
     term1=(((((MAXPOSVAL-costeta)*((a*b)>>VECPREC)))));
     m[3]=(int)(((           term1 +(c*sinteta))  ))>>VECPREC>>8;
    
     term1=(((((MAXPOSVAL-costeta)*((b*b)>>VECPREC)))))>>VECPREC;
     m[4]=(int)(((           term1 +costeta)	          ))>>8;
    
     term1=(((((MAXPOSVAL-costeta)*((b*c)>>VECPREC)))));
     m[5]=(int)(((           term1 -(a*sinteta))  ))>>VECPREC>>8;
    
     term1=(((((MAXPOSVAL-costeta)*((a*c)>>VECPREC)))));
     m[6]=(int)(((           term1 -(b*sinteta))  ))>>VECPREC>>8;
    
     term1=(((((MAXPOSVAL-costeta)*((b*c)>>VECPREC)))));
     m[7]=(int)(((           term1 +(a*sinteta))  ))>>VECPREC>>8;
    
     term1=(((((MAXPOSVAL-costeta)*((c*c)>>VECPREC)))))>>VECPREC;
     m[8]=(int)((            term1 +costeta	          ))>>8;
}


void init_mtr_trsf(int *m) 
{
  int tmp[9], i;
  cprodf_FFIX((int *)ROTX_001STEPF_FFIX, 3, 3, (int *)ROTY_001STEPF_FFIX, 3, 3, (int *)tmp);
  cprodf_FFIX((int *)tmp, 3, 3, (int *)ROTZ_001STEPF_FFIX, 3, 3, (int *)m);
  for(i=0; i<9; i++) {
  }
}
