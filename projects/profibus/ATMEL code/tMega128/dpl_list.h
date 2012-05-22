/************************** Filename: dp_list.h ******************************/
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
     \brief Headerfile for double pointered list.
     
*/

/*****************************************************************************/
/* contents:

   dpl_init_list__                  ( dplListPtr)

   dpl_list_empty__                 ( dplListPtr)
   dpl_list_not_empty__             ( dplListPtr)

   dpl_put_blk_to_list_start__      ( dplListPtr, dplBlkPtr )
   dpl_put_blk_to_list_end__        ( dplListPtr, dplBlkPtr )
   dpl_put_blk_queue_to_list_end__  ( dplListPtr, dplBlkStartPtr, dplBlkEndPtr )

   dpl_get_blk_from_list__          ( dplListPtr, dplAdrBlkPtr )
   dpl_remove_blk_from_list__       ( dplListPtr)
   dpl_get_remove_blk_from_list__   ( dplListPtr, dplAdrBlkPtr )
   dpl_remove_blk__                 ( dplBlkPtr

*/
/*****************************************************************************/
/* reinclude protection */


#ifndef DPLLIST_H
#define DPLLIST_H


/*****************************************************************************/
/*  CHECK PARAMETER                                                          */
/*****************************************************************************/
#define DPL_PTR_ATTR MEM_PTR_ATTR
#define DPL_HOST_PTR_COMPARE_TYPE   void *

#ifdef  DPL_PTR_ATTR


/*****************************************************************************/
/*  ANCHOR                                                                   */
/*****************************************************************************/

typedef struct dpl_list_cb
{
   struct dpl_list_cb DPL_PTR_ATTR * NextBlkPtr;
   struct dpl_list_cb DPL_PTR_ATTR * PrevBlkPtr;

}  _PACKED_ DPL_STRUC_LIST_CB;

#define DPL_STRUC_LIST_CB_PTR            DPL_STRUC_LIST_CB DPL_PTR_ATTR *

/*****************************************************************************/
/*  CASTS                                                                    */
/*****************************************************************************/


#define DPL_LIST_CB_PTR                                                       \
        DPL_STRUC_LIST_CB DPL_PTR_ATTR *

#define DPL_LIST_CB_PTR_PTR                                                   \
        DPL_STRUC_LIST_CB DPL_PTR_ATTR * DPL_PTR_ATTR *


/*****************************************************************************/
/*  LIST HANDLING                                                            */
/*****************************************************************************/


/*****************************************************************************/
/*                                                                           */
/*                           double pointered list                           */
/*                                                                           */
/*         +-------------------------------------------------------+         */
/*         |                                                       |         */
/*         |   list-anchor                                         |         */
/*         |    +------+     +------+     +------+     +------+    |         */
/*         +--> | next | --> | next | --> | next | ... | next | ---+         */
/*         +--- | prev | <-- | prev | <-- | prev | ... | prev | <--+         */
/*         |    +------+     | ...  |     | ...  |     | ...  |    |         */
/*         |                 +------+     +------+     +------+    |         */
/*         |                                                       |         */
/*         +-------------------------------------------------------+         */
/*                                                                           */
/*****************************************************************************/


/*****************************************************************************/
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  initialize desired double pointered list.                                */
/*                                                                           */
/*****************************************************************************/


#define dpl_init_list__( dplListPtr )                                         \
{                                                                             \
   /* list.next = list.prev = list */                                         \
   ((DPL_LIST_CB_PTR) (dplListPtr)) -> NextBlkPtr                             \
   = ((DPL_LIST_CB_PTR) (dplListPtr)) -> PrevBlkPtr                           \
   = (DPL_LIST_CB_PTR) (dplListPtr);                                          \
}                                                                             \


/*****************************************************************************/
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  check if desired double pointered list is empty.                         */
/*                                                                           */
/*****************************************************************************/


