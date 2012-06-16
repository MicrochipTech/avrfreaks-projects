/*******************************************************************
 * isr.h
 * written by      Ralf Meeh 26.04.2009
 * last changed by Ralf Meeh 26.04.2009
 * Function:       Service Routinen, alle Interrupts,Projektabhängig
 *******************************************************************/
ISR(TIMER0_OVF_vect);
void handle_10ms(void);
void handle_500ms(void);
void read_rotary(void);



