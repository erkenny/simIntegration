/******************************************************************************
  FILE:
    DSPRINTF.C

  DESCRIPTION:
    Vsprintf and Printf Functions

  REMARKS:

  AUTHORS:  adapted by R. Leinfellner, Markus Gunske

  Copyright (c) 1998 dSPACE GmbH, GERMANY

  $RCSfile: DSPRINTF.C $ $Revision: 1.9 $ $Date: 2008/07/28 09:46:38GMT+01:00 $
******************************************************************************/

#ifndef _VSPRINTF_C_
#define _VSPRINTF_C_

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdarg.h>
#include <dsprintf.h>
#include <dstypes.h>

#ifdef _DS1603
#include <dsmpc5554def.h>
#endif

#define ZEROPAD 1               /* pad with zero */
#define SIGN    2               /* unsigned/signed long */
#define PLUS    4               /* show plus */
#define SPACE   8               /* space if plus */
#define LEFT    16              /* left justified */
#define SPECIAL 32              /* 0x */
#define SMALL   64              /* use 'abcdef' instead of 'ABCDEF' */

#endif

#define INT_SIZE        1
#define LONG_SIZE       2
#define HALF_SIZE       3

static char *fbuf = 0;      /* buffer for format conversion */
static int fbufp;           /* pointer to buffer */
static int trail;           /* if 0 call trim-funktion if 1 not */

int vsnService = 0;
int BufferIdx = 0;

#define new_isdigit(c) ((c >= '0') && (c <= '9')) /* redefinition of isdigit() to avoid the */
                                                  /* linking of ctype.obj */

#ifdef _DS1603
#define DSPRINTF_MODF(f,iptr)  dsmpc5554_modff(f, iptr)
#else
#define DSPRINTF_MODF(f,iptr)  modf(f, iptr)
#endif

#ifdef _DS1603
#define CONST_0_0      0.0f
#define CONST_0_5      0.5f
#define CONST_1_0      1.0f
#define CONST_10_0     10.0f
#else
#define CONST_0_0      0.0
#define CONST_0_5      0.5
#define CONST_1_0      1.0
#define CONST_10_0     10.0
#endif

/* 
   For Platforms using the GNU compiler (like DS1006) this module implements the standard functions
   sprintf() and vsprintf() which are contained in the Microtec compiler library. Thus for platforms
   other than ds1006 these functions will get a prefix inside this module for avoiding conflicts with
   the corresponding functions from the compiler library.
*/
#ifdef _DS1006
#define DS_PREFIX(functionname) functionname
#else
#define DS_PREFIX(functionname) ds_ ## functionname
#endif


int BufferSize = 4096;
int BufferOverflow = 0;

#define CHECK_VSN_SERVICE       \
        if (vsnService)         \
        {                       \
            BufferIdx = 0;      \
            BufferOverflow = 0  \
        }

#define PRINTF_PF_DESTINATION_LIMIT_CHECK(cntVar)       \
        if ( vsnService && ((cntVar+1) >= BufferSize))  \
        {                                               \
            BufferOverflow =1;                          \
            return done;                                \
        } 

#define DESTINATION_LIMIT_CHECK_BREAK                   \
        if ( vsnService && (++BufferIdx >= BufferSize)) \
        {                                               \
            BufferOverflow =1;                          \
            break;                                      \
        }

#define DESTINATION_LIMIT_CHECK_CMD(cmd)                \
        if ( ! vsnService)                              \
        { cmd; }                                        \
        else                                            \
        {                                               \
          if( ! (++BufferIdx >= BufferSize))            \
          { cmd; }                                      \
        }
        
/*********************************************************************************************
**********... cut trailing zeros or decimal point...******************************************
*********************************************************************************************/
void trim(void)
{
  if (!trail)
  {
      while (fbuf[fbufp - 1] == '0')
	fbufp--;
      if (fbuf[fbufp - 1] == '.')
	fbufp--;
  }
}

/*********************************************************************************************
**********...correction of truncation errors...***********************************************
*********************************************************************************************/

