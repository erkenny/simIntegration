/******************************************************************************
*
* FILE:
*   Io1103.h
*
* RELATED FILES:
*   Io1103.c 
*
* DESCRIPTION:
*   DS1103 on-board I/O access functions for:
*    - ADC input
*    - DAC output
*    - incremental encoder support
*    - parallel port digital I/O
*    - cpu-temperature reading
*   
* 
*
* HISTORY: RK initial version
*
* dSPACE GmbH, Technologiepark 25, 33100 Paderborn, Germany
*
* $RCSfile: IO1103.H $ $Revision: 1.12 $ $Date: 2004/01/14 10:04:59GMT+01:00 $
******************************************************************************/
 
#ifndef __Io1103__
#define __Io1103__

#include <Dstypes.h>
#include <dsstd.h>


/******************************************************************************
  constant, macro, and type definitions
******************************************************************************/




/******************************************************************************
 user definitions for adc-access 
******************************************************************************/

/* def. for ds1103_adc_start(mask) */
#define DS1103_ADC1     1
#define DS1103_ADC2     2
#define DS1103_ADC3     4
#define DS1103_ADC4     8 
#define DS1103_ADC5     16
#define DS1103_ADC6     32
#define DS1103_ADC7     64
#define DS1103_ADC8     128

#define DS1103_ADC_CH1     1
#define DS1103_ADC_CH2     1
#define DS1103_ADC_CH3     1
#define DS1103_ADC_CH4     1
#define DS1103_ADC_CH5     2
#define DS1103_ADC_CH6     2 
#define DS1103_ADC_CH7     2 
#define DS1103_ADC_CH8     2 
#define DS1103_ADC_CH9     4 
#define DS1103_ADC_CH10    4 
#define DS1103_ADC_CH11    4 
#define DS1103_ADC_CH12    4 
#define DS1103_ADC_CH13    8 
#define DS1103_ADC_CH14    8 
#define DS1103_ADC_CH15    8 
#define DS1103_ADC_CH16    8 
#define DS1103_ADC_CH17   16
#define DS1103_ADC_CH18   32 
#define DS1103_ADC_CH19   64 
#define DS1103_ADC_CH20  128 

#define DS1103_ADC_CHANNEL1     0x00001
#define DS1103_ADC_CHANNEL2     0x00002
#define DS1103_ADC_CHANNEL3     0x00004
#define DS1103_ADC_CHANNEL4     0x00008
#define DS1103_ADC_CHANNEL5     0x00010
#define DS1103_ADC_CHANNEL6     0x00020 
#define DS1103_ADC_CHANNEL7     0x00040 
#define DS1103_ADC_CHANNEL8     0x00080 
#define DS1103_ADC_CHANNEL9     0x00100 
#define DS1103_ADC_CHANNEL10    0x00200 
#define DS1103_ADC_CHANNEL11    0x00400 
#define DS1103_ADC_CHANNEL12    0x00800 
#define DS1103_ADC_CHANNEL13    0x01000 
#define DS1103_ADC_CHANNEL14    0x02000 
#define DS1103_ADC_CHANNEL15    0x04000 
#define DS1103_ADC_CHANNEL16    0x08000 
#define DS1103_ADC_CHANNEL17    0x10000 
#define DS1103_ADC_CHANNEL18    0x20000 
#define DS1103_ADC_CHANNEL19    0x40000 
#define DS1103_ADC_CHANNEL20    0x80000 

/******************************************************************************/



/******************************************************************************
  user definitions for dac-access 
******************************************************************************/

#define DS1103_DACMODE_LATCHED         0
#define DS1103_DACMODE_TRANSPARENT      1

#define DS1103_SYNC_TRIGGER_FALLING      0
#define DS1103_SYNC_TRIGGER_RISING      1
/******************************************************************************/



/******************************************************************************
  user definitions for dio-access 
******************************************************************************/

#define DS1103_DIO1_OUT 1 
#define DS1103_DIO2_OUT 2 
#define DS1103_DIO3_OUT 4
#define DS1103_DIO4_OUT 8
#define DS1103_DIO1_IN 0 
#define DS1103_DIO2_IN 0 
#define DS1103_DIO3_IN 0
#define DS1103_DIO4_IN 0


#define DS1103_DIO0_SET 0x1
#define DS1103_DIO1_SET 0x2
#define DS1103_DIO2_SET 0x4
#define DS1103_DIO3_SET 0x8
#define DS1103_DIO4_SET 0x10
#define DS1103_DIO5_SET 0x20
#define DS1103_DIO6_SET 0x40
#define DS1103_DIO7_SET 0x80
#define DS1103_DIO8_SET 0x100
#define DS1103_DIO9_SET 0x200
#define DS1103_DIO10_SET 0x400
#define DS1103_DIO11_SET 0x800
#define DS1103_DIO12_SET 0x1000 
#define DS1103_DIO13_SET 0x2000
#define DS1103_DIO14_SET 0x4000
#define DS1103_DIO15_SET 0x8000
#define DS1103_DIO16_SET 0x10000
#define DS1103_DIO17_SET 0x20000
#define DS1103_DIO18_SET 0x40000
#define DS1103_DIO19_SET 0x80000
#define DS1103_DIO20_SET 0x100000
#define DS1103_DIO21_SET 0x200000
#define DS1103_DIO22_SET 0x400000
#define DS1103_DIO23_SET 0x800000
#define DS1103_DIO24_SET 0x1000000
#define DS1103_DIO25_SET 0x2000000
#define DS1103_DIO26_SET 0x4000000
#define DS1103_DIO27_SET 0x8000000
#define DS1103_DIO28_SET 0x10000000
#define DS1103_DIO29_SET 0x20000000
#define DS1103_DIO30_SET 0x40000000
#define DS1103_DIO31_SET 0x80000000

