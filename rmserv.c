/************************************************************************
* RMSERV.C
*
* Implementation file for RealMotion - Services
*
*
* (C) 1993-1999 dSPACE GmbH
*
* $RCSfile: rmserv.c $ $Revision: 1.6 $ $Date: 2006/06/29 13:17:38GMT+01:00 $
*
*************************************************************************/

#include <stdlib.h>
#include <string.h>
#include "dsvcm.h"
#include "rmserv.h"

/* $DSVERSION: RMSERVICE - Motion Service Module */
#define RMSERVICE_VER_MAR 2
#define RMSERVICE_VER_MIR 0
#define RMSERVICE_VER_MAI 0
#define RMSERVICE_VER_SPB VCM_VERSION_RELEASE
#define RMSERVICE_VER_SPN 0
#define RMSERVICE_VER_PLV 0

#if defined (_DS1003)
  #ifdef _INLINE
    #define __INLINE static inline
  #else
    #define __INLINE
  #endif
#elif ((defined _DS1005) || (defined _DS1103) || (defined _DS1401))
  #ifdef _INLINE
    #define __INLINE static
  #else
    #define __INLINE
  #endif
#elif defined (_DS1006)
  #ifdef _INLINE
    #define __INLINE static inline
  #else
    #define __INLINE
  #endif
#else
  #error rmserv.c: no processor board defined (e.g. _DS1003)
#endif

#ifndef _INLINE
#if defined  _DSHOST || defined _CONSOLE
#else
#if defined _DS1006 || defined _DS1005 || defined _DS1103 || defined _DS1401

DS_VERSION_SYMBOL(_rmservice_ver_mar, RMSERVICE_VER_MAR);
DS_VERSION_SYMBOL(_rmservice_ver_mir, RMSERVICE_VER_MIR);
DS_VERSION_SYMBOL(_rmservice_ver_mai, RMSERVICE_VER_MAI);
DS_VERSION_SYMBOL(_rmservice_ver_spb, RMSERVICE_VER_SPB);
DS_VERSION_SYMBOL(_rmservice_ver_spn, RMSERVICE_VER_SPN);
DS_VERSION_SYMBOL(_rmservice_ver_plv, RMSERVICE_VER_PLV);

#endif /* #if defined _DSXXXX */
#endif /* #if defined  _DSHOST || defined _CONSOLE */
#endif /* _INLINE */

RM_SERVICE_STRUCT rm_service_struct = {0x0, 0x0, 0x0, 0x5A5A };
RM_INTERFACE_DESCRIPTOR rm_interface;

/************************************************************************
  auxiliary variables for floating point conversion
************************************************************************/

#ifndef _INLINE
  volatile Float32         rm_fltconv_c40;
  volatile UInt32          rm_fltconv_ieee;
  volatile UInt32          rm_fltconv_src;
  volatile UInt32          rm_fltconv_dst;
  volatile UInt32          rm_fltconv_cnt;
#else
  extern volatile Float32  rm_fltconv_c40;
  extern volatile UInt32   rm_fltconv_ieee;
  extern volatile UInt32   rm_fltconv_src;
  extern volatile UInt32   rm_fltconv_dst;
  extern volatile UInt32   rm_fltconv_cnt;
#endif

#ifdef _DS1003
/************************************************************************
  convert single word from C40 to IEEE format
************************************************************************/

__INLINE UInt32 rm_to_ieee (Float32 value)
{
  rm_fltconv_c40 = value;
  asm("        push    r0");
  asm("        pushf   r0");
  asm("        toieee  @_rm_fltconv_c40,r0");
  asm("        stf     r0,@_rm_fltconv_ieee");
  asm("        popf    r0");
  asm("        pop     r0");
  return rm_fltconv_ieee;
}


/************************************************************************
  convert block from C40 to IEEE format (count >= 2)
************************************************************************/

