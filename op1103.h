/******************************************************************************
*
* MODULE
*   opcode definitions for both dual port memories on DS1103 board
*
* FILE
*   op1103.h
*
* RELATED FILES
*   def1103.h
*
* DESCRIPTION
*    - opcode definitions for dual port memories PPC <=> SLVDSP and
*      PPC <=> SLVMC
*
* AUTHOR(S)
*   H.-G. Limberg, A. Gropengiesser, M. Heier
*
* dSPACE GmbH, Technologiepark 25, 33100 Paderborn, Germany
*
* $RCSfile: op1103.h $ $Revision: 1.4 $ $Date: 2008/10/08 10:12:28GMT+01:00 $
* $ProjectName: e:/ARC/Products/ImplSW/RTLibSW/RTLib1103/Develop/DS1103/DS1103DPMEM.pj $
******************************************************************************/

#ifndef _OP1103_H_
#define _OP1103_H_


/******************************************************************************
* SLVDSP opcodes and command parameters
******************************************************************************/

/*--- command OP-code definitions -------------------------------------------*/

#define DSPDEF_FWREV_READ       0x0010             /* firmware revision read */
#define DSPDEF_USRFWREV_READ    0x0011        /* user firmware revision read */

#define DSPDEF_ADC_INIT         0x0020                 /* ADC initialization */
#define DSPDEF_ADC_READ         0x0021                           /* ADC read */
#define DSPDEF_ADC2_READ        0x0022                 /* 2-channel ADC read */

#define DSPDEF_PIO_INIT         0x0030             /* bit I/O initialization */
#define DSPDEF_PIO_READ         0x0031                       /* bit I/O read */
#define DSPDEF_PIO_WRITE        0x0032                      /* bit I/O write */
#define DSPDEF_PIO_SET          0x0033                      /* bit I/O write */
#define DSPDEF_PIO_CLR          0x0034                      /* bit I/O write */

#define DSPDEF_PWM_INIT         0x0040          /* simple PWM initialization */
#define DSPDEF_PWM_WRITE        0x0041       /* simple PWM duty cycle update */
#define DSPDEF_PWM_START        0x0042                   /* simple PWM start */
#define DSPDEF_PWM_STOP         0x0043                    /* simple PWM stop */
#define DSPDEF_PWM_OUT_SET      0x0044                     /* PWM output set */

#define DSPDEF_PWM3_INIT        0x0050         /* 3-phase PWM initialization */
#define DSPDEF_PWM3_WRITE       0x0051      /* 3-phase PWM duty cycle update */
#define DSPDEF_PWM3SV_INIT      0x0052    /* space vector PWM initialization */
#define DSPDEF_PWM3SV_WRITE     0x0053 /* space vector PWM duty cycle update */
#define DSPDEF_PWM3_START       0x0054 /* 3-phase and space vector PWM start */
#define DSPDEF_PWM3_STOP        0x0055  /* 3-phase and space vector PWM stop */
#define DSPDEF_PWM3_OUT_SET     0x0056              /* 3-phase PWM ouput set */

#define DSPDEF_D2F_INIT         0x0060                 /* D/F initialization */
#define DSPDEF_D2F_WRITE        0x0061               /* D/F frequency update */

#define DSPDEF_F2D_INIT         0x0070                 /* F/D initialization */
#define DSPDEF_F2D_READ         0x0071                           /* F/D read */

#define DSPDEF_PWM2D_INIT       0x0080     /* PWM measurement initialization */
#define DSPDEF_PWM2D_READ       0x0081               /* PWM measurement read */

#define DSPDEF_SCI_INIT         0x0090                 /* sci initialization */
#define DSPDEF_SCI_WRITE        0x0091             /* data byte write to sci */
#define DSPDEF_SCI_READ         0x0092            /* data byte read from sci */

#define DSPDEF_SPI_INIT         0x0100                 /* spi initialization */
#define DSPDEF_SPI_WRITE        0x0101             /* data byte write to spi */
#define DSPDEF_SPI_READ         0x0102            /* data byte read from spi */

#define DSPDEF_SUBINT_REQUEST   0x0110            /* requests a subinterrupt */


/*--- command parameter definitions -----------------------------------------*/

/* firmware revision read */

#define DSPDEF_FWREV_READ_PCNT    0                       /* parameter count */
#define DSPDEF_FWREV_READ_DINCNT  1                      /* input data count */
#define DSPDEF_FWREV_READ_DOUTCNT 0                     /* output data count */

/* user firmware revision read */

#define DSPDEF_USRFWREV_READ_PCNT    0                    /* parameter count */
#define DSPDEF_USRFWREV_READ_DINCNT  3                   /* input data count */
#define DSPDEF_USRFWREV_READ_DOUTCNT 0                  /* output data count */

/* ADC input */

#define DSPDEF_ADC_INIT_PCNT    1                         /* parameter count */
#define DSPDEF_ADC_INIT_MSK_LOC 0                      /* pin selection mask */

#define DSPDEF_ADC_READ_PCNT    2                         /* parameter count */
#define DSPDEF_ADC_READ_CH_LOC  0                          /* channel number */
#define DSPDEF_ADC_READ_INT_LOC 1                    /* sub-interrupt number */
#define DSPDEF_ADC_READ_DINCNT  1                        /* input data count */
#define DSPDEF_ADC_READ_DOUTCNT 0                          /* no output data */

