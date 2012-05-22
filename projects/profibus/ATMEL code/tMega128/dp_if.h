/************************** Filename: dp_if.h ********************************/
/* ========================================================================= */
/*                                                                           */
/* 0000  000   000  00000 0  000  0   0 0 0000                               */
/* 0   0 0  0 0   0 0     0 0   0 0   0 0 0   0      Einsteinstraße 6        */
/* 0   0 0  0 0   0 0     0 0     0   0 0 0   0      91074 Herzogenaurach    */
/* 0000  000  0   0 000   0 0     00000 0 0000       Germany                 */
/* 0     00   0   0 0     0 0     0   0 0 0                                  */
/* 0     0 0  0   0 0     0 0   0 0   0 0 0          Tel: ++49-9132-744-200  */
/* 0     0  0  000  0     0  000  0   0 0 0    GmbH  Fax: ++49-9132-744-204  */
/*                                                                           */
/* ========================================================================= */
/*                                                                           */
/* Technical support:       Peter FREDEHORST                                 */
/*                          Tel. : ++49-9132-744-2150                        */
/*                          Fax. : ++49-9132-744-29-2150                     */
/*                          eMail: pfredehorst@profichip.com                 */
/*                          eMail: support@profichip.com                     */
/*                                                                           */
/*****************************************************************************/

/*! \file
     \brief Headerfile for all VPC3+ / PROFIBUS definitions.
     
*/

/*****************************************************************************/
/* contents:

    1.0 VPC3+
    1.1     - register structure of VPC3+
    1.2     - literals for interrupt register
    1.3     - literals for status register
    1.4     - literals for mode register 0
    1.5     - literals for mode register 1
    1.6     - literals for mode register 2
    1.7     - basic macros for VPC3+

    2.0 - prm data
    2.1     - structure of prm data
    2.2     - literals for DPV1 Status 1
    2.3     - literals for DPV1 Status 2
    2.4     - literals for DPV1 Status 3
    2.5     - general defines for prm data
    2.6     - literals for structured prm data
    2.7     - returncodes for prm data
    2.8     - macros for prm data

    3.0 - cfg data
    3.1     - standard format of cfg byte
    3.2     - special format of cfg byte
    3.3     - literals for cfg byte
    3.4     - returncodes for cfg data
    3.5     - literals for chk config data
    3.6     - macros for cfg data
    3.7     - structure for real cfg data

    4.0 - input / output
    4.1     - structure for calculated input-/output-length
    4.2     - states for output buffer
    4.3     - macros for input buffer

    5.0 - set slave address
    5.1     - structure
    5.2     - macros

    6.0 - global control telegram
    6.1     - defines for GLOBAL CONTROL
    6.2     - macros for global control

    7.0 - diagnostic telegram
    7.1     - defines
    7.2     - structures
    7.3     - macros

    8.0 - subscriber
    8.1     - structures
    8.2     - macros

    9.0 - isochron mode
    9.1     - defines
    9.2     - structures

   10.0 - fdl layer
   10.1     - SAPs (service access points) for MSAC1
   10.2     - SAPs (service access points) for MSAC2
   10.3     - structure of a FDL-indication-response-buffer-head
   10.4     - structure of a FDL-sap-entry
   10.5     - structure of a sap-control-block
   10.6     - structure of a indication- or response-buffer
   10.7     - structure of the immediate-response-pdu of the FDL-RM-SAP
   10.8     - global structure of FDL state machine
   10.9     - function-codes
   10.A     - returncodes of FDL-services
   10.B     - helpful macros

   11.0 - DPV1
   11.1     - return codes for DPV1 services
   11.2     - errorcodes of DPV1 for ERROR_DECODE
   11.3     - errorcodes of DPV1 for ERROR_CODE_1
   11.4     - coding of abort
   11.5     - Function Codes for DPV1
   11.6     - general defines for DPV1
   11.7     - structures for DPV1
   11.8     - defines for ALARM
   11.9     - structure of ALARM

   12.0 - global system structure

   13.0 - ERROR defines

*/
/*****************************************************************************/
/* reinclude protection */


#ifndef DP_IF_H
#define DP_IF_H

#if BUSINTERFACE_8_BIT

   #define TVpc3Byte UBYTE

#endif//#if BUSINTERFACE_8_BIT

#if BUSINTERFACE_16_BIT

   typedef struct
   {
      UBYTE bLo;
      UBYTE bHi;
   } TVpc3Byte;

#endif//#if BUSINTERFACE_16_BIT

#if BUSINTERFACE_32_BIT

   typedef struct
   {
      UBYTE bLo;
      UBYTE bHi;
      UBYTE bLo1;
      UBYTE bHi1;
   } TVpc3Byte;

#endif//#if BUSINTERFACE_32_BIT

#define pVPC3Byte TVpc3Byte VPC3_PTR

/*-----------------------------------------------------------------------------------------------------------*/
/* 1.0 vpc3+                                                                                                 */
/*-----------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/* 1.1 register structure of vpc3+                                           */
/*---------------------------------------------------------------------------*/

#if VPC3_SERIAL_MODE

   extern void    Vpc3Write            ( VPC3_ADR wAddress, UBYTE bData );
   extern UBYTE   Vpc3Read             ( VPC3_ADR wAddress );

   extern VPC3_ADR   Vpc3AsicAddress;                          // global asic address

   #define bVpc3RwIntReqReg_L          (VPC3_ADR)( Vpc3AsicAddress + 0x00 )   // 000H     Interrupt request register low
   #define bVpc3RwIntReqReg_H          (VPC3_ADR)( Vpc3AsicAddress + 0x01 )   // 001H     Interrupt request register high

   #define bVpc3RoIntReg_L             (VPC3_ADR)( Vpc3AsicAddress + 0x02 )   // 002H     Interrupt register low
   #define bVpc3RoIntReg_H             (VPC3_ADR)( Vpc3AsicAddress + 0x03 )   // 003H     Interrupt register high
   #define bVpc3RoStatus_L             (VPC3_ADR)( Vpc3AsicAddress + 0x04 )   // 004H     status register low
   #define bVpc3RoStatus_H             (VPC3_ADR)( Vpc3AsicAddress + 0x05 )   // 005H     status register high

   #define bVpc3WoIntAck_L             (VPC3_ADR)( Vpc3AsicAddress + 0x02 )   // 002H     Interrupt acknowledge low
   #define bVpc3WoIntAck_H             (VPC3_ADR)( Vpc3AsicAddress + 0x03 )   // 003H     Interrupt acknowledge high
   #define bVpc3WoIntMask_L            (VPC3_ADR)( Vpc3AsicAddress + 0x04 )   // 004H     Interrupt mask register low
   #define bVpc3WoIntMask_H            (VPC3_ADR)( Vpc3AsicAddress + 0x05 )   // 005H     Interrupt mask register high

   #define bVpc3RwModeReg0_L           (VPC3_ADR)( Vpc3AsicAddress + 0x06 )   // 006H     mode register0 low
   #define bVpc3RwModeReg0_H           (VPC3_ADR)( Vpc3AsicAddress + 0x07 )   // 007H     mode register0 high

   #define bVpc3RoDinBufferSm          (VPC3_ADR)( Vpc3AsicAddress + 0x08 )   // 008H     buffer assignment of the DP_DIN_BUFFER state machine
   #define bVpc3RoNewDinBufferCmd      (VPC3_ADR)( Vpc3AsicAddress + 0x09 )   // 009H     the user makes a new DP_DIN_BUFFER available in the N state
   #define bVpc3RoDoutBufferSm         (VPC3_ADR)( Vpc3AsicAddress + 0x0A )   // 00AH     buffer assignment of the DP_DOUT_BUFFER state machine
   #define bVpc3RoNextDoutBufferCmd    (VPC3_ADR)( Vpc3AsicAddress + 0x0B )   // 00BH     the user fetches the last DP_DOUT_BUFFER from the N state
   #define bVpc3RoDiagBufferSm         (VPC3_ADR)( Vpc3AsicAddress + 0x0C )   // 00CH     buffer assignment for DP_DIAG_BUFFER state machine
   #define bVpc3RoNewDiagBufferCmd     (VPC3_ADR)( Vpc3AsicAddress + 0x0D )   // 00DH     the user makes a new DP_DIAG_BUFFER available to the VPC3+
   #define bVpc3RoUserPrmDataOkay      (VPC3_ADR)( Vpc3AsicAddress + 0x0E )   // 00EH     positive acknowledge for received user parameter data
   #define bVpc3RoUserPrmDataNotOkay   (VPC3_ADR)( Vpc3AsicAddress + 0x0F )   // 00FH     negative acknowledge for received user parameter data
   #define bVpc3RoUserCfgDataOkay      (VPC3_ADR)( Vpc3AsicAddress + 0x10 )   // 010H     positive acknowledge for received config data
   #define bVpc3RoUserCfgDataNotOkay   (VPC3_ADR)( Vpc3AsicAddress + 0x11 )   // 011H     negative acknowledge for received config data
   #define bVpc3RoDxbOutBufferSm       (VPC3_ADR)( Vpc3AsicAddress + 0x12 )   // 012H     buffer assignment of the DXB_OUT_BUFFER state machine
   #define bVpc3RoNextDxbOutBufferCmd  (VPC3_ADR)( Vpc3AsicAddress + 0x13 )   // 013H     the user fetches the last DXB_OUT_BUFFER
   #define bVpc3RoSsaBufferFreeCmd     (VPC3_ADR)( Vpc3AsicAddress + 0x14 )   // 014H     the user has fetched data from ssa buffer and enables buffer again
   #define bVpc3RoModeReg1             (VPC3_ADR)( Vpc3AsicAddress + 0x15 )   // 015H     current value of mode_reg1

   #define bVpc3WoModeReg1_S           (VPC3_ADR)( Vpc3AsicAddress + 0x08 )   // 008H     set b0..b7 in mode_reg1
   #define bVpc3WoModeReg1_R           (VPC3_ADR)( Vpc3AsicAddress + 0x09 )   // 009H     reset b0..b7 in mode_reg1
   #define bVpc3WoWdBaudControlVal     (VPC3_ADR)( Vpc3AsicAddress + 0x0A )   // 00AH     root value for baud rate monitoring
   #define bVpc3WoMinTsdrVal           (VPC3_ADR)( Vpc3AsicAddress + 0x0B )   // 00BH     MinTsdr time
   #define bVpc3WoModeReg2             (VPC3_ADR)( Vpc3AsicAddress + 0x0C )   // 00CH     set b0..b7 in mode_reg2
   #define bVpc3WoSyncPwReg            (VPC3_ADR)( Vpc3AsicAddress + 0x0D )   // 00DH     sync pulse width register
   #define bVpc3WoControlCommandReg    (VPC3_ADR)( Vpc3AsicAddress + 0x0E )   // 00EH
   #define bVpc3WoGroupSelectReg       (VPC3_ADR)( Vpc3AsicAddress + 0x0F )   // 00FH

   #define bVpc3RwTsAddr               (VPC3_ADR)( Vpc3AsicAddress + 0x16 )   // 016H     setup VPC3+ station address
   #define bVpc3RwFdlSapListPtr        (VPC3_ADR)( Vpc3AsicAddress + 0x17 )   // 017H     pointer fdl_sap_list
   #define bVpc3RwUserWdValue_L        (VPC3_ADR)( Vpc3AsicAddress + 0x18 )   // 018H     user watchdog value b0..b7
   #define bVpc3RwUserWdValue_H        (VPC3_ADR)( Vpc3AsicAddress + 0x19 )   // 019H     user watchdog value b8..b15
   #define bVpc3RwLenDoutBuf           (VPC3_ADR)( Vpc3AsicAddress + 0x1A )   // 01AH     length of dout buffers
   #define bVpc3RwDoutBufPtr1          (VPC3_ADR)( Vpc3AsicAddress + 0x1B )   // 01BH     segment base address of dout_buffer [0]
   #define bVpc3RwDoutBufPtr2          (VPC3_ADR)( Vpc3AsicAddress + 0x1C )   // 01CH     segment base address of dout_buffer [1]
   #define bVpc3RwDoutBufPtr3          (VPC3_ADR)( Vpc3AsicAddress + 0x1D )   // 01DH     segment base address of dout_buffer [2]
   #define bVpc3RwLenDinBuf            (VPC3_ADR)( Vpc3AsicAddress + 0x1E )   // 01EH     length of din buffers
   #define bVpc3RwDinBufPtr1           (VPC3_ADR)( Vpc3AsicAddress + 0x1F )   // 01FH     segment base address of din_buffer [0]
   #define bVpc3RwDinBufPtr2           (VPC3_ADR)( Vpc3AsicAddress + 0x20 )   // 020H     segment base address of din_buffer [1]
   #define bVpc3RwDinBufPtr3           (VPC3_ADR)( Vpc3AsicAddress + 0x21 )   // 021H     segment base address of din_buffer [2]
   #define bVpc3RwLenDxbOutBuf         (VPC3_ADR)( Vpc3AsicAddress + 0x22 )   // 022H     length of dxb buffers
   #define bVpc3RwDxbOutBufPtr1        (VPC3_ADR)( Vpc3AsicAddress + 0x23 )   // 023H     segment base address of dxbout_buffer1
   #define bVpc3RwLenDiagBuf1          (VPC3_ADR)( Vpc3AsicAddress + 0x24 )   // 024H     length of diag buffers [0]
   #define bVpc3RwLenDiagBuf2          (VPC3_ADR)( Vpc3AsicAddress + 0x25 )   // 025H     length of diag buffers [1]
   #define bVpc3RwDiagBufPtr1          (VPC3_ADR)( Vpc3AsicAddress + 0x26 )   // 026H     segment base address of diag_buffer [0]
   #define bVpc3RwDiagBufPtr2          (VPC3_ADR)( Vpc3AsicAddress + 0x27 )   // 027H     segment base address of diag_buffer [1]
   #define bVpc3RwLenCtrlBuf1          (VPC3_ADR)( Vpc3AsicAddress + 0x28 )   // 028H     length of aux buffer 1
   #define bVpc3RwLenCtrlBuf2          (VPC3_ADR)( Vpc3AsicAddress + 0x29 )   // 029H     length of aux buffer 2
   #define bVpc3RwAuxBufSel            (VPC3_ADR)( Vpc3AsicAddress + 0x2A )   // 02AH     assignment for aux buffers 1/2
   #define bVpc3RwAuxBufPtr1           (VPC3_ADR)( Vpc3AsicAddress + 0x2B )   // 02BH     segment base address of aux buffer 1
   #define bVpc3RwAuxBufPtr2           (VPC3_ADR)( Vpc3AsicAddress + 0x2C )   // 02CH     segment base address of aux buffer 2
   #define bVpc3RwLenSsaBuf            (VPC3_ADR)( Vpc3AsicAddress + 0x2D )   // 02DH     length of SET_SLAVE_ADDRESS buffer
   #define bVpc3RwSsaBufPtr            (VPC3_ADR)( Vpc3AsicAddress + 0x2E )   // 02EH     segment base address of SET_SLAVE_ADDRESS buffer
   #define bVpc3RwLenPrmData           (VPC3_ADR)( Vpc3AsicAddress + 0x2F )   // 02FH     max. length of input data in SET_PRM buffer
   #define bVpc3RwPrmBufPtr            (VPC3_ADR)( Vpc3AsicAddress + 0x30 )   // 030H     segment base address of SET_PRM buffer
   #define bVpc3RwLenCfgData           (VPC3_ADR)( Vpc3AsicAddress + 0x31 )   // 031H     length of input data in the CHECK_CONFIG buffer
   #define bVpc3RwCfgBufPtr            (VPC3_ADR)( Vpc3AsicAddress + 0x32 )   // 032H     segment base address of CHECK_CONFIG buffer
   #define bVpc3RwLenReadCfgData       (VPC3_ADR)( Vpc3AsicAddress + 0x33 )   // 033H     length of input data in the GET_CONFIG buffer
   #define bVpc3RwReadCfgBufPtr        (VPC3_ADR)( Vpc3AsicAddress + 0x34 )   // 034H     segment base address of GET_CONFIG buffer
   #define bVpc3RwLenDxbLinkBuf        (VPC3_ADR)( Vpc3AsicAddress + 0x35 )   // 035H     length of dxb link table buffer
   #define bVpc3RwDxbLinkBufPtr        (VPC3_ADR)( Vpc3AsicAddress + 0x36 )   // 036H     segment base address of dxb link table buffer
   #define bVpc3RwLenDxbStatusBuf      (VPC3_ADR)( Vpc3AsicAddress + 0x37 )   // 037H     length of dxb link status buffer
   #define bVpc3RwDxbStatusBufPtr      (VPC3_ADR)( Vpc3AsicAddress + 0x38 )   // 038H     segment base address of dxb link status buffer
   #define bVpc3RwRealNoAddChange      (VPC3_ADR)( Vpc3AsicAddress + 0x39 )   // 039H     address changes
   #define bVpc3RwIdentLow             (VPC3_ADR)( Vpc3AsicAddress + 0x3A )   // 03AH     IDENT_LOW
   #define bVpc3RwIdentHigh            (VPC3_ADR)( Vpc3AsicAddress + 0x3B )   // 03BH     IDENT_HIGH
   #define bVpc3RwGcCommand            (VPC3_ADR)( Vpc3AsicAddress + 0x3C )   // 03CH     last global control command
   #define bVpc3RwLenSpecPrmBuf        (VPC3_ADR)( Vpc3AsicAddress + 0x3D )   // 03DH     length of SPEC_PRM buffer
   #define bVpc3RwDxbOutBufPtr2        (VPC3_ADR)( Vpc3AsicAddress + 0x3E )   // 03EH     segment base address of dxbout_buffer2
   #define bVpc3RwDxbOutBufPtr3        (VPC3_ADR)( Vpc3AsicAddress + 0x3F )   // 03FH     segment base address of dxbout_buffer3

   #define bVpc3RwStartSapCtrlList     (VPC3_ADR)( Vpc3AsicAddress + 0x40 )   // 040H     SAP CONTROL BLOCK LIST

   #ifdef DP_TIMESTAMP

      #if DP_VPC3_4KB_MODE

         #define bVpc3RwCsStatus                (VPC3_ADR)( Vpc3AsicAddress + 0xFE0 )     // FE0H
         #define bVpc3RwCsCommand               (VPC3_ADR)( Vpc3AsicAddress + 0xFE1 )     // FE1H
         #define bVpc3RwCsCvStatus1             (VPC3_ADR)( Vpc3AsicAddress + 0xFE2 )     // FE2H
         #define bVpc3RwCsCvStatus2             (VPC3_ADR)( Vpc3AsicAddress + 0xFE3 )     // FE3H
         #define abVpc3RwCsCvTimeEvent          (VPC3_ADR)( Vpc3AsicAddress + 0xFE4 )     // FE4H
         #define abVpc3RwCsRecDelTime           (VPC3_ADR)( Vpc3AsicAddress + 0xFEC )     // FECH
         #define abVpc3RwCsCvPrevTimeEvent      (VPC3_ADR)( Vpc3AsicAddress + 0xFF0 )     // FF0H
         #define abVpc3RwCsCsInterval           (VPC3_ADR)( Vpc3AsicAddress + 0xFF8 )     // FF8H

      #else

         #define bVpc3RwCsStatus                (VPC3_ADR)( Vpc3AsicAddress + 0x7E0 )     // 7E0H
         #define bVpc3RwCsCommand               (VPC3_ADR)( Vpc3AsicAddress + 0x7E1 )     // 7E1H
         #define bVpc3RwCsCvStatus1             (VPC3_ADR)( Vpc3AsicAddress + 0x7E2 )     // 7E2H
         #define bVpc3RwCsCvStatus2             (VPC3_ADR)( Vpc3AsicAddress + 0x7E3 )     // 7E3H
         #define abVpc3RwCsCvTimeEvent          (VPC3_ADR)( Vpc3AsicAddress + 0x7E4 )     // 7E4H
         #define abVpc3RwCsRecDelTime           (VPC3_ADR)( Vpc3AsicAddress + 0x7EC )     // 7ECH
         #define abVpc3RwCsCvPrevTimeEvent      (VPC3_ADR)( Vpc3AsicAddress + 0x7F0 )     // 7F0H
         #define abVpc3RwCsCsInterval           (VPC3_ADR)( Vpc3AsicAddress + 0x7F8 )     // 7F8H

      #endif//#if DP_VPC3_4KB_MODE

   #endif//#ifdef DP_TIMESTAMP

   typedef struct
   {
      TVpc3Byte bIntReqReg_L;                         // 000H     Interrupt request register low
      TVpc3Byte bIntReqReg_H;                         // 001H     Interrupt request register high

      union
      {
         struct
         {                                            // [read]
            TVpc3Byte bIntReg_L;                      // 002H     Interrupt register low
            TVpc3Byte bIntReg_H;                      // 003H     Interrupt register high
            TVpc3Byte bStatus_L;                      // 004H     status register low
            TVpc3Byte bStatus_H;                      // 005H     status register high
         }sRead;

         struct
         {                                            // [write]
            TVpc3Byte bIntAck_L;                      // 002H     Interrupt acknowledge low
            TVpc3Byte bIntAck_H;                      // 003H     Interrupt acknowledge high
            TVpc3Byte bIntMask_L;                     // 004H     Interrupt mask register low
            TVpc3Byte bIntMask_H;                     // 005H     Interrupt mask register high
         }sWrite;
      }sReg;

      TVpc3Byte bModeReg0_L;                          // 006H     mode register0 low
      TVpc3Byte bModeReg0_H;                          // 007H     mode register0 high

      union
      {
         struct
         {                                            //          [read]
            TVpc3Byte bDinBufferSm;                   // 008H     buffer assignment of the DP_DIN_BUFFER state machine
            TVpc3Byte bNewDinBufferCmd;               // 009H     the user makes a new DP_DIN_BUFFER available in the N state
            TVpc3Byte bDoutBufferSm;                  // 00AH     buffer assignment of the DP_DOUT_BUFFER state machine
            TVpc3Byte bNextDoutBufferCmd;             // 00BH     the user fetches the last DP_DOUT_BUFFER from the N state
            TVpc3Byte bDiagBufferSm;                  // 00CH     buffer assignment for DP_DIAG_BUFFER state machine
            TVpc3Byte bNewDiagBufferCmd;              // 00DH     the user makes a new DP_DIAG_BUFFER available to the VPC3+
            TVpc3Byte bUserPrmDataOkay;               // 00EH     positive acknowledge for received user parameter data
            TVpc3Byte bUserPrmDataNotOkay;            // 00FH     negative acknowledge for received user parameter data
            TVpc3Byte bUserCfgDataOkay;               // 010H     positive acknowledge for received config data
            TVpc3Byte bUserCfgDataNotOkay;            // 011H     negative acknowledge for received config data
            TVpc3Byte bDxbOutBufferSm;                // 012H     buffer assignment of the DXB_OUT_BUFFER state machine
            TVpc3Byte bNextDxbOutBufferCmd;           // 013H     the user fetches the last DXB_OUT_BUFFER
            TVpc3Byte bSsaBufferFreeCmd;              // 014H     the user has fetched data from ssa buffer and enables buffer again
            TVpc3Byte bModeReg1;                      // 015H     current value of mode_reg1
         } sRead;

         struct
         {                                            //          [write]
            TVpc3Byte bModeReg1_S;                    // 008H     set b0..b7 in mode_reg1
            TVpc3Byte bModeReg1_R;                    // 009H     reset b0..b7 in mode_reg1
            TVpc3Byte bWdBaudControlVal;              // 00AH     root value for baud rate monitoring
            TVpc3Byte bMinTsdrVal;                    // 00BH     MinTsdr time
            TVpc3Byte bModeReg2;                      // 00CH     set b0..b7 in mode_reg2
            TVpc3Byte bSyncPwReg;                     // 00DH     sync pulse width register
            TVpc3Byte bControlCommandReg;             // 00EH
            TVpc3Byte bGroupSelectReg;                // 00FH
            TVpc3Byte breserved_10;                   // 010H
            TVpc3Byte breserved_11;                   // 011H
            TVpc3Byte bModeReg3;                      // 012H     set b0..b7 in mode_reg2
            TVpc3Byte breserved_13;                   // 013H
            TVpc3Byte breserved_14;                   // 014H
            TVpc3Byte breserved_15;                   // 015H
         } sWrite;
      }sCtrlPrm;

      TVpc3Byte bTsAddr;                              // 016H     setup VPC3+ station address
      TVpc3Byte bFdlSapListPtr;                       // 017H     pointer fdl_sap_list
      TVpc3Byte abUserWdValue[2];                     // 018H     user watchdog value b0..b7
                                                      // 019H     user watchdog value b8..b15
      TVpc3Byte bLenDoutBuf;                          // 01AH     length of dout buffers
      TVpc3Byte abDoutBufPtr[3];                      // 01BH     segment base address of dout_buffer [0]
                                                      // 01CH     segment base address of dout_buffer [1]
                                                      // 01DH     segment base address of dout_buffer [2]
      TVpc3Byte bLenDinBuf;                           // 01EH     length of din buffers
      TVpc3Byte abDinBufPtr[3];                       // 01FH     segment base address of din_buffer [0]
                                                      // 020H     segment base address of din_buffer [1]
                                                      // 021H     segment base address of din_buffer [2]
      TVpc3Byte bLenDxbOutBuf;                        // 022H     length of dxb buffers
      TVpc3Byte bDxbOutBufPtr1;                       // 023H     segment base address of dxbout_buffer1
      TVpc3Byte abLenDiagBuf[2];                      // 024H     length of diag buffers [0]
                                                      // 025H     length of diag buffers [1]
      TVpc3Byte abDiagBufPtr[2];                      // 026H     segment base address of diag_buffer [0]
                                                      // 027H     segment base address of diag_buffer [1]
      TVpc3Byte abLenCtrlBuf[2];                      // 028H     length of aux buffer 1
                                                      // 029H     length of aux buffer 2
      TVpc3Byte bAuxBufSel;                           // 02AH     assignment for aux buffers 1/2
      TVpc3Byte abAuxBufPtr[2];                       // 02BH     segment base address of aux buffer 1
                                                      // 02CH     segment base address of aux buffer 2
      TVpc3Byte bLenSsaBuf;                           // 02DH     length of SET_SLAVE_ADDRESS buffer
      TVpc3Byte bSsaBufPtr;                           // 02EH     segment base address of SET_SLAVE_ADDRESS buffer
      TVpc3Byte bLenPrmData;                          // 02FH     max. length of input data in SET_PRM buffer
      TVpc3Byte bPrmBufPtr;                           // 030H     segment base address of SET_PRM buffer
      TVpc3Byte bLenCfgData;                          // 031H     length of input data in the CHECK_CONFIG buffer
      TVpc3Byte bCfgBufPtr;                           // 032H     segment base address of CHECK_CONFIG buffer
      TVpc3Byte bLenReadCfgData;                      // 033H     length of input data in the GET_CONFIG buffer
      TVpc3Byte bReadCfgBufPtr;                       // 034H     segment base address of GET_CONFIG buffer
      TVpc3Byte bLenDxbLinkBuf;                       // 035H     length of dxb link table buffer
      TVpc3Byte bDxbLinkBufPtr;                       // 036H     segment base address of dxb link table buffer
      TVpc3Byte bLenDxbStatusBuf;                     // 037H     length of dxb link status buffer
      TVpc3Byte bDxbStatusBufPtr;                     // 038H     segment base address of dxb link status buffer
      TVpc3Byte bRealNoAddChange;                     // 039H     address changes
      TVpc3Byte bIdentLow;                            // 03AH     IDENT_LOW
      TVpc3Byte bIdentHigh;                           // 03BH     IDENT_HIGH
      TVpc3Byte bGcCommand;                           // 03CH     last global control command
      TVpc3Byte bLenSpecPrmBuf;                       // 03DH     length of SPEC_PRM buffer
      TVpc3Byte bDxbOutBufPtr2;                       // 03EH     segment base address of dxbout_buffer2
      TVpc3Byte bDxbOutBufPtr3;                       // 03FH     segment base address of dxbout_buffer3

      TVpc3Byte abSapCtrlList[SAP_LENGTH];            // 040H     SAP CONTROL BLOCK LIST
   }  VPC3_STRUC;

   #define VPC3_STRUC_PTR  VPC3_STRUC MEM_PTR  //VPC3_PTR

   extern VPC3_STRUC_PTR   pVpc3;                     // structure for vpc3+

