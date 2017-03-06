/*******************************************************************************
*
*  MODULE
*
*  FILE
*    dsstd.h
*
*  RELATED FILES
*
*  DESCRIPTION
*    Standard RTLib Macros and Functions
*
*  REMARKS
*
*  AUTHOR(S)
*    V. Lang
*
*  dSPACE GmbH, Technologiepark 25, 33100 Paderborn, Germany
*
*  $RCSfile: dsstd.h $ $Revision: 1.23 $ $Date: 2009/06/24 10:53:34GMT+01:00 $
*******************************************************************************/

#ifndef __DSSTD_H__
#define __DSSTD_H__

#include <dstypes.h>
#include <stddef.h>

/*******************************************************************************
  function prototypes
*******************************************************************************/

void dsstd_vcm_register(void);

int rtlib_background_hook(void (* fcnptr) (void));
void rtlib_background_hook_process();

/* interrupt-protected heap functions */
void * rtlib_calloc(size_t nelem, size_t elsize);
void   rtlib_free(void *ptr);
void * rtlib_malloc(size_t size);
void * rtlib_realloc(void *ptr, size_t size);

/*******************************************************************************
  constant, macro and type definitions
*******************************************************************************/

/* interrupt status type */
typedef UInt32 rtlib_int_status_t;
extern volatile UInt32 dsstd_init_flag;

#ifdef DEBUG_INIT
#define DSSTD_INIT_INFO_MSG() \
              msg_info_set(MSG_SM_RTLIB, 0, "init(): additional call ignored.");
#else
  #define DSSTD_INIT_INFO_MSG()
#endif

/* definitions for the background hook mechanism */
typedef void (rtlib_bg_fcn_t) ();

typedef struct {
  rtlib_bg_fcn_t **fcnptr_table;
  int length;
} rtlib_bg_fcn_table_t;

extern rtlib_bg_fcn_table_t rtlib_bg_fcn_table;


/*##############################################################################
#                                 DS1003                                       #
##############################################################################*/
#if defined (_DS1003)

  typedef long rtlib_io_addr_t;       /* data type for IO bus addresses */

  #define RTLIB_IO_MOD_IDX(base)  (((base) & 0xF0) >> 4)

  #define RTLIB_INT_ENABLE()                  INT_ENABLE()
  #define RTLIB_INT_DISABLE()                 INT_DISABLE()

  #define RTLIB_BACKGROUND_SERVICE()    \
  {                                     \
    host_service(0, 0);                 \
    rtlib_background_hook_process();    \
  }

  #define RTLIB_SLAVE_LOAD_ACKNOWLEDGE()      ds1003_slave_load_acknowledge()

  #define RTLIB_CONV_FLOAT32_TO_TI32(val)          *(volatile UInt32 *) &val
  #define RTLIB_CONV_FLOAT32_TO_IEEE32(val)        (UInt32)to_ieee((float)val)
  #define RTLIB_CONV_FLOAT32_FROM_TI32(val)        *(volatile Float32 *) &val
  #define RTLIB_CONV_FLOAT32_FROM_IEEE32(val)      (Float32)to_c40((long)val)
  #define RTLIB_CONV_FLOAT_TO_SATURATED_INT32(fl)  ((Int32)(fl))

  #define RTLIB_CONV_UINT_TO_INT(LEN, INPUT)  ((((Int##LEN) INPUT) <<(32-LEN))>>(32-LEN))

  #define RTLIB_INT_SAVE_AND_DISABLE(var_name) {              \
    asm("        PUSH    ST       ; save interrupt status");  \
    asm("        ANDN    2000H,ST ; disable interrupts");     }
  #define RTLIB_INT_RESTORE(var_name) \
    asm("        POP     ST       ; restore interrupt status")

  #define RTLIB_SRT_START(sample_time, isr)   isr_t1_start(sample_time)
  #define RTLIB_SRT_PERIOD(sample_time)       ds1003_timer1_set_period(sample_time)
  #define RTLIB_SRT_ISR_BEGIN()               isr_t1_begin()
  #define RTLIB_SRT_ISR_END()                 isr_t1_end()
  #define RTLIB_SRT_ENABLE()                  isr_t1_enable()
  #define RTLIB_SRT_DISABLE()                 isr_t1_disable()

  #define RTLIB_TIC_INIT()                    tic0_init()
  #define RTLIB_TIC_START()                   tic0_start()
  #define RTLIB_TIC_READ()                    (dsfloat)tic0_read()
  #define RTLIB_TIC_READ_TOTAL()              (dsfloat)tic0_read_total()
  #define RTLIB_TIC_HALT()                    tic0_halt()
  #define RTLIB_TIC_CONTINUE()                tic0_continue()
  #define RTLIB_TIC_DELAY(duration)           tic0_delay(duration)

  #define RTLIB_FORCE_IN_ORDER()
  #define RTLIB_SYNC()

  #define RTLIB_EXIT(val)                     exit(val)

  #define init() \
  { \
    if(!dsstd_init_flag) \
    { \
      DS1003_RTLIB_DEBUG_INIT(); \
      ds1003_env_init(); \
      dsstd_init_flag = 1; \
    } \
    else \
      DSSTD_INIT_INFO_MSG(); \
  }

  /* implicitly include needed header files */
  #include <std1003.h>
  #include <intpt40.h>
  #include <ds1003.h>
  #include <tic1003.h>
  #include <fltconv.h>