#define DSPDEF_ADC2_READ_PCNT    3                        /* parameter count */
#define DSPDEF_ADC2_READ_CH1_LOC 0                    /* ADC1 channel number */
#define DSPDEF_ADC2_READ_CH2_LOC 1                    /* ADC2 channel number */
#define DSPDEF_ADC2_READ_INT_LOC 2                   /* sub-interrupt number */
#define DSPDEF_ADC2_READ_DINCNT  2                       /* input data count */
#define DSPDEF_ADC2_READ_DOUTCNT 0                         /* no output data */

/* digital I/O */

#define DSPDEF_PIO_INIT_PCNT        3                     /* parameter count */
#define DSPDEF_PIO_INIT_CH_LOC      0                      /* channel number */
#define DSPDEF_PIO_INIT_SEL_MSK_LOC 1                  /* pin selection mask */
#define DSPDEF_PIO_INIT_DIR_MSK_LOC 2                 /* data direction mask */

#define DSPDEF_PIO_READ_PCNT    1                         /* parameter count */
#define DSPDEF_PIO_READ_CH_LOC  0                          /* channel number */
#define DSPDEF_PIO_READ_DINCNT  1                        /* input data count */
#define DSPDEF_PIO_READ_DOUTCNT 0                          /* no output data */

#define DSPDEF_PIO_WRITE_PCNT    1                        /* parameter count */
#define DSPDEF_PIO_WRITE_CH_LOC  0                         /* channel number */
#define DSPDEF_PIO_WRITE_DINCNT  0                          /* no input data */
#define DSPDEF_PIO_WRITE_DOUTCNT 1                      /* output data count */

#define DSPDEF_PIO_SET_PCNT     1                         /* parameter count */
#define DSPDEF_PIO_SET_CH_LOC   0                          /* channel number */
#define DSPDEF_PIO_SET_DINCNT   0                           /* no input data */
#define DSPDEF_PIO_SET_DOUTCNT  1                       /* output data count */

#define DSPDEF_PIO_CLR_PCNT     1                         /* parameter count */
#define DSPDEF_PIO_CLR_CH_LOC   0                          /* channel number */
#define DSPDEF_PIO_CLR_DINCNT   0                           /* no input data */
#define DSPDEF_PIO_CLR_DOUTCNT  1                       /* output data count */

/* simple PWM generation */

#define DSPDEF_PWM_INIT_PCNT    6                         /* parameter count */
#define DSPDEF_PWM_INIT_PRD_LOC 0                              /* PWM period */
#define DSPDEF_PWM_INIT_DTY_LOC 1                          /* PWM duty cycle */
#define DSPDEF_PWM_INIT_SCL_LOC 2                  /* timer clock prescaling */
#define DSPDEF_PWM_INIT_MOD_LOC 3                                /* PWM mode */
#define DSPDEF_PWM_INIT_POL_LOC 4                     /* PWM output polarity */
#define DSPDEF_PWM_INIT_MSK_LOC 5                            /* channel mask */

#define DSPDEF_PWM_WRITE_PCNT    1                        /* parameter count */
#define DSPDEF_PWM_WRITE_CH_LOC  0                         /* channel number */
#define DSPDEF_PWM_WRITE_DINCNT  0                          /* no input data */
#define DSPDEF_PWM_WRITE_DOUTCNT 1                      /* output data count */

#define DSPDEF_PWM_START_PCNT    1                        /* parameter count */
#define DSPDEF_PWM_START_MSK_LOC 0                           /* channel mask */

#define DSPDEF_PWM_STOP_PCNT    1                         /* parameter count */
#define DSPDEF_PWM_STOP_MSK_LOC 0                            /* channel mask */

#define DSPDEF_PWM_OUT_SET_PCNT       2                   /* parameter count */
#define DSPDEF_PWM_OUT_SET_MSK_LOC    0                      /* channel mask */
#define DSPDEF_PWM_OUT_SET_OS_LOC     1                  /* PWM output state */

/* 3-phase PWM generation */

#define DSPDEF_PWM3_INIT_PCNT        8                    /* parameter count */
#define DSPDEF_PWM3_INIT_PRD_LOC     0                         /* PWM period */
#define DSPDEF_PWM3_INIT_DTY1_LOC    1                 /* phase 1 duty cycle */
#define DSPDEF_PWM3_INIT_DTY2_LOC    2                 /* phase 2 duty cycle */
#define DSPDEF_PWM3_INIT_DTY3_LOC    3                 /* phase 3 duty cycle */
#define DSPDEF_PWM3_INIT_SCL_LOC     4             /* timer clock prescaling */
#define DSPDEF_PWM3_INIT_PRDDB_LOC   5                   /* dead-band period */
#define DSPDEF_PWM3_INIT_SCLDB_LOC   6         /* dead-band clock prescaling */
#define DSPDEF_PWM3_INIT_SYNCPOS_LOC 7            /* sync interrupt position */

#define DSPDEF_PWM3_WRITE_PCNT    0                       /* parameter count */
#define DSPDEF_PWM3_WRITE_DINCNT  0                         /* no input data */
#define DSPDEF_PWM3_WRITE_DOUTCNT 3                     /* output data count */

/* 3-phase space vector PWM generation */