#else

   typedef struct
   {
                                                      // address  register
      #if MOTOROLA_MODE
                                                      // -------------------------------------------------------------------
         TVpc3Byte bIntReqReg_H;                      // 000H     Interrupt request register high
         TVpc3Byte bIntReqReg_L;                      // 001H     Interrupt request register low

         union
         {
            struct
            {                                         // [read]
               TVpc3Byte bIntReg_H;                   // 002H     Interrupt register high
               TVpc3Byte bIntReg_L;                   // 003H     Interrupt register low
               TVpc3Byte bStatus_H;                   // 004H     status register high
               TVpc3Byte bStatus_L;                   // 005H     status register low
            }sRead;

            struct
            {                                         // [write]
               TVpc3Byte bIntAck_H;                   // 002H     Interrupt acknowledge high
               TVpc3Byte bIntAck_L;                   // 003H     Interrupt acknowledge low
               TVpc3Byte bIntMask_H;                  // 004H     Interrupt mask register high
               TVpc3Byte bIntMask_L;                  // 005H     Interrupt mask register low
            }sWrite;
         }sReg;

         TVpc3Byte bModeReg0_H;                       // 006H     mode register0 high
         TVpc3Byte bModeReg0_L;                       // 007H     mode register0 low

      #else

         TVpc3Byte bIntReqReg_L;                      // 000H     Interrupt request register low
         TVpc3Byte bIntReqReg_H;                      // 001H     Interrupt request register high

         union
         {
             struct
             {                                        // [read]
                 TVpc3Byte bIntReg_L;                 // 002H     Interrupt register low
                 TVpc3Byte bIntReg_H;                 // 003H     Interrupt register high
                 TVpc3Byte bStatus_L;                 // 004H     status register low
                 TVpc3Byte bStatus_H;                 // 005H     status register high
             }sRead;

             struct
             {                                        // [write]
                 TVpc3Byte bIntAck_L;                 // 002H     Interrupt acknowledge low
                 TVpc3Byte bIntAck_H;                 // 003H     Interrupt acknowledge high
                 TVpc3Byte bIntMask_L;                // 004H     Interrupt mask register low
                 TVpc3Byte bIntMask_H;                // 005H     Interrupt mask register high
             }sWrite;
         }sReg;

         TVpc3Byte bModeReg0_L;                       // 006H     mode register0 low
         TVpc3Byte bModeReg0_H;                       // 007H     mode register0 high

      #endif//#if MOTOROLA_MODE

      union
      {
         struct
         {                                            //          [read]
            TVpc3Byte bDinBufferSm;                   // 008H     buffer assignment of the DP_DIN_BUFFER state machine
            TVpc3Byte bNewDinBufferCmd;               // 009H     the user makes a new DP_DIN_BUFFER available in the N state
            TVpc3Byte bDoutBufferSm;                  // 00AH     buffer assignment of the DP_DOUT_BUFFER state machine
            TVpc3Byte bNextDoutBufferCmd;             // 00BH     the user fetches the last DP_DOUT_BUFFER from the N state
            TVpc3Byte bDiagBufferSm;                  // 00CH     buffer assignment for DP_DIAG_BUFFER state machine
            TVpc3Byte bNewDiagBufferCmd;              // 00DH     the user makes a new DP_DIAG_BUFFER available to the VPC3+
            TVpc3Byte bUserPrmDataOkay;               // 00EH     positive acknowledge for received user parameter data
            TVpc3Byte bUserPrmDataNotOkay;            // 00FH     negative acknowledge for received user parameter data
            TVpc3Byte bUserCfgDataOkay;               // 010H     positive acknowledge for received config data
            TVpc3Byte bUserCfgDataNotOkay;            // 011H     negative acknowledge for received config data
            TVpc3Byte bDxbOutBufferSm;                // 012H     buffer assignment of the DXB_OUT_BUFFER state machine
            TVpc3Byte bNextDxbOutBufferCmd;           // 013H     the user fetches the last DXB_OUT_BUFFER
            TVpc3Byte bSsaBufferFreeCmd;              // 014H     the user has fetched data from ssa buffer and enables buffer again
            TVpc3Byte bModeReg1;                      // 015H     current value of mode_reg1
         }  sRead;

         struct
         {                                            //          [write]
            TVpc3Byte bModeReg1_S;                    // 008H     set b0..b7 in mode_reg1
            TVpc3Byte bModeReg1_R;                    // 009H     reset b0..b7 in mode_reg1
            TVpc3Byte bWdBaudControlVal;              // 00AH     root value for baud rate monitoring
            TVpc3Byte bMinTsdrVal;                    // 00BH     MinTsdr time
            TVpc3Byte bModeReg2;                      // 00CH     set b0..b7 in mode_reg2
            TVpc3Byte bSyncPwReg;                     // 00DH     sync pulse width register
            TVpc3Byte bControlCommandReg;             // 00EH
            TVpc3Byte bGroupSelectReg;                // 00FH
            TVpc3Byte breserved_10;                   // 010H
            TVpc3Byte breserved_11;                   // 011H
            TVpc3Byte bModeReg3;                      // 012H     set b0..b7 in mode_reg2
            TVpc3Byte breserved_13;                   // 013H
            TVpc3Byte breserved_14;                   // 014H
            TVpc3Byte breserved_15;                   // 015H
         }  sWrite;
      }  sCtrlPrm;

      TVpc3Byte bTsAddr;                              // 016H     setup VPC3+ station address
      TVpc3Byte bFdlSapListPtr;                       // 017H     pointer fdl_sap_list
      TVpc3Byte abUserWdValue[2];                     // 018H     user watchdog value b0..b7
                                                      // 019H     user watchdog value b8..b15
      TVpc3Byte bLenDoutBuf;                          // 01AH     length of dout buffers
      TVpc3Byte abDoutBufPtr[3];                      // 01BH     segment base address of dout_buffer [0]
                                                      // 01CH     segment base address of dout_buffer [1]
                                                      // 01DH     segment base address of dout_buffer [2]
      TVpc3Byte bLenDinBuf;                           // 01EH     length of din buffers
      TVpc3Byte abDinBufPtr[3];                       // 01FH     segment base address of din_buffer [0]
                                                      // 020H     segment base address of din_buffer [1]
                                                      // 021H     segment base address of din_buffer [2]
      TVpc3Byte bLenDxbOutBuf;                        // 022H     length of dxb buffers
      TVpc3Byte bDxbOutBufPtr1;                       // 023H     segment base address of dxbout_buffer1
      TVpc3Byte abLenDiagBuf[2];                      // 024H     length of diag buffers [0]
                                                      // 025H     length of diag buffers [1]
      TVpc3Byte abDiagBufPtr[2];                      // 026H     segment base address of diag_buffer [0]
                                                      // 027H     segment base address of diag_buffer [1]
      TVpc3Byte abLenCtrlBuf[2];                      // 028H     length of aux buffer 1
                                                      // 029H     length of aux buffer 2
      TVpc3Byte bAuxBufSel;                           // 02AH     assignment for aux buffers 1/2
      TVpc3Byte abAuxBufPtr[2];                       // 02BH     segment base address of aux buffer 1
                                                      // 02CH     segment base address of aux buffer 2
      TVpc3Byte bLenSsaBuf;                           // 02DH     length of SET_SLAVE_ADDRESS buffer
      TVpc3Byte bSsaBufPtr;                           // 02EH     segment base address of SET_SLAVE_ADDRESS buffer
      TVpc3Byte bLenPrmData;                          // 02FH     max. length of input data in SET_PRM buffer
      TVpc3Byte bPrmBufPtr;                           // 030H     segment base address of SET_PRM buffer
      TVpc3Byte bLenCfgData;                          // 031H     length of input data in the CHECK_CONFIG buffer
      TVpc3Byte bCfgBufPtr;                           // 032H     segment base address of CHECK_CONFIG buffer
      TVpc3Byte bLenReadCfgData;                      // 033H     length of input data in the GET_CONFIG buffer
      TVpc3Byte bReadCfgBufPtr;                       // 034H     segment base address of GET_CONFIG buffer
      TVpc3Byte bLenDxbLinkBuf;                       // 035H     length of dxb link table buffer
      TVpc3Byte bDxbLinkBufPtr;                       // 036H     segment base address of dxb link table buffer
      TVpc3Byte bLenDxbStatusBuf;                     // 037H     length of dxb link status buffer
      TVpc3Byte bDxbStatusBufPtr;                     // 038H     segment base address of dxb link status buffer
      TVpc3Byte bRealNoAddChange;                     // 039H     address changes
      TVpc3Byte bIdentLow;                            // 03AH     IDENT_LOW
      TVpc3Byte bIdentHigh;                           // 03BH     IDENT_HIGH
      TVpc3Byte bGcCommand;                           // 03CH     last global control command
      TVpc3Byte bLenSpecPrmBuf;                       // 03DH     length of SPEC_PRM buffer
      TVpc3Byte bDxbOutBufPtr2;                       // 03EH     segment base address of dxbout_buffer2
      TVpc3Byte bDxbOutBufPtr3;                       // 03FH     segment base address of dxbout_buffer3

      TVpc3Byte abSapCtrlList[SAP_LENGTH];            // 040H     SAP CONTROL BLOCK LIST

      TVpc3Byte abDpBuffer[ASIC_USER_RAM];            // VPC3: 040H...7F0H: ram area for dp buffers
                                                      // SPC3: 040H...5F0H
      #ifdef ISOCHRON_MODE
         struct
         {
            UBYTE bStatus;                            //7C0, FC0
            UBYTE bCommand;
            UWORD wWindow;
            UWORD wDelayTime;
            ULONG dwSyncCycleTime;
            UWORD wNrOfSync;
            ULONG dwFirstWindow;
            UWORD wInputTime;
            UWORD wOutputTime;
            UBYTE bElimit;
         }  sHwPLL;

         TVpc3Byte abReservedHwPll[12];
      #endif//#ifdef ISOCHRON_MODE

      #ifdef DP_TIMESTAMP
         struct
         {
            TVpc3Byte bStatus;                        // 7E0H, FE0H
            TVpc3Byte bCommand;
            TVpc3Byte bCvStatus1;
            TVpc3Byte bCvStatus2;
            TVpc3Byte abCvTimeEvent[8];
            TVpc3Byte abRecDelTime[4];
            TVpc3Byte abCvPrevTimeEvent[8];           // 7F0H
            TVpc3Byte abCsInterval[2];                // 7F8H
         }  sCs;

         TVpc3Byte abReserved[6];                     // 7FAH
      #endif//#ifdef DP_TIMESTAMP
   }  VPC3_STRUC;

   #define VPC3_STRUC_PTR  VPC3_STRUC VPC3_PTR

   extern VPC3_STRUC_PTR   pVpc3;            // structure for vpc3+
   extern VPC3_ADR         Vpc3AsicAddress;  // global asic address

#endif//#if VPC3_SERIAL_MODE

/*---------------------------------------------------------------------------*/
/* 1.2 literals for interrupt register                                       */
/*---------------------------------------------------------------------------*/
//low Byte
#define VPC3_INT_CLOCK_SYNC             ((UBYTE)0x01)       // VPC3 received clock sync telegram
#define VPC3_INT_GO_LEAVE_DATA_EX       ((UBYTE)0x02)       // VPC3 has entered or left DATA_EX
#define VPC3_INT_BAUDRATE_DETECT        ((UBYTE)0x04)       // VPC3 has detected Baudrate
#define VPC3_INT_WD_DP_MODE_TIMEOUT     ((UBYTE)0x08)       // DP watchdog timeout
#define VPC3_INT_USER_TIMER_CLOCK       ((UBYTE)0x10)       // time base for USER_TIMER_CLOCKS has run out
#define VPC3_INT_DXB_LINK_ERROR         ((UBYTE)0x20)       // subscriber link error
#define VPC3_INT_NEW_EXT_PRM_DATA       ((UBYTE)0x40)       // New EXT_PRM_DATA received
#define VPC3_INT_DXB_OUT                ((UBYTE)0x80)       // NEW subscriber data received

//high Byte
#define VPC3_INT_NEW_GC_COMMAND         ((UBYTE)0x01)       // New Global control command received
#define VPC3_INT_NEW_SSA_DATA           ((UBYTE)0x02)       // New SSA_DATA received
#define VPC3_INT_NEW_CFG_DATA           ((UBYTE)0x04)       // New check_config received
#define VPC3_INT_NEW_PRM_DATA           ((UBYTE)0x08)       // New prm_data received
#define VPC3_INT_DIAG_BUFFER_CHANGED    ((UBYTE)0x10)       // diag_buffer has been changed
#define VPC3_INT_DX_OUT                 ((UBYTE)0x20)       // New DOut_data received
#define VPC3_INT_POLL_END_IND           ((UBYTE)0x40)       // DPV1
#define VPC3_INT_FDL_IND                ((UBYTE)0x80)       // DPV1

// -- interrupt events for 16bit register
#define VPC3_INT16_CLOCK_SYNC           ((UWORD)0x0001)     // VPC3 received clock sync telegram
#define VPC3_INT16_GO_LEAVE_DATA_EX     ((UWORD)0x0002)     // VPC3 has entered or left DATA_EX
#define VPC3_INT16_BAUDRATE_DETECT      ((UWORD)0x0004)     // VPC3 has detected Baudrate
#define VPC3_INT16_WD_DP_MODE_TIMEOUT   ((UWORD)0x0008)     // DP watchdog timeout
#define VPC3_INT16_USER_TIMER_CLOCK     ((UWORD)0x0010)     // time base for USER_TIMER_CLOCKS has run out
#define VPC3_INT16_DXB_LINK_ERROR       ((UWORD)0x0020)     // subscriber link error
#define VPC3_INT16_NEW_EXT_PRM_DATA     ((UWORD)0x0040)     // New EXT_PRM_DATA received
#define VPC3_INT16_DXB_OUT              ((UWORD)0x0080)     // NEW subscriber data received
#define VPC3_INT16_NEW_GC_COMMAND       ((UWORD)0x0100)     // New Global control command received
#define VPC3_INT16_NEW_SSA_DATA         ((UWORD)0x0200)     // New SSA_DATA received
#define VPC3_INT16_NEW_CFG_DATA         ((UWORD)0x0400)     // New check_config received
#define VPC3_INT16_NEW_PRM_DATA         ((UWORD)0x0800)     // New prm_data received
#define VPC3_INT16_DIAG_BUFFER_CHANGED  ((UWORD)0x1000)     // diag_buffer has been changed
#define VPC3_INT16_DX_OUT               ((UWORD)0x2000)     // New DOut_data received
#define VPC3_INT16_POLL_END_IND         ((UWORD)0x4000)     // DPV1
#define VPC3_INT16_FDL_IND              ((UWORD)0x8000)     // DPV1

// -- events for interrupt event handler --------------------------------------
#define VPC3_EV_CLOCK_SYNCHRONISATION   ((UWORD)0x0001)
#define VPC3_EV_MAC_RESET               ((UWORD)0x0001)
#define VPC3_EV_GO_LEAVE_DATA_EX        ((UWORD)0x0002)
#define VPC3_EV_BAUDRATE_DETECT         ((UWORD)0x0004)
#define VPC3_EV_WD_DP_TIMEOUT           ((UWORD)0x0008)
#define VPC3_EV_USER_TIMER_CLOCK        ((UWORD)0x0010)
#define VPC3_EV_DXB_LINK_ERROR          ((UWORD)0x0020)
#define VPC3_EV_NEW_EXT_PRM_DATA        ((UWORD)0x0040)
#define VPC3_EV_DXB_OUT                 ((UWORD)0x0080)
#define VPC3_EV_NEW_GC_COMMAND          ((UWORD)0x0100)
#define VPC3_EV_NEW_SSA_DATA            ((UWORD)0x0200)
#define VPC3_EV_NEW_CFG_DATA            ((UWORD)0x0400)
#define VPC3_EV_NEW_PRM_DATA            ((UWORD)0x0800)
#define VPC3_EV_DIAG_BUF_CHANGED        ((UWORD)0x1000)
#define VPC3_EV_DX_OUT                  ((UWORD)0x2000)
#define VPC3_EV_RESERVED                ((UWORD)0x4000)
#define VPC3_EV_NEW_INPUT_DATA          ((UWORD)0x8000)

/*---------------------------------------------------------------------------*/
/* 1.3 literals for status register                                          */
/*---------------------------------------------------------------------------*/
#define VPC3_PASS_IDLE                  ((UBYTE)0x01)
#define VPC3_DIAG_FLAG                  ((UBYTE)0x04)
#define WAIT_PRM                        ((UBYTE)0x00)
#define WAIT_CFG                        ((UBYTE)0x10)
#define DATA_EX                         ((UBYTE)0x20)
#define DP_ERROR                        ((UBYTE)0x30)
#define BAUD_SEARCH                     ((UBYTE)0x00)
#define BAUD_CONTROL                    ((UBYTE)0x40)
#define DP_MODE                         ((UBYTE)0x80)
#define WD_ERROR                        ((UBYTE)0xC0)

#define BAUDRATE_MASK                   ((UBYTE)0x0F)
#define BAUDRATE_12MBAUD                ((UBYTE)0x00)
#define BAUDRATE_6MBAUD                 ((UBYTE)0x01)
#define BAUDRATE_3MBAUD                 ((UBYTE)0x02)
#define BAUDRATE_1_5MBAUD               ((UBYTE)0x03)
#define BAUDRATE_500KBAUD               ((UBYTE)0x04)
#define BAUDRATE_187_50KBAUD            ((UBYTE)0x05)
#define BAUDRATE_93_75KBAUD             ((UBYTE)0x06)
#define BAUDRATE_45_45KBAUD             ((UBYTE)0x07)
#define BAUDRATE_19_20KBAUD             ((UBYTE)0x08)
#define BAUDRATE_9_60KBAUD              ((UBYTE)0x09)
#define BAUDRATE_AFTER_RESET            ((UBYTE)0xFF)

#define AT_MASK                         ((UBYTE)0xF0)
#define AT_VPC3                         ((UBYTE)0x00)
#define AT_VPC3B                        ((UBYTE)0xB0)
#define AT_VPC3C                        ((UBYTE)0xC0)
#define AT_MPI12X                       ((UBYTE)0xD0)
#define AT_VPC3S                        ((UBYTE)0xE0)

/*---------------------------------------------------------------------------*/
/* 1.4 literals for mode register 0                                          */
/*---------------------------------------------------------------------------*/
//low Byte
#define VPC3_DIS_START_CTRL             ((UBYTE)0x01)
#define VPC3_DIS_STOP_CTRL              ((UBYTE)0x02)
#define VPC3_FDL_DBB                    ((UBYTE)0x04)
#define VPC3_MINTSDR                    ((UBYTE)0x08)
#define VPC3_INT_POL_HIGH               ((UBYTE)0x10)
#define VPC3_EARLY_RDY                  ((UBYTE)0x20)
#define VPC3_SYNC_SUPP                  ((UBYTE)0x40)
#define VPC3_FREEZE_SUPP                ((UBYTE)0x80)

//high Byte
#define VPC3_DP_MODE                    ((UBYTE)0x01)
#define VPC3_EOI_TBASE_1ms              ((UBYTE)0x02)
#define VPC3_USR_TBASE_10ms             ((UBYTE)0x04)
#define VPC3_WD_TEST                    ((UBYTE)0x08)
#define VPC3_SPEC_PRM_BUF_MODE          ((UBYTE)0x10)
#define VPC3_SPEC_CLR_MODE              ((UBYTE)0x20)

/*---------------------------------------------------------------------------*/
/* 1.5 literals for mode register 1                                          */
/*---------------------------------------------------------------------------*/
#define VPC3_START                      ((UBYTE)0x01)
#define VPC3_EOI                        ((UBYTE)0x02)
#define VPC3_GO_OFFLINE                 ((UBYTE)0x04)
#define VPC3_USER_LEAVE_MASTER          ((UBYTE)0x08)
#define VPC3_EN_CHG_CFG_BUFFER          ((UBYTE)0x10)
#define VPC3_RES_USER_WD                ((UBYTE)0x20)
#define VPC3_EN_CHK_SSAP                ((UBYTE)0x80)

/*---------------------------------------------------------------------------*/
/* 1.6 literals for mode register 2                                          */
/*---------------------------------------------------------------------------*/
#define VPC3_GC_INT_MODE                ((UBYTE)0x01)
#define VPC3_NO_CHECK_GC_RESERVED       ((UBYTE)0x02)
#define VPC3_DX_INT_MODE                ((UBYTE)0x04)
#define VPC3_DX_INT_PORT                ((UBYTE)0x08)
#define VPC3_SYNC_ENA                   ((UBYTE)0x10)
#define VPC3_SYNC_POL                   ((UBYTE)0x20)
#define VPC3_NO_CHECK_PRM_RESERVED      ((UBYTE)0x40)
#define VPC3_4KB_MODE                   ((UBYTE)0x80)

/*---------------------------------------------------------------------------*/
/* 1.7 basic macros for VPC3+                                                */
/*---------------------------------------------------------------------------*/
#if VPC3_SERIAL_MODE

   // Startaddress of User-Area in VPC3-Format -----------------------------------
   #define VPC3_DP_BUF_START                          (UBYTE)( sizeof( VPC3_STRUC ) >> SEG_MULDIV)
   // Startaddress of FDL-List-Area in VPC3-Format -------------------------------
   #define VPC3_SAP_CTRL_LIST_START                   (UBYTE)(0x40 >> SEG_MULDIV)
   // set HW-Mode ----------------------------------------------------------------
   #define VPC3_SET_HW_MODE_HIGH(MODE_HIGH)           Vpc3Write( bVpc3RwModeReg0_L, MODE_HIGH )
   #define VPC3_SET_HW_MODE_LOW(MODE_LOW)             Vpc3Write( bVpc3RwModeReg0_L, MODE_LOW )
   // Get HW-Mode ----------------------------------------------------------------
   #define VPC3_GET_HW_MODE_HIGH()                    Vpc3Read( bVpc3RwModeReg0_H )
   #define VPC3_GET_HW_MODE_LOW()                     Vpc3Read( bVpc3RwModeReg0_L )
   // set interrupt indications --------------------------------------------------
   #define VPC3_SET_INT_REQ_REG_L(ISR_EVENT)          Vpc3Write( bVpc3RwIntReqReg_L, ISR_EVENT )
   #define VPC3_SET_INDICATIONS_HIGH(IND_HIGH)        Vpc3Write( bVpc3WoIntMask_H, IND_HIGH )
   #define VPC3_SET_INDICATIONS_LOW(IND_LOW)          Vpc3Write( bVpc3WoIntMask_L, IND_LOW )
   // -- Set Slave-Adresse -------------------------------------------------------
   #define VPC3_SET_STATION_ADDRESS(ADDRESS)          Vpc3Write( bVpc3RwTsAddr, ADDRESS )
   #define VPC3_GET_STATION_ADDRESS()                 Vpc3Read( bVpc3RwTsAddr )
   // -- Set min_TSDR ------------------------------------------------------------
   #define VPC3_SET_MINTSDR(MINTSDR)                  Vpc3Write( bVpc3WoMinTsdrVal, MINTSDR )
   // -- Set Baud Control --------------------------------------------------------
   #define VPC3_SET_BAUD_CNTRL(VALUE)                 Vpc3Write( bVpc3WoWdBaudControlVal, VALUE )
   //-- Ident-Nummer -------------------------------------------------------------
   //-- Set Ident-Number ---------------------------------------------------------
   #define VPC3_SET_IDENT_NUMBER_LOW(NR)              Vpc3Write( bVpc3RwIdentLow, NR )
   #define VPC3_SET_IDENT_NUMBER_HIGH(NR)             Vpc3Write( bVpc3RwIdentHigh, NR )
   // -- address change service --------------------------------------------------
   #define VPC3_SET_ADD_CHG_DISABLE()                 Vpc3Write( bVpc3RwRealNoAddChange, 0xFF )
   #define VPC3_SET_ADD_CHG_ENABLE()                  Vpc3Write( bVpc3RwRealNoAddChange, 0x00 )
   //-- start VPC3 ---------------------------------------------------------------
   #define VPC3_Start__()                             Vpc3Write( bVpc3WoModeReg1_S, VPC3_START )
   #define VPC3_ResetStart()                          Vpc3Write( bVpc3WoModeReg1_R, VPC3_START )
   //-- set VPC3 offline ---------------------------------------------------------
   #define VPC3_GoOffline()                           Vpc3Write( bVpc3WoModeReg1_S, VPC3_GO_OFFLINE )
   // -- read/write Mode register ------------------------------------------------
   #define VPC3_GET_MODE_REG_1()                      Vpc3Read( bVpc3RoModeReg1 )
   #define VPC3_SET_MODE_REG_2(bValue)                Vpc3Write( bVpc3WoModeReg2, bValue )
   // -- read Status register ----------------------------------------------------
   #define VPC3_GET_STATUS_L()                        Vpc3Read( bVpc3RoStatus_L )
   #define VPC3_GET_STATUS_H()                        Vpc3Read( bVpc3RoStatus_H )
   // -- read State of DP-State Machine ------------------------------------------
   #define VPC3_GET_DP_STATE()                        ( Vpc3Read( bVpc3RoStatus_L ) & 0x30 )
   // -- read WD-State Machine ---------------------------------------------------
   #define VPC3_GET_WD_STATE()                        ( Vpc3Read( bVpc3RoStatus_L ) & 0xC0)
   // -- read Baud-Rate ----------------------------------------------------------
   #define VPC3_GET_BAUDRATE()                        ( Vpc3Read( bVpc3RoStatus_H ) & BAUDRATE_MASK )
   // -- read ASIC-Type ----------------------------------------------------------
   #define VPC3_GET_ASIC_TYPE()                       ( Vpc3Read( bVpc3RoStatus_H ) & AT_MASK )
   // -- read VPC3-Offline/Passiv-Idle-State -------------------------------------
   #define VPC3_GET_OFF_PASS()                        ( Vpc3Read( bVpc3RoStatus_L ) & VPC3_PASS_IDLE )
   // -- set DP StateMachine to WAIT_PRM -----------------------------------------
   #define VPC3_SET_USER_LEAVE_MASTER()               Vpc3Write( bVpc3WoModeReg1_S, VPC3_USER_LEAVE_MASTER )
   // -- User-Watchdog -----------------------------------------------------------
   #define VPC3_SET_USER_WD_VALUE_HIGH(VALUE_HIGH)    Vpc3Write( bVpc3RwUserWdValue_H, VALUE_HIGH )
   #define VPC3_SET_USER_WD_VALUE_LOW(VALUE_LOW)      Vpc3Write( bVpc3RwUserWdValue_L, VALUE_LOW )
   // -- Reset User-Watchdog -----------------------------------------------------
   #define VPC3_RESET_USER_WD()                       Vpc3Write( bVpc3WoModeReg1_S, VPC3_RES_USER_WD )
   //-- VPC3_SET_EN_CHK_SSAP -----------------------------------------------------
   #define VPC3_SET_EN_CHK_SSAP()                     Vpc3Write( bModeReg1_S VPC3_EXTENSION = VPC3_EN_CHK_SSAP )
   #define VPC3_RESET_EN_CHK_SSAP()                   Vpc3Write( bModeReg1_R VPC3_EXTENSION = VPC3_EN_CHK_SSAP )

   #define VPC3_GET_DOUT_BUFFER_SM()                  ( Vpc3Read( bVpc3RoDoutBufferSm ) & 0x30 )
   #define VPC3_GET_DIN_BUFFER_SM()                   ( Vpc3Read( bVpc3RoDinBufferSm ) & 0x30 )
   #define VPC3_GET_NEXT_DOUT_BUFFER_CMD()            ( Vpc3Read( bVpc3RoNextDoutBufferCmd ) & ( VPC3_NEW_DOUT_BUF | VPC3_DOUT_BUF_CLEARED ) )

   #define VPC3_GET_DIAG_BUFFER_SM()                  ( Vpc3Read( bVpc3RoDiagBufferSm ) )
   #define VPC3_GET_NEW_DIAG_BUFFER_CMD()             ( Vpc3Read( bVpc3RoNewDiagBufferCmd ) )
   #define VPC3_GET_DIAG1_LENGTH()                    ( Vpc3Read( bVpc3RwLenDiagBuf1 ) )
   #define VPC3_GET_DIAG2_LENGTH()                    ( Vpc3Read( bVpc3RwLenDiagBuf2 ) )
   #define VPC3_GET_DIAG1_PTR()                       (VPC3_UNSIGNED8_PTR)(((VPC3_ADR)( Vpc3Read( bVpc3RwDiagBufPtr1 )) << SEG_MULDIV)+((VPC3_ADR)Vpc3AsicAddress))
   #define VPC3_GET_DIAG2_PTR()                       (VPC3_UNSIGNED8_PTR)(((VPC3_ADR)( Vpc3Read( bVpc3RwDiagBufPtr2 )) << SEG_MULDIV)+((VPC3_ADR)Vpc3AsicAddress))