#define DS1103_DIO0_CLEAR 0x1
#define DS1103_DIO1_CLEAR 0x2
#define DS1103_DIO2_CLEAR 0x4
#define DS1103_DIO3_CLEAR 0x8
#define DS1103_DIO4_CLEAR 0x10
#define DS1103_DIO5_CLEAR 0x20
#define DS1103_DIO6_CLEAR 0x40
#define DS1103_DIO7_CLEAR 0x80
#define DS1103_DIO8_CLEAR 0x100
#define DS1103_DIO9_CLEAR 0x200
#define DS1103_DIO10_CLEAR 0x400
#define DS1103_DIO11_CLEAR 0x800
#define DS1103_DIO12_CLEAR 0x1000 
#define DS1103_DIO13_CLEAR 0x2000
#define DS1103_DIO14_CLEAR 0x4000
#define DS1103_DIO15_CLEAR 0x8000
#define DS1103_DIO16_CLEAR 0x10000
#define DS1103_DIO17_CLEAR 0x20000
#define DS1103_DIO18_CLEAR 0x40000
#define DS1103_DIO19_CLEAR 0x80000
#define DS1103_DIO20_CLEAR 0x100000
#define DS1103_DIO21_CLEAR 0x200000
#define DS1103_DIO22_CLEAR 0x400000
#define DS1103_DIO23_CLEAR 0x800000
#define DS1103_DIO24_CLEAR 0x1000000
#define DS1103_DIO25_CLEAR 0x2000000
#define DS1103_DIO26_CLEAR 0x4000000
#define DS1103_DIO27_CLEAR 0x8000000
#define DS1103_DIO28_CLEAR 0x10000000
#define DS1103_DIO29_CLEAR 0x20000000
#define DS1103_DIO30_CLEAR 0x40000000
#define DS1103_DIO31_CLEAR 0x80000000

/******************************************************************************/





/******************************************************************************
  user definitions for incremental encoder interface-access 
******************************************************************************/


/* Parameter for use with ds1103_inc_init() */
/* inc_mode definitions */
#define DS1103_INC_CH1_TTL                0x0
#define DS1103_INC_CH2_TTL                0x0
#define DS1103_INC_CH3_TTL                0x0
#define DS1103_INC_CH4_TTL                0x0
#define DS1103_INC_CH5_TTL                0x0
#define DS1103_INC_CH6_TTL                0x0
#define DS1103_INC_CH7_uA                 0x0

#define DS1103_INC_CH1_RS422              0x1
#define DS1103_INC_CH2_RS422              0x1
#define DS1103_INC_CH3_RS422              0x1
#define DS1103_INC_CH4_RS422              0x1
#define DS1103_INC_CH5_RS422              0x1
#define DS1103_INC_CH6_RS422              0x4
#define DS1103_INC_CH7_VPP                0x4

/* Inc.channel 6 and 7 resolution */
#define DS1103_INC_CH6_16BIT         0x0 /* inc-ch6-mode: 16 bit */
#define DS1103_INC_CH7_38BIT         0x0 /* inc-ch7-mode: 38 bit */
#define DS1103_INC_CH6_32BIT         0x1 /* inc-ch6-mode: 32 bit */
#define DS1103_INC_CH7_22BIT         0x1 /* inc-ch7-mode: 22 bit */
#define DS1103_INC_CH6_24BIT         0x3 /* inc-ch6-mode: 24 bit */




/* definitions for line_subdiv */
#define DS1103_INC_LINE_SUBDIV_256   0               /* 256 fold */
#define DS1103_INC_LINE_SUBDIV_128   1               /* 128 fold */
#define DS1103_INC_LINE_SUBDIV_64    2               /* 64 fold */
#define DS1103_INC_LINE_SUBDIV_32    3               /* 32 fold */
#define DS1103_INC_LINE_SUBDIV_16    4               /* 16 fold */
#define DS1103_INC_LINE_SUBDIV_8     5               /* 8 fold */
#define DS1103_INC_LINE_SUBDIV_4     6               /* 4 fold */
#define DS1103_INC_LINE_SUBDIV_2     7               /* 2 fold */
#define DS1103_INC_LINE_SUBDIV_1     8               /* 1 fold */
#define DS1103_INC_LINE_SUBDIV_1_2   9               /* 1/2 fold */
#define DS1103_INC_LINE_SUBDIV_1_4  10               /* 1/4 fold */
#define DS1103_INC_LINE_SUBDIV_1_8  11               /* 1/8 fold */


/* definitions for ds1103_inc_index_read: return-values */
#define DS1103_INC_IDX_NOT_SET 0
#define DS1103_INC_IDX_SET 1

/* definitions for reset_enable: ds1103_inc_index_read and ds1103_inc_set_idxmode */
#define DS1103_INC_NO_RESETONIDX 0x0
#define DS1103_INC_RESETONIDX    0x1

#define DS1103_INC_CH1_NO_RESETONIDX      0x0
#define DS1103_INC_CH2_NO_RESETONIDX      0x0
#define DS1103_INC_CH3_NO_RESETONIDX      0x0
#define DS1103_INC_CH4_NO_RESETONIDX      0x0
#define DS1103_INC_CH5_NO_RESETONIDX      0x0
#define DS1103_INC_CH6_NO_RESETONIDX      0x0
#define DS1103_INC_CH7_NO_RESETONIDX      0x0
#define DS1103_INC_CH1_RESETONIDX         0x1
#define DS1103_INC_CH2_RESETONIDX         0x1
#define DS1103_INC_CH3_RESETONIDX         0x1
#define DS1103_INC_CH4_RESETONIDX         0x1
#define DS1103_INC_CH5_RESETONIDX         0x1
#define DS1103_INC_CH6_RESETONIDX         0x1
#define DS1103_INC_CH7_RESETONIDX         0x1


