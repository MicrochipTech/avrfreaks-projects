#ifndef _AVRSH_PROGMEM_H_
#define _AVRSH_PROGMEM_H_
////////////////////////////////////////////////////////////////////////////
// Name: progmem.h
// Author: B. Gian James <gian@BattleDroids.net>
// Description: This file is an effort to fix gcc's broken PROGMEM attribute
//              in C++.  
//
// $Id: progmem.h,v 1.6 2009/09/05 04:06:17 gian Exp $
////////////////////////////////////////////////////////////////////////////
#include <avr/pgmspace.h>

//extern char const __attribute__((__progmem__))  [];

extern char const __attribute__((__progmem__))  helpmenu1[];
extern char const __attribute__((__progmem__))  helpmenu2[];
extern char const __attribute__((__progmem__))  helpmenu3[];
extern char const __attribute__((__progmem__))  helpmenu4[];
extern char const __attribute__((__progmem__))  helpmenu4_1[];
extern char const __attribute__((__progmem__))  helpmenu5[];
extern char const __attribute__((__progmem__))  helpmenu6[];
extern char const __attribute__((__progmem__))  helpmenu7[];
extern char const __attribute__((__progmem__))  helpmenu8[];
extern char const __attribute__((__progmem__))  helpmenu9[];
extern char const __attribute__((__progmem__))  helpmenu10[];
extern char const __attribute__((__progmem__))  helpmenu11[];
extern char const __attribute__((__progmem__))  helpmenu12[];
extern char const __attribute__((__progmem__))  helpmenu13[];
extern char const __attribute__((__progmem__))  helpmenu14[];
extern char const __attribute__((__progmem__))  helpmenu15[];
extern char const __attribute__((__progmem__))  helpmenu16[];
extern char const __attribute__((__progmem__))  helpmenu17[];
extern char const __attribute__((__progmem__))  helpmenu7_5[];
extern char const __attribute__((__progmem__))  helpmenu16_1[];
extern char const __attribute__((__progmem__))  helpmenu18[];
extern char const __attribute__((__progmem__))  helpmenu19[];
extern char const __attribute__((__progmem__))  helpmenu20[];
extern char const __attribute__((__progmem__))  helpmenu21[];

extern char const __attribute__((__progmem__))  strCmdRead1[];
extern char const __attribute__((__progmem__))  strCmdRead2[];

extern char const __attribute__((__progmem__))  pmUnkCmd[];
extern char const __attribute__((__progmem__))  endl[];


// PortPin
extern char const __attribute__((__progmem__))  pmPB0[];
extern char const __attribute__((__progmem__))  pmPB1[];
extern char const __attribute__((__progmem__))  pmPB2[];
extern char const __attribute__((__progmem__))  pmPB3[];
extern char const __attribute__((__progmem__))  pmPB4[];
extern char const __attribute__((__progmem__))  pmPB5[];
extern char const __attribute__((__progmem__))  pmPB6[];
extern char const __attribute__((__progmem__))  pmPB7[];
extern char const __attribute__((__progmem__))  pmPC0[];
extern char const __attribute__((__progmem__))  pmPC1[];
extern char const __attribute__((__progmem__))  pmPC2[];
extern char const __attribute__((__progmem__))  pmPC3[];
extern char const __attribute__((__progmem__))  pmPC4[];
extern char const __attribute__((__progmem__))  pmPC5[];
extern char const __attribute__((__progmem__))  pmPC6[];
extern char const __attribute__((__progmem__))  pmPD0[];
extern char const __attribute__((__progmem__))  pmPD1[];
extern char const __attribute__((__progmem__))  pmPD2[];
extern char const __attribute__((__progmem__))  pmPD3[];
extern char const __attribute__((__progmem__))  pmPD4[];
extern char const __attribute__((__progmem__))  pmPD5[];
extern char const __attribute__((__progmem__))  pmPD6[];
extern char const __attribute__((__progmem__))  pmPD7[];

