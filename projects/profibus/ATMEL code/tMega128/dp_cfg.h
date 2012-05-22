/************************  Filename: dp_cfg.h ********************************/
/* ========================================================================= */
/*                                                                           */
/* 0000  000   000  00000 0  000  0   0 0 0000                               */
/* 0   0 0  0 0   0 0     0 0   0 0   0 0 0   0      Einsteinstraﬂe 6        */
/* 0   0 0  0 0   0 0     0 0     0   0 0 0   0      91074 Herzogenaurach    */
/* 0000  000  0   0 000   0 0     00000 0 0000       Germany                 */
/* 0     00   0   0 0     0 0     0   0 0 0                                  */
/* 0     0 0  0   0 0     0 0   0 0   0 0 0          Tel: ++49-9132-744-200  */
/* 0     0  0  000  0     0  000  0   0 0 0    GmbH  Fax: ++49-9132-744-204  */
/*                                                                           */
/* ========================================================================= */
/*                                                                           */
/* Function: Configuration file for VPC3+                                    */
/*                                                                           */
/* ------------------------------------------------------------------------- */
/*                                                                           */
/* Technical support:       Peter FREDEHORST                                 */
/*                          Tel. : ++49-9132-744-2150                        */
/*                          Fax. : ++49-9132-744-29-2150                     */
/*                          eMail: pfredehorst@profichip.com                 */
/*                          eMail: support@profichip.com                     */
/*                                                                           */
/*****************************************************************************/

/*! \file
     \brief Header file for VPC3+ configuration.

*/

/*****************************************************************************/
/* contents:

    - compiler switches
    - user defines
        -
        - length of buffers
        - defines for MSAC2
        - defines for MSAC1
        - defines for ALARM
        - defines for ISOCHRON
        - defines for DXB PUBLISHER
        - defines for DXB SUBSCRIBER
        - Hardwaremode
        - Interruptevents
    - calculating area (do not edit this part)

*/
/*****************************************************************************/
/* reinclude protection */


#ifndef DP_CFG_H
#define DP_CFG_H

/*--------------------------------------------------------------------------*/
/* profibus services                                                        */
/*--------------------------------------------------------------------------*/
/* todo: setup required services */

/*----------------------------------------------*/
/* NOTICE: Only VPC3+/B/C/S supports DPV1, DPV2 */
/*----------------------------------------------*/

/*!
   \def DP_MSAC_C1
   \brief Activate / Deactivate PROFIBUS DPV1 communication class1.

   Use following values:
   - 0 - Deactivate.
   - 1 - Activate.
*/
#define DP_MSAC_C1 0

/*!
   \def DP_MSAC_C2
   \brief Activate / Deactivate PROFIBUS DPV1 communication class1.

   Use following values:
   - 0 - Deactivate.
   - 1 - Activate.
*/
#define DP_MSAC_C2 0

/*!
   \def DP_ALARM
   \brief Activate / Deactivate PROFIBUS DPV1 alarm handling.

   Use following values:
   - 0 - Deactivate.
   - 1 - Activate.
*/
#define DP_ALARM   0

/*!
   \def DPV1_IM_SUPP
   \brief Activate / Deactivate I&M ( Identification and Maintenance ) functions.
   This functionality is mandatory for DPV1 slaves and will be checked during PROFIBUS certification.

   Use following values:
   - 0 - Deactivate.
   - 1 - Activate.
*/
#define DPV1_IM_SUPP 0

/*-----------------------------------------------------------------------*/
/* general slave parameter                                               */
/*-----------------------------------------------------------------------*/
/*!
   \def DP_ADDR
   \brief Set PROFIBUS slave address.

   Use following values:
   - 1 to 126
*/
#define DP_ADDR                     ((UBYTE)0x07)     // Slave address

/*!
   \def IDENT_NR
   \brief Set PROFIBUS ident number.

   Request your own number (www.profibus.com).
*/
#define IDENT_NR                    ((UWORD)0x4711)   // PROFIBUS Ident Number

/*!
   \def USER_WD
   \brief Set PROFIBUS user watchdog.
   The UserWatchdog is only active in DataExchange. The UserWatchdog
   isn't timebased, it's a counter of DataExchange-telegrams.
*/
#define USER_WD                     ((UWORD)0x01FF)   // User Watchdog

/*-----------------------------------------------------------------------*/
/* define buffer length                                                  */
/*-----------------------------------------------------------------------*/
/*!
   \def DIN_BUFSIZE
   \brief Set length of the DIn buffer (data from slave to master).

   Use following values:
   - 0 to 244
*/
#define DIN_BUFSIZE                 ((UBYTE)0x02)     // Length of the DIn Buffer (Data Slave to Master)                0..244

/*!
   \def DOUT_BUFSIZE
   \brief Set length of the DOut buffer (data from master to slave).

   Use following values:
   - 0 to 244
*/
#define DOUT_BUFSIZE                ((UBYTE)0x02)     // Length of the DOut Buffer     (Data Master to Slave)           0..244