dsfloat roundup(int exp)
{
#ifdef _DS1603
  static dsfloat d_tab[8]={0.05f, 0.005f, 0.0005f, 0.00005f, 0.000005f, 0.0000005f, 0.00000005f, 0.000000005f};
  static dsfloat m_tab[8]={5.0f, 50.0f, 500.0f, 5000.0f, 50000.0f, 500000.0f, 5000000.0f, 50000000.0f};
#else
  static dsfloat d_tab[8]={0.05, 0.005, 0.0005, 0.00005, 0.000005, 0.0000005, 0.00000005, 0.000000005};
  static dsfloat m_tab[8]={5, 50, 500, 5000, 50000, 500000, 5000000, 50000000};
#endif
  dsfloat f = CONST_0_0;

  if (exp == 0)
  {
    return CONST_0_5;
  }

  if(exp > 0 && exp < 9)
    return d_tab[exp-1];
  if(exp < 0 && exp > -9)
    return m_tab[-exp-1];

  if (exp > 0 )
  {
    f=d_tab[7];
    while (exp > 8)
    {
      f = f / CONST_10_0;
      exp--;
    }
  }
  else
    if (exp < 0)
    {
      f=m_tab[7];
      while (exp < -8)
      {
        f = f * CONST_10_0;
        exp++;
      }
    }
  return f;
}

/*********************************************************************************************
**********...transfer of integer part from a floating_point number...*************************
*********************************************************************************************/

int trunc (dsfloat f)
{
  dsfloat ipart;

  DSPRINTF_MODF(f, &ipart);

  return (int) ipart;
}

/*********************************************************************************************
**********...transfer of fractional part from a floating_point number...**********************
*********************************************************************************************/

dsfloat frac (dsfloat f)
{
  dsfloat ipart, fpart;

  fpart = DSPRINTF_MODF(f, &ipart);

  return fpart;
}

/*********************************************************************************************
**********...conversion from a double format (fnum) into a string format(s)*******************
*********************************************************************************************/

