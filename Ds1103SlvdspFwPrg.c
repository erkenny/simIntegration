/*******************************************************************************
*
*  MODULE
*    Template file for the DS1103 slave DSP burn application. This file should
*    be used to build a PPC-file, which programs the flash memory of the slave
*    DSP TMS320F240.
*
*  FILE
*    DS1103SLVDSP_FWX_PRG.c
*
*  RELATED FILES
*    fw240.obj      (current slave DSP firmware)
*    f240prg.obj    (flash programmer)
*
*  DESCRIPTION
*    This application programs the flash of the TMS320F240 slave DSP
*    on the DS1103.
*
*  REMARKS
*
*  AUTHOR(S)
*    M. Heier
*
*  dSPACE GmbH, Technologiepark 25, 33100 Paderborn, Germany
*
*  $RCSfile: Ds1103SlvdspFwPrg.c $ $Revision: 1.4 $ $Date: 2007/07/20 09:17:28GMT+01:00 $
*******************************************************************************/

#include <brtenv.h>
#include <dsfwinfo.h>
#include <usrdsp.h>         /* include user specific header, which contains  */
                            /* the user firmware revision numbers            */

/*******************************************************************************
  constant, macro and type definitions
*******************************************************************************/

#define SLVDSP1103_RSTSLV       0x0010                /* slave DSP reset bit */
#define SLVDSP1103_SLVBOOT      0x0040                 /* slave DSP mode bit */

#define DS1103_SLAVE_DSP_PRG_MEM_START   0x10200000  /* slave program memory */
#define DS1103_SLAVE_DSP_PRG_MEM_SIZE    0x240000

#define DS1103_SLVDSP_FW240_START        _fw240_data[]           /* firmware */
#define DS1103_SLVDSP_FW240_SIZE         _fw240_no_of_data     /* array size */
#define DS1103_SLVDSP_FW240_LAST_ADDR    _fw240_lastaddr     /* last address */

#define DS1103_SLVDSP_F240PRG_START      f240prg_data[]  /* flash programmer */
#define DS1103_SLVDSP_F240PRG_SIZE       f240prg_no_of_data    /* array size */
#define DS1103_SLVDSP_F240PRG_LAST_ADDR  f240prg_lastaddr    /* last address */

extern  UInt32  DS1103_SLVDSP_FW240_START;
extern  UInt32  DS1103_SLVDSP_FW240_SIZE;
extern  UInt32  DS1103_SLVDSP_FW240_LAST_ADDR;

extern  UInt32  DS1103_SLVDSP_F240PRG_START;
extern  UInt32  DS1103_SLVDSP_F240PRG_SIZE;
extern  UInt32  DS1103_SLVDSP_F240PRG_LAST_ADDR;

/*******************************************************************************
  object declarations
*******************************************************************************/

/* flash info locations */
#define DS1103_SLV_DSP_FL_FW_VERSION      0x0040         /* firmware version */
#define DS1103_SLV_DSP_FL_FW_COUNT        0x0041      /* programming counter */
#define DS1103_SLV_DSP_FL_FW_ORIGIN       0x0042                   /* origin */

/* defines for handling the communication via DPMEM */

#define DS1103_SLV_DSP_TIMEOUT_MSCMD   1.0   /* slave response timeout [sec] */
#define DS1103_SLV_DSP_TIMEOUT_SHORT  10.0   /* slave response timeout [sec] */
#define DS1103_SLV_DSP_TIMEOUT_LONG   20.0   /* slave response timeout [sec] */

/* location of communication flags */
#define DS1103_SLV_DSP_FL_CFG_SEC_OFFSET    0x01E00
#define DS1103_SLV_DSP_FL_CFG_SEC_START     0x10401E00

