/*This file is prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief Display 3D library.
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
#ifndef RASTER_H
#define RASTER_H

/*! \brief Update Face function
 */
extern void update_faces(void);
/*! \brief Draw Buffer Image for Bitmap Data into an Output Buffer
 *  \param *myobj   Input Buffer
 *  \param color    Background Color Value
 *  \param *RASTER  Output Buffer
 */
extern void draw_image_raster_bmp (OBJ3D_C8_FFIX *myobj, int color, char* RASTER);
/*! \brief Draw Buffer Image for Linear Data into an Output Buffer
 *  \param *myobj   Input Buffer
 *  \param color    a coordinate (x value)
 *  \param *RASTER  Output Buffer
 */
extern void draw_image_raster(OBJ3D_C8_FFIX *myobj, int color, char* RASTER);
/*! \brief Display Buffer Image on LCD
 *  \param *RASTER     Input Buffer
 */
extern void SEND_RASTER_BUF(char* RASTER);
/*! \brief Reset Display Buffer
 *  \param *RASTER      Input Buffer
 */
extern void reset_Raster(char* RASTER);

#endif //RASTER_H