/*##############################################################################
#                                 DS1004                                       #
##############################################################################*/
#elif defined (_DS1004)
  typedef UInt32 rtlib_io_addr_t;       /* data type for IO bus addresses */

  #define RTLIB_IO_MOD_IDX(base)  0

  #define RTLIB_INT_ENABLE()                  ds1004_set_int_priority_level(0)
  #define RTLIB_INT_DISABLE()                 ds1004_set_int_priority_level(24)

  #define RTLIB_BACKGROUND_SERVICE()  \
  {                                   \
    slave_array_svc();                \
    rtlib_background_hook_process();  \
  }

  #define RTLIB_SLAVE_LOAD_ACKNOWLEDGE()

/* Floating point conversion macros are not defined in RTLib1004 ! */

  #define RTLIB_INT_SAVE_AND_DISABLE(var_name) \
    var_name = ds1004_set_int_priority_level(24)
  #define RTLIB_INT_RESTORE(var_name)          \
    ds1004_set_int_priority_level(var_name)

  #define RTLIB_SRT_START(sample_time, isr)
  #define RTLIB_SRT_PERIOD(sample_time)
  #define RTLIB_SRT_ISR_BEGIN()
  #define RTLIB_SRT_ISR_END()
  #define RTLIB_SRT_ENABLE()
  #define RTLIB_SRT_DISABLE()

  #define RTLIB_TIC_INIT()                    tic_init()
  #define RTLIB_TIC_START()                   tic_start()
  #define RTLIB_TIC_READ()                    (dsfloat)tic_read()
  #define RTLIB_TIC_READ_TOTAL()              (dsfloat)tic_read_total()
  #define RTLIB_TIC_HALT()                    tic_halt()
  #define RTLIB_TIC_CONTINUE()                tic_continue()
  #define RTLIB_TIC_DELAY(duration)           tic_delay(duration)

  #define RTLIB_FORCE_IN_ORDER()
  #define RTLIB_SYNC()

  #define RTLIB_EXIT(val)                     exit(val)

  #define init() \
  { \
    if(!dsstd_init_flag) \
    { \
      ds1004_init(); \
      dsstd_init_flag = 1; \
    } \
    else \
      DSSTD_INIT_INFO_MSG(); \
  }

  /* implicitly include needed header files */
  #include <ds1004.h>
  #include <tic1004.h>
  #include <int1004.h>
  #include <dscmd.h>

/*##############################################################################
#                                 DS1005                                       #
##############################################################################*/
#elif defined (_DS1005)
  typedef UInt32 rtlib_io_addr_t;       /* data type for IO bus addresses */

  #define RTLIB_IO_MOD_IDX(base)  (base < PHS_BUS_BASE) ? (((base) & 0x07800000) >> 23) : (((base) >> 6) & 0x0F)

  #define RTLIB_INT_ENABLE()                  DS1005_GLOBAL_INTERRUPT_ENABLE()
  #define RTLIB_INT_DISABLE()                 DS1005_GLOBAL_INTERRUPT_DISABLE()

  #define RTLIB_BACKGROUND_SERVICE()     \
  {                                      \
    static Int32 _temp_cnt = 0;          \
                                         \
    host_service(0,0);                   \
    master_cmd_server();                 \
                                         \
    ds1005_gl_background_scan();         \
    if( (_temp_cnt++) > 999 )            \
    {                                    \
      _temp_cnt = 0;                     \
      ds1005_info_cpu_temperature_get(); \
    }                                    \
                                         \
    elog_service();                      \
    dsflash_background_service();        \
    if (DSFlashGlobalExitFlag == 1)      \
      dsflash_exit();                    \
    rtlib_background_hook_process();     \
  }

  #define RTLIB_SLAVE_LOAD_ACKNOWLEDGE()      ds1005_slv_boot_finished()

  #define RTLIB_CONV_FLOAT32_TO_TI32(val)          ds1005_conv32_ieee_to_ti((Float32)val)
  #define RTLIB_CONV_FLOAT32_TO_IEEE32(val)        *(volatile UInt32 *) &val
  #define RTLIB_CONV_FLOAT32_FROM_TI32(val)        ds1005_conv32_ti_to_ieee((UInt32) val)
  #define RTLIB_CONV_FLOAT32_FROM_IEEE32(val)      *(volatile Float32 *) &val
  #define RTLIB_CONV_FLOAT_TO_SATURATED_INT32(fl)  ((Int32)(fl))

  #define RTLIB_CONV_UINT_TO_INT(LEN, INPUT)  ((Int##LEN) INPUT)

  #define RTLIB_INT_SAVE_AND_DISABLE(var_name) \
    var_name = DS1005_GLOBAL_INTERRUPT_DISABLE()
  #define RTLIB_INT_RESTORE(var_name) \
    ds1005_set_interrupt_status(var_name)

  #define RTLIB_SRT_START(sample_time, isr)     ds1005_start_isr_timerA(sample_time, isr)
  #define RTLIB_SRT_PERIOD(sample_time)         ds1005_timerA_period_reload(sample_time)
  #define RTLIB_SRT_ISR_BEGIN()                 ds1005_begin_isr_timerA()
  #define RTLIB_SRT_ISR_END()                   ds1005_end_isr_timerA()
  #define RTLIB_SRT_ENABLE()                    ds1005_enable_hardware_int(DS1005_INT_TIMER_A)
  #define RTLIB_SRT_DISABLE()                   ds1005_disable_hardware_int(DS1005_INT_TIMER_A)
                                                
  #define RTLIB_TIC_INIT()                      
  #define RTLIB_TIC_START()                     ds1005_tic_start()
  #define RTLIB_TIC_READ()                      (dsfloat)ds1005_tic_read()
  #define RTLIB_TIC_READ_TOTAL()                (dsfloat)ds1005_tic_total_read()
  #define RTLIB_TIC_HALT()                      ds1005_tic_halt()
  #define RTLIB_TIC_CONTINUE()                  ds1005_tic_continue()
  #define RTLIB_TIC_DELAY(duration)             ds1005_tic_delay(duration)
                                                
  #define RTLIB_TIC_COUNT()                     (rtlib_tic_t)ds1005_tic_count()
  #define RTLIB_TIC_ELAPSED(tmr_cnt)            (dsfloat)ds1005_tic_elapsed((rtlib_tic_t)tmr_cnt)
  #define RTLIB_TIC_DIFF(tmr_cnt1, tmr_cnt2)    (dsfloat)ds1005_tic_diff((rtlib_tic_t)tmr_cnt1, (rtlib_tic_t)tmr_cnt2)

  #define RTLIB_FORCE_IN_ORDER()              asm( " eieio " )
  #define RTLIB_SYNC()                        asm( " sync " )

  #define RTLIB_ATOMIC_64BIT_MOV(from, to)    *((volatile Float64*)to) = *((volatile Float64*)from)

  #define RTLIB_EXIT(val)                     exit(val)