/*!
   \def PRM_BUFSIZE
   \brief Set length of the paramater buffer.

   The first seven byte are mandatory.

   Use following values:
   - 7 to 244
*/
#define PRM_BUFSIZE                 ((UBYTE)0x0A)     // Length of the Parameter Buffer                                 7..244

/*!
   \def DIAG_BUFSIZE
   \brief Set length of the diagnosis buffer.

   The first 6 bytes are mandatory.

   Use following values:
   - 6 to 244
*/
#define DIAG_BUFSIZE                ((UBYTE)0x06)     // Length of the Diagnosis Buffer                                 6..244

/*!
   \def CFG_BUFSIZE
   \brief Set length of the diagnosis buffer.

   Use following values:
   - 1 to 244
*/
#define CFG_BUFSIZE                 ((UBYTE)0x01)     // Length of the Configuration Buffer                             1..244

/*!
   \def SSA_BUFSIZE
   \brief Set length of the set slave address buffer.

   Use following values:
   - 0 - deactivates the service set slave address
   - 4 to 244 - service is activated
*/
#define SSA_BUFSIZE                 ((UBYTE)0x00)     // Length of the Input Data in the Set_Slave_Address-Buffer 0 and 4..244
                                                      // 0: SetSlaveAddress will be deactivated!

/*-----------------------------------------------------------------------*/
/* settings for I&M Function                                             */
/*-----------------------------------------------------------------------*/
/*!
   \def MANUFACTURER_ID
   \brief Set Manufacturer-Id.

   The Manufacturer-Id is necessary for I&M functionality.

   Request your own number (www.profibus.com).
*/
#define MANUFACTURER_ID             ((UWORD)0x01BC)

/*!
   \def IM1_SUPP
   \brief Activate / Deactivate I&M data record 1 (IM1).

   Use following values:
   - 0 - Deactivate.
   - 1 - Activate.
*/
#define IM1_SUPP 0                                    // optional for DPV1 devices

/*!
   \def IM2_SUPP
   \brief Activate / Deactivate I&M data record 2 (IM2).

   Use following values:
   - 0 - Deactivate.
   - 1 - Activate.
*/
#define IM2_SUPP 0                                    // optional for DPV1 devices

/*!
   \def IM3_SUPP
   \brief Activate / Deactivate I&M data record 3 (IM3).

   Use following values:
   - 0 - Deactivate.
   - 1 - Activate.
*/
#define IM3_SUPP 0                                    // optional for DPV1 devices

/*!
   \def IM4_SUPP
   \brief Activate / Deactivate I&M data record 4 (IM4).

   Use following values:
   - 0 - Deactivate.
   - 1 - Activate.
*/
#define IM4_SUPP 0                                    // optional for DPV1 devices

/*-----------------------------------------------------------------------*/
/* settings for MSAC_C1                                                  */
/*-----------------------------------------------------------------------*/
/*!
   \def C1_LEN
   \brief Length of the MSAC_C1 Data

   The parameter specifies the maximum length of user data, including
   Function_Num, Slot_number, Index, Length, transferred on the
   MSAC_C1 communication channel.

   Use following values:
   - 5..244
*/
#define C1_LEN                      ((UBYTE)0xF4)

/*-----------------------------------------------------------------------*/
/* settings for MSAC_C1 Alarm                                            */
/*-----------------------------------------------------------------------*/
/*!
   \def DP_ALARM_OVER_SAP50
   \brief Activate / Deactivate alarm acknowledge via SAP 50.

   Use following values:
   - 0 - Deactivate.
   - 1 - Activate.
*/
#if DP_ALARM
   #define DP_ALARM_OVER_SAP50  0                     // Alarm Acknowledge over SAP50
#endif//#if DP_ALARM

/*-----------------------------------------------------------------------*/
/* settings for MSAC_C2                                                  */
/*-----------------------------------------------------------------------*/
/*!
   \def DP_MSAC_C2_TIME
   \brief Activate / Deactivate MSAC-C2 time control.

   Use following values:
   - 0 - Deactivate.
   - 1 - Activate.
*/
#define DP_MSAC_C2_TIME 1                             // enables time control for msac-c2

/*!
   \def C2_NUM_SAPS
   \brief Define number of MSAC-C2 connections.
*/
#define C2_NUM_SAPS                 ((UBYTE)0x03)     // Number of MSAC_C2 connections

/*!
   \def C2_LEN
   \brief Length of the MSAC_C2 Data

   The parameter specifies the maximum length of an Initiate Request
   PDU including the Function_Num to the Resource Manager.
   The parameter specifies the maximum length of user data, including
   Function_Num, Slot_number, Index, Length, transferred on the
   MSAC_2 communication channel.

   Use following values:
   - 0,52..244
*/
#define C2_LEN                      ((UBYTE)0xF4)

/*!
   \def C2_FEATURES_SUPPORTED_1
   \brief Define supported features on MSAC-C2 connection.

   Fix value: 0x01 - read and write supported
*/
#define C2_FEATURES_SUPPORTED_1     ((UBYTE)0x01)     // default: 0x01, Read and Write supported

/*!
   \def C2_FEATURES_SUPPORTED_2
   \brief Define supported features on MSAC-C2 connection.

   Fix value: 0x00
*/
#define C2_FEATURES_SUPPORTED_2     ((UBYTE)0x00)     // default: 0x00