__INLINE void rm_to_ieee_block (UInt32 count, Float32 *src, UInt32 *dst)
{
  rm_fltconv_src = (UInt32) src;
  rm_fltconv_dst = (UInt32) dst;
  rm_fltconv_cnt = count - 2;
  asm("        push    r0");
  asm("        pushf   r0");
  asm("        push    ar0");
  asm("        push    ar1");
  asm("        push    st");
  asm("        push    rs");
  asm("        push    re");
  asm("        push    rc");
  asm("        ldi     @_rm_fltconv_cnt,rc");
  asm("        rptbd   $+4");
  asm("        ldi     @_rm_fltconv_src,ar0");
  asm("        ldi     @_rm_fltconv_dst,ar1");
  asm("        toieee  *ar0++(1),r0");
  asm("        toieee  *ar0++(1),r0");
  asm("     || stf     r0,*ar1++(1)");
  asm("        stf     r0,*ar1");
  asm("        pop     rc");
  asm("        pop     re");
  asm("        pop     rs");
  asm("        pop     st");
  asm("        pop     ar1");
  asm("        pop     ar0");
  asm("        popf    r0");
  asm("        pop     r0");
}
#endif


#if defined _DS1005 || defined _DS1006 || defined _DS1103 || defined _DS1401

UInt32 rm_to_ieee(Float32 val)
{
  return *(volatile UInt32 *) &val;
}

void rm_to_ieee_block(UInt32 count, Float32 *src, UInt32 *dst)
{
  UInt32 index;

  count = count / sizeof(Float32);

  for (index = 0; index < count; index++) 
  {
    dst[index] = *(volatile UInt32 *) &src[index];
  }
}
#endif