/* communication status */
#define FL_CFG_FLASHPRG_RUNS                    0x001
#define FL_CFG_FLASH_CHECKED                    0x002
#define FL_CFG_INFO_UPDATED                     0x003
#define FL_CFG_FLASH_ERASED                     0x004
#define FL_CFG_PRG_WAIT                         0x005
#define FL_CFG_PRG_MODE                         0x006
#define FL_CFG_PRG_READY                        0x007

#define FL_CFG_LAST_ERROR                       0x0FC

#define FL_CFG_PRG_ERROR                        0x0FC
#define FL_CFG_PARA_ERROR                       0x0FD
#define FL_CFG_ERASE_ERROR                      0x0FE
#define FL_CFG_FLASH_PROTECTED                  0x0FF

#define FL_CFG_EXT_RAM_BOOT                     0x000
#define FL_CFG_FLASH_BOOT                       0x001
#define FL_CFG_GO_ON                            0x002

#define FL_CFG_NO_DATA                          0x000
#define FL_CFG_VALID_DATA                       0x001
#define FL_CFG_DATA_READY                       0x002

/* global error definitions */
#define NO_ERROR                                0                /* no error */
#define FLASH_ERROR                             1      /* error on slave DSP */
#define PPC_ERROR                               2     /* error on master PPC */

#define DS1103_SLV_DSP_FLASH_SIZE               0x08000

typedef struct {
  UInt16        fl_cfg_firmware_version   ;
  UInt16        fl_cfg_prg_number         ;
  UInt16        fl_cfg_dummy              ;
  UInt16        fl_cfg_comm_flag          ;
  UInt16        fl_cfg_boot_mode          ;
  UInt16        fl_cfg_valid_data         ;
  UInt16        fl_cfg_dest_addr          ;
  UInt16        fl_cfg_transfer_data_size ;
  UInt16        fl_cfg_debug1             ;
  UInt16        fl_cfg_debug2             ;
} FL_CFG_SEC_TYPE ;

/* symbolic pointer */
#define FlCfgSecPtr ((volatile FL_CFG_SEC_TYPE *) DS1103_SLV_DSP_FL_CFG_SEC_START)

/* hardware manager flag */
extern volatile UInt32 hwm_fw_appl_state;

/*******************************************************************************
  global variables
*******************************************************************************/

volatile UInt16 *rst = (volatile UInt16 *) 0x10000040;     /* reset register */
volatile UInt16 *stp = (volatile UInt16 *) 0x10000088;     /* setup register */

UInt16 NewFwVersion;                       /* store current firmware version */
UInt16 OldFwVersion;                            /* read old firmware version */
UInt16 PrgNumber;                                     /* programming counter */
UInt16 OldDSpacePrg;                                            /* origin ID */


#pragma asm
.equ _dsfw_user_mar,    USER_MAJOR_RELEASE
.equ _dsfw_user_mir,    USER_MINOR_RELEASE
.equ _dsfw_user_submir, USER_SUBMINOR_RELEASE
#pragma endasm

/*******************************************************************************
  function declarations
*******************************************************************************/
/*******************************************************************************
*
*  FUNCTION
*    Function to set the hardware manager flag if exit is called!
*
*  SYNTAX
*    void update_exit(void)
*
*  DESCRIPTION
*
*  PARAMETERS
*
*  RETURNS       nothing
*
*  REMARKS
*
*******************************************************************************/

void update_exit(void)
{
  hwm_fw_appl_state = DS_FW_APPL_STATE_COMMON_ERROR;
}

/*******************************************************************************
*
*  FUNCTION
*    Evaluates and prints out the state of the communication.
*
*  SYNTAX
*    FlashErrorInfo()
*
*  DESCRIPTION
*    This function is called if an time out or handshake error has occured.
*    It evaluates the state of master-slave communication and the flash
*    programmer state. The contents of the communication flags are printed out.
*
*  PARAMETERS    none
*
*  RETURNS       nothing
*
*  REMARKS
*
*******************************************************************************/