#define dpl_list_empty__( dplListPtr )                                        \
(                                                                             \
   /* list.next == list */                                                    \
   ((DPL_HOST_PTR_COMPARE_TYPE)                                               \
   (((DPL_LIST_CB_PTR) (dplListPtr)) -> NextBlkPtr))                          \
   ==                                                                         \
   ((DPL_HOST_PTR_COMPARE_TYPE)                                               \
   (dplListPtr))                                                              \
)


/*****************************************************************************/
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  check if desired double pointered list is not empty.                     */
/*                                                                           */
/*****************************************************************************/


#define dpl_list_not_empty__( dplListPtr )                                    \
(                                                                             \
   /* list.next != list */                                                    \
   ((DPL_HOST_PTR_COMPARE_TYPE)                                               \
   (((DPL_LIST_CB_PTR) (dplListPtr)) -> NextBlkPtr))                          \
   !=                                                                         \
   ((DPL_HOST_PTR_COMPARE_TYPE)                                               \
   (dplListPtr))                                                              \
)


/*****************************************************************************/
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  put block to start of double pointered list.                             */
/*                                                                           */
/*****************************************************************************/


#define dpl_put_blk_to_list_start__( dplListPtr, dplBlkPtr )                  \
{                                                                             \
   /* list.next -> prev = blk */                                              \
   ((DPL_LIST_CB_PTR) (dplListPtr)) -> NextBlkPtr -> PrevBlkPtr               \
   = (DPL_LIST_CB_PTR) (dplBlkPtr);                                           \
                                                                              \
   /* blk.next = list.next */                                                 \
   ((DPL_LIST_CB_PTR) (dplBlkPtr)) -> NextBlkPtr                              \
   = ((DPL_LIST_CB_PTR) (dplListPtr)) -> NextBlkPtr;                          \
                                                                              \
   /* list.next = blk */                                                      \
   ((DPL_LIST_CB_PTR) (dplListPtr)) -> NextBlkPtr                             \
   = (DPL_LIST_CB_PTR) (dplBlkPtr);                                           \
                                                                              \
   /* blk.prev = list */                                                      \
   ((DPL_LIST_CB_PTR) (dplBlkPtr)) -> PrevBlkPtr                              \
   = (DPL_LIST_CB_PTR) (dplListPtr);                                          \
}                                                                             \


/*****************************************************************************/
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  put block to end of double pointered list.                               */
/*                                                                           */
/*****************************************************************************/


#define dpl_put_blk_to_list_end__( dplListPtr, dplBlkPtr )                    \
{                                                                             \
   /* blk.next = list */                                                      \
   ((DPL_LIST_CB_PTR) (dplBlkPtr)) -> NextBlkPtr                              \
   = (DPL_LIST_CB_PTR) (dplListPtr);                                          \
                                                                              \
   /* list.prev -> next = blk */                                              \
   ((DPL_LIST_CB_PTR) (dplListPtr)) -> PrevBlkPtr -> NextBlkPtr               \
   = (DPL_LIST_CB_PTR) (dplBlkPtr);                                           \
                                                                              \
   /* blk.prev = list.prev */                                                 \
   ((DPL_LIST_CB_PTR) (dplBlkPtr)) -> PrevBlkPtr                              \
   = ((DPL_LIST_CB_PTR) (dplListPtr)) -> PrevBlkPtr;                          \
                                                                              \
   /* list.prev = blk  */                                                     \
   ((DPL_LIST_CB_PTR) (dplListPtr)) -> PrevBlkPtr                             \
   = (DPL_LIST_CB_PTR) (dplBlkPtr);                                           \
}                                                                             \


/*****************************************************************************/
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  put queue of blocks to end of double pointered list.                     */
/*                                                                           */
/*****************************************************************************/


