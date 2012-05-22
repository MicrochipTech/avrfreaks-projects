/**************************  Filename: dp_user.c  ****************************/
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
/* Function:      Demo for PROFICHIP Extension Board AT89C5132               */
/*                This example simulates simple profibus device              */
/*                with 2 byte of input and 2 byte of output data.            */
/*                                                                           */
/* ------------------------------------------------------------------------- */
/*                                                                           */
/* Hardware requirements: ProfiChip Evaluation Board AT89C5132  (PA006101)   */
/*                        ProfiChip Evaluation Board VPC3+/C    (PA006103)   */
/*                                                                           */
/* ------------------------------------------------------------------------- */
/* memory:  0000H ... 7FFFH: RAM                                             */
/*          8000H ... 8FFFH: VPC3+                                           */
/*          9000H ... 9FFFH: Reserved                                        */
/*          A000H ... AFFFH: FPGA                                            */
/*          B000H ... BFFFH: RTC                                             */
/*          C000H ... CFFFH: LCD                                             */
/*          D000H ... DFFFH: I/O Port 0                                      */
/*          E000H ... EFFFH: I/O Port 1                                      */
/*          F000H ... FFFFH: I/O Port 2                                      */
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


/*****************************************************************************/
/* contents:

  - function prototypes
  - data structures
  - internal functions

*/
/*****************************************************************************/
/* include hierarchy */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "platform.h"
#include "dp_inc.h"

/*---------------------------------------------------------------------------*/
/* defines, structures                                                       */
/*---------------------------------------------------------------------------*/
// -- defines for user state
#define USER_STATE_CLEAR            ((UBYTE)0x00)
#define USER_STATE_RUN              ((UBYTE)0x01)
// -- defines for diagnostics
#define USER_TYPE_CFG_OK            ((UBYTE)0xFD)
#define USER_TYPE_APPL_RDY          ((UBYTE)0xFF)

typedef struct
{
   CFG_STRUCT  sCfgData;

   #if VPC3_SERIAL_MODE
      UBYTE abPrmData[ PRM_BUFSIZE ];
      UBYTE abCfgData[ CFG_BUFSIZE ];
   #endif//#if VPC3_SERIAL_MODE

   UBYTE abInput[2];
   UBYTE abOutput[2];
} USER_STRUC;

/*---------------------------------------------------------------------------*/
/* global user data definitions                                              */
/*---------------------------------------------------------------------------*/
VPC3_STRUC_PTR             pVpc3;               // pointer to Vpc3 structure
VPC3_STRUC_PTR             pVpc3Channel1;       // pointer to Vpc3 structure channel 1

VPC3_ADR                   Vpc3AsicAddress;     // global asic address

VPC3_SYSTEM_STRUC_PTR      pDpSystem;           // global system structure
VPC3_SYSTEM_STRUC          sDpSystemChannel1;   // global system structure

VPC3_STRUC_ERRCB           sVpc3Error;          // error structure
USER_STRUC                 sUser;               // user structure

#if VPC3_SERIAL_MODE
   VPC3_STRUC              sVpc3OnlyForInit;    // structure is used for initialization of VPC3+
                                                // buffer calculation
#endif//#if VPC3_SERIAL_MODE

/*---------------------------------------------------------------------------*/
/* defines, structures and variables for our demo application                */
/*---------------------------------------------------------------------------*/

#if VPC3_SERIAL_MODE
   #if VPC3_SPI
      ROMCONST__ UBYTE NAME[12] = { 0x45, 0x41, 0x53, 0x59, 0x34, 0x37, 0x31, 0x31, 0x2D, 0x53, 0x50, 0x49 }; //EASY4711-SPI
   #endif//#if VPC3_SPI

   #if VPC3_I2C
      ROMCONST__ UBYTE NAME[12] = { 0x45, 0x41, 0x53, 0x59, 0x34, 0x37, 0x31, 0x31, 0x2D, 0x49, 0x49, 0x43 }; //EASY4711-IIC
   #endif//#if VPC3_I2C
#else
      ROMCONST__ UBYTE NAME[12] = { 0x45, 0x41, 0x53, 0x59, 0x34, 0x37, 0x31, 0x31, 0x20, 0x20, 0x20, 0x20 }; //EASY4711