void FlashErrorInfo (void)
{
  UInt16 comm_flag;


  comm_flag = FlCfgSecPtr->fl_cfg_comm_flag;

  msg_info_printf(0,0,
    "Communication Flag : %lx", comm_flag);
  switch (comm_flag)
  {
    case 0x00:
      msg_info_set(0,0,"    Flash programmer <F240Prg> doesn't start!");
    break ;

    case FL_CFG_FLASHPRG_RUNS :
      msg_info_set(0,0,"    Flash programmer <F240Prg> is started!");
    break ;

    case FL_CFG_FLASH_CHECKED:
      msg_info_set(0,0,"    Flash protection is checked!");
    break ;

    case FL_CFG_INFO_UPDATED:
      msg_info_set(0,0,"    Info is updated!");
    break ;

    case FL_CFG_FLASH_ERASED:
      msg_info_set(0,0,"    Flash is erased!");
    break ;

    case FL_CFG_PRG_WAIT:
      msg_info_set(0,0,"    Wait for valid data!");
    break ;

    case FL_CFG_PRG_MODE:
      msg_info_set(0,0,"    Flash programming!");
    break ;

    case FL_CFG_PRG_READY:
      msg_info_set(0,0,"    Programming is ready!");
    break ;


    case FL_CFG_FLASH_PROTECTED:
      msg_info_set(0,0,"    Error : Flash is protected!");
    break ;

    case FL_CFG_ERASE_ERROR:
      msg_info_set(0,0,"    Error : During erase!");
    break ;

    case FL_CFG_PARA_ERROR:
      msg_info_set(0,0,"    Error : Invalid parameter!");
    break ;

    case FL_CFG_PRG_ERROR:
      msg_info_set(0,0,"    Error : During programming!");
    break ;

    default:
      msg_info_set(0,0,"    Invalid communication value!");
    break ;

  }

  msg_info_printf(0,0,
    "Boot Mode          : %4lx", FlCfgSecPtr->fl_cfg_boot_mode);
  switch (FlCfgSecPtr->fl_cfg_boot_mode)
  {

    case FL_CFG_EXT_RAM_BOOT:
      msg_info_set(0,0,"    External program RAM boot mode!");
    break ;

    case FL_CFG_FLASH_BOOT:
      msg_info_set(0,0,"    Flash boot mode!");
    break ;

    case FL_CFG_GO_ON:
      msg_info_set(0,0,"    Go on for erase!");
    break ;

    default:
      msg_info_set(0,0,"    Error : Invalid boot mode value!");
    break ;

  }

  msg_info_printf(0,0,
    "Valid Data         : %4lx", FlCfgSecPtr->fl_cfg_valid_data);
  switch (FlCfgSecPtr->fl_cfg_valid_data)
  {

    case FL_CFG_NO_DATA:
      msg_info_set(0,0,"    No data!");
    break ;

    case FL_CFG_VALID_DATA:
      msg_info_set(0,0,"    Valid data!");
    break ;

    case FL_CFG_DATA_READY:
      msg_info_set(0,0,"    No more data!");
    break ;

    default:
      msg_info_set(0,0,"    Error : Invalid valid data value!");
    break ;

  }


  msg_info_printf(0,0,
    "Destination Address: %4lx", FlCfgSecPtr->fl_cfg_dest_addr);
  msg_info_printf(0,0,
    "Transfer Data Size : %4lx", FlCfgSecPtr->fl_cfg_transfer_data_size);

}


/*******************************************************************************
*
*  FUNCTION
*    Loads flash programmer application <f240prg.c>.
*
*  SYNTAX
*    load_programer_f240prg()
*
*  DESCRIPTION
*    This function clears the slave DSP program memory and copies the flash
*    programer application to it.
*
*  PARAMETERS     none
*
*  RETURNS        nothing
*
*  REMARKS
*
*******************************************************************************/

