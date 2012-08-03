
#ifndef DELAY_H
#define DELAY_H

extern void Timer1Init (void);
extern void DelayUs (uint8_t us);
extern void DelayMs (uint16_t ms);
extern void InitIo (void);
extern void ExternIntInit (void);

#endif
