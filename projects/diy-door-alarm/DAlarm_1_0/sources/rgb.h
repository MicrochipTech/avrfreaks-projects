/*
 * rgb.h
 *
 * Created:
 * Author: PjV
 */ 

#ifndef TIMERS_H_
#define TIMERS_H_

void RGB_InitTimer(void);
void RGB_StartCounting(void);
void RGB_StopCounting(void);

void RGB_RedUpDown(void);
void RGB_GreenUpDown(void);
void RGB_BlueUpDown(void);

void RGB_Off(void);

#endif /* TIMERS_H_ */