/******************************************************************************/



/******************************************************************************
 user definitions for cpu temperature read
******************************************************************************/

#define DS1103_RDTMP_FINISHED          0
#define DS1103_RDTMP_WAIT_RDY1         1
#define DS1103_RDTMP_WAIT_RDY2         2
#define DS1103_RDTMP_ERROR             3

/******************************************************************************/





/******************************************************************************
 address definitions 
******************************************************************************/

/* definitions of io-registers */
#define DS1103_IOSTB      0x10000080
#define DS1103_TMRSTB     0x10000082
#define DS1103_ADC        0x10800000
#define DS1103_ADCCTRL    0x10000084
#define DS1103_DAC        0x10A00000  
#define DS1103_DIGIO      0x11200000
#define DS1103_SETUP      0x10000088   
#define DS1103_RESET      0x10000040
#define DS1103_ANALENCOD  0x10C00000
#define DS1103_DIGITENCOD 0x10E00000 
#define DS1103_RSTENC     0x10000086
#define DS1103_INTERRUPT  0x10000020
#define DS1103_IMR        0x10000024

/* control registers  */
#define DS1103_RESET          0x10000040               /*16*/
#define DS1103_DIGINC_RESET   0x10000086               /*16*/
#define DS1103_EEPROM         0x10000044               /*16*/
#define DS1103_ADC_CTRL       0x10000084               /*16*/
#define DS1103_IO_STROBE      0x10000080               /*16*/
#define DS1103_SYNC_CTRL      0x1000002C            /*32*//*p750*/                                    /*p750*/


/* ADCs single read 16 bit */
#define DS1103_ADC_1          0x10800000               /*16*/
#define DS1103_ADC_2          0x10800002               /*16*/
#define DS1103_ADC_3          0x10800004               /*16*/
#define DS1103_ADC_4          0x10800006               /*16*/
#define DS1103_ADC_5          0x10800008               /*16*/
#define DS1103_ADC_6          0x1080000A               /*16*/
#define DS1103_ADC_7          0x1080000C               /*16*/
#define DS1103_ADC_8          0x1080000E               /*16*/

/* ADCs double read 32 bit */
#define DS1103_ADC_12          0x10800000               /*32*/
#define DS1103_ADC_34         0x10800004               /*32*/
#define DS1103_ADC_56         0x10800008               /*32*/
#define DS1103_ADC_78         0x1080000C               /*32*/


/* DAC's single read 16 bit*/
#define DS1103_DAC_1          0x10A00000             /*16*/
#define DS1103_DAC_2          0x10A00002             /*16*/
#define DS1103_DAC_3          0x10A00004             /*16*/
#define DS1103_DAC_4          0x10A00006             /*16*/
#define DS1103_DAC_5          0x10A00008             /*16*/
#define DS1103_DAC_6          0x10A0000A             /*16*/
#define DS1103_DAC_7          0x10A0000C             /*16*/
#define DS1103_DAC_8          0x10A0000E             /*16*/

/* DAC's double read 32 bit*/
#define DS1103_DAC_12         0x10A00000            /*32*/
#define DS1103_DAC_43         0x10A00004            /*32*/
#define DS1103_DAC_56         0x10A00008            /*32*/
#define DS1103_DAC_78         0x10A0000C            /*32*/


/* analog INC 16bit */
#define DS1103_ANALENCOD_SYSCON   0x10C00000        /*16*/
#define DS1103_ANALENCOD_CNTCON   0x10C00002        /*16*/
#define DS1103_ANALENCOD_ARCTAN   0x10C00004        /*16*/
#define DS1103_ANALENCOD_CT1DATL  0x10C00006        /*16*/
#define DS1103_ANALENCOD_CT1DATH  0x10C00008        /*16*/
#define DS1103_ANALENCOD_CT2DAT   0x10C0001A        /*16*/
#define DS1103_ANALENCOD_UPDATCYC 0x10C0001C        /*16*/
#define DS1103_ANALENCOD_LINCNT   0x10C0001E        /*16*/
#define DS1103_INCSYSCON          0x10C00000
#define DS1103_INCCNTCON          0x10C00002


/* digital INC, 32bit */
#define DS1103_DIGITENCOD_PCR1     0x10E00000 
#define DS1103_DIGITENCOD_PCR2     0x10E00004
#define DS1103_DIGITENCOD_PCR3     0x10E00008
#define DS1103_DIGITENCOD_PCR4     0x10E0000C
#define DS1103_DIGITENCOD_PCR5     0x10E00040

/* digital IO, 32bit */
#define DS1103_DIGIO               0x11200000

/******************************************************************************/



/******************************************************************************
 register access definitions 
******************************************************************************/


#define DS1103_DIGINC_RESET_REG     ( (volatile UInt16*)DS1103_DIGINC_RESET )
#define DS1103_EEPROM_REG           ( (volatile UInt16*)DS1103_EEPROM )
#define DS1103_ADC_CTRL_REG         ( (volatile UInt16*)DS1103_ADC_CTRL )
#define DS1103_IO_STROBE_REG        ( (volatile UInt16*)DS1103_IO_STROBE )
#define DS1103_SYNC_CRTL_REG      ( (volatile UInt32*)DS1103_SYNC_CTRL )