int printf_fp (char * s, dsfloat fnum, int width, int precision,
		int flags, char fmt)
{
  dsfloat f = fnum;
  int neg = 0;
  int e = 0;
  int done = 0;
  int i, j=0, g_ch=0;

#define put(ch) (fbuf[fbufp++] = ch)

  if (!fbuf)
    fbuf = (char *)malloc(256);

  if (fbuf)
  {

    fbufp = 0;
    trail = 1;

    if (f < CONST_0_0)
    {
      f = -f;
      neg = 1;
    }

    if (precision == -1)
      precision = 6;
   /* else if (precision > 7)
      precision = 7;
    */
    if ((fmt == 'g') || (fmt == 'G'))
      g_ch=1;

#if defined _DS1006 | defined _DS1603
    {
        /*
         * treat special cases of
         * inifinity or NaN
         */
        float f32 = f;
        unsigned int *f_uint32 = (unsigned int *)&f32;

        if (((*f_uint32 & 0x7F800000) == 0x7F800000)) {
            if ((*f_uint32 & 0x007fffff) == 0x0) {
                /* Infinity */
                char inf[] = "<Infinity>";

                strcpy(s, inf);

                return strlen(inf);
            } else {
                /* not a number */
                char nan[] = "<NaN>";

                strcpy(s, nan);

                return strlen(nan);
            }
        }
    }
#endif //DS1006 //DS1603

    if (f == CONST_0_0)
    {
      e =- precision;
      put('0');
      if (((precision > 0) && (g_ch!=1)) || ((flags & SPECIAL) && (precision==0))
	     || ((flags & SPECIAL) && (g_ch==1) ))
        put ('.');
      if ( ((precision > 0) && (g_ch!=1)) || ((flags & SPECIAL) && (g_ch==1)) )
      {
        while (e < 0)
        {
          put ('0');
          e++;
        }
      }
    } else if (f < CONST_1_0)
           {
             do
             {
               f = CONST_10_0 * f;
               e--;
             } while (f < CONST_1_0);
           } else if (f >= CONST_10_0)
                  {
                    do
                    {
                      f = f / CONST_10_0;
                      e++;
                    } while (f >= CONST_10_0);
                  }

    if ((fmt == 'g') || (fmt == 'G'))
    {
      if( (e < -4) || (e >= precision ) )
      {
        if (flags & SMALL)
          fmt = 'e';
        else
          fmt = 'E';
        trail = 0;
      } else
      {
        fmt = 'f';
        trail = 0;
      }
    }
    if ((fmt == 'e') || (fmt == 'E'))
    {
      if (f != CONST_0_0)
      {
        f = f + roundup(precision);
        if (f >= CONST_10_0)
        {
          f = f / CONST_10_0;
          e++;
        }
        put ('0' + trunc(f));

        if ((flags & SPECIAL) && (precision==0))
          put ('.');

        if (precision > 0)
        {
          put ('.');
          for (i = 1; i <= precision; i++)
          {
            f = CONST_10_0 * frac(f);
            put ('0' + trunc(f));
          }
          trim();
        }
      }
      if (!(f == CONST_0_0 && g_ch == 1 ))
      {
        if (flags & SMALL)
          put ('e');
        else
          put ('E');
        if (e >= 0)
          put ('+');
        else
          put ('-');
        e = abs(e);
        if(e>99)
        {
          int e2;

          e2 = e;
          put('0' + e2 / 100);
          e2 = e2 % 100;
          put ('0' + e2 / 10);
          put ('0' + e2 % 10);
        } else
        {
      	  put ('0' + e / 10);
          put ('0' + e % 10);
        }
      }
    } else
    {
      if(f != CONST_0_0) /*---------------------------------------------------------*/
      {
        if(e >= 0)
        {
          f = f + roundup(precision + e);
        }
        else
        {
          f = f + roundup(precision);
        }

        if(f >= CONST_10_0)
        {
          f = f / CONST_10_0;
          e++;
        }

        /* if(f < CONST_1_0)
           {
             f = CONST_1_0;
           }
        */
      } /*--------------------------------------------------------------------*/

      if (e >= 0)
      {
        if (f != CONST_0_0)
        {
          while (e >= 0)
          {
            put ('0' + trunc(f));
            f = CONST_10_0 * frac(f);
            e--;
          }
          if ((precision > 0) || ((flags & SPECIAL) && (precision==0)))
            put ('.');
          if (precision > 0)
          {
            put ('0' + trunc(f));
            for (i = 1; i < precision; i++)
            {
              f=CONST_10_0 * frac(f);
              put ('0' + trunc(f));
            }
          }
          trim();
        }
      } else
      {
        if (!( f == CONST_0_0 && g_ch == 1))
        {
          put ('0');
          e++;
          if ((precision > 0) || ((flags & SPECIAL) && (precision==0)))
            put ('.');
          if (precision > 0)
          {
            j=0;
            while ((e < 0) && (j < precision))
            {
              put ('0');
              e++;
              j++;
            }
            for (i = 1; i <= precision; i++)
            {
              if (j < precision)
              {
                put ('0' + trunc(f));
                f= frac(f) * CONST_10_0;
                j++;
              }
            }
            trim();
          }
        }
      }
    }

  /* The number is all converted in 'fbuf'.
       Now write it with sign and appropriate padding.  */

    if (neg || (flags & PLUS) || (flags & SPACE))
      --width;

    width -= fbufp;

    if (!(flags & LEFT) && !(flags & ZEROPAD) && (width > 0))
         /* Pad with spaces on the left.  */
      while (width-- > 0)
      {
          PRINTF_PF_DESTINATION_LIMIT_CHECK(done);
          *s++ = ' ';
          done++;
      }

  /* Write the sign.  */
    if (neg)
    {
        PRINTF_PF_DESTINATION_LIMIT_CHECK(done);
        *s++ =  '-';
        done++;
    } else if (flags & PLUS)
           {
             PRINTF_PF_DESTINATION_LIMIT_CHECK(done);
             *s++ =  '+';
             done++;
           } else if (flags & SPACE)
                  {
                    PRINTF_PF_DESTINATION_LIMIT_CHECK(done);
                    *s++ =  ' ';
                    done++;
                  };

    if (!(flags & LEFT) && (flags & ZEROPAD) && (width > 0))
	   /* Pad with zeros on the left.  */
      while (width-- > 0)
      {
        PRINTF_PF_DESTINATION_LIMIT_CHECK(done);
        *s++ = '0';
        done++;
      };

    for(i=0; i < fbufp; i++)
    {
      PRINTF_PF_DESTINATION_LIMIT_CHECK(done);
      *s++ = fbuf[i];
      done++;
    }

    if (flags & LEFT)
	  /* Pad with spaces on the right.  */
      while (width-- > 0)
      {
        PRINTF_PF_DESTINATION_LIMIT_CHECK(done);
        *s++ = ' ';
        done++;
      }
  }

  return done;
}

