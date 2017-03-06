/******************************************************************************
*
* MODULE
*   parameter definitions for both dual port memories on DS1103 board
*
* FILE
*   para1103.h
*
* RELATED FILES
*   def1103.h
*
* DESCRIPTION
*    - parameter definitions for dual port memories PPC <=> SLVDSP and
*      PPC <=> SLVMC
*
* AUTHOR(S)
*   H.-G. Limberg
*
* dSPACE GmbH, Technologiepark 25, 33100 Paderborn, Germany
*
* $RCSfile: para1103.h $ $Revision: 1.2 $ $Date: 2003/08/07 14:54:21GMT+01:00 $
* $ProjectName: e:/ARC/Products/ImplSW/RTLibSW/RTLib1103/Develop/DS1103/DS1103DPMEM.pj $
******************************************************************************/

#ifndef _PARA1103_H_
#define _PARA1103_H_


/******************************************************************************
 function parameter constants for SLVDSP
******************************************************************************/

/* constants for slave DSP -> PPC interrupts */

#define SLVDSP1103_INT_DISABLE  0       /* sub interrupt generation disabled */
#define SLVDSP1103_INT_ENABLE   1        /* sub interrupt generation enabled */

/* constants for ADC input */

#define SLVDSP1103_ADC_CH1_MSK  0x0001                 /* ADC channel 1 mask */
#define SLVDSP1103_ADC_CH2_MSK  0x0002                 /* ADC channel 2 mask */
#define SLVDSP1103_ADC_CH9_MSK  0x0100                 /* ADC channel 9 mask */
#define SLVDSP1103_ADC_CH10_MSK 0x0200                 /* ADC channel 10 mask */

/* constants for digital I/O */

#define SLVDSP1103_BIT_IO_BIT0_MSK 0x01
#define SLVDSP1103_BIT_IO_BIT1_MSK 0x02
#define SLVDSP1103_BIT_IO_BIT2_MSK 0x04
#define SLVDSP1103_BIT_IO_BIT3_MSK 0x08
#define SLVDSP1103_BIT_IO_BIT4_MSK 0x10
#define SLVDSP1103_BIT_IO_BIT5_MSK 0x20
#define SLVDSP1103_BIT_IO_BIT6_MSK 0x40
#define SLVDSP1103_BIT_IO_BIT7_MSK 0x80

#define SLVDSP1103_BIT_IO_BIT0_IN 0x00
#define SLVDSP1103_BIT_IO_BIT1_IN 0x00
#define SLVDSP1103_BIT_IO_BIT2_IN 0x00
#define SLVDSP1103_BIT_IO_BIT3_IN 0x00
#define SLVDSP1103_BIT_IO_BIT4_IN 0x00
#define SLVDSP1103_BIT_IO_BIT5_IN 0x00
#define SLVDSP1103_BIT_IO_BIT6_IN 0x00
#define SLVDSP1103_BIT_IO_BIT7_IN 0x00

#define SLVDSP1103_BIT_IO_BIT0_OUT 0x01
#define SLVDSP1103_BIT_IO_BIT1_OUT 0x02
#define SLVDSP1103_BIT_IO_BIT2_OUT 0x04
#define SLVDSP1103_BIT_IO_BIT3_OUT 0x08
#define SLVDSP1103_BIT_IO_BIT4_OUT 0x10
#define SLVDSP1103_BIT_IO_BIT5_OUT 0x20
#define SLVDSP1103_BIT_IO_BIT6_OUT 0x40
#define SLVDSP1103_BIT_IO_BIT7_OUT 0x80

/* constants for simple PWM generation */

#define SLVDSP1103_PWM_MODE_ASYM 0                    /* asymmetric PWM mode */
#define SLVDSP1103_PWM_MODE_SYM 1                      /* symmetric PWM mode */

#define SLVDSP1103_PWM_POL_LOW  0          /* low active PWM output polarity */
#define SLVDSP1103_PWM_POL_HIGH 1         /* high active PWM output polarity */

#define SLVDSP1103_PWM_CH1_MSK  0x01                   /* PWM channel 1 mask */
#define SLVDSP1103_PWM_CH2_MSK  0x02                   /* PWM channel 2 mask */
#define SLVDSP1103_PWM_CH3_MSK  0x04                   /* PWM channel 3 mask */
#define SLVDSP1103_PWM_CH4_MSK  0x08                   /* PWM channel 4 mask */

