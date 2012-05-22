/***********************  Filename: dp_isr.c  ********************************/
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
     \brief Interrupt service routine.
     
*/

/*****************************************************************************/
/* contents:

  - function prototypes
  - data structures
  - internal functions

*/
/*****************************************************************************/
/* include hierarchy */

#include "platform.h"
#include "dp_inc.h"

/*---------------------------------------------------------------------------*/
/* function: VPC3_Isr ( interrupt service routine )                          */
/*---------------------------------------------------------------------------*/
/*!
  \brief Interrupt service routine of VPC3+.
  
  This function has to be chained to the external interrupt handler.
*/
void VPC3_Isr( void )
{
volatile UBYTE bResult;

   #if DP_INTERRUPT_MASK_8BIT == 0

	   CopyFromVpc3_( (MEM_UNSIGNED8_PTR)&pDpSystem->wInterruptEvent, ((UBYTE *)(VPC3_ADR)( Vpc3AsicAddress + 0x02 )), 2 );

		#if BIG_ENDIAN
			Swap16( &pDpSystem->wInterruptEvent );
		#endif//#if BIG_ENDIAN

   #endif//#if DP_INTERRUPT_MASK_8BIT == 0

   /*------------------------------------------------------------------------*/
   /* IND_NEW_PRM_DATA                                                       */
   /*------------------------------------------------------------------------*/
   #if( ISR_ENABLE_VPC3_INT_NEW_PRM_DATA == 1 )
      if( VPC3_GET_IND_NEW_PRM_DATA() )
      {
         VPC3_UNSIGNED8_PTR pPrmData;
         UBYTE              bPrmLength;

         bResult = VPC3_PRM_FINISHED;

         #if DP_INTERRUPT_MASK_8BIT == 0
            VPC3_CON_IND_NEW_PRM_DATA();
         #endif//#if DP_INTERRUPT_MASK_8BIT == 0

         do
         {
            bPrmLength = VPC3_GET_PRM_LEN();
            pPrmData = VPC3_GET_PRM_BUF_PTR();

            if( UserChkNewPrmData( pPrmData, bPrmLength ) == DP_OK )
            {
               #if DP_MSAC_C1
                  MSAC_C1_CheckIndNewPrmData( pPrmData, bPrmLength );
               #endif//#if DP_MSAC_C1

               bResult = VPC3_SET_PRM_DATA_OK();
            }//if( UserChkNewPrmData( VPC3_GET_PRM_BUF_PTR(), VPC3_GET_PRM_LEN() ) == DP_OK )
            else
            {
               bResult = VPC3_SET_PRM_DATA_NOT_OK();
            }//else of if( UserChkNewPrmData( VPC3_GET_PRM_BUF_PTR(), VPC3_GET_PRM_LEN() ) == DP_OK )
         }
         while( bResult == VPC3_PRM_CONFLICT );
       }//if( VPC3_GET_IND_NEW_PRM_DATA() )
   #endif//#if( ISR_ENABLE_VPC3_INT_NEW_PRM_DATA == 1 )

   /*------------------------------------------------------------------------*/
   /* IND_NEW_EXT_PRM_DATA                                                   */
   /*------------------------------------------------------------------------*/
   #if( ISR_ENABLE_VPC3_INT_NEW_EXT_PRM_DATA == 1 )
      if( VPC3_GET_IND_NEW_EXT_PRM_DATA() )
      {
         bResult = VPC3_PRM_FINISHED;

         do
         {
            if( UserChkNewExtPrmData( (VPC3_STRUC_PRM_BLOCK_PTR)VPC3_GET_PRM_BUF_PTR(), VPC3_GET_PRM_LEN() ) == DP_OK )
            {
               bResult = VPC3_SET_PRM_DATA_OK();
            }//if( UserChkNewExtPrmData( (VPC3_STRUC_PRM_BLOCK_PTR)VPC3_GET_PRM_BUF_PTR(), VPC3_GET_PRM_LEN() ) == DP_OK
            else
            {
               bResult = VPC3_SET_PRM_DATA_NOT_OK();
            }//else of if( UserChkNewExtPrmData( (VPC3_STRUC_PRM_BLOCK_PTR)VPC3_GET_PRM_BUF_PTR(), VPC3_GET_PRM_LEN() ) == DP_OK
         }
         while( bResult == VPC3_PRM_CONFLICT );
      }//if( VPC3_GET_IND_NEW_EXT_PRM_DATA() )
   #endif//#if( ISR_ENABLE_VPC3_INT_NEW_EXT_PRM_DATA == 1 )

   /*------------------------------------------------------------------------*/
   /* check config data , application specific!                              */
   /*------------------------------------------------------------------------*/
   #if( ISR_ENABLE_VPC3_INT_NEW_CFG_DATA == 1 )
      if( VPC3_GET_IND_NEW_CFG_DATA() )
      {
         VPC3_UNSIGNED8_PTR pCfgData;
         UBYTE              bCfgLength;

         bResult = VPC3_CFG_FINISHED;

         #if DP_INTERRUPT_MASK_8BIT == 0
            VPC3_CON_IND_NEW_CFG_DATA();
         #endif//#if DP_INTERRUPT_MASK_8BIT == 0

         do
         {
            bCfgLength = VPC3_GET_CFG_LEN();
            pCfgData = VPC3_GET_CFG_BUF_PTR();

            switch( UserChkNewCfgData( pCfgData, bCfgLength ) )
            {
               case VPC3_CFG_OK:
               {
                  #if DP_MSAC_C1
                     MSAC_C1_DoCfgOk();
                  #endif//#if DP_MSAC_C1

                  bResult = VPC3_SET_CFG_DATA_OK();
                  break;
               }//case VPC3_CFG_OK:

               case VPC3_CFG_FAULT:
               {
                  #if DP_MSAC_C1
                     MSAC_C1_DoCfgNotOk();
                  #endif//#if DP_MSAC_C1

                  bResult = VPC3_SET_CFG_DATA_NOT_OK();
                  break;
               }//case VPC3_CFG_FAULT:

               case VPC3_CFG_UPDATE:
               {
                  // Calculate the length of the input and output using the configuration bytes
                  if( DP_OK != VPC3_CalculateInpOutpLength(  pCfgData, bCfgLength ) )
                  {
                     #if DP_MSAC_C1
                        MSAC_C1_DoCfgNotOk();
                     #endif//#if DP_MSAC_C1

                     bResult = VPC3_SET_CFG_DATA_NOT_OK();
                  }//if( DP_OK != VPC3_CalculateInpOutpLength(  pCfgData, bCfgLength ) )
                  else
                  {
                     // set IO-Length
                     VPC3_SetIoDataLength();

                     #if DP_MSAC_C1
                        MSAC_C1_DoCfgOk();
                     #endif//#if DP_MSAC_C1

                     VPC3_SET_READ_CFG_LEN( bCfgLength );
                     VPC3_UPDATE_CFG_BUFFER();

                     bResult = VPC3_SET_CFG_DATA_OK();
                  }//else of if( DP_OK != VPC3_CalculateInpOutpLength(  pCfgData, bCfgLength ) )

                  break;
               }//case VPC3_CFG_UPDATE:

               default:
               {
                  break;
               }//default:
            }//switch( UserChkNewCfgData( pCfgData, bCfgLength ) )
         }
         while( bResult == VPC3_CFG_CONFLICT );
      }//if( VPC3_GET_IND_NEW_CFG_DATA() )
   #endif//#if( ISR_ENABLE_VPC3_INT_NEW_CFG_DATA == 1 )

   /*------------------------------------------------------------------------*/
   /* IND_GO_LEAVE_DATA_EX                                                   */
   /*------------------------------------------------------------------------*/
   #if( ISR_ENABLE_VPC3_INT_GO_LEAVE_DATA_EX == 1 )
      if( VPC3_GET_IND_GO_LEAVE_DATA_EX() )
      {
         #if DP_MSAC_C1
            MSAC_C1_LeaveDx();
         #endif//#if DP_MSAC_C1

         UserIsrGoLeaveDataExchange();

         VPC3_CON_IND_GO_LEAVE_DATA_EX();
      }//if( VPC3_GET_IND_GO_LEAVE_DATA_EX() )
   #endif//#if( ISR_ENABLE_VPC3_INT_GO_LEAVE_DATA_EX == 1 )

   /*------------------------------------------------------------------------*/
   /* IND_DX_OUT                                                             */
   /*------------------------------------------------------------------------*/
   #if( ISR_ENABLE_VPC3_INT_DX_OUT == 1 )
      if( VPC3_GET_IND_DX_OUT() )
      {
         #if DP_MSAC_C1
            MSAC_C1_CheckIndDxOut();
         #endif//#if DP_MSAC_C1

         UserIsrDxOut();

         VPC3_CON_IND_DX_OUT();
      }//if( VPC3_GET_IND_DX_OUT() )
   #endif//#if( ISR_ENABLE_VPC3_INT_DX_OUT == 1 )

   /*------------------------------------------------------------------------*/
   /* IND_DIAG_BUF_CHANGED                                                   */
   /*------------------------------------------------------------------------*/
   #if( ISR_ENABLE_VPC3_INT_DIAG_BUF_CHANGED == 1 )
      if( VPC3_GET_IND_DIAG_BUFFER_CHANGED() )
      {
         UserIsrDiagBufferChanged();

         VPC3_CON_IND_DIAG_BUFFER_CHANGED();
      }//if( VPC3_GET_IND_DIAG_BUFFER_CHANGED() )
   #endif//#if( ISR_ENABLE_VPC3_INT_DIAG_BUF_CHANGED == 1 )

   /*------------------------------------------------------------------------*/
   /* IND_WD_DP_TIMEOUT                                                      */
   /*------------------------------------------------------------------------*/
   #if( ISR_ENABLE_VPC3_INT_WD_DP_TIMEOUT == 1 )
      if( VPC3_GET_IND_WD_DP_MODE_TIMEOUT() )
      {
         UserIsrNewWdDpTimeout();

         VPC3_CON_IND_WD_DP_MODE_TIMEOUT();    // acknowledge
      }//if( VPC3_GET_IND_WD_DP_MODE_TIMEOUT() )
   #endif//#if( ISR_ENABLE_VPC3_INT_WD_DP_MODE_TIMEOUT == 1 )

   /*------------------------------------------------------------------------*/
   /* IND_MAC_RESET                                                          */
   /*------------------------------------------------------------------------*/
   #if( ISR_ENABLE_VPC3_INT_CLOCK_SYNC == 1 )
      if( VPC3_GET_IND_CLOCK_SYNC() )
      {
         #ifdef DP_TIMESTAMP
            UserIsrClockSynchronisation();
         #endif//#ifdef DP_TIMESTAMP

         VPC3_CON_IND_CLOCK_SYNC();
      }//if( VPC3_GET_IND_CLOCK_SYNC() )
   #endif//#if( ISR_ENABLE_VPC3_INT_CLOCK_SYNC == 1 )

   /*------------------------------------------------------------------------*/
   /* IND_BAUDRATE_DETECT                                                    */
   /*------------------------------------------------------------------------*/
   #if( ISR_ENABLE_VPC3_INT_BAUDRATE_DETECT == 1 )
      if( VPC3_GET_IND_BAUDRATE_DETECT() )
      {
         #if DP_MSAC_C2
            MSAC_C2_SetTimeoutIsr();
         #endif//#if DP_MSAC_C2

         UserIsrBaudrateDetect();

         VPC3_CON_IND_BAUDRATE_DETECT();
      }//if( VPC3_GET_IND_BAUDRATE_DETECT() )
   #endif//#if( ISR_ENABLE_VPC3_INT_BAUDRATE_DETECT == 1 )

   /*------------------------------------------------------------------------*/
   /* IND_NEW_GC_COMMAND                                                     */
   /*------------------------------------------------------------------------*/
   #if( ISR_ENABLE_VPC3_INT_NEW_GC_COMMAND == 1 )
      if( VPC3_GET_IND_NEW_GC_COMMAND() )
      {
         UserIsrNewGlobalControlCommand();

         VPC3_CON_IND_NEW_GC_COMMAND();
      }//if( VPC3_GET_IND_NEW_GC_COMMAND() )
   #endif//#if( ISR_ENABLE_VPC3_INT_NEW_GC_COMMAND == 1 )

   /*------------------------------------------------------------------------*/
   /* IND_NEW_SSA_DATA                                                       */
   /*------------------------------------------------------------------------*/
   #if( ISR_ENABLE_VPC3_INT_NEW_SSA_DATA == 1 )
      if( VPC3_GET_IND_NEW_SSA_DATA() )
      {
         UserIsrNewSetSlaveAddress();

         bResult = VPC3_FREE_SSA_BUF(); //release the Set Slave Address buffer

         VPC3_CON_IND_NEW_SSA_DATA();
      }//if( VPC3_GET_IND_NEW_SSA_DATA() )
   #endif//#if( ISR_ENABLE_VPC3_INT_NEW_SSA_DATA == 1 )


   /*------------------------------------------------------------------------*/
   /* IND_USER_TIMER_CLOCK                                                   */
   /*------------------------------------------------------------------------*/
   #if( ISR_ENABLE_VPC3_INT_USER_TIMER_CLOCK == 1 )
      if( VPC3_GET_IND_USER_TIMER_CLOCK() )
      {
         UserIsrTimerClock();

         #if DP_MSAC_C2
            MSAC_C2_TimerTick10msec();
         #endif//#if DP_MSAC_C2

         VPC3_CON_IND_USER_TIMER_CLOCK();
      }//if( VPC3_GET_IND_USER_TIMER_CLOCK() )
   #endif//#if( ISR_ENABLE_VPC3_INT_USER_TIMER_CLOCK == 1 )

   #ifdef DP_SUBSCRIBER
      /*---------------------------------------------------------------------*/
      /* IND_DXB_OUT                                                         */
      /*---------------------------------------------------------------------*/
      #if( ISR_ENABLE_VPC3_INT_DXB_OUT == 1 )
         if( VPC3_GET_IND_DXB_OUT() )
         {
            UserIsrDxbOut();

            VPC3_CON_IND_DXB_OUT();
         }//if( VPC3_GET_IND_DXB_OUT() )
      #endif//#if( ISR_ENABLE_VPC3_INT_DXB_OUT == 1 )

      /*---------------------------------------------------------------------*/
      /* IND_DXB_LINK_ERROR                                                  */
      /*---------------------------------------------------------------------*/
      #if( ISR_ENABLE_VPC3_INT_DXB_LINK_ERROR == 1 )
         if( VPC3_GET_IND_DXB_LINK_ERROR() )
         {
            UserIsrDxbLinkError();

            VPC3_CON_IND_DXB_LINK_ERROR();
         }//if( VPC3_GET_IND_DXB_LINK_ERROR() )
      #endif//#if( ISR_ENABLE_VPC3_INT_DXB_LINK_ERROR == 1 )
   #endif//#ifdef DP_SUBSCRIBER

   #if DP_FDL
      /*---------------------------------------------------------------------*/
      /* IND_POLL_END                                                        */
      /*---------------------------------------------------------------------*/
      #if( ISR_ENABLE_VPC3_INT_POLL_END_IND == 1 )
         if( VPC3_GET_IND_POLL_END_IND() )
         {
            VPC3_CON_IND_POLL_END_IND();
            FDL_PollEndIsr();
         }//if( VPC3_GET_IND_POLL_END_IND() )
      #endif//#if( ISR_ENABLE_VPC3_INT_POLL_END_IND == 1 )

      /*---------------------------------------------------------------------*/
      /* IND_REQ_PDU                                                         */
      /*---------------------------------------------------------------------*/
      #if( ISR_ENABLE_VPC3_INT_FDL_IND == 1 )
         if( VPC3_GET_IND_FDL_IND() )
         {
            //if( !VPC3_GET_IND_POLL_END_IND() )
            //{
               VPC3_CON_IND_FDL_IND();
               FDL_IndicationIsr();
            //}//if( !VPC3_GET_IND_POLL_END_IND() )
         }//if( VPC3_GET_IND_FDL_IND() )
       #endif//#if( ISR_ENABLE_VPC3_INT_FDL_IND == 1 )

   #endif//#if DP_FDL

   #if DP_INTERRUPT_MASK_8BIT == 0

      #if BIG_ENDIAN
			Swap16( &pDpSystem->wInterruptEvent );
		#endif//#if BIG_ENDIAN

		CopyToVpc3_( ((UBYTE *)(VPC3_ADR)( Vpc3AsicAddress + 0x02 )), (MEM_UNSIGNED8_PTR)&pDpSystem->wInterruptEvent, 2 );

   #endif//#if DP_INTERRUPT_MASK_8BIT == 0

   /*------------------------------------------------------------------------*/
   /* reset interrupt line                                                   */
   /*------------------------------------------------------------------------*/
   VPC3_SET_EOI();

}//void VPC3_Isr( void )

