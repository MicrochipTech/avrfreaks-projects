#ifndef SCREENS_H
#define SCREENS_H 1

#include "translations.h"

#define SCREEN_MENU_LINES 4

#define SCREEN_MENU 10
#define SCREEN_HOME 0
#define SCREEN_TEMPERATURE 1
#define SCREEN_HUMIDITY 2
#define SCREEN_TIME_AND_DATE (SCREEN_MENU_LINES-1)


static char PROGMEM screenMenu_0[] = TRANSLATION_HOME;
static char PROGMEM screenMenu_1[] = TRANSLATION_TEMPERATURE;
static char PROGMEM screenMenu_2[] = TRANSLATION_HUMIDITY;
static char PROGMEM screenMenu_3[] = TRANSLATION_TIME_AND_DATE;

static PGM_P screenMenu[SCREEN_MENU_LINES] PROGMEM = {
    screenMenu_0,
    screenMenu_1,
    screenMenu_2,
	screenMenu_3
};

extern uint8_t screen;

void screenTimeRefresh(void);
void screenSensorsRefresh(void);
void screenGraphRefresh(void);

void screenHomeDraw(void);
void screenMenuHandleButtons(void);
void screenMenuDraw(void);
void screenDateAndTimeDraw(void);
void screenDateAndTimeHandleButtons(void);
void screenGraphHandleButtons(void);
void screenGraphDraw(uint8_t);

#endif