/****************************************************************************/
/* rm_init_interface() preinitializes all variables with predefined values  */
/*                                                                          */
/* Parameters:  rm_number_of_elements       Number of elements              */
/*              rm_transformation_function  Pointer to the user defined     */
/*                                          transformation function         */
/*              rm_snapshot_function        Pointer to the user defined     */
/*                                          snapshot function               */
/*              buffer_size                 size of the memory to be        */
/*                                          allocated for the snapshot      */
/*                                          buffer                          */
/*                                                                          */
/* ReturnValue: NO_ERROR            o.k.                                    */
/*              RM_OUT_OF_MEMORY    dynamic memory missing or too small     */
/****************************************************************************/
RM_ERROR rm_init_interface(UInt32                    rm_number_of_elements,
                      RM_TRANS_FUNCTION_PTR          rm_transformation_function,
                      RM_SNAPSHOT_FUNCTION_PTR       rm_snapshot_function,
                      UInt32                         buffer_size )
{
   vcm_module_descriptor_type *vcm_module_ptr;

   vcm_module_ptr = vcm_module_register(VCM_MID_RMSERVICE,
                                        vcm_module_find(VCM_MID_RTLIB, NULL),
                                        VCM_TXT_RMSERVICE,
                                        RMSERVICE_VER_MAR, RMSERVICE_VER_MIR, RMSERVICE_VER_MAI,
                                        RMSERVICE_VER_SPB, RMSERVICE_VER_SPN, RMSERVICE_VER_PLV,
                                        0, 0);

  /* First perform basic initialization of descriptor structure */
   rm_interface.rm_sim_state       = RM_DSP_STOP;
   rm_interface.rm_sync_flag       = RM_NO_ACCESS;
   rm_interface.rm_num_members     = 0;
   rm_interface.rm_tsim            = 0.0;
   rm_interface.rm_tsim64          = 0.0;
   rm_interface.rm_service_state   = RM_PROCESSING;
   rm_interface.rm_member_data_ptr = (void*)0;
   rm_interface.rm_model_data_ptr  = (void*)0;
   rm_interface.rm_names_list_ptr  = (void*)0;
   rm_interface.rm_ring_buffer_ptr = (void*)0;
   rm_interface.rm_error_code      = RM_NO_ERROR;
   rm_interface.rm_mode_flag       = RM_NORMAL;

 /* initialize maneuver values */
   rm_interface.rm_maneuver_supported = RM_MANEUVER_UNSUPPORTED;
   rm_interface.rm_maneuver_start_time = 0.0;

   {
      int                 required_size;
      RM_MEMBER_DATA_PTR  ptr_member_data;
      RM_MEMBER_DATA_PTR  ptr_model_data;
      RM_DS_MEMBER_NAMES  *ptr_member_names;
      RM_RING_BUFFER      *ptr_ring_buf;

      /* Determine size of interface and name structure and allocate memory */
      required_size       = rm_number_of_elements * sizeof(RM_MEMBER_DATA);
      ptr_member_data     = (RM_MEMBER_DATA_PTR) malloc(required_size);
      ptr_model_data      = (RM_MEMBER_DATA_PTR) malloc(required_size);

      required_size       = rm_number_of_elements * sizeof(RM_DS_MEMBER_NAMES);
      ptr_member_names    = (RM_DS_MEMBER_NAMES*) malloc(required_size);
  
      required_size       = sizeof( RM_RING_BUFFER );
      ptr_ring_buf        = (RM_RING_BUFFER*) malloc(required_size);

      rm_interface.rm_snapshot_buffer_ptr =
                            (RM_SNAPSHOT_BUFFER_PTR*) malloc( buffer_size );

      /* Store function and buffer pointer */
      rm_interface.rm_transformation_function = rm_transformation_function;
      rm_interface.rm_snapshot_function       = rm_snapshot_function;

      /* Check for allocation success and fill the initialization values */
      /* Specific constants are used for initialization of the */
      /* translation vector and the diagonal and off-diagnoal elements */
      /* of the rotation matrix */
      if ( (ptr_model_data != (void*)0) && (ptr_member_data != (void*)0) &&
           (ptr_member_names != (void*)0) && (ptr_ring_buf != (void*)0) )
      {
        unsigned int i, j, k;

        /* Store the interface and names structure pointers */
        /* into the descriptor */
        rm_interface.rm_member_data_ptr  = ptr_member_data;
        rm_interface.rm_model_data_ptr   = ptr_model_data;
        rm_interface.rm_names_list_ptr   = ptr_member_names;
        rm_interface.rm_num_members      = rm_number_of_elements;
        rm_interface.rm_service_state    = RM_INIT;
        rm_interface.rm_mode_flag        = RM_NORMAL;
        rm_interface.rm_ring_buffer_ptr  = ptr_ring_buf;

        rm_interface.rm_ring_buffer_ptr->rm_nring_buf_elem  = 0;
        rm_interface.rm_ring_buffer_ptr->rm_downsampling    = 0;
        rm_interface.rm_ring_buffer_ptr->rm_dt              = 0;
        rm_interface.rm_ring_buffer_ptr->rm_count_isr       = 0;
        rm_interface.rm_ring_buffer_ptr->rm_top_elem_ptr    = (void*)0;
        rm_interface.rm_ring_buffer_ptr->rm_read_ptr        = (void*)0;
        rm_interface.rm_ring_buffer_ptr->rm_write_ptr       = (void*)0;
        rm_interface.rm_ring_buffer_ptr->rm_overflow        = RM_FALSE;

        rm_service_struct.InitFlag = RM_TRUE;

        /* Element loop */
        for ( i = 0; i < rm_interface.rm_num_members; i++ )
        {
           /* Initialize ids and contents of interface data strucutre */
           rm_interface.rm_member_data_ptr[i].rm_id = i;

           /* Column loop */
           for ( j = 0; j < RM_DIMENSION; j++ )
           {
              /* Row loop */
              for ( k = 0; k < RM_DIMENSION; k++ )
                 rm_interface.rm_member_data_ptr[i].rm_rot[j][k] =
                                                        RM_OFF_DIAGONAL_INIT;

              /* Correct setting of diagonal elements */
              /* and initialize translation */
              rm_interface.rm_member_data_ptr[i].rm_trans[j]  =
                                                 RM_TRANSLATE_INIT;
              rm_interface.rm_member_data_ptr[i].rm_rot[j][j] =
                                                 RM_DIAGONAL_INIT;
           }

           rm_interface.rm_model_data_ptr[i].rm_id =
                          rm_interface.rm_member_data_ptr[i].rm_id;

           rm_to_ieee_block( sizeof( Float32 ) * RM_DIMENSION,
              &(rm_interface.rm_member_data_ptr[i].rm_trans[0]),
              (UInt32 *) &(rm_interface.rm_model_data_ptr[i].rm_trans[0]) );

           rm_to_ieee_block( sizeof( Float32 ) * RM_DIMENSION * RM_DIMENSION,
              &(rm_interface.rm_member_data_ptr[i].rm_rot[0][0]),
              (UInt32 *) &(rm_interface.rm_model_data_ptr[i].rm_rot[0][0]) );

           /* Initialize ids and contents of names structure */
           rm_interface.rm_names_list_ptr[i].rm_id = i;
           rm_interface.rm_names_list_ptr[i].rm_names[0] = '\0';
        }
      }
      else
        rm_interface.rm_error_code = RM_OUT_OF_MEMORY;
   }


   if (rm_interface.rm_error_code == RM_NO_ERROR)
   {
      vcm_module_status_set(vcm_module_ptr,VCM_STATUS_INITIALIZED);
   }
   return( rm_interface.rm_error_code );
}