#define DS1103_SETUP_REG            ( (volatile UInt16*)DS1103_SETUP )
#define DS1103_RESET_REG            ( (volatile UInt16*)DS1103_RESET )

#define DS1103_IOSTB_REG            ( (volatile UInt16*)DS1103_IOSTB )
#define DS1103_TMRSTB_RE            ( (volatile UInt16*)DS1103_TMRSTB )

#define DS1103_ADC_REG              ( (volatile Int16*)DS1103_ADC )
#define DS1103_ADC_REG2             ( (volatile UInt32*)DS1103_ADC )

#define DS1103_DAC_REG              ( (volatile Int16*)DS1103_DAC ) 
#define DS1103_DAC_REG2             ( (volatile UInt32*)DS1103_DAC ) 

#define DS1103_DIGIO_REG            ( (volatile UInt32*)DS1103_DIGIO )

#define DS1103_ANALENCOD_REG        ( (volatile UInt16*)DS1103_ANALENCOD )
#define DS1103_DIGITENCOD_REG       ( (volatile Int32*)DS1103_DIGITENCOD )

#define DS1103_RSTENC_REG           ( (volatile UInt16*)DS1103_RSTENC )

#define DS1103_INTERRUPT_REG        ( (volatile UInt32*)DS1103_INTERRUPT )
#define DS1103_IMR_REG              ( (volatile UInt32*)DS1103_IMR )


#define adc_ctrl_bit  ( (volatile ds1103_adcctrl_reg*)DS1103_ADC_CTRL)->_adcctrl_bit
#define adc_ctrl_reg  ( (volatile ds1103_adcctrl_reg*)DS1103_ADC_CTRL)->_adcctrl_reg

#define sync_ctrl_bit ( (volatile ds1103_syncctrl_reg*)DS1103_SYNC_CTRL)->_syncctrl_bit
#define sync_ctrl_reg ( (volatile ds1103_syncctrl_reg*)DS1103_SYNC_CTRL)->_syncctrl_reg


#define iostb_bit     ( (volatile ds1103_iostb_reg*)DS1103_IO_STROBE)->_iostb_bit
#define iostb_reg     ( (volatile ds1103_iostb_reg*)DS1103_IO_STROBE)->_iostb_reg

#define setup_bit     ( (volatile ds1103_setup_reg*)DS1103_SETUP)->_setup_bit
#define setup_reg     ( (volatile ds1103_setup_reg*)DS1103_SETUP)->_setup_reg

#define rest_bit      ( (volatile ds1103_rest_reg*)DS1103_RESET)->_rest_bit
#define rest_reg      ( (volatile ds1103_rest_reg*)DS1103_RESET)->_rest_reg

#define eeprom_bit    ( (volatile ds1103_eeprom_reg*)DS1103_EEPROM)->_eeprom_bit
#define eeprom_reg    ( (volatile ds1103_eeprom_reg*)DS1103_EEPROM)->_eeprom_reg

#define inc_cntcon_bit ( (volatile ds1103_inc_cntcon_reg*)DS1103_INCCNTCON)->_inc_cntcon_bit
#define inc_cntcon_reg ( (volatile ds1103_inc_cntcon_reg*)DS1103_INCCNTCON)->_inc_cntcon_reg

#define inc_cntcon_rg  (*( (volatile UInt16*)DS1103_INCCNTCON) )

#define inc_syscon_bit ( (volatile ds1103_inc_syscon_reg*)DS1103_INCSYSCON)->_inc_syscon_bit
#define inc_syscon_reg ( (volatile ds1103_inc_syscon_reg*)DS1103_INCSYSCON)->_inc_syscon_reg

#define inc_hanning    ( (volatile ds1103_analenc*)DS1103_ANALENCOD ) 

/******************************************************************************/




/******************************************************************************
 internal definitions for adc-access 
******************************************************************************/

#define DS1103_ADC_SELADC1_OFFS  8
#define DS1103_ADC_SELADC2_OFFS 10 
#define DS1103_ADC_SELADC3_OFFS 12 
#define DS1103_ADC_SELADC4_OFFS 14 


#define DS1103_ADC_START_MSK 0xFF
#define DS1103_ADC_BUSY_MSK 0xFF

/* for adc_read_all */
#define DS1103_ADC_BUSY_MSK_16B 0x0F
#define DS1103_ADC_BUSY_MSK_12B 0xF0

/* def. start-delay after mux-adjustment */
#define DS1103_ADC_MUX_DELAY 2.00000e-6
#define DS1103_ADC_MUX_DELAY_ADVANCED 1.00000e-6

 /* input scaling factor: 2^-15 */
#define DS1103_ADC_SCALE     30.517578125e-6

/* def. for masking 12 bit ADCs */
#define DS1103_ADC_12BIT_MSK  0xFFF0
 
/******************************************************************************/



/******************************************************************************
  internal definitions for dac-access 
******************************************************************************/

#define DS1103_DAC_SCALE     32.7680000e3     /* output scaling factor: 2^15 */
#define DS1103_DAC_MIN       -1.0                 /* for negative saturation */
#define DS1103_DAC_MAX       999.969482422e-3     /* for positive saturation */ 

#define DS1103_DAC_STROBE  0x100 

/******************************************************************************/

/******************************************************************************
  internal definitions for SYNCIN and SYNCOUT
******************************************************************************/

/* mask for SYNCIN ADC */
#define DS1103_STRB_SYNCIN_ADC_MSK          0x000000FFL
/* mask for SYNCOUT DAC*/
#define DS1103_STRB_SYNCOUT_MSK            0x00000100L
/* mask for SYNCIN INC*/
#define DS1103_STRB_SYNCIN_INC_MSK         0x0000FC00L
/* mask for SYNCIN (INC+ADC)*/
#define DS1103_STRB_SYNCIN_MSK            0x0000FCFFL