#define DSPDEF_PWM3SV_INIT_PCNT        8                  /* parameter count */
#define DSPDEF_PWM3SV_INIT_PRD_LOC     0                       /* PWM period */
#define DSPDEF_PWM3SV_INIT_T1_LOC      1                 /* time period t1/2 */
#define DSPDEF_PWM3SV_INIT_T12_LOC     2          /* time period t1/2 + t2/2 */
#define DSPDEF_PWM3SV_INIT_SEC_LOC     3                           /* sector */
#define DSPDEF_PWM3SV_INIT_SCL_LOC     4           /* timer clock prescaling */
#define DSPDEF_PWM3SV_INIT_PRDDB_LOC   5                 /* dead-band period */
#define DSPDEF_PWM3SV_INIT_SCLDB_LOC   6       /* dead-band clock prescaling */
#define DSPDEF_PWM3SV_INIT_SYNCPOS_LOC 7          /* sync interrupt position */

#define DSPDEF_PWM3SV_WRITE_PCNT    0                     /* parameter count */
#define DSPDEF_PWM3SV_WRITE_DINCNT  0                       /* no input data */
#define DSPDEF_PWM3SV_WRITE_DOUTCNT 3                   /* output data count */

#define DSPDEF_PWM3_START_PCNT  0                         /* parameter count */

#define DSPDEF_PWM3_STOP_PCNT   0                         /* parameter count */

#define DSPDEF_PWM3_OUT_SET_PCNT       2                  /* parameter count */
#define DSPDEF_PWM3_OUT_SET_MSK_LOC    0          /* 3-phase PWM output mask */
#define DSPDEF_PWM3_OUT_SET_INT_LOC    1       /* 3-phase PWM interrupt mode */

/* D/F square wave generation */

#define DSPDEF_D2F_INIT_PCNT    3                         /* parameter count */
#define DSPDEF_D2F_INIT_PRD_LOC 0                      /* signal half period */
#define DSPDEF_D2F_INIT_SCL_LOC 1                  /* timer clock prescaling */
#define DSPDEF_D2F_INIT_CH4_LOC 2                    /* D/F channel 4 enable */

#define DSPDEF_D2F_WRITE_PCNT    1                        /* parameter count */
#define DSPDEF_D2F_WRITE_CH_LOC  0                         /* channel number */
#define DSPDEF_D2F_WRITE_DINCNT  0                          /* no input data */
#define DSPDEF_D2F_WRITE_DOUTCNT 1                      /* output data count */

/* F/D frequency measurement */

#define DSPDEF_F2D_INIT_PCNT     4                        /* parameter count */
#define DSPDEF_F2D_INIT_MAX1_LOC 0                  /* max timer ticks (msw) */
#define DSPDEF_F2D_INIT_MAX2_LOC 1                  /* max timer ticks (msw) */
#define DSPDEF_F2D_INIT_MAX3_LOC 2                  /* max timer ticks (msw) */
#define DSPDEF_F2D_INIT_MAX4_LOC 3                  /* max timer ticks (msw) */

#define DSPDEF_F2D_READ_PCNT    2                         /* parameter count */
#define DSPDEF_F2D_READ_CH_LOC  0                          /* channel number */
#define DSPDEF_F2D_READ_INT_LOC 1                    /* sub-interrupt number */
#define DSPDEF_F2D_READ_DINCNT  4                        /* input data count */
#define DSPDEF_F2D_READ_DOUTCNT 0                          /* no output data */

/* PWM measurement */

#define DSPDEF_PWM2D_INIT_PCNT  0                         /* parameter count */

#define DSPDEF_PWM2D_READ_PCNT    2                       /* parameter count */
#define DSPDEF_PWM2D_READ_CH_LOC  0                        /* channel number */
#define DSPDEF_PWM2D_READ_INT_LOC 1                  /* sub-interrupt number */
#define DSPDEF_PWM2D_READ_DINCNT  5                      /* input data count */
#define DSPDEF_PWM2D_READ_DOUTCNT 0                        /* no output data */

/* sci serial interface */

#define DSPDEF_SCI_INIT_PCNT        4                     /* parameter count */
#define DSPDEF_SCI_INIT_BAUD_LOC    0                            /* baudrate */
#define DSPDEF_SCI_INIT_PAR_LOC     1                /* kind of parity check */
#define DSPDEF_SCI_INIT_DATAB_LOC   2                  /* number of databits */
#define DSPDEF_SCI_INIT_STOPB_LOC   3                  /* number of stopbits */

#define DSPDEF_SCI_WRITE_PCNT    0                        /* parameter count */
#define DSPDEF_SCI_WRITE_DINCNT  0                          /* no input data */
#define DSPDEF_SCI_WRITE_DOUTCNT 1                      /* output data count */

#define DSPDEF_SCI_READ_PCNT    0                         /* parameter count */
#define DSPDEF_SCI_READ_DINCNT 18                        /* input data count */
#define DSPDEF_SCI_READ_DOUTCNT 0                          /* no output data */

/* spi serial interface */

#define DSPDEF_SPI_INIT_PCNT       5                      /* parameter count */
#define DSPDEF_SPI_INIT_MODE_LOC   0                    /* master/slave mode */
#define DSPDEF_SPI_INIT_BAUD_LOC   1                             /* baudrate */
#define DSPDEF_SPI_INIT_DATAB_LOC  2                   /* number of databits */
#define DSPDEF_SPI_INIT_CLKPOL_LOC 3               /* kind of clock polarity */
#define DSPDEF_SPI_INIT_CLKPHS_LOC 4                  /* kind of clock phase */