#if defined(RTLIB_IS_CPP_APPL) && !defined(__cplusplus)
  void _main(void);
  void _cxxfini(void);

  #define init()                     \
  {                                  \
    if(!dsstd_init_flag)             \
    {                                \
      DS1005_RTLIB_DEBUG_INIT();     \
      _main();                       \
      ds1005_init();                 \
      dsstd_init_flag = 1;           \
    }                                \
    else                             \
      DSSTD_INIT_INFO_MSG();         \
  }
#else
  #define init()                     \
  {                                  \
    if(!dsstd_init_flag)             \
    {                                \
      DS1005_RTLIB_DEBUG_INIT();     \
      ds1005_init();                 \
      dsstd_init_flag = 1;           \
    }                                \
    else                             \
      DSSTD_INIT_INFO_MSG();         \
  }
#endif /* RTLIB_IS_CXX_APPL */


  /* implicitly include needed header files */
  #include <hostsvc.h>
  #include <hsvc1005.h>
  #include <info1005.h>
  #include <int1005.h>
  #include <init1005.h>
  #include <tic1005.h>
  #include <fc1005.h>
  #include <dsflash.h>

/*##############################################################################
#                                 DS1006                                       #
##############################################################################*/
#elif defined (_DS1006)
  typedef UInt32 rtlib_io_addr_t;       /* data type for IO bus addresses */

  #define RTLIB_IO_MOD_IDX(base)  (base < PHS_BUS_BASE) ? (((base) & 0x07800000) >> 23) : (((base) >> 6) & 0x0F)

  #define RTLIB_INT_ENABLE()                  DS1006_GLOBAL_INTERRUPT_ENABLE()
  #define RTLIB_INT_DISABLE()                 DS1006_GLOBAL_INTERRUPT_DISABLE()

  #define RTLIB_BACKGROUND_SERVICE()     \
  {                                      \
    extern int kthread_service_func(void);\
    static Int32 _temp_cnt = 0;          \
                                         \
    host_service(0,0);                   \
    master_cmd_server();                 \
                                         \
    ds1006_gl_background_scan();         \
    if( (_temp_cnt++) > 999 )            \
    {                                    \
      _temp_cnt = 0;                     \
      ds1006_info_cpu_temperature_get(); \
    }                                    \
                                         \
    elog_service();                      \
                                         \
    rtlib_background_hook_process();     \
                                         \
    kthread_service_func();              \
  }

  #define RTLIB_SLAVE_LOAD_ACKNOWLEDGE()           ds1006_slv_boot_finished()

  #define RTLIB_CONV_FLOAT32_TO_TI32(val)          ds1006_conv32_ieee_to_ti((Float32) val)
  #define RTLIB_CONV_FLOAT32_TO_IEEE32(val)        *(volatile UInt32 *) &val
  #define RTLIB_CONV_FLOAT32_FROM_TI32(val)        ds1006_conv32_ti_to_ieee((UInt32) val)
  #define RTLIB_CONV_FLOAT32_FROM_IEEE32(val)      *(volatile Float32 *) &val
  #define RTLIB_CONV_FLOAT_TO_SATURATED_INT32(fl)  (ds1006_conv_float_to_int32(fl))

  #define RTLIB_CONV_UINT_TO_INT(LEN, INPUT)  ((Int##LEN) INPUT)

  #define RTLIB_INT_SAVE_AND_DISABLE(var_name) \
    var_name = ds1006_int_save_and_disable()

  #define RTLIB_INT_RESTORE(var_name) \
    ds1006_set_interrupt_status(var_name)

  #define RTLIB_SRT_START(sample_time, isr)   ds1006_start_isr_timerA(sample_time, isr)
  #define RTLIB_SRT_PERIOD(sample_time)       ds1006_timerA_period_reload(sample_time)
  #define RTLIB_SRT_ISR_BEGIN()               ds1006_begin_isr_timerA()
  #define RTLIB_SRT_ISR_END()                 ds1006_end_isr_timerA()
  #define RTLIB_SRT_ENABLE()                  ds1006_enable_hardware_int(DS1006_INT_TIMER_A)
  #define RTLIB_SRT_DISABLE()                 ds1006_disable_hardware_int(DS1006_INT_TIMER_A)

  #define RTLIB_TIC_INIT()
  #define RTLIB_TIC_START()                   ds1006_tic_start()
  #define RTLIB_TIC_READ()                    (dsfloat)ds1006_tic_read()
  #define RTLIB_TIC_READ_TOTAL()              (dsfloat)ds1006_tic_total_read()
  #define RTLIB_TIC_HALT()                    ds1006_tic_halt()
  #define RTLIB_TIC_CONTINUE()                ds1006_tic_continue()
  #define RTLIB_TIC_DELAY(duration)           ds1006_tic_delay(duration)

  #define RTLIB_TIC_COUNT()                   (rtlib_tic_t)ds1006_tic_count()
  #define RTLIB_TIC_ELAPSED(tmr_cnt)          (dsfloat)ds1006_tic_elapsed((rtlib_tic_t)tmr_cnt)
  #define RTLIB_TIC_DIFF(tmr_cnt1, tmr_cnt2)  (dsfloat)ds1006_tic_diff((rtlib_tic_t)tmr_cnt1, tmr_cnt2)

  #define RTLIB_FORCE_IN_ORDER()              DS1006_EIEIO
  #define RTLIB_SYNC()                        DS1006_SYNC

  #define RTLIB_ATOMIC_64BIT_MOV(from, to)    __asm__ __volatile__ (                  \
                                                " emms            \n"                 \
                                                " movq (%0), %%mm0\n"                 \
                                                " movq %%mm0, (%1)\n"                 \
                                                " emms            \n"                 \
                                                : : "r" ((const char*)(from)), "r" ((char*)(to)) : "memory")

  #define RTLIB_EXIT(val)                     exit(val)

  #define init()                    \
  {                                 \
    if(!dsstd_init_flag)            \
    {                               \
      DS1006_RTLIB_DEBUG_INIT();    \
      ds1006_init();                \
      dsstd_init_flag = 1;          \
    }                               \
    else                            \
      DSSTD_INIT_INFO_MSG();        \
  }

  /* implicitly include needed header files */
  #include <xmmintrin.h> 
  #include <hostsvc.h>
  #include <hsvc1006.h>
  #include <info1006.h>
  #include <int1006.h>
  #include <init1006.h>
  #include <tic1006.h>
  #include <fc1006.h>

