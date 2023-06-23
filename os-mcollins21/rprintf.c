/*---------------------------------------------------*/
/* Public Domain version of printf                   */
/*                                                   */
/* Rud Merriam, Compsult, Inc. Houston, Tx.          */
/*                                                   */
/* For Embedded Systems Programming, 1991            */
/*                                                   */
/*---------------------------------------------------*/

#include "rprintf.h"
/*---------------------------------------------------*/
/* The purpose of this routine is to output data the */
/* same as the standard printf function without the  */
/* overhead most run-time libraries involve. Usually */
/* the printf brings in many kiolbytes of code and   */
/* that is unacceptable in most embedded systems.    */
/*---------------------------------------------------*/

static func_ptr out_char;
static int do_padding;
static int left_flag;
static int len;
static int num1;
static int num2;
static char pad_character;

size_t strlen(const char *str) {
    unsigned int len = 0;
    while(str[len] != '\0') {
        len++;
    }
    return len;
}

int tolower(int c) {
    if(c < 'a') { // Check if c is uppercase
        c -= 'a' - 'A';
    }
    return c;
}

int isdig(int c) {
    if((c >= '0') && (c <= '9')){
        return 1;
    } else {
        return 0;
    }
}




/*---------------------------------------------------*/
/*                                                   */
/* This routine puts pad characters into the output  */
/* buffer.                                           */
/*                                                   */
static void padding( const int l_flag)
{
   int i;

   if (do_padding && l_flag && (len < num1))
      for (i=len; i<num1; i++)
          out_char( pad_character);
   }

/*---------------------------------------------------*/
/*                                                   */
/* This routine moves a string to the output buffer  */
/* as directed by the padding and positioning flags. */
/*                                                   */
static void outs( charptr lp)
{
   if(lp == NULL)
      lp = "(null)";
   /* pad on left if needed                          */
   len = strlen( lp);
   padding( !left_flag);

   /* Move string to the buffer                      */
   while (*lp && num2--)
      out_char( *lp++);

   /* Pad on right if needed                         */
   len = strlen( lp);
   padding( left_flag);
   }

/*---------------------------------------------------*/
/*                                                   */
/* This routine moves a number to the output buffer  */
/* as directed by the padding and positioning flags. */
/*                                                   */
static void outnum(unsigned int num, const int base)
{
   charptr cp;
   int negative;
   char outbuf[32];
   const char digits[] = "0123456789ABCDEF";

   /* Check if number is negative                    */
   /* NAK 2009-07-29 Negate the number only if it is not a hex value. */
   if (num < 0L && base != 16L) {
      negative = 1;
      num = -num;
      }
   else
      negative = 0;

   /* Build number (backwards) in outbuf             */
   cp = outbuf;
   do {
      *cp++ = digits[num % base];
      } while ((num /= base) > 0);
   if (negative)
      *cp++ = '-';
   *cp-- = 0;

   /* Move the converted number to the buffer and    */
   /* add in the padding where needed.               */
   len = strlen(outbuf);
   padding( !left_flag);
   while (cp >= outbuf)
      out_char( *cp--);
   padding( left_flag);
}

/*---------------------------------------------------*/
/*                                                   */
/* This routine gets a number from the format        */
/* string.                                           */
/*                                                   */
static int getnum( charptr* linep)
{
   int n;
   charptr cp;

   n = 0;
   cp = *linep;
   while (isdig((int)*cp))
      n = n*10 + ((*cp++) - '0');
   *linep = cp;
   return(n);
}

/*---------------------------------------------------*/
/*                                                   */
/* This routine operates just like a printf/sprintf  */
/* routine. It outputs a set of data under the       */
/* control of a formatting string. Not all of the    */
/* standard C format control are supported. The ones */
/* provided are primarily those needed for embedded  */
/* systems work. Primarily the floaing point         */
/* routines are omitted. Other formats could be      */
/* added easily by following the examples shown for  */
/* the supported formats.                            */
/*                                                   */

void esp_printf( const func_ptr f_ptr, charptr ctrl, ...)
{
  va_list args;
  va_start(args, *ctrl);
  esp_vprintf(f_ptr, ctrl, args);
  va_end( args );
  
}

void esp_vprintf( const func_ptr f_ptr, charptr ctrl, va_list argp)
{

   int long_flag;
   int dot_flag;

   char ch;
   //va_list argp;

   //va_start( argp, ctrl);
   out_char = f_ptr;

   for ( ; *ctrl; ctrl++) {

      /* move format string chars to buffer until a  */
      /* format control is found.                    */
      if (*ctrl != '%') {
         out_char(*ctrl);
         continue;
         }

      /* initialize all the flags for this format.   */
      dot_flag   =
      long_flag  =
      left_flag  =
      do_padding = 0;
      pad_character = ' ';
      num2=32767;

try_next:
      ch = *(++ctrl);

      if (isdig((int)ch)) {
         if (dot_flag)
            num2 = getnum(&ctrl);
         else {
            if (ch == '0')
               pad_character = '0';

            num1 = getnum(&ctrl);
            do_padding = 1;
         }
         ctrl--;
         goto try_next;
      }

      switch (tolower((int)ch)) {
         case '%':
              out_char( '%');
              continue;

         case '-':
              left_flag = 1;
              break;

         case '.':
              dot_flag = 1;
              break;

         case 'l':
              long_flag = 1;
              break;
	
         case 'i':
         case 'd':
              if (long_flag || ch == 'D') {
                 outnum( va_arg(argp, long), 10L);
                 continue;
                 }
              else {
                 outnum( va_arg(argp, int), 10L);
                 continue;
                 }
         case 'x':
              outnum( (long)va_arg(argp, int), 16L);
              continue;

         case 's':
              outs( va_arg( argp, charptr));
              continue;

         case 'c':
              out_char( va_arg( argp, int));
              continue;

         case '\\':
              switch (*ctrl) {
                 case 'a':
                      out_char( 0x07);
                      break;
                 case 'h':
                      out_char( 0x08);
                      break;
                 case 'r':
                      out_char( 0x0D);
                      break;
                 case 'n':
                      out_char( 0x0D);
                      out_char( 0x0A);
                      break;
                 default:
                      out_char( *ctrl);
                      break;
                 }
              ctrl++;
              break;

         default:
              continue;
         }
      goto try_next;
      }
   va_end( argp);
   }

/*---------------------------------------------------*/