/******************************************************************************/


/******************************************************************************
  internal definitions for incremental encoder interface-access 
******************************************************************************/

/* mask for inc-ch6 as 24 bit counter */
#define DS1103_DIGINC2_MASK   0xFFFFFF00      

/* mask for finecount ch.7 */
#define DS1103_INC_CH7_FINECMSK 0x3F

/* definitions for scaling */
#define DS1103_INC2_LINESCAL_FINEC  3.9062500e-3      /* 256 fold 6 bit finecount */


/* definitions for Hanning: Cntcon-Reg. */
#define DS1103_HANINC_CT1CM0                  0x1
#define DS1103_HANINC_CT1CM1                  0x2
#define DS1103_HANINC_CT1ENC                  0x4
#define DS1103_HANINC_CT1ERR                  0x8
#define DS1103_HANINC_CT2CM0                 0x10
#define DS1103_HANINC_CT2CM1                 0x20
#define DS1103_HANINC_CT2ENC                 0x40
#define DS1103_HANINC_CT2ERR                 0x80
#define DS1103_HANINC_CT1LM0                0x100
#define DS1103_HANINC_CT1LM1                0x200
#define DS1103_HANINC_CT1LM2                0x400
#define DS1103_HANINC_CT1LEN                0x800
#define DS1103_HANINC_CT2LM0               0x1000
#define DS1103_HANINC_CT2LM1               0x2000
#define DS1103_HANINC_CT2LM2               0x4000
#define DS1103_HANINC_CT2LEN               0x8000
#define DS1103_HANINC_CT1_LMDE0_MSK        0xF8FF
#define DS1103_HANINC_CT2_LMDE0_MSK        0x8FFF
#define DS1103_HANINC_CT1_LOAD_DISABLE_MSK 0xF7FF
#define DS1103_HANINC_CT2_LOAD_DISABLE_MSK 0x7FFF
#define DS1103_HANINC_CT1_LMDE2             0x200
#define DS1103_HANINC_CT2_LMDE2            0x2000
#define DS1103_HANINC_CT1_FOURFOLD            0x3
#define DS1103_HANINC_CT2_FOURFOLD           0x30

/******************************************************************************/





/******************************************************************************
 internal definitions for eeprom-access
******************************************************************************/


#define DS1103_EEPROM           0x10000044
#define DS1103_EEPROM_RD                 1
#define DS1103_EEPROM_WR                 0
#define DS1103_TMP_SENSOR             0x0E
#define DS1103_RD_ACCESS              0x20
#define DS1103_EEPROM_RD_TMP        0xAA00
#define DS1103_EEPROM_RD_MAXTMP     0xA100
#define DS1103_GLOBAL_RAM_EEPROM      0x17
#define DS1103_EEPROM_EERDY           0x80
#define DS1103_EEPROM_EERACK          0x40


/******************************************************************************
 internal definitions for STRB-Register
******************************************************************************/

#define DS1103_STRB_INC6   0x8000
#define DS1103_STRB_INC5   0x4000
#define DS1103_STRB_INC4   0x2000
#define DS1103_STRB_INC3   0x1000
#define DS1103_STRB_INC2   0x0800
#define DS1103_STRB_INC1   0x0400
#define DS1103_STRB_DAC      0x0100
#define DS1103_STRB_ADC8   0x0080
#define DS1103_STRB_ADC7   0x0040
#define DS1103_STRB_ADC6   0x0020
#define DS1103_STRB_ADC5   0x0010
#define DS1103_STRB_ADC4   0x0008
#define DS1103_STRB_ADC3   0x0004
#define DS1103_STRB_ADC2   0x0002
#define DS1103_STRB_ADC1   0x0001

/******************************************************************************
 internal definitions for SYNC_CTRL-Register
******************************************************************************/


#define DS1103_SYNC_START_ADC1      0x000001
#define DS1103_SYNC_START_ADC2      0x000002
#define DS1103_SYNC_START_ADC3      0x000004
#define DS1103_SYNC_START_ADC4      0x000008
#define DS1103_SYNC_START_ADC5      0x000010
#define DS1103_SYNC_START_ADC6      0x000020
#define DS1103_SYNC_START_ADC7      0x000040
#define DS1103_SYNC_START_ADC8      0x000080
#define DS1103_SYNC_EXT_STRB_INC1   0x000100
#define DS1103_SYNC_EXT_STRB_INC2   0x000200
#define DS1103_SYNC_EXT_STRB_INC3   0x000400
#define DS1103_SYNC_EXT_STRB_INC4   0x000800
#define DS1103_SYNC_EXT_STRB_INC5   0x001000
#define DS1103_SYNC_EXT_STRB_INC6   0x002000
#define DS1103_SYNC_EXT_STRB_DAC   0x004000
#define DS1103_SYNC_TRIG_EDGE_DAC   0x008000
#define DS1103_SYNC_TRIG_EDGE_ADC   0x010000

#define DS1103_SYNC_TRIGGER_RISING 1
#define DS1103_SYNC_TRIGGER_FALLING 0

#define DS1103_TRIGGER_DISABLE 0
#define DS1103_TRIGGER_ENABLE 1


/******************************************************************************
  other definitions
******************************************************************************/

#define DS1103_OFF 0
#define DS1103_ON  1