#define SLVDSP1103_PWM_TTL_LOW      0x0           /* set PWM outputs TTL low */
#define SLVDSP1103_PWM_ACTIVE_LOW   0x1        /* set PWM outputs active low */
#define SLVDSP1103_PWM_ACTIVE_HIGH  0x2       /* set PWM outputs active high */
#define SLVDSP1103_PWM_TTL_HIGH     0x3          /* set PWM outputs TTL high */

/* constants for 3-phase PWM generation */

#define SLVDSP1103_PWM3_SYNC_OFF     0            /* sync interrupt disabled */
#define SLVDSP1103_PWM3_SYNC_LEFT  1.0        /* sync interrupt left aligned */
#define SLVDSP1103_PWM3_SYNC_CENT  0xFFFF         /* sync interrupt centered */

#define SLVDSP1103_PWM3_TTL_LOW      0x0          /* set PWM outputs TTL low */
#define SLVDSP1103_PWM3_ACTIVE_LOW   0x1       /* set PWM outputs active low */
#define SLVDSP1103_PWM3_ACTIVE_HIGH  0x2      /* set PWM outputs active high */
#define SLVDSP1103_PWM3_TTL_HIGH     0x3         /* set PWM outputs TTL high */

#define SLVDSP1103_PWM3_INT_ON       0x1            /* enables PWM interrupt */
#define SLVDSP1103_PWM3_INT_OFF      0x0           /* disables PWM interrupt */

/* constants for D/F square wave generation */

#define SLVDSP1103_D2F_CH4_DISABLE 0               /* D/F channel 4 disabled */
#define SLVDSP1103_D2F_CH4_ENABLE 1                 /* D/F channel 4 enabled */

/* constants for F/D frequency measurement */

#define SLVDSP1103_F2D_OLD      0                         /* no event change */
#define SLVDSP1103_F2D_NEW      1                      /* new event captured */

/* constants for PWM measurement */

#define SLVDSP1103_PWM2D_OLD    0                         /* no event change */
#define SLVDSP1103_PWM2D_NEW    1                      /* new event captured */

/* constants for SCI communication init */

#define SLVDSP1103_SCI_RS442    0           /* set serial port to RS422 mode */
#define SLVDSP1103_SCI_RS232    1           /* set serial port to RS232 mode */

#define SLVDSP1103_SCI_NOPAR       0x00                         /* no parity */
#define SLVDSP1103_SCI_ODDPAR      0x20                        /* odd parity */
#define SLVDSP1103_SCI_EVENPAR     0x60                       /* even parity */

#define SLVDSP1103_SCI_1_STOPBIT   0x00                         /* 1 stopbit */
#define SLVDSP1103_SCI_2_STOPBIT   0x80                        /* 2 stopbits */

/* constants for SPI communication init */

#define SLVDSP1103_SPI_MAXBAUD     2.5e6        /* max. baudrate is 2.5 Mbps */
#define SLVDSP1103_SPI_MASTER      0x0004           /* specify SPI as master */
#define SLVDSP1103_SPI_SLAVE       0x0000            /* specify SPI as slave */
#define SLVDSP1103_SPI_CLKPOL_RISE 0x0000   /* data is output on rising edge */
#define SLVDSP1103_SPI_CLKPOL_FALL 0x0040              /* or on falling edge */
#define SLVDSP1103_SPI_CLKPHS_WOD  0x0000    /* data is output without delay */
#define SLVDSP1103_SPI_CLKPHS_WD   0x0008    /* or with one half-cycle delay */


/******************************************************************************
 function parameter constants for SLVMC
******************************************************************************/

/* constants for slave MC -> PPC interrupts */

#define DEF1103_INT_DISABLE  0          /* sub interrupt generation disabled */
#define DEF1103_INT_ENABLE   1           /* sub interrupt generation enabled */


/*---------------------------------------------*/
#define DEF1103_CAN_WARN_LEVEL        160 /* warning if buffer is 70% filled */
#define DEF1103_CAN_SUBINT_BUFFERWARN 15
#define DEF1103_CAN_BUFFERWARN_QUEUE   0


#endif
