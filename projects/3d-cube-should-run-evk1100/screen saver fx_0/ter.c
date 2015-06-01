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
#include <avr32/io.h>
#include <string.h>
#include "board.h"
#include "gpio.h"
#include "lcd_nokia.h"
#include "CONF/conf_3Dcube.h"
#include "3dengine.h"
#include "raster.h"

//_____ M A C R O S ________________________________________________________

//_____ D E F I N I T I O N S ______________________________________________

//_____ D E C L A R A T I O N S ____________________________________________
int Face_to_prints[3];                    //!< Printed Face Declaration
int Number_Face_to_print = 0 ;            //!< Number of Face to Display
extern unsigned char mycolor;             //!< Current mycolor variable
extern OBJ3D_C8_FFIX tmpcenters;          //!< Barycenter current pointer



void update_faces(void);
void update_faces()
{
  // Here we considere on z coordinate of tempcenters object.
  #define zbuffer ((signed char *)&tmpcenters.ob[FACE_NUMBER*2 + 0])

  int Faces[6] = {0, 1, 2, 3, 4, 5};

  int swap, temp, i;
  do
  {
    swap = 0;
    for (i = 0; i < 5; i++)
      // If Next Buffer to display has an z value higher than current one, swap current z with new z face value and stop
      if (zbuffer[Faces[i]] > zbuffer[Faces[i+1]])
      {
        temp = Faces[i];
        Faces[i] = Faces[i+1];
        Faces[i+1] = temp;
        swap = 1;
      }
  }
  while (swap);

  Number_Face_to_print = 0;
  // Display the 3 first one faces
  for (i = 0; i < 3; i++)
  {
    Face_to_prints[i] = Faces[5-i];
    if (zbuffer[Face_to_prints[i]]) Number_Face_to_print++;
    else break;
  }
}


void draw_image_raster_bmp (OBJ3D_C8_FFIX *myobj, int color, char* RASTER)
{
  int i;
  if(myobj->lien == NULL) {
    for(i=0; i<myobj->nbliens;i++) {
      RASTER[(SCR_WIDE/2-myobj->ob[i]) + (SCR_HEIGHT/2-myobj->ob[i+myobj->nbpoints])*SCR_WIDE] = color;
    }
  } else {
    for(i=0; i<myobj->nbliens;i++) {
      RASTER[(SCR_WIDE/2-myobj->ob[i]) + (SCR_HEIGHT/2-myobj->ob[i+myobj->nbpoints])*SCR_WIDE] = myobj->lien[i];
    }
  }
}

void my_put_line_raster(int x0, int y0, int x1, int y1, unsigned char color, char* RASTER);
void my_put_line_raster(int x0, int y0, int x1, int y1, unsigned char color, char* RASTER)
{

        int dy = y1 - y0;
        int dx = x1 - x0;
        int stepx, stepy;

        if (dy < 0) { dy = -dy;  stepy = -1; } else { stepy = 1; }
        if (dx < 0) { dx = -dx;  stepx = -1; } else { stepx = 1; }
        dy <<= 1;
        dx <<= 1;
      if( (x0)+(y0)*SCR_HEIGHT < 128*128)
	RASTER[(x0)+(y0)*SCR_HEIGHT]=color;
        if (dx > dy) {
            int fraction = dy - (dx >> 1);
            while (x0 != x1) {
                if (fraction >= 0) {
                    y0 += stepy;
                    fraction -= dx;
                }
                x0 += stepx;
                fraction += dy;
	if( (x0)+(y0)*SCR_HEIGHT < 128*128)
          RASTER[(x0)+(y0)*SCR_HEIGHT]=color;
            }
        } else {
            int fraction = dx - (dy >> 1);
            while (y0 != y1) {
                if (fraction >= 0) {
                    x0 += stepx;
                    fraction -= dy;
                }
                y0 += stepy;
                fraction += dx;
       if( (x0)+(y0)*SCR_HEIGHT < 128*128 )
	RASTER[(x0)+(y0)*SCR_HEIGHT]=color;
            }
        }
 }
void draw_image_raster(OBJ3D_C8_FFIX *myobj, int color, char* RASTER) {
  int i;
  int x1, y1, x2, y2;
  for(i=0; i<myobj->nbliens;i++) {
    x1=(unsigned char)(SCR_WIDE/2-((int)(myobj->ob[myobj->lien[2*i]])));
    y1=(unsigned char)(SCR_HEIGHT/2-((int)(myobj->ob[myobj->lien[2*i]+ myobj->nbpoints])));
    x2=(unsigned char)(SCR_WIDE/2-((int)(myobj->ob[myobj->lien[2*i+1]])));
    y2=(unsigned char)(SCR_HEIGHT/2-((int)(myobj->ob[myobj->lien[2*i+1]+myobj->nbpoints])));
    my_put_line_raster(x1, y1, x2, y2, color, RASTER);
  }
}


void SEND_RASTER_BUF(char * RASTER) {
      lcd_nokia_putbuffer((unsigned char *)RASTER);
}

void reset_Raster(char* RASTER)
{
  memset((void *)RASTER,(int )mycolor, (size_t )SCR_WIDE*SCR_HEIGHT);
}