#define DS1103_BITSELECT0   0x1
#define DS1103_BITSELECT1   0x2
#define DS1103_BITSELECT2   0x4
#define DS1103_BITSELECT3   0x8
#define DS1103_BITSELECT4  0x10
#define DS1103_BITSELECT5  0x20
#define DS1103_BITSELECT6  0x40

#define DS1103_TRIGGER_DISABLE 0
#define DS1103_TRIGGER_ENABLE 1

/******************************************************************************/



/******************************************************************************
  type definitions
******************************************************************************/


/*****************************************************************************
  type definition for Interrupt Reg. access
*****************************************************************************/


typedef struct {
  volatile UInt32 iir;
  volatile UInt32 imr;
  volatile UInt32 ifr;
}ds1103_interrupt_reg;





/*****************************************************************************
  structure for adc-access
*****************************************************************************/

typedef struct {
    volatile Int16 adc2;
    volatile Int16 adc1;
    volatile Int16 adc4;
    volatile Int16 adc3;  
    volatile Int16 adc6;
    volatile Int16 adc5;  
    volatile Int16 adc8;
    volatile Int16 adc7;  
} ds1103_adc_access;


/*****************************************************************************
  definition of the ADCCTRL - Register structure
*****************************************************************************/


typedef  union {
  struct {
   UInt16   seladc4    :2;
   UInt16   seladc3    :2;
   UInt16   seladc2    :2;
   UInt16   seladc1    :2;
   UInt16   busyadc8   :1;
   UInt16   busyadc7   :1;
   UInt16   busyadc6   :1;
   UInt16   busyadc5   :1;
   UInt16   busyadc4   :1;
   UInt16   busyadc3   :1; 
   UInt16   busyadc2   :1;
   UInt16   busyadc1   :1;
   }_adcctrl_bit;
   UInt16 _adcctrl_reg;      /* ADC control word        */
} ds1103_adcctrl_reg;




/*****************************************************************************
  definition of the IOSTB - Register structure
*****************************************************************************/


/* redsg */
typedef  union {
  struct {
   UInt16     stbinc6    :1;
   UInt16     stbinc5    :1;
   UInt16     stbinc4    :1;
   UInt16     stbinc3    :1;
   UInt16     stbinc2    :1;
   UInt16     stbinc1    :1;
   UInt16     reserved   :1;
   UInt16     upddac     :1;
   UInt16     scadc8     :1;
   UInt16     scadc7     :1;
   UInt16     scadc6     :1;
   UInt16     scadc5     :1; 
   UInt16     scadc4     :1;
   UInt16     scadc3     :1;
   UInt16     scadc2     :1;
   UInt16     scadc1     :1;
     }_iostb_bit;               /* ADC Control bit fields  */
   UInt16 _iostb_reg;           /* ADC control word */
  } ds1103_iostb_reg;





/*****************************************************************************
  definition of the Setup - Register structure
*****************************************************************************/

  
typedef union {
  struct {
   UInt16    rstonidx2              :1;
   UInt16    rstonidx1              :1;
   UInt16    incmode6               :1;
   UInt16    incmode5               :1;
   UInt16    incmode4               :1;
   UInt16    incmode3               :1;
   UInt16    incmode2               :1;
   UInt16    incmode1               :1;
   UInt16    dactr                  :1;
   UInt16    slvboot                :1;
   UInt16    uart2md                :1;
   UInt16    uart1md                :1;
   UInt16    incmode7               :1;
   UInt16    rstonidx5              :1;
   UInt16    rstonidx4              :1;
   UInt16    rstonidx3              :1;
    }_setup_bit;
   UInt16 _setup_reg;          /* Config-register word */
 }ds1103_setup_reg;


/*****************************************************************************
  definition of the SYNC-CTRL - Register structure
*****************************************************************************/

typedef  union {
  struct {
   UInt16   startadc1      :1;
   UInt16   startadc2      :1;
   UInt16   startadc3      :1;
   UInt16   startadc4      :1;
   UInt16   startadc5      :1;
   UInt16   startadc6      :1;
   UInt16   startadc7      :1;
   UInt16   startadc8      :1;
   UInt16   extstrbinc1    :1;
   UInt16   extstrbinc2    :1;
   UInt16   extstrbinc3    :1;
   UInt16   extstrbinc4    :1;
   UInt16   extstrbinc5    :1;
   UInt16   extstrbinc6    :1;
   UInt16   extstrbdac     :1;
   UInt16   trigedgedac    :1;
   UInt16   trigedgeadc    :1;
   UInt16   reserved       :15;
       }_syncctrl_bit;
   UInt32 _syncctrl_reg;      /* SYNC control double word        */
} ds1103_syncctrl_reg;





  
/*****************************************************************************
  definition of the Reset - Register structure
*****************************************************************************/


typedef union {
  struct {
   UInt16   not_used               :2;
   UInt16   delpcacen              :1; 
   UInt16   bootcan                :1; 
   UInt16   digsel                 :4;
   UInt16   reserved               :3;
   UInt16   rstslv                 :1;
   UInt16   rstinc6                :1;
   UInt16   rstcan                 :1;
   UInt16   rstuart                :1;
   UInt16   rstdac                 :1;
    }_rest_bit;
   UInt16 _rest_reg;           /* reset-register word */
 } ds1103_rest_reg;


/*****************************************************************************
  type definition of the Dig. Inc. Reset - Register 
*****************************************************************************/


typedef union {
  struct {
   UInt16   reserved               :10;
   UInt16   rstinc5                :1;
   UInt16   rstinc4                :1;
   UInt16   rstinc3                :1;
   UInt16   rstinc2                :1;
   UInt16   rstinc1                :1;
  }_restinc_bit;
   UInt16 _restinc_reg;           /* resetinc-register word */
 } ds1103_restinc_reg;



