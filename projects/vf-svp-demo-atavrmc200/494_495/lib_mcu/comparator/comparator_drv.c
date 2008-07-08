//! @file comparator_drv.c,v
//!
//! Copyright (c) 2004 Atmel.
//!
//! Please read file license.txt for copyright notice.
//!
//! @brief This file contains the low level functions for the analog comparators
//!
//! @version 1.5 pwm3-ac-ctrl-motor-0_0_6 $Id: comparator_drv.c,v 1.5 2005/09/05 08:29:40 ebouin Exp $
//!
//! @todo
//! @bug

//_____  I N C L U D E S ___________________________________________________
#include "config.h"
#include "comparator_drv.h"

//_____ M A C R O S ________________________________________________________


//_____ P R I V A T E    D E C L A R A T I O N _____________________________


//_____ D E F I N I T I O N ________________________________________________


//_____ D E C L A R A T I O N ______________________________________________
//! Configures the comparator0 accordingly to the comparator0 Defines
//! Configuration values.
//! Take care that you have to select the comparators' inputs ports as
//! analog inputs thanks to the DIDR0 and DIDR1 registers.
//!
#ifdef USE_COMP0 //!< this define is set on config.h file

void init_comparator0(void)
{
#   if (COMPARATOR0_IT == 1)
       Enable_comparator0_interrupt();
#   elif (COMPARATOR0_IT == 0)
       Disable_comparator0_interrupt();
#   else
#      error (COMPARATOR0_IT should be 0 or 1... See config.h file)
#   endif


#   if (COMPARATOR0_IT_EVENT == 3)
       Comparator0_it_on_output_rising_edge();
#   elif (COMPARATOR0_IT_EVENT == 2)
       Comparator0_it_on_output_falling_edge();
#   elif (COMPARATOR0_IT_EVENT == 0)
       Comparator0_it_on_output_toggle();
#   else
#      error (COMPARATOR0_IT_EVENT should be 0, 2 or 3... See config.h file)
#   endif

#   if (COMPARATOR0_NEGATIVE_INPUT == 5)
       Set_comparator0_ni_dac();
#   elif (COMPARATOR0_NEGATIVE_INPUT == 4)
       Set_comparator0_ni_acmp();
#   elif (COMPARATOR0_NEGATIVE_INPUT == 3)
       Set_comparator0_ni_vref_1_60();
#   elif (COMPARATOR0_NEGATIVE_INPUT == 2)
       Set_comparator0_ni_vref_2_13();
#   elif (COMPARATOR0_NEGATIVE_INPUT == 1)
       Set_comparator0_ni_vref_3_20();
#   elif (COMPARATOR0_NEGATIVE_INPUT == 0)
       Set_comparator0_ni_vref_6_40();
#   else
#      error (COMPARATOR0_NEGATIVE_INPUT should be 0, 1, 2, 3, 4 or 5... See config.h file)
#   endif
    Enable_comparator0();
}
#endif // USE_COMP0

//! Configures the comparator1 accordingly to the comparator1 Defines
//! Configuration values.
//! Take care that you have to select the comparators' inputs ports as
//! analog inputs thanks to the DIDR0 and DIDR1 registers.
//!
#ifdef USE_COMP1 //!< this define is set on config.h file

void init_comparator1(void)
{
    Enable_comparator1();
#   if (COMPARATOR1_IT == 1)
       Enable_comparator1_interrupt();
#   elif (COMPARATOR1_IT == 0)
       Disable_comparator1_interrupt();
#   else
#      error (COMPARATOR1_IT should be 0 or 1... See config.h file)
#   endif


#   if (COMPARATOR1_IT_EVENT == 3)
       Comparator1_it_on_output_rising_edge();
#   elif (COMPARATOR1_IT_EVENT == 2)
       Comparator1_it_on_output_falling_edge();
#   elif (COMPARATOR1_IT_EVENT == 0)
       Comparator1_it_on_output_toggle();
#   else
#      error (COMPARATOR1_IT_EVENT should be 0, 2 or 3... See config.h file)
#   endif

#   if (COMPARATOR1_NEGATIVE_INPUT == 5)
       Set_comparator1_ni_dac();
#   elif (COMPARATOR1_NEGATIVE_INPUT == 4)
       Set_comparator1_ni_acmp();
#   elif (COMPARATOR1_NEGATIVE_INPUT == 3)
       Set_comparator1_ni_vref_1_60();
#   elif (COMPARATOR1_NEGATIVE_INPUT == 2)
       Set_comparator1_ni_vref_2_13();
#   elif (COMPARATOR1_NEGATIVE_INPUT == 1)
       Set_comparator1_ni_vref_3_20();
#   elif (COMPARATOR1_NEGATIVE_INPUT == 0)
       Set_comparator1_ni_vref_6_40();
#   else
#      error (COMPARATOR1_NEGATIVE_INPUT should be 0, 1, 2, 3, 4 or 5... See config.h file)
#   endif
}
#endif // USE_COMP1

//! Configures the comparator2 accordingly to the comparator2 Defines
//! Configuration values.
//! Take care that you have to select the comparators' inputs ports as
//! analog inputs thanks to the DIDR0 and DIDR1 registers.
//!
#ifdef USE_COMP2  //!< this define is set on config.h file

void init_comparator2(void)
{
    Enable_comparator2();
#   if (COMPARATOR2_IT == 1)
       Enable_comparator2_interrupt();
#   elif (COMPARATOR2_IT == 0)
       Disable_comparator2_interrupt();
#   else
#      error (COMPARATOR2_IT should be 0 or 1... See config.h file)
#   endif


#   if (COMPARATOR2_IT_EVENT == 3)
       Comparator2_it_on_output_rising_edge();
#   elif (COMPARATOR2_IT_EVENT == 2)
       Comparator2_it_on_output_falling_edge();
#   elif (COMPARATOR2_IT_EVENT == 0)
       Comparator2_it_on_output_toggle();
#   else
#      error (COMPARATOR2_IT_EVENT should be 0, 2 or 3... See config.h file)
#   endif

#   if (COMPARATOR2_NEGATIVE_INPUT == 5)
       Set_comparator2_ni_dac();
#   elif (COMPARATOR2_NEGATIVE_INPUT == 4)
       Set_comparator2_ni_acmp();
#   elif (COMPARATOR2_NEGATIVE_INPUT == 3)
       Set_comparator2_ni_vref_1_60();
#   elif (COMPARATOR2_NEGATIVE_INPUT == 2)
       Set_comparator2_ni_vref_2_13();
#   elif (COMPARATOR2_NEGATIVE_INPUT == 1)
       Set_comparator2_ni_vref_3_20();
#   elif (COMPARATOR2_NEGATIVE_INPUT == 0)
       Set_comparator2_ni_vref_6_40();
#   else
#      error (COMPARATOR2_NEGATIVE_INPUT should be 0, 1, 2, 3, 4 or 5... See config.h file)
#   endif
}
#endif // USE_COMP2