#define DSPDEF_SPI_WRITE_PCNT    0                        /* parameter count */
#define DSPDEF_SPI_WRITE_DINCNT  0                          /* no input data */
#define DSPDEF_SPI_WRITE_DOUTCNT 1                      /* output data count */

#define DSPDEF_SPI_READ_PCNT    0                         /* parameter count */
#define DSPDEF_SPI_READ_DINCNT 18                        /* input data count */
#define DSPDEF_SPI_READ_DOUTCNT 0                          /* no output data */

/* subint request */

#define DSPDEF_SUBINT_REQUEST_PCNT       1                /* parameter count */
#define DSPDEF_SUBINT_REQUEST_SUBINT_LOC 0           /* sub-interrupt number */

/******************************************************************************
* SLVMC opcodes and command parameters
******************************************************************************/

/*--- command OP-code definitions -------------------------------------------*/

#define DEF1103_CAN_MEM_READ              0x0012          /* RAM read */
#define DEF1103_CAN_MEM_WRITE             0x0015          /* RAM write */
#define DEF1103_CAN_TX_MSG_REG            0x0017          /* TX-Msg register */
#define DEF1103_CAN_TX_MSG_SLEEP          0x0018          /* TX-Msg sleep */
#define DEF1103_CAN_TX_MSG_WAKEUP         0x0019          /* TX-Msg wakeup */
#define DEF1103_CAN_RM_MSG_REG            0x001A          /* RM-Msg register */
#define DEF1103_CAN_RM_MSG_SLEEP          0x001B          /* RM-Msg sleep */
#define DEF1103_CAN_RM_MSG_WAKEUP         0x001C          /* RM-Msg wakeup */
#define DEF1103_CAN_RX_MSG_REG            0x001E          /* RX-Msg register */
#define DEF1103_CAN_RX_MSG_SLEEP          0x001F          /* RX-Msg sleep */
#define DEF1103_CAN_RX_MSG_WAKEUP         0x0020          /* RX-Msg wakeup */
#define DEF1103_CAN_RQTX_MSG_REG          0x0021          /* RQ-Msg register */
#define DEF1103_CAN_RQTX_MSG_SLEEP        0x0022          /* RQ-Msg sleep */
#define DEF1103_CAN_RQTX_MSG_WAKEUP       0x0023          /* RQ-Msg wakeup */
#define DEF1103_CAN_MSG_ALL_SLEEP         0x0024          /* Msg all sleep */
#define DEF1103_CAN_MSG_ALL_WAKEUP        0x0025          /* Msg all wakeup */
#define MCDEF_STDMASK_SET                 0x0026          /* RX Standard mask set */
#define MCDEF_EXTMASK_SET                 0x0027          /* RX Extended mask set */
#define MCDEF_STDMASK_READ_REG            0x0028          /* Standard mask read */
#define MCDEF_EXTMASK_READ_REG            0x0029          /* Extended mask read */
#define DEF1103_CAN_MSGID_READ            0x002A          /* Msg Identifier read */
#define DEF1103_CAN_MSG_PROC_REG          0x002B          /* Msg processed register */
#define MCDEF_RQTX_MSG_PROC_REG           0x002C          /* RQ-Msg transmit-processed register */
#define MCDEF_MSG_TRIGGER                 0x002D          /* Msg trigger */
#define DEF1103_CAN_BUSOFF_GO             0x002E          /* go busoff */
#define DEF1103_CAN_BUSOFF_RETURN         0x002F          /* busoff return */
#define DEF1103_CAN_BUSSTATUS_READ_REG    0x0030          /* busstatus read  */
#define DEF1103_CAN_STUFFBIT_ERR_REG      0x0031          /* stuffbit error read  */
#define DEF1103_CAN_FORM_ERR_REG          0x0032          /* form error read  */
#define DEF1103_CAN_ACK_ERR_REG           0x0033          /* acknowledge error read  */
#define DEF1103_CAN_BIT1_ERR_REG          0x0034          /* bit1 error read  */
#define DEF1103_CAN_BIT0_ERR_REG          0x0035          /* bit0 error read  */
#define DEF1103_CAN_CRC_ERR_REG           0x0036          /* crc error read  */
#define DEF1103_CAN_TX_OK_REG             0x0037          /* Tx-ok read  */
#define DEF1103_CAN_RX_OK_REG             0x0038          /* Rx-ok read  */
#define DEF1103_CAN_RX_LOST_REG           0x0039          /* Rx-lost read  */
#define DEF1103_CAN_TX_MSG_WRITE          0x003A          /* Tx-write */
#define DEF1103_CAN_RM_MSG_WRITE          0x003B          /* RM-write */
#define MCDEF_INIT_INFO                   0x003C          /* init-info */
#define DEF1103_CAN_RQRX_MSG_REG          0x003D          /* RQRX-Msg register */
#define DEF1103_CAN_RQRX_MSG_PROC_REG     0x003E
#define MCDEF_RQTX_MSG_TRIGGER            0x003F          /* rq_tx-msg_trigger */
#define MCDEF_RQTX_MSG_ACTIVATE           0x0040
#define DEF1103_CAN_RQRX_MSG_SLEEP        0x0041          /* RQRX-Msg sleep */
#define DEF1103_CAN_RQRX_MSG_WAKEUP       0x0042          /* RQRX-Msg wakeup */
#define DEF1103_CAN_BUFFERWARN_ENABLE     0x0043
#define DEF1103_CAN_BUSOFF_ENABLE         0x0044
#define DEF1103_CAN_GET_VERSION           0x0045
#define MCDEF_STDARBMASK_SET              0x0046          /* RX ARB Standard mask set */
#define MCDEF_EXTARBMASK_SET              0x0047          /* RX ARB Extended mask set */
#define DEF1103_CAN_INIT_INFO             0x0050          /* init-info */
#define DEF1103_CAN_START_CONTR           0x0051          /* Start CAN controller */
#define DEF1103_CAN_TX_MSG_SEND           0x0052          /* TX-Msg send */
#define DEF1103_CAN_DATA_LOST_REG         0x0053
#define DEF1103_CAN_MAILBOX_ERROR_REG     0x0054
#define DEF1103_CAN_MSGMASK15_READ_REG    0x0055
#define DEF1103_CAN_SUBINT_REQUEST        0x0056          /* requests a subinterrupt */
#define DEF1103_CAN_STDMASK_READ_REG      0x0057          /* Standard mask read */
#define DEF1103_CAN_EXTMASK_READ_REG      0x0058          /* Extended mask read */
#define DEF1103_CAN_MASK15_SET            0x0059          /* MB15 Standard mask set */
#define DEF1103_CAN_ARBMASK15_SET         0x005A          /* MB15 Extended mask set */
#define DEF1103_CAN_MSG_TRIGGER           0x005B          /* Msg trigger */
#define DEF1103_CAN_RQTX_MSG_TRIGGER      0x005C          /* RQTX-Msg trigger */