#endif//#if VPC3_SERIAL_MODE

//default configuration data for startup
#define UserCfgDataLength ((UBYTE)0x01)
ROMCONST__ UBYTE DefCfg[1] = { 0x31 };

/*---------------------------------------------------------------------------*/
/* function prototypes                                                       */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* function: UserAlarm ( is also called from alarm state machine !!!! )      */
/*---------------------------------------------------------------------------*/
UBYTE UserAlarm( UBYTE bAlarmType, UBYTE bCheckDiagFlag )
{
MEM_UNION_DIAG sDiag;
UBYTE           bRetValue;
UBYTE           bExtDiagFlag;
UBYTE           bDiagLength;
UBYTE           bError;
UWORD           wDiagEvent;

   bRetValue = 0x00;

   wDiagEvent = (UWORD)bAlarmType;

   //don't send diagnostic twice!
   if(    ( wDiagEvent  != pDpSystem->wOldDiag  )
       && ( FALSE == pDpSystem->bUserDiagActive )
     )
   {
      memset( &pDpSystem->abUserDiagnostic[0], 0x00, sizeof( pDpSystem->abUserDiagnostic ) );
      sDiag.pToDiagArray = pDpSystem->abUserDiagnostic;

      switch( bAlarmType )
      {
         case USER_TYPE_CFG_OK:
         {
            bExtDiagFlag = STAT_DIAG_SET;
            bExtDiagFlag = 0x00;
            bDiagLength = 0x00;
            break;
         }//case USER_TYPE_CFG_OK:

         case USER_TYPE_APPL_RDY:
         default:
         {
            bExtDiagFlag = 0x00;
            bDiagLength = 0x00;
            break;
         }//default:
      }//switch( bAlarmType )

      pDpSystem->bUserDiagActive = TRUE;

      bError = VPC3_SetDiagnosis( sDiag, bDiagLength, bExtDiagFlag, bCheckDiagFlag );

      if( bError == DP_OK )
      {
         pDpSystem->wOldDiag = wDiagEvent;

         bRetValue = DP_OK;
      }//if( bError == DP_OK )
      else
      {
         pDpSystem->bUserDiagActive = FALSE;

         #ifdef RS232_SERIO
            print_string("DIAG_DP_Error: ");
            print_hexbyte( bError );
            print_hexbyte( bAlarmType );
         #endif//#ifdef RS232_SERIO

         bRetValue = bError;
      }//else of if( bError == DP_OK )
   }//if(    ( wDiagEvent  != pDpSystem->wOldDiag  ) ...

   return bRetValue;
}//UBYTE UserAlarm( UBYTE bAlarmType, UBYTE bCheckDiagFlag )

/*--------------------------------------------------------------------------*/
/* function: ApplicationReady                                               */
/*--------------------------------------------------------------------------*/
void ApplicationReady( void )
{
   #if DPV1_IM_SUPP
      pDpSystem->awImIndex[ 0x03 ] = 0x0000;
   #endif//#if DPV1_IM_SUPP

   //todo
   //make here your own initialization

   //read input data
   #ifdef EvaBoard_AT89C5132
      sUser.abInput[0] = *READ_PORT1;
      sUser.abInput[1] = *READ_PORT2;
   #else
      sUser.abInput[0] = 0x03;   //demo
      sUser.abInput[1] = 0x04;   //demo
   #endif//#ifdef EvaBoard_AT89C5132

   VPC3_InputDataUpdate( &sUser.abInput[0] );

   //reset Diag.Stat
   if( UserAlarm( USER_TYPE_APPL_RDY, FALSE ) == DP_OK )
   {
      pDpSystem->wEvent &= ~VPC3_EV_NEW_CFG_DATA;    // clear event
      pDpSystem->bApplicationReady = TRUE;
   }//if( UserAlarm( USER_TYPE_APPL_RDY, FALSE ) == DP_OK )
}//void ApplicationReady( void )