#else

   // Startaddress of User-Area in VPC3-Format -----------------------------------
   #define VPC3_DP_BUF_START                          (UBYTE)(((VPC3_ADR)&pVpc3->abDpBuffer[0] VPC3_EXTENSION - (VPC3_ADR)Vpc3AsicAddress) >> SEG_MULDIV)
   // Startaddress of FDL-List-Area in VPC3-Format -------------------------------
   #define VPC3_SAP_CTRL_LIST_START                   (UBYTE)(((VPC3_ADR)&pVpc3->abSapCtrlList - (VPC3_ADR)Vpc3AsicAddress) >> SEG_MULDIV)
   // set HW-Mode ----------------------------------------------------------------
   #define VPC3_SET_HW_MODE_HIGH(MODE_HIGH)           pVpc3->bModeReg0_H VPC3_EXTENSION = MODE_HIGH
   #define VPC3_SET_HW_MODE_LOW(MODE_LOW)             pVpc3->bModeReg0_L VPC3_EXTENSION = MODE_LOW
   // Get HW-Mode ----------------------------------------------------------------
   #define VPC3_GET_HW_MODE_HIGH()                    pVpc3->bModeReg0_H VPC3_EXTENSION
   #define VPC3_GET_HW_MODE_LOW()                     pVpc3->bModeReg0_L VPC3_EXTENSION
   // set interrupt indications --------------------------------------------------
   #define VPC3_SET_INT_REQ_REG_L(ISR_EVENT)          pVpc3->bIntReqReg_L VPC3_EXTENSION = ISR_EVENT
   #define VPC3_SET_INDICATIONS_HIGH(IND_HIGH)        pVpc3->sReg.sWrite.bIntMask_H VPC3_EXTENSION = IND_HIGH
   #define VPC3_SET_INDICATIONS_LOW(IND_LOW)          pVpc3->sReg.sWrite.bIntMask_L VPC3_EXTENSION = IND_LOW
   // -- Set Slave-Adresse -------------------------------------------------------
   #define VPC3_SET_STATION_ADDRESS(ADDRESS)          pVpc3->bTsAddr VPC3_EXTENSION = ADDRESS
   #define VPC3_GET_STATION_ADDRESS()                 pVpc3->bTsAddr VPC3_EXTENSION
   // -- Set min_TSDR ------------------------------------------------------------
   #define VPC3_SET_MINTSDR(MINTSDR)                  pVpc3->sCtrlPrm.sWrite.bMinTsdrVal VPC3_EXTENSION = MINTSDR
   // -- Set Baud Control --------------------------------------------------------
   #define VPC3_SET_BAUD_CNTRL(VALUE)                 pVpc3->sCtrlPrm.sWrite.bWdBaudControlVal VPC3_EXTENSION = VALUE
   //-- Ident-Nummer -------------------------------------------------------------
   //-- Set Ident-Number ---------------------------------------------------------
   #define VPC3_SET_IDENT_NUMBER_LOW(NR)              pVpc3->bIdentLow VPC3_EXTENSION  = NR
   #define VPC3_SET_IDENT_NUMBER_HIGH(NR)             pVpc3->bIdentHigh VPC3_EXTENSION = NR
   // -- address change service --------------------------------------------------
   #define VPC3_SET_ADD_CHG_DISABLE()                 pVpc3->bRealNoAddChange VPC3_EXTENSION = 0xFF
   #define VPC3_SET_ADD_CHG_ENABLE()                  pVpc3->bRealNoAddChange VPC3_EXTENSION = 0x00
   //-- start VPC3 ---------------------------------------------------------------
   #define VPC3_Start__()                             pVpc3->sCtrlPrm.sWrite.bModeReg1_S VPC3_EXTENSION = VPC3_START
   #define VPC3_ResetStart()                          pVpc3->sCtrlPrm.sWrite.bModeReg1_R VPC3_EXTENSION = VPC3_START
   //-- set VPC3 offline ---------------------------------------------------------
   /*!
      \def VPC3_GoOffline()
   
      \brief Go to the offline state.
   */
   #define VPC3_GoOffline()                           pVpc3->sCtrlPrm.sWrite.bModeReg1_S VPC3_EXTENSION = VPC3_GO_OFFLINE
   // -- read/write Mode register ------------------------------------------------
   #define VPC3_GET_MODE_REG_1()                      pVpc3->sCtrlPrm.sRead.bModeReg1 VPC3_EXTENSION
   #define VPC3_SET_MODE_REG_2(bValue)                pVpc3->sCtrlPrm.sWrite.bModeReg2 VPC3_EXTENSION = bValue
   // -- read Status register ----------------------------------------------------
   #define VPC3_GET_STATUS_L()                        pVpc3->sReg.sRead.bStatus_L VPC3_EXTENSION
   #define VPC3_GET_STATUS_H()                        pVpc3->sReg.sRead.bStatus_H VPC3_EXTENSION
   // -- read State of DP-State Machine ------------------------------------------
   /*!
      \def VPC3_GET_DP_STATE()
      
      \brief Fetching the status of the PROFIBUS DP state machine.
      
      With the macro VPC3_GET_DP_STATE(), the application is informed whether 
      the VPC3+ has entered the data exchange state or left it.
      
      \retval WAIT_PRM - Wait for parameter assignment
      \retval WAIT_CFG - Wait for configuration
      \retval DATA_EX - Data Exchange
      \retval DP_ERROR - Error
   */
   #define VPC3_GET_DP_STATE()                        (pVpc3->sReg.sRead.bStatus_L VPC3_EXTENSION & 0x30)
   // -- read WD-State Machine ---------------------------------------------------
   /*!
      \def VPC3_GET_WD_STATE()
      
      \brief Fetching the status of the watchdog state machine.
      
      \retval BAUD_SEARCH - Baudrate search
      \retval BAUD_CONTROL - Checking the baudrate
      \retval DP_MODE - DP_MODE, bus watchdog is activated
      \retval WD_ERROR - Error
   */
   #define VPC3_GET_WD_STATE()                        (pVpc3->sReg.sRead.bStatus_L VPC3_EXTENSION & 0xC0)
   // -- read Baud-Rate ----------------------------------------------------------
   /*!
      \def VPC3_GET_BAUDRATE()
      
      \brief Read baud rate.
      
      \retval BAUDRATE_12MBAUD - 12MBaud      
      \retval BAUDRATE_6MBAUD - 6MBaud        
      \retval BAUDRATE_3MBAUD - 3MBaud       
      \retval BAUDRATE_1_5MBAUD - 1.5MBaud      
      \retval BAUDRATE_500KBAUD - 500KBaud      
      \retval BAUDRATE_187_50KBAUD - 187.5 KBaud   
      \retval BAUDRATE_93_75KBAUD - 93.75 KBaud   
      \retval BAUDRATE_45_45KBAUD - 45.45 KBaud   
      \retval BAUDRATE_19_20KBAUD - 19.2 KBaud       
      \retval BAUDRATE_9_60KBAUD - 9.6KBaud    
   */
   #define VPC3_GET_BAUDRATE()                        (pVpc3->sReg.sRead.bStatus_H VPC3_EXTENSION & BAUDRATE_MASK)
   // -- read ASIC-Type ----------------------------------------------------------
   /*!
      \def VPC3_GET_ASIC_TYPE()
      
      \brief Read asic type.
      
      \retval AT_VPC3    
      \retval AT_VPC3B   
      \retval AT_VPC3C   
      \retval AT_MPI12X  
      \retval AT_VPC3S   
   */
   #define VPC3_GET_ASIC_TYPE()                       (pVpc3->sReg.sRead.bStatus_H VPC3_EXTENSION & AT_MASK)
   // -- read VPC3-Offline/Passiv-Idle-State -------------------------------------
   /*!
      \def VPC3_GET_OFF_PASS()
      
      \brief Check the offline state.
      
      \retval 0 - VPC3+ is in offline state      
      \retval 1 - VPC3+ is in passive idle state        
   */
   #define VPC3_GET_OFF_PASS()                        (pVpc3->sReg.sRead.bStatus_L VPC3_EXTENSION & VPC3_PASS_IDLE)
   // -- set DP StateMachine to WAIT_PRM -----------------------------------------
   /*!
      \def VPC3_SET_USER_LEAVE_MASTER()
      \brief Enter the state Wait_Prm.
      
      The VPC3_SET_USER_LEAVE_MASTER() macro causes the VPC3+ to change into the "Wait_Prm" state. 
   */
   #define VPC3_SET_USER_LEAVE_MASTER()               pVpc3->sCtrlPrm.sWrite.bModeReg1_S VPC3_EXTENSION = VPC3_USER_LEAVE_MASTER
   // -- User-Watchdog -----------------------------------------------------------
   #define VPC3_SET_USER_WD_VALUE_HIGH(VALUE_HIGH)    pVpc3->abUserWdValue[1] VPC3_EXTENSION = VALUE_HIGH
   #define VPC3_SET_USER_WD_VALUE_LOW(VALUE_LOW)      pVpc3->abUserWdValue[0] VPC3_EXTENSION = VALUE_LOW
   // -- Reset User-Watchdog -----------------------------------------------------
   #define VPC3_RESET_USER_WD()                       pVpc3->sCtrlPrm.sWrite.bModeReg1_S VPC3_EXTENSION = VPC3_RES_USER_WD
   //-- VPC3_SET_EN_CHK_SSAP -----------------------------------------------------
   #define VPC3_SET_EN_CHK_SSAP()                     pVpc3->sCtrlPrm.sWrite.bModeReg1_S VPC3_EXTENSION = VPC3_EN_CHK_SSAP
   #define VPC3_RESET_EN_CHK_SSAP()                   pVpc3->sCtrlPrm.sWrite.bModeReg1_R VPC3_EXTENSION = VPC3_EN_CHK_SSAP

   #define VPC3_GET_DOUT_BUFFER_SM()                  ( pVpc3->sCtrlPrm.sRead.bDoutBufferSm VPC3_EXTENSION & 0x30 )
   #define VPC3_GET_DIN_BUFFER_SM()                   ( pVpc3->sCtrlPrm.sRead.bDinBufferSm VPC3_EXTENSION & 0x30 )
   #define VPC3_GET_NEXT_DOUT_BUFFER_CMD()            ( pVpc3->sCtrlPrm.sRead.bNextDoutBufferCmd VPC3_EXTENSION & ( VPC3_NEW_DOUT_BUF | VPC3_DOUT_BUF_CLEARED ) )

   #define VPC3_GET_DIAG_BUFFER_SM()                  ( pVpc3->sCtrlPrm.sRead.bDiagBufferSm VPC3_EXTENSION )
   #define VPC3_GET_NEW_DIAG_BUFFER_CMD()             ( pVpc3->sCtrlPrm.sRead.bNewDiagBufferCmd VPC3_EXTENSION )
   #define VPC3_GET_DIAG1_LENGTH()                    ( pVpc3->abLenDiagBuf[0] VPC3_EXTENSION )
   #define VPC3_GET_DIAG2_LENGTH()                    ( pVpc3->abLenDiagBuf[1] VPC3_EXTENSION )
   #define VPC3_GET_DIAG1_PTR()                       (VPC3_UNSIGNED8_PTR) (((VPC3_ADR)(pVpc3->abDiagBufPtr[0] VPC3_EXTENSION) << SEG_MULDIV)+((VPC3_ADR)Vpc3AsicAddress))
   #define VPC3_GET_DIAG2_PTR()                       (VPC3_UNSIGNED8_PTR) (((VPC3_ADR)(pVpc3->abDiagBufPtr[1] VPC3_EXTENSION) << SEG_MULDIV)+((VPC3_ADR)Vpc3AsicAddress))

   #define VPC3_GET_INT_REQ_REG_H()                   pVpc3->bIntReqReg_H VPC3_EXTENSION
   #define VPC3_GET_INT_REQ_REG_L()                   pVpc3->bIntReqReg_L VPC3_EXTENSION

   #define VPC3_GET_INT_REG_H()                       pVpc3->sReg.sRead.bIntReg_H VPC3_EXTENSION
   #define VPC3_GET_INT_REG_L()                       pVpc3->sReg.sRead.bIntReg_L VPC3_EXTENSION
#endif//#if VPC3_SERIAL_MODE

#if VPC3_SERIAL_MODE

   #if DP_INTERRUPT_MASK_8BIT

      // -- read out reasons for indications ( Vpc3Isr() ) --------------------------
      #define VPC3_GET_IND_CLOCK_SYNC()            (Vpc3Read( bVpc3RoIntReg_L ) & VPC3_INT_CLOCK_SYNC)
      #define VPC3_GET_IND_GO_LEAVE_DATA_EX()      (Vpc3Read( bVpc3RoIntReg_L ) & VPC3_INT_GO_LEAVE_DATA_EX)
      #define VPC3_GET_IND_BAUDRATE_DETECT()       (Vpc3Read( bVpc3RoIntReg_L ) & VPC3_INT_BAUDRATE_DETECT)
      #define VPC3_GET_IND_WD_DP_MODE_TIMEOUT()    (Vpc3Read( bVpc3RoIntReg_L ) & VPC3_INT_WD_DP_MODE_TIMEOUT)
      #define VPC3_GET_IND_USER_TIMER_CLOCK()      (Vpc3Read( bVpc3RoIntReg_L ) & VPC3_INT_USER_TIMER_CLOCK)
      #define VPC3_GET_IND_DXB_LINK_ERROR()        (Vpc3Read( bVpc3RoIntReg_L ) & VPC3_INT_DXB_LINK_ERROR)
      #define VPC3_GET_IND_NEW_EXT_PRM_DATA()      (Vpc3Read( bVpc3RoIntReg_L ) & VPC3_INT_NEW_EXT_PRM_DATA)
      #define VPC3_GET_IND_DXB_OUT()               (Vpc3Read( bVpc3RoIntReg_L ) & VPC3_INT_DXB_OUT)
      #define VPC3_GET_IND_NEW_GC_COMMAND()        (Vpc3Read( bVpc3RoIntReg_H ) & VPC3_INT_NEW_GC_COMMAND)
      #define VPC3_GET_IND_NEW_SSA_DATA()          (Vpc3Read( bVpc3RoIntReg_H ) & VPC3_INT_NEW_SSA_DATA)
      #define VPC3_GET_IND_NEW_CFG_DATA()          (Vpc3Read( bVpc3RoIntReg_H ) & VPC3_INT_NEW_CFG_DATA)
      #define VPC3_GET_IND_NEW_PRM_DATA()          (Vpc3Read( bVpc3RoIntReg_H ) & VPC3_INT_NEW_PRM_DATA)
      #define VPC3_GET_IND_DIAG_BUFFER_CHANGED()   (Vpc3Read( bVpc3RoIntReg_H ) & VPC3_INT_DIAG_BUFFER_CHANGED)
      #define VPC3_GET_IND_DX_OUT()                (Vpc3Read( bVpc3RoIntReg_H ) & VPC3_INT_DX_OUT)
      #define VPC3_GET_IND_POLL_END_IND()          (Vpc3Read( bVpc3RoIntReg_H ) & VPC3_INT_POLL_END_IND)
      #define VPC3_GET_IND_FDL_IND()               (Vpc3Read( bVpc3RoIntReg_H ) & VPC3_INT_FDL_IND)

      // -- Acknowledging the indication -------------------------------------------
      #define VPC3_CON_IND_CLOCK_SYNC()            Vpc3Write( bVpc3WoIntAck_L, VPC3_INT_CLOCK_SYNC )
      #define VPC3_CON_IND_GO_LEAVE_DATA_EX()      Vpc3Write( bVpc3WoIntAck_L, VPC3_INT_GO_LEAVE_DATA_EX )
      #define VPC3_CON_IND_BAUDRATE_DETECT()       Vpc3Write( bVpc3WoIntAck_L, VPC3_INT_BAUDRATE_DETECT )
      #define VPC3_CON_IND_WD_DP_MODE_TIMEOUT()    Vpc3Write( bVpc3WoIntAck_L, VPC3_INT_WD_DP_MODE_TIMEOUT )
      #define VPC3_CON_IND_USER_TIMER_CLOCK()      Vpc3Write( bVpc3WoIntAck_L, VPC3_INT_USER_TIMER_CLOCK )
      #define VPC3_CON_IND_DXB_LINK_ERROR()        Vpc3Write( bVpc3WoIntAck_L, VPC3_INT_DXB_LINK_ERROR )
      #define VPC3_CON_IND_NEW_EXT_PRM_DATA()      Vpc3Write( bVpc3WoIntAck_L, VPC3_INT_NEW_EXT_PRM_DATA )
      #define VPC3_CON_IND_DXB_OUT()               Vpc3Write( bVpc3WoIntAck_L, VPC3_INT_DXB_OUT )
      #define VPC3_CON_IND_NEW_GC_COMMAND()        Vpc3Write( bVpc3WoIntAck_H, VPC3_INT_NEW_GC_COMMAND )
      #define VPC3_CON_IND_NEW_SSA_DATA()          Vpc3Write( bVpc3WoIntAck_H, VPC3_INT_NEW_SSA_DATA )
      #define VPC3_CON_IND_NEW_CFG_DATA()
      #define VPC3_CON_IND_NEW_PRM_DATA()
      #define VPC3_CON_IND_DIAG_BUFFER_CHANGED()   Vpc3Write( bVpc3WoIntAck_H, VPC3_INT_DIAG_BUFFER_CHANGED )
      #define VPC3_CON_IND_DX_OUT()                Vpc3Write( bVpc3WoIntAck_H, VPC3_INT_DX_OUT )
      #define VPC3_CON_IND_POLL_END_IND()          Vpc3Write( bVpc3WoIntAck_H, VPC3_INT_POLL_END_IND )
      #define VPC3_CON_IND_FDL_IND()               Vpc3Write( bVpc3WoIntAck_H, VPC3_INT_FDL_IND )

      // poll read out reasons for indications --------------------------------------
      #define VPC3_POLL_IND_CLOCK_SYNC()          (Vpc3Read( bVpc3RwIntReqReg_L ) & VPC3_INT_CLOCK_SYNC)
      #define VPC3_POLL_IND_BAUDRATE_DETECT()     (Vpc3Read( bVpc3RwIntReqReg_L ) & VPC3_INT_BAUDRATE_DETECT)
      #define VPC3_POLL_IND_USER_TIMER_CLOCK()    (Vpc3Read( bVpc3RwIntReqReg_L ) & VPC3_INT_USER_TIMER_CLOCK)
      #define VPC3_POLL_IND_GO_LEAVE_DATA_EX()    (Vpc3Read( bVpc3RwIntReqReg_L ) & VPC3_INT_GO_LEAVE_DATA_EX)
      #define VPC3_POLL_IND_WD_DP_MODE_TIMEOUT()  (Vpc3Read( bVpc3RwIntReqReg_L ) & VPC3_INT_WD_DP_MODE_TIMEOUT)
      #define VPC3_POLL_IND_NEW_EXT_PRM_DATA()    (Vpc3Read( bVpc3RwIntReqReg_L ) & VPC3_INT_NEW_EXT_PRM_DATA)
      #define VPC3_POLL_IND_NEW_GC_COMMAND()      (Vpc3Read( bVpc3RwIntReqReg_H ) & VPC3_INT_NEW_GC_COMMAND)
      #define VPC3_POLL_IND_NEW_SSA_DATA()        (Vpc3Read( bVpc3RwIntReqReg_H ) & VPC3_INT_NEW_SSA_DATA)
      #define VPC3_POLL_IND_NEW_CFG_DATA()        (Vpc3Read( bVpc3RwIntReqReg_H ) & VPC3_INT_NEW_CFG_DATA)
      #define VPC3_POLL_IND_NEW_PRM_DATA()        (Vpc3Read( bVpc3RwIntReqReg_H ) & VPC3_INT_NEW_PRM_DATA)
      #define VPC3_POLL_IND_DIAG_BUFFER_CHANGED() (Vpc3Read( bVpc3RwIntReqReg_H ) & VPC3_INT_DIAG_BUFFER_CHANGED)
      #define VPC3_POLL_IND_DX_OUT()              (Vpc3Read( bVpc3RwIntReqReg_H ) & VPC3_INT_DX_OUT)
      #define VPC3_POLL_IND_POLL_END_IND()        (Vpc3Read( bVpc3RwIntReqReg_H ) & VPC3_INT_POLL_END_IND)
      #define VPC3_POLL_IND_FDL_IND()             (Vpc3Read( bVpc3RwIntReqReg_H ) & VPC3_INT_FDL_IND)

   #else

      // -- read out reasons for indications ( Vpc3Isr() ) --------------------------
      #define VPC3_GET_IND_CLOCK_SYNC()            ( pDpSystem->wInterruptEvent & VPC3_INT16_CLOCK_SYNC )
      #define VPC3_GET_IND_GO_LEAVE_DATA_EX()      ( pDpSystem->wInterruptEvent & VPC3_INT16_GO_LEAVE_DATA_EX )
      #define VPC3_GET_IND_BAUDRATE_DETECT()       ( pDpSystem->wInterruptEvent & VPC3_INT16_BAUDRATE_DETECT )
      #define VPC3_GET_IND_WD_DP_MODE_TIMEOUT()    ( pDpSystem->wInterruptEvent & VPC3_INT16_WD_DP_MODE_TIMEOUT )
      #define VPC3_GET_IND_USER_TIMER_CLOCK()      ( pDpSystem->wInterruptEvent & VPC3_INT16_USER_TIMER_CLOCK )
      #define VPC3_GET_IND_DXB_LINK_ERROR()        ( pDpSystem->wInterruptEvent & VPC3_INT16_DXB_LINK_ERROR )
      #define VPC3_GET_IND_NEW_EXT_PRM_DATA()      ( pDpSystem->wInterruptEvent & VPC3_INT16_NEW_EXT_PRM_DATA )
      #define VPC3_GET_IND_DXB_OUT()               ( pDpSystem->wInterruptEvent & VPC3_INT16_DXB_OUT )
      #define VPC3_GET_IND_NEW_GC_COMMAND()        ( pDpSystem->wInterruptEvent & VPC3_INT16_NEW_GC_COMMAND )
      #define VPC3_GET_IND_NEW_SSA_DATA()          ( pDpSystem->wInterruptEvent & VPC3_INT16_NEW_SSA_DATA )
      #define VPC3_GET_IND_NEW_CFG_DATA()          ( pDpSystem->wInterruptEvent & VPC3_INT16_NEW_CFG_DATA )
      #define VPC3_GET_IND_NEW_PRM_DATA()          ( pDpSystem->wInterruptEvent & VPC3_INT16_NEW_PRM_DATA )
      #define VPC3_GET_IND_DIAG_BUFFER_CHANGED()   ( pDpSystem->wInterruptEvent & VPC3_INT16_DIAG_BUFFER_CHANGED )
      #define VPC3_GET_IND_DX_OUT()                ( pDpSystem->wInterruptEvent & VPC3_INT16_DX_OUT )
      #define VPC3_GET_IND_POLL_END_IND()          ( pDpSystem->wInterruptEvent & VPC3_INT16_POLL_END_IND )
      #define VPC3_GET_IND_FDL_IND()               ( pDpSystem->wInterruptEvent & VPC3_INT16_FDL_IND )

      // -- Acknowledging the indication -------------------------------------------
      #define VPC3_CON_IND_CLOCK_SYNC()
      #define VPC3_CON_IND_GO_LEAVE_DATA_EX()
      #define VPC3_CON_IND_BAUDRATE_DETECT()
      #define VPC3_CON_IND_WD_DP_MODE_TIMEOUT()
      #define VPC3_CON_IND_USER_TIMER_CLOCK()
      #define VPC3_CON_IND_DXB_LINK_ERROR()
      #define VPC3_CON_IND_NEW_EXT_PRM_DATA()
      #define VPC3_CON_IND_DXB_OUT()
      #define VPC3_CON_IND_NEW_GC_COMMAND()
      #define VPC3_CON_IND_NEW_SSA_DATA()
      #define VPC3_CON_IND_NEW_CFG_DATA()          ( pDpSystem->wInterruptEvent &= ~VPC3_INT16_NEW_CFG_DATA )
      #define VPC3_CON_IND_NEW_PRM_DATA()          ( pDpSystem->wInterruptEvent &= ~VPC3_INT16_NEW_PRM_DATA )
      #define VPC3_CON_IND_DIAG_BUFFER_CHANGED()
      #define VPC3_CON_IND_DX_OUT()
      #define VPC3_CON_IND_POLL_END_IND()
      #define VPC3_CON_IND_FDL_IND()

      // poll read out reasons for indications --------------------------------------
      #define VPC3_POLL_IND_CLOCK_SYNC()           ( pDpSystem->wPollInterruptEvent & VPC3_INT16_CLOCK_SYNC )
      #define VPC3_POLL_IND_BAUDRATE_DETECT()      ( pDpSystem->wPollInterruptEvent & VPC3_INT16_BAUDRATE_DETECT )
      #define VPC3_POLL_IND_USER_TIMER_CLOCK()     ( pDpSystem->wPollInterruptEvent & VPC3_INT16_USER_TIMER_CLOCK )
      #define VPC3_POLL_IND_GO_LEAVE_DATA_EX()     ( pDpSystem->wPollInterruptEvent & VPC3_INT16_GO_LEAVE_DATA_EX )
      #define VPC3_POLL_IND_WD_DP_MODE_TIMEOUT()   ( pDpSystem->wPollInterruptEvent & VPC3_INT16_WD_DP_MODE_TIMEOUT )
      #define VPC3_POLL_IND_NEW_EXT_PRM_DATA()     ( pDpSystem->wPollInterruptEvent & VPC3_INT16_NEW_EXT_PRM_DATA )
      #define VPC3_POLL_IND_NEW_GC_COMMAND()       ( pDpSystem->wPollInterruptEvent & VPC3_INT16_NEW_GC_COMMAND )
      #define VPC3_POLL_IND_NEW_SSA_DATA()         ( pDpSystem->wPollInterruptEvent & VPC3_INT16_NEW_SSA_DATA )
      #define VPC3_POLL_IND_NEW_CFG_DATA()         ( pDpSystem->wPollInterruptEvent & VPC3_INT16_NEW_CFG_DATA )
      #define VPC3_POLL_IND_NEW_PRM_DATA()         ( pDpSystem->wPollInterruptEvent & VPC3_INT16_NEW_PRM_DATA )
      #define VPC3_POLL_IND_DIAG_BUFFER_CHANGED()  ( pDpSystem->wPollInterruptEvent & VPC3_INT16_DIAG_BUFFER_CHANGED )
      #define VPC3_POLL_IND_DX_OUT()               ( pDpSystem->wPollInterruptEvent & VPC3_INT16_DX_OUT )
      #define VPC3_POLL_IND_POLL_END_IND()         ( pDpSystem->wPollInterruptEvent & VPC3_INT16_POLL_END_IND )
      #define VPC3_POLL_IND_FDL_IND()              ( pDpSystem->wPollInterruptEvent & VPC3_INT16_FDL_IND )

   #endif//#if DP_INTERRUPT_MASK_8BIT

   // -- Ending the Indication ---------------------------------------------------
   #define VPC3_SET_EOI() Vpc3Write( bVpc3WoModeReg1_S, VPC3_EOI )

   #define DP_LOCK_IND()                                       \
   {                                                           \
      Vpc3Write( bVpc3WoIntMask_H, 0xFF );                     \
                                                               \
      Vpc3Write( bVpc3WoIntMask_L, 0xFF );                     \
   }//#define DP_LOCK_IND()


   #define DP_UNLOCK_IND()                                     \
   {                                                           \
      Vpc3Write( bVpc3WoIntMask_H, pDpSystem->bIntIndHigh );   \
                                                               \
      Vpc3Write( bVpc3WoIntMask_L, pDpSystem->bIntIndLow );    \
   }//#define DP_UNLOCK_IND()