/*##############################################################################
#                                 DS1102                                       #
##############################################################################*/
#elif defined (_DS1102)
  typedef UInt32 rtlib_io_addr_t;       /* data type for IO bus addresses */

  #define RTLIB_IO_MOD_IDX(base) 0

  #define RTLIB_INT_ENABLE()                  asm("      OR    2000H,ST")
  #define RTLIB_INT_DISABLE()                 asm("      ANDN  2000H,ST")

  #define RTLIB_BACKGROUND_SERVICE()    \
  {                                     \
    host_service(0, 0);                 \
    rtlib_background_hook_process();    \
  }

  #define RTLIB_SLAVE_LOAD_ACKNOWLEDGE()

  #define RTLIB_CONV_FLOAT32_TO_TI32(val)          *(volatile UInt32 *) &val
  #define RTLIB_CONV_FLOAT32_TO_IEEE32(val)        (UInt32)cvtie3((float)val)
  #define RTLIB_CONV_FLOAT32_FROM_TI32(val)        *(volatile Float32 *) &val
  #define RTLIB_CONV_FLOAT32_FROM_IEEE32(val)      (Float32)cvtdsp((UInt32)(val))
  #define RTLIB_CONV_FLOAT_TO_SATURATED_INT32(fl)  ((Int32)(fl))


  #define RTLIB_INT_SAVE_AND_DISABLE(var_name) {             \
    asm("        PUSH    ST       ; save interrupt status"); \
    asm("        ANDN    2000H,ST ; disable interrupts");    }
  #define RTLIB_INT_RESTORE(var_name) \
    asm("        POP     ST       ; restore interrupt status")

  #define RTLIB_SRT_START(sample_time, isr)   isr_t1_start(sample_time)
  #define RTLIB_SRT_PERIOD(sample_time)       ds1102_timer1_set_period (sample_time)
  #define RTLIB_SRT_ISR_BEGIN()               isr_t1_begin()
  #define RTLIB_SRT_ISR_END()                 isr_t1_end()
  #define RTLIB_SRT_ENABLE()                  isr_t1_enable()
  #define RTLIB_SRT_DISABLE()                 isr_t1_disable()

  #define RTLIB_TIC_INIT()                    tic0_init()
  #define RTLIB_TIC_START()                   tic0_start()
  #define RTLIB_TIC_READ()                    (dsfloat)tic0_read()
  #define RTLIB_TIC_READ_TOTAL()              (dsfloat)tic0_read_total()
  #define RTLIB_TIC_HALT()                    tic0_halt()
  #define RTLIB_TIC_CONTINUE()                tic0_continue()
  #define RTLIB_TIC_DELAY(duration)           tic0_delay(duration)

  #define RTLIB_FORCE_IN_ORDER()
  #define RTLIB_SYNC()

  #define RTLIB_EXIT(val)                     exit(val)

  #define init() \
  { \
    if(!dsstd_init_flag) \
    { \
      DS1102_RTLIB_DEBUG_INIT(); \
      ds1102_init(); \
      dsstd_init_flag = 1; \
    } \
    else \
      DSSTD_INIT_INFO_MSG(); \
  }

  /* implicitly include needed header files */
  #include <ds1102.h>
  #include <std1102.h>
  #include <tic3x.h>