void load_programmer_f240prg(void)
{
  volatile UInt16 *slvdsp_prgmem_p =
    (volatile UInt16 *) DS1103_SLAVE_DSP_PRG_MEM_START;
  UInt32 i;

  /* clear slave DSP program memory */
  for (i = 0; i <= (DS1103_SLVDSP_F240PRG_LAST_ADDR / 2); i++)
    slvdsp_prgmem_p[i] = 0;

  /* copy <F240prg.c> to slave DSP program memory */
  for (i = 0; i < DS1103_SLVDSP_F240PRG_SIZE; i += 2)
  {
    slvdsp_prgmem_p = (volatile UInt16 *) (DS1103_SLAVE_DSP_PRG_MEM_START
                                                           + (f240prg_data[i]));
    *slvdsp_prgmem_p = f240prg_data[i + 1];
  }
}

/*******************************************************************************
*
*  FUNCTION
*    Starts the flash programmer.
*
*  SYNTAX
*    Int16 start_programer_f240prg()
*
*  DESCRIPTION
*    This function starts the flash programmer application on slave DSP
*    and waits for receiving current firmware information from the slave.
*
*  PARAMETERS   none
*
*  RETURNS      error code   (NO_ERROR, FLASH_ERROR)
*
*  REMARKS
*
*******************************************************************************/

Int16 start_programmer_f240prg(void)
{
  Float64 time;
  ts_timestamp_type start_time, end_time;
  *rst |= SLVDSP1103_RSTSLV;                               /* stop slave DSP */
  *rst &= ~SLVDSP1103_RSTSLV;                             /* reset slave DSP */

  /* wait for slave DSP handshake */
  ts_timestamp_read(&start_time);
  
  do
  {
    master_cmd_server();
    host_service(0,0);

    ts_timestamp_read(&end_time);
    time= ts_timestamp_interval(&start_time,&end_time);
    if ( time > DS1103_SLV_DSP_TIMEOUT_SHORT)
    {
      msg_info_set(0,0,"Time out, slave not responding!");
      return(FLASH_ERROR);                  /* timeout, if no slave response */
    }
  }
  while ((FlCfgSecPtr->fl_cfg_comm_flag < FL_CFG_FLASHPRG_RUNS ) ||
    FlCfgSecPtr->fl_cfg_comm_flag >= FL_CFG_LAST_ERROR);


  *stp &= ~SLVDSP1103_SLVBOOT;                             /* set FLASH-Mode */

  FlCfgSecPtr->fl_cfg_boot_mode = FL_CFG_FLASH_BOOT;    /* trigger slave DSP */

  /* wait for slave DSP handshake */
  ts_timestamp_read(&start_time);
  
  do
  {
    master_cmd_server();
    host_service(0,0);

    ts_timestamp_read(&end_time);
    time= ts_timestamp_interval(&start_time,&end_time);
    if ( time > DS1103_SLV_DSP_TIMEOUT_SHORT)
    {
      msg_info_set(0,0,"Time out, slave not responding!");
      return(FLASH_ERROR);                  /* timeout, if no slave response */
    }
  } while(FlCfgSecPtr->fl_cfg_comm_flag != FL_CFG_INFO_UPDATED);

  /* read firmware information */
  OldFwVersion = FlCfgSecPtr->fl_cfg_firmware_version;            /* version */
  PrgNumber = FlCfgSecPtr->fl_cfg_prg_number;         /* programming counter */
  OldDSpacePrg = FlCfgSecPtr->fl_cfg_dummy;                        /* origin */

  if(PrgNumber == 0x0FFFF)                     /* check for counter overflow */
    PrgNumber = 0;

  return(NO_ERROR);

}

/*******************************************************************************
*
*  FUNCTION
*    Loads firmware <fw240.c> to dual-port memory and performs the flash
*    programming process.
*
*  SYNTAX
*    Int16 load_firmware_fw240()
*
*  DESCRIPTION
*    This function copies the firmware application <fw240.c> from PPC memory
*    to a local buffer. From this buffer which portrais the data location
*    within the flash memory the firmware is transfered in several data blocks
*    through the dual-port memory to the slave DSP. The slave read the data
*    blocks from the DPMEM and programs the flash. The flash programming
*    process is controlled by handshake between master and slave.
*
*  PARAMETERS   none
*
*  RETURNS      error code   (NO_ERROR, FLASH_ERROR, PPC_ERROR)
*
*  REMARKS
*
*******************************************************************************/