/*!
   \def C2_PROFILE_FEATURES_1
   \brief Define profile features on MSAC-C2 connection.

   Value: 0x00 or profile specific
*/
#define C2_PROFILE_FEATURES_1       ((UBYTE)0x00)     // default: 0x00, vendor or profile specific

/*!
   \def C2_PROFILE_FEATURES_2
   \brief Define profile features on MSAC-C2 connection.

   Value: 0x00 or profile specific
*/
#define C2_PROFILE_FEATURES_2       ((UBYTE)0x00)     // default: 0x00, vendor or profile specific

/*!
   \def C2_PROFILE_NUMBER
   \brief Define profile number on MSAC-C2 connection.

   Value: 0x0000 or profile specific
*/
#define C2_PROFILE_NUMBER           ((UWORD)0x0000)   // default: 0x0000, vendor or profile specific

/*-----------------------------------------------------------------------*/
/* settings for DPV2 - Isochron Mode                                     */
/*-----------------------------------------------------------------------*/
/*!
   \def SYNCH_PULSEWIDTH
   \brief Set width of synch pulse (1/12µs).

   Defaultvalue: 0xFF
*/
#define SYNCH_PULSEWIDTH            ((UBYTE)0xFF)     // Width of Synchpulse in 1/12µs

/*-----------------------------------------------------------------------*/
/* settings for DPV2 - DXB Subscriber                                    */
/*-----------------------------------------------------------------------*/
/*!
   \def MAX_LINK_SUPPORTED
   \brief Set number of supported DxB links.

   Fix value: 0x01 - Profichip asics support only one DxB connection!
*/
#define MAX_LINK_SUPPORTED          ((UBYTE)0x01)     // Number of Links

/*!
   \def MAX_DATA_PER_LINK
   \brief Define maximum number of data on DxB link.

   Value: 1..244
*/
#define MAX_DATA_PER_LINK           ((UBYTE)0x10)     // maximal Number of Data per Link

/*---------------------------------------------------------------------------*/
/* set hardware modes                                                        */
/*---------------------------------------------------------------------------*/
/*!
   \def DP_VPC3_4KB_MODE
   \brief Activate / Deactivate VPC3+ 4kByte-mode.

   Use following values:
   - 0 - VPC3+ works in 2kByte mode.
   - 1 - VPC3+ works in 4kByte mode.
*/
#define DP_VPC3_4KB_MODE 1

/*-----------------------------------------------------------------------*/
/* ModeRegister0 (7..0) ( page 15 )                                      */
/*-----------------------------------------------------------------------*/
//
//  bit  7       6      5        4         3       2       1          0
//  --------------------------------------------------------------------------
//  | Freeze | Sync | Early | Int_Pol | MinTSDR |  WD  | Dis_Stop | Dis_Start |
//  |  supp. | supp.|  RDY  |         |         | BASE | Control  | Control   |
//  --------------------------------------------------------------------------
//       1       1      0        0         0       0        0          0        = 0xC0 // Default
//
/*!
   \def INIT_VPC3_MODE_REG_L
   \brief Set Moderegister 0 (Bit 0.. Bit 7).


   <b>Bit 0:</b> Dis_Start_Control - Disable Startbit Control
      - 0 - Monitoring the following start bit is enabled.
      - 1 - Monitoring the following start bit is switched off

   \attention A Set-Param telegram overwrites this memory cell in the DP mode.(Refer to the user specific data.)

   <b>Bit 1:</b> Dis_Stop_Control - Disable Stopbit Control
      - 0 - Stop bit monitoring is enabled.
      - 1 - Stop bit monitoring is switched off

   \attention A Set-Param telegram overwrites this memory cell in the DP mode.(Refer to the user specific data.)

   <b>Bit 2:</b> WD_Base: Watchdog Time Base
      - 0 - Watchdog time base is 10 ms (default state)
      - 1 - Watchdog time base is  1 ms

   <b>Bit 3:</b> MinTSDR: Default setting for the MinTSDR after reset for DP operation or combi operation.
      - 0 - Pure DP operation (default configuration!)

   <b>Bit 4:</b> INT_Pol: Interrupt Polarity
      - 0 - The interrupt output is low-active.
      - 1 - The interrupt output is high-active.

   <b>Bit 5:</b> Early_Rdy: Early Ready
      - 0 - Normal Ready: Ready is generated when data is valid (read) or when data has been accepted (write).
      - 1 - Ready is generated one clock pulse earlier.

   <b>Bit 6:</b> Sync_Supported: Sync_Mode support
      - 0 - Sync_Mode is not supported.
      - 1 - Sync_Mode is supported.

   <b>Bit 7:</b> Freeze_Supported - Freeze_Mode support
      - 0 - Freeze_Mode is not supported.
      - 1 - Freeze_Mode is supported
*/
#define INIT_VPC3_MODE_REG_L        ((UBYTE)0xC0)