/*---------------------------------------------------------------------------*/
/* function: ProfibusInit                                                    */
/*---------------------------------------------------------------------------*/
/*!
  \brief Initializing of PROFIBUS slave communication.
*/
void ProfibusInit( void )
{
DP_ERROR_CODE       bError;

   /*-----------------------------------------------------------------------*/
   /* init user data                                                        */
   /*-----------------------------------------------------------------------*/
   memset( &sUser, 0, sizeof(sUser) );

   /*-----------------------------------------------------------------------*/
   /* initialize VPC3                                                       */
   /*-----------------------------------------------------------------------*/
   #if VPC3_SERIAL_MODE
      Vpc3AsicAddress = (VPC3_ADR)VPC3_ASIC_ADDRESS;
      pVpc3 = &sVpc3OnlyForInit;
      pDpSystem = &sDpSystemChannel1;

      memset( pVpc3, 0, sizeof( VPC3_STRUC ) );
   #else
      pVpc3Channel1 = (VPC3_STRUC_PTR)VPC3_ASIC_ADDRESS;
      Vpc3AsicAddress = (VPC3_ADR)VPC3_ASIC_ADDRESS;
      pVpc3 = pVpc3Channel1;
      pDpSystem = &sDpSystemChannel1;
   #endif//#if VPC3_SERIAL_MODE

   /*-----------------------------------------------------------------------*/
   /* initialize global system structure                                    */
   /*-----------------------------------------------------------------------*/
   memset( pDpSystem, 0, sizeof( VPC3_SYSTEM_STRUC ));

   /* TRUE  deactivates diagnosis handling ! */
   /* FALSE activates   diagnosis handling ! */
   pDpSystem->bUserDiagActive   = FALSE;
   pDpSystem->wOldDiag          = 0x00;
   pDpSystem->bApplicationReady = FALSE;
   pDpSystem->bState            = USER_STATE_CLEAR;

   //todo:
   //insert your real configuration data here
   sUser.sCfgData.bLength = UserCfgDataLength; // length of configuration data
   memcpy( &sUser.sCfgData.abData[0], &DefCfg[0], sUser.sCfgData.bLength );

   ClrResetVPC3Channel1();

   bError = VPC3_MemoryTest();

   if( DP_OK == bError )
   {
      #ifdef EvaBoard_AT89C5132
         bError = VPC3_Initialization( (*READ_PORT0 & 0x7F), IDENT_NR, sUser.sCfgData );     // address of slave; PORT0
      #else
         bError = VPC3_Initialization( DP_ADDR, IDENT_NR, sUser.sCfgData );                  // address of slave
      #endif//#ifdef EvaBoard_AT89C5132

      if( DP_OK == bError )
      {
         EnableInterruptVPC3Channel1();

         //todo: before startup the VPC3+, make here your own initialzations

         VPC3_Start();
      }//if( DP_OK == bError )
      else
      {
         sVpc3Error.bErrorCode = bError;
         FatalError( _DP_USER, __LINE__, &sVpc3Error );
      }//else of if( DP_OK == bError )
   }//if( DP_OK == bError )
   else
   {
      sVpc3Error.bErrorCode = bError;
      FatalError( _DP_USER, __LINE__, &sVpc3Error );
   }//else of if( DP_OK == bError )
}//void ProfibusInit( void )

