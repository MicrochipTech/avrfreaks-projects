/* gps lap timer */

#ifndef __NES_LAP_TIMER_H__
#define __NES_LAP_TIMER_H__


#define MAX_LAPS 100

extern void ServiceUSB (void);
extern void StopUSB (void);
extern void InitUSB (void);

struct _LapData
{
	float Time;
};

extern struct _LapData LapTimes[MAX_LAPS];
extern uint8_t NoLaps;

extern float TopSpeed, TopSpeedTime;
extern float CumulativeDistance;
extern float CumulativeStoppedTime;
extern float CumulativeRunTime;

extern uint8_t USBConnected;

extern void LCDClear (void);
extern uint8_t LCDAddress (uint8_t X, uint8_t Y);
extern void LCDString_P (const prog_char *String);
extern void LCDCommand (uint8_t Command);

extern char PrintBuffer[];

#endif