/* ***** BEGIN LICENSE BLOCK *****
 * Version: RCSL 1.0/RPSL 1.0
 *
 * Portions Copyright (c) 1995-2002 RealNetworks, Inc. All Rights Reserved.
 *
 * The contents of this file, and the files included with this file, are
 * subject to the current version of the RealNetworks Public Source License
 * Version 1.0 (the "RPSL") available at
 * http://www.helixcommunity.org/content/rpsl unless you have licensed
 * the file under the RealNetworks Community Source License Version 1.0
 * (the "RCSL") available at http://www.helixcommunity.org/content/rcsl,
 * in which case the RCSL will apply. You may also obtain the license terms
 * directly from RealNetworks.  You may not use this file except in
 * compliance with the RPSL or, if you have a valid RCSL with RealNetworks
 * applicable to this file, the RCSL.  Please see the applicable RPSL or
 * RCSL for the rights, obligations and limitations governing use of the
 * contents of the file.
 *
 * This file is part of the Helix DNA Technology. RealNetworks is the
 * developer of the Original Code and owns the copyrights in the portions
 * it created.
 *
 * This file, and the files included with this file, is distributed and made
 * available on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, AND REALNETWORKS HEREBY DISCLAIMS ALL SUCH WARRANTIES,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT.
 *
 * Technology Compatibility Kit Test Suite(s) Location:
 *    http://www.helixcommunity.org/content/tck
 *
 * Contributor(s):
 *
 * ***** END LICENSE BLOCK ***** */

/**************************************************************************************
 * Fixed-point MP3 decoder
 * Jon Recker (jrecker@real.com), Ken Cooke (kenc@real.com)
 * June 2003
 *
 * subband.c - subband transform (synthesis filterbank implemented via 32-point DCT
 *               followed by polyphase filter)
 **************************************************************************************/

#include "coder.h"
#include "assembly.h"

#include "conf_player.h"
#include "pwm_dac.h"

#include "board.h"

extern unsigned short outBuf[]; 	// Output buffer.
extern unsigned short *outPtr; 	// Pointer to the current output buffer position.
extern Bool dacStarted;	// Whether the dac is started or not.

/**************************************************************************************
 * Function:    Subband
 *
 * Description: do subband transform on all the blocks in one granule, all channels
 *
 * Inputs:      filled MP3DecInfo structure, after calling IMDCT for all channels
 *              vbuf[ch] and vindex[ch] must be preserved between calls
 *
 * Outputs:     decoded PCM data, interleaved LRLRLR... if stereo
 *
 * Return:      0 on success,  -1 if null input pointers
 **************************************************************************************/
int Subband(MP3DecInfo *mp3DecInfo, short *pcmBuf)
{
	int b;
	HuffmanInfo *hi;
	IMDCTInfo *mi;
	SubbandInfo *sbi;

	/* validate pointers */
	if (!mp3DecInfo || !mp3DecInfo->HuffmanInfoPS || !mp3DecInfo->IMDCTInfoPS || !mp3DecInfo->SubbandInfoPS)
		return -1;

	hi = (HuffmanInfo *)mp3DecInfo->HuffmanInfoPS;
	mi = (IMDCTInfo *)(mp3DecInfo->IMDCTInfoPS);
	sbi = (SubbandInfo*)(mp3DecInfo->SubbandInfoPS);

	if (mp3DecInfo->nChans == 2) {
		/* stereo */
		for (b = 0; b < BLOCK_SIZE; b++) {
			// If we're at the end of the output buffer, rewind it.
			if(outPtr >= outBuf + (OUTBUF_SIZE-1)) {
				/* If the dac isn't started yet, we start it here. */
				if(!dacStarted) {
					// Start playing from the beginning of the buffer.
					pwm_dac_start(mp3DecInfo->samprate, 2, 16, (unsigned short*)outBuf, PBA_HZ);
					dacStarted = TRUE;
				}
				outPtr = outBuf;
			}

			/* If we're about to overwrite samples that the dac hasn't consumed yet, wait. */
			while(dacStarted && ((unsigned short*)pwm_dac.sample_buffer < (outPtr + (2*NBANDS))) && (pwm_dac.sample_buffer > outPtr));

			FDCT32(mi->outBuf[0][b], sbi->vbuf + 0*32, sbi->vindex, (b & 0x01), mi->gb[0]);
			FDCT32(mi->outBuf[1][b], sbi->vbuf + 1*32, sbi->vindex, (b & 0x01), mi->gb[1]);
			PolyphaseStereo((short*)outPtr, sbi->vbuf + sbi->vindex + VBUF_LENGTH * (b & 0x01), polyCoef);
			sbi->vindex = (sbi->vindex - (b & 0x01)) & 7;
			outPtr += (2 * NBANDS);
		}
	} else {
		/* mono */
		for (b = 0; b < BLOCK_SIZE; b++) {
			/* If we're at the end of the output buffer, rewind it. */
			if(outPtr >= outBuf + (OUTBUF_SIZE-1)) {
				/* If the dac isn't started yet, we start it here. */
				if(!dacStarted) {
					// Start playing from the beginning of the buffer.
					pwm_dac_start(mp3DecInfo->samprate, 1, 16, (unsigned short*)outBuf, PBA_HZ);
					dacStarted = TRUE;
				}
				outPtr = outBuf;
			}

			/* If we're about to overwrite samples that the dac hasn't consumed yet, wait. */
			while(dacStarted && ((unsigned short*)pwm_dac.sample_buffer < (outPtr + NBANDS)) && (pwm_dac.sample_buffer) > outPtr);

			FDCT32(mi->outBuf[0][b], sbi->vbuf + 0*32, sbi->vindex, (b & 0x01), mi->gb[0]);
			PolyphaseMono((short*)outPtr, sbi->vbuf + sbi->vindex + VBUF_LENGTH * (b & 0x01), polyCoef);
			sbi->vindex = (sbi->vindex - (b & 0x01)) & 7;
			outPtr += NBANDS;
		}
	}

	return 0;
}