#else

   #if DP_INTERRUPT_MASK_8BIT

      // -- read out reasons for indications ( Vpc3Isr() ) --------------------------
      #define VPC3_GET_IND_CLOCK_SYNC()            ( pVpc3->sReg.sRead.bIntReg_L VPC3_EXTENSION & VPC3_INT_CLOCK_SYNC )
      #define VPC3_GET_IND_GO_LEAVE_DATA_EX()      ( pVpc3->sReg.sRead.bIntReg_L VPC3_EXTENSION & VPC3_INT_GO_LEAVE_DATA_EX )
      #define VPC3_GET_IND_BAUDRATE_DETECT()       ( pVpc3->sReg.sRead.bIntReg_L VPC3_EXTENSION & VPC3_INT_BAUDRATE_DETECT )
      #define VPC3_GET_IND_WD_DP_MODE_TIMEOUT()    ( pVpc3->sReg.sRead.bIntReg_L VPC3_EXTENSION & VPC3_INT_WD_DP_MODE_TIMEOUT )
      #define VPC3_GET_IND_USER_TIMER_CLOCK()      ( pVpc3->sReg.sRead.bIntReg_L VPC3_EXTENSION & VPC3_INT_USER_TIMER_CLOCK )
      #define VPC3_GET_IND_DXB_LINK_ERROR()        ( pVpc3->sReg.sRead.bIntReg_L VPC3_EXTENSION & VPC3_INT_DXB_LINK_ERROR )
      #define VPC3_GET_IND_NEW_EXT_PRM_DATA()      ( pVpc3->sReg.sRead.bIntReg_L VPC3_EXTENSION & VPC3_INT_NEW_EXT_PRM_DATA )
      #define VPC3_GET_IND_DXB_OUT()               ( pVpc3->sReg.sRead.bIntReg_L VPC3_EXTENSION & VPC3_INT_DXB_OUT )
      #define VPC3_GET_IND_NEW_GC_COMMAND()        ( pVpc3->sReg.sRead.bIntReg_H VPC3_EXTENSION & VPC3_INT_NEW_GC_COMMAND )
      #define VPC3_GET_IND_NEW_SSA_DATA()          ( pVpc3->sReg.sRead.bIntReg_H VPC3_EXTENSION & VPC3_INT_NEW_SSA_DATA )
      #define VPC3_GET_IND_NEW_CFG_DATA()          ( pVpc3->sReg.sRead.bIntReg_H VPC3_EXTENSION & VPC3_INT_NEW_CFG_DATA )
      #define VPC3_GET_IND_NEW_PRM_DATA()          ( pVpc3->sReg.sRead.bIntReg_H VPC3_EXTENSION & VPC3_INT_NEW_PRM_DATA )
      #define VPC3_GET_IND_DIAG_BUFFER_CHANGED()   ( pVpc3->sReg.sRead.bIntReg_H VPC3_EXTENSION & VPC3_INT_DIAG_BUFFER_CHANGED )
      #define VPC3_GET_IND_DX_OUT()                ( pVpc3->sReg.sRead.bIntReg_H VPC3_EXTENSION & VPC3_INT_DX_OUT )
      #define VPC3_GET_IND_POLL_END_IND()          ( pVpc3->sReg.sRead.bIntReg_H VPC3_EXTENSION & VPC3_INT_POLL_END_IND )
      #define VPC3_GET_IND_FDL_IND()               ( pVpc3->sReg.sRead.bIntReg_H VPC3_EXTENSION & VPC3_INT_FDL_IND )

      // -- Acknowledging the indication -------------------------------------------
      #define VPC3_CON_IND_CLOCK_SYNC()            pVpc3->sReg.sWrite.bIntAck_L VPC3_EXTENSION = VPC3_INT_CLOCK_SYNC
      #define VPC3_CON_IND_GO_LEAVE_DATA_EX()      pVpc3->sReg.sWrite.bIntAck_L VPC3_EXTENSION = VPC3_INT_GO_LEAVE_DATA_EX
      #define VPC3_CON_IND_BAUDRATE_DETECT()       pVpc3->sReg.sWrite.bIntAck_L VPC3_EXTENSION = VPC3_INT_BAUDRATE_DETECT
      #define VPC3_CON_IND_WD_DP_MODE_TIMEOUT()    pVpc3->sReg.sWrite.bIntAck_L VPC3_EXTENSION = VPC3_INT_WD_DP_MODE_TIMEOUT
      #define VPC3_CON_IND_USER_TIMER_CLOCK()      pVpc3->sReg.sWrite.bIntAck_L VPC3_EXTENSION = VPC3_INT_USER_TIMER_CLOCK
      #define VPC3_CON_IND_DXB_LINK_ERROR()        pVpc3->sReg.sWrite.bIntAck_L VPC3_EXTENSION = VPC3_INT_DXB_LINK_ERROR
      #define VPC3_CON_IND_NEW_EXT_PRM_DATA()      pVpc3->sReg.sWrite.bIntAck_L VPC3_EXTENSION = VPC3_INT_NEW_EXT_PRM_DATA
      #define VPC3_CON_IND_DXB_OUT()               pVpc3->sReg.sWrite.bIntAck_L VPC3_EXTENSION = VPC3_INT_DXB_OUT
      #define VPC3_CON_IND_NEW_GC_COMMAND()        pVpc3->sReg.sWrite.bIntAck_H VPC3_EXTENSION = VPC3_INT_NEW_GC_COMMAND
      #define VPC3_CON_IND_NEW_SSA_DATA()          pVpc3->sReg.sWrite.bIntAck_H VPC3_EXTENSION = VPC3_INT_NEW_SSA_DATA
      #define VPC3_CON_IND_DIAG_BUFFER_CHANGED()   pVpc3->sReg.sWrite.bIntAck_H VPC3_EXTENSION = VPC3_INT_DIAG_BUFFER_CHANGED
      #define VPC3_CON_IND_DX_OUT()                pVpc3->sReg.sWrite.bIntAck_H VPC3_EXTENSION = VPC3_INT_DX_OUT
      #define VPC3_CON_IND_POLL_END_IND()          pVpc3->sReg.sWrite.bIntAck_H VPC3_EXTENSION = VPC3_INT_POLL_END_IND
      #define VPC3_CON_IND_FDL_IND()               pVpc3->sReg.sWrite.bIntAck_H VPC3_EXTENSION = VPC3_INT_FDL_IND

      // poll read out reasons for indications --------------------------------------
      #define VPC3_POLL_IND_CLOCK_SYNC()           ( pVpc3->bIntReqReg_L VPC3_EXTENSION & VPC3_INT_CLOCK_SYNC )
      #define VPC3_POLL_IND_BAUDRATE_DETECT()      ( pVpc3->bIntReqReg_L VPC3_EXTENSION & VPC3_INT_BAUDRATE_DETECT )
      #define VPC3_POLL_IND_USER_TIMER_CLOCK()     ( pVpc3->bIntReqReg_L VPC3_EXTENSION & VPC3_INT_USER_TIMER_CLOCK )
      #define VPC3_POLL_IND_GO_LEAVE_DATA_EX()     ( pVpc3->bIntReqReg_L VPC3_EXTENSION & VPC3_INT_GO_LEAVE_DATA_EX )
      #define VPC3_POLL_IND_WD_DP_MODE_TIMEOUT()   ( pVpc3->bIntReqReg_L VPC3_EXTENSION & VPC3_INT_WD_DP_MODE_TIMEOUT )
      #define VPC3_POLL_IND_NEW_EXT_PRM_DATA()     ( pVpc3->bIntReqReg_L VPC3_EXTENSION & VPC3_INT_NEW_EXT_PRM_DATA )
      #define VPC3_POLL_IND_NEW_GC_COMMAND()       ( pVpc3->bIntReqReg_H VPC3_EXTENSION & VPC3_INT_NEW_GC_COMMAND )
      #define VPC3_POLL_IND_NEW_SSA_DATA()         ( pVpc3->bIntReqReg_H VPC3_EXTENSION & VPC3_INT_NEW_SSA_DATA )
      #define VPC3_POLL_IND_NEW_CFG_DATA()         ( pVpc3->bIntReqReg_H VPC3_EXTENSION & VPC3_INT_NEW_CFG_DATA )
      #define VPC3_POLL_IND_NEW_PRM_DATA()         ( pVpc3->bIntReqReg_H VPC3_EXTENSION & VPC3_INT_NEW_PRM_DATA )
      #define VPC3_POLL_IND_DIAG_BUFFER_CHANGED()  ( pVpc3->bIntReqReg_H VPC3_EXTENSION & VPC3_INT_DIAG_BUFFER_CHANGED )
      #define VPC3_POLL_IND_DX_OUT()               ( pVpc3->bIntReqReg_H VPC3_EXTENSION & VPC3_INT_DX_OUT )
      #define VPC3_POLL_IND_POLL_END_IND()         ( pVpc3->bIntReqReg_H VPC3_EXTENSION & VPC3_INT_POLL_END_IND )
      #define VPC3_POLL_IND_FDL_IND()              ( pVpc3->bIntReqReg_H VPC3_EXTENSION & VPC3_INT_FDL_IND )

   #else

      // -- read out reasons for indications ( Vpc3Isr() ) --------------------------
      #define VPC3_GET_IND_CLOCK_SYNC()            ( pDpSystem->wInterruptEvent & VPC3_INT16_CLOCK_SYNC )
      #define VPC3_GET_IND_GO_LEAVE_DATA_EX()      ( pDpSystem->wInterruptEvent & VPC3_INT16_GO_LEAVE_DATA_EX )
      #define VPC3_GET_IND_BAUDRATE_DETECT()       ( pDpSystem->wInterruptEvent & VPC3_INT16_BAUDRATE_DETECT )
      #define VPC3_GET_IND_WD_DP_MODE_TIMEOUT()    ( pDpSystem->wInterruptEvent & VPC3_INT16_WD_DP_TIMEOUT )
      #define VPC3_GET_IND_USER_TIMER_CLOCK()      ( pDpSystem->wInterruptEvent & VPC3_INT16_USER_TIMER_CLOCK )
      #define VPC3_GET_IND_DXB_LINK_ERROR()        ( pDpSystem->wInterruptEvent & VPC3_INT16_DXB_LINK_ERROR )
      #define VPC3_GET_IND_NEW_EXT_PRM_DATA()      ( pDpSystem->wInterruptEvent & VPC3_INT16_NEW_EXT_PRM_DATA )
      #define VPC3_GET_IND_DXB_OUT()               ( pDpSystem->wInterruptEvent & VPC3_INT16_DXB_OUT )
      #define VPC3_GET_IND_NEW_GC_COMMAND()        ( pDpSystem->wInterruptEvent & VPC3_INT16_NEW_GC_COMMAND )
      #define VPC3_GET_IND_NEW_SSA_DATA()          ( pDpSystem->wInterruptEvent & VPC3_INT16_NEW_SSA_DATA )
      #define VPC3_GET_IND_NEW_CFG_DATA()          ( pDpSystem->wInterruptEvent & VPC3_INT16_NEW_CFG_DATA )
      #define VPC3_GET_IND_NEW_PRM_DATA()          ( pDpSystem->wInterruptEvent & VPC3_INT16_NEW_PRM_DATA )
      #define VPC3_GET_IND_DIAG_BUFFER_CHANGED()   ( pDpSystem->wInterruptEvent & VPC3_INT16_DIAG_BUFFER_CHANGED )
      #define VPC3_GET_IND_DX_OUT()                ( pDpSystem->wInterruptEvent & VPC3_INT16_DX_OUT )
      #define VPC3_GET_IND_POLL_END_IND()          ( pDpSystem->wInterruptEvent & VPC3_INT16_POLL_END_IND )
      #define VPC3_GET_IND_FDL_IND()               ( pDpSystem->wInterruptEvent & VPC3_INT16_FDL_IND )

      // -- Acknowledging the indication -------------------------------------------
      #define VPC3_CON_IND_CLOCK_SYNC()
      #define VPC3_CON_IND_GO_LEAVE_DATA_EX()
      #define VPC3_CON_IND_BAUDRATE_DETECT()
      #define VPC3_CON_IND_WD_DP_TIMEOUT()
      #define VPC3_CON_IND_USER_TIMER_CLOCK()
      #define VPC3_CON_IND_DXB_LINK_ERROR()
      #define VPC3_CON_IND_NEW_EXT_PRM_DATA()
      #define VPC3_CON_IND_DXB_OUT()
      #define VPC3_CON_IND_NEW_GC_COMMAND()
      #define VPC3_CON_IND_NEW_SSA_DATA()
      #define VPC3_CON_IND_NEW_CFG_DATA()          ( pDpSystem->wInterruptEvent &= ~VPC3_INT16_NEW_CFG_DATA )
      #define VPC3_CON_IND_NEW_PRM_DATA()          ( pDpSystem->wInterruptEvent &= ~VPC3_INT16_NEW_PRM_DATA )
      #define VPC3_CON_IND_DIAG_BUFFER_CHANGED()
      #define VPC3_CON_IND_DX_OUT()
      #define VPC3_CON_IND_POLL_END_IND()
      #define VPC3_CON_IND_FDL_IND()

      // poll read out reasons for indications --------------------------------------
      #define VPC3_POLL_IND_CLOCK_SYNC()           ( pDpSystem->wPollInterruptEvent & VPC3_INT16_CLOCK_SYNC )
      #define VPC3_POLL_IND_BAUDRATE_DETECT()      ( pDpSystem->wPollInterruptEvent & VPC3_INT16_BAUDRATE_DETECT )
      #define VPC3_POLL_IND_USER_TIMER_CLOCK()     ( pDpSystem->wPollInterruptEvent & VPC3_INT16_USER_TIMER_CLOCK )
      #define VPC3_POLL_IND_GO_LEAVE_DATA_EX()     ( pDpSystem->wPollInterruptEvent & VPC3_INT16_GO_LEAVE_DATA_EX )
      #define VPC3_POLL_IND_WD_DP_MODE_TIMEOUT()   ( pDpSystem->wPollInterruptEvent & VPC3_INT16_WD_DP_MODE_TIMEOUT )
      #define VPC3_POLL_IND_NEW_EXT_PRM_DATA()     ( pDpSystem->wPollInterruptEvent & VPC3_INT16_NEW_EXT_PRM_DATA )
      #define VPC3_POLL_IND_NEW_GC_COMMAND()       ( pDpSystem->wPollInterruptEvent & VPC3_INT16_NEW_GC_COMMAND )
      #define VPC3_POLL_IND_NEW_SSA_DATA()         ( pDpSystem->wPollInterruptEvent & VPC3_INT16_NEW_SSA_DATA )
      #define VPC3_POLL_IND_NEW_CFG_DATA()         ( pDpSystem->wPollInterruptEvent & VPC3_INT16_NEW_CFG_DATA )
      #define VPC3_POLL_IND_NEW_PRM_DATA()         ( pDpSystem->wPollInterruptEvent & VPC3_INT16_NEW_PRM_DATA )
      #define VPC3_POLL_IND_DIAG_BUFFER_CHANGED()  ( pDpSystem->wPollInterruptEvent & VPC3_INT16_DIAG_BUFFER_CHANGED )
      #define VPC3_POLL_IND_DX_OUT()               ( pDpSystem->wPollInterruptEvent & VPC3_INT16_DX_OUT )
      #define VPC3_POLL_IND_POLL_END_IND()         ( pDpSystem->wPollInterruptEvent & VPC3_INT16_POLL_END_IND )
      #define VPC3_POLL_IND_FDL_IND()              ( pDpSystem->wPollInterruptEvent & VPC3_INT16_FDL_IND )

   #endif//#if DP_INTERRUPT_MASK_8BIT

   #define VPC3_SET_IRR_1(IRR_BYTE)                ( pVpc3->bIntReqReg_L VPC3_EXTENSION |= IRR_BYTE )
   #define VPC3_SET_IRR_2(IRR_BYTE)                ( pVpc3->bIntReqReg_H VPC3_EXTENSION |= IRR_BYTE )

   // -- Ending the Indication ---------------------------------------------------
   #define VPC3_SET_EOI() pVpc3->sCtrlPrm.sWrite.bModeReg1_S VPC3_EXTENSION = VPC3_EOI

   #define DP_LOCK_IND()                                                      \
   {                                                                          \
      pVpc3->sReg.sWrite.bIntMask_H VPC3_EXTENSION = 0xFF;                    \
                                                                              \
      pVpc3->sReg.sWrite.bIntMask_L VPC3_EXTENSION = 0xFF;                    \
   }//#define DP_LOCK_IND()


   #define DP_UNLOCK_IND()                                                    \
   {                                                                          \
      pVpc3->sReg.sWrite.bIntMask_H VPC3_EXTENSION = pDpSystem->bIntIndHigh;  \
                                                                              \
      pVpc3->sReg.sWrite.bIntMask_L VPC3_EXTENSION = pDpSystem->bIntIndLow;   \
   }//#define DP_UNLOCK_IND()

#endif//#if VPC3_SERIAL_MODE


/*-----------------------------------------------------------------------------------------------------------*/
/* 2.0 parameter telegram                                                                                    */
/*-----------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/* 2.1 structure of prm data                                                 */
/*---------------------------------------------------------------------------*/

/*
|--------------|------------|---------------|-----------------------|------------|-----------|------------------|-------------------|
|      7       |      6     |       5       |           4           |      3     |     2     |         1        |          0        |
|--------------|------------|---------------|-----------------------|------------|-----------|------------------|-------------------|
|   Lock_Req   | Unlock_Req |   Sync_Req    |       Freeze_Req      |    WD_On   |     0     |         0        |          0        |
|--------------|------------|---------------|-----------------------|------------|-----------|------------------|-------------------|
| WD_Fact_1 (1 bis 255)                                                                                                             |
|-----------------------------------------------------------------------------------------------------------------------------------|
| WD_Fact_2 (1 bis 255)                                                                                                             |
|-----------------------------------------------------------------------------------------------------------------------------------|
| Min. Station Delay Responder (min Tsdr)                                                                                           |
|-----------------------------------------------------------------------------------------------------------------------------------|
| Ident_Number (high)                                                                                                               |
|-----------------------------------------------------------------------------------------------------------------------------------|
| Ident_Number (low)                                                                                                                |
|-----------------------------------------------------------------------------------------------------------------------------------|
| Group_Ident                                                                                                                       |
|--------------|------------|---------------|-----------------------|------------|-----------|------------------|-------------------|
| DPV1_Enable  | Fail_Safe  | Publisher_En  |       reserved        |  reserved  |  WD_Base  | Dis_Stop_Control | Dis_Start_Control |
|--------------|------------|---------------|-----------------------|------------|-----------|------------------|-------------------|
| En_Pull_Plug | En_Process | En_Diagnostic | En_Manufacturer_Spec. | En_Status  | En_Update |    reserviert    | Chk_Cfg_Mode      |
|    Alarm     |    Alarm   |    Alarm      |         Alarm         |   Alarm    |   Alarm   |                  |                   |
|--------------|------------|---------------|-----------------------|------------|-----------|------------------|-------------------|
|   PrmCmd     |  reserved  |   reserved    |       IsoM_Req        | Prm_Struct |                 Alarm_Mode                       |
|--------------|------------|---------------|-----------------------|------------|-----------|------------------|-------------------|
|                                                            User_Prm_Data                                                          |
|-----------------------------------------------------------------------------------------------------------------------------------|
*/

typedef struct
{
   UBYTE bStationState;
   UBYTE bWdFactor1;
   UBYTE bWdFactor2;
   UBYTE bMinTsdr;
   UBYTE bIdentHigh;
   UBYTE bIdentLow;
   UBYTE bGroupIdent;
   UBYTE bDpv1Status1;
   UBYTE bDpv1Status2;
   UBYTE bDpv1Status3;

   UBYTE bUserPrmData;
}  STRUC_PRM;
#define VPC3_STRUC_PRM_PTR  STRUC_PRM  VPC3_PTR

/*---------------------------------------------------------------------------*/
/* 2.1 literals for Station State                                            */
/*---------------------------------------------------------------------------*/
#define STATION_STATE_WD_ON               ((UBYTE)0x08)

/*---------------------------------------------------------------------------*/
/* 2.2 literals for DPV1 Status 1                                            */
/*---------------------------------------------------------------------------*/
#define DPV1_STATUS_1_RESERVED_1          ((UBYTE)0x03)
#define DPV1_STATUS_1_WD_BASE_1MS         ((UBYTE)0x04)
#define DPV1_STATUS_1_RESERVED_2          ((UBYTE)0x08)
#define DPV1_STATUS_1_RESERVED_3          ((UBYTE)0x10)
#define DPV1_STATUS_1_PUBLISHER_ENABLE    ((UBYTE)0x20)
#define DPV1_STATUS_1_FAIL_SAFE           ((UBYTE)0x40)
#define DPV1_STATUS_1_DPV1_ENABLE         ((UBYTE)0x80)

/*---------------------------------------------------------------------------*/
/* 2.3 literals for DPV1 Status 2                                            */
/*---------------------------------------------------------------------------*/
#define DPV1_STATUS_2_CHECK_CFG_MODE      ((UBYTE)0x01)
#define CHECK_CFG                         ((UBYTE)0x00)
#define NO_CHECK_CFG                      ((UBYTE)0x01)
#define DPV1_STATUS_2_RESERVED_4          ((UBYTE)0x02)

#define ALARM_TYPE_UPDATE_VALUE           ((UBYTE)0x04)
#define ALARM_TYPE_STATUS_VALUE           ((UBYTE)0x08)
#define ALARM_TYPE_MANUFACTURER_VALUE     ((UBYTE)0x10)
#define ALARM_TYPE_DIAGNOSTIC_VALUE       ((UBYTE)0x20)
#define ALARM_TYPE_PROCESS_VALUE          ((UBYTE)0x40)
#define ALARM_TYPE_PULLPLUG_VALUE         ((UBYTE)0x80)

#define DPV1_STATUS_2_ALARM_TYPE_MASK     (   ALARM_TYPE_UPDATE_VALUE       \
                                            | ALARM_TYPE_STATUS_VALUE       \
                                            | ALARM_TYPE_MANUFACTURER_VALUE \
                                            | ALARM_TYPE_DIAGNOSTIC_VALUE   \
                                            | ALARM_TYPE_PROCESS_VALUE      \
                                            | ALARM_TYPE_PULLPLUG_VALUE     \
                                          )
#define ALARM_TYPE_NONE_VALUE             ((UBYTE)0x00)
#define ALARM_TYPE_ALL_VALUE              ((UBYTE)0xFF)

/*---------------------------------------------------------------------------*/
/* 2.4 literals for DPV1 Status 3                                            */
/*---------------------------------------------------------------------------*/
#define DPV1_STATUS_3_ALARM_MODE_MASK     ((UBYTE)0x07)
#define DPV1_STATUS_3_PRM_STRUCTURE       ((UBYTE)0x08)
#define DPV1_STATUS_3_ISOM_REQUEST        ((UBYTE)0x10)
#define DPV1_STATUS_3_RESERVED_UNUSED5    ((UBYTE)0x70)
#define DPV1_STATUS_3_PRM_CMD             ((UBYTE)0x80)

/*---------------------------------------------------------------------------*/
/* 2.5 general defines for prm data                                          */
/*---------------------------------------------------------------------------*/
#define PRM_LEN_NORM                      ((UBYTE)0x07)
#define PRM_LEN_DPV1                      ((UBYTE)0x0A)

/*---------------------------------------------------------------------------*/
/* 2.6 literals for structured prm data                                      */
/*---------------------------------------------------------------------------*/
#define PBC_PRMCMD                        ((UBYTE)0x02)
#define PBC_DXB_LINKTABLE                 ((UBYTE)0x03)
#define PBC_ISOCHRON                      ((UBYTE)0x04)
#define PBC_DXB_SUBSCRIBERTABLE           ((UBYTE)0x07)
#define PBC_TIME_AR                       ((UBYTE)0x08)
#define PBC_USER_PRM_DATA                 ((UBYTE)0x81)

typedef struct
{
   UBYTE bLength;
   UBYTE bCommand;
   UBYTE bSlotNr;
   UBYTE bIndex;
   UBYTE bUserPrmData;
}  STRUC_PRM_BLOCK;
#define VPC3_STRUC_PRM_BLOCK_PTR  STRUC_PRM_BLOCK  VPC3_PTR

typedef struct
{
   UBYTE bLength;
   UBYTE bCommand;
   UBYTE bSlotNr;
   UBYTE bIndex;
   UBYTE abCsInterval[2];
   UBYTE abCsDelayInterval[8];
}  STRUC_PRM_BLOCK_CS;
#define VPC3_STRUC_PRM_BLOCK_CS_PTR  STRUC_PRM_BLOCK_CS  VPC3_PTR

