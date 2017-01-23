/******************************************************************************
 * 
 * Controller Area Network (CAN) Demo-Application
 * Atmel AVR with Microchip MCP2515 
 * 
 * Copyright (C) 2005 Martin THOMAS, Kaiserslautern, Germany
 * <eversmith@heizung-thomas.de>
 * http://www.siwawi.arubi.uni-kl.de/avr_projects
 *
 *****************************************************************************
 *
 * File    : mcp2515_bittime.h
 * Version : 0.9
 * 
 * Summary : MCP2515 CAN-timing values. Calculated with 
 *           a spreadsheet-based "Preprocessor" with information
 *           from the MCP2515 and AT90CAN128 datasheets.
 *
 *****************************************************************************/

#include "mcp2515_defs.h"

/*-----------------------*/
/* global settings       */
/*-----------------------*/

/* CFG3 */

// Start of Frame SOF (CLKOUT)
// either SOF_ENABLE or SOF_DISABLE
#define MCP_GENERAL_SOF (SOF_DISABLE)

// Wake up Filter (WAKFIL)
// either WAKFIL_ENABLE	or WAKFIL_DISABLE
#define MCP_GENERAL_WAKFIL (WAKFIL_DISABLE)


/*-----------------------*/
/* 125 kBPS @ 4MHZ F_OSC */
/*-----------------------*/

/* CNF1 */

// SJW as defined in mcp_defs.h
// here: SJW = 1*TQ
#define MCP_4MHz_125kBPS_SJW (SJW1)

// Prescaler = (BRP+1)*2
// here Prescaler = 4 -> BRP=1
#define MCP_4MHz_125kBPS_BRP (1)


/* CNF2 */

// BLT-Mode defined in CNF3 (0 or BTLMODE from mcp_defs.h)
#define MCP_4MHz_125kBPS_BLTMODE (BTLMODE)

// 3 samples (SAMPLE_3X) or 1 sample (SAMPLE_1X)
#define MCP_4MHz_125kBPS_SAM   (SAMPLE_1X)

// (Phase Segment 1) PHSEG1 = PS1 - 1
// here: PS1 calculated as 2 
#define MCP_4MHz_125kBPS_PHSEG1 ((2-1)<<3)

// (Propagation Delay) PRSEG = PRSEQTQ-1
// here PRSEQTQ = 2
#define MCP_4MHz_125kBPS_PRSEG (2-1)

/* CNF3 */

// (Phase Segment 2) PHSEG2 = PS2 - 1
// here: PS2 calculated as 3
#define MCP_4MHz_125kBPS_PHSEG (3-1)


#define MCP_4MHz_125kBPS_CFG1 (MCP_4MHz_125kBPS_SJW | MCP_4MHz_125kBPS_BRP)
#define MCP_4MHz_125kBPS_CFG2 (MCP_4MHz_125kBPS_BLTMODE | MCP_4MHz_125kBPS_SAM | \
				MCP_4MHz_125kBPS_PHSEG1 | MCP_4MHz_125kBPS_PRSEG)
#define MCP_4MHz_125kBPS_CFG3 (MCP_GENERAL_SOF | MCP_GENERAL_WAKFIL | \
				MCP_4MHz_125kBPS_PHSEG)


/*-----------------------*/
/* 20 kBPS @ 4MHZ F_OSC  */
/*-----------------------*/

/* CNF1 */

// SJW as defined in mcp_defs.h
// here: SJW = 1*TQ
#define MCP_4MHz_20kBPS_SJW (SJW1)

// Prescaler = (BRP+1)*2
// here Prescaler = 10 -> BRP=4  (20TQ)
#define MCP_4MHz_20kBPS_BRP (4)


/* CNF2 */

// BLT-Mode defined in CNF3 (0 or BTLMODE from mcp_defs.h)
#define MCP_4MHz_20kBPS_BLTMODE (BTLMODE)

// 3 samples (SAMPLE_1X) or 1 sample (SAMPLE_1X)
#define MCP_4MHz_20kBPS_SAM   (SAMPLE_1X)

// (Phase Segment 1) PHSEG1 = PS1 - 1
// here: PS1 calculated as 8
#define MCP_4MHz_20kBPS_PHSEG1 ((8-1)<<3)

// (Propagation Delay) PRSEG = PRSEQTQ-1
// here PRSEQTQ = 3
#define MCP_4MHz_20kBPS_PRSEG (3-1)

/* CNF3 */

// (Phase Segment 2) PHSEG2 = PS2 - 1
// here: PS2 calculated as 8
#define MCP_4MHz_20kBPS_PHSEG (8-1)


#define MCP_4MHz_20kBPS_CFG1 (MCP_4MHz_20kBPS_SJW | MCP_4MHz_20kBPS_BRP)
#define MCP_4MHz_20kBPS_CFG2 (MCP_4MHz_20kBPS_BLTMODE | MCP_4MHz_20kBPS_SAM | \
				MCP_4MHz_20kBPS_PHSEG1 | MCP_4MHz_20kBPS_PRSEG)
#define MCP_4MHz_20kBPS_CFG3 (MCP_GENERAL_SOF | MCP_GENERAL_WAKFIL | \
				MCP_4MHz_20kBPS_PHSEG)