/*-----------------------------------------------------------------------*/
/* ModeRegister0 (15..8) ( page 15 )                                     */
/*-----------------------------------------------------------------------*/
//
//  bit15      14       13           12           11           10         9         8
//  ------------------------------------------------------------------------------------
//  | Res | PrmCmd | Spec_Clear | Spec_Prm |  SetExtPrm  | User_Time | EOI_Time |  DP  |
//  |     |  Supp  |    Mode    | Buf_Mode |     Supp    |    Base   |   Base   | Mode |
//  ------------------------------------------------------------------------------------
//     0       0        1            0              0           1         0         1   = 0x25 // Default
//
/*!
   \def INIT_VPC3_MODE_REG_H
   \brief Set Moderegister 0 (Bit 8.. Bit 15).


   <b>Bit 8:</b> DP_Mode: DP_Mode enable
      - 0 - Monitoring the following start bit is enabled.
      - 1 - Monitoring the following start bit is switched off

   <b>Bit 9:</b> EOI_Time_Base: End-of-Interrupt Timebase
      - 0 - The interrupt inactive time is at least 1 µsec long.
      - 1 - The interrupt inactive time is at least 1 ms long

   <b>Bit 19:</b> User_Time_Base: Timebase of the cyclical User_Time_Clock-Interrupt
      - 0 - The User_Time_Clock-Interrupt occurs every 1 ms.
      - 1 - The User_Time_Clock-Interrupt occurs every 10 ms. (mandatory DPV1)

   <b>Bit 11:</b> Set_Ext_Prm_Supported: Set_Ext_Prm telegram support
      - 0 - SAP 53 is deactivated
      - 1 - SAP 53 is activated

   <b>Bit 12:</b> Spec_Prm_Buf_Mode: Special Parameter Buffer Mode
      - 0 - No special parameter buffer.
      - 1 - Special parameter buffer mode. Parameterization data will be stored directly in the special parameter buffer.

   <b>Bit 13:</b> Spec_Clear_Mode: Special Clear Mode (Fail Safe Mode)
      - 0 - No special clear mode.
      - 1 - Special clear mode. VPC3+ will accept data telegrams with data unit = 0

   <b>Bit 14:</b> PrmCmd_Supported: PrmCmd support for redundancy
      - 0 - PrmCmd is not supported.
      - 1 - PrmCmd is supported

   <b>Bit 15:</b> Reserved
*/
#define INIT_VPC3_MODE_REG_H        ((UBYTE)0x25)


/*-----------------------------------------------------------------------*/
/* ModeRegister2 (7..0) ( page 19 )   (only VPC3+B, VPC3+C)              */
/*-----------------------------------------------------------------------*/
//
//  bit  7          6          5        4       3          2          1            0
//  --------------------------------------------------------------------------------------
//  |  4KB  |   No_Check   | SYNC_  | SYNC_ | DX_Int_ | DX_Int_ |  No_Check_  | NEW_GC_  |
//  |  Mode | Prm_Reserved |  Pol   |  ENA  |  Port   |  Mode   | GC_RESERVED | Int_Mode |
//  --------------------------------------------------------------------------------------
//       1          0          0        0       0          1          0            1        = 0x05 or 0x85
//
/*!
   \def INIT_VPC3_MODE_REG_2
   \brief Set Moderegister 2 (Bit 0.. Bit 7).

   <b>Bit 0:</b> GC_Int_Mode: Controls generation of GC Interrupt
      - 0 - GC Interrupt is only generated, if changed GC telegram is received
      - 1 - GC Interrupt is generated after every GC telegram (default)

   <b>Bit 1:</b> No_Check_GC_Reserved: Disables checking of the reserved GC bits
      - 0 - Reserved bits of GC-telegram are checked (default).
      - 1 - Reserved bits of GC-telegram are not checked.

   <b>Bit 2:</b> DX_Int_Mode: Mode of Dataexchange Interrupt
      - 0 - DX Interrupt only generated, if DOUT length not 0 (default).
      - 1 - DX Interrupt generated after every DX-telegram

   <b>Bit 3:</b> DX_Int_Port: Port mode for Dataexchange Interrupt
      - 0 - DX Interrupt not assigned to port DATA_EXCH (default).
      - 1 - DX Interrupt (synchronized to GC-SYNC) assigned to port DATA_EXCH.

   <b>Bit 4:</b> SYNC_Ena: Enable generation of SYNC pulse (for Isochron Mode only)
      - 0 - SYNC pulse generation is disabled (default).
      - 1 - SYNC pulse generation is enabled.

   <b>Bit 5:</b> SYNC_Pol: Polarity of SYNC pulse (for Isochron Mode only)
      - 0 - negative polarity of SYNC pulse (default)
      - 1 - positive polarity of SYNC pulse

   <b>Bit 6:</b> No_Check_Prm_Reserved: Disables checking of the reserved Prm bits
      - 0 - Reserved bits of Prm-telegram are checked (default).
      - 1 - Reserved bits of Prm-telegram are not checked.

   <b>Bit 7:</b> 4kB_Mode: Size of internal RAM
      - 0 - 2kB RAM (default).
      - 1 - 4kB RAM
*/
#if DP_VPC3_4KB_MODE
   #define INIT_VPC3_MODE_REG_2    ((UBYTE)0x85)