Int16 load_firmware_fw240(void)
{
  volatile UInt16 *dpmem_p = (volatile UInt16 *) DS1103_SLAVE_DSP_DP_RAM_START;
  UInt16 *FlashArray;
  UInt16 i,j;
  UInt16 slvaddr, slvsize;
  UInt16 next_address, rest_size;
  UInt16 size_of_FlashArray;
  UInt16 max_block_size;
  UInt16 BlockFlag;
  UInt16 block_number = 0;
  
  Float64 time;
  ts_timestamp_type start_time, end_time;
  

  FlashArray = (UInt16 *) malloc (0x8000);     /* allocate FlashArray buffer */

  if (!FlashArray)
  {
    msg_error_set(0,0,"PPC memory allocation error!");
    return(PPC_ERROR);
  }

  FlCfgSecPtr->fl_cfg_boot_mode = FL_CFG_GO_ON;         /* trigger slave DSP */

  /* fill buffer with zeros */
  for(i = 0; i < (DS1103_SLV_DSP_FLASH_SIZE / 2); i++)
    FlashArray[i] = 0x0000;

  /* copy <fw240.c> from ppc memory to local buffer */
  for (i = 0; i < DS1103_SLVDSP_FW240_SIZE; i += 2)
    FlashArray[(_fw240_data[i] / 2)] = _fw240_data[i + 1];

  /* get size of FlashArray[] */
  size_of_FlashArray = DS1103_SLVDSP_FW240_LAST_ADDR / 2;

  /* set maximum size of data space within DPMEM */
  max_block_size = DS1103_SLV_DSP_FL_CFG_SEC_OFFSET / 2;

  next_address = 0x0000;                /* start Flash programming at 0x0000 */
  rest_size = size_of_FlashArray;                  /* set total size of data */

  /* set new firmware information, increment programming counter */
  FlashArray[DS1103_SLV_DSP_FL_FW_VERSION] = slvdsp_get_firmware_version();
  FlashArray[DS1103_SLV_DSP_FL_FW_COUNT]   = PrgNumber + 1;

	/* clear DPMEM for transmitting data blocks */
  for (i = 0; i < max_block_size; i++)
    dpmem_p[i] = 0;

  /* copy firmware <fw240.c> to DPMEM and programe Flash (by slave) */
  msg_info_set(0,0,"Programming process has started ...");

  do
  {
    master_cmd_server();
    host_service(0,0);

    /* calculate block transfer data */
    ++block_number;                      /* number of block to be transfered */

    if(rest_size < max_block_size)
    {
      slvsize = rest_size + 2;          /* (+2) get last entry in FlashArray */
      slvaddr = next_address;
      BlockFlag = 0;         /* mark last data block and end of transmission */
      msg_info_printf(0,0,
        "... programming %2d. and last block of data ...",block_number);
    }
    else
    {
      slvsize = max_block_size;
      rest_size -= max_block_size;
      slvaddr = next_address;
      next_address += max_block_size;
      BlockFlag = 1;         /* transmission is not over, there is data left */
      msg_info_printf(0,0,"... programming %2d. block of data ...",block_number);
    }

    /* wait for slave DSP handshake */
    ts_timestamp_read(&start_time);
  
    do
    {
      master_cmd_server();
      host_service(0,0);
      ts_timestamp_read(&end_time);
      time= ts_timestamp_interval(&start_time,&end_time);
      if ( time > DS1103_SLV_DSP_TIMEOUT_LONG)
      {
        msg_info_set(0,0,"Time out, slave not responding!");
        return(FLASH_ERROR);
      }
    } while(FlCfgSecPtr->fl_cfg_comm_flag != FL_CFG_PRG_WAIT);

    j = 0x00;                      /* copy each data block at start of DPMEM */

    /* copy (slvsize) of data blocks to DPMEM */
    for (i = slvaddr; i < (slvaddr + slvsize); i++)
      dpmem_p[j++] = FlashArray[i];

    /* write communication data flags to DPMEM */
    FlCfgSecPtr->fl_cfg_dest_addr = slvaddr;      /* set destination address */
    FlCfgSecPtr->fl_cfg_transfer_data_size = slvsize;    /* set size of data */
    FlCfgSecPtr->fl_cfg_valid_data = FL_CFG_VALID_DATA; /* set valid data id */

    /* wait for slave DSP handshake */
    ts_timestamp_read(&start_time);
  
    do
    {
      master_cmd_server();
      host_service(0,0);

	  ts_timestamp_read(&end_time);
      time= ts_timestamp_interval(&start_time,&end_time);
      if ( time > DS1103_SLV_DSP_TIMEOUT_LONG)
      {
        msg_info_set(0,0,"Time out, slave not responding!");
        return(FLASH_ERROR);                /* timeout, if no slave response */
      }
    } while(FlCfgSecPtr->fl_cfg_comm_flag != FL_CFG_PRG_MODE);

    FlCfgSecPtr->fl_cfg_valid_data = FL_CFG_NO_DATA;  /* reset valid data id */

  } while (BlockFlag == 1);          /* last data block has been transmitted */

  return(NO_ERROR);

}