/*********************************************************************************************
**********...string to integer...*************************************************************
*********************************************************************************************/

unsigned int skip_atoi(const char **s)
{
  int i=0;

  while (new_isdigit(**s))
	i = i*10 + ( *((*s)++) - '0');
  return i;
}

/*********************************************************************************************
**********...conversion from a integer format (num) into a string format(str)...**************
*********************************************************************************************/

char * inumber(char * str, long num, int base, int size, int precision ,int type)
{
  char c,sign,tmp[36];
  const char *digits="0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
  int i, help;
	
  if (type & SMALL) 
    digits="0123456789abcdefghijklmnopqrstuvwxyz";
  if (type & LEFT) 
    type &= ~ZEROPAD;
  if (base<2 || base>36)
    return 0;
  c = (type & ZEROPAD) ? (char)'0' : (char)' ' ;
  if (type&SIGN && num<0) 
  {
    sign='-';
    num = -num;
  }	else if (type & PLUS)
           sign='+';
         else if (type & SPACE)
                sign = ' ';
              else
                sign = (char)0;
  if (sign) 
    size--;
  if (type & SPECIAL)
    if (base == 16) 
      size -= 2;
    else if (base==8)
           size--;
  i=0;
  if (num==0)
    tmp[i++]='0';
  else 
  {
    unsigned long dnum = num;

    while (dnum != 0)  
    {
      help = dnum % base;
      if (help < 0 || help > 36) 
	    break;
      tmp[i++]=digits[help];
      dnum = dnum / base;
    }
  }

  if (i > precision) 
    precision = i;
  size -= precision;
    
/* Pad with spaces on the left.  */
  if (!(type & (ZEROPAD + LEFT)))
    while (size-->0)
    {
        DESTINATION_LIMIT_CHECK_CMD(*str++ = ' ';);        
    }

/* Write the sign.  */
  if (sign)
  {
      DESTINATION_LIMIT_CHECK_CMD(*str++ = sign;);
  }
    
  if (type & SPECIAL)
  {
    if (base==8)
    {
        DESTINATION_LIMIT_CHECK_CMD(*str++ = '0';);        
    }      
    else if (base==16)
         {
            DESTINATION_LIMIT_CHECK_CMD(*str++ = '0';);            
            DESTINATION_LIMIT_CHECK_CMD(*str++ = digits[33];);            
         }
  }
  
/* Pad with zeros or spaces on the left.  */
  if (!(type & LEFT))
  {
    while(size-- > 0)
    {
        DESTINATION_LIMIT_CHECK_CMD(*str++ = c;);        
    }
  }
        
  while(i < precision--)
  {
      DESTINATION_LIMIT_CHECK_CMD(*str++ = '0';);      
  } 
         
  while(i-- > 0)
  {
      DESTINATION_LIMIT_CHECK_CMD(*str++ = tmp[i];);      
  }
    
/* Pad with spaces on the right.  */
  while(size-- > 0)
  {
      DESTINATION_LIMIT_CHECK_CMD(*str++ = ' ';);      
  }  
           
  return str;
}

