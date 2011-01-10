// menu.h

// mt __flash typedef struct
typedef struct PROGMEM
{
    unsigned char state;
    unsigned char input;
    unsigned char nextstate;
} MENU_NEXTSTATE;


// mt __flash typedef struct
typedef struct PROGMEM
{
    unsigned char state;
    // char __flash *pText;
    PGM_P pText;
    char (*pFunc)(char input);
} MENU_STATE;


// Menu text
// mtA, these where all of the same structure as in the follow. line
// __flash char MT_AVRBF[]                         = "AVR Butterfly";
//const char MT_AVRBF[] PROGMEM                     = "AVR Butterfly GCC";
const char MT_AVRBF[] PROGMEM                     = "LDRGPS";
const char MT_GPS[] PROGMEM                       = "Position";
const char MT_TIME[] PROGMEM                      = "Time";
const char MT_TIME_CLOCK[] PROGMEM                = "Clock";
const char MT_TIME_CLOCK_ADJUST[] PROGMEM         = "Adjust Clock";
const char MT_TIME_DATE[] PROGMEM                 = "Date";
const char MT_TIME_DATE_ADJUST[] PROGMEM          = "Adjust Date";
const char MT_LIGHT[] PROGMEM                     = "Light";
const char MT_OPTIONS[] PROGMEM                   = "Options";
const char MT_OPTIONS_DISPLAY[] PROGMEM           = "Display";
const char MT_OPTIONS_DISPLAY_CONTRAST[] PROGMEM  = "Adjust contrast";
const char MT_OPTIONS_BOOT[] PROGMEM              = "Bootloader";
const char MT_OPTIONS_POWER_SAVE[] PROGMEM        = "Power Save Mode";
const char MT_OPTIONS_AUTO_POWER_SAVE[] PROGMEM   = "Auto Power Save";
const char MT_OPTIONS_KEYCLICK[] PROGMEM          = "Key Click";
// mtE

// mt MENU_NEXTSTATE menu_nextstate[] = { 
const MENU_NEXTSTATE menu_nextstate[] PROGMEM = {
//  STATE                       INPUT       NEXT STATE
    {ST_AVRBF,                  KEY_PLUS,   ST_OPTIONS_BOOT},
    {ST_AVRBF,                  KEY_NEXT,   ST_AVRBF_REV},
    {ST_AVRBF,                  KEY_MINUS,  ST_GPS},

    {ST_AVRBF_REV,              KEY_PREV,   ST_AVRBF},

    {ST_GPS,                	KEY_PLUS,   ST_AVRBF},
    {ST_GPS,                  	KEY_NEXT,   ST_GPS_POS},
    {ST_GPS,                	KEY_PREV,   ST_AVRBF},
    {ST_GPS,                  	KEY_MINUS,  ST_TIME},

    {ST_GPS_POS,                KEY_PREV,   ST_GPS},

    {ST_TIME,                   KEY_PLUS,   ST_GPS},
    {ST_TIME,                   KEY_NEXT,   ST_TIME_CLOCK},
    {ST_TIME,                   KEY_PREV,   ST_AVRBF},
    {ST_TIME,                   KEY_MINUS,  ST_LIGHT},

    {ST_TIME_CLOCK,             KEY_PLUS,   ST_TIME_DATE},
    {ST_TIME_CLOCK,             KEY_NEXT,   ST_TIME_CLOCK_FUNC},
    {ST_TIME_CLOCK,             KEY_PREV,   ST_TIME},
    {ST_TIME_CLOCK,             KEY_MINUS,  ST_TIME_DATE},

    {ST_TIME_CLOCK_ADJUST,      KEY_ENTER,  ST_TIME_CLOCK_ADJUST_FUNC},
    {ST_TIME_CLOCK_ADJUST,      KEY_PREV,   ST_TIME_CLOCK_FUNC},    

    {ST_TIME_DATE,              KEY_PLUS,   ST_TIME_CLOCK},
    {ST_TIME_DATE,              KEY_NEXT,   ST_TIME_DATE_FUNC},
    {ST_TIME_DATE,              KEY_PREV,   ST_TIME},
    {ST_TIME_DATE,              KEY_MINUS,  ST_TIME_CLOCK},

    {ST_TIME_DATE_ADJUST,       KEY_ENTER,  ST_TIME_DATE_ADJUST_FUNC},
    {ST_TIME_DATE_ADJUST,       KEY_PREV,   ST_TIME_DATE_FUNC},

    {ST_LIGHT,                  KEY_PLUS,   ST_TIME},
    {ST_LIGHT,                  KEY_NEXT,   ST_LIGHT_FUNC},
    {ST_LIGHT,                  KEY_PREV,   ST_AVRBF},
    {ST_LIGHT,                  KEY_MINUS,  ST_OPTIONS},

    {ST_OPTIONS,                KEY_PLUS,   ST_LIGHT},
    {ST_OPTIONS,                KEY_NEXT,   ST_OPTIONS_DISPLAY},
    {ST_OPTIONS,                KEY_PREV,   ST_AVRBF},
    {ST_OPTIONS,                KEY_MINUS,  ST_OPTIONS_BOOT},

    {ST_OPTIONS_DISPLAY,        KEY_PLUS,   ST_OPTIONS_AUTO_POWER_SAVE},
    {ST_OPTIONS_DISPLAY,        KEY_NEXT,   ST_OPTIONS_DISPLAY_CONTRAST},
    {ST_OPTIONS_DISPLAY,        KEY_PREV,   ST_OPTIONS},
    {ST_OPTIONS_DISPLAY,        KEY_MINUS,  ST_OPTIONS_POWER_SAVE},

    {ST_OPTIONS_DISPLAY_CONTRAST, KEY_ENTER,    ST_OPTIONS_DISPLAY_CONTRAST_FUNC},
    {ST_OPTIONS_DISPLAY_CONTRAST, KEY_PREV,     ST_OPTIONS_DISPLAY},

    {ST_OPTIONS_POWER_SAVE,       KEY_PLUS,     ST_OPTIONS_DISPLAY_CONTRAST},
    {ST_OPTIONS_POWER_SAVE,       KEY_NEXT,     ST_OPTIONS_POWER_SAVE_FUNC},
    {ST_OPTIONS_POWER_SAVE,       KEY_PREV,     ST_OPTIONS},
    {ST_OPTIONS_POWER_SAVE,       KEY_MINUS,    ST_OPTIONS_AUTO_POWER_SAVE},

    {ST_OPTIONS_AUTO_POWER_SAVE,  KEY_PLUS,     ST_OPTIONS_POWER_SAVE},
    {ST_OPTIONS_AUTO_POWER_SAVE,  KEY_NEXT,     ST_OPTIONS_AUTO_POWER_SAVE_FUNC},
    {ST_OPTIONS_AUTO_POWER_SAVE,  KEY_PREV,     ST_OPTIONS},
    {ST_OPTIONS_AUTO_POWER_SAVE,  KEY_MINUS,    ST_OPTIONS_DISPLAY},

    {ST_OPTIONS_BOOT,             KEY_PLUS,     ST_OPTIONS},
    {ST_OPTIONS_BOOT,             KEY_NEXT,     ST_OPTIONS_BOOT_FUNC},
    {ST_OPTIONS_BOOT,             KEY_PREV,     ST_AVRBF},
    {ST_OPTIONS_BOOT,             KEY_MINUS,    ST_AVRBF},
        
    {0,                         0,          0}
};


