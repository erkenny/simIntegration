#ifndef __DSP_CMD_INTERFACE__
#define __DSP_CMD_INTERFACE__

/* DSCMD.H **************************************************************
*                                                                       *
* Copyright 1996 dSPACE GmbH, Germany                                   *
*                                                                       *
* Author   : GS                                                         *
* Modified : GS  17.07.1996  initial version                            *
*            GS  24.10.1997  DS1103 support                             *
*            GS  10.07.1998  direct DS1004 DP-MEM communic.for CtrlDesk *
*                                                                       *
* $RCSfile: DSCMD.H $ $Revision: 1.4 $ $Date: 2009/02/03 17:41:44GMT+01:00 $          * 
*************************************************************************/

#include "dstypes.h"

#define DSSERV_ARRAY_PENDING  0
#define DSSERV_ARRAY_BUSY     1
#define DSSERV_ARRAY_COMPLETE 2

/* error codes of some command server functions */

#define DSSERV_ERR_INVALID_TASK_NO      -1
#define DSSERV_ERR_INSTANCE_LIST_FULL   -2
#define DSSERV_ERR_TABLE_MALLOC         -3
#define DSSERV_ERR_SERVICE_NOT_CALLED   -4

#define DSSERV_ERR_INVALID_PARAMETER    -1
#define DSSERV_ERR_INVALID_ENTRY_HANDLE -2



typedef struct _mp_admin_type {
        UInt32 buffer_offset;       /* offset (in 32-Bit-Words) to data buffer */
        UInt32 buffer_size;         /* size   (in 32-Bit-Words) of data buffer */
        Int32  owner;               /* current owner of buffer (0=nobody)      */
        UInt32 state;               /* 0=pending / 1=busy / 2=complete         */

        UInt32 entries;             /* number of bytes in data buffer          */
        UInt32 access;              /* control read=0/write=1 access for slave */
        union {
            struct {
                UInt32 address_lo;
                UInt32 address_hi;
            } s;
            void * address;
        } a;

        UInt32 offset;
        UInt32 next_owner;
        UInt32 unused1;
        UInt32 unused2;
} mp_admin_info;

#define ADMIN_INFO_SIZE 12  /* 32-Bit words */

#define SERVICE_CMD_CHANNEL_SIZE    6


typedef enum _DsServError
{
  DSSERV_NO_ERROR = 0,
  DSSERV_NO_MEMORY,
  DSSERV_ILLEGAL_CPU_NO
} DsServError;

typedef struct _CmdRec
{
        Int32  Command;                /* Command flag                    */
        Int32  DestProcId;             /* Destination processor (0=local) */
        Int32  Adr;                    /* Address to read/write           */
        Int32  Ofs;                    /* Offset (optional for Ptr-Cmds)  */
        Int32  OutpPos;                /* Date: 32 bit commands separate  */
        Int32  Val;                    /* Date: 32 bit commands separate  */
} DsCmdRec;


#ifndef DSCMD_QUEUE_SIZE
#define DSCMD_QUEUE_SIZE 64
#endif

typedef struct _DsCmdQueue {
        Int32    Length;
        Int32    Read;
        Int32    Write;
        DsCmdRec Cmd[ DSCMD_QUEUE_SIZE ];
} DsCmdQueue;



typedef enum _DsCmdEnum
{
        DSCMD_NOP     = 0,
        DSCMD_DELAY,
        DSCMD_ENDSEQ,
        DSCMD_ADDADR,
        DSCMD_ADDPTR,
        DSCMD_REMOVEADR,
        DSCMD_REMOVEPTR,
        DSCMD_WRITEADR,
        DSCMD_WRITEPTR,
        DSCMD_WRITEADR64,
        DSCMD_WRITEPTR64,
        DSCMD_ADDFLTADR64TO32,
        DSCMD_ADDFLTPTR64TO32,
        DSCMD_REMOVEFLTADR64TO32,
        DSCMD_REMOVEFLTPTR64TO32,
        DSCMD_COPYARRAY,
        DSCMD_COPYARRAYIND,
        DSCMD_ADDSVCTABLE,
        DSCMD_REMOVESVCTABLE,
        DSCMD_WRITEADR16,
        DSCMD_WRITEPTR16,
        DSCMD_WRITEADR8,
        DSCMD_WRITEPTR8,
        DSCMD_COPYARRAYDSTIND,
        DSCMD_COPYARRAYMISALG,
        DSCMD_COPYARRAYINDMISALG,
        DSCMD_COPYARRAYDSTINDMISALG,
        DSCMD_COPYDOUBLEARRAYDIRECT,
        DSCMD_COPYDOUBLEARRAYINDIRECT,
        DSCMD_COPYDOUBLEARRAYDSTIND,
        DSCMD_REMOVEALLSVCTABLES,
        DSCMD_TRIGGERACK,
        DSCMD_GETTIMESTAMP,
        DSCMD_GETFRTIMESTRUCT,
        DSCMD_RTLIB_CONTROL,
        DSCMD_PHS_CONTROL
 
} DsCmdEnum;

 
#define DSCMD_FIRST_COMMAND     DSCMD_ADDADR
#define DSCMD_LAST_COMMAND      DSCMD_PHS_CONTROL

/* Prototypes for master_cmd_server */
extern int  master_cmd_server_init( Int32 cpu_count );
extern int  master_cmd_add_svc_entry( Int32 cpu_no, Int32 * cmd_ptr, Int32 * result_ptr, Int32 result_size );
extern int  master_cmd_array_channel( Int32 cpu_no, Int32 * smem_ptr, Int32 smem_size );


/* Prototypes for slave_cmd_server */
extern int  slave_cmd_server_init( Int32 * cmd_ptr, Int32 * result_ptr, Int32 result_size);
extern int  slave_init_array_svc( Int32 * smem_ptr, Int32 smem_size );
extern void slave_array_svc();

/* service table reset function */
extern int master_RemoveAllServiceTables (Int32 task_no);


typedef struct _ServiceTableEntry {
        Int32  CmdBufferLen;           /* Len of CmdBuffer    */
        Int32* CmdBufferPtr;           /* Cmd to send         */
        Int32  ResultBufferLen;        /* Len of ResultBuffer */
        Int32* ResultBufferPtr;        /* Data to receive     */
        Int32  EventBufferLen;         /* unused              */
        Int32* EventBufferPtr;         /* unused              */
        Int32  PosManagementLen;       /* Result-Buffer usage */
        Int32* PosManagementPtr;       /* Result-Buffer usage */
} ServiceTableEntry;


/* include C source if C compiler inline option selected (optimize) */

#ifdef _INLINE
#include <dsserver.c>
#else
extern void master_cmd_server(void);
extern void slave_cmd_server(void);
#endif

#endif