/*****************************************************************************
  type definition for eeprom access
*****************************************************************************/


typedef union {
  struct {
  UInt16 eedata   :8;
  UInt16 eerdy    :1;
  UInt16 eeack    :1;
  UInt16 eewr     :1;
  UInt16 eeadr    :5;
    }_eeprom_bit;
  UInt16 _eeprom_reg;
 }ds1103_eeprom_reg;



/*****************************************************************************
  type definition for Dig. Inc. access
*****************************************************************************/

typedef struct {
  volatile Int32 poscnt1;
  volatile Int32 poscnt2;
  volatile Int32 poscnt3;
  volatile Int32 poscnt4;
  volatile Int32 poscnt5;
}ds1103_digenc;
   

 

/*****************************************************************************
  type definitions for the Hanning-Tacho Controller
*****************************************************************************/


typedef struct {
  volatile UInt16 syscon;
  volatile UInt16 cntcon;
  volatile UInt16 encsign;
  volatile Int16  ct1datl;
  volatile Int16  ct1dath;
           UInt16 dummy[8]; 
  volatile Int16  ct2dat;
  volatile UInt16 updatcyc;
  volatile UInt16 lincnt;
  
}ds1103_analenc;


typedef union {
  struct {
  UInt16 ct2len :1;
  UInt16 ct2lm   :3;
  UInt16 ct1len   :1;
  UInt16 ct1lm   :3;
  UInt16 ct2err   :1;
  UInt16 ct2enc   :1;
  UInt16 ct2cm   :2;
  UInt16 ct1err   :1;
  UInt16 ct1enc   :1;
  UInt16 ct1cm   :2;
   }_inc_cntcon_bit;
  UInt16 _inc_cntcon_reg;
 }ds1103_inc_cntcon_reg;


   

typedef union {
  struct {
  UInt16 tstren :1;
  UInt16 tstrin   :1;
  UInt16 tstbin   :1;
  UInt16 tstain   :1;
  UInt16 en2tst   :1;
  UInt16 en1tst   :1;
  UInt16 en2dir   :1;
  UInt16 en1dir   :1;
  UInt16 adbit   :4;
  UInt16 admod   :2;
  UInt16 updflg   :1;
  UInt16 updmod :1;
   }_inc_syscon_bit;
  UInt16 _inc_syscon_reg;
 }ds1103_inc_syscon_reg;


/*****************************************************************************
  DAC access structure
*****************************************************************************/


typedef  struct {
   volatile UInt16 dac2;
   volatile UInt16 dac1;   
   volatile UInt16 dac4;
   volatile UInt16 dac3;
   volatile UInt16 dac6;
   volatile UInt16 dac5;
   volatile UInt16 dac8;
   volatile UInt16 dac7;
      }ds1103_dac_load;


/*****************************************************************************
  external used macros
*****************************************************************************/

/******************************************************************************
*
* Macro:
*    ds1103_external_trigger_enable()
*
* SYNTAX: ds1103_external_trigger_enable()
*
* DESCRIPTION: This macro enables the external trigger
*
* PARAMETERS: none
*
* RETURNS: nothing
*
* NOTE: Restart of ds1103_slave_dsp_pwm3_init() is necessary to disable external trigger.
*
*
******************************************************************************/

#define ds1103_external_trigger_enable() { \
  ds1103_slave_dsp_communication_init();   \
  ds1103_slave_dsp_bit_io_init(0, 2, 0x08, 0x00); }


/*****************************************************************************
  internal used macros
*****************************************************************************/


/******************************************************************************
*
* Macro:
*    ds1103_delay_loop(time)
*
* SYNTAX: ds1103_delay_loop(time)
*
* DESCRIPTION: This macro is used for time-delays.
*
* PARAMETERS: Float64 time (in seconds): defines the delay
*
* RETURNS: nothing
*
* NOTE:
*
******************************************************************************/
#define ds1103_delay_loop(time){ \
RTLIB_TIC_DELAY(time); \
}



/******************************************************************************
*
* Macro:
*    ds1103_inc2_strobe_inp1()
*
*  SYNTAX:
*          ds1103_inc2_strobe_inp1()
*
* DESCRIPTION: This Macro loads Counter 1 of the Hanning-Inrementalencoder
*              interface with the value which is stored in the corresponding
*              latch.
*
* PARAMETERS: no
*
* RETURNS: nothing
*
* NOTE:
*
******************************************************************************/
#define ds1103_inc2_strobe_inp1() { \
    UInt16 cntcon; \
    cntcon = inc_cntcon_reg;  \
    inc_cntcon_reg = ( cntcon & DS1103_HANINC_CT1_LMDE0_MSK ); \
    DS1103_EIEIO;             \
    inc_cntcon_reg = cntcon | DS1103_HANINC_CT1LEN;   \
    DS1103_EIEIO;             \
    *DS1103_IOSTB_REG = (0x400 << ds1103_inc_chan[channel] ); \
    DS1103_EIEIO;  \
    inc_cntcon_reg = cntcon;  \
    DS1103_EIEIO;             \
}






