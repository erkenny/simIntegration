/******************************************************************************
*
* MODULE
*   General master-slave communication functions (hardware independent).
*   Common definitions for master and slave CPU
*
* FILE
*   dscomdef.h
*
* RELATED FILES
*   dsmcom.c, dsscom.c
*
* DESCRIPTION
*   Definition of
*   - general error codes
*   - general command OP-codes
*   - parameter count and sequence for individual commands
*   - communication address and data types
*
* AUTHOR(S)
*   H.-J. Miks, H.-G. Limberg
*
* dSPACE GmbH, Technologiepark 25, 33100 Paderborn, Germany
*
* $RCSfile: dscomdef.h $ $Revision: 1.3 $ $Date: 2006/09/01 10:57:02GMT+01:00 $
******************************************************************************/

#ifndef __DSCOMDEF_H__
#define __DSCOMDEF_H__

/******************************************************************************
* constant and macro definitions
******************************************************************************/

/* slave error codes */

#define DSCOMDEF_NO_ERROR         0
#define DSCOMDEF_ALLOC_ERROR      10
#define DSCOMDEF_BUFFER_OVERFLOW  11
#define DSCOMDEF_ILLEGAL_USR_IDX  13

#define DSCOMDEF_TIMEOUT_ERROR    15

/* special error code for slave initialization acknowledge */

#define DSCOMDEF_INIT_ACK         12

/* general constants */

#define DSCOMDEF_TIMEOUT          1.0f        /* slave response timeout [sec] */
#define DSCOMDEF_FIFO_TIMEOUT     1000              /* FIFO timeout [cycles] */
#define DSCOMDEF_AUTO_INDEX       -1           /* automatic index generation */
#define DSCOMDEF_TASK_LIST_INC    8                   /* task list increment */
#define DSCOMDEF_CMDTBL_INC       8               /* command table increment */
#define DSCOMDEF_SPI_CMDTBL_INC   8           /* spi command table increment */

/* command mode constants */

#define DSCOMDEF_MODE_INDIRECT    0            /* indirect command execution */
#define DSCOMDEF_MODE_DIRECT      1              /* direct command execution */

/* command OP-codes */

#define DSCOMDEF_NO_COMMAND       0x0000                       /* no command */
#define DSCOMDEF_CMD_REG          0x0002             /* command registration */

/* parameter count and sequence definitions */

/* command registration */

#define DSCOMDEF_CMD_REG_PCNT     3/* parm count (not inc. target fct parms) */
#define DSCOMDEF_CMD_REG_IDX_LOC  0                         /* command index */
#define DSCOMDEF_CMD_REG_CMD_LOC  1                       /* command OP-code */
#define DSCOMDEF_CMD_REG_PCNT_LOC 2                       /* parameter count */
#define DSCOMDEF_CMD_REG_PARM_LOC 3            /* target function parameters */

/* FIFO command registration */

#define DSCOMDEF_FIFO_DATA_CNT     2/* data count (not inc. target fct data) */
#define DSCOMDEF_FIFO_DATA_IDX_LOC 0                           /* fifo index */
#define DSCOMDEF_FIFO_DATA_CNT_LOC 1                           /* data count */
#define DSCOMDEF_FIFO_DATA_LOC     2                        /* transmit data */

/* valid command width */
#define DSCOMDEF_CMD_WIDTH 16

/* specifications for faster mode communication */

#define DSCOMDEF_MASTER_READY 0
#define DSCOMDEF_SLAVE_READY  1

#define DSCOMDEF_ERROR_SET   1
#define DSCOMDEF_ERROR_CLEAR 0

#define DSCOMDEF_BUSY     1
#define DSCOMDEF_NOT_BUSY 0

#endif /* __DSCOMDEF_H__ */