/*******************************************************************************
*
*  FUNCTION
*
*
*  SYNTAX
*    stop_programer_f240prg(void)
*
*  DESCRIPTION
*    TYPE DESCRIPTION HERE
*
*  PARAMETERS
*
*  RETURNS
*
*  REMARKS
*
*******************************************************************************/

Int16 stop_programer_f240prg(void)
{
  Float64 time;
  ts_timestamp_type start_time, end_time;
  
  /* stop flash programming routine (on slave DSP) */
  FlCfgSecPtr->fl_cfg_valid_data = FL_CFG_DATA_READY;

  /* wait for slave DSP handshake,
     slave has finished and persists in while(1)-loop */
  ts_timestamp_read(&start_time);
  
  do
  {
    master_cmd_server();
    host_service(0,0);

    ts_timestamp_read(&end_time);
    time= ts_timestamp_interval(&start_time,&end_time);
    if ( time > DS1103_SLV_DSP_TIMEOUT_SHORT)
    {
      msg_info_set(0,0,"Time out, slave not responding!");
      return(FLASH_ERROR);                /* timeout, if no slave response */
    }
  } while(FlCfgSecPtr->fl_cfg_comm_flag != FL_CFG_PRG_READY);

  return(NO_ERROR);

}

/*******************************************************************************
*
*  FUNCTION
*    Main function.
*
*  SYNTAX
*    main()
*
*  DESCRIPTION
*    This function performs the whole flash programming process.
*    It uses the following functions declared within this module:
*       - load_programmer_f240prg()
*       - start_programmer_f240prg()
*       - load_firmware_fw240()
*       - stop_programer_f240prg()
*       - FlashErrorInfo()
*
*  PARAMETERS   none
*
*  RETURNS      nothing
*
*  REMARKS
*
*******************************************************************************/