/*##############################################################################
#                                 DS1103                                       #
##############################################################################*/
#elif defined (_DS1103)
  typedef UInt32 rtlib_io_addr_t;       /* data type for IO bus addresses */

  #define RTLIB_IO_MOD_IDX(base) 0

  #define RTLIB_INT_ENABLE()                  DS1103_GLOBAL_INTERRUPT_ENABLE()
  #define RTLIB_INT_DISABLE()                 DS1103_GLOBAL_INTERRUPT_DISABLE()

  #define RTLIB_BACKGROUND_SERVICE() {  \
    static Int32 _temp_cnt = 0;         \
    host_service(0,0);                  \
    master_cmd_server();                \
    if( (_temp_cnt++) > 999 )           \
    {                                   \
      _temp_cnt = 0;                    \
      ds1103_cpu_temperature();         \
    }                                   \
    elog_service();                     \
    rtlib_background_hook_process();    }

  #define RTLIB_SLAVE_LOAD_ACKNOWLEDGE()

  #define RTLIB_INT_SAVE_AND_DISABLE(var_name) {  \
    var_name = ds1103_get_interrupt_status();     \
    DS1103_GLOBAL_INTERRUPT_DISABLE();            }
  #define RTLIB_INT_RESTORE(var_name) \
    ds1103_set_interrupt_status(var_name)

  #define RTLIB_SRT_START(sample_time, isr)    ds1103_start_isr_timerA(sample_time, isr)
  #define RTLIB_SRT_PERIOD(sample_time)        ds1103_timerA_period_reload(sample_time);
  #define RTLIB_SRT_ISR_BEGIN()                ds1103_begin_isr_timerA()
  #define RTLIB_SRT_ISR_END()                  ds1103_end_isr_timerA()
  #define RTLIB_SRT_ENABLE()                   ds1103_enable_hardware_int(DS1103_INT_TIMER_A)
  #define RTLIB_SRT_DISABLE()                  ds1103_disable_hardware_int(DS1103_INT_TIMER_A)
                                              
  #define RTLIB_TIC_INIT()                    
  #define RTLIB_TIC_START()                    ds1103_tic_start()
  #define RTLIB_TIC_READ()                     (dsfloat)ds1103_tic_read()
  #define RTLIB_TIC_READ_TOTAL()               (dsfloat)ds1103_tic_total_read()
  #define RTLIB_TIC_HALT()                     ds1103_tic_halt()
  #define RTLIB_TIC_CONTINUE()                 ds1103_tic_continue()
  #define RTLIB_TIC_DELAY(duration)            ds1103_tic_delay(duration)
                                              
  #define RTLIB_TIC_COUNT()                    (rtlib_tic_t)ds1103_tic_count()
  #define RTLIB_TIC_ELAPSED(tmr_cnt)           (dsfloat)ds1103_tic_elapsed((rtlib_tic_t)tmr_cnt)
  #define RTLIB_TIC_DIFF(tmr_cnt1, tmr_cnt2)   (dsfloat)ds1103_tic_diff((rtlib_tic_t)tmr_cnt1, (rtlib_tic_t)tmr_cnt2)

  #define RTLIB_FORCE_IN_ORDER()              asm( " eieio " )
  #define RTLIB_SYNC()                        asm( " sync " )

  #define RTLIB_EXIT(val)                     exit(val)



#if defined(RTLIB_IS_CPP_APPL) && !defined(__cplusplus)
  void _main(void);
  void _cxxfini(void);

  #define init()                     \
  {                                  \
    if(!dsstd_init_flag)             \
    {                                \
      DS1103_RTLIB_DEBUG_INIT();     \
      _main();                       \
      ds1103_init();                 \
      dsstd_init_flag = 1;           \
    }                                \
    else                             \
      DSSTD_INIT_INFO_MSG();         \
  }
#else
  #define init()                     \
  {                                  \
    if(!dsstd_init_flag)             \
    {                                \
       DS1103_RTLIB_DEBUG_INIT();    \
      ds1103_init();                 \
      dsstd_init_flag = 1;           \
    }                                \
    else                             \
      DSSTD_INIT_INFO_MSG();         \
  }
#endif /* RTLIB_IS_CXX_APPL */


  /* implicitly include needed header files */
  #include <hostsvc.h>
  #include <int1103.h>
  #include <init1103.h>
  #include <tic1103.h>
  #include <ds1103.h>