#else
   #define INIT_VPC3_MODE_REG_2    ((UBYTE)0x05)
#endif//#if DP_VPC3_4KB_MODE

/*-----------------------------------------------------------------------*/
/* ModeRegister3 (7..0) ( page 19 )   (VPC3+C)                           */
/*-----------------------------------------------------------------------*/
//
//  bit  7          6          5          4          3         2       1         0
//  -----------------------------------------------------------------------------------
//  | RESERVED | RESERVED | RESERVED | RESERVED |   PLL   | En-Chk | DX-OUT | GC_Int  |
//  |          |          |          |          |   SUPP  |  SSAP  |  Mode  | Mode_Ex |
//  -----------------------------------------------------------------------------------
//       0          0          0          0          0         0       0         0        = 0x00
//
/*!
   \def INIT_VPC3_MODE_REG_3
   \brief Set Moderegister 2 (Bit 0.. Bit 3).

   <b>Bit 0:</b> GC_Int_Mode_Ext: extend GC_Int_Mode, works only if GC_Int_Mode=1
      - 0 - GC Interrupt is only generated, if changed GC telegram is received
      - 1 - GC Interrupt is only generated, if GC telegram with changed Control_Command is received

   <b>Bit 1:</b> DX_Int_Mode_2: Mode of DX_out interrupt
      - 0 - DX_Out interrupt is generated after each Data_Exch telegram
      - 1 - DX_Out interrupt is only generated, if received data is not equal to current data in DX_Out buffer of user

   <b>Bit 2:</b> En_Chk_SSAP: Evaluation of Source Address Extension
      - 0 - VPC3+ accept any value of S_SAP
      - 1 - VPC3+ only process the received telegram if the S_SAP match to the default values presented by the IEC 61158

   <b>Bit 3:</b> PLL_Supported: Enables PLL for clock save
      - 0 - PLL is disabled
      - 1 - PLL is enabled; For use of PLL, SYNC_Ena must be set.

   <b>Bit 4:</b> Reserved

   <b>Bit 5:</b> Reserved

   <b>Bit 6:</b> Reserved

   <b>Bit 7:</b> Reserved
*/
#define INIT_VPC3_MODE_REG_3    ((UBYTE)0x00)

/*---------------------------------------------------------------------------*/
/* set interrupt indications                                                 */
/*---------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------*/
/* Interrupt Mask Register (7..0) ( page 23 )                            */
/*-----------------------------------------------------------------------*/
//  bit7       6          5           4             3             2          1         0
//  ----------------------------------------------------------------------------------------
//  | DXB | New_Ext  | DXB_LINK | User_Timer |    WD_DP     | Baud_Rate | Go/Leave | Clock  |
//  | OUT | PRM_Data |  ERROR   |   Clock    | Mode_Timeout |   detect  |  DataEx  | Sync   |
//  ----------------------------------------------------------------------------------------
//     0       0          0           0             0             0          2         0      = 0x02 // Default
//
/*!
   \def INIT_VPC3_IND_L
   \brief Set interrupt mask register (Bit 0.. Bit 7).

   <b>Bit 0:</b> Clock_Sync: The VPC3+D/S has received a Clock_Value telegram or an error occurs.
      - 0 - The VPC3+ doesn't generates an interrupt.
      - 1 - The VPC3+ generates an interrupt.

   <b>Bit 1:</b> Go/Leave_DATA_EX: The DP_SM has entered or exited the DATA_EX state
      - 0 - The VPC3+ doesn't generates an interrupt.
      - 1 - The VPC3+ generates an interrupt.

   <b>Bit 2:</b> Baudrate_Detect: The VPC3+ has left the Baud_Search state and found a baud rate.
      - 0 - The VPC3+ doesn't generates an interrupt.
      - 1 - The VPC3+ generates an interrupt.

   <b>Bit 3:</b> WD_DP_Control_Timeout: The watchdog timer has run out in the DP_Control WD state
      - 0 - The VPC3+ doesn't generates an interrupt.
      - 1 - The VPC3+ generates an interrupt.

   <b>Bit 4:</b> User_Timer_Clock: The time base for the User_Timer_Clocks has run out ( 1 /10ms).
      - 0 - The VPC3+ doesn't generates an interrupt.
      - 1 - The VPC3+ generates an interrupt.

   <b>Bit 5:</b> DXB_Link_Error: The Watchdog cycle is elapsed and at least one Publisher-Subscriber connection breaks down.
      - 0 - The VPC3+ doesn't generates an interrupt.
      - 1 - The VPC3+ generates an interrupt.

   <b>Bit 6:</b> New_Ext_Prm_Data: The VPC 3+ has received a Set_Ext_Param telegram and made the data available in the Prm buffer.
      - 0 - The VPC3+ doesn't generates an interrupt.
      - 1 - The VPC3+ generates an interrupt.

   <b>Bit 7:</b> DXB_Out: VPC 3+ has received a DXB telegram and made the new output data available in the N buffer.
      - 0 - The VPC3+ doesn't generates an interrupt.
      - 1 - The VPC3+ generates an interrupt.
*/
#define INIT_VPC3_IND_L  0x02