void main ()
{
  volatile UInt16 *dpmem_p = (volatile UInt16 *) DS1103_SLAVE_DSP_DP_RAM_START;
  char fw_version[2] = { '\0', '\0' };
  UInt32 i;
  Int16 error;
  Float64 time;
  ts_timestamp_type start_time, end_time;


  ds1103_init();                           /* initialization fo DS1103 borad */
  atexit(update_exit);

  ts_timestamp_read(&start_time);
  do
  {
    master_cmd_server();
    host_service(0,0);
    ts_timestamp_read(&end_time);
    time= ts_timestamp_interval(&start_time,&end_time);
  }
  while (time < DS1103_SLV_DSP_TIMEOUT_MSCMD);

  msg_info_set(0,0,"--- DS1103 Slave DSP Flash Programmer (Vs. 1.0) ---");

  /* set global symbols containing firmware information */
	slvdsp_set_firmware_version();

	/* read new firmware version as defined in ds240fwv.h */
  NewFwVersion = slvdsp_get_firmware_version();

  /* update HWM flag */
	hwm_fw_appl_state = DS_FW_APPL_STATE_BUSY;

  ts_timestamp_read(&start_time);
  do
  {
    master_cmd_server();
    host_service(0,0);
    ts_timestamp_read(&end_time);
    time= ts_timestamp_interval(&start_time,&end_time);
  }
  while (time < DS1103_SLV_DSP_TIMEOUT_MSCMD);

  /* clear DPMEM for communication */
  for (i = 0; i < ((DS1103_SLAVE_DSP_DP_RAM_SIZE / 2) - 0x10); i++)
    dpmem_p[i] = 0;

  /* copy <f240prg> application to program memory of slave DSP */
  msg_info_set(0,0,"Loading flash programmer ...");

  load_programmer_f240prg();

  ts_timestamp_read(&start_time);
  do
  {
    master_cmd_server();
    host_service(0,0);
    ts_timestamp_read(&end_time);
    time= ts_timestamp_interval(&start_time,&end_time);
  }
  while (time < DS1103_SLV_DSP_TIMEOUT_MSCMD);

  msg_info_set(0,0,"Programmer application is loaded to DS1103 board!");

  *stp |= SLVDSP1103_SLVBOOT;                                /* set RAM-Mode */

  msg_info_set(0,0,"Starting flash programmer ...");

  /* start slave DSP and programming application */
  error = start_programmer_f240prg();

  ts_timestamp_read(&start_time);
  do
  {
    master_cmd_server();
    host_service(0,0);
    ts_timestamp_read(&end_time);
    time= ts_timestamp_interval(&start_time,&end_time);
  }
  while (time < DS1103_SLV_DSP_TIMEOUT_MSCMD);

  if (error != NO_ERROR)
  {
    if (error == FLASH_ERROR)
    {
      msg_error_set(0,0,
        "Error starting flash programmer! Please view log file for detailed information!");
      FlashErrorInfo();
    }
    exit(1) ;
  }

  msg_info_set(0,0,"Flash programmer has started!");

  /* generate firmware information output */
  msg_info_set(0,0,"Flash info:");

  if ((OldFwVersion == 0x1000) || (OldDSpacePrg == 0x4453))
    OldFwVersion = (OldFwVersion >> 1) | 0x0001;    /* convert to new format */

  if (OldFwVersion & (SLVDSP1103_FW_MAINT | SLVDSP1103_FW_VERSION))
  {
    switch (OldFwVersion & SLVDSP1103_FW_VERSION)
    {
      case SLVDSP1103_FW_ALPHA:
        fw_version[0] = 'a';
        break;

      case SLVDSP1103_FW_BETA:
        fw_version[0] = 'b';
        break;

      default:
        fw_version[0] = '\0';
    }

    if (OldFwVersion & SLVDSP1103_FW_ORIGIN)
    {
      msg_info_printf(0, 0,
        "   - dSPACE firmware rev. %d.%d.%s%d detected.",
        (OldFwVersion & SLVDSP1103_FW_MAJOR) >> 11,
        (OldFwVersion & SLVDSP1103_FW_MINOR) >> 7,
        fw_version,
        (OldFwVersion & SLVDSP1103_FW_MAINT) >> 3);
    }
    else
    {
      msg_info_printf(0, 0,
        "   - User firmware based on dSPACE firmware rev. %d.%d.%s%d detected.",
        (OldFwVersion & SLVDSP1103_FW_MAJOR) >> 11,
        (OldFwVersion & SLVDSP1103_FW_MINOR) >> 7,
        fw_version,
        (OldFwVersion & SLVDSP1103_FW_MAINT) >> 3);
    }

  }
  else
  {
    if (OldFwVersion & SLVDSP1103_FW_ORIGIN)
    {
      msg_info_printf(0, 0,
        "   - dSPACE firmware rev. %d.%d detected.",
        (OldFwVersion & SLVDSP1103_FW_MAJOR) >> 11,
        (OldFwVersion & SLVDSP1103_FW_MINOR) >> 7);

    }
    else
    {
      msg_info_printf(0, 0,
        "   - User firmware based on dSPACE firmware rev. %d.%d detected.",
        (OldFwVersion & SLVDSP1103_FW_MAJOR) >> 11,
        (OldFwVersion & SLVDSP1103_FW_MINOR) >> 7);
    }
  }

  msg_info_printf(0,0,"   - Flash has been programmed %5d time(s)!",PrgNumber);

  /* load firmware <fw240.c> and program Flash */
  msg_info_set(0,0,"Prepare flash programming ...");

  error = load_firmware_fw240();

  if (error != NO_ERROR)
  {
    if (error == FLASH_ERROR)
    {
      msg_error_set(0,0,
        "Error during flash programming! Please view log file for detailed information!");
      FlashErrorInfo();
    }
    else if (error == PPC_ERROR)
      msg_error_set(0,0,
        "Error during preparing flash programming! Programming aborted!");
    exit(1) ;
  }

  /* stop programer application on slave DSP */
  error = stop_programer_f240prg();

  if (error != NO_ERROR)
  {
    if (error == FLASH_ERROR)
    {
      msg_error_set(0,0,
        "Error during flash programming! Please view log file for detailed information!");
      FlashErrorInfo();
    }
    exit(1) ;
	}

  ts_timestamp_read(&start_time);
  do
  {
    master_cmd_server();
    host_service(0,0);
    ts_timestamp_read(&end_time);
    time= ts_timestamp_interval(&start_time,&end_time);
  }
  while (time < DS1103_SLV_DSP_TIMEOUT_MSCMD);

  if (NewFwVersion & SLVDSP1103_FW_ORIGIN)
  {
    /* dSPACE firmware */
		if (NewFwVersion & (SLVDSP1103_FW_MAINT | SLVDSP1103_FW_VERSION))
    {
      switch (NewFwVersion & SLVDSP1103_FW_VERSION)
      {
        case SLVDSP1103_FW_ALPHA:
          fw_version[0] = 'a';
          break;

        case SLVDSP1103_FW_BETA:
          fw_version[0] = 'b';
          break;

        default:
          fw_version[0] = '\0';
      }

      msg_info_printf(0, 0,
        "   - dSPACE firmware version %d.%d.%s%d has been programmed successfully!",
        (NewFwVersion & SLVDSP1103_FW_MAJOR) >> 11,
        (NewFwVersion & SLVDSP1103_FW_MINOR) >> 7,
        fw_version,
        (NewFwVersion & SLVDSP1103_FW_MAINT) >> 3);

    }
    else
    {
      msg_info_printf(0, 0,
        "   - dSPACE firmware version %d.%d has been programmed successfully!",
        (NewFwVersion & SLVDSP1103_FW_MAJOR) >> 11,
        (NewFwVersion & SLVDSP1103_FW_MINOR) >> 7);
    }
  }
  else
  {
    /* user firmware */
		msg_info_printf(0, 0,
      "   - User firmware version %d.%d.%d has been programmed successfully!",
      USER_MAJOR_RELEASE,
      USER_MINOR_RELEASE,
      USER_SUBMINOR_RELEASE);
	}

  /* update HWM flag */
	hwm_fw_appl_state = DS_FW_APPL_STATE_READY;

  while(1)
  {
    master_cmd_server();
    host_service(0,0);
  }

}