typedef struct
{
   UBYTE bLength;
   UBYTE bCommand;
   UBYTE bSlotNr;
   UBYTE bSpecifier;
   UBYTE bFunction;
   UBYTE bProperties;
   UWORD wOutputHoldTime;
}  STRUC_PRM_BLOCK_PRMCMD;
#define VPC3_STRUC_PRM_BLOCK_PRMCMD_PTR  STRUC_PRM_BLOCK_PRMCMD  VPC3_PTR

/*---------------------------------------------------------------------------*/
/* 2.7 returncodes prm data                                                  */
/*---------------------------------------------------------------------------*/
#define VPC3_PRM_FINISHED                 ((UBYTE)0x00)
#define VPC3_PRM_CONFLICT                 ((UBYTE)0x01)
#define VPC3_PRM_NOT_ALLOWED              ((UBYTE)0x03)

/*---------------------------------------------------------------------------*/
/* 2.8 macros for prm data                                                   */
/*---------------------------------------------------------------------------*/
#if VPC3_SERIAL_MODE

   // read length of prm-Data --------------------------------------------------
   #define VPC3_GET_PRM_LEN()             Vpc3Read( bVpc3RwLenPrmData )
   // get pointer to prm-buffer ------------------------------------------------
   #define VPC3_GET_PRM_BUF_PTR()         (VPC3_UNSIGNED8_PTR)(((VPC3_ADR)( Vpc3Read   ( bVpc3RwPrmBufPtr )) << SEG_MULDIV)+((VPC3_ADR)Vpc3AsicAddress))
   // acknowledge prm-data -----------------------------------------------------
   #define VPC3_SET_PRM_DATA_NOT_OK()     Vpc3Read    ( bVpc3RoUserPrmDataNotOkay )
   #define VPC3_SET_PRM_DATA_OK()         Vpc3Read    ( bVpc3RoUserPrmDataOkay )

#else

   // read length of prm-Data --------------------------------------------------
   /*!
      \def VPC3_GET_PRM_LEN()
      \brief Fetch parameter buffer length.
      
      \retval length of the parameter data buffer. 
   */
   #define VPC3_GET_PRM_LEN()             pVpc3->bLenPrmData  VPC3_EXTENSION
   
   // get pointer to prm-buffer ------------------------------------------------
   /*!
      \def VPC3_GET_PRM_BUF_PTR()
      \brief Fetch pointer to parameter buffer.
      
      \retval Address parameter data buffer.
   */
   #define VPC3_GET_PRM_BUF_PTR()         (VPC3_UNSIGNED8_PTR)(((VPC3_ADR)(pVpc3->bPrmBufPtr VPC3_EXTENSION) << SEG_MULDIV)+((VPC3_ADR)Vpc3AsicAddress))
   
   // acknowledge prm-data -----------------------------------------------------
   /*!
      \def VPC3_SET_PRM_DATA_NOT_OK()
      \brief Acknowledge parameter assignment.
      
      The received parameter assignment isn't OK. The transferred parameter can't be used in the device. 

      \retval VPC3_PRM_FINISHED - No further parameter assignment message is present --> end of sequence.
      \retval VPC3_PRM_CONFLICT - Another parameter assignment message is present --> repeat check of requested parameter assignment. 
      \retval VPC3_PRM_NOT_ALLOWED - Access in present bus mode is not permitted.
   */
   #define VPC3_SET_PRM_DATA_NOT_OK()     pVpc3->sCtrlPrm.sRead.bUserPrmDataNotOkay  VPC3_EXTENSION

   /*!
      \def VPC3_SET_PRM_DATA_OK()
      \brief Acknowledge parameter assignment.
      
      The received parameter assignment is OK.
      
      \retval VPC3_PRM_FINISHED - No further parameter assignment message is present --> end of sequence.
      \retval VPC3_PRM_CONFLICT - Another parameter assignment message is present --> repeat check of requested parameter assignment. 
      \retval VPC3_PRM_NOT_ALLOWED - Access in present bus mode is not permitted.
   */
   #define VPC3_SET_PRM_DATA_OK()         pVpc3->sCtrlPrm.sRead.bUserPrmDataOkay     VPC3_EXTENSION

#endif//#if VPC3_SERIAL_MODE

/*-----------------------------------------------------------------------------------------------------------*/
/* 3.0 configuration telegram                                                                                */
/*-----------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/* 3.1 standard format of cfg byte                                           */
/*---------------------------------------------------------------------------*/
//
//        MSB                         LSB
//       +---+---+---+---+---+---+---+---+
//Bit-No | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
//       +---+---+---+---+---+---+---+---+
//         |   |   |   |   |   |   |   |      meaning:
//         |   |   |   |   |   |   |   |
//         |   |   |   |   +---+---+---+----  length of data
//         |   |   |   |                      00 1 byte resp. 1 word
//         |   |   |   |                      .
//         |   |   |   |                      .
//         |   |   |   |                      15 16 byte resp. 16 words
//         |   |   |   |
//         |   |   |   |
//         |   |   +---+--------------------  input/output
//         |   |                              00 specific identifier formats
//         |   |                              01 input
//         |   |                              10 output
//         |   |                              11 input-output
//         |   |
//         |   +----------------------------  length format 1)
//         |                                   0 byte byte structure
//         |                                   1 word word structure
//         |
//         +--------------------------------  consistency over
//                                             0 byte or word
//                                             1 whole length
//

/*---------------------------------------------------------------------------*/
/* 3.2 special format of cfg byte                                            */
/*---------------------------------------------------------------------------*/
//
//        MSB                         LSB
//       +---+---+---+---+---+---+---+---+
//Bit-No | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
//       +---+---+---+---+---+---+---+---+
//         |   |   |   |   |   |   |   |      meaning:
//         |   |   |   |   |   |   |   |
//         |   |   |   |   |   |   |   |
//         |   |   |   |   +---+---+---+----- length of manufacturer
//         |   |   |   |                      specific data 2)
//         |   |   |   |
//         |   |   +---+--------------------- fixed to 00
//         |   |
//         +---+----------------------------- input/output
//                                            00 free place
//                                            01 it follows 1 length byte for inputs
//                                            10 it follows 1 length byte for outputs
//                                            11 it follows - 1 length byte for outputs
//                                                          - 1 length byte for inputs
//
// -----------------------------------------------------------------------------
// 2) The length information of the manufacturer specific data shall be interpreted as follows:
//
// In case of Chk_Cfg:
//
//   0             No manufacturer specific data follow; no data in Real_Cfg_Data.
//   1 to 14       Manufacturer specific data of specified length follow;
//                 these shall be identical with the data in Real_Cfg_Data.
//   15            No manufacturer specific data follow;
//                 the verification can be omitted
//
// In case of DDLM_Get_Cfg :
//
//   0             No manufacturer specific data follow
//   1 to 14       Manufacturer specific data with specified length follow
//   15            Not allowed

// The structure of the length bytes is as follows:
//
//        MSB                         LSB
//       +---+---+---+---+---+---+---+---+
//Bit-No | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
//       +---+---+---+---+---+---+---+---+
//         |   |   |   |   |   |   |   |      meaning:
//         |   |   |   |   |   |   |   |
//         |   |   +---+---+---+---+---+----- length of inputs/outputs
//         |   |                              00 1 byte resp. 1 word
//         |   |                              .
//         |   |                              .
//         |   |                              63 64 byte resp. 64 words
//         |   |
//         |   +----------------------------- Length format 3)
//         |                                   0 byte byte structure
//         |                                   1 word word structure
//         |
//         +--------------------------------- consistency over
//                                             0 byte or word
//                                             1 whole length

/*---------------------------------------------------------------------------*/
/* 3.3 literals for cfg-bytes                                                */
/*---------------------------------------------------------------------------*/
#define VPC3_CFG_IS_BYTE_FORMAT           ((UBYTE)0x30)
#define VPC3_CFG_BF_LENGTH                ((UBYTE)0x0f)
#define VPC3_CFG_LENGTH_IS_WORD_FORMAT    ((UBYTE)0x40)
#define VPC3_CFG_BF_INP_EXIST             ((UBYTE)0x10)
#define VPC3_CFG_BF_OUTP_EXIST            ((UBYTE)0x20)
#define VPC3_CFG_SF_OUTP_EXIST            ((UBYTE)0x80)
#define VPC3_CFG_SF_INP_EXIST             ((UBYTE)0x40)
#define VPC3_CFG_SF_LENGTH                ((UBYTE)0x3f)

/*---------------------------------------------------------------------------*/
/* 3.4 returncodes cfg data                                                  */
/*---------------------------------------------------------------------------*/
#define VPC3_CFG_FINISHED                 ((UBYTE)0x00)
#define VPC3_CFG_CONFLICT                 ((UBYTE)0x01)
#define VPC3_CFG_NOT_ALLOWED              ((UBYTE)0x03)

/*---------------------------------------------------------------------------*/
/* 3.5 literals for chk config data                                          */
/*---------------------------------------------------------------------------*/
#define VPC3_CFG_OK                       ((UBYTE)0x00)
#define VPC3_CFG_FAULT                    ((UBYTE)0x01)
#define VPC3_CFG_UPDATE                   ((UBYTE)0x02)

/*---------------------------------------------------------------------------*/
/* 3.6 macros for cfg data                                                   */
/*---------------------------------------------------------------------------*/
#if VPC3_SERIAL_MODE

   // the Config-Buffer must be exchanged for the Read_Config-Buffer
   #define VPC3_UPDATE_CFG_BUFFER()          Vpc3Write( bVpc3WoModeReg1_S, VPC3_EN_CHG_CFG_BUFFER )
   // read length of cfg-buffers ----------------------------------------------
   #define VPC3_GET_READ_CFG_LEN()           Vpc3Read( bVpc3RwLenReadCfgData )
   #define VPC3_GET_CFG_LEN()                Vpc3Read( bVpc3RwLenCfgData )
   // set length of cfg-data --------------------------------------------------
   #define VPC3_SET_READ_CFG_LEN(LEN)        Vpc3Write( bVpc3RwLenReadCfgData, LEN )
   // get pointer to cfg-buffers ----------------------------------------------
   #define VPC3_GET_READ_CFG_BUF_PTR()       (VPC3_UNSIGNED8_PTR)(((VPC3_ADR)( Vpc3Read   ( bVpc3RwReadCfgBufPtr )) << SEG_MULDIV ) + ((VPC3_ADR)Vpc3AsicAddress ))
   #define VPC3_GET_CFG_BUF_PTR()            (VPC3_UNSIGNED8_PTR)(((VPC3_ADR)( Vpc3Read   ( bVpc3RwCfgBufPtr     )) << SEG_MULDIV ) + ((VPC3_ADR)Vpc3AsicAddress ))
   // acknowledge prm-data -----------------------------------------------------
   #define VPC3_SET_CFG_DATA_NOT_OK()        Vpc3Read( bVpc3RoUserCfgDataNotOkay )
   #define VPC3_SET_CFG_DATA_OK()            Vpc3Read( bVpc3RoUserCfgDataOkay )

#else

   // the Config-Buffer must be exchanged for the Read_Config-Buffer

   /*!
      \def VPC3_UPDATE_CFG_BUFFER()
   
      \brief Config buffer will be exchanged with Read Config buffer.
   */
   #define VPC3_UPDATE_CFG_BUFFER()          pVpc3->sCtrlPrm.sWrite.bModeReg1_S VPC3_EXTENSION = VPC3_EN_CHG_CFG_BUFFER

   // read length of cfg-buffers ----------------------------------------------
   /*!
      \def VPC3_GET_READ_CFG_LEN()
   
      \brief Fetch configuration length of read config buffer.
   
      \retval Length of entries in the read configuration buffer.
   */
   #define VPC3_GET_READ_CFG_LEN()           pVpc3->bLenReadCfgData VPC3_EXTENSION

   /*!
      \def VPC3_GET_CFG_LEN()
   
      \brief Fetch configuration length of check config buffer.
   
      \retval Length of entries in the check configuration buffer.
   */
   #define VPC3_GET_CFG_LEN()                pVpc3->bLenCfgData VPC3_EXTENSION

   /*!
      \def VPC3_SET_READ_CFG_LEN(LEN)
   
      \brief Set length of read config buffer.
   
      \param Length of entries in the read configuration buffer.
   */
   #define VPC3_SET_READ_CFG_LEN(LEN)        pVpc3->bLenReadCfgData VPC3_EXTENSION = LEN
   
   // get pointer to cfg-buffers ----------------------------------------------
   /*!
      \def VPC3_GET_READ_CFG_BUF_PTR()
   
      \brief Fetch pointer to read config buffer.
   
      \retval Address of read configuration buffer.
   */
   #define VPC3_GET_READ_CFG_BUF_PTR()       (VPC3_UNSIGNED8_PTR)(((VPC3_ADR)( pVpc3->bReadCfgBufPtr VPC3_EXTENSION) << SEG_MULDIV )+((VPC3_ADR)Vpc3AsicAddress))

   /*!
      \def VPC3_GET_CFG_BUF_PTR()
   
      \brief Fetch pointer to check config buffer.
   
      \retval Address of check configuration buffer.
   */
   #define VPC3_GET_CFG_BUF_PTR()            (VPC3_UNSIGNED8_PTR)(((VPC3_ADR)( pVpc3->bCfgBufPtr     VPC3_EXTENSION) << SEG_MULDIV )+((VPC3_ADR)Vpc3AsicAddress))

   // acknowledge cfg-data -----------------------------------------------------
   /*!
      \def VPC3_SET_CFG_DATA_NOT_OK()
      \brief Acknowledge configuration message.
      
      The received configuration message isn't OK.
      
      \retval VPC3_CFG_FINISHED - No further configuration message is present --> end of sequence.
      \retval VPC3_CFG_CONFLICT - An additional configuration message is present --> repeat verification of the requested configuration. 
      \retval VPC3_CFG_NOT_ALLOWED - Access is not permitted in the present bus mode.
   */
   #define VPC3_SET_CFG_DATA_NOT_OK()        pVpc3->sCtrlPrm.sRead.bUserCfgDataNotOkay  VPC3_EXTENSION

   /*!
      \def VPC3_SET_CFG_DATA_OK()
      \brief Acknowledge configuration message.
      
      The received configuration message is OK.
      
      \retval VPC3_CFG_FINISHED - No further configuration message is present --> end of sequence.
      \retval VPC3_CFG_CONFLICT - An additional configuration message is present --> repeat verification of the requested configuration. 
      \retval VPC3_CFG_NOT_ALLOWED - Access is not permitted in the present bus mode.
   */
   #define VPC3_SET_CFG_DATA_OK()            pVpc3->sCtrlPrm.sRead.bUserCfgDataOkay     VPC3_EXTENSION

#endif//#if VPC3_SERIAL_MODE

/*---------------------------------------------------------------------------*/
/* 3.7 structure for real cfg data                                           */
/*---------------------------------------------------------------------------*/
// -- cfg structure -----------------------------------------------------------
/*!
  \brief Structure for configuration data.
*/
typedef struct
{
    UBYTE bLength;                  /*!< length of configuration data */
    UBYTE abData[ CFG_BUFSIZE ];    /*!< array of configuration data  */   
}  CFG_STRUCT;


/*-----------------------------------------------------------------------------------------------------------*/
/* 4.0 input / output                                                                                        */
/*-----------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/* 4.1 structure for calculated input-/output-length                         */
/*---------------------------------------------------------------------------*/
typedef struct
{
   UBYTE bInputDataLength;               // calculated input length  (VPC3->DP-Master)
   UBYTE bOutputDataLength;              // calculated output length (DP-Master->VPC3)
}   VPC3_IO_DATA_LEN;

/*---------------------------------------------------------------------------*/
/* 4.2 states for output buffer                                              */
/*---------------------------------------------------------------------------*/
#define VPC3_NEW_DOUT_BUF                 ((UBYTE)0x04)
#define VPC3_DOUT_BUF_CLEARED             ((UBYTE)0x08)

/*---------------------------------------------------------------------------*/
/* 4.3 macros for input buffer                                               */
/*---------------------------------------------------------------------------*/
#if VPC3_SERIAL_MODE
   #define VPC3_INPUT_UPDATE()         Vpc3Read( bVpc3RoNewDinBufferCmd )
#else
   #define VPC3_INPUT_UPDATE()         pVpc3->sCtrlPrm.sRead.bNewDinBufferCmd  VPC3_EXTENSION
#endif//#if VPC3_SERIAL_MODE

/*-----------------------------------------------------------------------------------------------------------*/
/* 5.0 set slave address                                                                                     */
/*-----------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/* 5.1 structure                                                             */
/*---------------------------------------------------------------------------*/
typedef struct
{
   UBYTE bTsAddr;
   UBYTE bIdentHigh;
   UBYTE bIdentLow;
   UBYTE bNoAddressChanged;
}  STRUC_SSA_BLOCK;
#define VPC3_STRUC_SSA_BLOCK_PTR  STRUC_SSA_BLOCK  VPC3_PTR
#define MEM_STRUC_SSA_BLOCK_PTR  STRUC_SSA_BLOCK  MEM_PTR

/*---------------------------------------------------------------------------*/
/* 5.2 macros                                                                */
/*---------------------------------------------------------------------------*/
#if VPC3_SERIAL_MODE

   // -- read length of set-slave-address ----------------------------------------
   #define VPC3_GET_SSA_LEN()  Vpc3Read( bVpc3RwLenSsaBuf )
   // -- get pointer to ssa buffer -----------------------------------------------
   #define VPC3_GET_SSA_BUF_PTR() (VPC3_UNSIGNED8_PTR)(((VPC3_ADR)( Vpc3Read( bVpc3RwSsaBufPtr )) << SEG_MULDIV ) + ((VPC3_ADR)Vpc3AsicAddress ))
   // -- acknowledge ssa commando ------------------------------------------------
   #define VPC3_FREE_SSA_BUF() Vpc3Read( bVpc3RoSsaBufferFreeCmd )

#else

   // -- read length of set-slave-address ----------------------------------------
   /*!
      \def VPC3_GET_SSA_LEN()
   
      \brief Fetch length of set slave address message.
   
      \retval Length of set slave address telegram.
   */
   #define VPC3_GET_SSA_LEN()  pVpc3->bLenSsaBuf  VPC3_EXTENSION

   // -- get pointer to ssa buffer -----------------------------------------------
   /*!
      \def VPC3_GET_SSA_BUF_PTR()
   
      \brief Fetch pointer to set slave address buffer.
   
      \retval Address of set slave address buffer.
   */
   #define VPC3_GET_SSA_BUF_PTR() (VPC3_UNSIGNED8_PTR)(((VPC3_ADR)(pVpc3->bSsaBufPtr VPC3_EXTENSION) << SEG_MULDIV)+((VPC3_ADR)Vpc3AsicAddress))
   
   // -- acknowledge ssa commando ------------------------------------------------
   /*!
      \def VPC3_FREE_SSA_BUF()
      \brief Acknowledgment of the Set_Slave_Address service.
      
      \retval VPC3_SSA_FINISHED - No further ssa message is present --> end of sequence.
      \retval VPC3_SSA_CONFLICT - An additional ssa message is present --> repeat verification of the requested ssa. 
      \retval VPC3_SSA_NOT_ALLOWED - Access is not permitted in the present bus mode.
   */
   #define VPC3_FREE_SSA_BUF() pVpc3->sCtrlPrm.sRead.bSsaBufferFreeCmd VPC3_EXTENSION

#endif//#if VPC3_SERIAL_MODE

/*---------------------------------------------------------------------------*/
/* 5.3 returncodes ssa service                                               */
/*---------------------------------------------------------------------------*/
#define VPC3_SSA_FINISHED                 ((UBYTE)0x00)
#define VPC3_SSA_CONFLICT                 ((UBYTE)0x01)
#define VPC3_SSA_NOT_ALLOWED              ((UBYTE)0x03)


/*-----------------------------------------------------------------------------------------------------------*/
/* 6.0 global control telegram                                                                               */
/*-----------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/* 6.1 defines for GLOBAL CONTROL                                            */
/*---------------------------------------------------------------------------*/
#define GC_OPERATE                        ((UBYTE)0x00)
#define GC_CLEAR                          ((UBYTE)0x02)

/*---------------------------------------------------------------------------*/
/* 6.2 macros for global control                                             */
/*---------------------------------------------------------------------------*/
// -- read Global-Control-Command ---------------------------------------------
#if VPC3_SERIAL_MODE
   #define VPC3_GET_GC_COMMAND() Vpc3Read( bVpc3RwGcCommand )
#else
   /*!
      \def VPC3_GET_GC_COMMAND()
      \brief Fetch the global control command.
      
      \retval Bit 0 - reserved
      \retval Bit 1 - Clear_Data - With this command, the output data is cleared and made available to the user.
      \retval Bit 2 - Unfreeze - With "Unfreeze", freezing the input data is cancelled.
      \retval Bit 3 - Freeze - The input data is "frozen". The application will fetch new input data only if the master sends the next "freeze" command. 
      \retval Bit 4 - Usync - The command "Unsync" cancels the "Sync"command.
      \retval Bit 5 - Sync - The output data received last is made availabe to the application. The output
                        data transmitted subsequently is passed to the application only after the next
                        "Sync" command is issued.
      \retval Bit 6 - reserved
      \retval Bit 7 - reserved
   */
   #define VPC3_GET_GC_COMMAND() pVpc3->bGcCommand VPC3_EXTENSION
#endif//#if VPC3_SERIAL_MODE

/*-----------------------------------------------------------------------------------------------------------*/
/* 7.0 diagnostic telegram                                                                                   */
/*-----------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/* 7.1 defines                                                               */
/*---------------------------------------------------------------------------*/
#define EXT_DIAG_RESET                    ((UBYTE)0x00)
#define DIAG_RESET                        ((UBYTE)0x00)
#define EXT_DIAG_SET                      ((UBYTE)0x01)
#define STAT_DIAG_RESET                   ((UBYTE)0x00)
#define STAT_DIAG_SET                     ((UBYTE)0x02)
#define EXT_STAT_DIAG_SET                 ((UBYTE)0x03)
#define OVF_DIAG_SET                      ((UBYTE)0x04)
#define DIAG_WAIT_FOR_ALARM               ((UBYTE)0x80)

#define DIAG_REV_SIGN                     ((UBYTE)0xC0)
#define DIAG_KEN_SIGN                     ((UBYTE)0x40)
#define DIAG_CHN_SIGN                     ((UBYTE)0x80)
#define DIAG_DEV_SIGN                     ((UBYTE)0x00)

#define DIAG_TYPE_MASK                    ((UBYTE)0xC0)
#define DIAG_TYPE_DEV                     ((UBYTE)0x00)
#define DIAG_TYPE_KEN                     ((UBYTE)0x40)
#define DIAG_TYPE_CHN                     ((UBYTE)0x80)
#define DIAG_TYPE_REV                     ((UBYTE)0xC0)

#define DIAG_NORM_DIAG_SIZE               ((UBYTE)0x06)
#define DIAG_TYPE_CHN_SIZE                ((UBYTE)0x03)
#define DIAG_TYPE_REV_SIZE                ((UBYTE)0x01)

#define STATUS_DIAG_HEAD_SIZE             ((UBYTE)0x04)
#define STATUS_TYPE_SIGN                  ((UBYTE)0x80)
#define STATUS_TYPE_STATUS_MESSAGE        ((UBYTE)0x01)
#define STATUS_TYPE_MODUL_STATUS          ((UBYTE)0x02)
#define STATUS_TYPE_PRM_COMMAND_ACK       ((UBYTE)0x1E)
#define STATUS_TYPE_H_STATUS_MESSAGE      ((UBYTE)0x1F)
#define STATUS_TYPE_MANU_MIN              ((UBYTE)0x20)
#define STATUS_TYPE_MANU_MAX              ((UBYTE)0x7E)

#define ALARM_TYPE_SIGN                   ((UBYTE)0x00)
#define ALARM_DIAG_HEAD_SIZE              ((UBYTE)0x04)

/*---------------------------------------------------------------------------*/
/* 7.2 structures                                                            */
/*---------------------------------------------------------------------------*/
// -- revision diagnosis data -------------------------------------------------
typedef struct
{
   UBYTE   bRevision;
}  STRUC_REV_DIAG;

// -- slot signed diagnosis data ----------------------------------------------
typedef struct
{
   UBYTE   bHeader;
   UBYTE   abIdentifierDiagnosis[1];
}  STRUC_KEN_DIAG;

//defines for channel related diagnostic
//        MSB                         LSB
//       +---+---+---+---+---+---+---+---+
//Bit-No | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
//       +---+---+---+---+---+---+---+---+
//         |   |   |   |   |   |   |   |    meaning:
//         |   |   |   |   |   |   |   |
//         |   |   |   +---+---+---+---+--- error type
//         |   |   |
//         +---+---+----------------------- channel type

//channel type of statuscode
#define STATUS_1BIT                       ((UBYTE)0x20)
#define STATUS_2BIT                       ((UBYTE)0x40)
#define STATUS_4BIT                       ((UBYTE)0x60)
#define STATUS_BYTE                       ((UBYTE)0x80)
#define STATUS_WORD                       ((UBYTE)0xA0)
#define STATUS_DWORD                      ((UBYTE)0xC0)

//error type of statuscode (PROFIBUS NORM)
#define STATUS_SHORT_CIRCUIT              ((UBYTE)0x01)
#define STATUS_UNDERVOLTAGE               ((UBYTE)0x02)
#define STATUS_OVERVOLTAGE                ((UBYTE)0x03)
#define STATUS_OVERLOAD                   ((UBYTE)0x04)
#define STATUS_OVERTEMPERATURE            ((UBYTE)0x05)
#define STATUS_LINE_BREAK                 ((UBYTE)0x06)
#define STATUS_UPPER_LIMIT_VALUE          ((UBYTE)0x07)
#define STATUS_LOWER_LIMIT_VALUE          ((UBYTE)0x08)
#define STATUS_ERROR                      ((UBYTE)0x09)
#define STATUS_RESERVED_10                ((UBYTE)0x0A)
#define STATUS_RESERVED_11                ((UBYTE)0x0B)
#define STATUS_RESERVED_12                ((UBYTE)0x0C)
#define STATUS_RESERVED_13                ((UBYTE)0x0D)
#define STATUS_RESERVED_14                ((UBYTE)0x0E)
#define STATUS_RESERVED_15                ((UBYTE)0x0F)

//manufacturer specific between 16 and 31
//Siemens Error Codes
#define STATUS_PRM_ERROR                  ((UBYTE)0x10)
#define STATUS_VOLTAGE                    ((UBYTE)0x11)
#define STATUS_FUSE_ERROR                 ((UBYTE)0x12)
#define STATUS_GND_ERROR                  ((UBYTE)0x13)
#define STATUS_REF_ERROR                  ((UBYTE)0x14)
#define STATUS_UNKNOWN_ERROR              ((UBYTE)0x1B)

//defines for channel number in/out
#define CH_NUMBER_INOUT                   ((UBYTE)0xC0)
#define CH_NUMBER_OUT                     ((UBYTE)0x80)
#define CH_NUMBER_IN                      ((UBYTE)0x40)

// -- channel diagnosis data --------------------------------------------------
typedef struct
{
   UBYTE   bHeader;
   UBYTE   bChannel;
   UBYTE   bTypeOfDiagnosis;
}  STRUC_CHN_DIAG;

// -- device diagnosis data ---------------------------------------------------
typedef struct
{
   UBYTE   bHeader;
   UBYTE   abDeviceRelatedDiagnosis[9];
}  STRUC_DEV_DIAG;