/****************************************************************************/
/* Clean-up function                                                        */
/*                                                                          */
/* Parameters:  none                                                        */
/*                                                                          */
/* ReturnValue: none                                                        */
/*                                                                          */
/****************************************************************************/
void rm_close_interface()
{
   /* First perform basic initialization of descriptor structure */
   rm_interface.rm_sim_state         = RM_DSP_STOP;
   rm_interface.rm_sync_flag         = RM_NO_ACCESS;
   rm_interface.rm_num_members       = 0;
   rm_interface.rm_tsim              = 0.0;
   rm_interface.rm_tsim64            = 0.0;
   rm_interface.rm_service_state     = RM_PROCESSING;
   rm_interface.rm_error_code        = RM_NO_ERROR;
   rm_interface.rm_mode_flag         = RM_NORMAL;

   if( rm_interface.rm_member_data_ptr != (void*)0 )
     free( rm_interface.rm_member_data_ptr );
   rm_interface.rm_member_data_ptr = (void*)0;

   if( rm_interface.rm_model_data_ptr != (void*)0 )
     free( rm_interface.rm_model_data_ptr );
   rm_interface.rm_model_data_ptr  = (void*)0;

   if( rm_interface.rm_names_list_ptr != (void*)0 )
     free( rm_interface.rm_names_list_ptr );
   rm_interface.rm_names_list_ptr  = (void*)0;

   if( rm_interface.rm_ring_buffer_ptr != (void*)0 )
     free( rm_interface.rm_ring_buffer_ptr );
   rm_interface.rm_ring_buffer_ptr = (void*)0;

   if( rm_interface.rm_snapshot_buffer_ptr != (void*)0 )
     rm_interface.rm_snapshot_buffer_ptr = (void*)0;

   if( rm_interface.rm_transformation_function != (void*)0 )
     rm_interface.rm_transformation_function = (void*)0;

   rm_service_struct.InitFlag = 0x5A5A;
}

/****************************************************************************/
/* Registration function                                                    */
/*                                                                          */
/* Parameters:  rm_id           Index of the member                         */
/*              rm_name[]       Name of the member                          */
/*                                                                          */
/* ReturnValue: RM_NO_ERROR         o.k.                                    */
/*              RM_OUT_OF_RANGE     wrong index                             */
/*              RM_NOT_INITIALIZED  rm_init_interface() failed or has not   */
/*                                  been called before.                     */
/*                                                                          */
/****************************************************************************/
RM_ERROR rm_set_member_name( unsigned int rm_id, char rm_name[] )
{
   int char_count;
   int copy_length;

   if ( rm_service_struct.InitFlag == RM_TRUE )
   {
     if ( rm_id < rm_interface.rm_num_members )
     {
        copy_length = strlen( rm_name );
        if(copy_length > RM_MAX_LENGTH_NAMES)
           copy_length = RM_MAX_LENGTH_NAMES;

        /* Copy name string to destination */
        for ( char_count = 0; char_count < copy_length; char_count++ )
          rm_interface.rm_names_list_ptr[rm_id].rm_names[char_count] =
                rm_name[char_count];

        /* Terminate name string */
        rm_interface.rm_names_list_ptr[rm_id].rm_names[char_count] = '\0';
     }
     else
        rm_interface.rm_error_code = RM_OUT_OF_RANGE;
  }
  else
     rm_interface.rm_error_code = RM_NOT_INITIALIZED;

  return (rm_interface.rm_error_code);
}

/****************************************************************************/
/* Preinitialize one current member, so that it does not have to be         */
/*                      transformed later                                   */
/*                                                                          */
/* Parameters:  current_transfer    current member to preinitialize         */
/*                                                                          */
/* ReturnValue: RM_NO_ERROR              o.k.                               */
/*              RM_OUT_OF_RANGE          wrong index                        */
/*              RM_NOT_INITIALIZED       rm_init_interface() failed or has  */
/*                                       not been called before.            */
/*                                                                          */
/****************************************************************************/
RM_ERROR rm_init_member( RM_MEMBER_DATA current_transfer )
{
  if( rm_service_struct.InitFlag == RM_TRUE )
  {
    if( current_transfer.rm_id < rm_interface.rm_num_members )
      rm_interface.rm_member_data_ptr[current_transfer.rm_id] =
                                                           current_transfer;
    else
      rm_interface.rm_error_code = RM_OUT_OF_RANGE;
  }
  else
    rm_interface.rm_error_code = RM_NOT_INITIALIZED;

  return (rm_interface.rm_error_code);
}