/*---------------------------------------------------------------------------*/
/* function: ProfibusMain                                                    */
/*---------------------------------------------------------------------------*/
/*!
  \brief The application program has to call this function cyclically so that the PROFIBUS DP slave services can be processed.
*/
void ProfibusMain( void )
{
VPC3_UNSIGNED8_PTR pToOutputBuffer;    // pointer to output buffer
UBYTE              bOutputState;       // state of output data

   /*-------------------------------------------------------------------*/
   /* trigger watchdogs                                                 */
   /*-------------------------------------------------------------------*/
   // toggle user watchdog
   VPC3_RESET_USER_WD();   // toggle user watchdog

   /*-------------------------------------------------------------------*/
   /* internal state machine                                            */
   /*-------------------------------------------------------------------*/
   if( pDpSystem->bState == USER_STATE_CLEAR )
   {
   	// clear data
   	memset( &sUser.abOutput[0] , 0, 2 );
   	memset( &sUser.abInput[0]  , 0, 2 );
   	
   	#ifdef EvaBoard_AT89C5132
   	   *WRITE_PORT1 = 0x00;
   	   *WRITE_PORT2 = 0x00;
   	#endif//#ifdef EvaBoard_AT89C5132
   	
   	pDpSystem->bState = USER_STATE_RUN;
   }//if( pDpSystem->bState == USER_STATE_CLEAR )

   /*-------------------------------------------------------------------*/
   /* VPC3+ DP-state                                                    */
   /*-------------------------------------------------------------------*/
   switch( VPC3_GET_DP_STATE() )
   {
      case WAIT_PRM:
      {
         #ifdef EvaBoard_AT89C5132
            // set LED's
            CLR_LED_YLW__;
            SET_LED_RED__;
         #endif//#ifdef EvaBoard_AT89C5132

         break;
      }//case WAIT_PRM:

      case WAIT_CFG:
      {
         #ifdef EvaBoard_AT89C5132
            // set LED's
            CLR_LED_YLW__;
            SET_LED_RED__;
         #endif//#ifdef EvaBoard_AT89C5132

         break;
      }//case WAIT_CFG:

      case DATA_EX:
      {
         #ifdef EvaBoard_AT89C5132
            // set LED's
            SET_LED_YLW__;
            CLR_LED_RED__;
         #endif//#ifdef EvaBoard_AT89C5132

         if(    ( pDpSystem->bApplicationReady == TRUE )
             && ( pDpSystem->bState == USER_STATE_RUN  )
           )
         {
            /*-------------------------------------------------------------------*/
            /* user application                                                  */
            /*-------------------------------------------------------------------*/
            #ifdef EvaBoard_AT89C5132

               if( sUser.abInput[0] != *READ_PORT1 )
               {
                  sUser.abInput[0] = *READ_PORT1;
                  pDpSystem->wEvent |= VPC3_EV_NEW_INPUT_DATA;
               }//if( sUser.abInput[0] != *READ_PORT1 )

               if( sUser.abInput[1] != *READ_PORT2 )
               {
               	sUser.abInput[1] = *READ_PORT2;
               	pDpSystem->wEvent |= VPC3_EV_NEW_INPUT_DATA;
               }//if( sUser.abInput[1] != *READ_PORT2 )

            #endif//#ifdef EvaBoard_AT89C5132

            /*-------------------------------------------------------------------*/
            /* profibus input                                                    */
            /*-------------------------------------------------------------------*/
            if( pDpSystem->wEvent & VPC3_EV_NEW_INPUT_DATA )
            {
               VPC3_InputDataUpdate( &sUser.abInput[0] );
               pDpSystem->wEvent &= ~VPC3_EV_NEW_INPUT_DATA;
            }//if( pDpSystem->wEvent & VPC3_EV_NEW_INPUT_DATA )
         }//if(    ( pDpSystem->bApplicationReady == TRUE ) ...

         break;
      }//case DATA_EX:

      case DP_ERROR:
      default:
      {
         sVpc3Error.bErrorCode = VPC3_GET_DP_STATE();
         FatalError( _DP_USER, __LINE__, &sVpc3Error );
         break;
      }//case DP_ERROR:
   }//switch( VPC3_GET_DP_STATE() )

   /*-------------------------------------------------------------------*/
   /* profibus output                                                   */
   /*-------------------------------------------------------------------*/
   if( pDpSystem->wEvent & VPC3_EV_DX_OUT )
   {
      pDpSystem->wEvent &= ~VPC3_EV_DX_OUT;       // clear event
      pToOutputBuffer = VPC3_GetDoutBufPtr( &bOutputState );
      if( pToOutputBuffer != NULL_PTR )
      {
         //todo:
         //handle here data (master --> slave)

         CopyFromVpc3_( &sUser.abOutput[0], pToOutputBuffer, (UWORD)pDpSystem->bOutputDataLength );

         /*-----------------------------------------------------------*/
         /* user application                                          */
         /*-----------------------------------------------------------*/
         #ifdef EvaBoard_AT89C5132
            *WRITE_PORT1 = sUser.abOutput[0];
            *WRITE_PORT2 = sUser.abOutput[1];
         #endif//#ifdef EvaBoard_AT89C5132
      }//if( pToOutputBuffer != NULL_PTR )
   }//if( pDpSystem->wEvent & VPC3_EV_DX_OUT )

   /*-------------------------------------------------------------------*/
   /* handle here profibus interrupt events                             */
   /*-------------------------------------------------------------------*/
   if( pDpSystem->wEvent & VPC3_EV_NEW_CFG_DATA )
   {
      ApplicationReady();
   }//if( pDpSystem->wEvent & VPC3_EV_NEW_CFG_DATA )
}//void ProfibusMain( void )