// -- status message data (coded as device diagnosis (see above) --------------
typedef struct
{
   UBYTE   bHeader;           // equal to DIAG_DEV_SIGN
   UBYTE   bAlarmType;
   UBYTE   bSlotNr;
   UBYTE   bSpecifier;
   UBYTE   abAlarmDataDescription[1];
}  STRUC_STATUS_DIAG;

// alarm data (coded as device diagnosis (see above) --------------------------
typedef struct
{
   UBYTE   bHeader;           // equal to DIAG_DEV_SIGN
   UBYTE   bAlarmType;
   UBYTE   bSlotNr;
   UBYTE   bSpecifier;
   UBYTE   abAlarmData[1];
}  STRUC_ALARM_DIAG;
#define STRUC_ALARM_DIAG_PTR  STRUC_ALARM_DIAG  MEM_PTR

typedef union
{
   STRUC_REV_DIAG     sRevisionDiag;
   STRUC_KEN_DIAG     sIdentifierRelatedDiag;
   STRUC_CHN_DIAG     sChannelRelatedDiag;
   STRUC_DEV_DIAG     sDeviceRelatedDiag;
   STRUC_STATUS_DIAG  sStatusAlarm;
   STRUC_ALARM_DIAG   sAlarm;
} UNION_DIAG;
#define MEM_DIAG_PTR   UNION_DIAG  MEM_PTR

typedef union
{
   MEM_DIAG_PTR       pToDiagStructure;
   MEM_UNSIGNED8_PTR  pToDiagArray;

} MEM_UNION_DIAG;

/*---------------------------------------------------------------------------*/
/* 7.3 macros                                                                */
/*---------------------------------------------------------------------------*/
//-- Control diagnostics buffer ------------
#if VPC3_SERIAL_MODE
   #define VPC3_GET_DIAG_FLAG()  ( Vpc3Read( bVpc3RoStatus_L ) & VPC3_DIAG_FLAG )
#else
   /*!
      \def VPC3_GET_DIAG_FLAG()
      \brief Fetch the status of the diagnostic buffer.
      
      \retval TRUE - diagnostic buffer has not been sent (or static diagnosis)
      \retval FALSE - the diagnostic buffer has been sent
   */
   #define VPC3_GET_DIAG_FLAG()  ( pVpc3->sReg.sRead.bStatus_L VPC3_EXTENSION & VPC3_DIAG_FLAG )
#endif//#if VPC3_SERIAL_MODE


/*-----------------------------------------------------------------------------------------------------------*/
/* 8.0 subscriber                                                                                            */
/*-----------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/* 8.1 structures                                                            */
/*---------------------------------------------------------------------------*/
#ifdef DP_SUBSCRIBER
typedef struct
{
   UBYTE       bPublisherAddress;
   UBYTE       bLinkStatus;
}  DXB_STATUS;

typedef struct
{
   UBYTE       bHeader;
   UBYTE       bStatusType;
   UBYTE       bSlotNr;
   UBYTE       bSpecifier;
   DXB_STATUS  bLinkStatus[MAX_LINK_SUPPORTED];
}  VPC3_DXB_LINK_STATUS;
#define VPC3_DXB_LINK_STATUS_PTR  VPC3_DXB_LINK_STATUS  VPC3_PTR

typedef struct
{
   UBYTE       bPublisherAddress;
   UBYTE       bPublisherLength;
   UBYTE       bSampleOffset;
   UBYTE       bSampleLength;
}  DXB_LINK_TABLE;

typedef struct
{
   DXB_LINK_TABLE  sLinkTableEntry[MAX_LINK_SUPPORTED];
}  VPC3_DXB_LINK_TABLE;
#define VPC3_DXB_LINK_TABLE_PTR  VPC3_DXB_LINK_TABLE  VPC3_PTR

typedef struct
{
   UBYTE       bPublisherAddress;
   UBYTE       bPublisherLength;
   UBYTE       bSampleOffset;
   UBYTE       bDestinationSlotNumber;
   UBYTE       bOffsetDataArea;
   UBYTE       bSampleLength;
}  DXB_SUBSCRIBER_TABLE;

typedef struct
{
   UBYTE                   bVersion;
   DXB_LINK_TABLE          sLinkTableEntry[MAX_LINK_SUPPORTED];
}  PRM_DXB_LINK_TABLE;
#define PRM_DXB_LINK_TABLE_PTR  PRM_DXB_LINK_TABLE  VPC3_PTR

typedef struct
{
   UBYTE                   bVersion;
   DXB_SUBSCRIBER_TABLE    bSubscriberTableEntry[MAX_LINK_SUPPORTED];
}  PRM_SUBSCRIBER_TABLE;
#define PRM_SUBSCRIBER_TABLE_PTR  PRM_SUBSCRIBER_TABLE  VPC3_PTR

typedef struct
{
   UBYTE       bPublisherAddress;
   UBYTE       bSampleLength;
   UBYTE       bSampleData;
}  VPC3_DXB_OUT;
#define VPC3_DXB_OUT_PTR  VPC3_DXB_OUT  VPC3_PTR
#endif//#ifdef DP_SUBSCRIBER

/*---------------------------------------------------------------------------*/
/* 8.2 macros                                                                */
/*---------------------------------------------------------------------------*/
#if VPC3_SERIAL_MODE

   #define VPC3_GET_NEXT_DXB_OUT_BUFFER_CMD()      ( Vpc3Read( bVpc3RoNextDoutBufferCmd ) & ( VPC3_NEW_DOUT_BUF | VPC3_DOUT_BUF_CLEARED ) )
   #define VPC3_GET_DXB_OUT_BUFFER_SM()            ( Vpc3Read( bVpc3RoDoutBufferSm ) & 0x30 )

   #define VPC3_GET_DXB_LINK_STATUS_BUF_PTR()      (VPC3_UNSIGNED8_PTR)(((VPC3_ADR)( Vpc3Read( bVpc3RwDxbStatusBufPtr )) << SEG_MULDIV ) + ((VPC3_ADR)Vpc3AsicAddress ))
   #define VPC3_GET_DXB_LINK_STATUS_LEN()          Vpc3Read( bVpc3RwLenDxbStatusBuf )
   #define VPC3_SET_DXB_LINK_STATUS_LEN(LEN)       Vpc3Write( bVpc3RwLenDxbStatusBuf, LEN )

   #define VPC3_GET_DXB_LINK_TABLE_BUF_PTR()       (VPC3_UNSIGNED8_PTR)(((VPC3_ADR)( Vpc3Read( bVpc3RwDxbLinkBufPtr )) << SEG_MULDIV ) + ((VPC3_ADR)Vpc3AsicAddress ))
   #define VPC3_GET_DXB_LINK_TABLE_LEN()           Vpc3Read( bVpc3RwLenDxbLinkBuf )
   #define VPC3_SET_DXB_LINK_TABLE_LEN(LEN)        Vpc3Write( bVpc3RwLenDxbLinkBuf, LEN )

#else

   #define VPC3_GET_NEXT_DXB_OUT_BUFFER_CMD()      ( pVpc3->sCtrlPrm.sRead.bNextDoutBufferCmd VPC3_EXTENSION & ( VPC3_NEW_DOUT_BUF | VPC3_DOUT_BUF_CLEARED ) )
   #define VPC3_GET_DXB_OUT_BUFFER_SM()            ( pVpc3->sCtrlPrm.sRead.bDoutBufferSm VPC3_EXTENSION & 0x30 )

   #define VPC3_GET_DXB_LINK_STATUS_BUF_PTR()      (VPC3_UNSIGNED8_PTR)(((VPC3_ADR)(pVpc3->bDxbStatusBufPtr VPC3_EXTENSION) << SEG_MULDIV)+((VPC3_ADR)Vpc3AsicAddress))
   #define VPC3_GET_DXB_LINK_STATUS_LEN()          pVpc3->bLenDxbStatusBuf VPC3_EXTENSION
   #define VPC3_SET_DXB_LINK_STATUS_LEN(value)     pVpc3->bLenDxbStatusBuf VPC3_EXTENSION = value

   #define VPC3_GET_DXB_LINK_TABLE_BUF_PTR()       (VPC3_UNSIGNED8_PTR)(((VPC3_ADR)(pVpc3->bDxbLinkBufPtr VPC3_EXTENSION) << SEG_MULDIV)+((VPC3_ADR)Vpc3AsicAddress))
   #define VPC3_GET_DXB_LINK_TABLE_LEN()           pVpc3->bLenDxbLinkBuf VPC3_EXTENSION
   #define VPC3_SET_DXB_LINK_TABLE_LEN(value)      pVpc3->bLenDxbLinkBuf VPC3_EXTENSION = value

#endif//#if VPC3_SERIAL_MODE


/*-----------------------------------------------------------------------------------------------------------*/
/* 9.0 isochron mode                                                                                         */
/*-----------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/* 9.1 defines                                                               */
/*---------------------------------------------------------------------------*/
#define PLL_OFFLINE                       ((UBYTE)0x01)
#define PLL_WAIT_SYNCH                    ((UBYTE)0x02)
#define PLL_WAIT_DELAY                    ((UBYTE)0x04)
#define PLL_RUN                           ((UBYTE)0x08)
#define PLL_RUN_PLL                       ((UBYTE)0x10)

/*---------------------------------------------------------------------------*/
/* 9.2 structures                                                            */
/*---------------------------------------------------------------------------*/
typedef struct
{
   UBYTE   T_IMPLICIT;
   ULONG   T_BASE_DP;
   UWORD   T_DP;
   UBYTE   T_MAPC;
   ULONG   T_BASE_IO;
   UWORD   T_I;
   UWORD   T_O;
   ULONG   T_DX;
   UWORD   T_PLL_W;
   UWORD   T_PLL_D;
}  sIsochron;
#define psIsochron sIsochron  MEM_PTR

#define cSizeOfIsochron                   ((UBYTE)0x18)

/*-----------------------------------------------------------------------------------------------------------*/
/* 9.3 time stamp                                                                                            */
/*-----------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/* 9.4 defines                                                               */
/*---------------------------------------------------------------------------*/
// defines for command
#define CS_CM_STOP_CLOCK_SYNC             ((UBYTE)0x01)
#define CS_CM_IGNORE_CYCLIC_STATE_MACHINE ((UBYTE)0x02)
#define CS_CM_CLOCK_VALUE_CHECK_ENABLE    ((UBYTE)0x04)

/*---------------------------------------------------------------------------*/
/* 9.5 structures                                                            */
/*---------------------------------------------------------------------------*/
typedef struct
{
   struct
   {
      UBYTE bStatus;
      UBYTE bCommand;
      UBYTE bCvStatus1;
      UBYTE bCvStatus2;
      UBYTE abCvTimeEvent[8];
      UBYTE abRdTime[4];
      UBYTE abCvPrevTimeEvent[8];
   }  sClock;

   UBYTE    abCsInterval[2];
   UBYTE    abCsDelayInterval[8];
}  T_TIMESTAMP;

/*-----------------------------------------------------------------------------------------------------------*/
/* 10.0 fdl layer                                                                                            */
/*-----------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/* 10.1 SAPs (service access points) for MSAC1                               */
/*---------------------------------------------------------------------------*/
#define DP_C2_RM_SAP_NR                   ((UBYTE)0x31)
#define DP_C1_ALARM_SAP_NR                ((UBYTE)0x32)
#define DP_C1_RD_WR_SAP_NR                ((UBYTE)0x33)

/*---------------------------------------------------------------------------*/
/* 10.2 SAPs (service access points) for MSAC2                               */
/*---------------------------------------------------------------------------*/
#define MSAC_C2_MAX_INPUT_ITEMS           ((UBYTE)((2*DP_C2_NUM_SAPS)+1)) // number of elements in the input queue
#define MSAC_C2_SAP_NR_LOW                ((UBYTE)0x00)                   // lowest possible SAP for MSAC_C2
#define MSAC_C2_SAP_NR_HIGH               ((UBYTE)0x30)                   // highest possible SAP for MSAC_C2 (dp_mode =1)

/*---------------------------------------------------------------------------*/
/* 10.3 structure of a FDL-indication-response-buffer-head                   */
/*---------------------------------------------------------------------------*/
// -- defines for buffer-control-byte ----------------------------------------
#define PCNTRL_USER                       ((UBYTE)0x80)  // 1: buffer in user
#define PCNTRL_IND                        ((UBYTE)0x40)  // 1: indication occured
#define PCNTRL_RESP                       ((UBYTE)0x20)  // 1: response provided
#define PCNTRL_INUSE                      ((UBYTE)0x10)  // 1: buffer in use

// -- structure of a FDL-indication-response-buffer-head ---------------------
typedef struct
{
   UBYTE bControl;               // buffer-control
   UBYTE bMaxIndicationLength;   // max ind-buffer-length
   UBYTE bIndicationLength;      // actual ind-response-length
   UBYTE bFunctionCode;       // ind-response-function-code
}  FDL_BUFFER_HEADER;
#define FDL_BUFFER_HEADER_PTR FDL_BUFFER_HEADER  VPC3_PTR
#define cSizeFdlBufferHeader              ((UBYTE)0x04)

/*---------------------------------------------------------------------------*/
/* 10.4 structure of a FDL-sap-entry                                         */
/*---------------------------------------------------------------------------*/
typedef struct
{
   UBYTE bRespSendSapNr;
   UBYTE bReqSa;
   UBYTE bAccessReqSsap;
   UBYTE bEventServSup;
   UBYTE abVpc3SegIndPtr[2];
   UBYTE bVpc3SegRespPtr;
}  FDL_SAP_CNTRL;
#define FDL_SAP_CNTRL_PTR FDL_SAP_CNTRL  VPC3_PTR
#define cSizeFdlSapCntrlEntry   ((UBYTE)0x07)

// -- structure of a FDL-indication-response-buffer-head ---------------------
typedef struct
{
   UBYTE bControl;               /*!< control byte foR buffer management */
   UBYTE bMaxIndicationLength;   /*!< maximal length of indication buffer */
   UBYTE bIndicationLength;      /*!< current indication/response length */
   UBYTE bFunctionCode;          /*!< current indication/response-function-code */
   UBYTE bDpv1Service;           /*!< current DPV1 service */
   UBYTE bSlotNr;                /*!< current slot number */
   UBYTE bIndex;                 /*!< current index number */
   UBYTE bDataLength;            /*!< current data length */
}  MSG_HEADER;
#define MSG_HEADER_PTR MSG_HEADER  MEM_PTR
#define cSizeMsgHeader   ((UBYTE)0x08)

/*---------------------------------------------------------------------------*/
/* 10.5 structure of a sap-control-block                                     */
/*---------------------------------------------------------------------------*/
// -- defines for FDL SAP TYPE -----------------------------------------------
typedef enum
{
   FDL_RM_SAP      = ((UBYTE)0x00),
   FDL_MSAC_C1_SAP = ((UBYTE)0x01),
   FDL_MSAC_C2_SAP = ((UBYTE)0x02)
} FDL_SAP_TYPE;

// -- defines for FDL STATE --------------------------------------------------
typedef enum
{
   ENABLED          =  ((UBYTE)0x00),
   DISABLED         =  ((UBYTE)0x01),
   DISABLED_FOR_RM  =  ((UBYTE)0x02),
   DISABLED_IN_RM   =  ((UBYTE)0x03)
} FDL_STATE;

typedef struct
{
   FDL_BUFFER_HEADER_PTR   psToIndBuffer[2];       // ind-buffer-address uP-formatted
   FDL_BUFFER_HEADER_PTR   psToRespBuffer;         // resp-buffer-address uP-formatted

   FDL_SAP_CNTRL_PTR       psToSapCntrlList;       // pointer to VPC3-SAP uP-formatted
   FDL_SAP_CNTRL           sSapCntrlEntry;         // content of VPC3-sap

   MSG_HEADER              sMsgHeader;

   FDL_STATE               eFdlState;
   FDL_SAP_TYPE            eSapType;

   UBYTE                   abVpc3SegIndPtr[2];     // ind-buffer-address, VPC3-segmented format
   UBYTE                   bSapNr;
   UBYTE                   bReqSsap;
   UBYTE                   bIndBufLength;
   UBYTE                   bRespBufLength;
   UBYTE                   bServSup;
   UBYTE                   bReqSa;
}  FDL_SAP_HEADER;
#define FDL_SAP_HEADER_PTR  FDL_SAP_HEADER  MEM_PTR

/*---------------------------------------------------------------------------*/
/* 10.6 structure of a indication- or response-buffer                        */
/*---------------------------------------------------------------------------*/
typedef struct
{
   UBYTE data_len;       // length of netto-data
   UBYTE fc;             // function-code
   UBYTE user_data[246]; // netto-data
}  IND_RESP_BUF;
#define IND_RESP_BUF_PTR        IND_RESP_BUF  VPC3_PTR
#define MSAC_C2_DATA_BUF        IND_RESP_BUF
#define MSAC_C2_DATA_BUF_PTR    MSAC_C2_DATA_BUF  VPC3_PTR

/*---------------------------------------------------------------------------*/
/* 10.7 structure of the immediate-response-pdu of the FDL-RM-SAP            */
/*---------------------------------------------------------------------------*/
typedef struct
{
   UBYTE bDpv1Service;
   UBYTE bSapNr;
   UBYTE bSendTimeOutH;
   UBYTE bSendTimeOutL;
}  FDL_RM_ANSWER;
#define FDL_RM_ANSWER_PTR FDL_RM_ANSWER  VPC3_PTR

/*---------------------------------------------------------------------------*/
/* 10.8 global structure of FDL state machine                                */
/*---------------------------------------------------------------------------*/
// -- defines for resource manager state -------------------------------------
typedef enum
{
   RM_LOCKED       = ((UBYTE)0x01),    // do not treat rm-sap (channel not opened)
   RM_LOADABLE     = ((UBYTE)0x02),    // load rm-sap (immediate response was sent or channel was opened)
   RM_LOADED       = ((UBYTE)0x03)     // do not load rm-sap (immediate-response not sent yet)
} FDL_RM_STATE;

#if DP_FDL
   #define SIZE_OF_SEARCH_KEY ((UBYTE)0x41)
   typedef struct
   {
      UBYTE                   bOpenChannel;
      // saps for use of rm
      FDL_RM_STATE            eRmState;   // state of resource manager
      FDL_RM_ANSWER_PTR       psRmRespBuffer;
      // list for fast sap-finding
      UBYTE                   abSearchSap[ SIZE_OF_SEARCH_KEY ];
      // sap-list with any sap-information
      FDL_SAP_HEADER          asSapHeaderList[ FDL_SAP_MAX ];
   }  FDL_STRUC;
#endif//#if DP_FDL

#define FDL_RM_BUF_LENGTH                 ((UBYTE)0x04)
#define FDL_DPV2_SEC_BUF_LENGTH           ((UBYTE)0x04)

/*---------------------------------------------------------------------------*/
/* 10.9 function-codes                                                       */
/*---------------------------------------------------------------------------*/
#define SS_ALL                            ((UBYTE)0x00)
#define SS_SDN_L                          ((UBYTE)0x01)
#define SS_SDN_H                          ((UBYTE)0x02)
#define SS_SDN_LH                         ((UBYTE)0x03)
#define SS_SDN_SRD_LH                     ((UBYTE)0x04)
#define SS_SDA_L                          ((UBYTE)0x05)
#define SS_SDA_H                          ((UBYTE)0x06)
#define SS_SDA_LH                         ((UBYTE)0x07)
#define SS_SRD_DDB                        ((UBYTE)0x08)
#define SS_SRD_L                          ((UBYTE)0x09)
#define SS_SRD_H                          ((UBYTE)0x0A)
#define SS_SRD_LH                         ((UBYTE)0x0B)
#define SS_DDB_REQ                        ((UBYTE)0x0C)
#define SS_DDB_RES_L                      ((UBYTE)0x0D)
#define SS_DDB_RES_H                      ((UBYTE)0x0E)
#define SS_DDB_RES_LH                     ((UBYTE)0x0F)

#define FC_RESP_L                         ((UBYTE)0x08)
#define FC_RESP_H                         ((UBYTE)0x0A)

#define DEFAULT_SAP                       ((UBYTE)0x40)
#define SAP_ALL                           ((UBYTE)0x7F)
#define SA_ALL                            ((UBYTE)0x7F)

/*---------------------------------------------------------------------------*/
/* 10.A returncodes of FDL-services                                          */
/*---------------------------------------------------------------------------*/
#define FDL_OK                            ((UBYTE)0x00)
#define FDL_SNI                           ((UBYTE)0x01)
#define FDL_IUSE                          ((UBYTE)0x02)
#define FDL_IVP                           ((UBYTE)0x03)
#define FDL_ACT                           ((UBYTE)0x04)
#define FDL_DACT                          ((UBYTE)0x05)
#define FDL_SND                           ((UBYTE)0x06)
#define FDL_NUIB                          ((UBYTE)0x07)
#define FDL_UBLEN                         ((UBYTE)0x08)
#define FDL_FPTR                          ((UBYTE)0x09)
#define FDL_NRB                           ((UBYTE)0x0A)
#define FDL_NURB                          ((UBYTE)0x0B)
#define FDL_SBNI                          ((UBYTE)0x0C)
#define FDL_RLEN                          ((UBYTE)0x0D)
#define FDL_SBUF                          ((UBYTE)0x0E)
#define FDL_SDA                           ((UBYTE)0x0F)
#define FDL_NRD                           ((UBYTE)0x40)
#define FDL_OPEN_ERR                      ((UBYTE)0xAA)

#define FDL_RR                            ((UBYTE)0x10)    // do not change
#define FDL_RS                            ((UBYTE)0x20)    // do not change

#define FDL_PRIMARY_BUF                   ((UBYTE)0x01)
#define FDL_SECONDARY_BUF                 ((UBYTE)0x02)

/*---------------------------------------------------------------------------*/
/* 10.B helpful macros                                                       */
/*---------------------------------------------------------------------------*/
#if VPC3_SERIAL_MODE

   #define _INIT_FDL_BUF_HEAD( _dest, LEN )                    \
   {                                                           \
      VPC3_ADR dest = (_dest);                                 \
                                                               \
      Vpc3Write( dest + 0, 0 );                                \
      Vpc3Write( dest + 1, LEN );                              \
      Vpc3Write( dest + 2, 0 );                                \
      Vpc3Write( dest + 3, 0 );                                \
   }//#define _INIT_FDL_BUF_HEAD(FDL_PTR, LEN)

#else

   #define _INIT_FDL_BUF_HEAD( FDL_PTR, LEN )                  \
   {                                                           \
      ( FDL_PTR )->bControl             = 0;                   \
      ( FDL_PTR )->bIndicationLength    = 0;                   \
      ( FDL_PTR )->bFunctionCode        = 0;                   \
      ( FDL_PTR )->bMaxIndicationLength = (LEN);               \
   }//#define _INIT_FDL_BUF_HEAD(FDL_PTR, LEN)

#endif//#if VPC3_SERIAL_MODE

#if VPC3_SERIAL_MODE

   #define COPY_SAP_PARAMETER__( _dest, _src)                  \
   {                                                           \
      VPC3_ADR dest = (_dest);                                 \
      FDL_SAP_CNTRL MEM_PTR src   =  (_src);                   \
                                                               \
      Vpc3Write( dest + 0, src->bRespSendSapNr );              \
      Vpc3Write( dest + 1, src->bReqSa );                      \
      Vpc3Write( dest + 2, src->bAccessReqSsap );              \
      Vpc3Write( dest + 3, src->bEventServSup );               \
      Vpc3Write( dest + 4, src->abVpc3SegIndPtr[0] );          \
      Vpc3Write( dest + 5, src->abVpc3SegIndPtr[1] );          \
      Vpc3Write( dest + 6, src->bVpc3SegRespPtr );             \
   }//#define COPY_SAP_PARAMETER__(_dest, _src)

#else

   #define COPY_SAP_PARAMETER__( _dest, _src)                  \
   {                                                           \
      FDL_SAP_CNTRL VPC3_PTR dest = (_dest);                   \
      FDL_SAP_CNTRL MEM_PTR src   =  (_src);                   \
                                                               \
      dest->bRespSendSapNr     =  src->bRespSendSapNr;         \
      dest->bReqSa             =  src->bReqSa;                 \
      dest->bAccessReqSsap     =  src->bAccessReqSsap;         \
      dest->bEventServSup      =  src->bEventServSup;          \
      dest->abVpc3SegIndPtr[0] =  src->abVpc3SegIndPtr[0];     \
      dest->abVpc3SegIndPtr[1] =  src->abVpc3SegIndPtr[1];     \
      dest->bVpc3SegRespPtr    =  src->bVpc3SegRespPtr;        \
   }//#define COPY_SAP_PARAMETER__(_dest, _src)

#endif//#if VPC3_SERIAL_MODE

#define INCR_VPC3_UP_PTR__(_buf_size)                          \
{                                                              \
   bVpc3SegmentAddress += (UBYTE)((_buf_size)>>SEG_MULDIV);    \
   pToVpc3 = &pToVpc3[(_buf_size)];                            \
}//#define INCR_VPC3_UP_PTR__(_buf_size)

#define SWAP_WORD(w) ((((w)&0x00ff)*0x100) + (((w)>>8)&0x00ff))


/*-----------------------------------------------------------------------------------------------------------*/
/* 11.0 DPV1                                                                                                 */
/*-----------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/* 11.1 return codes for DPV1 services                                       */
/*---------------------------------------------------------------------------*/
typedef enum
{
   DPV1_OK     = ((UBYTE)0x00),      //must be the same as for return no_error
   DPV1_NOK    = ((UBYTE)0x81),      //negative response
   DPV1_DELAY  = ((UBYTE)0x82),      //user give pdu later back
   DPV1_ABORT  = ((UBYTE)0x83),      //user want abort the connection
   DPV1_INIT   = ((UBYTE)0xFF)
} DPV1_RET_VAL;

/*----------------------------------------------------------------------------*/
/* 11.2 errorcodes of DPV1 for ERROR_DECODE                                   */
/*----------------------------------------------------------------------------*/
#define DPV1_ERRDC_DPV1                   ((UBYTE)0x80) // Error_decode, DPV1 fixed value
#define DPV1_ERRDC_FMS                    ((UBYTE)0xFE) // Error_decode, FMS  fixed value
#define DPV1_ERRDC_HART                   ((UBYTE)0xFF) // Error_decode, HART fixed value

