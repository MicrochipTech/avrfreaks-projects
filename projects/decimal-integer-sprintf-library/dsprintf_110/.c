/*****************************************************************************
Title  :   Decimal Integer Sprintf Library
Author :   SA Development
Version:   1.10
*****************************************************************************/

#include "stdarg.h"
#include "string.h"
#include "avr/pgmspace.h"

#include "dsprintf.h"

#include "dsprintf_settings.h"

//verify dsprintf_enable_maxint setting
#if (DSPRINTF_ENABLE_MAXINT==64)
#define TEMPSTRING_MAX 22
#elif (DSPRINTF_ENABLE_MAXINT==32)
#define TEMPSTRING_MAX 12
#elif (DSPRINTF_ENABLE_MAXINT==16)
#define TEMPSTRING_MAX 7
#else
#error DSPRINTF_ENABLE_MAXINT must be 16, 32, or 64
#endif

//flags
#define FL_ZEROFILLED         (_BV(0)) //zerofilled takes priority over left justify if both are specified
#define FL_LEFTJUSTIFY        (_BV(1))
#define FL_SHOWPLUSSIGN       (_BV(2))
#define FL_NEGATIVE           (_BV(3))
#define FL_INT64              (_BV(4))
#define FL_INT32              (_BV(5))

//format error string if error checking is enabled
#if (DSPRINTF_ENABLE_ERRORCHECKING==1)
PROGMEM char FMTERR[]="(fmterr)";
#endif

//states
enum {ST_COPY, ST_PERCENT, ST_WIDTH, ST_PRECISION, ST_CONVERSION, ST_RENDERSIGN, ST_RENDERPAD, ST_RENDERSTRING};