// Registers
extern char const __attribute__((__progmem__))  pmPORTB[];
extern char const __attribute__((__progmem__))  pmPORTC[];
extern char const __attribute__((__progmem__))  pmPORTD[];
extern char const __attribute__((__progmem__)) pmDDRD [];
extern char const __attribute__((__progmem__))  pmDDRB[];
extern char const __attribute__((__progmem__))  pmDDRC[];
extern char const __attribute__((__progmem__))  pmPINB[];
extern char const __attribute__((__progmem__))  pmPINC[];
extern char const __attribute__((__progmem__))  pmPIND[];
extern char const __attribute__((__progmem__))  pmSREG[];
extern char const __attribute__((__progmem__))  pmGPIOR0[];
extern char const __attribute__((__progmem__))  pmGPIOR1[];
extern char const __attribute__((__progmem__))  pmGPIOR2[];
extern char const __attribute__((__progmem__))  pmTCCR0B[];
extern char const __attribute__((__progmem__))  pmOCR0A[];
extern char const __attribute__((__progmem__))  pmSPCR[];
extern char const __attribute__((__progmem__))  pmACSR[];
extern char const __attribute__((__progmem__))  pmMCUSR[];
extern char const __attribute__((__progmem__))  pmSPMCSR[];
extern char const __attribute__((__progmem__))  pmCLKPR[];
extern char const __attribute__((__progmem__))  pmOSCCAL[];
extern char const __attribute__((__progmem__))  pmEICRA[];
extern char const __attribute__((__progmem__))  pmPCMSK1[];
extern char const __attribute__((__progmem__))  pmTIMSK1[];
extern char const __attribute__((__progmem__))  pmADCL[];
extern char const __attribute__((__progmem__))  pmADCSRA[];
extern char const __attribute__((__progmem__))  pmADMUX[];
extern char const __attribute__((__progmem__))  pmDIDR1[];
extern char const __attribute__((__progmem__))  pmTCCR1B[];
extern char const __attribute__((__progmem__))  pmTCNT1L[];
extern char const __attribute__((__progmem__))  pmTCNT1H[];
extern char const __attribute__((__progmem__))  pmICR1L[];
extern char const __attribute__((__progmem__))  pmOCR1AL[];
extern char const __attribute__((__progmem__))  pmOCR1AH[];
extern char const __attribute__((__progmem__))  pmOCR1BL[];
extern char const __attribute__((__progmem__))  pmTCCR2A[];
extern char const __attribute__((__progmem__))  pmOCR2B[];
extern char const __attribute__((__progmem__))  pmTCNT2[];
extern char const __attribute__((__progmem__))  pmTWBR[];
extern char const __attribute__((__progmem__))  pmTWAR[];
extern char const __attribute__((__progmem__))  pmTWCR[];
extern char const __attribute__((__progmem__))  pmUCSR0A[];
extern char const __attribute__((__progmem__))  pmUCSR0C[];
extern char const __attribute__((__progmem__))  pmUBRR0L[];
extern char const __attribute__((__progmem__))  pmUDR0[];
extern char const __attribute__((__progmem__))  pmTIFR0[];
extern char const __attribute__((__progmem__))  pmTIFR1[];
extern char const __attribute__((__progmem__))  pmTIFR2[];
extern char const __attribute__((__progmem__))  pmPCIFR[];
extern char const __attribute__((__progmem__))  pmEIFR[];
extern char const __attribute__((__progmem__))  pmEIMSK[];
extern char const __attribute__((__progmem__))  pmEECR[];
extern char const __attribute__((__progmem__))  pmEEDR[];
extern char const __attribute__((__progmem__))  pmEEARL[];
extern char const __attribute__((__progmem__))  pmEEARH[];
extern char const __attribute__((__progmem__))  pmGTCCR[];
extern char const __attribute__((__progmem__))  pmTCCR0A[];
extern char const __attribute__((__progmem__))  pmTCNT0[];
extern char const __attribute__((__progmem__))  pmOCR0B[];
extern char const __attribute__((__progmem__))  pmSPDR[];
extern char const __attribute__((__progmem__))  pmSMCR[];
extern char const __attribute__((__progmem__))  pmMCUCR[];
extern char const __attribute__((__progmem__))  pmWDTCSR[];
extern char const __attribute__((__progmem__))  pmPRR[];
extern char const __attribute__((__progmem__))  pmPCICR[];
extern char const __attribute__((__progmem__))  pmPCMSK0[];
extern char const __attribute__((__progmem__))  pmTIMSK0[];
extern char const __attribute__((__progmem__))  pmTIMSK2[];
extern char const __attribute__((__progmem__))  pmADCH[];
extern char const __attribute__((__progmem__))  pmADCSRB[];
extern char const __attribute__((__progmem__)) pmDIDR0 [];
extern char const __attribute__((__progmem__))  pmTCCR1A[];
extern char const __attribute__((__progmem__))  pmTCCR1C[];
extern char const __attribute__((__progmem__))  pmICR1H[];
extern char const __attribute__((__progmem__))  pmOCR1BH[];
extern char const __attribute__((__progmem__))  pmTCCR2B[];
extern char const __attribute__((__progmem__))  pmOCR2A[];
extern char const __attribute__((__progmem__))  pmASSR[];
extern char const __attribute__((__progmem__))  pmTWSR[];
extern char const __attribute__((__progmem__))  pmTWDR[];
extern char const __attribute__((__progmem__))  pmTWAMR[];
extern char const __attribute__((__progmem__))  pmUCSR0B[];
extern char const __attribute__((__progmem__))  pmUBRR0H[];