/******************************************************************************
*
* Macro:
*    ds1103_inc2_strobe_inp2()
*
* SYNTAX:
*       ds1103_inc2_strobe_inp2()
*
* DESCRIPTION: This Macro loads Counter 2 of the Hanning-Inrementalencoder
*              interface with the value which is stored in the corresponding
*              latch.
*
* PARAMETERS: no
*
* RETURNS: nothing
*
* NOTE:
*
******************************************************************************/
#define ds1103_inc2_strobe_inp2() { \
    UInt16 cntcon; \
    cntcon = inc_cntcon_reg;  \
    inc_cntcon_reg = ( cntcon & DS1103_HANINC_CT2_LMDE0_MSK ) | DS1103_HANINC_CT2LEN; \
    DS1103_EIEIO;             \
    inc_cntcon_reg = cntcon | DS1103_HANINC_CT2LEN;   \
    DS1103_EIEIO;             \
    *DS1103_IOSTB_REG = (0x400 << ds1103_inc_chan[channel] ); \
    DS1103_EIEIO;  \
    inc_cntcon_reg = cntcon;  \
    DS1103_EIEIO;             \
}




/******************************************************************************
  object declarations
******************************************************************************/

extern Int32 ds1103_fine_count_offs;
extern volatile Int16 ds1103_ppc_temperature;


/******************************************************************************
  function prototypes
******************************************************************************/

#ifdef _INLINE
  #define __INLINE static
#else
  #define __INLINE extern 
#endif


 extern void ds1103_inc_init(UInt16 channel, UInt16 inc_mode);

/**sync***********************************************************************/
 __INLINE void ds1103_syncin_edge_setup(UInt16 edge);
 __INLINE void ds1103_syncout_edge_setup(UInt16 edge);
 __INLINE void ds1103_syncin_trigger(void);
 __INLINE void ds1103_syncout_trigger(void);
/**adc************************************************************************/
 __INLINE void ds1103_adc_start(UInt16 mask);
 __INLINE void ds1103_adc_delayed_start(UInt16 mask);
 __INLINE void ds1103_adc_trigger_setup(UInt16 converter, UInt16 state);
 __INLINE void ds1103_adc_mux(UInt16 channel);
 __INLINE void ds1103_adc_mux_all(UInt16 adc1_chan, UInt16 adc2_chan,
                                  UInt16 adc3_chan, UInt16 adc4_chan);
 __INLINE Float64 ds1103_adc_read_ch(UInt16 channel);
 __INLINE Float64 ds1103_adc_read_conv(UInt16 converter);
 __INLINE void ds1103_adc_read_mux(UInt16* adc_scantable,
                                   UInt16 scantable_size, Float64* pvalues);
 __INLINE void ds1103_adc_read2(UInt16 converter, Float64 *value1,
                                Float64 *value2);
 __INLINE void ds1103_adc_read_all(Float64 *value1, Float64 *value2,
                                   Float64 *value3, Float64 *value4,
                                   Float64 *value5, Float64 *value6,
                                   Float64 *value7, Float64 *value8);
 __INLINE void ds1103_adc_multi_conv_read(UInt32 channel_mask,
                                   Float64 *pvalues);

/**dac************************************************************************/
 __INLINE void ds1103_dac_write(UInt16 channel, Float64 value);
 __INLINE void ds1103_dac_write2(UInt16 channel, Float64 value1,
                                 Float64 value2);
 __INLINE void ds1103_dac_reset(void);
 __INLINE void ds1103_dac_init(UInt16 dac_mode);
 __INLINE void ds1103_dac_strobe(void);
 __INLINE void ds1103_dac_trigger_setup(UInt16 state);

 /**inc************************************************************************/
 __INLINE void ds1103_inc_set_idxmode(UInt16 channel, UInt16 idx_mode);
 __INLINE void ds1103_inc_reset(UInt16 channel);

 __INLINE Float64 ds1103_inc_position_read(UInt16 channel, UInt16 line_subdiv);
 __INLINE Float64 ds1103_inc_position_read_immediately(UInt16 channel, UInt16 line_subdiv);
 
 __INLINE Float64 ds1103_inc_delta_position_read(UInt16 channel,UInt16 line_subdiv);
 __INLINE Float64 ds1103_inc_delta_position_read_immediately(UInt16 channel,UInt16 line_subdiv);

 __INLINE Int64 ds1103_inc_counter_read(UInt16 channel);
 __INLINE Int64 ds1103_inc_counter_read_immediately(UInt16 channel);

 __INLINE Int64 ds1103_inc_counter_rd(UInt16 channel);
 __INLINE Int64 ds1103_inc_counter_rd_immediately(UInt16 channel);

 __INLINE Int16 ds1103_inc_index_read(UInt16 channel, UInt16 reset_enable);
 


 __INLINE void ds1103_inc_position_write(UInt16 channel, Float64 position,
                                         UInt16 line_subdiv);
 __INLINE void ds1103_inc_trigger_setup(UInt16 channel, UInt16 state);
 __INLINE void ds1103_inc_counter_clear(UInt16 channel);
 __INLINE void ds1103_inc_counter_write(UInt16 channel, Int32 count);
 __INLINE void ds1103_inc_ch6ch7_write_latch(UInt16 channel, UInt32 count);
 __INLINE void ds1103_inc_reset_enable(UInt16 channel);
 
 /**bit************************************************************************/
 __INLINE void ds1103_bit_io_init(UInt16 mask);
 __INLINE void ds1103_bit_io_config(UInt16 mask);
 __INLINE void ds1103_bit_io_write(UInt32 value);
 __INLINE void ds1103_bit_io_set(UInt32 mask);
 __INLINE void ds1103_bit_io_clear(UInt32 mask);
 __INLINE UInt32 ds1103_bit_io_read(void);
 
 /**etc************************************************************************/
 __INLINE Int16 ds1103_cpu_temp_read(Int16* temperature);
 __INLINE Int16 ds1103_cpu_temperature(void);

#undef __INLINE

/******************************************************************************
  inclusion of source file(s) for inline expansion
******************************************************************************/

#ifdef _INLINE
  #include <Io1103.c>
#endif

#endif