/*********************************************************************************************
*******...conversion from a unsigned integer format (num) into a string format(str)...********
*********************************************************************************************/
char * unumber(char * str, unsigned long num, int base, int size, int precision ,int type)
{
  char c,sign,tmp[36];
  const char *digits="0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
  int i, help;

  if (type & SMALL) 
    digits="0123456789abcdefghijklmnopqrstuvwxyz";
  if (type & LEFT) 
    type &= ~ZEROPAD;
  if (base<2 || base>36)
    return 0;
  c = (type & ZEROPAD) ? (char)'0' : (char)' ' ;
  if (type&SIGN && num<0) 
  {
    sign = '-';
    num = -num;
  } else if (type & PLUS)
           sign = 0;
         else if (type & SPACE)
                sign = ' ';
              else 
                sign = (char)0;
  if (sign) 
    size--;
  if (type & SPECIAL)
    if (base == 16) 
      size -= 2;
    else if (base == 8) 
           size--;
  i=0;
  if (num==0)
    tmp[i++]='0';
  else while (num!=0)
	   {
         help = num % base;
         if (help < 0 || help > 36) 
           break;
         tmp[i++] = digits[help];
         num = num / base;
       }
  if (i > precision) 
    precision = i;
  size -= precision;

/* Pad with spaces on the left.  */
  if (!(type & (ZEROPAD + LEFT)))
  {
      while(size-->0)      
      {
          DESTINATION_LIMIT_CHECK_CMD(*str++ = ' ';);          
      }
  }
          

/* Write the sign.  */
  if (sign)
  {
      DESTINATION_LIMIT_CHECK_CMD(*str++ = sign;);      
  }
    
        
  if (type&SPECIAL)
  {
    if (base==8)
    {
        DESTINATION_LIMIT_CHECK_CMD(*str++ = '0';);        
    }      
    else if (base==16)
         {
             DESTINATION_LIMIT_CHECK_CMD(*str++ = '0';);             
             DESTINATION_LIMIT_CHECK_CMD(*str++ = digits[33];);             
         }
  }
    

/* Pad with zeros or spaces on the left.  */
  if (!(type & LEFT))
  {
      while(size-- > 0)
      {
          DESTINATION_LIMIT_CHECK_CMD(*str++ = c;);          
      }      
  }  
    
  while(i < precision--)
  {
      DESTINATION_LIMIT_CHECK_CMD(*str++ = '0';);      
  }  
  
  while(i-- > 0)
  {
      DESTINATION_LIMIT_CHECK_CMD(*str++ = tmp[i];);      
  }  

/* Pad with spaces on the right.  */
    while(size-->0)
    {
        DESTINATION_LIMIT_CHECK_CMD(*str++ = ' ';);        
    }  
    return str;
}

/*********************************************************************************************
**********...Formats and outputs data to 'buf'...*********************************************
*********************************************************************************************/


/*********************************************************************************************
**********...Formats and stores a series of characters and values in 'buf'...*****************
*********************************************************************************************/