/*##############################################################################
#                                 DS1104                                       #
##############################################################################*/
#elif defined (_DS1104)

  typedef UInt32 rtlib_io_addr_t;       /* data type for IO bus addresses */

  #define RTLIB_IO_MOD_IDX(base) 0

  #define RTLIB_INT_ENABLE()                  DS1104_GLOBAL_INTERRUPT_ENABLE()
  #define RTLIB_INT_DISABLE()                 DS1104_GLOBAL_INTERRUPT_DISABLE()

  #define RTLIB_BACKGROUND_SERVICE() {  \
    host_service(0,0);                  \
    master_cmd_server();                \
    elog_service();                     \
    rtlib_background_hook_process();    }

  #define RTLIB_SLAVE_LOAD_ACKNOWLEDGE()      ds1104_slv_boot_finished()

  #define RTLIB_INT_SAVE_AND_DISABLE(var_name) {  \
    var_name = ds1104_get_interrupt_status();     \
    DS1104_GLOBAL_INTERRUPT_DISABLE(); }

  #define RTLIB_INT_RESTORE(var_name) \
    ds1104_set_interrupt_status(var_name)

  #define RTLIB_SRT_START(sample_time, isr)    ds1104_start_isr_timer0(sample_time, isr)
  #define RTLIB_SRT_PERIOD(sample_time)        ds1104_timer0_period_reload(sample_time);
  #define RTLIB_SRT_ISR_BEGIN()                ds1104_begin_isr_timer0()
  #define RTLIB_SRT_ISR_END()                  ds1104_end_isr_timer0()
  #define RTLIB_SRT_ENABLE()                   ds1104_enable_hardware_int(DS1104_INT_TIMER_0)
  #define RTLIB_SRT_DISABLE()                  ds1104_disable_hardware_int(DS1104_INT_TIMER_0)
                                               
  #define RTLIB_TIC_INIT()                     
  #define RTLIB_TIC_START()                    ds1104_tic_start()
  #define RTLIB_TIC_READ()                     (dsfloat)ds1104_tic_read()
  #define RTLIB_TIC_READ_TOTAL()               (dsfloat)ds1104_tic_total_read()
  #define RTLIB_TIC_HALT()                     ds1104_tic_halt()
  #define RTLIB_TIC_CONTINUE()                 ds1104_tic_continue()
  #define RTLIB_TIC_DELAY(duration)            ds1104_tic_delay(duration)

  #define RTLIB_TIC_COUNT()                    (rtlib_tic_t)ds1104_tic_count()
  #define RTLIB_TIC_ELAPSED(tmr_cnt)           (dsfloat)ds1104_tic_elapsed((rtlib_tic_t)tmr_cnt)
  #define RTLIB_TIC_DIFF(tmr_cnt1, tmr_cnt2)   (dsfloat)ds1104_tic_diff((rtlib_tic_t)tmr_cnt1, (rtlib_tic_t)tmr_cnt2)

  #define RTLIB_FORCE_IN_ORDER()              asm( " eieio " )
  #define RTLIB_SYNC()                        asm( " sync " )

  #define RTLIB_EXIT(val)                     exit(val)


#if defined(RTLIB_IS_CPP_APPL) && !defined(__cplusplus)
  void _main(void);
  void _cxxfini(void);

  #define init()                     \
  {                                  \
    if(!dsstd_init_flag)             \
    {                                \
      DS1104_RTLIB_DEBUG_INIT();     \
      _main();                       \
      ds1104_init();                 \
      dsstd_init_flag = 1;           \
    }                                \
    else                             \
      DSSTD_INIT_INFO_MSG();         \
  }
#else
  #define init()                     \
  {                                  \
    if(!dsstd_init_flag)             \
    {                                \
      DS1104_RTLIB_DEBUG_INIT();     \
      ds1104_init();                 \
      dsstd_init_flag = 1;           \
    }                                \
    else                             \
      DSSTD_INIT_INFO_MSG();         \
  }
#endif /* RTLIB_IS_CXX_APPL */


  /* implicitly include needed header files */
  #include <hostsvc.h>
  #include <int1104.h>
  #include <init1104.h>
  #include <tic1104.h>
  #include <ds1104.h>

