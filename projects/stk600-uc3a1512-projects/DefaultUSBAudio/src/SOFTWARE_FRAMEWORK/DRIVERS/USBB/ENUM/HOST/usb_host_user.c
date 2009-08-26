/* This source file is part of the ATMEL AVR32-SoftwareFramework-AT32UC3A-1.4.0 Release */

/*This file is prepared for Doxygen automatic documentation generation.*/
/*! \file ******************************************************************
 *
 * \brief Processing of USB device specific enumeration requests.
 *
 * This file contains the specific request decoding for enumeration process.
 *
 * - Compiler:           IAR EWAVR32 and GNU GCC for AVR32
 * - Supported devices:  All AVR32 devices with a USB module can be used.
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


//_____ I N C L U D E S ____________________________________________________

#include "conf_usb.h"


#if USB_HOST_FEATURE == ENABLED

#include <stdio.h>
#include "conf_usb.h"
#include "usb_host_enum.h"
#include "usb_host_user.h"
#include "usb_audio.h"
#include "host_audio_task.h"


//_____ M A C R O S ________________________________________________________


//_____ D E F I N I T I O N S ______________________________________________


//_____ P R I V A T E   D E C L A R A T I O N S ____________________________

extern volatile cs_feature_unit_t g_cs_feature[];
extern volatile U8                g_cs_num_features_unit;

extern volatile U8   g_n_channels[];
extern volatile U8   g_n_bits_per_sample[];
extern volatile U32  g_sample_freq[];
                U8   state=0;

U8 n_interface=0;

//_____ D E C L A R A T I O N S ____________________________________________

//! Initialisation
//!
void host_user_check_class_init(void)
{
   state=0;
   n_interface=0;
   g_cs_num_features_unit=0;
}


//! This function is called by the standard USB host_check_class() function when
//! the Host is analysing the configuration descriptor previously sent by a USB Device.
//! The function will be called for each descriptor found in the configuration descriptor.
//!
Bool host_user_check_class(const U8* descriptor)
{
   U32 num;
   const U8* p_bmaControls;
   U8  n_sample_freq=0;
   U8  i;

   switch (descriptor[OFFSET_FIELD_DESCRIPTOR_TYPE])
   {
   case INTERFACE_DESCRIPTOR:
      if (n_interface < MAX_INTERFACE_SUPPORTED)
         n_interface++;
      break;

   case CS_INTERFACE:

      switch (descriptor[OFFSET_FIELD_DESCRIPTOR_SUBTYPE])
      {
      case HEADER_SUB_TYPE:
         // This variable allow to detect AUDIO CONTROL sub class to AUDIO STREAMING
         // sub class transition.
         state++;
         break;

      case FEATURE_UNIT_SUB_TYPE:
         if( state!=1 )
            break;

         // Still in AUDIO CONTROL sub class
         // Analysis of the Features Units

         // Ensure no out-of-limit access in array.
         num = (descriptor[0]-7)>MAX_BMA_CONTROLS ? MAX_BMA_CONTROLS : descriptor[0]-7;

         g_cs_feature[g_cs_num_features_unit].n_bmaControls = num;
         g_cs_feature[g_cs_num_features_unit].unit          = descriptor[3];

         p_bmaControls = &descriptor[6]; // Point on beginning of bmaControls infos
         for( i=0 ; i<num ; i++,p_bmaControls++ )
         {
            g_cs_feature[g_cs_num_features_unit].bmaControls[i]=p_bmaControls[0];
         }
         g_cs_num_features_unit++;

      case FORMAT_SUB_TYPE:
         if( state<2 )
            break;

         // We are in AUDIO STREAMING sub class
         // Analysis of the sampling frequencies

         const U8* p_sampling_freq= &descriptor[8];
               U8  id_interface=Get_interface_number(n_interface-1);

         g_n_channels[id_interface]        = descriptor[4];
         g_n_bits_per_sample[id_interface] = descriptor[6];
         n_sample_freq                     = descriptor[7];
         g_sample_freq[id_interface]=0;
         for( i=0 ; i<n_sample_freq ; i++, p_sampling_freq+=3 )
         {
            // Keep the max declared sampling frequency. An other policy would be to keep the
            // highest sampling frequency which is compatible with the CPU and PBA frequency
            g_sample_freq[id_interface]= max(
               g_sample_freq[id_interface]
            ,  p_sampling_freq[0] + (p_sampling_freq[1]<<8) + (p_sampling_freq[2]<<16));
         }

         break;
      }
      break;

   default:
      break;
   }
   return TRUE;
}

#endif  // USB_HOST_FEATURE == ENABLED