/*---------------------------------------------------------------------------*/
/* function: FatalError                                                     */
/*---------------------------------------------------------------------------*/
void FatalError( DP_ERROR_FILE bFile, UWORD wLine, VPC3_ERRCB_PTR sVpc3Error )
{
   DP_WriteDebugBuffer__( FatalError__, sVpc3Error->bFunction, sVpc3Error->bErrorCode );

   while(1)
   {
   }//while(1)
}//void FatalError( DP_ERROR_FILE bFile, UWORD wLine, VPC3_ERRCB_PTR sVpc3Error )

/*---------------------------------------------------------------------------*/
/* function: UserChkNewPrmData                                               */
/*---------------------------------------------------------------------------*/
/*!
   \brief Checking parameter data.
   The user has to program the function for checking the received parameter data.

   \param pPrmData - address of parameter data
   \param bPrmLength - length of parameter data

   \retval DP_OK - The transferred parameterization is OK.
   \retval DP_NOK - The transferred parameterization isn't OK.
*/
DP_ERROR_CODE UserChkNewPrmData( VPC3_UNSIGNED8_PTR pPrmData, UBYTE bPrmLength )
{
VPC3_STRUC_PRM_PTR   psToPrmData;
DP_ERROR_CODE        bRetValue;

   pDpSystem->bDPV1 = 0x00;

   bRetValue = DP_OK;

   if( bPrmLength == PRM_LEN_DPV1 )
   {
      #if VPC3_SERIAL_MODE
         //copy prm-data from VPC3+S to local structure
         CopyFromVpc3_( (MEM_UNSIGNED8_PTR)&sUser.abPrmData[0], pPrmData, bPrmLength );
         psToPrmData = ( VPC3_STRUC_PRM_PTR )&sUser.abPrmData[0];
      #else
         psToPrmData = ( VPC3_STRUC_PRM_PTR )pPrmData;
      #endif//#if VPC3_SERIAL_MODE

      if(    (( psToPrmData->bDpv1Status1 & 0xF8 ) != 0x00 )
          || (( psToPrmData->bDpv1Status2 & 0xFF ) != 0x00 )
          || (( psToPrmData->bDpv1Status3 & 0xFF ) != 0x00 )
        )
      {
         bRetValue = DP_PRM_DPV1_STATUS;
      }//if(    (( psToPrmData->bDpv1Status1 & 0xF8 ) != 0x00 ) ...
   }//if( bPrmLength == PRM_LEN_DPV1 )
   else
   {
      bRetValue = DP_PRM_LEN_ERROR;
   }//else of if( bPrmLength == PRM_LEN_DPV1 )

   return bRetValue;
}//DP_ERROR_CODE UserChkNewPrmData( VPC3_UNSIGNED8_PTR pPrmData, UBYTE bPrmLength )

/*---------------------------------------------------------------------------*/
/* function: UserCalculateInpOutpLength                                     */
/*---------------------------------------------------------------------------*/
DP_ERROR_CODE UserCalculateInpOutpLength( VPC3_UNSIGNED8_PTR pToCfgData, UBYTE bCfgLength )
{
   #if VPC3_SERIAL_MODE
      pToCfgData = pToCfgData;   //avoid warning
      //it's not necessary here to copy again, copy done in UserChkNewCfgData
      //CopyFromVpc3_( (MEM_UNSIGNED8_PTR)&sUser.abCfgData[0], pCfgData, bCfgLength );
      return VPC3_CalculateInpOutpLength( (MEM_UNSIGNED8_PTR)&sUser.abCfgData[0], bCfgLength );
   #else
      return VPC3_CalculateInpOutpLength( pToCfgData, bCfgLength );
   #endif//#if VPC3_SERIAL_MODE
}//DP_ERROR_CODE UserCalculateInpOutpLength( VPC3_UNSIGNED8_PTR pToCfgData, UBYTE bCfgLength )

