#include <heport.h>
#define DEF_NSPRINTF
#include <helq.h>
#include <hevt.h>

/* nsprintf( buffer, sizeof(buffer), "text %12.6f %u %lu", 3.1415926535, 465, 12347634 );
 * to be verified by program module ../qq/qqnsprin.cpp

  * %s	  - string
  * %v	  - Visible string
  * %f	  - double
  * %g	  - same as %f
  * %d	  - decimale integer
  * %ld	  - decimale long
  * %u	  - unsigned decimale integer
  * %lu	  - unsigned decimale long
  * %x	  - Hex integer


  See also
  snprintf() - comes with MSVC 5.0
*/

#ifndef LONGSIZE
#define LONGSIZE 32
#endif
#ifndef INTSIZE
#define INTSIZE 32
#endif

#define NEXTPARAM_AMD64 if ( p.li == &params ) p.li = &params2 ; \
			else if ( p.li == &params2 ) p.li = &params3 ; \
			else if ( p.li == &params3 ) p.li = &params4 ; \
			else if ( p.li == &params4 ) p.li = &paramsx ; \
					else p.li++ ;


extern "C" {

FUNCT void nsprintf(
char *dest ,			/* eax Destination */
int nrest_of_destintation , /* edi size of destination buffer */
const char *fmt ,		/* esi Formatstring, similar to printf */
long params				/* edx Pointer to all parameters */
#ifdef AMD64
, long params2			/* ecx */
, long params3			/* r8d */
, long params4			/* r9d */
, long paramsx			/* stack */
#endif
)
{
uint16_t fs ;					/* Fieldsize, if given */
uint16_t pr ;					/* precision, if given */
bool flag_field_size ;		/* Flag: fieldsize given */
bool flag_precision ;		/* Flag: precision given */
bool flag_long ;			/* Flag: long given (expl) %ld %lu */
bool flag_leading_zeroes ;					/* Flag: print leading zeroes */
bool flag_leading_plus ;	/* Flag: leading plus */

union {
	double *f ;
	float *fl ;
	char **s ;
	char *c ;
	int *i ;		/* Parameter, if parameter is int  */
	long *li ;		/* Parameter, if parameter is long	*/
		} p ;
int i ;
int iPowerten ;
unsigned int uiN ;
signed int siN ;
unsigned int base ;
long li ;
unsigned long lu ;
double w ;
double round_add ;
double pot ;
char *ps ;
char ch ;
char dbu[24] ;			/* enough for 64 bit long conversion */
int id ;				/* Number of digits before decimale point (dp) */
uint ip ;
char *startdest ;
char *newdest ;
char *pchr ;
char *pchw ;
double dEngineering ;
uint uPotence ;
int islen ;
int addlen ;
int iOut ;
bool flag_had_round ;

/* next instruction makes trouble with GNU for DOS
 * p.c = &params ;	even if params is defined as char
 * but p.i works :
 */
p.li = &params ;		 /* ebp-292 = ebp+20 only for 8088, incompatible to 68000,VAX and others */

for(;;)
	{
	if ( *fmt == 0 )		/* * ebp-292 edx */
		{
		if ( nrest_of_destintation > 0 )	/* if there is space available */
			{
			*dest++ = 0 ;
			}
		return ;
		}
	if ( nrest_of_destintation <= 1 )		/* ebp+12 */
		{
		goto M_not_enough_space ;
		}
	if ( *fmt != '%' )
		{
		/* Just string, no Format */
		*dest++ = *fmt++ ;				/* *ebp+8 Mark: String buffer too small */
		--nrest_of_destintation ;		/* ebp+12 */
		continue ;
		}
	/* else, there is a % sign found, format a variable */
	++fmt ;								/* ebp+16  * Skip % sign */
	if ( *fmt == '%' )
		{			/* Store a %% as a single % in destination */
		*dest++ = *fmt++ ;				/* *ebp+8 Mark: String buffer too small */
		--nrest_of_destintation ;		/* ebp+12 */
		continue ;
		}
	flag_long = false ;						/* Init all flags to 'not applied' */
	flag_field_size = false ;
	flag_precision = false ;
	flag_leading_zeroes = false ;
	flag_leading_plus = false ;
	fs = 8 ;							/* The standard field size */
	if ( *fmt == '+' )
		{
		flag_leading_plus = true ;
		++fmt ;
		}
	if ( *fmt == '0' )
		{
		flag_leading_zeroes = true ;
		++fmt ;
		}
	if ( *fmt >= '1' && *fmt <= '9' )
		{
		if ( asctouw( &fmt, &fs ) )		/* Get the field size */
			continue ;		/* Some bad number, ignore format */
		flag_field_size = true ;			/* Field size given */
		}
	if ( *fmt == '.' )
		{
		++fmt ;
		if ( asctouw( &fmt, &pr ) )		/* String interpret to unsigned int */
			continue ;		/* Some bad number, ignore format */
		flag_precision = true ;			/* precision given */
		}
	if ( *fmt == 'l' || *fmt == 'L' )			   /* Flag long */
		{
		++fmt ;
		flag_long = true ;
		}
	startdest = dest ;
	/* Accessories of format '%012.5' interpreted, now variable key char */
	switch ( *fmt )
		{

	case 'c' :
	case 'C' :
		++fmt ;
		*dest++ = *(p.c) ;
#ifdef AMD64
	NEXTPARAM_AMD64 ;
#else
		p.i++ ; /* Ignore high byte of char	 */
#endif
						/* therefore add 2 (sizeof int) to ptr */
		--nrest_of_destintation ;
		break ;

	case 's' :
	case 'S' :
		++fmt ;
		ps = *p.s ;
#ifdef AMD64
	NEXTPARAM_AMD64 ;
#else
	p.s++ ;
#endif
		if ( ps == 0 )
			{
			ps = "[NULL]" ;
			}
		for(;;)
			{
			if ( !(ch = *ps++) ) break ;		/* End at 0-Byte */
			*dest++ = ch ;						/* copy one byte to destination */
			--nrest_of_destintation ;
			if ( nrest_of_destintation < 1 )
				{
				goto M_not_enough_space ;
				}
			}
		break ;

	case 'v' :
	case 'V' :			/* Visible String %v */
		++fmt ;
#ifdef AMD64
	ps = *p.s ;
	NEXTPARAM_AMD64 ;
#else
		ps = *(p.s++) ;
#endif
		if ( ps == 0 )
			{
			if ( nrest_of_destintation < 7 )
				{
				goto M_not_enough_space ;
				}
			*dest++ = '[' ;
			*dest++ = 'N' ;
			*dest++ = 'U' ;
			*dest++ = 'L' ;
			*dest++ = 'L' ;
			*dest++ = ']' ;

			nrest_of_destintation -= 6;
			break ;
			}

		if ( nrest_of_destintation < 3 )
			{
			goto M_not_enough_space ;
			}
		*dest++ = '[' ;							/* Begin of visible string */
		--nrest_of_destintation ;

		for(;;)
			{
			if ( !(ch = *ps++) ) break ;		/* End at 0-Byte */
			if ( ch < ' ' || ch == '\\' || ch >= 0x7f || (ch & 0x80) || ch == ']' || ch == '[' )
				{
				if ( nrest_of_destintation < 5 )
					{
					goto M_not_enough_space ;
					}
				*dest++ = '\\' ;
				--nrest_of_destintation ;

				/* To be decoded and read by asctostq.cpp  asctostrquot() */
					 if ( ch == '\t' ) { *dest++ = 't' ; --nrest_of_destintation ; }
				else if ( ch == '\012' ) { *dest++ = 'l' ; --nrest_of_destintation ; }
				else if ( ch == '\015' ) { *dest++ = 'r' ; --nrest_of_destintation ; }
				else if ( ch == '\\' ) { *dest++ = '\\' ; --nrest_of_destintation ; }
				else
					{		/* All other codes Octal */
					*dest++ = '0' + ( ( ch >> 6 ) & 0x03 ) ;
					--nrest_of_destintation ;
					*dest++ = '0' + ( ( ch >> 3 ) & 0x07 ) ;
					--nrest_of_destintation ;
					*dest++ = '0' + ( ( ch		) & 0x07 ) ;
					--nrest_of_destintation ;
					}
				}
			else
				{
				if ( nrest_of_destintation < 3 )
					{
					goto M_not_enough_space ;
					}
				*dest++ = ch ;						/* copy one byte to destination */
				--nrest_of_destintation ;
				}
			}

		if ( nrest_of_destintation < 1 )
			{
			goto M_not_enough_space ;
			}
		*dest++ = ']' ;							/* End of visible string */
		--nrest_of_destintation ;
		break ;


	case 'd' :
	case 'D' :
		++fmt ;
		base = 10 ;
		iOut = fs ;
		if ( flag_long )
			{										/* Long,  */
			li = *(p.li) ;
#ifdef AMD64
			NEXTPARAM_AMD64 ;
#else
			p.li++ ;
#endif
			if ( li < 0 )
				{
				if ( nrest_of_destintation < 3 )  goto M_not_enough_space ;		/* Not enough room for -4\00 */
				*dest++ = '-' ;
				--iOut ;
				--nrest_of_destintation ;
				lu = -li ;
				}
			else
				{
				if ( flag_leading_plus )
					{
					if ( nrest_of_destintation < 2 )
						{
						goto M_not_enough_space ;
						}
					*dest++ = '+' ;
					--iOut ;
					--nrest_of_destintation ;
					}
				lu = li ;
				}
			flag_leading_plus = false ;
			/* must have set iOut and lu */
			goto M_fmt_lu ;
			}
		else
			{
			i = *(p.i) ;
#ifdef AMD64
			NEXTPARAM_AMD64 ;
#else
			p.i++ ;
#endif
			if ( i < 0 )
				{
				if ( nrest_of_destintation < 4 )
					{
					goto M_not_enough_space ;	/* Not enough room for -4\00 */
					}
				*dest++ = '-' ;
				--iOut ;
				--nrest_of_destintation ;
				uiN = -i ;
				}
			else
				{
				if ( flag_leading_plus )
					{
					if ( nrest_of_destintation < 2 )
						{
						goto M_not_enough_space ;
						}
					*dest++ = '+' ;
					--iOut ;
					--nrest_of_destintation ;
					}
				uiN = i ;
				}
			flag_leading_plus = false ;
			goto M_fmt_u ;	// Convert the number in uiN
			}


	case 'u' :
	case 'U' :									/* Unsigned dec. */
		++fmt ;
		base = 10 ;

		if ( flag_long )
			{										/* Long,  */
M_convert_hexl :
			lu = *(p.li) ;
#ifdef AMD64
			NEXTPARAM_AMD64 ;
#else
			p.li++ ;
#endif
			iOut = fs ;
			if ( flag_leading_plus )
				{
				if ( nrest_of_destintation < 2 )
					{
					goto M_not_enough_space ;
					}
				*dest++ = '+' ;
				--iOut ;
				--nrest_of_destintation ;
				}

M_fmt_lu :
			/* Convert lu to unsigned ASCII string */
			for ( iPowerten = 0 ; ; ++iPowerten )
				{
				dbu[iPowerten] = (char) ( ( lu % (uint32_t)base )+'0' ) ;
				if ( dbu[iPowerten] > '9' ) dbu[iPowerten] += 'A' - '9'-1 ;
				lu = lu / (uint32_t)base ;
				if ( flag_leading_zeroes )
					{
					if ( iPowerten+1 >= iOut ) break ;
					}
				else
					{
					if ( lu == 0 ) break ;
					}
				if ( iPowerten >= sizeof(dbu) ) break ;
				}
			/* iPowerten is now the last filled digit number */
			/* exp 1328 : now iPowerten = 3, dbu = "8231" */
			}
		else
			{
M_convert_hexs:
			uiN = *(p.i) ;
#ifdef AMD64
			NEXTPARAM_AMD64 ;
#else
			p.i++ ;
#endif

M_fmt_u:		// Convert the number in uiN
			for ( iPowerten = 0 ; ; ++iPowerten )
				{
				dbu[iPowerten] = (char)( ( uiN % base ) + '0' ) ;
				if ( dbu[iPowerten] > '9' ) dbu[iPowerten] += 'A' - '9'-1 ;
				uiN = uiN / base ;
				if ( uiN == 0 ) break ;
				if ( iPowerten >= sizeof(dbu) ) break ;
				}
			}
		/* dbu[iPowerten] is the last convertet digit. the highmost of the number */
		/* dbu[0] is the units digits */
		/* dbu[1] is the tens digits */
		/* %d %ld %u %lu */
		if ( nrest_of_destintation < iPowerten+2 )	goto M_not_enough_space ;
/*		if ( flag_field_size )
			{
			if ( fs >= nrest_of_destintation ) goto M_not_enough_space ;
			int iPadding ;
			iPadding = fs - iPowerten - 1 ;
			while( iPadding > 0 )
				{
				--iPadding ;
				*dest++ = flag_leading_zeroes ? '0' : ' ' ;
				--nrest_of_destintation ;
				}
			}
*/
		/* iPowerten is the index of the high most digit in dbu */
		if ( nrest_of_destintation < iPowerten+2 ) goto M_not_enough_space ;
		for(;;)
			{
			*dest++ = dbu[iPowerten] ;
			--nrest_of_destintation ;
			--iPowerten ;
			if ( iPowerten < 0 ) break ;
			}
		break ;


	case 'x' :
	case 'X' :
		++fmt ;
		base = 16 ;
		iOut = fs ;
		if ( flag_long ) goto M_convert_hexl ;
		goto M_convert_hexs ;

	case 'f' :
	case 'F' :
	case 'g' :
	case 'G' :					/* Somehow double */
		++fmt ;
		w = *(p.f) ;
#ifdef AMD64
		NEXTPARAM_AMD64 ;
#else
		p.f++ ;
#endif
		iOut = 0 ;
		if ( w < 0.0 )
			{
			if ( nrest_of_destintation < 2 )
				{
				goto M_not_enough_space ;
				}
			*dest++ = '-' ;
			++iOut ;
			--nrest_of_destintation ;
			w = -w ;
			}
		else
			{
			if ( flag_leading_plus )
				{
				if ( nrest_of_destintation < 2 )
					{
					goto M_not_enough_space ;
					}
				*dest++ = '+' ;
				++iOut ;
				--nrest_of_destintation ;
				}
			}
		if ( w > 1000000.0 )
			{				/* Force E - format */
			pot = 1.0 ;
			i = 0 ;
			while ( w/pot > 1000.0 )
				{
				pot *= 1000.0 ;
				i += 3 ;
				}
			w /= pot ;
			uiN = (uint)w ;
			if ( uiN > 999 ) uiN = 999 ;

			if ( uiN >= 100 )
				{
				*dest++ = (char)( '0'+ uiN/100 );
				--nrest_of_destintation ;
				w -= 100.0 * (uiN/100) ;
				uiN %= 100 ;
				}
			if ( uiN >= 10 )
				{
				*dest++ = (char)( '0'+uiN/10 );
				w -= 10.0 * (uiN/10) ;
				--nrest_of_destintation ;
				uiN %= 10 ;
				}
			*dest++ = (char)( '0'+uiN );
			--nrest_of_destintation ;
			/* all the digits behind the DP. */
			w -= 1.0 * (uiN/1) ;
			*dest++ = '.' ;
			--nrest_of_destintation ;
			if ( !flag_precision ) pr = 8 ;
			for ( ip = 0 ; ip < pr ; ++ip )
				{
				if ( nrest_of_destintation < 4 )
					{
					goto M_not_enough_space ;
					}
				uiN = (uint)(w*10.0) ;
				if ( uiN > 9 ) uiN = 9 ;
				*dest++ = (char)( '0'+uiN );
				--nrest_of_destintation ;
				w = w*10.0 - (double)uiN ;
				}
			if ( nrest_of_destintation < 3 )
				{
				goto M_not_enough_space ;
				}
			*dest++ = 'E' ;
			--nrest_of_destintation ;
			uiN = i ;						/* Exponent */
			base = 10 ;
			flag_long = false ;			/* Init all flags to 'not applied' */
			goto M_fmt_u ;				/* Exponent as dezimal */
			}

		if ( !flag_precision )
			{
			if ( flag_field_size )
				{
				if ( fs > iOut+1 )
					{
					pr = fs - iOut - 1 ;
					}
				else
					{
					pr = 0 ;
					}
				}
			else
				pr = 5 ;
			}

		flag_had_round = false ;

		if ( w >= 100000.0 )
			{
			w /= 1000000.0 ; id = 6 ;
			}
		else if ( w >= 10000.0 )
			{
			w /= 100000.0 ; id = 5 ;
			}
		else if ( w >= 1000.0 )
			{
			w /= 10000.0 ; id = 4 ;
			}
		else if ( w >= 100.0 )
			{
			w /= 1000.0 ; id = 3 ;
			}
		else if ( w >= 10.0 )
			{
			w /= 100.0 ; id = 2 ;
			}
		else if ( w >= 1.0 )
			{
			w /= 10.0 ; id = 1 ;
			}
		else
			{
			if ( nrest_of_destintation < 4 )
				{
				goto M_not_enough_space ;
				}
			*dest++ = '0' ;
			++iOut ;
			--nrest_of_destintation ;
			*dest++ = '.' ;
			++iOut ;
			--nrest_of_destintation ;
			id = 0 ;
			round_add = 0.5 ;
			for ( uiN = 0 ; uiN < pr ; ++uiN )
				{
				round_add /= 10.0 ;
				}
			w += round_add ;
			flag_had_round = true ;
			}

		if ( !flag_precision )
			{
			if ( flag_field_size )
				{
				if ( pr > id ) pr -= id ;
				else pr = 0 ;
				}
			}

		if ( !flag_had_round )
			{
			round_add = 0.5 ;
			for ( siN = 0 ; siN < id+pr ; ++siN )
				{
				round_add /= 10.0 ;
				}
			w += round_add ;
			flag_had_round = true ;
			}

		for(;;)
			{
			if ( id < 0 )		/* if has converted all pre dp digits */
				{
				if ( -id >= pr ) break ;	/* has converted enough */
				}

			if ( nrest_of_destintation < 2 && id > 0 )	/* enough for one more digit plus end 0	 */
				{
				goto M_not_enough_space ;
				}
			w *= 10.0 ;
			uiN = (uint)w ;
			if ( w <= 0.0 ) { uiN = 0 ; w = 0.0 ; }
			if ( uiN > 9 ) uiN = 9 ;
			*dest++ = (char)( '0'+uiN );
			--nrest_of_destintation ;
			w -= (double)uiN ;
			--id ;				/* This digit processed */
			if ( id == 0 && pr == 0 )
				{				/* no after decimale point digits to convert */
				break ;
				}
			if ( id == 0 )		/* create decimale point */
				{
				if ( nrest_of_destintation < 3 )
					{
					goto M_not_enough_space ;
					}
				*dest++ = '.' ;
				--nrest_of_destintation ;
				}
			}
		break ;



	case 'm' :
	case 'M' :					/* Somehow float */
		dEngineering = 1000.0 ;
		uPotence = 3 ;
		goto M_E ;
	case 'e' :
	case 'E' :					/* Somehow float */
		dEngineering = 10.000 ;
		uPotence = 1 ;
M_E: ;
		++fmt ;
		w = *(p.f) ;
#ifdef AMD64
		NEXTPARAM_AMD64 ;
#else
		p.f++ ;
#endif
		iOut = 0 ;
		if ( w < 0.0 )
			{
			if ( nrest_of_destintation < 5 )
				{
				goto M_not_enough_space ;
				}
			*dest++ = '-' ;
			--nrest_of_destintation ;
			w = -w ;
			}
		else
			{
			if ( flag_leading_plus )
				{
				if ( nrest_of_destintation < 5 )
					{
					goto M_not_enough_space ;
					}
				*dest++ = '+' ;
				--nrest_of_destintation ;
				}
			}
		/* Force E - format */
		pot = 1.0 ;
		i = 0 ;
		while ( w/pot > dEngineering )
			{
			pot *= dEngineering ;
			i += uPotence ;
			}
		w /= pot ;
		uiN = (uint)w ;
		if ( uPotence == 3 )
			{
			if ( uiN > 999 ) uiN = 999 ;
			}
		else
			{
			if ( uiN > 9 ) uiN = 9 ;
			}

		if ( uiN >= 100 )
			{
			*dest++ = (char)( '0'+ uiN/100 );
			--nrest_of_destintation ;
			w -= 100.0 * (uiN/100) ;
			uiN %= 100 ;
			}
		if ( uiN >= 10 )
			{
			*dest++ = (char)( '0'+uiN/10 );
			w -= 10.0 * (uiN/10) ;
			--nrest_of_destintation ;
			uiN %= 10 ;
			}
		*dest++ = (char)( '0'+uiN );
		--nrest_of_destintation ;
		/* all the digits behind the DP. */
		w -= 1.0 * (uiN/1) ;
		*dest++ = '.' ;
		--nrest_of_destintation ;
		if ( !flag_precision ) pr = 8 ;
		for ( ip = 0 ; ip < pr ; ++ip )
			{
			if ( nrest_of_destintation < 4 )
				{
				goto M_not_enough_space ;
				}
			uiN = (uint)(w*10.0) ;
			if ( uiN > 9 ) uiN = 9 ;
			*dest++ = (char)( '0'+uiN );
			--nrest_of_destintation ;
			w = w*10.0 - (double)uiN ;
			}
		if ( nrest_of_destintation < 5 )
			{
			goto M_not_enough_space ;
			}
		*dest++ = 'E' ;
		--nrest_of_destintation ;
		if ( i >= 0 )
			{
			lu = i ;						/* Exponent */
			*dest++ = '+' ;
			--nrest_of_destintation ;
			}
		else
			{
			lu = -i ;						/* Exponent */
			*dest++ = '-' ;
			--nrest_of_destintation ;
			}
		if ( lu < 10 )
			{
			*dest++ = '0' ;
			--nrest_of_destintation ;
			}
		base = 10 ;
		flag_long = false ;			/* Init all flags to 'not applied' */
		flag_leading_zeroes = false ;
		flag_leading_plus = false ;
		goto M_fmt_u ;				/* Exponent as dezimal */

	default:
		break ;

		}			/* End switch */
	islen = dest - startdest ;
	if ( flag_field_size )
		{
		if ( islen < fs && nrest_of_destintation > 1 )
			{
			addlen = fs - islen ;
			if ( addlen > nrest_of_destintation - 1 ) addlen = nrest_of_destintation - 1 ;
			newdest = dest + addlen ;
			pchr = dest - 1 ;
			pchw = newdest - 1 ;
			while( islen > 0 )
				{
				*pchw-- = *pchr-- ;
				--islen ;
				}
			nrest_of_destintation -= addlen ;
			while ( addlen > 0 )
				{
				if ( flag_leading_zeroes ) *pchw-- = '0' ; else *pchw-- = ' ' ;
				--addlen ;
				}
			dest = newdest ;
			}
		}
	}

M_not_enough_space:
if ( nrest_of_destintation > 2 )
	{
	*dest++ = '~' ;
	--nrest_of_destintation ;
	}
if ( nrest_of_destintation > 1 )
	{
	*dest++ = '0' ;
	--nrest_of_destintation ;
	}
return ;
}		/* end FUNCT nsprintf() */

}		/* end extern "C" */

