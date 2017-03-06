/*******************************************************************************
*
* MODULE
*   dsts - Timestamp generation functions
*
* FILE
*   ts1103.h
*
* RELATED FILES
*   dsts.c, dsts.h
*
* DESCRIPTION
*
* REMARKS
*
* AUTHOR(S)
*   T. Woelfer
*
* dSPACE GmbH, Technologiepark 25, 33100 Paderborn, Germany
*
* $RCSfile: ts1103.h $ $Revision: 1.3 $ $Date: 2003/10/21 15:33:44GMT+01:00 $
* $ProjectName: e:/ARC/Products/ImplSW/RTLibSW/RTLib1103/Develop/DS1103/DSTS1103.pj $
*******************************************************************************/

#ifndef __ts1103_h__
#define __ts1103_h__

#include <tic1103.h>
#include <ds1103.h>
#include <int1103.h>


/*******************************************************************************
  constant, macro, and type definitions
*******************************************************************************/

/* ---- constant definitions ------------------------------------------------ */

#define TS_SINGLE_RELOAD_VALUE 4294967296.0

#define TS_MIT_PERIODE_SINGLE  DS1103_TIME_PER_TICK
#define TS_MIT_PERIODE_MULTI   DS1103_TIME_PER_TICK

#define TS_MAT_PERIODE_SINGLE  (((double)TS_SINGLE_RELOAD_VALUE) * dsts_mit_period)
#define TS_MAT_PERIODE_MULTI   (((double)TS_SINGLE_RELOAD_VALUE) * dsts_mit_period)

/* -------------------------------------------------------------------------- */

#define TS_MAT_CONFIGURE_MASTER_SENDER(gl)
#define TS_MAT_CONFIGURE_SLAVE_SENDER(gl)
#define TS_MAT_CONFIGURE_MASTER_RECEIVER(gl)
#define TS_MAT_CONFIGURE_SLAVE_RECEIVER(gl)

#define TS_TIMEBASE_INIT_SINGLE()
#define TS_TIMEBASE_INIT_MULTI_MASTER()
#define TS_TIMEBASE_INIT_MULTI_SLAVE()

/*----------------------------------------------------------------------------*/

#define TS_TIMEBASE_READ(ts)                     \
{                                                \
  asm(".supervisoron"                       "\n" \
      " mfmsr 5"                            "\n" \
      " lis   4,0xFFFF"                     "\n" \
      " ori   4,4,0x7FFF"                   "\n" \
      " and   6,5,4"                        "\n" \
      " mtmsr 6"                            "\n" \
      " L..get_timebase:"                   "\n" \
      " mftbu 4"                            "\n" \
      " stw   4,0x0(3)"                     "\n" \
      " mftb  6"                            "\n" \
      " stw   6,0x4(3)"                     "\n" \
      " mftbu 6"                            "\n" \
      " cmp   0,0x0,4,6"                    "\n" \
      " bc    0x4,0x2,L..get_timebase"      "\n" \
      " mtmsr 5"                            "\n" \
      " isync"          );                       \
}

#define TS_TIMEBASE_READ_FAST(ts)                \
{                                                \
  asm(".supervisoron "                      "\n" \
      " L..get_timebase_fast:"              "\n" \
      " mftbu 4"                            "\n" \
      " stw   4,0x0(3)"                     "\n" \
      " mftb  6"                            "\n" \
      " stw   6,0x4(3)"                     "\n" \
      " mftbu 6"                            "\n" \
      " cmp   0,0x0,4,6"                    "\n" \
      " bc    0x4,0x2,L..get_timebase_fast"   ); \
}

/*----------------------------------------------------------------------------*/

#define TS_TIMEBASE_RESET()   \
{                             \
  asm(".supervisoron"    "\n" \
      " lis   3, 0"      "\n" \
      " mttb  3"         "\n" \
      " mttbu 3"         "\n" \
      " eieio"  );            \
}

#define TS_TIMEBASE_RESET_LOW() \
{                               \
  asm(".supervisoron"    "\n"   \
      " lis   3, 0"      "\n"   \
      " mttb  3"         "\n"   \
      " eieio"  );              \
}


#endif /* __ts1103_h__ */

