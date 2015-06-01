/*This file is prepared for Doxygen automatic documentation generation.*/
/*! \file ******************************************************************
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
 ***************************************************************************/

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
#ifndef _3DENGINE_H
#define _3DENGINE_H

//_____ D E F I N I T I O N S ______________________________________________
#include <math.h>

//! 3D Object Definition
typedef struct {
   float zoom;
   signed char *ob;
   unsigned char *lien;
   double* zbuffer;
   unsigned int nbpoints;
   unsigned int nbliens;
} OBJ3D_C8_FFIX;


//! Rotation X Fixe Point
extern const int ROTX_001STEPF_FFIX[9];

//! Rotation Y Fixe Point
extern const int ROTY_001STEPF_FFIX[9];

//! Rotation Z Fixe Point
extern const int ROTZ_001STEPF_FFIX[9];

//! Cosinus Table Fixe Point
extern const int COSTABLE_STEP_0_050000_QUADPOS[];

//! Sinus Table Fixe Point
extern const int SINTABLE_STEP_0_050000_QUADPOS[];

/*! \brief 3D Matrix Transformation Fixe Point
 *  \param *m  Matrix Pointer
 *  \param teta Teta Value   
 *  \param a    a coordinate (x value)
 *  \param b    b coordinate (y value)
 *  \param c    x coordinate (z value)
 */
extern void mrot_C8_FULL_FFIX(int *m,int teta,int a,int b,int c);

/*! \brief 3D Matrix Transformation Fixe Point
 *  \param *ob    Input Buffer
 *  \param *ob2   Output Buffer
 *  \param *tsf   Matrix Pointer
 */
extern void transforme_C8_FFIX(OBJ3D_C8_FFIX *ob,OBJ3D_C8_FFIX *ob2,int *tsf);

/*! \brief Initialize 3D Matrix 
 *  \param *m    Matrix Pointer
 */
extern void init_mtr_trsf(int *m) ;

#endif //_3DENGINE_H

