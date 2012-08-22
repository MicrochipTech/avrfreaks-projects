//
// file: initializations.h
//

#ifndef _INITIALIZATIONS_H_
#define _INITIALIZATIONS_H_

#include "freertos_usart_driver.h"

void vInitClock(void);
void initUsarts(void);
void vPortPreparation(void);
void setupTimer1Interrupt( void );



// USART data structs
extern USART_data_t USARTC0_data;
extern USART_data_t USARTC1_data;
extern USART_data_t USARTD0_data;
extern USART_data_t USARTD1_data;
extern USART_data_t USARTE0_data;


#endif // initializations.h
