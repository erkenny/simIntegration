/******************************************************************************
*
* MODULE
*   Common definitions for DS1103
*
* FILE
*   ds1103.h
*
* RELATED FILES
*
*
* DESCRIPTION
*   Definition of
*   - addresses
*   - msg-error-code
*   - definitions for debugging
*
* AUTHOR(S)
*   R. Kraft
*
* dSPACE GmbH, Technologiepark 25, 33100 Paderborn, Germany
*
* $RCSfile: DS1103.H $ $Revision: 1.2 $
* $ProjectName: e:/ARC/Products/ImplSW/RTLibSW/RTLib1103/Develop/DS1103/DS1103.pj $
******************************************************************************/


#ifndef __DS1103_H__
#define __DS1103_H__

#include <Dstypes.h>

#define DS1103_LOCAL_RAM_START  0x00000000
#define DS1103_GLOBAL_RAM_START 0x08000000
#define DS1103_SLAVE_DSP_DP_RAM_START 0x10400000
#define DS1103_SLAVE_DSP_DP_RAM_SIZE  0x2000
#define DS1103_SLAVE_MC_DP_RAM_START  0x10600000
#define DS1103_SLAVE_MC_DP_RAM_SIZE   0x2000

/******************************************************************************
  constant, macro, and type definitions
******************************************************************************/
#define DS1103_EIEIO asm( " eieio " )
#define DS1103_SYNC  asm( " sync " )

/* definitions for debugging via msg-module */
#define DS1103_DEBUG_INIT 1
#define DS1103_DEBUG_POLL 2

/******************************************************************************
  object declarations
******************************************************************************/

extern volatile UInt16 ds1103_debug;

/* ---- RTLIB1103 DEBUG MACROS ---------------------------------------------- */

#ifdef DEBUG_INIT
  #define DS1103_DEBUG_INIT_FCN()    ds1103_debug |= DS1103_DEBUG_INIT
#else
  #define DS1103_DEBUG_INIT_FCN()
#endif


#ifdef DEBUG_POLL
    #define DS1103_DEBUG_POLL_FCN()  ds1103_debug |= DS1103_DEBUG_POLL
#else
    #define DS1103_DEBUG_POLL_FCN()
#endif


#define DS1103_RTLIB_DEBUG_INIT()   \
{                                   \
  DS1103_DEBUG_INIT_FCN();          \
  DS1103_DEBUG_POLL_FCN();          \
}

/* msg-error-codes */
/* are defined in ds1103_msg.h */

#endif