#define DEF1103_CAN_MSG_SET               0x005D
#define DEF1103_CAN_MSG_SEND_ID           0x005E
#define DEF1103_CAN_SERV_MULTI            0x005F

#define DEF1103_CAN_TXQUEUE_INIT          0x0060          /* send queue init */
#define DEF1103_CAN_TXQUEUE_SEND_ID       0x0061          /* send queue */
#define DEF1103_CAN_BAUDRATE_SET          0x0062          /* set baudrate */
#define DEF1103_CAN_TXQUEUE_CLEAR         0x0063          /* clear send queue */

/*--- command parameter definitions -----------------------------------------*/

#define DEF1103_CAN_MSG_MASK                              1
#define DEF1103_CAN_MSG_PERIOD                            2
#define DEF1103_CAN_MSG_ID                                4
#define DEF1103_CAN_MSG_QUEUE                             8
#define DEF1103_CAN_MSG_DSMCR_BUFFER                     16
#define DEF1103_CAN_MSG_LEN                              32

#define DEF1103_CAN_NO_INFO          0
#define DEF1103_CAN_DATA_INFO        1
#define DEF1103_CAN_MSG_INFO         2     /* delivers identifier, format */
#define DEF1103_CAN_PROCESSED_INFO   4     /* delivers processed-flag */
#define DEF1103_CAN_TIMECOUNT_INFO   8     /* delivers Timecount */
#define DEF1103_CAN_DELTATIME_INFO  16     /* delivers Deltacount */
#define DEF1103_CAN_DELAYTIME_INFO  32     /* delivers Delaycount */


/* timer sync */

#define DEF1103_CAN_TIMERSYNC_TIMECLO_LOC       0 
#define DEF1103_CAN_TIMERSYNC_TIMECHI_LOC       1
#define DEF1103_CAN_TIMERSYNC_WRAPC_LOC         2
#define DEF1103_CAN_TIMERSYN_SUBINT             0


/* RAM access */

/* data memory access */

#define DEF1103_CAN_MEM_READ_PCNT               2        /* parameter count */
#define DEF1103_CAN_MEM_READ_ADDR_LOC           0        /* memory address  */
#define DEF1103_CAN_MEM_READ_CNT_LOC            1        /* input data count*/
#define DEF1103_CAN_MEM_READ_DOUTCNT            0        /* no output data */

#define DEF1103_CAN_MEM_WRITE_PCNT              2        /* parameter count */
#define DEF1103_CAN_MEM_WRITE_ADDR_LOC          0        /* memory address */
#define DEF1103_CAN_MEM_WRITE_CNT_LOC           1        /* output data count */
#define DEF1103_CAN_MEM_WRITE_DINCNT            0        /* no input data */


/* init-info */

#define DEF1103_CAN_INIT_INFO_CLCKL_LOC         0
#define DEF1103_CAN_INIT_INFO_CLCKH_LOC         1
#define DEF1103_CAN_INIT_INFO_BTR0_LOC          2
#define DEF1103_CAN_INIT_INFO_BTR1_LOC          3
#define DEF1103_CAN_INIT_INFO_BOFF_INTNO_LOC    4
#define DEF1103_CAN_INIT_INFO_MB15FORMAT        5
#define DEF1103_CAN_INIT_INFO_PCNT              6


/* TX-Msg register */