/*##############################################################################
#                                 DS1401                                       #
##############################################################################*/
#elif defined (_DS1401)

  typedef UInt32 rtlib_io_addr_t;       /* data type for IO bus addresses */

  #define RTLIB_IO_MOD_IDX(base)   ((base >> 21) & 0x0F)

  #define RTLIB_INT_ENABLE()                  DS1401_GLOBAL_INTERRUPT_ENABLE()
  #define RTLIB_INT_DISABLE()                 DS1401_GLOBAL_INTERRUPT_DISABLE()

  #define RTLIB_BACKGROUND_SERVICE() {  \
    ds1401_sys_service();               \
    host_service(0,0);                  \
    master_cmd_server();                \
    dsflash_background_service();       \
    if (DSFlashGlobalExitFlag == 1)     \
      dsflash_exit();                   \
    elog_service();                     \
    rtlib_background_hook_process();    }

  #define RTLIB_SLAVE_LOAD_ACKNOWLEDGE()

  #define RTLIB_INT_SAVE_AND_DISABLE(var_name) {  \
    var_name = ds1401_get_interrupt_status();     \
    DS1401_GLOBAL_INTERRUPT_DISABLE();            }
  #define RTLIB_INT_RESTORE(var_name) \
    ds1401_set_interrupt_status(var_name)

  #define RTLIB_SRT_START(sample_time, isr)    ds1401_start_isr_decrementer(sample_time, isr)
  #define RTLIB_SRT_PERIOD(sample_time)        ds1401_decrementer_period_set((Float64)sample_time)
  #define RTLIB_SRT_ISR_BEGIN()                ds1401_begin_isr_decrementer()
  #define RTLIB_SRT_ISR_END()                  ds1401_end_isr_decrementer()
  #define RTLIB_SRT_ENABLE()                   ds1401_enable_hardware_int(DS1401_INT_DECREMENTER)
  #define RTLIB_SRT_DISABLE()                  ds1401_disable_hardware_int(DS1401_INT_DECREMENTER)
                                              
  #define RTLIB_TIC_INIT()                    
  #define RTLIB_TIC_START()                    ds1401_tic_start()
  #define RTLIB_TIC_READ()                     (dsfloat)ds1401_tic_read()
  #define RTLIB_TIC_READ_TOTAL()               (dsfloat)ds1401_tic_total_read()
  #define RTLIB_TIC_HALT()                     ds1401_tic_halt()
  #define RTLIB_TIC_CONTINUE()                 ds1401_tic_continue()
  #define RTLIB_TIC_DELAY(duration)            ds1401_tic_delay(duration)

  #define RTLIB_TIC_COUNT()                    (rtlib_tic_t)ds1401_tic_count()
  #define RTLIB_TIC_ELAPSED(tmr_cnt)           (dsfloat)ds1401_tic_elapsed((rtlib_tic_t)tmr_cnt)
  #define RTLIB_TIC_DIFF(tmr_cnt1, tmr_cnt2)   (dsfloat)ds1401_tic_diff((rtlib_tic_t)tmr_cnt1, (rtlib_tic_t)tmr_cnt2)

  #define RTLIB_FORCE_IN_ORDER()              asm( " eieio " )
  #define RTLIB_SYNC()                        asm( " sync " )

  #define RTLIB_EXIT(val)                     exit(val)


#if defined(RTLIB_IS_CPP_APPL) && !defined(__cplusplus)
  void _main(void);
  void _cxxfini(void);

  #define init()                     \
  {                                  \
    if(!dsstd_init_flag)             \
    {                                \
      DS1401_RTLIB_DEBUG_INIT();     \
      _main();                       \
      ds1401_init();                 \
      dsstd_init_flag = 1;           \
    }                                \
    else                             \
      DSSTD_INIT_INFO_MSG();         \
  }
#else
  #define init()                     \
  {                                  \
    if(!dsstd_init_flag)             \
    {                                \
      DS1401_RTLIB_DEBUG_INIT();     \
      ds1401_init();                 \
      dsstd_init_flag = 1;           \
    }                                \
    else                             \
      DSSTD_INIT_INFO_MSG();         \
  }
#endif /* RTLIB_IS_CXX_APPL */

  /* implicitly include needed header files */
  #include <hostsvc.h>
  #include <info1401.h>
  #include <int1401.h>
  #include <init1401.h>
  #include <tic1401.h>
  #include <ds1401.h>
  #include <sys1401.h>
  #include <dsflash.h>

/*##############################################################################
#              Rapid Pro Control Unit - MPC565                                 #
##############################################################################*/
#elif defined (_RP565)

// [TBD] !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

  typedef UInt32 rtlib_io_addr_t;       /* data type for IO bus addresses */

  #define RTLIB_INT_ENABLE()                   DSINT_GLOBAL_INTERRUPT_ENABLE()
  #define RTLIB_INT_DISABLE()                  DSINT_GLOBAL_INTERRUPT_DISABLE()

  #define RTLIB_BACKGROUND_SERVICE() {  \
           dci_ecu_rapidpro_bckgrnd (); \
           dsconfig_background (); \
                                     }
                                     
  #define RTLIB_SLAVE_LOAD_ACKNOWLEDGE()

  #define RTLIB_INT_SAVE_AND_DISABLE(var_name) DSINT_SAVE_AND_DISABLE(var_name)
  #define RTLIB_INT_RESTORE(var_name)          DSINT_RESTORE(var_name)

  #define RTLIB_TIC_INIT()
  #define RTLIB_TIC_START()
  #define RTLIB_TIC_READ()                    0.0
  #define RTLIB_TIC_READ_TOTAL()
  #define RTLIB_TIC_HALT()
  #define RTLIB_TIC_CONTINUE()
  #define RTLIB_TIC_DELAY(duration)

  #define RTLIB_TIC_COUNT()
  #define RTLIB_TIC_ELAPSED(tmr_cnt)
  #define RTLIB_TIC_DIFF(tmr_cnt1, tmr_cnt2)

  #define RTLIB_FORCE_IN_ORDER()              asm( " eieio " )
  #define RTLIB_SYNC()                        asm( " sync " )

  #define RTLIB_EXIT(val)                     exit(val)


#if defined(RTLIB_IS_CPP_APPL) && !defined(__cplusplus)
  void _main(void);
  void _cxxfini(void);
  #define init()