/****************************************************************************/
/*                                                                          */
/* Set one members current position, specified in homogeneous coordinates   */
/*                                                                          */
/* Parameters:  rm_id               Index of the member                     */
/*              homog_data_ptr      Pointer to the homogeneous data array   */
/*                                                                          */
/* ReturnValue: none                                                        */
/*                                                                          */
/****************************************************************************/
void rm_set_homog_member_pos( unsigned int             rm_id, 
                              RM_HOMOG_MEMBER_DATA_PTR homog_data_ptr )
{
  int row;
  int column;

  for (row = 0; row < RM_DIMENSION; row++)
  {
    rm_interface.rm_member_data_ptr[rm_id].rm_trans[row] = 
      (*homog_data_ptr)[3][row];
    for (column = 0; column < RM_DIMENSION; column++)
    {
      rm_interface.rm_member_data_ptr[rm_id].rm_rot[row][column] =
        (*homog_data_ptr)[column][row];
    }
  }
}

/****************************************************************************/
/* Returns a pointer to the member data of the requested member             */
/*                                                                          */
/* Parameters:  current_member_id     the id of the requested member        */
/*                                                                          */
/* ReturnValue: the requested member  o.k.                                  */
/*              NULL                  the requested member does not exist   */
/*              RM_NOT_INITIALIZED    rm_init_interface() failed or has not */
/*                                    been called before.                   */
/*                                                                          */
/****************************************************************************/
RM_MEMBER_DATA_PTR rm_get_member_data_ptr( UInt32 current_member_id )
{
  RM_MEMBER_DATA_PTR   rm_member_return = (void*)0;

  if( rm_service_struct.InitFlag == RM_TRUE )
  {
    if( (rm_service_struct.InitFlag == RM_TRUE) &&
        (current_member_id < rm_interface.rm_num_members) )
      rm_member_return =
            &(rm_interface.rm_member_data_ptr[ current_member_id ]);
  }
  else
     rm_interface.rm_error_code = RM_NOT_INITIALIZED;

  return (rm_member_return);
}

/****************************************************************************/
/* RealTime Function that calls the user defined snapshot function          */
/*              to store the model data into user defined buffers.          */
/*                                                                          */
/* Note: Requires successful initiliazation by rm_init_interface().         */
/*                                                                          */
/* Parameters:  rm_state       simulation state                             */
/*              tsim           simulation time                              */
/*                                                                          */
/* ReturnValue: none                                                        */
/*                                                                          */
/****************************************************************************/
void rm_snapshot(UInt32 rm_state, Float32 tsim)
{
  if( rm_interface.rm_service_state == RM_INIT )
  {
    if( rm_interface.rm_mode_flag == RM_NORMAL )
    {
       /* test if the output buffer can be written and is */
       /* not locked by the program */

       if( rm_interface.rm_sync_flag == RM_DSP_ACCESS )
       {
          rm_interface.rm_service_state = RM_PROCESSING;
          if (rm_interface.rm_snapshot_function != (void*)0)
            rm_interface.rm_snapshot_function(rm_interface.rm_snapshot_buffer_ptr);
          rm_interface.rm_sim_state = ( RM_STATE) rm_state;
          rm_interface.rm_tsim      = tsim;
          rm_interface.rm_tsim64    = (Float64) tsim;
          rm_interface.rm_service_state = RM_FROZEN;
       }
    }
    else
    {
       rm_interface.rm_ring_buffer_ptr->rm_count_isr++;

       if( rm_interface.rm_ring_buffer_ptr->rm_count_isr >=
           rm_interface.rm_ring_buffer_ptr->rm_downsampling )
       {
          rm_interface.rm_service_state = RM_PROCESSING;
          if (rm_interface.rm_snapshot_function != (void*)0)
            rm_interface.rm_snapshot_function(
                                     rm_interface.rm_snapshot_buffer_ptr);
          rm_interface.rm_sim_state                     = ( RM_STATE) rm_state;
          rm_interface.rm_tsim                          = tsim;
          rm_interface.rm_tsim64                        = (Float64) tsim;
          rm_interface.rm_ring_buffer_ptr->rm_count_isr = 0;
          rm_interface.rm_service_state = RM_FROZEN;
      }
    }
  }
}

