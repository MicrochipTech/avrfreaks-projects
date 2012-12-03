#include "Ptc_KTY81-210.h"


//******************************************************************************
// Descrizione  : PTC TY81/210
// -----------------------------------------------------------------------------
static const double PTC_KTY81_210[TEMP_ROW][6] PROGMEM = {
//Temperatura Gradi celsius, Coeff. % di temperatura, Res min,  ResTipica, Res Max in Ohm
  { -55, 0.99, 471 , 485 , 500 , 3.02  },
  { -50, 0.98, 495 , 510 , 524 , 2.92  },
  { -40, 0.96, 547 , 562 , 576 , 2.74  },
  { -30, 0.93, 603 , 617 , 632 , 2.55  },
  { -20, 0.91, 662 , 677 , 691 , 2.35  },
  { -10, 0.88, 726 , 740 , 754 , 2.14  },
  { 0  , 0.85, 794 , 807 , 820 , 1.91  },
  { 10 , 0.83, 865 , 877 , 889 , 1.67  },
  { 20 , 0.80, 941 , 951 , 962 , 1.41  },
  { 25 , 0.79, 980 , 990 , 1000, 1.27  },
  { 30 , 0.78, 1018, 1029, 1041, 1.39  },
  { 40 , 0.75, 1097, 1111, 1125, 1.64  },
  { 50 , 0.73, 1180, 1196, 1213, 1.91  },
  { 60 , 0.71, 1266, 1286, 1305, 2.19  },
  { 70 , 0.69, 1355, 1378, 1402, 2.49  },
  { 80 , 0.67, 1447, 1475, 1502, 2.8   },
  { 90 , 0.65, 1543, 1575, 1607, 3.12  },
  { 100, 0.63, 1642, 1679, 1716, 3.46  },
  { 110, 0.61, 1745, 1786, 1828, 3.83  },
  { 120, 0.58, 1849, 1896, 1943, 4.33  },
  { 125, 0.55, 1900, 1950, 2000, 4.66  },
  { 130, 0.52, 1950, 2003, 2056, 5.07  },
  { 140, 0.45, 2044, 2103, 2162, 6.28  },
  { 150, 0.35, 2124, 2189, 2254, 8.55  }
  };

// -----------------------------------------------------------------------------
// Converte il valore di resistenza in Temperatura
// -----------------------------------------------------------------------------
bool ConvRtoT( double Res, double *Tp, double *Tolleranza )
{
 int    i;
 double Rmin, 
        Rtyp,
        Rmax,
        K, 
        Temp, 
        RmaxPrec;

 //Troviamo la resistenza più vicina
 for( i = 0; i < TEMP_ROW; i++ ){
    
    Rmin  = pgm_read_float( &PTC_KTY81_210[ i ][ IDX_Rmin ] );
    Rmax  = pgm_read_float( &PTC_KTY81_210[ i ][ IDX_Rmax ] );
    Temp  = pgm_read_float( &PTC_KTY81_210[ i ][ IDX_T    ] );
    
    if( Res >= Rmin && Res <= Rmax ){
        *Tp = Temp;
        return true;
        }
    
    if( i == 0 ){
      RmaxPrec = Rmax;
      continue;
      }

     if(Res > RmaxPrec && Res < Rmin ){
       //non è un valore preciso: sta tra i due intervalli
       Rtyp         = pgm_read_float( &PTC_KTY81_210[ i ][ IDX_Rtyp ] );
       K            = pgm_read_float( &PTC_KTY81_210[ i ][ IDX_K    ] );
       *Tolleranza  = pgm_read_float( &PTC_KTY81_210[ i ][ IDX_Toll ] );
     
       *Tp = (( Res/Rtyp -1)*100) / K + Temp;

       return true;
       }

    RmaxPrec = Rmax;
    }
 return false ;
}
