#include "enc.h"

void int0_init(void)
{
	MCUCR |= _BV(ISC00);
	MCUCR |= _BV(ISC01);
	GIMSK |= _BV(INT0);
}

void int1_init(void)
{
	MCUCR |= _BV(ISC10);
	MCUCR |= _BV(ISC11);
	GIMSK |= _BV(INT1);
}



uint8_t impulsator(uint8_t liczba)
{ 
   if((bit_is_clear(ENC_PORT,KANAL_A_PIN)))
   {
      _delay_ms(4);
      unsigned char zajetosc = 0;
      while((bit_is_clear(ENC_PORT,KANAL_A_PIN))){} // czekam na narastające zbocze na kanale A
      _delay_ms(4);
      if((bit_is_set(ENC_PORT,KANAL_B_PIN)))
      {
         if(liczba < 255 && ! zajetosc)
         {
            liczba++;
            _delay_ms(4);
            zajetosc = 1;
         }
      }
      if((bit_is_clear(ENC_PORT,KANAL_B_PIN)))
      {
         if(liczba >= 255 && ! zajetosc)
         {
            liczba--;
            _delay_ms(4);
            zajetosc = 1;
         }
      }
   }
   _delay_ms(20);
return(liczba);
}

uint8_t check_key(uint8_t laststate)
{
	uint8_t tmp = ENC_PORT;

	if(bit_is_set(tmp,6))
	{
		if(laststate)
		return(1);
		else
		return(0);
	}
	if(bit_is_clear(tmp,6))
	{
		if(laststate)
		return(1);
		else
		return(0);
	}
	return(laststate);
}


uint8_t enc_dir(uint8_t tact)
{
_delay_ms(25);
uint8_t tmp = ENC_PORT;
if(bit_is_set(tmp,5))
{
   if(bit_is_set(tmp,6))
   tact = 0;
   if(bit_is_clear(tmp,6))
   tact = 1;
}
if(bit_is_clear(tmp,5))
{
   if(bit_is_set(tmp,6))
   tact = 1;
   if(bit_is_clear(tmp,6))
   tact = 0;
}
return(tact);
} 