/****************************************************************************/
/*                                                                          */
/*                                                                          */
/*                                                                          */
/* Parameters:  rm_state       simulation state                             */
/*                                                                          */
/* ReturnValue: none                                                        */
/*                                                                          */
/****************************************************************************/
void rm_set_sim_state( UInt32 rm_state )
{
  rm_interface.rm_sim_state = (RM_STATE) rm_state;
}

/****************************************************************************/
/* Background Function that calls the user defined transformation function  */
/*      to do the transformation for the model data. Later we convert the   */
/*      received model_data from dsp float to ieee format and store it into */
/*      the output buffer that will be read by the PC program               */
/*                                                                          */
/* Note: Requires successful initilization by rm_init_interface().          */
/*                                                                          */
/* Parameters:  none                                                        */
/*                                                                          */
/* ReturnValue: none                                                        */
/*                                                                          */
/****************************************************************************/
void rm_service_realmotion()
{
  unsigned int    i;

  /* calculate size of one ring buffer element and buffer size */
  unsigned int              ring_buffer_elem_size;
  unsigned int              ring_buffer_size;
  RM_RING_BUFFER_ELEMENT*   next_write_ptr;

  /* test if we have valid data in the snapshot buffer */
  if( rm_interface.rm_service_state == RM_FROZEN )
  {
     if( rm_interface.rm_mode_flag == RM_NORMAL )
     {
       /* test if the output buffer can be written and is */
       /* not locked by the program */
       if( rm_interface.rm_sync_flag == RM_DSP_ACCESS )
       {
          /* lock the snapshot buffer for RM_SERVICE_REALMOTION   */
          rm_interface.rm_service_state = RM_PROCESSING;

          for ( i = 0; i < rm_interface.rm_num_members; i++ )
             rm_interface.rm_member_data_ptr[i].rm_id = i;

          if (rm_interface.rm_transformation_function != (void*)0)
          {
            /* call the user defined transformation function if specified */
            rm_interface.rm_transformation_function(
                                    rm_interface.rm_snapshot_buffer_ptr,
                                    rm_interface.rm_member_data_ptr );
          }

          /* do the float conversion and store the data */
          /* into the output buffer */
          for ( i = 0; i < rm_interface.rm_num_members; i++ )
          {
             rm_interface.rm_model_data_ptr[i].rm_id =
                            rm_interface.rm_member_data_ptr[i].rm_id;

             rm_to_ieee_block(
                  sizeof(Float32) * (RM_DIMENSION+RM_DIMENSION*RM_DIMENSION),
                  &(rm_interface.rm_member_data_ptr[i].rm_trans[0]),
                  (UInt32 *) &(rm_interface.rm_model_data_ptr[i].rm_trans[0]) );
          }

          /* release the snapshot buffer for RM_SNAPSHOT */
          rm_interface.rm_service_state = RM_INIT;
       }
       rm_interface.rm_sync_flag = RM_HOST_ACCESS;

     }
     else  /* RING_BUFFERED MODE */
     {
       if( rm_interface.rm_sync_flag == RM_DSP_ACCESS )
       {
          rm_interface.rm_service_state = RM_PROCESSING;

          ring_buffer_elem_size = rm_interface.rm_num_members *            \
                                  sizeof( RM_MEMBER_DATA ) +               \
                                  sizeof(rm_sim_state_union) +             \
                                  sizeof(Float32);
          ring_buffer_size      = ring_buffer_elem_size *                  \
                  (rm_interface.rm_ring_buffer_ptr->rm_nring_buf_elem - 1);

          /* increment the write pointer and store in local variable */
          next_write_ptr = (RM_RING_BUFFER_ELEMENT*)                       \
                ((UInt32) rm_interface.rm_ring_buffer_ptr->rm_write_ptr +
                 ring_buffer_elem_size);

          /* wrap around, if end of buffer is reached */
          if( next_write_ptr > (RM_RING_BUFFER_ELEMENT*)                   \
             ((UInt32) rm_interface.rm_ring_buffer_ptr->rm_top_elem_ptr +
               ring_buffer_size) )
             next_write_ptr = (RM_RING_BUFFER_ELEMENT*)
                   (UInt32) rm_interface.rm_ring_buffer_ptr->rm_top_elem_ptr;

          /* read pointer reached?, if so, the buffer is full */
          if( next_write_ptr != rm_interface.rm_ring_buffer_ptr->rm_read_ptr )
          {
             /* set simulation states and times */
             rm_interface.rm_ring_buffer_ptr->rm_write_ptr->state.rm_sim_state =
                                                rm_interface.rm_sim_state;
             *((UInt32 *) &rm_interface.rm_ring_buffer_ptr->rm_write_ptr->rm_tsim)
                                       = rm_to_ieee ( rm_interface.rm_tsim );

             if (rm_interface.rm_transformation_function != (void*)0)
             {
               /* call the user defined transformation function if specified */
               rm_interface.rm_transformation_function(
                                       rm_interface.rm_snapshot_buffer_ptr,
                                       rm_interface.rm_member_data_ptr );
             }

             for( i = 0; i < rm_interface.rm_num_members; i++ )
             {
               rm_interface.rm_ring_buffer_ptr->rm_write_ptr->member[i].rm_id=i;

               rm_to_ieee_block(
                    sizeof(Float32) * (RM_DIMENSION+RM_DIMENSION*RM_DIMENSION),
                    &(rm_interface.rm_member_data_ptr[i].rm_trans[0]),
                    (UInt32 *) &(rm_interface.rm_ring_buffer_ptr->\
                                     rm_write_ptr->member[i].rm_trans[0]) );
             }

             /* store the incremented write pointer to current write pointer*/
             rm_interface.rm_ring_buffer_ptr->rm_write_ptr = next_write_ptr;
             rm_interface.rm_ring_buffer_ptr->rm_overflow  = RM_FALSE;
          }
          else
             rm_interface.rm_ring_buffer_ptr->rm_overflow = RM_TRUE;

          rm_interface.rm_service_state = RM_INIT;
       } /* RM_DSP_ACCESS */

     } /* RM_RING_BUFFERED_MODE */
  } /* RM_PROCESSING */
}