#define DEF1103_CAN_TX_MSG_REG_IDENTL_LOC       0
#define DEF1103_CAN_TX_MSG_REG_IDENTH_LOC       1
#define DEF1103_CAN_TX_MSG_REG_FORMAT_LOC       2
#define DEF1103_CAN_TX_MSG_REG_STARTL_LOC       3
#define DEF1103_CAN_TX_MSG_REG_STARTH_LOC       4
#define DEF1103_CAN_TX_MSG_REG_REPETL_LOC       5
#define DEF1103_CAN_TX_MSG_REG_REPETH_LOC       6
#define DEF1103_CAN_TX_MSG_REG_TIMEOUTL_LOC     7
#define DEF1103_CAN_TX_MSG_REG_TIMEOUTH_LOC     8
#define DEF1103_CAN_TX_MSG_REG_INFORM_LOC       9
#define DEF1103_CAN_TX_MSG_REG_INTNO_LOC       10
#define DEF1103_CAN_TX_MSG_REG_MSGNO_LOC       11
#define DEF1103_CAN_TX_MSG_REG_DOUTCNT          0
#define DEF1103_CAN_TX_MSG_REG_PCNT            12


/* RM-Msg register */

#define DEF1103_CAN_RM_MSG_REG_IDENTL_LOC       0
#define DEF1103_CAN_RM_MSG_REG_IDENTH_LOC       1
#define DEF1103_CAN_RM_MSG_REG_FORMAT_LOC       2
#define DEF1103_CAN_RM_MSG_REG_INFORM_LOC       3
#define DEF1103_CAN_RM_MSG_REG_INTNO_LOC        4
#define DEF1103_CAN_RM_MSG_REG_MSGNO_LOC        5
#define DEF1103_CAN_RM_MSG_REG_DOUTCNT          0
#define DEF1103_CAN_RM_MSG_REG_PCNT             6


/* RX-Msg register */

#define DEF1103_CAN_RX_MSG_REG_IDENTL_LOC       0
#define DEF1103_CAN_RX_MSG_REG_IDENTH_LOC       1
#define DEF1103_CAN_RX_MSG_REG_FORMAT_LOC       2
#define DEF1103_CAN_RX_MSG_REG_INFORM_LOC       3
#define DEF1103_CAN_RX_MSG_REG_INTNO_LOC        4
#define DEF1103_CAN_RX_MSG_REG_MSGNO_LOC        5
#define DEF1103_CAN_RX_MSG_REG_DOUTCNT          0
#define DEF1103_CAN_RX_MSG_REG_PCNT             6


/* RQ-Msg register */

#define DEF1103_CAN_RQTX_MSG_REG_IDENTL_LOC     0
#define DEF1103_CAN_RQTX_MSG_REG_IDENTH_LOC     1
#define DEF1103_CAN_RQTX_MSG_REG_FORMAT_LOC     2
#define DEF1103_CAN_RQTX_MSG_REG_STARTL_LOC     3
#define DEF1103_CAN_RQTX_MSG_REG_STARTH_LOC     4
#define DEF1103_CAN_RQTX_MSG_REG_REPETL_LOC     5
#define DEF1103_CAN_RQTX_MSG_REG_REPETH_LOC     6
#define DEF1103_CAN_RQTX_MSG_REG_TIMEOUTL_LOC   7
#define DEF1103_CAN_RQTX_MSG_REG_TIMEOUTH_LOC   8
#define DEF1103_CAN_RQTX_MSG_REG_TXINFORM_LOC   9
#define DEF1103_CAN_RQTX_MSG_REG_TXINTNO_LOC    10
#define DEF1103_CAN_RQTX_MSG_REG_MSGNO_LOC      11
#define DEF1103_CAN_RQTX_MSG_REG_DOUTCNT         0
#define DEF1103_CAN_RQTX_MSG_REG_PCNT           12


/* RQ-tx-Msg activate */

#define MCDEF_RQTX_MSG_ACTIVATE_PCNT      1
#define MCDEF_RQTX_MSG_ACTIVATE_IDX_LOC   0  


/* RQ-rx-Msg register */

#define DEF1103_CAN_RQRX_MSG_REG_RXINFORM_LOC   0
#define DEF1103_CAN_RQRX_MSG_REG_RXINTNO_LOC    1
#define DEF1103_CAN_RQRX_MSG_REG_MSGNO_LOC      2
#define DEF1103_CAN_RQRX_MSG_REG_DOUTCNT        0
#define DEF1103_CAN_RQRX_MSG_REG_PCNT           3


/* Msg write */

#define DEF1103_CAN_MSG_WRITE_PCNT              6
#define DEF1103_CAN_MSG_WRITE_PCNT2             2
#define DEF1103_CAN_MSG_WRITE_IDX_LOC           0
#define DEF1103_CAN_MSG_WRITE_DATALEN_LOC       1
#define DEF1103_CAN_MSG_WRITE_DATA_LOC          2


/* Msg send */

#define DEF1103_CAN_MSG_SEND_PCNT               8
#define DEF1103_CAN_MSG_SEND_PCNT2              4
#define DEF1103_CAN_MSG_SEND_IDX_LOC            0
#define DEF1103_CAN_MSG_SEND_DATALEN_LOC        1
#define DEF1103_CAN_MSG_SEND_DELAYL_LOC         2
#define DEF1103_CAN_MSG_SEND_DELAYH_LOC         3
#define DEF1103_CAN_MSG_SEND_DATA_LOC           4


/* Msg trigger */

#define DEF1103_CAN_MSG_TRIGGER_PCNT            3
#define DEF1103_CAN_MSG_TRIGGER_IDX_LOC         0
#define DEF1103_CAN_MSG_TRIGGER_DELAYL_LOC      1
#define DEF1103_CAN_MSG_TRIGGER_DELAYH_LOC      2


/* direct command with one parameters */