void dsprintf_(unsigned char APgm, char *AString, const char* AFmtString, va_list va)
  {
    unsigned char state,fmt_char,flags,width,precision,arg_chars;
    const char *fmt_ptr;
  	char *str_ptr,*arg_ptr,ts1[TEMPSTRING_MAX];

    //needless assignements to prevent unitialized warning, they consume flash but prevent the warning
    flags=0;
    width=0;
    precision=0;
    arg_chars=0;
    arg_ptr=NULL;

    //state machine loop    
    for(state=ST_COPY, fmt_ptr=AFmtString, str_ptr=AString;;)
      {
        //grab fmt_char from sram or eeprom
        fmt_char=(APgm?pgm_read_byte(fmt_ptr):*fmt_ptr);

        //exit loop if fmt_char is 0
        if (!fmt_char)
          {
            *str_ptr=0;
            return;
          }

        //process state
        switch(state)
          {
            case ST_COPY         : //look for % sign else add character to output
                                   if (fmt_char=='%')
                                     {
                                       //clear variables
                                       flags=0;
                                       width=0;
                                       precision=0;
                                       state=ST_PERCENT;
                                     }
                                   else *str_ptr++=fmt_char;
                                   break;

            case ST_PERCENT      : //process double %
                                   if (fmt_char=='%')
                                     {
                                       *str_ptr++=fmt_char;
                                       state=ST_COPY;
                                     }
                                   else
                                   //set zerofilled flag
                                   if (fmt_char=='0')
                                     flags|=FL_ZEROFILLED;
                                   else
                                   //set showplussign flag
                                   if (fmt_char=='+')
                                     flags|=FL_SHOWPLUSSIGN;
                                   else
                                   //set leftjustify flag
                                   if (fmt_char=='-')
                                     flags|=FL_LEFTJUSTIFY;
                                   else
                                   //move to precision if decimal point
                                   if (fmt_char=='.')
                                     state=ST_PRECISION;
                                   else
                                     {
                                       //if numeric lets move to width else move to conversion
                                       if (fmt_char>='1' && fmt_char<='9')
                                         state=ST_WIDTH;
                                       else state=ST_CONVERSION;
                                       continue; //using continue does not advance fmt_ptr
                                     }
                                   break;

            case ST_WIDTH        : //process numeric into width
                                   if (fmt_char>='0' && fmt_char<='9')
                                     {
                                       width*=10;
                                       width+=fmt_char-'0';
                                     }
                                   else
                                   //move to precision if decimal point
                                   if (fmt_char=='.')
                                     state=ST_PRECISION;
                                   else
                                     {
                                       //move to conversion
                                       state=ST_CONVERSION;
                                       continue; //using continue does not advance fmt_ptr
                                     }
                                   break;

            case ST_PRECISION    : //process numeric into precision
                                   if (fmt_char>='0' && fmt_char<='9')
                                     {
                                       precision*=10;
                                       precision+=fmt_char-'0';
                                     }
                                   else
                                     {
                                       //move to conversion
                                       state=ST_CONVERSION;
                                       continue; //using continue does not advance fmt_ptr
                                     }
                                   break;

            case ST_CONVERSION   : //process string
                                   if (fmt_char=='s' || fmt_char=='S')
                                     {
                                       char *ptr1;

                                       //get char* pointer
                                       arg_ptr=va_arg(va,char*);

                                       //count length
                                       for (ptr1=arg_ptr,arg_chars=0;(fmt_char=='S')?pgm_read_byte(ptr1):*ptr1;ptr1++)
                                         arg_chars++;

                                       //shorten if precision is specified and smaller
                                       if (precision && precision<arg_chars)
                                         arg_chars=precision;

                                       //render
                                       render1:
                                       flags&=~(FL_ZEROFILLED | FL_SHOWPLUSSIGN);
                                       if (flags & FL_LEFTJUSTIFY)
                                         state=ST_RENDERSTRING;
                                       else state=ST_RENDERPAD;
                                       continue; //using continue does not advance fmt_ptr
                                     }
                                   else
                                   //process integer
                                   if (fmt_char=='d' || fmt_char=='u')
                                     {
                                       #if (DSPRINTF_ENABLE_MAXINT==64)
                                       if (flags & FL_INT64)
                                         {
                                           unsigned long long ull1;
                                           unsigned char c2;

                                           //get unsigned long long pointer
                                           ull1=va_arg(va,unsigned long long);

                                           //handle negative
                                           if (fmt_char=='d' && ull1&0x8000000000000000)
                                             {
                                               flags|=FL_NEGATIVE;
                                               ull1=-(long long)ull1;
                                             }

                                           //convert to string
                                           for (arg_chars=21,c2=21-precision;arg_chars!=255;arg_chars--)
                                             {
                                               if (precision && c2==arg_chars)
                                                 ts1[arg_chars]='.';
                                               else
                                               if (ull1)
                                                 {
                                                   ts1[arg_chars]='0'+ull1%10;
                                                   ull1/=10;
                                                 }
                                               else ts1[arg_chars]='0';
                                             }
                                           arg_chars=22;
                                         }
                                       else
                                       #endif
                                       #if (DSPRINTF_ENABLE_MAXINT>=32)
                                       if (flags & FL_INT32)
                                         {
                                           unsigned long ul1;
                                           unsigned char c2,c3;

                                           //get unsigned long pointer
                                           ul1=va_arg(va,unsigned long);

                                           //handle negative
                                           if (fmt_char=='d' && ul1&0x80000000)
                                             {
                                               flags|=FL_NEGATIVE;
                                               ul1=-(long)ul1;
                                             }

                                           //convert to string
                                           for (arg_chars=11,c2=11-precision;arg_chars!=255;arg_chars--)
                                             {
                                               if (precision && c2==arg_chars)
                                                 c3='.';
                                               else
                                               if (ul1)
                                                 {
                                                   c3='0'+ul1%10;
                                                   ul1/=10;
                                                 }
                                               else c3='0';
                                               ts1[arg_chars]=c3;
                                             }
                                           arg_chars=12;
                                         }
                                       else
                                       #endif
                                         {
                                           unsigned int ui1;
                                           unsigned char c2,c3;

                                           //get unsigned int pointer
                                           ui1=va_arg(va,unsigned int);

                                           //handle negative
                                           if (fmt_char=='d' && ui1&0x8000)
                                             {
                                               flags|=FL_NEGATIVE;
                                               ui1=-(int)ui1;
                                             }

                                           //convert to string  
                                           for (arg_chars=6,c2=6-precision;arg_chars!=255;arg_chars--)
                                             {
                                               if (precision && c2==arg_chars)
                                                 c3='.';
                                               else
                                               if (ui1)
                                                 {
                                                   c3='0'+ui1%10;
                                                   ui1/=10;
                                                 }
                                               else c3='0';
                                               ts1[arg_chars]=c3;
                                             }
                                           arg_chars=7;
                                         }

                                       //remove preceding zeros
                                       render2:
                                       arg_ptr=ts1;
                                       while (*arg_ptr=='0' && arg_chars>1 && *(arg_ptr+1)!='.')
                                         {
                                           arg_ptr++;
                                           arg_chars--;
                                         }

                                       //render
                                       if ((flags & FL_ZEROFILLED) || (flags & FL_LEFTJUSTIFY))
                                         state=ST_RENDERSIGN;
                                       else state=ST_RENDERPAD;
                                       continue; //using continue does not advance fmt_ptr
                                     }
                                   else
                                   #if (DSPRINTF_ENABLE_PERCENTX==1)
                                   //process hexidecimal
                                   if (fmt_char=='x' || fmt_char=='X')
                                     {
                                       unsigned int ui1=0;
                                       unsigned char c2;

                                       //set length
                                       #if (DSPRINTF_ENABLE_MAXINT==64)
                                       if (flags & FL_INT64)
                                         arg_chars=16;
                                       else
                                       #endif
                                       #if (DSPRINTF_ENABLE_MAXINT>=32)
                                       if (flags & FL_INT32)
                                         arg_chars=8;
                                       else
                                       #endif
                                         arg_chars=4;

                                       //convert to string
                                       for (c2=arg_chars-1;c2!=255;c2--)
                                         {
                                           if ((c2+1)%4==0)
                                             ui1=va_arg(va,unsigned int);
                                           ts1[c2]='0'+ui1%16;
                                           ui1/=16;
                                         }

                                       //convert values over 9 to A-F or a-f
                                       for (c2=0;c2<arg_chars;c2++)
                                         {
                                           if (ts1[c2]>'9')
                                             ts1[c2]+=(fmt_char=='X')?7:39;
                                         }

                                       goto render2;
                                     }
                                   else
                                   #endif
                                   #if (DSPRINTF_ENABLE_MAXINT==64)
                                   //set int64 flag
                                   if (fmt_char=='L')
                                     flags|=FL_INT64;
                                   else
                                   #endif
                                   #if (DSPRINTF_ENABLE_MAXINT>=32)
                                   //set int32 or int64 flags
                                   if (fmt_char=='l')
                                     {
                                       #if (DSPRINTF_ENABLE_MAXINT==64)
                                       //if int32 is already set move to int64
                                       if (flags & FL_INT32)
                                         flags|=FL_INT64;
                                       else
                                       #endif
                                       //set int32
                                       flags|=FL_INT32;
                                     }
                                   else
                                   #endif
                                   #if (DSPRINTF_ENABLE_PERCENTC==1)
                                   //character
                                   if (fmt_char=='c')
                                     {
                                       //get char (sent as an int)
                                       ts1[0]=va_arg(va,int);

                                       //setup arg values for rendering
                                       arg_ptr=ts1;
                                       arg_chars=1;

                                       //render
                                       goto render1;
                                     }
                                   else
                                   #endif
                                   #if (DSPRINTF_ENABLE_PERCENTQW==1)
                                   //fill character
                                   if (fmt_char=='q' || fmt_char=='w')
                                     {
                                       //if precision is not set get int as character to fill with
                                       //user can also specify the character to fill with as
                                       //the precision
                                       if (!precision)
                                         precision=va_arg(va,int);

                                       //w conversion fills to a specific string length depending on the existing string size
                                       if (fmt_char=='w')
                                         {
                                           if (str_ptr-AString>width)
                                             width=0;
                                           else width=width-(str_ptr-AString);
                                         }

                                       //fill  
                                       while (width)
                                         {
                                           *str_ptr++=precision;
                                           width--;
                                         }

                                       //move back to copy state  
                                       state=ST_COPY;
                                     }
                                   else
                                   #endif
                                     {
                                       //bad conversion character                                       
                                       #if (DSPRINTF_ENABLE_ERRORCHECKING==1)
                                       for (arg_ptr=FMTERR;pgm_read_byte(arg_ptr);arg_ptr++)
                                         *str_ptr++=pgm_read_byte(arg_ptr);
                                       #endif

                                       //move back to copy state  
                                       state=ST_COPY;
                                       continue; //using continue does not advance fmt_ptr
                                     }
                                   break;

            case ST_RENDERSIGN   : //add negative sign to output if negative flag is set
                                   if (flags & FL_NEGATIVE)
                                     *str_ptr++='-';
                                   else
                                   //add plus sign if flag is set and not negative
                                   if (flags & FL_SHOWPLUSSIGN)
                                     *str_ptr++='+';

                                   //move through renderstates to process flag settings
                                   if (flags & FL_ZEROFILLED)
                                     state=ST_RENDERPAD;
                                   else state=ST_RENDERSTRING;
                                   continue; //using continue does not advance fmt_ptr

            case ST_RENDERPAD    : //add spaces or zeros to output
                                   while (width>arg_chars+(((flags & FL_NEGATIVE) || (flags & FL_SHOWPLUSSIGN))?1:0))
                                     {
                                       *str_ptr++=(flags & FL_ZEROFILLED)?'0':' ';
                                       width--;
                                     }

                                   //move through renderstates to process flag settings
                                   if (flags & FL_ZEROFILLED)
                                     {
                                       state=ST_RENDERSTRING;
                                       continue; //using continue does not advance fmt_ptr
                                     }
                                   else
                                   if (flags & FL_LEFTJUSTIFY)
                                     //move back to copy state
                                     state=ST_COPY;
                                   else
                                     {
                                       state=ST_RENDERSIGN;
                                       continue; //using continue does not advance fmt_ptr
                                     }
                                   break;

            case ST_RENDERSTRING : //add string to output
                                   {
                                     unsigned char c2;

                                     c2=arg_chars;
                                     while(c2)
                                       {
                                         *str_ptr++=(fmt_char=='S')?pgm_read_byte(arg_ptr):*arg_ptr;
                                         arg_ptr++;
                                         c2--;
                                       }
                                   }

                                   //move through renderstates to process flag settings
                                   if ((flags & FL_ZEROFILLED) || !(flags & FL_LEFTJUSTIFY))
                                     //move back to copy state
                                     state=ST_COPY;
                                   else
                                     {
                                       state=ST_RENDERPAD;
                                       continue; //using continue does not advance fmt_ptr
                                     }
                                   break;
          }

        //advance format string pointer
        fmt_ptr++;
      }
  }

void dsprintf(char *AString, const char* AFmtString, ...)
  {
    va_list va;

    va_start(va,AFmtString);
    dsprintf_(0,AString,AFmtString,va);
    va_end(va);
  }

void dsprintf_P(char *AString, const char* AFmtString, ...)
  {
    va_list va;

    va_start(va,AFmtString);
    dsprintf_(1,AString,AFmtString,va);
    va_end(va);
  }