/*-----------------------------------------------------------------------*/
/* Interrupt Mask Register (15..8) ( page 23 )                           */
/*-----------------------------------------------------------------------*/
//  bit  15         14       13        12            11       10       9         8
//  -----------------------------------------------------------------------------------
//  | REQ_PDU | POLL_END | DX_OUT | Diag_Buffer | New_PRM | NewCfg | NewSSA |  NewGC  |
//  |   Ind   |    Ind   |        |   Changed   |   Data  |  Data  |  Data  | Command |
//  -----------------------------------------------------------------------------------
//       0          0        1          1            1       1        0         1      = 0x3D // Default
//
/*!
   \def INIT_VPC3_IND_H
   \brief Set interrupt mask register (Bit 8.. Bit 15).

   <b>Bit 8:</b> New_GC_Command: The VPC3+ has received a Global_Control telegram and this byte is stored in the R_GC_Command RAM cell.
      - 0 - The VPC3+ doesn't generates an interrupt.
      - 1 - The VPC3+ generates an interrupt.

   <b>Bit 9:</b> New_SSA_Date: The VPC3+ has received a Set_Slave_Address telegram and made the data available in the SSA buffer.
      - 0 - The VPC3+ doesn't generates an interrupt.
      - 1 - The VPC3+ generates an interrupt.

   <b>Bit 10:</b> New_Cfg_Data: The VPC3+ has received a Check_Cfg telegram and made the data available in the Cfg buffer.
      - 0 - The VPC3+ doesn't generates an interrupt.
      - 1 - The VPC3+ generates an interrupt.

   <b>Bit 11:</b> New_Prm_Data: The VPC3+ has received a Set_Param telegram and made the data available in the Prm buffer.
      - 0 - The VPC3+ doesn't generates an interrupt.
      - 1 - The VPC3+ generates an interrupt.

   <b>Bit 12:</b> Diag_Buffer_Changed: Due to the request made by New_Diag_Cmd, VPC3+ exchanged the diagnostics buffer and again made the old buffer available to the user.
      - 0 - The VPC3+ doesn't generates an interrupt.
      - 1 - The VPC3+ generates an interrupt.

   <b>Bit 13:</b> DX_Out: The VPC3+ has received a DX-Out telegram and made the data available in the DX-Out buffer.
      - 0 - The VPC3+ doesn't generates an interrupt.
      - 1 - The VPC3+ generates an interrupt.

   <b>Bit 14:</b> Poll_End_Ind: The VPC 3+ has send the response to an acyclic service.
      - 0 - The VPC3+ doesn't generates an interrupt.
      - 1 - The VPC3+ generates an interrupt.

   <b>Bit 15:</b> FDL_Ind: The VPC 3+ has received an acyclic service request and made the data available in an indication buffer.
      - 0 - The VPC3+ doesn't generates an interrupt.
      - 1 - The VPC3+ generates an interrupt.
*/
#define INIT_VPC3_IND_H  0x3D

#if VPC3_SERIAL_MODE

   /*-----------------------------------------------------------------------*/
   /* Interrupt Mask Register (15..0) ( page 23 )                           */
   /*-----------------------------------------------------------------------*/
   //  bit  15         14       13         12           11       10       9         8       7       6          5           4             3             2          1         0
   //  ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
   //  | REQ_PDU | POLL_END | DX_OUT | Diag_Buffer | New_PRM | NewCfg | NewSSA |  NewGC  | DXB | New_Ext  | DXB_LINK | User_Timer |    WD_DP     | Baud_Rate | Go/Leave | Clock  |
   //  |   Ind   |    Ind   |        |   Changed   |   Data  |  Data  |  Data  | Command | OUT | PRM_Data |  ERROR   |   Clock    | Mode_Timeout |   detect  |  DataEx  | Sync   |
   //  ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
   //       0          0        1          1            1        1        0        1        0       0          0           0             0             0          1         0      = 0x3D02 // Default
   //
   #define SM_INTERRUPT_MASK ((UWORD)0x3D02)

#endif//#if VPC3_SERIAL_MODE

/*---------------------------------------------------------------------------*/
/* end of user defines                                                       */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* !!!!! do not edit this calculating part !!!!!                             */
/*---------------------------------------------------------------------------*/
// == Segmentation VPC3+ ===============================================================
#if DP_VPC3_4KB_MODE
   // 16 Byte segmentation
   #define SEG_OFFSET               ((UBYTE)0x0F)
   #define SEG_MULDIV               ((UBYTE)0x04)
   #define SEG_ADDBYTE              ((UWORD)0xFFF0)
   #define SEG_ADDWORD              ((UWORD)0xFFF0)
#else
   // 8 Byte segmentation
   #define SEG_OFFSET               ((UBYTE)0x07)
   #define SEG_MULDIV               ((UBYTE)0x03)
   #define SEG_ADDBYTE              ((UWORD)0xFFF8)
   #define SEG_ADDWORD              ((UWORD)0xFFF8)
#endif//#if DP_VPC3_4KB_MODE