int DS_PREFIX(vsprintf)(char *buf, const char *fmt, va_list args)
{
  int len;
  int i;
  char * str;   // wird auf den Param buf zeigen
  char *s, ch;
  int *ip;

  int flags;              /* flags to number() */

  int field_width;        /* width of output field */
  int precision;          /* min. # of digits for integers; max
                             number of chars for from string */
  int done;
  int lenmod;

  for (str=buf ; *fmt ; ++fmt)
  {
    if (*fmt != '%')
    {
      DESTINATION_LIMIT_CHECK_BREAK;
      *str++ = *fmt;
      continue;
    }
			
	/* process flags */
    flags = 0;
    repeat:
    ++fmt;          /* this also skips first '%' */
    switch (*fmt) 
    {
      case '-': flags |= LEFT; goto repeat;
      case '+': flags |= PLUS; goto repeat;
      case ' ': flags |= SPACE; goto repeat;
      case '#': flags |= SPECIAL; goto repeat;
      case '0': flags |= ZEROPAD; goto repeat;
    }
		
	/* get field width */
    field_width = -1;
    if (new_isdigit(*fmt))
      field_width = (int)skip_atoi(&fmt);
    else if (*fmt == '*')
         {
			    /* it's the next argument */
           field_width = va_arg(args, int);
           ++fmt;                                
           if (field_width < 0) 
           {
             field_width = -field_width;
             flags |= LEFT;
           }
         }

	/* get the precision */
    precision = -1;
    if (*fmt == '.')
    {
      ++fmt;  
      if (new_isdigit(*fmt))
        precision = (int)skip_atoi(&fmt);
      else if (*fmt == '*')
           {
             /* it's the next argument */
             precision = va_arg(args, int);
             ++fmt;                       
           }
      if (precision < 0)
        precision = 6;
    }

	/* get length modifier */
    switch (*fmt)
    {
      case 'L':
      case 'l': lenmod = LONG_SIZE;
                fmt++;
                break;
      case 'h': lenmod = HALF_SIZE;
                fmt++;
                break;
      default : lenmod = INT_SIZE;
    }

	/* get format */
    switch (*fmt)
    {
      
/****************** the argument is printed as a character *******************/

      case 'c': ch= (flags & ZEROPAD) ? (char)'0' : (char)' ';

	/* Pad with zeros or spaces on the left.  */
                if (!(flags & LEFT))
                {  
                    while (--field_width > 0)
                    {
                        DESTINATION_LIMIT_CHECK_BREAK
                        *str++ = ch;
                    }  
                }
                
                DESTINATION_LIMIT_CHECK_BREAK
                *str = (unsigned char) va_arg(args, int);
                
                if (*str != 0)
                {
                    DESTINATION_LIMIT_CHECK_BREAK
                    str++;
                }  

	/* Pad with spaces on the right.  */
                while (--field_width > 0)
                {
                    DESTINATION_LIMIT_CHECK_BREAK
                    *str++ = ' ';
                }  
                
                break;

/********************* the argument is printed as a string ********************/

      case 's': s = va_arg(args, char *);
                len = strlen(s);
                if (precision < 0)
                  precision = len;
                else if (len > precision)
                       len = precision;
                ch = (flags & ZEROPAD) ? (char)'0' : (char)' ';

	/* Pad with zeros or spaces on the left.  */
                if (!(flags & LEFT))
                {  
                    while (len < field_width--)
                    {
                        DESTINATION_LIMIT_CHECK_BREAK
                        *str++ = ch;
                    }
                }
                
                for (i = 0; i < len; ++i)
                {
                    DESTINATION_LIMIT_CHECK_BREAK
                    *str++ = *s++;
                }  

	/* Pad with spaces on the right.  */
                while (len < field_width--)
                {
                    DESTINATION_LIMIT_CHECK_BREAK
                    *str++ = ' ';
                }  
                
                break;

/******************* unsigned octal conversion is performed ******************/

      case 'o': flags |= SMALL;
                DESTINATION_LIMIT_CHECK_BREAK
                str = unumber(str, (unsigned long) va_arg(args, unsigned int), 8,
                field_width, precision, flags);
                break;

/******* the argument is printed in an implementation-defined format *********/

      case 'p': if (field_width == -1) 
                {
                  field_width = 16;
                  flags |= ZEROPAD;
                }
                DESTINATION_LIMIT_CHECK_BREAK
                str = inumber(str,(long)va_arg(args, void *), 16,
                field_width, precision, flags);
                break;

/**************** unsigned hexadecimal conversion is performed ***************/

      case 'x': flags |= SMALL;
      case 'X': switch(lenmod)
                {
#ifdef _DS1006
                  case HALF_SIZE: DESTINATION_LIMIT_CHECK_BREAK
                                  str = unumber(str, (unsigned long) va_arg(args, int),16, field_width, precision, flags);
                                  break;
#else
                  case HALF_SIZE: DESTINATION_LIMIT_CHECK_BREAK
                                  str = unumber(str, (unsigned long) va_arg(args, unsigned short),16, field_width, precision, flags); 
                                  break;
#endif                                 
                  case LONG_SIZE: DESTINATION_LIMIT_CHECK_BREAK
                                  str = unumber(str, va_arg(args, unsigned long), 16, field_width, precision, flags);
                                  break;

                  default:        DESTINATION_LIMIT_CHECK_BREAK
                                  str = unumber(str, (unsigned long) va_arg(args, unsigned int), 16,field_width, precision, flags);
                }
                break;

/******************* signed decimal conversion is performed ******************/

      case 'd':
      case 'i': {
                  long llong;

                  flags |= SIGN;
                  switch(lenmod)
                  {
#ifdef _DS1006
                    case HALF_SIZE: llong = va_arg(args, int);
#else
                    case HALF_SIZE: llong = va_arg(args, short);
#endif
                                    break;
                    case LONG_SIZE: llong = va_arg(args, long);
                                    break;
                    default: llong = va_arg(args, int);
                  }
                  DESTINATION_LIMIT_CHECK_BREAK
                  str = inumber(str, llong, 10, field_width, precision, flags);
		        }	
                break;

/****************** unsigned decimal conversion is performed *****************/

      case 'u': switch(lenmod)
                {
#ifdef _DS1006
                  case HALF_SIZE: DESTINATION_LIMIT_CHECK_BREAK
                                  str = unumber(str, (unsigned long) va_arg(args, int), 10,field_width, precision, flags);
                                  break;
#else
                  case HALF_SIZE: DESTINATION_LIMIT_CHECK_BREAK
                                  str = unumber(str, (unsigned long) va_arg(args, unsigned short), 10,field_width, precision, flags);
                                  break;
#endif
                                                
                  case LONG_SIZE: DESTINATION_LIMIT_CHECK_BREAK
                                  str = unumber(str, va_arg(args, unsigned long), 10,field_width, precision, flags);
                                  break;

                  default:        DESTINATION_LIMIT_CHECK_BREAK
                                  str = unumber(str, (unsigned long) va_arg(args, unsigned int), 10,field_width, precision, flags);
                }
                break;

/*causes of characters output so far to be written into the designated integer */

      case 'n': ip = va_arg(args, int *);
                *ip = (str - buf);
                break;

/*********** signed decimal floating-point conversion is performed ***********/

      case 'f':
      case 'e': flags |= SMALL;
      case 'g': flags |= SMALL;
      case 'E':
      case 'G': done = printf_fp(str, (dsfloat) va_arg(args, double),
                field_width, precision, flags, *fmt);
                if (done > 0)
                  str += done;  
                break;

/********************* a single percent sign is performed ********************/
      default:  if (*fmt != '%')
                {
                    *str++ = '%';
                }  
                
                if (*fmt)
                {
                    *str++ = *fmt;
                }  
                else
                {
                    --fmt;
                }  
                
                break;
    }
  }
  *str = '\0';
  
  if (vsnService)
  {
      vsnService = 0;
      BufferIdx = 0;
      
      if (BufferOverflow)
      {
        BufferOverflow = 0;
        return -2;
      }
  }  
  return str-buf;
}