/****************************************************************************/
/*                                                                          */
/* Foreground Function that makes the previously calculated model data      */
/*      available for host access.                                          */
/*                                                                          */
/*                                                                          */
/* Note: Requires successful initiliazation by rm_init_interface().         */
/*                                                                          */
/* Parameters:  none                                                        */
/*                                                                          */
/* ReturnValue: none                                                        */
/*                                                                          */
/****************************************************************************/
void rm_foreground_service( double tsim )
{
  unsigned int    i;

  /* calculate size of one ring buffer element and buffer size */
  unsigned int              ring_buffer_elem_size;
  unsigned int              ring_buffer_size;
  RM_RING_BUFFER_ELEMENT*   next_write_ptr;

  /* check buffer mode */
  if( rm_interface.rm_mode_flag == RM_NORMAL )
  {
    /* test if the output buffer can be written and is */
    /* not locked by the program */
    if( rm_interface.rm_sync_flag == RM_DSP_ACCESS )
    {
       rm_interface.rm_tsim      = tsim;
       rm_interface.rm_tsim64    = tsim;

       for ( i = 0; i < rm_interface.rm_num_members; i++ )
          rm_interface.rm_member_data_ptr[i].rm_id = i;

       /* do the float conversion and store the data */
       /* into the output buffer */
       for ( i = 0; i < rm_interface.rm_num_members; i++ )
       {
          rm_interface.rm_model_data_ptr[i].rm_id =
                         rm_interface.rm_member_data_ptr[i].rm_id;

          rm_to_ieee_block(
               sizeof(Float32) * (RM_DIMENSION+RM_DIMENSION*RM_DIMENSION),
               &(rm_interface.rm_member_data_ptr[i].rm_trans[0]),
               (UInt32 *) &(rm_interface.rm_model_data_ptr[i].rm_trans[0]) );
       }
    }
    rm_interface.rm_sync_flag = RM_HOST_ACCESS;

  }
  else  /* RING_BUFFERED MODE */
  {
    rm_interface.rm_ring_buffer_ptr->rm_count_isr++;

    if(rm_interface.rm_ring_buffer_ptr->rm_count_isr >=
       rm_interface.rm_ring_buffer_ptr->rm_downsampling)
    {
      rm_interface.rm_ring_buffer_ptr->rm_count_isr = 0;

      if( rm_interface.rm_sync_flag == RM_DSP_ACCESS )
      {
         rm_interface.rm_tsim   = tsim;
         rm_interface.rm_tsim64 = tsim;

         ring_buffer_elem_size = rm_interface.rm_num_members *            \
                                 sizeof( RM_MEMBER_DATA ) +               \
                                 sizeof(rm_sim_state_union) +             \
                                 sizeof(Float32);
        
         ring_buffer_size      = ring_buffer_elem_size *                  \
                 (rm_interface.rm_ring_buffer_ptr->rm_nring_buf_elem - 1);

         /* increment the write pointer and store in local variable */
         next_write_ptr = (RM_RING_BUFFER_ELEMENT*)                       \
               ((UInt32) rm_interface.rm_ring_buffer_ptr->rm_write_ptr +
                ring_buffer_elem_size);

         /* wrap around, if end of buffer is reached */
         if( next_write_ptr > (RM_RING_BUFFER_ELEMENT*)                   \
            ((UInt32) rm_interface.rm_ring_buffer_ptr->rm_top_elem_ptr +
              ring_buffer_size) )
            next_write_ptr = (RM_RING_BUFFER_ELEMENT*)
                  (UInt32) rm_interface.rm_ring_buffer_ptr->rm_top_elem_ptr;

         /* read pointer reached?, if so, the buffer is full */
         if( next_write_ptr != rm_interface.rm_ring_buffer_ptr->rm_read_ptr )
         {
            /* set simulation states and times */
            rm_interface.rm_ring_buffer_ptr->rm_write_ptr->state.rm_sim_state =
                                               rm_interface.rm_sim_state;
            *((UInt32 *) &rm_interface.rm_ring_buffer_ptr->rm_write_ptr->rm_tsim)
                                      = rm_to_ieee ( rm_interface.rm_tsim );

            for( i = 0; i < rm_interface.rm_num_members; i++ )
            {
              rm_interface.rm_ring_buffer_ptr->rm_write_ptr->member[i].rm_id=i;

              rm_to_ieee_block(
                   sizeof(Float32) * (RM_DIMENSION+RM_DIMENSION*RM_DIMENSION),
                   &(rm_interface.rm_member_data_ptr[i].rm_trans[0]),
                   (UInt32 *) &(rm_interface.rm_ring_buffer_ptr->\
                                    rm_write_ptr->member[i].rm_trans[0]) );
            }

            /* store the incremented write pointer to current write pointer*/
            rm_interface.rm_ring_buffer_ptr->rm_write_ptr = next_write_ptr;
            rm_interface.rm_ring_buffer_ptr->rm_overflow  = RM_FALSE;
         }
         else
            rm_interface.rm_ring_buffer_ptr->rm_overflow = RM_TRUE;
      } /* RM_DSP_ACCESS */
    }

  } /* RM_RING_BUFFERED_MODE */
}