/*---------------------------------------------------------------------------*/
/* function: UserChkNewCfgData                                               */
/*---------------------------------------------------------------------------*/
/*!
   \brief Checking configuration data.
   The function VPC3_Isr() or VPC3_Poll() calls this function if the VPC3+
   has received a Check_Cfg message and has made the data available in the Cfg buffer.

   The user has to program the function for checking the received configuration data.

   \param[in] pCfgData - address of check configuration data
   \param[in] bCfgLength - length of configuration data

   \retval VPC3_CFG_OK - The transferred configuration is OK.
   \retval VPC3_CFG_FAULT - The transferred configuration isn't OK.
   \retval VPC3_CFG_UPDATE - The transferred configuration is OK, but it's different
                             from read configuration buffer. The user will exchange
                             verified configuration with read configuration buffer.
*/
UBYTE UserChkNewCfgData( VPC3_UNSIGNED8_PTR pCfgData, UBYTE bCfgLength )
{
#if VPC3_SERIAL_MODE
   MEM_UNSIGNED8_PTR    pToCfgData;
#else
   VPC3_UNSIGNED8_PTR   pToCfgData;
#endif//#if VPC3_SERIAL_MODE
UBYTE                   bRetValue;
UBYTE                   i;

   bRetValue = VPC3_CFG_OK;

   #if VPC3_SERIAL_MODE
      //copy cfg-data from VPC3+S to local structure
      CopyFromVpc3_( (MEM_UNSIGNED8_PTR)&sUser.abCfgData[0], pCfgData, bCfgLength );
      pToCfgData = &sUser.abCfgData[0];
   #else
      pToCfgData = pCfgData;
   #endif//#if VPC3_SERIAL_MODE

   if( bCfgLength == sUser.sCfgData.bLength )
   {
      for( i = 0; i < bCfgLength; i++ )
      {
         if( sUser.sCfgData.abData[ i ] != *pToCfgData )
         {
            bRetValue = VPC3_CFG_FAULT;
         }//if( sUser.sCfgData.abData[ i ] != *pToCfgData )

         pToCfgData++;
      }//for( i = 0; i < bCfgLength; i++ )
   }//if( bCfgLength != bRealCfgLength )
   else
   {
      bRetValue = VPC3_CFG_FAULT;
   }//else of if( bCfgLength != bRealCfgLength )

   if( ( bRetValue == VPC3_CFG_OK ) || ( bRetValue == VPC3_CFG_UPDATE ) )
   {
      UserAlarm( USER_TYPE_CFG_OK, FALSE );
      pDpSystem->wEvent |= VPC3_EV_NEW_CFG_DATA;
   }//if( ( bRetValue == VPC3_CFG_OK ) || ( bRetValue == VPC3_CFG_UPDATE ) )

   return bRetValue;
}//UBYTE UserChkNewCfgData( VPC3_UNSIGNED8_PTR pCfgData, UBYTE bCfgLength )

/*---------------------------------------------------------------------------*/
/* function: UserIsrGoLeaveDataExchange                                      */
/*---------------------------------------------------------------------------*/
/*!
   \brief The function VPC3_Isr() or VPC3_Poll() calls this function if the
   DP-Statemachine has entered the DataEx-mode or has exited it.
   With the function VPC3_GET_DP_STATE() you can find out the state of VPC3+.
*/
void UserIsrGoLeaveDataExchange( void )
{
   if( VPC3_GET_DP_STATE() != DATA_EX )
   {
      pDpSystem->bState = USER_STATE_CLEAR;
      pDpSystem->bApplicationReady = FALSE;
   }//if( VPC3_GET_DP_STATE() != DATA_EX )
}//void UserIsrGoLeaveDataExchange( void )

/*---------------------------------------------------------------------------*/
/* function: UserIsrDxOut                                                    */
/*---------------------------------------------------------------------------*/
/*!
   \brief The function VPC3_Isr() or VPC3_Poll() calls this function if the VPC3+
   has received a DataExchange message and has made the new output data
   available in the N-buffer. In the case of Power_On or Leave_Master, the
   VPC3+ clears the content of the buffer, and generates this event also.
*/
void UserIsrDxOut( void )
{
   pDpSystem->wEvent |= VPC3_EV_DX_OUT;
}//void UserIsrDxOut( void )