// == DPV1/DPV2 ========================================================================
#define DP_C1_USE_RD_WR_SAP         0x01
#define DP_C1_USE_ALARM_SAP         0x04

// == MSAC1 ============================================================================
#if DP_ALARM
   #define DP_MSAC_C1               1
#endif//#if DP_ALARM

#if DP_MSAC_C1

   #define DP_C1_LEN                C1_LEN

   // == Alarm ========================================================================
   #if DP_ALARM
      #if DP_ALARM_OVER_SAP50
         #define DP_C1_SAPS         ( DP_C1_USE_RD_WR_SAP | DP_C1_USE_ALARM_SAP )
      #else
         #define DP_C1_SAPS         ( DP_C1_USE_RD_WR_SAP )
      #endif//#if DP_ALARM_OVER_SAP50
   #else
      #define DP_C1_SAPS            ( DP_C1_USE_RD_WR_SAP )
   #endif//#if DP_ALARM
#else
   #define DP_C1_SAPS               0x00
   #define DP_C1_LEN                ((UBYTE)0x00)

   #define DP_ALARM                 0
#endif//#if DP_MSAC_C1

// == MSAC2 ============================================================================
#if DP_MSAC_C2
   #define DP_C2_NUM_SAPS           C2_NUM_SAPS
   #define DP_C2_LEN                C2_LEN
#else
   #define DP_C2_NUM_SAPS           ((UBYTE)0x00)
   #define DP_C2_LEN                ((UBYTE)0x00)
#endif//#if DP_MSAC_C2

// == Subscriber =======================================================================
#ifdef DP_SUBSCRIBER
   #define DP_MAX_LINK_SUPPORTED    MAX_LINK_SUPPORTED
   #define DP_MAX_DATA_PER_LINK     MAX_DATA_PER_LINK
#else
   #define DP_MAX_LINK_SUPPORTED    0
   #define DP_MAX_DATA_PER_LINK     0
#endif//#ifdef DP_SUBSCRIBER

// == Calculating FDL list =============================================================
#if ( DP_MSAC_C1 || DP_MSAC_C2 )
   #define DP_FDL                   1
#else
   #define DP_FDL                   0
#endif//#if ( DP_MSAC_C1 || DP_MSAC_C2 )

#if (DP_C1_SAPS & DP_C1_USE_RD_WR_SAP)
   #define DP_TEMP_X1               1
#else
   #define DP_TEMP_X1               0
#endif//#if (DP_C1_SAPS & DP_C1_USE_RD_WR_SAP)

#if (DP_C1_SAPS & DP_C1_USE_ALARM_SAP )
   #define DP_TEMP_X2               1
   #define SAP_50
#else
   #define DP_TEMP_X2               0
#endif//#if (DP_C1_SAPS & DP_C1_USE_ALARM_SAP )

#if DP_MSAC_C1
   #define DP_TEMP_X (DP_TEMP_X1 + DP_TEMP_X2)
#else
   #define DP_TEMP_X                0
#endif//#if DP_MSAC_C1

#if DP_MSAC_C2
   #define DP_MAX_SAPS              (DP_TEMP_X + DP_C2_NUM_SAPS + 1 )
#else
   #define DP_MAX_SAPS              DP_TEMP_X
#endif//#if DP_MSAC_C2

#define DP_ORG_LENGTH               0x40 // organizational parameter

#ifdef DP_TIMESTAMP
   #define DP_TIMESTAMP_LENGTH      0x20
#else
   #define DP_TIMESTAMP_LENGTH      0x00
#endif//#ifdef DP_TIMESTAMP

#if DP_FDL
   #define FDL_SAP_MAX DP_MAX_SAPS
   /* length of vpc3-sap-list */
   #define SAP_LENGTH               (((FDL_SAP_MAX*7)+SEG_OFFSET) & SEG_ADDBYTE)
#else
   #define SAP_LENGTH               0x10
#endif//#if DP_FDL

#if DP_VPC3_4KB_MODE
   #define ASIC_RAM_LENGTH          0x1000
   #define ASIC_USER_RAM            (ASIC_RAM_LENGTH - DP_ORG_LENGTH - SAP_LENGTH - DP_TIMESTAMP_LENGTH)
#else
   #define ASIC_RAM_LENGTH          0x800
   #define ASIC_USER_RAM            (ASIC_RAM_LENGTH - DP_ORG_LENGTH - SAP_LENGTH - DP_TIMESTAMP_LENGTH)
#endif//#if DP_VPC3_4KB_MODE