#if 0
/*---------------------------------------------------------------------------*/
/* function: VPC3_Poll                                                       */
/*---------------------------------------------------------------------------*/
void VPC3_Poll( void )
{
volatile UBYTE bResult;

   #if DP_INTERRUPT_MASK_8BIT == 0

      CopyFromVpc3_( (MEM_UNSIGNED8_PTR)&pDpSystem->wPollInterruptEvent, ((UBYTE *)(VPC3_ADR)( Vpc3AsicAddress )), 2 );

      #if BIG_ENDIAN
         Swap16( &pDpSystem->wPollInterruptEvent );
      #endif//#if BIG_ENDIAN

      pDpSystem->wPollInterruptEvent &= SM_INTERRUPT_MASK;

      if( pDpSystem->wPollInterruptEvent > 0 )
      {

   #endif//#if DP_INTERRUPT_MASK_8BIT == 0

         /*------------------------------------------------------------------*/
         /* IND_NEW_PRM_DATA                                                 */
         /*------------------------------------------------------------------*/
         if( VPC3_POLL_IND_NEW_PRM_DATA() )
         {
            VPC3_UNSIGNED8_PTR pPrmData;
            UBYTE              bPrmLength;

            #if DP_INTERRUPT_MASK_8BIT == 0
               VPC3_CON_IND_NEW_PRM_DATA();
            #endif//#if DP_INTERRUPT_MASK_8BIT == 0

            bResult = VPC3_PRM_FINISHED;

            do
            {
               bPrmLength = VPC3_GET_PRM_LEN();
               pPrmData = VPC3_GET_PRM_BUF_PTR();

               if( UserChkNewPrmData( pPrmData, bPrmLength ) == DP_OK )
               {
                  #if DP_MSAC_C1
                     MSAC_C1_CheckIndNewPrmData( pPrmData, bPrmLength );
                  #endif

                  bResult = VPC3_SET_PRM_DATA_OK();
               }//if( UserChkNewPrmData( pPrmData, bPrmLength ) == DP_OK )
               else
               {
                  bResult = VPC3_SET_PRM_DATA_NOT_OK();
               }//else of if( UserChkNewPrmData( pPrmData, bPrmLength ) == DP_OK )
            }
            while( bResult == VPC3_PRM_CONFLICT );
         }//if( VPC3_POLL_IND_NEW_PRM_DATA() )

         /*------------------------------------------------------------------*/
         /* check config data , application specific!                        */
         /*------------------------------------------------------------------*/
         if( VPC3_POLL_IND_NEW_CFG_DATA() )
         {
            VPC3_UNSIGNED8_PTR pCfgData;
            UBYTE              bCfgLength;

            #if DP_INTERRUPT_MASK_8BIT == 0
               VPC3_CON_IND_NEW_CFG_DATA();
            #endif//#if DP_INTERRUPT_MASK_8BIT == 0

            bResult = VPC3_CFG_FINISHED;

            do
            {
               bCfgLength = VPC3_GET_CFG_LEN();
               pCfgData = VPC3_GET_CFG_BUF_PTR();

               switch( UserChkNewCfgData( pCfgData, bCfgLength ) )
               {
                  case VPC3_CFG_OK:
                  {
                     #if DP_MSAC_C1
                        MSAC_C1_DoCfgOk();
                     #endif

                     bResult = VPC3_SET_CFG_DATA_OK();
                     break;
                  }//case VPC3_CFG_OK:

                  case VPC3_CFG_FAULT:
                  {
                     #if DP_MSAC_C1
                        MSAC_C1_DoCfgNotOk();
                     #endif

                     bResult = VPC3_SET_CFG_DATA_NOT_OK();
                     break;
                  }//case VPC3_CFG_FAULT:

                  case VPC3_CFG_UPDATE:
                  {
                     // Calculate the length of the input and output using the configuration bytes
                     if( DP_OK != UserCalculateInpOutpLength( pCfgData, bCfgLength ) )
                     {
                        #if DP_MSAC_C1
                           MSAC_C1_DoCfgNotOk();
                        #endif

                        bResult = VPC3_SET_CFG_DATA_NOT_OK();
                     }//if( DP_OK != UserCalculateInpOutpLength( pCfgData, bCfgLength ) )
                     else
                     {
                        // set IO-Length
                        VPC3_SetIoDataLength();

                        #if DP_MSAC_C1
                           MSAC_C1_DoCfgOk();
                        #endif

                        VPC3_SET_READ_CFG_LEN( bCfgLength );
                        VPC3_UPDATE_CFG_BUFFER();

                        bResult = VPC3_SET_CFG_DATA_OK();
                     }//else of if( DP_OK != UserCalculateInpOutpLength( pCfgData, bCfgLength ) )
                     break;
                  }//case VPC3_CFG_UPDATE:

                  default:
                  {
                     break;
                  }//default:
               }//switch( UserChkNewCfgData( pCfgData, bCfgLength ) )
            }
            while( bResult == VPC3_CFG_CONFLICT );
         }//if( VPC3_POLL_IND_NEW_CFG_DATA() )

         /*------------------------------------------------------------------*/
         /* IND_GO_LEAVE_DATA_EX                                             */
         /*------------------------------------------------------------------*/
         if( VPC3_POLL_IND_GO_LEAVE_DATA_EX() )
         {
            #if DP_MSAC_C1
               MSAC_C1_LeaveDx();
            #endif

            UserIsrGoLeaveDataExchange();

            VPC3_CON_IND_GO_LEAVE_DATA_EX();
         }//if( VPC3_POLL_IND_GO_LEAVE_DATA_EX() )

         /*------------------------------------------------------------------*/
         /* IND_DX_OUT                                                       */
         /*------------------------------------------------------------------*/
         if( VPC3_POLL_IND_DX_OUT() )
         {
            #if DP_MSAC_C1
               MSAC_C1_CheckIndDxOut();
            #endif

            UserIsrDxOut();

            VPC3_CON_IND_DX_OUT();
         }//if( VPC3_POLL_IND_DX_OUT() )

         /*------------------------------------------------------------------*/
         /* IND_DIAG_BUF_CHANGED                                             */
         /*------------------------------------------------------------------*/
         if( VPC3_POLL_IND_DIAG_BUFFER_CHANGED() )
         {
            UserIsrDiagBufferChanged();

            VPC3_CON_IND_DIAG_BUFFER_CHANGED();
         }//if( VPC3_POLL_IND_DIAG_BUFFER_CHANGED() )

         /*------------------------------------------------------------------*/
         /* IND_NEW_GC_COMMAND                                               */
         /*------------------------------------------------------------------*/
         if( VPC3_POLL_IND_NEW_GC_COMMAND() )
         {
            UserIsrNewGlobalControlCommand();

            VPC3_CON_IND_NEW_GC_COMMAND();
         }//if( VPC3_POLL_IND_NEW_GC_COMMAND() )

         /*------------------------------------------------------------------*/
         /* IND_USER_TIMER_CLOCK                                             */
         /*------------------------------------------------------------------*/
         if( VPC3_POLL_IND_USER_TIMER_CLOCK() )
         {
            UserIsrTimerClock();

            #if DP_MSAC_C2
               MSAC_C2_TimerTick10msec();
            #endif//#if DP_MSAC_C2

            VPC3_CON_IND_USER_TIMER_CLOCK();
         }//if( VPC3_POLL_IND_USER_TIMER_CLOCK() )

         /*------------------------------------------------------------------*/
         /* IND_NEW_SSA_DATA                                                 */
         /*------------------------------------------------------------------*/
         if( VPC3_POLL_IND_NEW_SSA_DATA() )
         {
            UserIsrNewSetSlaveAddress();
         
            bResult = VPC3_FREE_SSA_BUF(); //release the Set Slave Address buffer
         
            VPC3_CON_IND_NEW_SSA_DATA();
         }//if( VPC3_POLL_IND_NEW_SSA_DATA() )

         /*------------------------------------------------------------------*/
         /* IND_CLOCK_SYNC                                                   */
         /*------------------------------------------------------------------*/
         #ifdef DP_TIMESTAMP
            if( VPC3_POLL_IND_CLOCK_SYNC() )
            {
               UserIsrClockSynchronisation();

               VPC3_CON_IND_CLOCK_SYNC();
            }//if( VPC3_POLL_IND_CLOCK_SYNC() )
         #endif//#ifdef DP_TIMESTAMP

         #ifdef DP_SUBSCRIBER
            /*---------------------------------------------------------------*/
            /* IND_DXB_OUT                                                   */
            /*---------------------------------------------------------------*/
            if( VPC3_POLL_IND_DXB_OUT() )
            {
               UserIsrDxbOut();

               VPC3_CON_IND_DXB_OUT();
            }//if( VPC3_POLL_IND_DXB_OUT() )

            /*---------------------------------------------------------------*/
            /* IND_DXB_LINK_ERROR                                            */
            /*---------------------------------------------------------------*/
            if( VPC3_POLL_IND_DXB_LINK_ERROR() )
            {
               UserIsrDxbLinkError();

               VPC3_CON_IND_DXB_LINK_ERROR();
            }//if( VPC3_POLL_IND_DXB_LINK_ERROR() )
         #endif//#ifdef DP_SUBSCRIBER

         #if DP_FDL
            /*---------------------------------------------------------------*/
            /* IND_REQ_PDU                                                   */
            /*---------------------------------------------------------------*/
            if( VPC3_POLL_IND_FDL_IND() )
            {
               if( !VPC3_POLL_IND_POLL_END_IND() )
               {
                  VPC3_CON_IND_FDL_IND();
                  FDL_IndicationIsr();
               }//if( !VPC3_POLL_IND_POLL_END_IND() )
            }//if( VPC3_POLL_IND_FDL_IND() )

            /*---------------------------------------------------------------*/
            /* IND_POLL_END                                                  */
            /*---------------------------------------------------------------*/
            if( VPC3_POLL_IND_POLL_END_IND() )
            {
               VPC3_CON_IND_POLL_END_IND();

               FDL_PollEndIsr();
            }//if( VPC3_POLL_IND_POLL_END_IND() )
         #endif//#if DP_FDL

         #if DP_INTERRUPT_MASK_8BIT == 0

            #if BIG_ENDIAN
               Swap16( &pDpSystem->wPollInterruptEvent );
            #endif//#if BIG_ENDIAN

            CopyToVpc3_( ((UBYTE *)(VPC3_ADR)( Vpc3AsicAddress + 0x02 )), (MEM_UNSIGNED8_PTR)&pDpSystem->wPollInterruptEvent, 2 );

         #endif//#if DP_INTERRUPT_MASK_8BIT == 0

   #if DP_INTERRUPT_MASK_8BIT == 0

      }//if( pDpSystem->wPollInterruptEvent > 0 )

   #endif//#if DP_INTERRUPT_MASK_8BIT == 0
}//void VPC3_Poll( void )
#endif //#if 0

/*****************************************************************************/
/*  Copyright (C) profichip GmbH 2009. Confidential.                         */
/*****************************************************************************/