/*----------------------------------------------------------------------------*/
/* 11.3 errorcodes of DPV1 for ERROR_CODE_1                                   */
/*----------------------------------------------------------------------------*/
// error_class
#define DPV1_ERRCL_APPLICATION            ((UBYTE)0xA0)
#define DPV1_ERRCL_ACCESS                 ((UBYTE)0xB0)
#define DPV1_ERRCL_RESSOURCE              ((UBYTE)0xC0)
#define DPV1_ERRCL_USER                   ((UBYTE)0xD0)
// error_code for  DPV1_ERRCL_APPLICATION
#define DPV1_ERRCL_APP_READ               ((UBYTE)0x00)
#define DPV1_ERRCL_APP_WRITE              ((UBYTE)0x01)
#define DPV1_ERRCL_APP_MODULE             ((UBYTE)0x02)
#define DPV1_ERRCL_APP_VERSION            ((UBYTE)0x08)
#define DPV1_ERRCL_APP_NOTSUPP            ((UBYTE)0x09)
#define DPV1_ERRCL_APP_USER               ((UBYTE)0x0A)
// error_code for  DPV1_ERRCL_ACCESS
#define DPV1_ERRCL_ACC_INV_INDEX          ((UBYTE)0x00)
#define DPV1_ERRCL_ACC_WRITE_LEN          ((UBYTE)0x01)
#define DPV1_ERRCL_ACC_INV_SLOT           ((UBYTE)0x02)
#define DPV1_ERRCL_ACC_TYPE               ((UBYTE)0x03)
#define DPV1_ERRCL_ACC_INV_AREA           ((UBYTE)0x04)
#define DPV1_ERRCL_ACC_STATE              ((UBYTE)0x05)
#define DPV1_ERRCL_ACC_ACCESS             ((UBYTE)0x06)
#define DPV1_ERRCL_ACC_INV_RANGE          ((UBYTE)0x07)
#define DPV1_ERRCL_ACC_INV_PARAM          ((UBYTE)0x08)
#define DPV1_ERRCL_ACC_INV_TYPE           ((UBYTE)0x09)
#define DPV1_ERRCL_ACC_USER               ((UBYTE)0x0A)
#define DPV1_ERRCL_ACC_IM_BACKUP          ((UBYTE)0x0A)
// error_code for  DPV1_ERRCL_RESOURCE
#define DPV1_ERRCL_RES_READ_CONSTRAIN     ((UBYTE)0x00)
#define DPV1_ERRCL_RES_WRITE_CONSTRAIN    ((UBYTE)0x01)
#define DPV1_ERRCL_RES_BUSY               ((UBYTE)0x02)
#define DPV1_ERRCL_RES_UNAVAIL            ((UBYTE)0x03)
#define DPV1_ERRCL_RES_USER               ((UBYTE)0x08)
// error_code for  msac_c1_transmit_delay
#define DPV1_NO_VALID_SAP                 ((UBYTE)0x01)
#define DPV1_ERR_INV_PARAM                ((UBYTE)0x02)
#define DPV1_ERR_NO_JOB                   ((UBYTE)0x03)

/*----------------------------------------------------------------------------*/
/* 11.4 coding of abort                                                       */
/*----------------------------------------------------------------------------*/
/* subnet */
#define MSAC_C2_SUBNET_NO                 ((UBYTE)0x00)
#define MSAC_C2_SUBNET_LOCAL              ((UBYTE)0x01)
#define MSAC_C2_SUBNET_REMOTE             ((UBYTE)0x02)
/* instance */
#define MSAC_C2_INSTANCE_FDL              ((UBYTE)0x00)
#define MSAC_C2_INSTANCE_MSAC_C2          ((UBYTE)0x10)
#define MSAC_C2_INSTANCE_USER             ((UBYTE)0x20)
#define MSAC_C2_INSTANCE_RESERVED         ((UBYTE)0x30)
/* reason */
#define MSAC_C2_ABT_SE                    ((UBYTE)0x01)   // sequence error
#define MSAC_C2_ABT_FE                    ((UBYTE)0x02)   // invalid request PDU received
#define MSAC_C2_ABT_TO                    ((UBYTE)0x03)   // timeout of the connection
#define MSAC_C2_ABT_RE                    ((UBYTE)0x04)   // invalid response PDU received
#define MSAC_C2_ABT_IV                    ((UBYTE)0x05)   // invalid service from USER
#define MSAC_C2_ABT_STO                   ((UBYTE)0x06)   // Send_Timeout requested was too small
#define MSAC_C2_ABT_IA                    ((UBYTE)0x07)   // invalid additional address information
#define MSAC_C2_ABT_OC                    ((UBYTE)0x08)   // waiting for FDL_DATA_REPLY.con
#define MSAC_C2_ABT_RES                   ((UBYTE)0x0F)   // resource error

/*----------------------------------------------------------------------------*/
/* 11.5 Function Codes for DPV1                                               */
/*----------------------------------------------------------------------------*/
#define DPV1_FC_ERROR                     ((UBYTE)0x80)
#define DPV1_FC_INITIATE                  ((UBYTE)0x57)
#define DPV1_FC_ABORT                     ((UBYTE)0x58)
#define DPV1_FC_READ                      ((UBYTE)0x5E)
#define DPV1_FC_WRITE                     ((UBYTE)0x5F)
#define DPV1_FC_ALARM_ACK                 ((UBYTE)0x5C)
#define DPV1_FC_IDLE                      ((UBYTE)0x48)
#define DPV1_FC_TRANSPORT                 ((UBYTE)0x51)
#define DPV1_FC_RM                        ((UBYTE)0x56)
// mask for function code
#define MSAC_C2_FN_MASK                   ((UBYTE)0x1F)
#define MSAC_C2_FN_CONNECT                ((UBYTE)0x17)
#define MSAC_C2_FN_DATA                   ((UBYTE)0x11)
#define MSAC_C2_FN_DISCONNECT             ((UBYTE)0x18)
#define MSAC_C2_FN_IDLE                   ((UBYTE)0x08)
#define MSAC_C2_FN_DS_READ                ((UBYTE)0x1E)
#define MSAC_C2_FN_DS_WRITE               ((UBYTE)0x1F)

/*----------------------------------------------------------------------------*/
/* 11.6 general defines for DPV1                                              */
/*----------------------------------------------------------------------------*/
#define MSAC_C2_TIMEOUT_12MBAUD           ((UWORD)0x0064)
#define MSAC_C2_TIMEOUT_6MBAUD            ((UWORD)0x0064)
#define MSAC_C2_TIMEOUT_3MBAUD            ((UWORD)0x0064)
#define MSAC_C2_TIMEOUT_1_5MBAUD          ((UWORD)0x0064)
#define MSAC_C2_TIMEOUT_500KBAUD          ((UWORD)0x00C8)
#define MSAC_C2_TIMEOUT_187_50KBAUD       ((UWORD)0x0190)

#define MSAC_C2_MIN_POLL_TIMEOUT          ((UWORD)0x0001)
#define MSAC_C2_MAX_POLL_TIMEOUT          ((UWORD)0x7FFF)

#define MSAC_C2_MIN_PDU_LEN               ((UBYTE)0x04)
#define MSAC_C2_LEN_DISCONNECT_PDU        ((UBYTE)0x04)
#define MSAC_C2_LEN_IDLE_PDU              ((UBYTE)0x02)
#define MSAC_C2_LEN_RESSOURCE_PDU         ((UBYTE)0x04)
#define MSAC_C2_MIN_CC_PDU_SIZE           ((UBYTE)0x10)
#define MSAC_C2_MAX_PDU_SIZE              ((UBYTE)0xF0)
#define MSAC_C2_MIN_CR_PDU_SIZE           ((UBYTE)0x14)

#define MSAC_C2_MAX_PDU                   ((UBYTE)0xF4)   // PDU with DPV1-Header (4Byte) + 240 User Data
#define MSAC_C2_LEN_INITIATE_RES_HEAD     ((UBYTE)0x0C)   // Initiate response, no addr_data[]
#define DPV1_LEN_NEG_RESPONSE             ((UBYTE)0x04)   // Negative Response PDU
#define DPV1_LEN_HEAD_DATA                ((UBYTE)0x04)   // Fct,Slot,Index,Len

#define MSAC_C2_FUNCTION_NUM_EXTENSION    ((UBYTE)0x40)
#define MSAC_C2_ERROR_FRAME               ((UBYTE)0xC0)
#define DPV1_ERROR_BIT_RESPONSE           ((UBYTE)0x80)

#define MSAC_C2_IND_RECV                  ((UBYTE)0x01)   // indication receive
#define MSAC_C2_RESP_SENT                 ((UBYTE)0x02)   // response sent
#define MSAC_C2_IND_DISABLE_DONE          ((UBYTE)0x10)   // ind_disable_done

#define MSAC_C2_FC_SRD_L                  ((UBYTE)0x0C)
#define VPC3_MSAC_C1_CLOSE_FAILED         ((UBYTE)0x90)

typedef enum
{
   MSAC_C2_RESP_DIRECTION = ((UBYTE)0x01),
   MSAC_C2_REQ_DIRECTION  = ((UBYTE)0x02)
} MSAC2_DIRECTION;

/*----------------------------------------------------------------------------*/
/* 11.7 structures for DPV1                                                   */
/*----------------------------------------------------------------------------*/
typedef struct
{
   UBYTE function_num;
   UBYTE slot_num;
   UBYTE index;
   UBYTE length;
   UBYTE pdu_data[1];
}  DPV1_PDU;

typedef struct
{
   UBYTE function_num;
   UBYTE slot_num;
   UBYTE index;
   UBYTE length;
   UBYTE pdu_data[1];
}  DPV1_READ_PDU;

typedef struct
{
   UBYTE function_num;
   UBYTE slot_num;
   UBYTE index;
   UBYTE length;
   UBYTE pdu_data[1];
}  DPV1_WRITE_PDU;

typedef struct
{
   UBYTE function_num;
   UBYTE err_decode;
   UBYTE err_code1;
   UBYTE err_code2;
}  DPV1_NEG_RES_PDU;

#if DP_MSAC_C2

   typedef struct _MSAC_C2_TRANSPORT_PDU
   {
      UBYTE  function_num;
      UBYTE  slot_num;
      UBYTE  index;
      UBYTE  length;
      UBYTE  pdu_data[1];
   }  MSAC_C2_TRANSPORT_PDU;

   typedef struct
   {
      UBYTE  bDpv1Service;
      UBYTE  bReserved1;
      UBYTE  bReserved2;
      UBYTE  bReserved3;
      UWORD  wSendTimeOut;
      UBYTE  bFeaturesSupported1;
      UBYTE  bFeaturesSupported2;
      UBYTE  bProfileFeaturesSupported1;
      UBYTE  bProfileFeaturesSupported2;
      UWORD  wProfileIdentNumber;
      UBYTE  bS_Type;
      UBYTE  bS_Len;
      UBYTE  bD_Type;
      UBYTE  bD_Len;
      UBYTE  abAddrData[ C2_LEN-16 ];  //max length from dp_cfg.h
   }  MSAC_C2_INITIATE_REQ_PDU;
   #define MSAC_C2_INITIATE_REQ_PDU_PTR   MSAC_C2_INITIATE_REQ_PDU  VPC3_PTR
   #define INITIATE_REQ_PDU_PTR   MSAC_C2_INITIATE_REQ_PDU  MEM_PTR

   typedef struct _MSAC_C2_INITIATE_SUB_PARAM
   {
      UBYTE  bApi;
      UBYTE  bScl;
      UBYTE  abNetAddr[6];
      UBYTE  abMacAddr[1]; //len is defined by S_len or D_len !!
   }  MSAC_C2_INITIATE_SUB_PARAM;
   #define D_ADDR_PTR   MSAC_C2_INITIATE_SUB_PARAM  MEM_PTR

   typedef struct _MSAC_C2_INITIATE_RES_PDU
   {
      UBYTE  bDpv1Service;
      UBYTE  bMaxLenDataUnit;
      UBYTE  bFeaturesSupported1;
      UBYTE  bFeaturesSupported2;
      UBYTE  bProfileFeaturesSupported1;
      UBYTE  bProfileFeaturesSupported2;
      UWORD  bProfileIdentNumber;
      UBYTE  bS_Type;
      UBYTE  bS_Len;
      UBYTE  bD_Type;
      UBYTE  bD_Len;
      UBYTE  abAddrData[ C2_LEN-12 ];  //max length from dp_cfg.h
   }  MSAC_C2_INITIATE_RES_PDU;
   #define INITIATE_RES_PDU_PTR MSAC_C2_INITIATE_RES_PDU   VPC3_PTR

   typedef struct _MSAC_C2_ABORT_PDU
   {
      UBYTE  function_num;
      UBYTE  subnet;
      UBYTE  instance_reason;
      UBYTE  reserved; /* MSAC_C2 + 1 Byte, no additional Detail */
   }  MSAC_C2_ABORT_PDU;

#endif//#if DP_MSAC_C2

typedef struct
{
   UBYTE bDpv1Service;
   UBYTE bSlotNr;
   UBYTE bAlarmType;
   UBYTE bSpecifier;
}  MSAC_C1_ALARM_ACK_PDU;
#define ALARM_ACK_REQ_PTR MSAC_C1_ALARM_ACK_PDU  MEM_PTR

typedef union
{
   UBYTE                     ubyte_ptr;
   DPV1_READ_PDU             read;
   DPV1_WRITE_PDU            write;
   MSAC_C1_ALARM_ACK_PDU     alarm;

   #if DP_MSAC_C2
      MSAC_C2_TRANSPORT_PDU     transport;
      MSAC_C2_INITIATE_REQ_PDU  initiate_req;
      MSAC_C2_ABORT_PDU         abort;
      MSAC_C2_INITIATE_RES_PDU  initiate_res;
   #endif//#if DP_MSAC_C2

   DPV1_NEG_RES_PDU          neg;
   DPV1_PDU                  response;
}DPV1_UNION_PDU;

#define DPV1_PTR   DPV1_UNION_PDU  VPC3_PTR

typedef struct
{
   UBYTE                      bC2Service;
   UBYTE                      reserved1;
   UWORD                      reserved2;
   UWORD                      wSendTimeOut;
   UWORD                      features_supported;
   UWORD                      profile_features_supported;
   UWORD                      profile_ident_number;
}  MSAC_C2_CR_BUF;
#define MSAC_C2_CR_BUF_PTR            MSAC_C2_CR_BUF  VPC3_PTR

typedef struct
{
   UBYTE                      bC2Service;
   UBYTE                      max_pdu_size;
   UWORD                      features_supported;
   UWORD                      profile_features_supported;
   UWORD                      profile_ident_number;
}  MSAC_C2_CC_BUF;
#define MSAC_C2_CC_BUF_PTR            MSAC_C2_CC_BUF  VPC3_PTR

typedef struct
{
   UBYTE                      bC2Service;
   UBYTE                      bLocation;
   UBYTE                      bReasonCode;
   UBYTE                      bReserved;
}  MSAC_C2_DR_BUF;
#define MSAC_C2_DR_BUF_PTR            MSAC_C2_DR_BUF  MEM_PTR

typedef struct
{
   UBYTE                      bC2Service;
   UBYTE                      slot;
   UBYTE                      index;
   UBYTE                      length;
}  MSAC_C2_DE_BUF;
#define MSAC_C2_DE_BUF_PTR            MSAC_C2_DE_BUF  VPC3_PTR

// input queue element
typedef struct
{
   MSG_HEADER_PTR       psMsgHeader;
   VPC3_UNSIGNED8_PTR   pToDpv1Data;
   UBYTE                bRetValue;
   UBYTE                bSapNr;
   UBYTE                bFdlCode;
}  MSAC_C2_REC_QUEUE;
#define MSAC_C2_REC_QUEUE_PTR    MSAC_C2_REC_QUEUE  MEM_PTR

// reserve queue for ind disable buffer
typedef struct
{
   UBYTE  data_len;                              // length of netto-data
   UBYTE  fc;                                    // function-code
   UBYTE  user_data[MSAC_C2_LEN_DISCONNECT_PDU]; // netto-data
}  MSAC_C2_RES_IND_QUEUE;

//defines for MSAC_C2 connection state
typedef enum
{
   MSAC_C2_CS_CLOSE_CHANNEL    = ((UBYTE)0x01),       // initialization state
   MSAC_C2_CS_AWAIT_CONNECT    = ((UBYTE)0x04),       // first state
   MSAC_C2_CS_AWAIT_IND        = ((UBYTE)0x05),       // state after successfull connect
   MSAC_C2_CS_PROVIDE_IND      = ((UBYTE)0x06),
   MSAC_C2_CS_PROVIDE_RESP     = ((UBYTE)0x07),
   MSAC_C2_CS_DISABLE_CONNECT  = ((UBYTE)0x08)
} MSAC2_CONN_STATE;

//defines for MSAC_C2 error codes
typedef enum
{
   MSAC_C2_EC_OK               = ((UBYTE)0x00),       //  without errors
   MSAC_C2_EC_USER_ERR         = ((UBYTE)0x01),       //  invalid response data
   MSAC_C2_EC_IUSE             = ((UBYTE)0x02),       //  inuse bit set //V504
   MSAC_C2_EC_INV_S_D_LEN_ERR  = ((UBYTE)0x03),       //  S,D-len error
   MSAC_C2_EC_REQ_TIMEOUT      = ((UBYTE)0x05),       //  req_time_out
   MSAC_C2_EC_DC_BY_USER       = ((UBYTE)0x0D),       //  user response is disconnect
   MSAC_C2_EC_DC_BY_MASTER     = ((UBYTE)0x0E),       //  master request is disconnect
   MSAC_C2_EC_REMOTE_ERROR     = ((UBYTE)0x10)        //  invalid pdu from the master
} MSAC2_ERROR_CODE;

//defines for MSAC_C2 user_break
typedef enum
{
   MSAC_C2_UB_NO_BREAK         = ((UBYTE)0x00),
   MSAC_C2_UB_BREAK            = ((UBYTE)0x01)
} MSAC2_USER_BREAK;

// MSAC_C2 connections
typedef struct
{
   MSG_HEADER_PTR          psMsgHeader;
   MSG_HEADER_PTR          psMsgHeaderRes;
   MSG_HEADER              sMsgHeader;

   VPC3_UNSIGNED8_PTR      pToDpv1Data;

   UWORD                   wSendTimeOut;              // time_out for the connection

   MSAC2_CONN_STATE        eState;                    // connection state
   MSAC2_ERROR_CODE        eErrorCode;
   MSAC2_USER_BREAK        eUserBreak;                // user with disconnect request

   UBYTE                   bSapNr;                    // sap_nr FDL
   UBYTE                   bCnId;                     // connection id
   UBYTE                   bC2Service;                // fc of actual pdu
   UBYTE                   bDrReasonCode;             // save reason_code of MSAC_C2_disconnect()
   UBYTE                   bDrLocation;               // save location of MSAC_C2_disconnect()
   UBYTE                   bTimerIndex;               // index for used timer
   UBYTE                   bTimerBreak;               // timeout may be ignored
   UBYTE                   bTimerActive;              // timer has started
}  MSAC_C2_CONNECT_ITEM;
#define MSAC_C2_CONNECT_ITEM_PTR  MSAC_C2_CONNECT_ITEM  MEM_PTR

// -- defines for MSAC2 - Timer -----------------------------------------------------
#define MSAC_C2_TIMER_OK                  ((UBYTE)10)
#define MSAC_C2_TIMER_ALREADY_STOPPED     ((UBYTE)20)
#define MSAC_C2_TIMER_ALREADY_RUNS        ((UBYTE)30)

typedef struct
{
   UWORD  wActValue;
   UBYTE  bRunning;
}  MSAC_C2_TIMER;

#define MSAC_C1_IND_DS_READ               ((UBYTE)0x01)
#define MSAC_C1_IND_DS_WRITE              ((UBYTE)0x02)
#define MSAC_C1_IND_ALARM_QUIT            ((UBYTE)0x04)
#define MSAC_C1_IND_ALARM_QUIT_SAP50      ((UBYTE)0x08)

/*----------------------------------------------------------------------------*/
/* 11.8 defines for ALARM                                                     */
/*----------------------------------------------------------------------------*/
// max length of alarm-fifo
#define ALARM_MAX_FIFO                    ((UBYTE)0x20)
// defines for Status-Types
#define STATUS_MESSAGE                    ((UBYTE)0x81)
#define MODUL_STATUS                      ((UBYTE)0x82)
#define STATUS_PRM_COMMAND_ACK            ((UBYTE)0x9E)
#define STATUS_RED_STATE                  ((UBYTE)0x9F)

#define SET_ALARM_AL_STATE_CLOSED         ((UBYTE)0x01)
#define SET_ALARM_SEQ_NR_ERROR            ((UBYTE)0x02)
#define SET_ALARM_SPECIFIER_ERROR         ((UBYTE)0x03)
#define SET_ALARM_LIMIT_EXPIRED           ((UBYTE)0x04)
#define SET_ALARM_PENDING                 ((UBYTE)0x05)
#define SET_ALARM_ALARMTYPE_NOTSUPP       ((UBYTE)0x06)
#define SET_ALARM_OK                      ((UBYTE)0x00)

#define SET_STATUS_NOT_ENABLED            ((UBYTE)0x01)
#define SET_STATUS_OK                     ((UBYTE)0x00)

// defines for alarm-type
#define ALARM_TYPE_MAX                    ((UBYTE)0x07)

#define ALARM_TYPE_DIAGNOSTIC             ((UBYTE)0x01)
#define ALARM_TYPE_PROCESS                ((UBYTE)0x02)
#define ALARM_TYPE_PULL                   ((UBYTE)0x03)
#define ALARM_TYPE_PLUG                   ((UBYTE)0x04)
#define ALARM_TYPE_STATUS                 ((UBYTE)0x05)
#define ALARM_TYPE_UPDATE                 ((UBYTE)0x06)
#define ALARM_TYPE_MANU_MIN               ((UBYTE)0x20)
#define ALARM_TYPE_MANU_MAX               ((UBYTE)0x7E)

// defines for sequence_mode
#define SEQC_MODE_TOTAL_00                ((UBYTE)0x00)
#define SEQC_MODE_OFF                     ((UBYTE)0x01)
#define SEQC_MODE_TOTAL_02                ((UBYTE)0x02)
#define SEQC_MODE_TOTAL_04                ((UBYTE)0x04)
#define SEQC_MODE_TOTAL_08                ((UBYTE)0x08)
#define SEQC_MODE_TOTAL_12                ((UBYTE)0x0C)
#define SEQC_MODE_TOTAL_16                ((UBYTE)0x10)
#define SEQC_MODE_TOTAL_24                ((UBYTE)0x18)
#define SEQC_MODE_TOTAL_32                ((UBYTE)0x20)

// defines for specifier
#define SPEC_GENERAL                      ((UBYTE)0x00)
#define SPEC_APPEARS                      ((UBYTE)0x01)
#define SPEC_DISAPPEARS                   ((UBYTE)0x02)

#define SPEC_MASK                         ((UBYTE)0x03)
#define SPEC_SEQ_START                    ((UBYTE)0x03)   //use it to shift the sequence number down to bit 0 or up to bit 3
#define MAX_SEQ_NR                        ((UBYTE)0x20)
#define SEQUENCE_NUMBER_ALL               ((UBYTE)0xFF)
#define SEQ_NR_MASK                       ((UBYTE)0xF8)

#define SPEC_ADD_ACK                      ((UBYTE)0x04)

#define cSizeOfAlarmHeader                ((UBYTE)0x04)

/*----------------------------------------------------------------------------*/
/* 11.9 structure of ALARM                                                    */
/*----------------------------------------------------------------------------*/
typedef struct
{
   DPL_STRUC_LIST_CB    dplListHead;

   UBYTE                bHeader;
   UBYTE                bAlarmType;
   UBYTE                bSlotNr;
   UBYTE                bSpecifier;
   UBYTE                bUserDiagLength;
   UBYTE                bCallback;
   MEM_UNSIGNED8_PTR    pToUserDiagData;
}  ALARM_STATUS_PDU;
#define ALARM_STATUS_PDU_PTR   ALARM_STATUS_PDU  MEM_PTR

typedef union
{
   DPL_STRUC_LIST_CB_PTR sListPtr;
   ALARM_STATUS_PDU_PTR  psAlarm;
}ALARM_UNION_ALARM;


/*-----------------------------------------------------------------------------------------------------------*/
/* 12.0 global system structure                                                                              */
/*-----------------------------------------------------------------------------------------------------------*/
#define MSAC_C1_MAX_PDU_SIZE              ((UBYTE)0xF0)
#define MSAC_C1_MAX_PDU                   ((UBYTE)0xF4)   // PDU with DPV1-Header (4Byte) + 240 User Data
#define MSAC_C1_MIN_PDU_SIZE              ((UBYTE)0x44)   // I&M function are mandatory

//defines for MSAC1 Operation Mode
typedef enum
{
    DP_OPMODE_V0        = ((UBYTE)0x00),
    DP_OPMODE_V1        = ((UBYTE)0x01)
} MSAC1_OPMODE;

//defines for MSAC1 Start State
typedef enum
{
    DP_SS_IDLE          = ((UBYTE)0x00),
    DP_SS_STOP          = ((UBYTE)0x01),
    DP_SS_START_AGAIN   = ((UBYTE)0x02),
    DP_SS_RUN           = ((UBYTE)0x03)
} MSAC1_START_STATE;

// -- structures --------------------------------------------------------------
typedef struct
{
   MSAC1_OPMODE         eOperationMode;
   MSAC1_START_STATE    eStartState;
   UBYTE                bDxEntered;
   UBYTE                bFdlClosing;
   UBYTE                bActiveJob;
   UBYTE                bC1Service;
   UBYTE                bMSAC1Sactivate;

   MSG_HEADER_PTR       psMsgHeader;
   VPC3_UNSIGNED8_PTR   pToDpv1Data;

   MSG_HEADER_PTR       psMsgHeaderSAP50;
   VPC3_UNSIGNED8_PTR   pToDpv1DataSAP50;
}  C1_STRUC;

#if DP_MSAC_C2
typedef struct
{
   UWORD                      wSendTimeOut;
   UBYTE                      bReadRecPtr;
   UBYTE                      bWriteRecPtr;
   UBYTE                      bStartRecPtr;                       // first element of receive queue
   UBYTE                      bEndRecPtr;                         // last  element of receive queue
   UBYTE                      bNrOfSap;

   UBYTE                      bEnabled;
   UBYTE                      bCloseChannelRequest;               // memory item for close_channel request of the user,
                                                                  // used to reject a double request
   #ifdef REDUNDANCY
      UBYTE                   bRedTimerTick;
   #endif//#ifdef REDUNDANCY

   MSAC_C2_INITIATE_REQ_PDU   sInitiateReq;
   MSAC_C2_REC_QUEUE_PTR      psActRecQueue;
   MSAC_C2_REC_QUEUE          asRecQueue[MSAC_C2_MAX_INPUT_ITEMS];
   MSAC_C2_CONNECT_ITEM       asConnectionList[DP_C2_NUM_SAPS];    // list of connection-descriptions
   MSAC_C2_TIMER              asTimerList[ DP_C2_NUM_SAPS ];
}  C2_STRUC;
#endif//#if DP_MSAC_C2

#define AL_STATE_CLOSED                   ((UBYTE)0x10)
#define AL_STATE_OPEN                     ((UBYTE)0x11)

#define AL_TYPE_MAX                       ((UBYTE)0x07)   // dpv1 draft specification - do not use cast !
#define AL_SEQUENCE_MAX                   ((UBYTE)0x20)   // dpv1 draft specification - do not use cast !

//defines for sequence_status
#define AL_SEQUENCE_STATUS_SIZE           ((UBYTE)(((ALARM_TYPE_MAX * MAX_SEQ_NR)+7)/8))

#define AL_ALARM_STATUS_ACTION_SET        ((UBYTE)0x11)
#define AL_ALARM_STATUS_ACTION_CHECK      ((UBYTE)0x49)
#define AL_ALARM_STATUS_ACTION_RESET      ((UBYTE)0x57)