#define dpl_put_blk_queue_to_list_end__( dplListPtr, dplBlkStartPtr, dplBlkEndPtr ) \
{                                                                                   \
   /* list.prev -> next = first blk */                                              \
   ((DPL_LIST_CB_PTR) (dplListPtr)) -> PrevBlkPtr -> NextBlkPtr                     \
   = (DPL_LIST_CB_PTR) (dplBlkStartPtr);                                            \
                                                                                    \
   /* first blk.prev = list.prev */                                                 \
   ((DPL_LIST_CB_PTR) (dplBlkStartPtr)) -> PrevBlkPtr                               \
   = ((DPL_LIST_CB_PTR) (dplListPtr)) -> PrevBlkPtr;                                \
                                                                                    \
   /* list.prev = last blk */                                                       \
   ((DPL_LIST_CB_PTR) (dplListPtr)) -> PrevBlkPtr                                   \
   = (DPL_LIST_CB_PTR) (dplBlkEndPtr);                                              \
                                                                                    \
   /* last blk.next = list */                                                       \
   ((DPL_LIST_CB_PTR) (dplBlkEndPtr)) -> NextBlkPtr                                 \
   = (DPL_LIST_CB_PTR) (dplListPtr);                                                \
}                                                                                   \


/*****************************************************************************/
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  get block from start of desired double pointered list.                   */
/*                                                                           */
/*****************************************************************************/


#define dpl_get_blk_from_list__( dplListPtr, dplAdrBlkPtr )                   \
{                                                                             \
   /* blk = list.next */                                                      \
   *((DPL_LIST_CB_PTR_PTR) (dplAdrBlkPtr))                                    \
   = ((DPL_LIST_CB_PTR) (dplListPtr)) -> NextBlkPtr;                          \
}                                                                             \


/*****************************************************************************/
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  remove block from start of desired double pointered list.                */
/*                                                                           */
/*****************************************************************************/


#define dpl_remove_blk_from_list__( dplListPtr )                              \
{                                                                             \
   /* list.next = list.next -> next */                                        \
   ((DPL_LIST_CB_PTR) (dplListPtr)) -> NextBlkPtr                             \
   = ((DPL_LIST_CB_PTR) (dplListPtr)) -> NextBlkPtr -> NextBlkPtr;            \
                                                                              \
   /* list.next -> prev = list */                                             \
   ((DPL_LIST_CB_PTR) (dplListPtr)) -> NextBlkPtr -> PrevBlkPtr               \
   = (DPL_LIST_CB_PTR) (dplListPtr);                                          \
}                                                                             \


/*****************************************************************************/
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  get and remove block from start of desired double pointered list.        */
/*                                                                           */
/*****************************************************************************/


#define dpl_get_remove_blk_from_list__(dplListPtr, dplAdrBlkPtr)              \
{                                                                             \
   dpl_get_blk_from_list__((dplListPtr), (dplAdrBlkPtr));                     \
                                                                              \
   dpl_remove_blk_from_list__(dplListPtr);                                    \
}                                                                             \


/*****************************************************************************/
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  remove block from anywhere inside a double pointered list.               */
/*                                                                           */
/*****************************************************************************/


#define dpl_remove_blk__( dplBlkPtr )                                         \
{                                                                             \
   /* blk.prev -> next = blk.next */                                          \
   ((DPL_LIST_CB_PTR) (dplBlkPtr)) -> PrevBlkPtr -> NextBlkPtr                \
   = ((DPL_LIST_CB_PTR) (dplBlkPtr)) -> NextBlkPtr;                           \
                                                                              \
   /* blk.next -> prev = blk.prev */                                          \
   ((DPL_LIST_CB_PTR) (dplBlkPtr)) -> NextBlkPtr -> PrevBlkPtr                \
   = ((DPL_LIST_CB_PTR) (dplBlkPtr)) -> PrevBlkPtr;                           \
}                                                                             \


/*****************************************************************************/
/* parameter check */


#else
   #error dplist handler: no list handling data attribute DPL_PTR_ATTR defined !
#endif


/*****************************************************************************/
/* reinclude-protection */


#else
   #pragma message ("The header DPLLIST.H is included twice or more !")
#endif


/*****************************************************************************/
/*  Copyright (C) profichip GmbH 2009. Confidential.                         */
/*****************************************************************************/

