//
// util.h
//
extern void InitTC1(void);
extern void EnableTC1(void);
extern void AppTimer1Stuff(void);

//
// These functions work only if TC1 is active
//
extern void delayHalfSecond(void);
extern void delay100msecs(void);
extern void delay300msecs(void);
extern void delayOneSecond(void);
extern void FlashLed(uint8 duration, uint8 cnt);
extern void BlinkFastNoInts(void);
extern void BlinkFast(void);
extern void ShortBlinkFast(void);
extern void BlinkSlow(void);


extern volatile uint8 Timer1Counter2;

extern void assertFailed(const char* str, int line);
#define ASSERT(_x) if (!(_x)) assertFailed(NULL,__LINE__);
