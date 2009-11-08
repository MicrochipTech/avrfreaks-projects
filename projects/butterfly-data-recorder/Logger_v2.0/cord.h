#ifndef BF_RECORD_H_
#define BF_RECORD_H_

// Determines whether to use the DataFlash memory or the EEprom memory to store
// the logged data. Comment out to use the EEPROM
#define USE_DFLASH


extern volatile uint8_t gRecording;
extern volatile int16_t gRecordTimer;
extern volatile int16_t gRecordInterval;

char RecordFunc(char);
void recordData();
void recordDataHeader();
void memDump();
void memFlush();
char memDumpFunc(char);
char setIntervalFunc(char);
void ShowInterval(void);

#endif