// Shell
extern char const __attribute__((__progmem__))  pmHelp[];
extern char const __attribute__((__progmem__))  pmMenu[];
extern char const __attribute__((__progmem__))  pmEna[];
extern char const __attribute__((__progmem__))  pmVars[];
extern char const __attribute__((__progmem__))  pmScript[];
extern char const __attribute__((__progmem__))  pmEcho[];
extern char const __attribute__((__progmem__))  pmCat[];
extern char const __attribute__((__progmem__))  pmRead[];
extern char const __attribute__((__progmem__))  pmWrite[];
extern char const __attribute__((__progmem__))  pmSet[];
extern char const __attribute__((__progmem__))  pmFCPU[];
extern char const __attribute__((__progmem__))  pmTimer[];
extern char const __attribute__((__progmem__))  pmAuth[];
extern char const __attribute__((__progmem__))  pmPasswd[];
extern char const __attribute__((__progmem__))  pmPrint[];
extern char const __attribute__((__progmem__))  pmPowerup[];
extern char const __attribute__((__progmem__))  pmPowerdown[];
extern char const __attribute__((__progmem__))  pmTimer[];
extern char const __attribute__((__progmem__))  pmEnaSvc[];
//extern char const __attribute__((__progmem__))  [];
//extern char const __attribute__((__progmem__))  [];
//extern char const __attribute__((__progmem__))  [];
//extern char const __attribute__((__progmem__))  [];
//extern char const __attribute__((__progmem__))  [];
//extern char const __attribute__((__progmem__))  [];
//extern char const __attribute__((__progmem__))  [];
//extern char const __attribute__((__progmem__))  [];


#ifdef _AVRSH_FILESYSTEM_H_
// DirectoryService
extern char const __attribute__((__progmem__))  pmRoot[];
extern char const __attribute__((__progmem__))  pmRegisters[];
extern char const __attribute__((__progmem__))  pmPins[];
extern char const __attribute__((__progmem__))  pmPeripherals[];
extern char const __attribute__((__progmem__))  pmUsers[];
extern char const __attribute__((__progmem__))  pmDirHead[];
extern char const __attribute__((__progmem__))  pmDirFormat[];
extern char const __attribute__((__progmem__))  pmNULL[];
#endif


#endif	/* _AVRSH_PROGMEM_H_ */