/*---------------------------------------------------------------------------*/
/* function: UserIsrDiagBufferChanged                                        */
/*---------------------------------------------------------------------------*/
/*!
   \brief The function VPC3_Isr() or VPC3_Poll() calls this function if the VPC3+ has
   exchanged the diagnostic buffers, and made the old buffer available again to the user.
*/
void UserIsrDiagBufferChanged( void )
{
   // diagnosis buffer has been changed
   pDpSystem->bUserDiagActive = FALSE;
   // Fetch new diagnosis buffer
   pDpSystem->pDiagBuffer = VPC3_GetDiagBufPtr();
}//void UserIsrDiagBufferChanged( void )

/*---------------------------------------------------------------------------*/
/* function: UserIsrNewWdDpTimeout                                           */
/*---------------------------------------------------------------------------*/
/*!
   \brief The function VPC3_Isr() or VPC3_Poll() calls this function if the
   watchdog timer expired in the WD mode DP_Control.
   The communication between master and slave is time controlled, every time you're
   disconnecting the PROFIBUS master or you're disconnecting the PROFIBUS cable you'll
   get this event.
*/
void UserIsrNewWdDpTimeout( void )
{
    //not used in our application
}//void UserIsrNewWdDpTimeout( void )

/*---------------------------------------------------------------------------*/
/* function: UserIsrClockSynchronisation                                     */
/*---------------------------------------------------------------------------*/
#ifdef DP_TIMESTAMP
   void UserIsrClockSynchronisation( void )
   {
      //not used in our application
   }//void UserIsrClockSynchronisation( void )
#endif//#ifdef DP_TIMESTAMP

/*---------------------------------------------------------------------------*/
/* function: UserIsrBaudrateDetect                                           */
/*---------------------------------------------------------------------------*/
/*!
   \brief The function VPC3_Isr() or VPC3_Poll() calls this function if the VPC3+
   has exited the Baud_Search mode and has found a baudrate.
   With the macro VPC3_GET_BAUDRATE() you can detect the baudrate.
*/
void UserIsrBaudrateDetect( void )
{
   //not used in our application
}//void UserIsrBaudrateDetect( void )

/*---------------------------------------------------------------------------*/
/* function: UserIsrNewGlobalControlCommand                                  */
/*---------------------------------------------------------------------------*/
/*!
   \brief The function VPC3_Isr() or VPC3_Poll() calls this function if the VPC3+
   has received a Global_Control message. The GC_Command_Byte can be read out
   with the macro VPC3_GET_GC_COMMAND().
*/
void UserIsrNewGlobalControlCommand( void )
{
   //not used in our application
}//void UserIsrNewGlobalControlCommand( void )

/*---------------------------------------------------------------------------*/
/* function: UserIsrNewSetSlaveAddress                                       */
/*---------------------------------------------------------------------------*/
/*!
   \brief The function VPC3_Isr() or VPC3_Poll() calls this function if the VPC3+
   has received a Set_Slave_Address message and made the data available in the SSA
   buffer.
*/
void UserIsrNewSetSlaveAddress( void )
{
	//not used in our application
/*
#if VPC3_SERIAL_MODE
	MEM_STRUC_SSA_BLOCK_PTR psSsa;
#else
	VPC3_STRUC_SSA_BLOCK_PTR psSsa;
#endif//	#if VPC3_SERIAL_MODE

	#if VPC3_SERIAL_MODE 
		//copy ssa-data from VPC3+S to local structure
	   CopyFromVpc3_( (MEM_UNSIGNED8_PTR)&sUser.abSsaData[0], VPC3_GET_SSA_BUF_PTR(), 4 );
     	psSsa = (MEM_STRUC_SSA_BLOCK_PTR)&sUser.abSsaData[0];
   #else
     	psSsa = (VPC3_STRUC_SSA_BLOCK_PTR) VPC3_GET_SSA_BUF_PTR();
	#endif//#if VPC3_SERIAL_MODE
   
   //store the new address and the bit bNoAddressChanged for the next startup
   print_string("\r\nNewAddr: ");
   print_hexbyte(psSsa->bTsAddr);
   print_hexbyte(psSsa->bNoAddressChanged);
   print_hexbyte(psSsa->bIdentHigh);
   print_hexbyte(psSsa->bIdentLow);
*/
}//void UserIsrNewSetSlaveAddress( void )


/*****************************************************************************/
/*  Copyright (C) profichip GmbH 2009. Confidential.                         */
/*****************************************************************************/