// mt MENU_STATE menu_state[] = {
const MENU_STATE menu_state[] PROGMEM = {
//  STATE                               STATE TEXT                  STATE_FUNC
    {ST_AVRBF,                          MT_AVRBF,                   NULL},
    {ST_AVRBF_REV,                      NULL,                       Revision},
    {ST_TIME,                           MT_TIME,                    NULL},
    {ST_TIME_CLOCK,                     MT_TIME_CLOCK,              NULL},
    {ST_TIME_CLOCK_FUNC,                NULL,                       ShowClock},
    {ST_TIME_CLOCK_ADJUST,              MT_TIME_CLOCK_ADJUST,       NULL},
    {ST_TIME_CLOCK_ADJUST_FUNC,         NULL,                       SetClock},
    {ST_TIME_DATE,                      MT_TIME_DATE,               NULL},
    {ST_TIME_DATE_FUNC,                 NULL,                       ShowDate},
    {ST_TIME_DATE_ADJUST,               MT_TIME_DATE_ADJUST,        NULL},
    {ST_TIME_DATE_ADJUST_FUNC,          NULL,                       SetDate},    
    {ST_LIGHT,                          MT_LIGHT,                   NULL},
    {ST_LIGHT_FUNC,                     NULL,                       LightFunc},
    {ST_OPTIONS,                        MT_OPTIONS,                 NULL},
    {ST_OPTIONS_DISPLAY,                MT_OPTIONS_DISPLAY,         NULL},
    {ST_OPTIONS_DISPLAY_CONTRAST,       MT_OPTIONS_DISPLAY_CONTRAST,NULL},
    {ST_OPTIONS_DISPLAY_CONTRAST_FUNC,  NULL,                       SetContrast},
    {ST_OPTIONS_BOOT,                   MT_OPTIONS_BOOT,            NULL},
    {ST_OPTIONS_BOOT_FUNC,              NULL,                       BootFunc},    
    {ST_OPTIONS_POWER_SAVE,             MT_OPTIONS_POWER_SAVE,      NULL},
    {ST_OPTIONS_POWER_SAVE_FUNC,        NULL,                       PowerSaveFunc},
    {ST_OPTIONS_AUTO_POWER_SAVE,        MT_OPTIONS_AUTO_POWER_SAVE, NULL},
    {ST_OPTIONS_AUTO_POWER_SAVE_FUNC,   NULL,                       AutoPower},
    {ST_GPS,                     	    MT_GPS,                     NULL},
    {ST_GPS_POS,                        NULL,                       ShowGPSPos},

    {0,                                 NULL,                       NULL},

};