/****************************************************************************/
/*                                                                          */
/* API Function for setting/ resetting maneuver support.                    */
/*                                                                          */
/* Note: Requires successful initiliazation by rm_init_interface().         */
/*                                                                          */
/* Parameters:  maneuver_support  RM_MANEUVER_SUPPORTED                     */
/*                                RM_MANEUVER_UNSUPPORTED                   */
/*                                                                          */
/* ReturnValue: none                                                        */
/*                                                                          */
/****************************************************************************/
void rm_set_maneuver_support( RM_MANEUVER_SUPPORT maneuver_support )
{
  rm_interface.rm_maneuver_supported = maneuver_support;
}

/****************************************************************************/
/*                                                                          */
/* API Function for setting the maneuver start time.                        */
/*                                                                          */
/* Note: Requires successful initiliazation by rm_init_interface().         */
/*                                                                          */
/* Parameters:  maneuver_start_time  the start time of the maneuver         */
/*                                                                          */
/* ReturnValue: none                                                        */
/*                                                                          */
/****************************************************************************/
void rm_set_maneuver_start_time( double maneuver_start_time )
{
  if( RM_MANEUVER_SUPPORTED == rm_interface.rm_maneuver_supported )
  {
    rm_interface.rm_maneuver_start_time = maneuver_start_time;
  }
}




#undef __INLINE

