//Revisions number
#define SWHIGH  0
#define SWLOW   7
// mt
#define SWLOWLOW 2

// main.h

void Initialization(void);
unsigned char StateMachine(char state, unsigned char stimuli);
char BootFunc(char input);
char PowerSaveFunc(char input);
char AutoPower(char input);
void Delay(unsigned int millisec);
void OSCCAL_calibration(void);

extern volatile uint8_t gNewSession;
extern volatile uint8_t gDumpAll;

#define BOOL    char

#define FALSE   0
#define TRUE    (!FALSE)
#define NULL    0

#define AUTO    3

// Macro definitions
//mtA - 
// sbi and cbi are not longer supported by the avr-libc
// to avoid version-conflicts the macro-names have been 
// changed to sbiBF/cbiBF "everywhere"
#define sbiBF(port,bit)  (port |= (1<<bit))   //set bit in port
#define cbiBF(port,bit)  (port &= ~(1<<bit))  //clear bit in port
//mtE

// Menu state machine states
#define ST_AVRBF                        10
#define ST_TIME                         20
#define ST_TIME_CLOCK                   21
#define ST_TIME_CLOCK_FUNC              22
#define ST_TIME_CLOCK_ADJUST            23
#define ST_TIME_CLOCK_ADJUST_FUNC       24
#define ST_TIME_CLOCKFORMAT_ADJUST      25
#define ST_TIME_CLOCKFORMAT_ADJUST_FUNC 36
#define ST_TIME_DATE                    27
#define ST_TIME_DATE_FUNC               28
#define ST_TIME_DATE_ADJUST             29
#define ST_TIME_DATE_ADJUST_FUNC        30
#define ST_TIME_DATEFORMAT_ADJUST       31
#define ST_TIME_DATEFORMAT_ADJUST_FUNC  32
#define ST_RECORD                       40
#define ST_RECORD_START                 41
#define ST_RECORD_START_FUNC            42
#define ST_RECORD_MDUMP                 43
#define ST_RECORD_MDUMP_FUNC            44
#define ST_RECORD_SETINTV               45
#define ST_RECORD_SETINTV_FUNC          46
#define ST_TEMPERATURE                  60
#define ST_TEMPERATURE_FUNC             61
#define ST_VOLTAGE                      70
#define ST_VOLTAGE_FUNC                 71
#define ST_LIGHT                        80
#define ST_LIGHT_FUNC                   81
#define ST_OPTIONS                      90
#define ST_OPTIONS_DISPLAY              91
#define ST_OPTIONS_DISPLAY_CONTRAST     92
#define ST_OPTIONS_DISPLAY_CONTRAST_FUNC 93
#define ST_OPTIONS_BOOT                 94
#define ST_OPTIONS_BOOT_FUNC            95
#define ST_OPTIONS_POWER_SAVE           96
#define ST_OPTIONS_POWER_SAVE_FUNC      97
#define ST_OPTIONS_AUTO_POWER_SAVE      98
#define ST_OPTIONS_AUTO_POWER_SAVE_FUNC 99


