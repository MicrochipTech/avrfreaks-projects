#ifndef CONFIG_H
#define CONFIG_H

#define DEBUG_MMC_INIT 1
#define USE_MMC 1
// FIXME - make a multiple of sizeof(logentry_t)
#define MMC_BUF_LEN 256
#define USE_CALIBRATE 1
#define INCLUDE_PCF_SETTER USE_CALIBRATE
#define USE_OSCOPE 1
#define USE_GLCD 1
#define USE_UART 0
#define USE_RTC 1

#define GLCD_OVERPAINT 1

// this is to be set manually to fine-tune the clock
#define ADC_CLOCK_ERROR (-22)

#endif