#define ISR_ENABLE_VPC3_INT_CLOCK_SYNC              0
#define ISR_ENABLE_VPC3_INT_GO_LEAVE_DATA_EX        0
#define ISR_ENABLE_VPC3_INT_BAUDRATE_DETECT         0
#define ISR_ENABLE_VPC3_INT_WD_DP_TIMEOUT           0
#define ISR_ENABLE_VPC3_INT_USER_TIMER_CLOCK        0
#define ISR_ENABLE_VPC3_INT_DXB_LINK_ERROR          0
#define ISR_ENABLE_VPC3_INT_NEW_EXT_PRM_DATA        0
#define ISR_ENABLE_VPC3_INT_DXB_OUT                 0
#define ISR_ENABLE_VPC3_INT_NEW_GC_COMMAND          0
#define ISR_ENABLE_VPC3_INT_NEW_SSA_DATA            0
#define ISR_ENABLE_VPC3_INT_NEW_CFG_DATA            0
#define ISR_ENABLE_VPC3_INT_NEW_PRM_DATA            0
#define ISR_ENABLE_VPC3_INT_DIAG_BUF_CHANGED        0
#define ISR_ENABLE_VPC3_INT_DX_OUT                  0
#define ISR_ENABLE_VPC3_INT_RESERVED                0
#define ISR_ENABLE_VPC3_INT_SERVICE_ERROR           0
#define ISR_ENABLE_VPC3_INT_POLL_END_IND            0
#define ISR_ENABLE_VPC3_INT_FDL_IND                 0


#if( INIT_VPC3_IND_L & 0x01 )
   #undef  ISR_ENABLE_VPC3_INT_CLOCK_SYNC
   #define ISR_ENABLE_VPC3_INT_CLOCK_SYNC           1
#endif
#if( INIT_VPC3_IND_L & 0x02 )
   #undef  ISR_ENABLE_VPC3_INT_GO_LEAVE_DATA_EX
   #define ISR_ENABLE_VPC3_INT_GO_LEAVE_DATA_EX     1
#endif
#if( INIT_VPC3_IND_L & 0x04 )
   #undef  ISR_ENABLE_VPC3_INT_BAUDRATE_DETECT
   #define ISR_ENABLE_VPC3_INT_BAUDRATE_DETECT      1
#endif
#if( INIT_VPC3_IND_L & 0x08 )
   #undef  ISR_ENABLE_VPC3_INT_WD_DP_TIMEOUT
   #define ISR_ENABLE_VPC3_INT_WD_DP_TIMEOUT        1
#endif
#if( INIT_VPC3_IND_L & 0x10 )
   #undef  ISR_ENABLE_VPC3_INT_USER_TIMER_CLOCK
   #define ISR_ENABLE_VPC3_INT_USER_TIMER_CLOCK     1
#endif
#if( INIT_VPC3_IND_L & 0x20 )
   #undef  ISR_ENABLE_VPC3_INT_DXB_LINK_ERROR
   #define ISR_ENABLE_VPC3_INT_DXB_LINK_ERROR       1
#endif
#if( INIT_VPC3_IND_L & 0x40 )
   #undef  ISR_ENABLE_VPC3_INT_NEW_EXT_PRM_DATA
   #define ISR_ENABLE_VPC3_INT_NEW_EXT_PRM_DATA     1
#endif
#if( INIT_VPC3_IND_L & 0x80 )
   #undef  ISR_ENABLE_VPC3_INT_DXB_OUT
   #define ISR_ENABLE_VPC3_INT_DXB_OUT              1
#endif


#if( INIT_VPC3_IND_H & 0x01 )
   #undef  ISR_ENABLE_VPC3_INT_NEW_GC_COMMAND
   #define ISR_ENABLE_VPC3_INT_NEW_GC_COMMAND       1
#endif
#if( INIT_VPC3_IND_H & 0x02 )
   #undef  ISR_ENABLE_VPC3_INT_NEW_SSA_DATA
   #define ISR_ENABLE_VPC3_INT_NEW_SSA_DATA         1
#endif
#if( INIT_VPC3_IND_H & 0x04 )
   #undef  ISR_ENABLE_VPC3_INT_NEW_CFG_DATA
   #define ISR_ENABLE_VPC3_INT_NEW_CFG_DATA         1
#endif
#if( INIT_VPC3_IND_H & 0x08 )
   #undef  ISR_ENABLE_VPC3_INT_NEW_PRM_DATA
   #define ISR_ENABLE_VPC3_INT_NEW_PRM_DATA         1
#endif
#if( INIT_VPC3_IND_H & 0x10 )
   #undef  ISR_ENABLE_VPC3_INT_DIAG_BUF_CHANGED
   #define ISR_ENABLE_VPC3_INT_DIAG_BUF_CHANGED     1
#endif
#if( INIT_VPC3_IND_H & 0x20 )
   #undef  ISR_ENABLE_VPC3_INT_DX_OUT
   #define ISR_ENABLE_VPC3_INT_DX_OUT               1
#endif
#if( INIT_VPC3_IND_H & 0x40 )
   #undef  ISR_ENABLE_VPC3_INT_POLL_END_IND
   #define ISR_ENABLE_VPC3_INT_POLL_END_IND         1
#endif
#if( INIT_VPC3_IND_H & 0x80 )
   #undef  ISR_ENABLE_VPC3_INT_FDL_IND
   #define ISR_ENABLE_VPC3_INT_FDL_IND              1
#endif


/*****************************************************************************/
/* reinclude-protection */


#else
    #pragma message "The header DP_CFG.H is included twice or more !"
#endif

/*****************************************************************************/
/*  Copyright (C) profichip GmbH 2009. Confidential.                         */
/*****************************************************************************/

