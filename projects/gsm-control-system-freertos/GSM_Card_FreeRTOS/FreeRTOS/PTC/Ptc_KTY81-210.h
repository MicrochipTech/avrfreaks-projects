#ifndef PTC_KTY81_210H
#define PTC_KTY81_210H

#include <avr/pgmspace.h>
#include <stdbool.h>

#define TEMP_ROW 24
#define IDX_T     0
#define IDX_K     1
#define IDX_Rmin  2
#define IDX_Rtyp  3
#define IDX_Rmax  4
#define IDX_Toll  5

bool ConvRtoT( double , double *, double *);

// ------------------------------------------------------------------------------
#endif

