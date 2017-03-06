/******************************************************************************
*
* MODULE
*   definitions for both dual port memories on DS1103 board
*
* FILE
*   def1103.h
*
* RELATED FILES
*
*
* DESCRIPTION
*    - definitions for dual port memories PPC <=> SLVDSP and PPC <=> SLVMC
*
* AUTHOR(S)
*   H.-G. Limberg
*
* dSPACE GmbH, Technologiepark 25, 33100 Paderborn, Germany
*
* $RCSfile: def1103.h $ $Revision: 1.3 $ $Date: 2008/10/08 09:48:02GMT+01:00 $
* $ProjectName: e:/ARC/Products/ImplSW/RTLibSW/RTLib1103/Develop/DS1103/DS1103DPMEM.pj $
******************************************************************************/

#ifndef __DEF1103_H__
#define __DEF1103_H__

/*--- function module control SLVDSP ----------------------------------------*/

/*
  comment out corresponding defines in order to exclude individual
  function modules from compilation
*/

#define SLVDSP1103_ADC                                          /* ADC input */
#define SLVDSP1103_PIO                                    /* digital bit I/O */
#define SLVDSP1103_PWM                              /* simple PWM generation */
#define SLVDSP1103_PWM3                            /* 3-phase PWM generation */
#define SLVDSP1103_PWM3SV             /* 3-phase space vector PWM generation */
#define SLVDSP1103_D2F                         /* D/F square wave generation */
#define SLVDSP1103_F2D                          /* F/D frequency measurement */
#define SLVDSP1103_PWM2D                            /* PWM/D PWM measurement */
#define SLVDSP1103_SCI                                  /* SCI communication */
#define SLVDSP1103_SPI                                  /* SPI communication */

/*----- set generally used symbols to processor specific values ------*/

#ifdef _DS1103
  #include <ds1103.h>
  #include <sint1103.h>
  #define SLVDSP_DP_RAM_START  DS1103_SLAVE_DSP_DP_RAM_START
  #define SLVMC_DP_RAM_START   DS1103_SLAVE_MC_DP_RAM_START
  #define ADDR_FACTOR          DEF1103_MST_ADDR_INC     /* DSP DP RAM is byte addressed from PPC  */
#endif

#ifdef _DS1103_SLAVE_DSP
  #include <sint1103.h>
  #define SLVDSP_DP_RAM_START  0xF000
  #define ADDR_FACTOR          DEF1103_SLVDSP_ADDR_INC  /* DSP DP RAM is 16 bit addressed from DSP */
#endif

#ifdef _DS1103_SLAVE_MC
  #include <dpmslv.h>
  #include <sint164.h>        /* sub interrupt addresses and defines */
#endif



/*--- address increments ----------------------------------------------------*/

#define DEF1103_MST_ADDR_INC    2         /* DP RAMs are byte addressed from PPC  */
#define DEF1103_SLVDSP_ADDR_INC 1         /* DSP DP RAM is 16 bit addressed from DSP */
#define DEF1103_SLVMC_ADDR_INC  2         /* MC DP RAM is 16 bit addressed from DSP */

/*--- communication buffers -------------------------------------------------*/
/*--- SLVDSP ----------------------------------------------------------------*/
/*
  Note: Dual-port memory address offsets 0x0FF0..0x0FFF are reserved for sub
  interrupt support. Thus DEF1103_SLVDSP1103_BUFFER_NUM * (2 * SLVDSP1103_BUFFER_SIZE + 3) must
  not exceed 4079.
*/

#define DEF1103_SLVDSP_BUFFER_NUM   3
#define DEF1103_SLVDSP_BUFFER_SIZE  512

/*--- Firmware Version Exchange Addresses (do. not change !) ----------------*/

#define DEF1103_SLVDSP_DSPACE_FW_ADDR     (SLVDSP_DP_RAM_START  + \
                                          (0xF00 * ADDR_FACTOR))

#define DEF1103_SLVDSP_USER_MAJ_FW_ADDR   (SLVDSP_DP_RAM_START  + \
                                          (0xF01 * ADDR_FACTOR))
#define DEF1103_SLVDSP_USER_MIN_FW_ADDR   (SLVDSP_DP_RAM_START  + \
                                          (0xF02 * ADDR_FACTOR))
#define DEF1103_SLVDSP_USER_SUB_FW_ADDR   (SLVDSP_DP_RAM_START  + \
                                          (0xF03 * ADDR_FACTOR))
#define DEF1103_SLVDSP_FW_BURNC_LO_ADDR   (SLVDSP_DP_RAM_START  + \
                                          (0xF05 * ADDR_FACTOR))
#define DEF1103_SLVDSP_FW_BURNC_HI_ADDR   (SLVDSP_DP_RAM_START  + \
                                          (0xF06 * ADDR_FACTOR))

/*--- SLVMC -----------------------------------------------------------------*/

/*
  Note: Dual-port memory address offsets 0x????..0x???? are reserved for sub
  interrupt support. Thus DEF1103_SLVMC_BUFFER_NUM * (2 * SLVDSP1103_BUFFER_SIZE + 3) must
  not exceed ????.
*/

#define DEF1103_CAN_BUFFER_NUM   7
#define DEF1103_CAN_BUFFER_SIZE  256
#define DEF1103_CAN_ADDR_INC     DEF1103_SLVMC_ADDR_INC

/* address for TX-Queue-info, occupies 2 words, do not change! */
#define DEF1103_CAN_TXQUEUE_INFO_ADDR    (SLVMC_DP_RAM_START  + 0x1FD0)
#define DEF1103_CAN_TIMERSYNC_ADDR       (SLVMC_DP_RAM_START  + 0x1FE0)

/*--- Firmware Version Exchange Address     (do. not change !) --------------*/
#define DEF1103_CAN_FIRMWARE_ADDR        (SLVMC_DP_RAM_START  + 0x02)
#define DEF1103_CAN_USER_MAJ_FW_ADDR     (SLVMC_DP_RAM_START  + 0x04)
#define DEF1103_CAN_USER_MIN_FW_ADDR     (SLVMC_DP_RAM_START  + 0x06)
#define DEF1103_CAN_USER_SUB_ADDR        (SLVMC_DP_RAM_START  + 0x08)
#define DEF1103_CAN_FW_BURNC_LO_ADDR     (SLVMC_DP_RAM_START  + 0x0C)
#define DEF1103_CAN_FW_BURNC_HI_ADDR     (SLVMC_DP_RAM_START  + 0x10)

#define DEF1103_CAN_MSG_REGISTRY_DEFAULT_SIZE 50
#define DEF1103_CAN_MSG_REGISTRY_DEFAULT_REALLOC_SIZE 50


#include <op1103.h>                                      /* command op-codes */
#include <para1103.h>                           /* slave function parameters */

#endif