#else
  #define init() \
  { \
    if(!dsstd_init_flag) \
    { \
      ds1602_init(); \
      dsstd_init_flag = 1; \
    } \
    else \
      DSSTD_INIT_INFO_MSG(); \
  }
#endif /* RTLIB_IS_CXX_APPL */

  #define RTLIB_SIMSTATE_INFO_SET(simstate_info)  dci_ecu_rapidpro_substate_set(simstate_info)

  /* implicitly include needed header files */
  #include <DSMPC565Int.h>
  #include <stdlib.h>  

/*##############################################################################
#              Rapid Pro Control Unit - MPC5554                                #
##############################################################################*/
#elif defined (_DS1603)

  typedef UInt32 rtlib_io_addr_t;       /* data type for IO bus addresses */

  #define RTLIB_INT_ENABLE()                   DSMPC5554INT_GLOBAL_INTERRUPT_ENABLE()
  #define RTLIB_INT_DISABLE()                  DSMPC5554INT_GLOBAL_INTERRUPT_DISABLE()

  #define RTLIB_BACKGROUND_SERVICE() {  \
           dci_ecu_rapidpro_bckgrnd (); \
           dsconfig_background (); \
                                     }

  #define RTLIB_INT_SAVE_AND_DISABLE(var_name) DSMPC5554INT_SAVE_AND_DISABLE(var_name)
  #define RTLIB_INT_RESTORE(var_name)          DSMPC5554INT_RESTORE(var_name)

  #define RTLIB_TIC_INIT()
  #define RTLIB_TIC_START()                    ds1603_tic_start()
  #define RTLIB_TIC_READ()                     (dsfloat)ds1603_tic_read()
  #define RTLIB_TIC_READ_TOTAL()               (dsfloat)ds1603_tic_total_read()
  #define RTLIB_TIC_HALT()                     ds1603_tic_halt()
  #define RTLIB_TIC_CONTINUE()                 ds1603_tic_continue()
  #define RTLIB_TIC_DELAY(duration)            ds1603_tic_delay(duration)
  #define RTLIB_TIC_COUNT()                    (rtlib_tic_t)ds1603_tic_count()
  #define RTLIB_TIC_ELAPSED(tmr_cnt)           (dsfloat)ds1603_tic_elapsed((rtlib_tic_t)tmr_cnt)
  #define RTLIB_TIC_DIFF(tmr_cnt1, tmr_cnt2)   (dsfloat)ds1603_tic_diff((rtlib_tic_t)tmr_cnt1, (rtlib_tic_t)tmr_cnt2)

  #define RTLIB_FORCE_IN_ORDER()               asm( " msync " )
  #define RTLIB_SYNC()                         asm( " isync " )

  #define RTLIB_EXIT(val)                      exit(val)


#if defined(RTLIB_IS_CPP_APPL) && !defined(__cplusplus)
  void _main(void);
  void _cxxfini(void);
  #define init()
#else
  #define init() \
  { \
    if(!dsstd_init_flag) \
    { \
      ds1603_init(); \
      dsstd_init_flag = 1; \
    } \
    else \
      DSSTD_INIT_INFO_MSG(); \
  }
#endif /* RTLIB_IS_CXX_APPL */

  #define RTLIB_SIMSTATE_INFO_SET(simstate_info)  dci_ecu_rapidpro_substate_set(simstate_info)

  /* implicitly include needed header files */
  #include <stdlib.h>
  #include <dsMPC5554Int.h>
  #include <dsECUcmdRP.h>
  #include <dsconfig.h>
  #include <DS1603Tic.h>

/*----------------------------------------------------------------------------*/
#endif

#define global_enable()                       RTLIB_INT_ENABLE()
#define global_disable()                      RTLIB_INT_DISABLE()

/*##############################################################################
#                                 COMMON                                       #
##############################################################################*/

#define RTLIB_MALLOC_PROT(pointer, size) \
{                                        \
  long status;                           \
  RTLIB_INT_SAVE_AND_DISABLE(status);    \
  pointer = malloc(size);                \
  RTLIB_INT_RESTORE(status);             \
}

#define RTLIB_CALLOC_PROT(pointer, nobj, size) \
{                                              \
  long status;                                 \
  RTLIB_INT_SAVE_AND_DISABLE(status);          \
  pointer = calloc(nobj, size);                \
  RTLIB_INT_RESTORE(status);                   \
}


#define RTLIB_REALLOC_PROT(pointer, size) \
{                                         \
  long status;                            \
  RTLIB_INT_SAVE_AND_DISABLE(status);     \
  pointer = realloc(pointer, size);       \
  RTLIB_INT_RESTORE(status);              \
}

#define RTLIB_FREE_PROT(pointer)         \
{                                        \
  long status;                           \
  RTLIB_INT_SAVE_AND_DISABLE(status);    \
  free(pointer);                         \
  RTLIB_INT_RESTORE(status);             \
}

#define RTLIB_MEMSET_PROT(pointer, value, size)\
{                                        \
  long status;                           \
  RTLIB_INT_SAVE_AND_DISABLE(status);    \
  memset(pointer, value, size);          \
  RTLIB_INT_RESTORE(status);             \
}

/*
 * return the processor's serial number
 */
#define RTLIB_GET_SERIAL_NUMBER()    (CFG_PTR->__cfg_serial_nr)

#endif /* __DSSTD_H__ */

