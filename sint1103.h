/******************************************************************************
*
* MODULE
*   definitions for both dual port memories on DS1103 board
*
* FILE
*   sint1103.h
*
* RELATED FILES
*
* DESCRIPTION
*    - slave DSP and slave MC sub-interrupt definitions
*
* AUTHOR(S)
*   H.-G. Limberg
*
* dSPACE GmbH, Technologiepark 25, 33100 Paderborn, Germany
*
* $RCSfile: sint1103.h $ $Revision: 1.2 $ $Date: 2003/08/07 14:54:22GMT+01:00 $
* $ProjectName: e:/ARC/Products/ImplSW/RTLibSW/RTLib1103/Develop/DS1103/DS1103DPMEM.pj $
******************************************************************************/

#ifndef _SINT1103_H_
#define _SINT1103_H_

/*--- sint defines -------------------------------------------------*/

#define SINT1103_SINT_MEM_WIDTH 16

/* memory addresse for interupt generating / acknowledging */
#define SINT1103_DSP2PPC_SEND_ADDR (SLVDSP_DP_RAM_START + ADDR_FACTOR * 0x0FFE)
#define SINT1103_PPC2DSP_SEND_ADDR (SLVDSP_DP_RAM_START + ADDR_FACTOR * 0x0FFF)

#define SINT1103_MC2PPC_SEND_ADDR  (SLVMC_DP_RAM_START  + 0x1FFC)
#define SINT1103_PPC2MC_SEND_ADDR  (SLVMC_DP_RAM_START  + 0x1FFE)

/* ------------------------------------------------------------------------- */


/* memory addresses for sub-interupt communication */
#define SINT1103_DSP2PPC_FW_SUBINT_ADDR           (SLVDSP_DP_RAM_START + ADDR_FACTOR * 0x0FFA)
#define SINT1103_DSP2PPC_FW_ACKNOWLEDGE_ADDR      (SLVDSP_DP_RAM_START + ADDR_FACTOR * 0x0FFC)
#define SINT1103_DSP2PPC_USR_SUBINT_ADDR          (SLVDSP_DP_RAM_START + ADDR_FACTOR * 0x0FF6)
#define SINT1103_DSP2PPC_USR_ACKNOWLEDGE_ADDR     (SLVDSP_DP_RAM_START + ADDR_FACTOR * 0x0FF8)
#define SINT1103_PPC2DSP_SUBINT_ADDR              (SLVDSP_DP_RAM_START + ADDR_FACTOR * 0x0FF2)
#define SINT1103_PPC2DSP_ACKNOWLEDGE_ADDR         (SLVDSP_DP_RAM_START + ADDR_FACTOR * 0x0FF4)

#define SINT1103_MC2PPC_SUBINT_ADDR               (SLVMC_DP_RAM_START  + 0x1FF8)
#define SINT1103_MC2PPC_ACKNOWLEDGE_ADDR          (SLVMC_DP_RAM_START  + 0x1FFA)
#define SINT1103_PPC2MC_SUBINT_ADDR               (SLVMC_DP_RAM_START  + 0x1FF4)
#define SINT1103_PPC2MC_ACKNOWLEDGE_ADDR          (SLVMC_DP_RAM_START  + 0x1FF6)


/* ------------------------------------------------------------------------- */

/* number of sub-interrupts */
#define SINT1103_MC2PPC_SINT_NUMBER                16   /* maximum 16, do not change !!! */
#define SINT1103_PPC2MC_SINT_NUMBER                16   /* maximum 16 */
#define SINT1103_PPC2DSP_SINT_NUMBER               16   /* maximum 32, not supported by firmware */
#define SINT1103_DSP2PPC_USR_SINT_NUMBER           16   /* maximum 32, not supported by firmware */
#define SINT1103_DSP2PPC_FW_SINT_NUMBER            25   /* maximum 32, do not change !!! */

/* ------------------------------------------------------------------------- */

#define SINT1103_DSP_FW_TASK_OFFSET                32   /* do not change ! */
/* ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */

/*--- sub interrupt numbers for DSP firmware (do not change !) ------------------*/

/* sint 0 is reserved for future use */
#define SINT1103_DSP2PPC_FW_ADC_CH1     1
#define SINT1103_DSP2PPC_FW_ADC_CH2     2
#define SINT1103_DSP2PPC_FW_ADC_CH3     3
#define SINT1103_DSP2PPC_FW_ADC_CH4     4
#define SINT1103_DSP2PPC_FW_ADC_CH5     5
#define SINT1103_DSP2PPC_FW_ADC_CH6     6
#define SINT1103_DSP2PPC_FW_ADC_CH7     7
#define SINT1103_DSP2PPC_FW_ADC_CH8     8
#define SINT1103_DSP2PPC_FW_ADC_CH9     9
#define SINT1103_DSP2PPC_FW_ADC_CH10    10
#define SINT1103_DSP2PPC_FW_ADC_CH11    11
#define SINT1103_DSP2PPC_FW_ADC_CH12    12
#define SINT1103_DSP2PPC_FW_ADC_CH13    13
#define SINT1103_DSP2PPC_FW_ADC_CH14    14
#define SINT1103_DSP2PPC_FW_ADC_CH15    15
#define SINT1103_DSP2PPC_FW_ADC_CH16    16

#define SINT1103_DSP2PPC_FW_F2D_CH1     17
#define SINT1103_DSP2PPC_FW_F2D_CH2     18
#define SINT1103_DSP2PPC_FW_F2D_CH3     19
#define SINT1103_DSP2PPC_FW_F2D_CH4     20

#define SINT1103_DSP2PPC_FW_PWM2D_CH1   21
#define SINT1103_DSP2PPC_FW_PWM2D_CH2   22
#define SINT1103_DSP2PPC_FW_PWM2D_CH3   23
#define SINT1103_DSP2PPC_FW_PWM2D_CH4   24

#endif