#define DEF1103_CAN_DATA_DIRECT_PCNT            1
#define DEF1103_CAN_DATA_DIRECT_IDX_LOC         0  /* msg_no */


/* direct command with zero parameters */
#define DEF1103_CAN_DATA0_DIRECT_PCNT           0

/* mask set */

#define DEF1103_CAN_MASK_SET_PCNT               2
#define DEF1103_CAN_MASK_MASKL_LOC              0
#define DEF1103_CAN_MASK_MASKH_LOC              1


/* msg id read register */

#define DEF1103_CAN_MSGID_READ_PCNT             1
#define DEF1103_CAN_MSGID_READ_MBNO_LOC         0
#define DEF1103_CAN_MSGID_READ_DINCNT           3
#define DEF1103_CAN_MSGID_READ_DOUTCNT          0
#define DEF1103_CAN_MSGID_READ_IDENTL_LOC       0
#define DEF1103_CAN_MSGID_READ_IDENTH_LOC       1
#define DEF1103_CAN_MSGID_READ_FORMAT_LOC       2


/* Msg processed-info register */

#define DEF1103_CAN_PROCESSED_INFO_REG_PCNT     1
#define DEF1103_CAN_PROCESSED_INFO_REG_DOUTCNT  0
#define DEF1103_CAN_PROCESSED_INFO_DINCNT       4
#define DEF1103_CAN_PROCESSED_INFO_MSGNO_LOC    0

#define DEF1103_CAN_PROCESSED_INFO_READ_PROCESS_LOC   0
#define DEF1103_CAN_PROCESSED_INFO_READ_TIMECL_LOC    1
#define DEF1103_CAN_PROCESSED_INFO_READ_TIMECH_LOC    2
#define DEF1103_CAN_PROCESSED_INFO_READ_WRAPC_LOC     3


/* Send_CAN_Status*/

#define DEF1103_CAN_STATUS_INFO_DINCNT          2
#define DEF1103_CAN_STATUS_INFO_REG_DOUTCNT     0
#define DEF1103_CAN_STATUS_INFO_REG_PCNT        0


/* Msg status read */

#define DEF1103_CAN_MSG_STATUS_READ_DINCNT      12


/* Msg RX all read */

#define DEF1103_CAN_MSG_RX_ALL_READ_DINCNT      12


/* Msg RQ tx all read */

#define DEF1103_CAN_MSG_RQTX_ALL_READ_DINCNT    12


/* Msg RQ rx all read */

#define DEF1103_CAN_MSG_RQRX_ALL_READ_DINCNT    12


/* RQ-Msg-tx-processed register */

#define DEF1103_CAN_RQTX_MSG_PROC_REG_PCNT        1
#define DEF1103_CAN_RQTX_MSG_PROC_REG_MSGNO_LOC   0
#define DEF1103_CAN_RQTX_MSG_PROC_REG_DOUTCNT     0 


/* RQ-Msg-tx-processed */ 

#define DEF1103_CAN_RQTX_MSG_PROC_DINCNT        4
#define DEF1103_CAN_RQTX_MSG_PROC_FLAG_LOC      0
#define DEF1103_CAN_RQTX_MSG_PROC_TCLO_LOC      1
#define DEF1103_CAN_RQTX_MSG_PROC_TCHI_LOC      2
#define DEF1103_CAN_RQTX_MSG_PROC_WRAP_LOC      3


/* service register */

#define DEF1103_CAN_SERVICE1_REG_DINCNT         1
#define DEF1103_CAN_SERVICE2_REG_DINCNT         2


/* CAN_msg_set */
#define DEF1103_CAN_MSG_SET_PCNT                4
#define DEF1103_CAN_MSG_SET_MSGNO_LOC           0
#define DEF1103_CAN_MSG_SET_TYPE_LOC            1 
#define DEF1103_CAN_MSG_SET_VALUE_L_LOC         2 
#define DEF1103_CAN_MSG_SET_VALUE_H_LOC         3 


/* CAN_msg_send_id */
#define DEF1103_CAN_TX_MSG_SEND_ID_PCNT        11
#define DEF1103_CAN_TX_MSG_SEND_ID_PCNT2        7
#define DEF1103_CAN_TX_MSG_SEND_ID_IDX_LOC      0  
#define DEF1103_CAN_TX_MSG_SEND_ID_IDENTL_LOC   1
#define DEF1103_CAN_TX_MSG_SEND_ID_IDENTH_LOC   2
#define DEF1103_CAN_TX_MSG_SEND_ID_DELAY_L_LOC  3
#define DEF1103_CAN_TX_MSG_SEND_ID_DELAY_H_LOC  4
#define DEF1103_CAN_TX_MSG_SEND_ID_DATALEN_LOC  5 
#define DEF1103_CAN_TX_MSG_SEND_ID_DATA_LOC     6


/* CAN_serv_multi */
#define DEF1103_CAN_SERV_MULTI_DINCNT         32
#define DEF1103_CAN_SERV_MULTI_DOUTCNT         0 
#define DEF1103_CAN_SERV_MULTI_PCNT            3
#define DEF1103_CAN_SERV_MULTI_INFORM_L_LOC    0
#define DEF1103_CAN_SERV_MULTI_INFORM_H_LOC    1