int vsnprintf(char *buf, size_t size, const char *fmt, va_list args)
{
    /*
    DESCRIPTION
    The function vsnprintf() is equivalent to the functions printf(),
    fprintf(), sprintf(), snprintf(), respectively, except that its
    called with a va_list instead of a variable number of arguments.
    
    The function do not call the va_end macro. Consequently, the value
    of ap is undefined after the call. The application should call va_end(ap)
    itself afterwards.
    
    RETURN VALUE
    Upon successful return, the function returns the number of characters
    printed  (not  including  the  trailing  '\0'  used  to  end  output to
    strings).
     
    The function vsnprintf() do not write more than size bytes
    (including the trailing '\0'). If the output was truncated due to this
    limit then the return value is the number of characters
    (not including the trailing '\0') which would have been written to
    the final string if enough space had been available. Thus, a return
    value of size or more means that the output was truncated. If an output
    error is encountered, a negative value is returned.
    */

    int FullStringLength;
    char* tmpBuf;

    /*
    The snprintf() and vsnprintf() functions conform to C99.
    Concerning the return value of snprintf(), SUSv2 and C99 contradict
    each other: when snprintf() is called with size=0 then SUSv2 stipulates
    an unspecified return value less than 1...
    */
    if (size < 1) return -1;

    tmpBuf = (char *)malloc(BufferSize);
    
    if ( ! tmpBuf) return -1;
    
    vsnService = 1;
    BufferIdx = 0;
    FullStringLength= vsprintf(tmpBuf, fmt, args);
    
    if ( ! FullStringLength)
    {
        free(tmpBuf);
        return -1;
    } 
    
    /*
    ...while C99 allows destination to be NULL in this case, and gives the
    return value (as always) as the number of characters that would have been
    written in case the output string has been large enough.
    */
    if (NULL == buf)
    {
        free(tmpBuf);
        return FullStringLength;
    }
    
    strncpy(buf, tmpBuf, size-1);
    free(tmpBuf);
    buf[size-1]='\0';
    
    return FullStringLength;
}

int DS_PREFIX(sprintf)(char * buf, const char *fmt, ...)
{
  va_list args;
  int i;

  va_start(args, fmt);
  i = DS_PREFIX(vsprintf)(buf,fmt,args);
  va_end(args);

  return i;
}

int snprintf(char* buf, size_t size, const char* fmt, ...)
{
  va_list args;
  int i;

  va_start(args, fmt);
  i = vsnprintf(buf,size,fmt,args);
  va_end(args);

  return i;
}
