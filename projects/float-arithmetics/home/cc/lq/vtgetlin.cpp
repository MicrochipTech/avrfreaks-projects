/* Programmed by Hans-Juergen Herbert
*/

#include <heport.h>
#include <helq.h>
#include <hevt.h>

extern void vtgetlineanz( char *pch, int cupos ) ;

/* ---------------------------------------------------------------------- */
FUNCT int vtgetlin(		// get a line from vtkey() echo to vtocha()
char	 *str ,			// where to save the string
int lns )				// size of string buffer, length = lns-1
{
register char *ptr ;
register int zzz ;
char uz ;
register int ind ;
int ii ;
char *ptr2 ;
bool bInclude ;
bool bDisplay ;

ptr = str ;
ind = 0 ;
bInclude = false ;
bDisplay = false ;

if ( lns <= 0 )
	{
	uprintf( "Data Field too small" );
	return 22 ;		// error : data field too small
	}
if ( lns > 1600 )	// neccesary for TCP single telegramme messages
	{
	uprintf( "vtgetlin: data field too long" );
	return 23 ;		// error : data field too big
	}

for( ; ; )
	{
	zzz = vtkey() ;
	if ( zzz == VTK_DELETE )	// The Delete Key
		{
		bDisplay = true ;
		ptr2 = ptr ;			// Shift from here
		for(;;)
			{
			if ( ptr2 >= str+lns-2 ) break ;
			*(ptr2) = *(ptr2+1) ;
			++ptr2 ;
			}
		ptr2 = ptr ;		// Display all characters right from here
		ii = 0 ;
		for(;;)
			{
			if ( ptr2 >= str+lns-2 ) break ;
			if ( *ptr2 >= 0 && *ptr2 < ' ' ) break ;
			if ( *ptr == 0 ) break ;
			vtocha( *(ptr2) ) ;
			++ii ;
			++ptr2 ;
			}
		vtocha( ' ' ) ; ++ii ;
		while ( ii ) { vtocha( 0x08 ) ; --ii ; }
		continue ;
		}
	if ( zzz == VTK_INSERT || zzz == 0x16 )		/* Include ^V */
		{
		bDisplay = true ;
		bInclude = (bool)!bInclude ;
		if ( bDisplay ) vtgetlineanz(str,ind);
		continue ;
		}
	if ( CUR_RIGHT(zzz) )	/* Control -D holt einzelne character */
		{		/* 17.3.1990 : statt <= jetzt < */
		if ( ind < lns-1 )	/* 1 Reserve fuer 0-Byte */
			{		/* Noch im String */
			zzz = *ptr ;
			if ( zzz < 0x20 || zzz == 0x7f ) continue ; /* hole keine Steuerzeichen */
#ifdef TEST
DBprintf("getline: character %xH wegen ^D\n",zzz) ;
#endif
			vtocha( zzz ) ; /* character ist abgespeichert */
			++ind ;
			++ptr ;
			}
		}

	else if ( zzz == 0x12 || zzz == VTK_END ) /* Control -R holt alle character */
		{												/* Die Taste END auch */
		for (;;)
			{
			if ( ind >= lns-1 ) /* 1 Reserve fuer 0-Byte */
				{		/* Nicht mehr im String */
				break ;
				}
			uz = *ptr ;
			if ( uz < 0x20 || uz == 0x7f ) break ; /* hole keine Steuerzeichen */
#ifdef TEST
DBprintf("getline: character %xH wegen ^R\n", zzz ) ;
#endif
			vtocha( uz ) ; /* character ist abgespeichert */
			++ind ;
			++ptr ;
			}
		if ( bDisplay ) vtgetlineanz(str,ind);
		}

	else if ( zzz < 0xff && zzz >= 0x20 && zzz != 0x7f )
		{			/* gueltiges character */
		if ( bInclude )
			{
			ptr2 = str+lns-2 ;	/* Neues Ziel */
			for(;;)
				{
				if ( ptr2 <= ptr ) break ;
				*ptr2 = *(ptr2-1) ;		/* Nach oben schieben */
				--ptr2 ;
				}
			}
		if ( ind < lns-1 )	/* 1 Reserve fuer 0-Byte */
			{		/* Noch im String */
			++ind ;
#ifdef TEST
DBprintf("getline: character %xH wegen Eingabe\n", zzz );
#endif
			vtocha( *ptr++ = zzz ) ; /* character ist abgespeichert */
			if ( bDisplay ) vtgetlineanz(str,ind);
			}
		}
	else if ( zzz == 0x08 || CUR_LEFT(zzz) || zzz == 0x7f )
		{
		if ( ind > 0 )
			{
			--ind ;
			--ptr ;
			vtocha(8) ;		// Backspace
			vtocha(' ') ;	// Blank
			vtocha(8) ;		// Backspace
			if ( bDisplay ) vtgetlineanz(str,ind);
			}
		}
	else if ( zzz == 0x15 ) // Control-U clears entire line
		{
		while( ind > 0 )
			{
			--ind ;
			--ptr ;
			vtocha(8) ;		// Backspace
			vtocha(' ') ;	// Blank
			vtocha(8) ;		// Backspace
			}
		if ( bDisplay ) vtgetlineanz(str,ind);
		}
	else if ( zzz == VTK_ACK || zzz == 0x1a )	// Abort by ESCAPE or Control Z
		{
		if ( bDisplay )
			{
			for (;;)
				{
				if ( ind >= lns-1 ) /* 1 Reserve fuer 0-Byte */
					{				// Not in string
					break ;
					}
				zzz = *ptr ;
				if ( zzz < 0x20 || zzz == 0x7f ) break ; /* hole keine Steuerzeichen */
#ifdef TEST
DBprintf("getline: character %xH wegen ^R\n", zzz ) ;
#endif
				vtocha( zzz ) ; /* character ist abgespeichert */
				++ind ;
				++ptr ;
				}
			 vtgetlineanz(str,ind);
			}
		*ptr = 0 ;
		return 44 ;		/* Code Abbruch */
		}
	else if ( zzz == VTK_ENTER )	 /* Abschluss */
		{
		if ( bDisplay )
			{
			for (;;)
				{
				if ( ind >= lns-1 ) /* 1 Reserve fuer 0-Byte */
					{		/* Nicht mehr im String */
					break ;
					}
				zzz = *ptr ;
				if ( zzz < 0x20 || zzz == 0x7f ) break ; /* hole keine Steuerzeichen */
#ifdef TEST
DBprintf("getline: character %xH wegen ^R\n", zzz ) ;
#endif
				vtocha( zzz ) ; /* character ist abgespeichert */
				++ind ;
				++ptr ;
				}
			 vtgetlineanz(str,ind);
			}
		*ptr = 0 ;
		return 0 ;
		}
	}	/* end for(;;) */
}		/* end vtgetlin() */

// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
// ----------------------------------------------------------------------
FUNCT void vtgetlineanz(
char *pch ,
int cupos )
{
int ii ;
int slen ;

ii = cupos ;
while ( ii ) { vtocha( 0x08 ) ; --ii ; }

slen = 0 ;
for(;;)
	{
	if ( *pch == 0 ) break ;
	vtocha( *(pch) ) ;
	++slen ;
	++pch ;
	}
vtocha( ' ' ) ; ++slen ;

ii = slen-cupos ;
while ( ii ) { vtocha( 0x08 ) ; --ii ; }
}

