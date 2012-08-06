/*
 * os.h
 *
 *  Created on: May 8, 2011
 *      Author: dani
 */

#ifndef OS_H_
#define OS_H_

#define OS_100us_RUN_TIMER  (100)
#define OS_1ms_RUN_TIMER    (10)
#define OS_100ms_RUN_TIMER  (1000)
#define OS_200ms_RUN_TIMER  (2000)
#define OS_500ms_RUN_TIMER  (5000)

//Alex
typedef struct
{
   bool bUpPushButtonPressed;
   bool bLowPushButtonPressed;
} tPushButtonStatus;
//end Alex

extern volatile uint8_t  OS_u8Cnt1ms;
extern volatile uint16_t OS_u16Cnt100ms;
extern volatile uint16_t OS_u16Cnt200ms;
extern volatile uint16_t OS_u16Cnt500ms;

extern void OS_vInit(void);
extern void OS_vCyclicTask100us(void);
extern void OS_vCyclicTask1ms(void);
extern void OS_vCyclicTask100ms(void);
extern void OS_vCyclicTask200ms(void);
extern void OS_vCyclicTask500ms(void);
extern void OS_vBackgroundTask(void);
extern void OS_vStartFreeRunTimer(void);

#endif /* OS_H_ */
