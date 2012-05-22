
#include "platform.h"
#include "dp_inc.h"

/*---------------------------------------------------------------------------*/
/* function: SetResetVPC3Channel1                                            */
/*---------------------------------------------------------------------------*/
/*!
  \brief Set reset pin of VPC3+ to "high signal".
*/
void SetResetVPC3Channel1( void )
{
   /*
      Add your own code.
   */
}//void SetResetVPC3Channel1( void )

/*---------------------------------------------------------------------------*/
/* function: ClrResetVPC3Channel1                                            */
/*---------------------------------------------------------------------------*/
/*!
  \brief Set reset pin of VPC3+ to "low signal".
*/
void ClrResetVPC3Channel1( void )
{
   /*
      Add your own code.
   */
}//void ClrResetVPC3Channel1( void )

/*---------------------------------------------------------------------------*/
/* function: EnableInterruptVPC3Channel1                                     */
/*---------------------------------------------------------------------------*/
/*!
  \brief Enable VPC3+ microcontroller interrupt.
*/
void EnableInterruptVPC3Channel1( void )
{
   /*
      Add your own code.
   */
}//void EnableInterruptVPC3Channel1( void )

/*---------------------------------------------------------------------------*/
/* function: EnableInterruptVPC3Channel1                                     */
/*---------------------------------------------------------------------------*/
/*!
  \brief Disable VPC3+ microcontroller interrupt.
*/
void DisableInterruptVPC3Channel1( void )
{
   /*
      Add your own code.
   */
}//void DisableInterruptVPC3Channel1( void )


int main( void )
{
   /*
      Add your own code.
   */

   SetResetVPC3Channel1();          // set reset hardware pin

   //initialize VPC3+C/S
   ProfibusInit();

   while(1)
   {
      /*
         Add your own code.
      */

      // call PROFIBUS
      ProfibusMain();

   }//while(1)

   return 0;
}//void main( void )


/*
interrupt fuction
{
    VPC3_Isr();
}


*/