#define DEF1103_CAN_SVC_BUSSTATUS          0x00000001
#define DEF1103_CAN_SVC_STDMASK            0x00000002
#define DEF1103_CAN_SVC_EXTMASK            0x00000004
#define DEF1103_CAN_SVC_MSG_MASK15         0x00000008
#define DEF1103_CAN_SVC_TX_OK              0x00000010
#define DEF1103_CAN_SVC_RX_OK              0x00000020
#define DEF1103_CAN_SVC_CRC_ERR            0x00000040
#define DEF1103_CAN_SVC_ACK_ERR            0x00000080
#define DEF1103_CAN_SVC_FORM_ERR           0x00001000
#define DEF1103_CAN_SVC_STUFFBIT_ERR       0x00002000
#define DEF1103_CAN_SVC_BIT1_ERR           0x00004000
#define DEF1103_CAN_SVC_BIT0_ERR           0x00008000
#define DEF1103_CAN_SVC_RX_LOST            0x00010000
#define DEF1103_CAN_SVC_DATA_LOST          0x00020000
#define DEF1103_CAN_SVC_VERSION            0x00040000
#define DEF1103_CAN_SVC_MAILBOX_ERR        0x00080000
#define DEF1103_CAN_SVC_TXQUEUE_OVERFLOW_COUNT  0x02000000


/* Tx queue init */

#define DEF1103_CAN_MSG_TXQUEUE_SIZE              64
#define DEF1103_CAN_MSG_TXQUEUE_OVERRUN_OVERWRITE  0
#define DEF1103_CAN_MSG_TXQUEUE_OVERRUN_IGNORE     1
#define DEF1103_CAN_MSG_TXQUEUE_INIT_PCNT          4
#define DEF1103_CAN_MSG_TXQUEUE_INIT_MSGNO_LOC     0
#define DEF1103_CAN_MSG_TXQUEUE_INIT_POLICY_LOC    1
#define DEF1103_CAN_MSG_TXQUEUE_INIT_DELAY_L_LOC   2
#define DEF1103_CAN_MSG_TXQUEUE_INIT_DELAY_H_LOC   3


/* Msg send id queued */

#define DEF1103_CAN_TXQUEUE_SEND_ID_PCNT           4
#define DEF1103_CAN_TXQUEUE_SEND_ID_PCNT2          8
#define DEF1103_CAN_TXQUEUE_SEND_ID_MSGNO_LOC      0
#define DEF1103_CAN_TXQUEUE_SEND_ID_ID_LOC_L       1
#define DEF1103_CAN_TXQUEUE_SEND_ID_ID_LOC_H       2
#define DEF1103_CAN_TXQUEUE_SEND_ID_DATALEN_LOC    3  
#define DEF1103_CAN_TXQUEUE_SEND_ID_DATA_LOC       4


/* Baudrate set */

#define DEF1103_CAN_CHANNEL_SET_BAUDRATE_PCNT      2
#define DEF1103_CAN_CHANNEL_SET_BAUDRATE_BTR0_LOC  0
#define DEF1103_CAN_CHANNEL_SET_BAUDRATE_BTR1_LOC  1


/* Clear TX-queue */

#define DEF1103_CAN_TXQUEUE_CLEAR_PCNT             1


/************* fw 1_x ***************/

/* init-info */

#define MCDEF_INIT_INFO_BRATEL_LOC      0
#define MCDEF_INIT_INFO_BRATEH_LOC      1
#define MCDEF_INIT_INFO_CLCKL_LOC       2
#define MCDEF_INIT_INFO_CLCKH_LOC       3 
#define MCDEF_INIT_INFO_BTR0_LOC        4
#define MCDEF_INIT_INFO_BTR1_LOC        5  
#define MCDEF_INIT_INFO_BOFF_INTNO_LOC  6
#define MCDEF_INIT_INFO_PCNT            7


/* Msg trigger */

#define MCDEF_MSG_TRIGGER_PCNT          1
#define MCDEF_MSG_TRIGGER_IDX_LOC       0 


/* global stdmask set */

#define MCDEF_STDMASK_SET_PCNT          1


/* global stdarbmask set */

#define MCDEF_STDARBMASK_SET_PCNT          1

/* global extarbmask set */

#define MCDEF_EXTARBMASK_SET_PCNT          2
#define MCDEF_EXTARBMASK_MSKLO_LOC         0
#define MCDEF_EXTARBMASK_MSKHI_LOC         1


/* global extmask set */

#define MCDEF_EXTMASK_SET_PCNT          2
#define MCDEF_EXTMASK_MSKLO_LOC         0
#define MCDEF_EXTMASK_MSKHI_LOC         1


/* global stdmask read register */

#define MCDEF_STDMASK_READ_REG_DINCNT       1


/* global extmask read register */

#define MCDEF_EXTMASK_READ_REG_DINCNT       2 


/* global extmask read */

#define MCDEF_EXTMASK_READ_PCNT             2
#define MCDEF_EXTMASK_READ_MSKLO_LOC        0
#define MCDEF_EXTMASK_READ_MSKHI_LOC        1


/* Msg RX read */

#define MCDEF_MSG_RX_READ_DINCNT           12


/* Msg RX all read */

#define MCDEF_MSG_RX_ALL_READ_DINCNT       12


/* Msg RQ rx all read */

#define MCDEF_MSG_RQRX_ALL_READ_DINCNT     12


/* Msg RQ tx all read */

#define MCDEF_MSG_RQTX_ALL_READ_DINCNT     12


#define MCDEF_MSG_STATUS_READ_DINCNT       12 

#endif