typedef struct
{
   DPL_STRUC_LIST_CB dplAlarmQueue;
   DPL_STRUC_LIST_CB dplAlarmAckQueue;

   UBYTE       bState;
   UBYTE       bEnabled;      //DPV1_STATUS 2, indicates the type of alarms
   UBYTE       bMode;         //DPV1_STATUS 3

   UBYTE       bTypeStatus;
   UBYTE       abSequenceStatus[ AL_SEQUENCE_STATUS_SIZE ];

   UBYTE       bSequence;     //FALSE: only one alarm of a specific ALARM_TYPE can be active at one time
                              //TRUE : several alarms (2 to 32) of the same or different ALARM_TYPE can be
                              //       active at one time
   UBYTE       bLimit;        //contains the maximum number of alarms
                              //allowed by the actual master-slave connection
   UBYTE       bCount;        //contains the number of alarms, which have been sent
}  AL_STRUC;

//defines for TIMESTAMP

//defines for TYPE OF SIGNAL EVENTS
#define TOSE_DELTA_TRIGGER_DISCRETE       ((UBYTE)0x01)
#define TOSE_TIME_TRIGGER_DISCRETE        ((UBYTE)0x02)
#define TOSE_DELTA_TRIGGER_ANALOGUE       ((UBYTE)0x04)
#define TOSE_DELTA_TRIGGER_ANALOGUE_LIMIT ((UBYTE)0x05)
#define TOSE_TIME_TRIGGER_ANALOGUE        ((UBYTE)0x06)

//TimeStamp Event structure
typedef struct
{
   UBYTE bTypeOfSignalEvent;
   UBYTE bSlotNr;
   UBYTE bIOChannel;
   UBYTE bByte1;
   UBYTE bByte2;
   UBYTE bByte3;
   UBYTE abTimeStamp[8];
}  TS_TIMESTAMP_STRUC;
#define TS_TIMESTAMP_STRUC_PTR   TS_TIMESTAMP_STRUC MEM_PTR
#define cSizeOfTsStruc                 ((UBYTE)0x0E)

//Delta_Trigger_Discrete structure
typedef struct
{
   UBYTE bTypeOfSignalEvent;
   UBYTE bSlotNr;
   UBYTE bIOChannel;
   UBYTE bValue;
   UBYTE bReserved1;
   UBYTE bReserved2;
   UBYTE abTimeStamp[8];
}  TS_DELTA_TRIGGER_STRUC;
#define TS_DELTA_TRIGGER_STRUC_PTR   TS_DELTA_TRIGGER_STRUC MEM_PTR

//Time_Trigger_Discrete structure
typedef struct
{
   UBYTE bTypeOfSignalEvent;
   UBYTE bSlotNr;
   UBYTE bIOChannel;
   UBYTE bValue;
   UBYTE bReserved1;
   UBYTE bReserved2;
   UBYTE abTimeStamp[8];
}  TS_TIME_TRIGGER_STRUC;
#define TS_TIME_TRIGGER_STRUC_PTR   TS_TIME_TRIGGER_STRUC MEM_PTR

//Delta_Trigger_Analogue structure
typedef struct
{
   UBYTE bTypeOfSignalEvent;
   UBYTE bSlotNr;
   UBYTE bIOChannel;
   UBYTE bReason;
   UBYTE bValueHighByte;
   UBYTE bValueLowByte;
   UBYTE abTimeStamp[8];
}  TS_DELTA_TRIGGER_ANALOGUE_STRUC;
#define TS_DELTA_TRIGGER_ANALOGUE_STRUC_PTR   TS_DELTA_TRIGGER_ANALOGUE_STRUC MEM_PTR

//Delta_Trigger_Analogue_Limit structure
typedef struct
{
   UBYTE bTypeOfSignalEvent;
   UBYTE bSlotNr;
   UBYTE bIOChannel;
   UBYTE bReason;
   UBYTE bValueHighByte;
   UBYTE bValueLowByte;
   UBYTE abTimeStamp[8];
}  TS_DELTA_TRIGGER_ANALOGUE_LIMIT_STRUC;
#define TS_DELTA_TRIGGER_ANALOGUE_LIMIT_STRUC_PTR   TS_DELTA_TRIGGER_ANALOGUE_LIMIT_STRUC MEM_PTR

//Time_Trigger_Analogue structure
typedef struct
{
   UBYTE bTypeOfSignalEvent;
   UBYTE bSlotNr;
   UBYTE bIOChannel;
   UBYTE bReason;
   UBYTE bValueHighByte;
   UBYTE bValueLowByte;
   UBYTE abTimeStamp[8];
}  TS_TIME_TRIGGER_ANALOGUE_STRUC;
#define TS_TIME_TRIGGER_ANALOGUE_STRUC_PTR   TS_TIME_TRIGGER_ANALOGUE_STRUC MEM_PTR

//defines for TYPE OF EXCEPTION EVENTS
#define TOEE_START_THE_PROCESS            ((UBYTE)0x80)
#define TOEE_CLOCK_DIFFERENCE             ((UBYTE)0x83)
#define TOEE_STOP_THE_PROCESS             ((UBYTE)0x84)
#define TOEE_BUFFER_OVERFLOW              ((UBYTE)0x85)
#define TOEE_CHANGE_ACTIVE_STATION        ((UBYTE)0x86)
#define TOEE_REDUNDANCY_TRANSFER_FAILURE  ((UBYTE)0x87)

//defines for SubType
#define TOEE_ST_EXCEPTION_IS_COMING       ((UBYTE)0x01)
#define TOEE_ST_EXCEPTION_IS_GOING        ((UBYTE)0x00)

//Exception Event structure
typedef struct
{
   UBYTE bTypeOfExceptionEvent;
   UBYTE bSlotNr;
   UBYTE bNotUsed1;
   UBYTE bSubType;
   UBYTE bNotUsed2;
   UBYTE bNotUsed3;
   UBYTE abTimeStamp[8];
}  TS_EXEPTION_STRUC;
#define TS_EXEPTION_STRUC_PTR   TS_EXEPTION_STRUC MEM_PTR

#define cSizeOfTsEvent        ((UBYTE)0x0E)

typedef union
{
   TS_TIMESTAMP_STRUC                     sTsTimeStampEvent;
   TS_DELTA_TRIGGER_STRUC                 sTsDeltaTriggerEvent;
   TS_TIME_TRIGGER_STRUC                  sTsTimeTriggerEvent;
   TS_DELTA_TRIGGER_ANALOGUE_STRUC        sTsDeltaTriggerAnalogueEvent;
   TS_DELTA_TRIGGER_ANALOGUE_LIMIT_STRUC  sTsDeltaTriggerAnalogueLimitEvent;
   TS_TIME_TRIGGER_ANALOGUE_STRUC         sTsTimeTriggerAnalogueEvent;
   TS_EXEPTION_STRUC                      sTsExeptionEvent;
} TS_UNION_STRUC;

#define TS_UNION_STRUC_PTR   TS_UNION_STRUC MEM_PTR


//defines for PRMCMD Function
#define FUNC_BACKUP_REQUEST               ((UBYTE)0x01)
#define FUNC_PRIMARY_REQUEST              ((UBYTE)0x02)
#define FUNC_MASK_START_STOP_MSAC1S       ((UBYTE)0x0C)
#define FUNC_NO_ACTION                    ((UBYTE)0x00)
#define FUNC_STOP_MSAC1S                  ((UBYTE)0x04)
#define FUNC_START_MSAC1S                 ((UBYTE)0x08)
#define FUNC_RESET_MSAC1S                 ((UBYTE)0x0C)
#define FUNC_CHECK_PROPERTIES             ((UBYTE)0x10)
#define FUNC_CHECK_RESERVED_1             ((UBYTE)0x20)
#define FUNC_CHECK_MASTER_STATE_CLEAR     ((UBYTE)0x40)
#define FUNC_CHECK_RESERVED_2             ((UBYTE)0x80)

//defines for PRMCMD Properties
#define PROP_PRIM_REQ_MS0_MS1_USED        ((UBYTE)0x01)
#define PROP_PRIM_START_STOPMSAC1S_USED   ((UBYTE)0x02)
#define PROP_ADDRESS_CHANGED              ((UBYTE)0x04)
#define PROP_ADDRESS_OFFSET_64            ((UBYTE)0x08)
#define PROP_CHECK_RESERVED_1             ((UBYTE)0x10)
#define PROP_CHECK_RESERVED_2             ((UBYTE)0x20)
#define PROP_CHECK_RESERVED_3             ((UBYTE)0x40)
#define PROP_CHECK_RESERVED_4             ((UBYTE)0x80)

#define PRM_CMD_LENGTH                    ((UBYTE)0x12)

#define PC_PRMCMD_SUPPORTED               ((UBYTE)0x01)
#define PC_SEND_PRMCMD_ACK                ((UBYTE)0x02)

typedef struct
{
   UBYTE bSlotNr;
   UBYTE bSpecifier;
   UBYTE bFunction;
   UBYTE bProperties;
   UWORD wOutputHoldTime;
}  RED_PRM_CMD;
#define RED_PRM_CMD_PTR   RED_PRM_CMD MEM_PTR

//defines for SWITCH OVER
#define SWO_DATAEX_TO_WAITPRM             ((UBYTE)0x01)
#define SWO_WAITPRM_TO_DATAEX             ((UBYTE)0x02)
#define SWO_PLC_RUN_TO_STOP               ((UBYTE)0x03)
#define SWO_PLC_STOP_TO_RUN               ((UBYTE)0x04)
#define SWO_BAUDRATE_DETECT               ((UBYTE)0x05)

//defines for RedState_1/RedState_2
#define RS_BACKUP                         ((UBYTE)0x01)
#define RS_PRIMARY                        ((UBYTE)0x02)
#define RS_HW_DEFEKT                      ((UBYTE)0x04)
#define RS_DATA_EXCHANGE                  ((UBYTE)0x08)
#define RS_MASTER_STATE_CLEAR             ((UBYTE)0x10)
#define RS_BAUDRATE_FOUND                 ((UBYTE)0x20)
#define RS_TOH_STARTED                    ((UBYTE)0x40)
#define RS_RESERVED                       ((UBYTE)0x80)

typedef struct
{
   UBYTE bHeaderByte;   //fix to 0x08
   UBYTE bStatusType;   //RedStateDiagnosis: 0x9F; PrmCommandAck: 0x9E
   UBYTE bSlotNr;       //fix to 0x00
   UBYTE bSpecifier;    //
   UBYTE bFunction;     //
   UBYTE bRedState_1;   //State from the initiator
   UBYTE bRedState_2;   //State from the other slave device
   UBYTE bRedState_3;   //Application specific
}  RED_STATE_STRUC;
#define RED_STATE_STRUC_PTR   RED_STATE_STRUC MEM_PTR
#define cSizeOfRedStateDiag               ((UBYTE)0x08)

#define RED_CHANNEL_1                     ((UBYTE)0x01)
#define RED_CHANNEL_2                     ((UBYTE)0x02)
#define RED_CHANNEL                       ((UBYTE)0x03)

#define RISM_ST_POWER_ON                  ((UBYTE)0x00)
#define RISM_ST_S_WAITING                 ((UBYTE)0x01)
#define RISM_ST_S_PRIMARY                 ((UBYTE)0x02)
#define RISM_ST_C_CONFIGURE               ((UBYTE)0x03)
#define RISM_ST_BACKUP                    ((UBYTE)0x04)
#define RISM_ST_BP_PARTNER_ACK            ((UBYTE)0x05)
#define RISM_ST_BP_SWITCHOVER             ((UBYTE)0x06)
#define RISM_ST_BP_PRM_CMD                ((UBYTE)0x07)
#define RISM_ST_BP_DX                     ((UBYTE)0x08)
#define RISM_ST_PRIMARY                   ((UBYTE)0x09)
#define RISM_ST_PB_PARTNER_ACK            ((UBYTE)0x0A)
#define RISM_ST_PB_SWITCHOVER             ((UBYTE)0x0B)
#define RISM_ST_NIL                       ((UBYTE)0xFF)

#define RISM_EV_RED_C_CHECK_IND           ((UWORD)0x0001)
#define RISM_EV_RED_C_SwitchoverPosCnf    ((UWORD)0x0002)
#define RISM_EV_RED_C_SwitchoverNegCnf    ((UWORD)0x0004)
#define RISM_EV_RED_C_SwitchoverInd       ((UWORD)0x0008)
#define RISM_EV_FSPMS_Prm_Cmd_Ind         ((UWORD)0x0010)
#define RISM_EV_FSPMS_New_Output_Ind      ((UWORD)0x0020)
#define RISM_EV_FSPMS_Stopped_Ind         ((UWORD)0x0080)
//#define RISM_EV_MS2_AR_ACTIVE           ((UWORD)0x0100)
#define RISM_EV_SwitchoverDone            ((UWORD)0x1000)

typedef struct
{
   UWORD wEvent;
   UBYTE bStatus;
}  sRedIndication;
#define RED_IND_STRUC_PTR   sRedIndication MEM_PTR

#define REDUNDANCY_PRIMARY_ADDR           ((UBYTE)0x01)
#define REDUNDANCY_BACKUP_ADDR            ((UBYTE)0x02)
#define REDUNDANCY_CHANGE_ADDR            ((UBYTE)0xFC)
#define REDUNDANCY_RESET_ADDR             ((UBYTE)0xFE)
#define REDUNDANCY_NIL_ADDR               ((UBYTE)0xFF)

// -- defines for user state
#define USER_STATE_CLEAR                  ((UBYTE)0x00)
#define USER_STATE_RUN                    ((UBYTE)0x01)

// -- dp system structure -----------------------------------------------------
typedef struct
{
   UWORD                   wEvent;
   #if DP_INTERRUPT_MASK_8BIT == 0
      UWORD                wInterruptEvent;
      UWORD                wPollInterruptEvent;
   #endif//#if DP_INTERRUPT_MASK_8BIT == 0
   UWORD                   wOldDiag;
   UBYTE                   bState;
   UBYTE                   bUserDiagActive;
   UBYTE                   abUserDiagnostic[ DIAG_BUFSIZE ];
   UBYTE                   bDPV1;
   UBYTE                   bApplicationReady;

   UBYTE                   bOutputDataLength;                  // calculated output length (data from DP-Master to VPC3)
   UBYTE                   bInputDataLength;                   // calculated input length  (data from VPC3 to DP-Master)
   UWORD                   wVpc3UsedDpBufferMemory;            // consumed user_memory

   UBYTE                   bIntIndHigh;                        // interrupt indication high byte
   UBYTE                   bIntIndLow;                         // interrupt indication low byte

   UBYTE                   bDinBufsize;                        // Length of the DIn Buffer (Data Slave to Master)                0..244
   UBYTE                   bDoutBufsize;                       // Length of the DOut Buffer     (Data Master to Slave)           0..244
   UBYTE                   bPrmBufsize;                        // Length of the Parameter Buffer                                 7..244
   UBYTE                   bDiagBufsize;                       // Length of the Diagnosis Buffer                                 6..244
   UBYTE                   bCfgBufsize;                        // Length of the Configuration Buffer                             1..244
   UBYTE                   bSsaBufsize;                        // Length of the Input Data in the Set_Slave_Address-Buffer 0 and 4..244

   UBYTE                   bDpv1Status1;
   UBYTE                   bDpv1Status2;
   UBYTE                   bDpv1Status3;

   UWORD                   wAsicUserRam;

   #if DP_FDL

      UWORD                awImIndex[0x10];

      #if DP_MSAC_C1
         C1_STRUC          sC1;
      #endif//#if DP_MSAC_C1

      #if DP_MSAC_C2
         C2_STRUC          sC2;
      #endif//#if DP_MSAC_C2

      FDL_STRUC            sFdl;
      AL_STRUC             sAl;

   #endif//#if DP_FDL

   #ifdef REDUNDANCY
      RED_PRM_CMD          sPrmCmd;

      UBYTE                bPrmCommandAck;

      UBYTE                bChannel;

      UBYTE                bRedState;
      UBYTE                bRedPartnerState;
      UBYTE                bRedPreferedPrim;
      UWORD                wRedToPrime;
      UBYTE                bRedClear;
      UBYTE                bRedTredcom;
      UBYTE                bRedPrmCmdActive;
      UBYTE                bRedNotReadyForP;
      UBYTE                bRedMS2_AR_Active;
      UBYTE                bRedTohStopped;
      UBYTE                bRedSwitchover;
      sRedIndication       sRedIndication;
      RED_IND_STRUC_PTR    ptrToRedIndOtherSlave;

      RED_STATE_STRUC      sRedState;

      RED_STATE_STRUC_PTR  ptrToRedStateOtherSlave;
   #endif//#ifdef REDUNDANCY

   VPC3_UNSIGNED8_PTR      pDoutBuffer1;                       // pointer to output buffer 1
   VPC3_UNSIGNED8_PTR      pDoutBuffer2;                       // pointer to output buffer 2
   VPC3_UNSIGNED8_PTR      pDoutBuffer3;                       // pointer to output buffer 3

   VPC3_UNSIGNED8_PTR      pDinBuffer1;                        // pointer to input buffer 1
   VPC3_UNSIGNED8_PTR      pDinBuffer2;                        // pointer to input buffer 2
   VPC3_UNSIGNED8_PTR      pDinBuffer3;                        // pointer to input buffer 3

   VPC3_UNSIGNED8_PTR      pDiagBuffer1;                       // pointer to diagnosis buffer 1
   VPC3_UNSIGNED8_PTR      pDiagBuffer2;                       // pointer to diagnosis buffer 2

   #ifdef DP_SUBSCRIBER
      VPC3_UNSIGNED8_PTR   pDxbOutBuffer1;                     // pointer to dxb output buffer 1
      VPC3_UNSIGNED8_PTR   pDxbOutBuffer2;                     // pointer to dxb output buffer 2
      VPC3_UNSIGNED8_PTR   pDxbOutBuffer3;                     // pointer to dxb output buffer 3
   #endif//#ifdef DP_SUBSCRIBER

   VPC3_UNSIGNED8_PTR      pDiagBuffer;                        // pointer to current diagnosis buffer
}  VPC3_SYSTEM_STRUC;
#define VPC3_SYSTEM_STRUC_PTR    VPC3_SYSTEM_STRUC   MEM_PTR

extern VPC3_SYSTEM_STRUC_PTR   pDpSystem;       // global system structure

/*---------------------------------------------------------------------------*/
/* 13.0 defines, structure for IM                                            */
/*---------------------------------------------------------------------------*/
// structure for I&M CALL
#define IM_FN_CALL                        ((UBYTE)0x08)

#if LITTLE_ENDIAN
   #define IM0_INDEX_0                    ((UWORD)0xE8FD)
   #define IM0_INDEX_1                    ((UWORD)0xE9FD)
   #define IM0_INDEX_2                    ((UWORD)0xEAFD)
   #define IM0_INDEX_3                    ((UWORD)0xEBFD)
   #define IM0_INDEX_4                    ((UWORD)0xECFD)
#else
   #define IM0_INDEX_0                    ((UWORD)0xFDE8)
   #define IM0_INDEX_1                    ((UWORD)0xFDE9)
   #define IM0_INDEX_2                    ((UWORD)0xFDEA)
   #define IM0_INDEX_3                    ((UWORD)0xFDEB)
   #define IM0_INDEX_4                    ((UWORD)0xFDEC)
#endif//#if LITTLE_ENDIAN

typedef struct
{
   UBYTE bExtendedFunctionNumber;
   UBYTE bReserved;
   UWORD wIndex;
   UBYTE abData[1];
}  sIMCALL;
#define psIMCALL   sIMCALL  MEM_PTR
#define psVPC3IMCALL sIMCALL  VPC3_PTR
#define cSizeOfImCall   ((UBYTE)0x04)

//defines for I&M supported
#define IM00_SUPPORTED                    ((UWORD)0x0001)
#define IM01_SUPPORTED                    ((UWORD)0x0002)
#define IM02_SUPPORTED                    ((UWORD)0x0004)
#define IM03_SUPPORTED                    ((UWORD)0x0008)
#define IM04_SUPPORTED                    ((UWORD)0x0010)
#define IM05_SUPPORTED                    ((UWORD)0x0020)
#define IM06_SUPPORTED                    ((UWORD)0x0040)
#define IM07_SUPPORTED                    ((UWORD)0x0080)
#define IM08_SUPPORTED                    ((UWORD)0x0100)
#define IM09_SUPPORTED                    ((UWORD)0x0200)
#define IM10_SUPPORTED                    ((UWORD)0x0400)
#define IM11_SUPPORTED                    ((UWORD)0x0800)
#define IM12_SUPPORTED                    ((UWORD)0x1000)
#define IM13_SUPPORTED                    ((UWORD)0x2000)
#define IM14_SUPPORTED                    ((UWORD)0x4000)
#define IM15_SUPPORTED                    ((UWORD)0x8000)

// structure for I&M0 (mandatory)
typedef struct
{
   UBYTE abHeader[10];
   UWORD wManufacturerID;
   UBYTE abOrderID[20];
   UBYTE abSerialNumber[16];
   UBYTE abHardwareRevision[2];
   UBYTE abSoftwareRevision[4];
   UWORD wRevCounter;
   UWORD wProfileID;
   UWORD wProfileSpecificType;
   UBYTE abIMVersion[2];
   UWORD wIMSupported;
}  sIM0;

#define cSizeOfIM0 ((UBYTE)0x40)

// structure for I&M1 (optional)
typedef struct
{
   UBYTE abHeader[10];
   UBYTE abTagFunction[32];
   UBYTE abTagLocation[22];
}  sIM1;

#define cSizeOfIM1 ((UBYTE)0x40)

// structure for I&M2 (optional)
typedef struct
{
   UBYTE abHeader[10];
   UBYTE abInstallationDate[16];
   UBYTE abReserved[38];
}  sIM2;

#define cSizeOfIM2 ((UBYTE)0x40)

// structure for I&M3 (optional)
typedef struct
{
   UBYTE abHeader[10];
   UBYTE abDescriptor[54];
}  sIM3;

#define cSizeOfIM3 ((UBYTE)0x40)

// structure for I&M4 (optional)
typedef struct
{
   UBYTE abHeader[10];
   UBYTE abSignature[54];
}  sIM4;

#define cSizeOfIM4 ((UBYTE)0x40)

/*---------------------------------------------------------------------------*/
/* 14.0 defines, structure for sDP_VERSION                                   */
/*---------------------------------------------------------------------------*/
#define DP_COMP_INSTALLED_MSAC1_IFA       ((UWORD)0x0001)
#define DP_COMP_INSTALLED_SUB_AL          ((UWORD)0x0002)
#define DP_COMP_INSTALLED_SUB_AL_50       ((UWORD)0x0004)

#define DP_COMP_INSTALLED_MSAC2_IFA       ((UWORD)0x0100)

#define DP_COMP_INSTALLED_IM              ((UWORD)0x0200)

#define DP_COMP_INSTALLED_VPC3_B          ((UWORD)0x1000)
#define DP_COMP_INSTALLED_VPC3_C          ((UWORD)0x2000)
#define DP_COMP_INSTALLED_VPC3_D          ((UWORD)0x4000)
#define DP_COMP_INSTALLED_VPC3_S          ((UWORD)0x8000)

typedef struct
{
   UWORD wComponentsInstalled;

   UBYTE bMainInterface;
   UBYTE bFunction;
   UBYTE bBugfix;
}  sDP_VERSION;
#define psDP_VERSION sDP_VERSION  MEM_PTR

/*-----------------------------------------------------------------------------------------------------------*/
/* 15.0 ERROR defines                                                                                        */
/*-----------------------------------------------------------------------------------------------------------*/
typedef enum
{
    DP_FATAL_ERROR                  = 0x00, // fatal error

    DP_OK                           = 0x01, // OK
    DP_NOK                          = 0x02, // OK

    DP_NOT_OFFLINE_ERROR            = 0x10, // VPC3 is not in OFFLINE state
    DP_ADDRESS_ERROR                = 0x11, // Slave Address Error
    DP_CALCULATE_IO_ERROR           = 0x12,

    DP_DOUT_LEN_ERROR               = 0x13,
    DP_DIN_LEN_ERROR                = 0x14,
    DP_DIAG_LEN_ERROR               = 0x15,
    DP_PRM_LEN_ERROR                = 0x16,
    DP_SSA_LEN_ERROR                = 0x17,
    DP_CFG_LEN_ERROR                = 0x18,
    DP_LESS_MEM_ERROR               = 0x19,
    DP_LESS_MEM_FDL_ERROR           = 0x1A,

    DP_PRM_ENTRY_ERROR              = 0x20,
    DP_PRM_DXB_MAX_LINK_ERROR       = 0x21,
    DP_PRM_DXB_ERROR                = 0x22,
    DP_CFG_ENTRY_ERROR              = 0x23,
    DP_CFG_UPDATE_ERROR             = 0x24,
    DP_PRM_DPV1_STATUS              = 0x25,
    DP_PRM_BLOCK_ERROR              = 0x26,
    DP_PRM_ALARM_ERROR              = 0x27,
    DP_PRMCMD_LEN_ERROR             = 0x28,
    DP_PRM_SOLL_IST_ERROR           = 0x29,
    DP_PRM_DXB_WD_ERROR             = 0x2A,
    DP_PRM_USER_PRM_BLOCK_ERROR     = 0x2B,

    DP_PRM_SERVICE_NOT_SUPPORTED    = 0x2F,

    DP_DIAG_BUFFER_ERROR            = 0x30,
    DP_DIAG_SEQUENCE_ERROR          = 0x31,
    DP_DIAG_OLD_DIAG_NOT_SEND_ERROR = 0x32,
    DP_DIAG_NOT_POSSIBLE_ERROR      = 0x33,
    DP_DIAG_NO_BUFFER_ERROR         = 0x34,
    DP_DIAG_BUFFER_LENGTH_ERROR     = 0x35,
    DP_DIAG_CONTROL_BYTE_ERROR      = 0x36,

    C1_DATA_LEN_ERROR               = 0x70,

    C2_DATA_LEN_ERROR               = 0x80,
    C2_DATA_POLL_TIMEOUT_ERROR      = 0x81,
    C2_DATA_SAP_ERROR               = 0x82,
    C2_NO_CONN_RESOURCE             = 0x83,
    C2_INV_LOWER_LAYER              = 0x84,
    C2_ENABLED_ERROR                = 0x85,
    C2_RESOURCE_ERROR               = 0x86,
    C2_INV_CN_ID                    = 0x87,
    C2_USER_ERR                     = 0x88,
    C2_DOUBLE_REQUEST               = 0x89,
    C2_ALREADY_DISCONNECTED         = 0x8A,

    SSC_MAX_DATA_PER_LINK           = 0x90,

    DP_EEPROM_ERROR                 = 0xF1, // Hardware errors
    DP_VPC3_ERROR                   = 0xF4,
    DP_SRAM_ERROR                   = 0xFF

} DP_ERROR_CODE;

typedef enum
{
   _DP_USER    = 0x10,
   _DP_IF      = 0x20,
   _DP_ISR     = 0x30,
   _DP_FDL     = 0x40,
   _DP_C1      = 0x50,
   _DP_C2      = 0x60,
   _TIME_IF    = 0x70
} DP_ERROR_FILE;


typedef struct
{
   UBYTE bFunction;
   UBYTE bErrorCode;
   UWORD bDetail;
   UBYTE bCnId;
}   VPC3_STRUC_ERRCB;
#define VPC3_ERRCB_PTR   VPC3_STRUC_ERRCB  MEM_PTR
extern VPC3_STRUC_ERRCB   sVpc3Error;


/*****************************************************************************/
/* reinclude-protection */


#else
    #pragma message "The header DP_IF.H is included twice or more !"
#endif


/*****************************************************************************/
/*  Copyright (C) profichip GmbH 2001. Confidential.                         */
/*****************************************************************************/
